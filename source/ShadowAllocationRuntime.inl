// OnyxOak modification project: Extra Night Shadows Fix and Better Headlights.
// Project direction, integration and visual testing by OnyxOak; Codex-assisted development.
// Modification notice: 2026-09-27. See ATTRIBUTION.md for upstream credits and GPL-3.0.
// Official release: https://www.nexusmods.com/gta4/mods/1459

// Included after module imports and CShadows declarations. This experimental
// adapter is opt-in and has not been run in GTA IV. No install occurs from the
// offline build; the game integration activates only on a later chosen launch.
namespace PlayerShadowAllocation
{
    namespace allocation = fusionfix::shadows::ce::allocation;
    namespace budget = fusionfix::shadows::budget;
    using fusionfix::shadows::Vec3;
    using fusionfix::shadows::FloatingPointState;

    static SafetyHookInline selectionHook;
    static SafetyHookMid collectHook, finalizeHook, lampDistanceHook;
    static bool nativeLampPriority=false;
    static std::atomic<uint32_t> lampDistanceAdjusted{0};
    static std::atomic<bool> ready{false}, unsupportedThread{false};
    static std::atomic<DWORD> ownerThread{0};
    static uintptr_t gameBase = 0;
    static bool cameraPriority = false;
    static std::atomic<uint32_t> cameraPasses{0}, cameraFallbacks{0};
    static bool publicationEnabled = false; // Immutable after ready is published.
    static std::string installStatus = "not_requested"; // Init-only; diagnostics reads after ready publication.
    // Diagnostics are counters only: no per-frame logging/allocations or I/O.
    static std::atomic<uint32_t> appliedPasses{0}, observedPasses{0}, fallbackPasses{0}, staleKeysCleared{0};

    struct State
    {
        budget::ShadowAllocationPass pass;
        Vec3 player{}, drivingFocus{};
        fusionfix::shadows::ShadowDrivingFocus motionFocus;
        std::array<budget::PlayerShadowBudget::Slot,7> previousSelection{};
        fusionfix::shadows::ShadowView view{};
        uintptr_t ped = 0, occupiedCar = 0, lastCar = 0;
        uint32_t frame = 0, viewFrame = 0;
        uintptr_t stackAnchor = 0;
        unsigned depth = 0;
        bool committed = false;
    };
    static thread_local State state;

    struct Invocation
    {
        Invocation() noexcept { ++state.depth; if (state.depth == 1) state.committed = false; }
        ~Invocation() noexcept { state.pass.EndInvocation(); --state.depth; }
        Invocation(const Invocation&) = delete;
        Invocation& operator=(const Invocation&) = delete;
    };
    static std::atomic<uint32_t> lampRemoved{0}, lampMissingInput{0}, lampDroppedPresent{0};
    static std::array<std::atomic<uint32_t>,8> rejectedPassReasons{};
    static std::atomic<uint32_t> rejectedAdapterChecks{0};
    static void RejectPass() noexcept
    {
        if (state.pass.Active()) { ++fallbackPasses; ++rejectedAdapterChecks; }
        state.pass.Cancel();
    }

    static rage::CLightSource* CurrentLights() noexcept
    {
        return *reinterpret_cast<rage::CLightSource**>(gameBase + allocation::LightArrayPointerRva);
    }
    static uint32_t CurrentCount() noexcept
    {
        return *reinterpret_cast<uint32_t*>(gameBase + allocation::LightCountRva);
    }
    static bool Enabled() noexcept
    {
        return ready.load(std::memory_order_acquire) && !unsupportedThread.load(std::memory_order_relaxed) &&
            bExtraNightShadows && bHeadlightShadows && bVehicleNightShadows;
    }
    static std::atomic<uint32_t> captureCalls{0}, captureValid{0}, perspectiveCalls{0};
    static std::atomic<int> captureWidth{0}, captureHeight{0}, activeWidth{0}, activeHeight{0};
    static SafetyHookMid cameraCaptureHook;
    static std::atomic_flag gameplayViewLock = ATOMIC_FLAG_INIT;
    static fusionfix::shadows::ShadowView gameplayView{};
    static uint32_t gameplayViewFrame = 0;
    static fusionfix::shadows::ShadowView ReadView(const rage::grcViewport* viewport) noexcept
    {
        fusionfix::shadows::ShadowView view{};
        if (!viewport || !rage::grcDevice::ms_nActiveWidth || !rage::grcDevice::ms_nActiveHeight ||
            !fusionfix::shadows::IsGameplayViewport(viewport->mIsPerspective,
                viewport->mWidth,viewport->mHeight,*rage::grcDevice::ms_nActiveWidth,
                *rage::grcDevice::ms_nActiveHeight)) return view;
        std::memcpy(view.view, viewport->mViewMatrix, sizeof(view.view));
        std::memcpy(view.projection, viewport->mProjectionMatrix, sizeof(view.projection));
        view.valid = true;
        for (const auto& row : view.view) for (float f : row) if (!std::isfinite(f)) view.valid = false;
        for (const auto& row : view.projection) for (float f : row) if (!std::isfinite(f)) view.valid = false;
        if (std::abs(view.projection[0][0]) < 0.001f || std::abs(view.projection[1][1]) < 0.001f)
            view.valid = false;
        return view;
    }
    static bool InstallCameraCapture() noexcept
    {
        const auto base = reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
        // CE setter verified in the running CE executable: mov [viewport],ecx.
        // Guard both opcode and relocated destination before installing.
        const auto site = reinterpret_cast<const uint8_t*>(base + 0x30C6F);
        constexpr uint8_t prefix[]{0x89,0x0D};
        if (!rage::pCurrentViewport || std::memcmp(site,prefix,sizeof(prefix)) ||
            *reinterpret_cast<const uint32_t*>(site+2) != reinterpret_cast<uintptr_t>(rage::pCurrentViewport) ||
            site[6]!=0xC6 || site[7]!=0x05) return false;
        cameraCaptureHook = safetyhook::create_mid(base + 0x30C6F, [](SafetyHookContext& regs) {
            const FloatingPointState fp;
            if (!CShadows::pFrameCounter) return;
            ++captureCalls;
            const auto* viewport = reinterpret_cast<const rage::grcViewport*>(regs.ecx);
            if (viewport && viewport->mIsPerspective) {
                ++perspectiveCalls; captureWidth = viewport->mWidth; captureHeight = viewport->mHeight;
                activeWidth = rage::grcDevice::ms_nActiveWidth ? *rage::grcDevice::ms_nActiveWidth : 0; activeHeight = rage::grcDevice::ms_nActiveHeight ? *rage::grcDevice::ms_nActiveHeight : 0;
            }
            const auto view = ReadView(viewport);
            if (view.valid) ++captureValid;
            if (view.valid && !gameplayViewLock.test_and_set(std::memory_order_acquire)) {
                gameplayView = view; gameplayViewFrame = *CShadows::pFrameCounter;
                gameplayViewLock.clear(std::memory_order_release);
            }
        });
        return static_cast<bool>(cameraCaptureHook);
    }
    static bool Prepare() noexcept
    {
        if (!Enabled() || !CShadows::pFrameCounter || !CTimer::m_snTimeInMilliseconds ||
            !CPlayer::getLocalPlayerPed || !CPlayer::findPlayerCar)
            return false;
        const auto ped = CPlayer::getLocalPlayerPed();
        if (!ped) return false;
        const auto matrix = *reinterpret_cast<const float* const*>(ped + 0x20);
        if (!matrix) return false;
        const Vec3 position{matrix[12], matrix[13], matrix[14]};
        if (!std::isfinite(position.x) || !std::isfinite(position.y) || !std::isfinite(position.z)) return false;
        if (ped != state.ped) { state.lastCar = 0; state.view = {}; }
        state.ped = ped;
        state.player = position;
        state.occupiedCar = CPlayer::findPlayerCar();
        if (state.occupiedCar) {
            state.lastCar = state.occupiedCar;
            const auto carMatrix = *reinterpret_cast<const float* const*>(state.occupiedCar + 0x20);
            if (carMatrix && std::isfinite(carMatrix[12]) && std::isfinite(carMatrix[13]) && std::isfinite(carMatrix[14]))
                state.player = {carMatrix[12], carMatrix[13], carMatrix[14]};
        }
        state.drivingFocus=state.motionFocus.Update(state.player,state.occupiedCar,static_cast<uint32_t>(*CTimer::m_snTimeInMilliseconds));
        state.frame = *CShadows::pFrameCounter;
        state.stackAnchor = 0;
        state.view = {};
        if (cameraPriority && rage::pCurrentViewport) {
            state.view = ReadView(rage::GetCurrentViewport());
            // Shadow passes replace the current viewport. Keep only a recent,
            // full-resolution perspective view. Selection can consume a view on a different
            // thread; publish a consistent snapshot without waiting in either hook.
            if (!state.view.valid && !gameplayViewLock.test_and_set(std::memory_order_acquire)) {
                if (gameplayView.valid && state.frame - gameplayViewFrame <= 1) state.view = gameplayView;
                gameplayViewLock.clear(std::memory_order_release);
            }
            if (state.view.valid) ++cameraPasses; else ++cameraFallbacks;
        }
        if (reinterpret_cast<uintptr_t>(CurrentLights()) < 0x10000) return false;
        state.pass.Begin({state.frame, static_cast<uint32_t>(*CTimer::m_snTimeInMilliseconds),
                          ped, state.occupiedCar != 0}, CurrentLights(), CurrentCount());
        return state.pass.Active();
    }

    static bool InfluencesPlayer(const rage::CLightSource& light) noexcept
    {
        const Vec3 position{light.mPosition.x,light.mPosition.y,light.mPosition.z};
        const Vec3 direction{light.mDirection.x,light.mDirection.y,light.mDirection.z};
        if (light.mFlags & 0x100u) {
            if (fusionfix::shadows::BeamTouchesReceiver(state.player, state.occupiedCar ? 3.0f : 1.5f,
                position, direction, light.mOuterConeAngle, light.mRadius)) return true;
            return !state.occupiedCar && NearbyVehicleLighting36::Relevant(state.ped,state.frame,
                static_cast<uint32_t>(*CTimer::m_snTimeInMilliseconds),state.player,position,direction,
                light.mOuterConeAngle,light.mRadius,static_cast<uint32_t>(light.mCastShadows));
        }
        // Nearby lamps retain their protected share; scenery uses the reach slider.
        if (fusionfix::shadows::EvaluateGeometry(state.player,position).distanceSquared > 15.24f*15.24f) return false;
        return fusionfix::shadows::LightVolumeContains(state.player,position,direction,
            light.mType,light.mRadius,light.mOuterConeAngle);
    }

    static uint64_t LampGeometry(const rage::CLightSource& light) noexcept
    {
        // A stationary lamp key reused at a new position must not inherit its
        // old slot's hold interval. Vehicle keys lack a verified generation
        // field; the adapter does not claim to detect every pool-pointer reuse.
        uint32_t words[6];
        std::memcpy(words, &light.mPosition, 12);
        words[3] = static_cast<uint32_t>(light.mType);
        words[4] = static_cast<uint32_t>(light.mInteriorIndex);
        words[5] = static_cast<uint32_t>(light.mRoomIndex);
        uint64_t hash = 14695981039346656037ull;
        for (auto word : words) { hash ^= word; hash *= 1099511628211ull; }
        return hash;
    }

    // Audited boundary BEFORE distance is consumed by either cache refresh or
    // native dynamic sorting. Never substitute final lamp indices after cache work.
    static void AdjustLampDistance(SafetyHookContext& regs) noexcept {
        const FloatingPointState fp;
        if(!nativeLampPriority || !publicationEnabled || !Enabled() || state.depth!=1 ||
            !state.pass.Active() || !state.view.valid) return;
        const auto* lights=CurrentLights(); const auto count=CurrentCount();
        const auto address=reinterpret_cast<uintptr_t>(lights);
        const auto index=*reinterpret_cast<const uint32_t*>(regs.esp+0x14);
        if(address<0x10000 || count>4096 || index>=count || regs.edi!=address ||
            regs.esi!=index*sizeof(rage::CLightSource) || address>UINTPTR_MAX-count*sizeof(rage::CLightSource)) return;
        const auto& light=lights[index];
        if(!(light.mFlags&6u) || (light.mFlags&0x100u) ||
            *reinterpret_cast<const uint32_t*>(regs.esp+0x1C)!=light.mFlags) return;
        const Vec3 position{light.mPosition.x,light.mPosition.y,light.mPosition.z};
        const float visibleWeight=fusionfix::shadows::ShadowViewWeight(state.view,position,
            {light.mDirection.x,light.mDirection.y,light.mDirection.z},light.mRadius,false);
        const float viewWeight=1.0f+(visibleWeight-1.0f)*(state.occupiedCar?1.0f:0.5f);
        const int feet=fusionfix::shadows::ShadowReachFeet(FusionFixSettings.Get("PREF_LAMP_REACH"));
        const float reach=feet>0?feet*0.3048f:60.0f;
        const float original=regs.xmm0.f32[0];
        const float adjusted=fusionfix::shadows::NativeLampPriorityDistance(original,
            fusionfix::shadows::EvaluateGeometry(state.player,position).distanceSquared,viewWeight,reach);
        if(adjusted!=original && std::isfinite(adjusted) && adjusted>=0) {
            regs.xmm0.f32[0]=adjusted; ++lampDistanceAdjusted;
        }
    }

    static void Collect(SafetyHookContext& regs) noexcept
    {
        const FloatingPointState fp;
        if (!Enabled() || state.depth != 1 || !state.pass.Active()) return;
        if (!state.stackAnchor) state.stackAnchor = regs.esp;
        if (state.stackAnchor != regs.esp) { RejectPass(); return; }
        const auto index = static_cast<uint32_t>(regs.edx);
        auto* lights = CurrentLights();
        const auto count = CurrentCount();
        const auto address = reinterpret_cast<uintptr_t>(lights);
        if (address < 0x10000 || count > 4096 ||
            address > UINTPTR_MAX - static_cast<uintptr_t>(count) * sizeof(rage::CLightSource) ||
            index >= count || regs.edi != address ||
            regs.esi != index * sizeof(rage::CLightSource) ||
            *reinterpret_cast<const uint32_t*>(regs.esp + 0x14) != index ||
            *reinterpret_cast<const uint32_t*>(regs.esp + 0x18) != regs.esi)
        {
            RejectPass();
            return;
        }
        const auto& light = lights[index];
        const auto flags = *reinterpret_cast<const uint32_t*>(regs.esp + 0x1C);
        if (flags != light.mFlags || !(flags & 4u) || ((flags & 2u) && light.mShadowCacheIndex < 0))
        {
            RejectPass();
            return;
        }
        const auto key = static_cast<uint32_t>(light.mCastShadows); // Audited opaque +60 key.
        auto geometry = fusionfix::shadows::EvaluateGeometry(state.player,
            {light.mPosition.x, light.mPosition.y, light.mPosition.z});
        auto kind = budget::Kind::Lamp;
        if (flags & 0x100u)
            kind = fusionfix::shadows::ce::IsVehicleBeam(key, state.occupiedCar ? state.occupiedCar : state.lastCar)
                ? budget::Kind::PlayerBeam : budget::Kind::OtherBeam;
        if (flags & 0x100u)
            geometry.distanceSquared = fusionfix::shadows::ReceiverDistanceSquared(state.player,
                {light.mPosition.x,light.mPosition.y,light.mPosition.z},state.occupiedCar ? 3.0f : 1.5f);
        const int feet = fusionfix::shadows::ShadowReachFeet(FusionFixSettings.Get(
            kind == budget::Kind::Lamp ? "PREF_LAMP_REACH" : "PREF_HEADLIGHT_REACH"));
        const float configuredReach = feet > 0 ? feet * 0.3048f : (kind == budget::Kind::Lamp ? 60.0f : 35.0f);
        const float reach = kind == budget::Kind::OtherBeam && std::isfinite(light.mRadius) && light.mRadius > 0
            ? (std::max)(configuredReach,light.mRadius) : configuredReach;
        const auto viewWeight=fusionfix::shadows::ShadowViewWeight(state.view,
            {light.mPosition.x,light.mPosition.y,light.mPosition.z},
            {light.mDirection.x,light.mDirection.y,light.mDirection.z},light.mRadius,(flags&0x100u)!=0);
        const float priorityDistance=state.occupiedCar && kind==budget::Kind::Lamp && viewWeight>1.0f
            ? fusionfix::shadows::DrivingLampPriorityDistance(state.player,state.drivingFocus,
                {light.mPosition.x,light.mPosition.y,light.mPosition.z},geometry.distanceSquared,viewWeight)
            : -1.0f;
        state.pass.Observe({key, index, kind, geometry.distanceSquared,
                           InfluencesPlayer(light), true,
                           kind == budget::Kind::Lamp ? LampGeometry(light) : 0,
                           viewWeight, reach * reach, priorityDistance}, &light);
        if (!state.pass.Active()) { ++fallbackPasses; ++rejectedPassReasons[state.pass.FailureCode() & 7]; }
    }

    static void Finalize(SafetyHookContext& regs) noexcept
    {
        const FloatingPointState fp;
        if (!Enabled() || state.depth != 1 || !state.pass.Active()) return;
        if (!state.stackAnchor || state.stackAnchor != regs.esp ||
            !CShadows::pFrameCounter || *CShadows::pFrameCounter != state.frame ||
            CPlayer::getLocalPlayerPed() != state.ped || CPlayer::findPlayerCar() != state.occupiedCar)
        {
            RejectPass();
            return;
        }
        auto* indices = reinterpret_cast<int32_t*>(regs.esp + allocation::SelectedIndicesStackOffset);
        const auto traceTick=GetTickCount();
        auto traceChoice=[&](unsigned stage,unsigned slot,int index) noexcept {
            if(index<0 || static_cast<unsigned>(index)>=CurrentCount())return;
            const auto& light=CurrentLights()[index];
            const auto key=static_cast<uint32_t>(light.mCastShadows);
            ShadowTrace34::Track(key);
            ShadowTrace34::Emit({stage,state.frame,traceTick,key,static_cast<int>(slot),index,
                static_cast<int>(light.mFlags),static_cast<int>(light.mShadowCacheIndex),
                light.mPosition.x,light.mPosition.y,light.mPosition.z,light.mRadius});
        };
        for(unsigned slot=0;slot<7;++slot) traceChoice(1,slot,indices[slot]);
        // Candidate35: preserve native lamp slots and rank only the remaining beam space.
        // Observe mode tests the whole transaction against a private copy.
        // Switching to publication requires a new explicitly selected launch.
        std::array<int32_t, 7> observation{};
        if (!publicationEnabled) std::memcpy(observation.data(), indices, sizeof(observation));
        if (state.pass.Commit(CurrentLights(), CurrentCount(), publicationEnabled ? indices : observation.data(), true))
        {
            const auto& traced=state.pass.LastSelection();
            for(unsigned slot=0;slot<7;++slot)
                if(traced.validMask&(1u<<slot)) traceChoice(2,slot,static_cast<int>(traced.slots[slot].index));
            if (publicationEnabled) {
                ++appliedPasses; state.committed = true;
                const auto& selection=state.pass.LastSelection();
                for(const auto& old:state.previousSelection) {
                    if(!old.key || old.kind!=budget::Kind::Lamp) continue;
                    bool retained=false;
                    for(const auto& now:selection.slots)
                        if(now.key==old.key && now.generation==old.generation) retained=true;
                    if(!retained) {
                        ++lampRemoved;
                        ShadowTrace34::Emit({3,state.frame,traceTick,static_cast<uint32_t>(old.key),
                            state.pass.WasObserved(old.key,old.generation)?1:0,0,0,0,0,0,0,0});
                        if(state.pass.WasObserved(old.key,old.generation)) ++lampDroppedPresent;
                        else ++lampMissingInput;
                    }
                }
                state.previousSelection=selection.slots;
            }
            else ++observedPasses;
        }
        else { ++fallbackPasses; ++rejectedPassReasons[state.pass.FailureCode() & 7]; }
    }

    static void __cdecl Select()
    {
        const Invocation invocation;
        {
            const FloatingPointState fp;
            DWORD expected = 0;
            const DWORD current = GetCurrentThreadId();
            ownerThread.compare_exchange_strong(expected, current, std::memory_order_relaxed);
            if (ownerThread.load(std::memory_order_relaxed) != current)
                unsupportedThread.store(true, std::memory_order_relaxed);
            if (state.depth != 1 || !Prepare()) RejectPass();
        }
        // Hooks are immutable once published. The unsafe call avoids the
        // hook-object mutex around native execution (including recursion).
        selectionHook.unsafe_ccall<void>();
        // Native copying marks holes inactive but leaves their previous keys.
        // Clear only those dead identities before native frame-buffer copying.
        // This prevents lighting lookup from finding a stale, unrendered map.
        if (state.depth == 1 && state.committed && publicationEnabled) {
            auto* records = reinterpret_cast<void*>(gameBase + fusionfix::shadows::ce::casterguard::SlotKeyRva - 0xF8);
            staleKeysCleared.fetch_add(fusionfix::shadows::ClearInactiveShadowKeys(records,0x880),std::memory_order_relaxed);
        }
    }

    static bool Install(bool publish)
    {
        gameBase = reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
        publicationEnabled = publish;
        if (!allocation::ValidateMappedImage(reinterpret_cast<const uint8_t*>(gameBase),
                fusionfix::shadows::ce::ImageSize, gameBase))
        {
            installStatus = "guard_failed " + fusionfix::shadows::ce::diagnostics::Describe(
                reinterpret_cast<const uint8_t*>(gameBase), fusionfix::shadows::ce::ImageSize, gameBase);
            return false;
        }
        // Prepare all trampolines before any write. Mid hooks are inert until
        // ready is published AND execution is inside our selection wrapper.
        auto begin = safetyhook::InlineHook::create(gameBase + allocation::SelectionRva,
            Select, safetyhook::InlineHook::StartDisabled);
        auto collect = safetyhook::MidHook::create(gameBase + allocation::CollectRva,
            Collect, safetyhook::MidHook::StartDisabled);
        auto finish = safetyhook::MidHook::create(gameBase + allocation::FinalizeRva,
            Finalize, safetyhook::MidHook::StartDisabled);
        auto lampDistance = safetyhook::MidHook::create(gameBase + allocation::LampDistanceRva,
            AdjustLampDistance, safetyhook::MidHook::StartDisabled);
        if (!begin || !collect || !finish || !lampDistance)
        {
            installStatus = std::string("hook_creation_failed begin=") + (begin ? "1" : "0") +
                " collect=" + (collect ? "1" : "0") + " finish=" + (finish ? "1" : "0");
            return false;
        }
        selectionHook = std::move(*begin);
        collectHook = std::move(*collect);
        finalizeHook = std::move(*finish);
        lampDistanceHook = std::move(*lampDistance);
        if (!lampDistanceHook.enable() || !collectHook.enable() || !finalizeHook.enable() || !selectionHook.enable())
        {
            installStatus = "hook_enable_failed";
            // Leave any enabled trampolines owned and inert; freeing them while
            // another thread is executing a stub would be unsafe.
            return false;
        }
        ready.store(true, std::memory_order_release);
        installStatus = "enabled";
        OutputDebugStringW(publish ? L"FusionFix experimental shadow allocator: publication enabled.\n" :
            L"FusionFix experimental shadow allocator: observing only; native output unchanged.\n");
        return true;
    }
}
