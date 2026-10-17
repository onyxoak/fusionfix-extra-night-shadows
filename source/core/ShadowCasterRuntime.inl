// OnyxOak modification project: Extra Night Shadows Fix and Better Headlights.
// Project direction, integration and visual testing by OnyxOak; Codex-assisted development.
// Modification notice: 2026-09-27. See ATTRIBUTION.md for upstream credits and GPL-3.0.
// Official release: https://www.nexusmods.com/gta4/mods/1459

// Candidate 14: narrowly scoped immediate-render experiment. AE3310 may queue
// AE0690 instead: that path does not visit the entity hook and is NOT fixed here.
namespace OwnHeadlightCaster
{
    namespace policy = fusionfix::shadows::caster;
    namespace guard = fusionfix::shadows::ce::casterguard;
    static thread_local policy::Context context;
    static bool (*boxExclude216)(uint32_t,uintptr_t,uint32_t) noexcept=nullptr;
    static std::atomic<bool> enabled{false};
    static bool diagnosticCounters=false; // Startup-only; does not control exclusion policy.
    static uintptr_t base = 0; static void (*labRenderObserver)(uint32_t,uint32_t,uint32_t,bool) noexcept=nullptr;
    static std::atomic<uint32_t> passes{0}, deferredPasses{0}, ownPasses{0};
    static std::atomic<uint32_t> casterVisits{0}, carExcluded{0}, occupantsExcluded{0};
    static std::atomic<uint32_t> trafficCarExcluded{0};
    static uintptr_t (*additionalCasterOwner)(uintptr_t) noexcept=nullptr;
    // Set only by our owned dual-lamp adapter installation; recheck the tuple
    // on every pass so foreign/legacy adapters retain their original behavior.
    static decltype(fusionfix::shadows::ce::AdditionalVehicleBeamPriority32) singlePriority146=nullptr;
    static decltype(fusionfix::shadows::ce::AdditionalVehicleBeamOwner) singleOwner146=nullptr;
    static decltype(fusionfix::shadows::ce::AdditionalVehicleBeam) singleBeam146=nullptr;

    static policy::Context Capture(void* renderPass) noexcept
    {
        policy::Context result{};
        if (!enabled.load(std::memory_order_acquire) || !renderPass ||
            ((!bHeadlightShadows || !bVehicleNightShadows) && !additionalCasterOwner && !boxExclude216))
            return result;
        experiment_perf::Count(diagnosticCounters,passes);
        // TLS identity cannot be carried into the deferred renderer. Observe it
        // explicitly and leave that path alone rather than applying stale state.
        if (*reinterpret_cast<const uint8_t*>(base + guard::DeferredFlagRva))
        {
            experiment_perf::Count(diagnosticCounters,deferredPasses);
            return result;
        }
        const auto pass = reinterpret_cast<const uint8_t*>(renderPass);
        if (*reinterpret_cast<const int32_t*>(pass + 0x938) == -1) return result;
        const auto slot = *reinterpret_cast<const uint32_t*>(pass + 0x940);
        if (slot < 1 || slot > 7) return result;
        const auto offset = slot * 0x110;
        const auto key = *reinterpret_cast<const uint32_t*>(base + guard::SlotKeyRva + offset);
        const auto kind = *reinterpret_cast<const uint32_t*>(base + guard::SlotKindRva + offset);
        const bool active = *reinterpret_cast<const uint8_t*>(base + guard::SlotActiveRva + offset) == 1;
        if(active&&boxExclude216)result.boxLightKey216=key;
        ShadowTrace34::Emit({5,CShadows::pFrameCounter?*CShadows::pFrameCounter:0,GetTickCount(),key,
            static_cast<int>(slot),static_cast<int>(kind),active?1:0,0,0,0,0,0});
        if(labRenderObserver)labRenderObserver(key,slot,kind,active);
        const bool headlights=bHeadlightShadows && bVehicleNightShadows;
        const auto car = CPlayer::findPlayerCar?CPlayer::findPlayerCar():0;
        const bool nativeOwn146=car && (key==car || (car<UINTPTR_MAX && key==car+1));
        const bool singleWitness146=headlights && kind==4 && active &&
            (bTrafficSelfShadowFix || (car && !nativeOwn146)) && singlePriority146 &&
            fusionfix::shadows::ce::AdditionalVehicleBeamPriority32==singlePriority146 &&
            fusionfix::shadows::ce::AdditionalVehicleBeamOwner==singleOwner146 &&
            fusionfix::shadows::ce::AdditionalVehicleBeam==singleBeam146;
        fusionfix::shadows::ce::BeamSelection32 selection146{};
        if(singleWitness146) selection146=fusionfix::shadows::ce::ResolveBeamSelection32(key,car);
        // The inherited 64-key submission history is smaller than a busy
        // scene's two-per-car key set. Private lamp authority comes from the
        // fresh pool/model lifetime registry, not that lossy admission cache.
        if (headlights && bTrafficSelfShadowFix && kind == 4 && active &&
            fusionfix::shadows::ce::AdditionalVehicleBeamOwner)
            result.trafficBeamOwner = singleWitness146?selection146.privateOwner:fusionfix::shadows::ce::AdditionalVehicleBeamOwner(key);
        if (bTrafficSelfShadowFix && kind == 4 && active && !result.trafficBeamOwner && additionalCasterOwner)
            result.trafficBeamOwner = additionalCasterOwner(key);
        // Exclude only the source vehicle from its own immediate
        // headlight pass. Confirm the opaque key was recently accepted by our submission
        // adapter; do not infer an owner pointer or dereference that key.
        if (headlights && bTrafficSelfShadowFix && kind == 4 && active &&
            CShadows::gStableHeadlightShadow.IsSubmittedBeam(key))
            result.trafficBeamKey = key;
        if (!headlights || !(singleWitness146?selection146.own:policy::OwnBeam(slot, kind, active, key, car))) return result;
        result.car = car;
        result.ownBeam = true;
        // Audited CE/official FusionFix: driver followed by eight passengers.
        std::memcpy(result.occupants.data(), reinterpret_cast<const void*>(car + 0xF50),
                    sizeof(result.occupants));
        experiment_perf::Count(diagnosticCounters,ownPasses);
        return result;
    }

    static bool Exclude(uintptr_t entity, uint32_t type, bool artificial) noexcept
    {
        if (!enabled.load(std::memory_order_acquire)) return false;
        if(artificial&&context.boxLightKey216&&boxExclude216&&boxExclude216(context.boxLightKey216,entity,type))return true;
        experiment_perf::Count(diagnosticCounters,casterVisits);
        if (((!bHeadlightShadows || !bVehicleNightShadows) && !context.trafficBeamOwner) ||
            !policy::Exclude(context, entity, type, artificial)) return false;
        if (type == 2) { experiment_perf::Count(diagnosticCounters,carExcluded); if (context.trafficBeamKey) experiment_perf::Count(diagnosticCounters,trafficCarExcluded); }
        else experiment_perf::Count(diagnosticCounters,occupantsExcluded);
        return true;
    }
}

namespace ShadowDiagnostics
{
    static std::atomic<bool> ready{false};
    static std::filesystem::path path;
    static uint64_t lastWrite = 0;
    static bool guardPassed = false;
    static int casterMode = 0, allocationMode = 0;
    static std::string startupGuardDetails;
    static bool startupWritten = false;
    static bool admissionInstalled = false;

    static void Write() noexcept
    {
        if (!ready.load(std::memory_order_acquire)) return;
        const auto now = GetTickCount64();
        if (now - lastWrite < 5000) return;
        lastWrite = now;
        ShadowTrace34::Flush();
        // Low-frequency game-event I/O, never inside submission/caster hooks.
        // Failure to write diagnostics must not escape into game code.
        try
        {
            std::ofstream log(path, std::ios::app);
            if (!startupWritten)
            {
                log << "candidate=44 startup_guard " << startupGuardDetails << '\n';
                log << "candidate=44 allocator_startup " << PlayerShadowAllocation::installStatus << '\n';
                if (log.good()) startupWritten = true;
            }
            log << "candidate=44 tick=" << now << " admission_installed=" << admissionInstalled
                << " traffic_self_shadow_fix=" << bTrafficSelfShadowFix
                << " traffic_car_excluded=" << OwnHeadlightCaster::trafficCarExcluded.load()
                << " close_headlight_relevance=" << bCloseHeadlightRelevance << " caster_guard=" << guardPassed
                << " caster_requested=" << casterMode << " caster_enabled=" << OwnHeadlightCaster::enabled.load()
                << " lamp_policy=scene_camera_cache_continuity allocation_mode=" << allocationMode << " allocation_ready=" << PlayerShadowAllocation::ready.load()
                << " allocation_thread_block=" << PlayerShadowAllocation::unsupportedThread.load()

                << " capture_calls=" << PlayerShadowAllocation::captureCalls.load()
                << " capture_valid=" << PlayerShadowAllocation::captureValid.load()
                << " view_w=" << PlayerShadowAllocation::captureWidth.load()
                << " view_h=" << PlayerShadowAllocation::captureHeight.load()
                << " device_w=" << PlayerShadowAllocation::activeWidth.load()
                << " device_h=" << PlayerShadowAllocation::activeHeight.load()
                << " crash_trace=" << (shadow_crash_trace::handler != nullptr)                << " camera_priority=" << PlayerShadowAllocation::cameraPriority
                << " scene_camera_reads=" << PlayerShadowAllocation::sceneCameraReads.load()
                << " auxiliary_views_rejected=" << PlayerShadowAllocation::auxiliaryViewsRejected.load()
                << " camera_passes=" << PlayerShadowAllocation::cameraPasses.load()
                << " camera_fallbacks=" << PlayerShadowAllocation::cameraFallbacks.load()
                << " trace_enabled=" << ShadowTrace34::enabled.load()
                << " trace_dropped=" << ShadowTrace34::recorder.dropped.load()
                << " native_lamp_priority=" << PlayerShadowAllocation::nativeLampPriority
                << " cache_dependency_checks=" << PlayerShadowAllocation::cacheDependencyChecks.load()
                << " cache_dependency_redirected=" << PlayerShadowAllocation::cacheDependencyRedirected.load()
                << " cache_dependency_deferred=" << PlayerShadowAllocation::cacheDependencyDeferred.load()
                << " cache_dependency_rejected=" << PlayerShadowAllocation::cacheDependencyRejected.load()
                << " continuity_compares=" << PlayerShadowAllocation::continuityComparisons.load()
                << " continuity_overrides=" << PlayerShadowAllocation::continuityOverrides.load()
                << " continuity_rejected=" << PlayerShadowAllocation::continuityRejected.load()
                << " lamp_distance_adjusted=" << PlayerShadowAllocation::lampDistanceAdjusted.load()
                << " nearby_receivers=" << NearbyVehicleLighting36::enabled.load()
                << " receiver_captures=" << NearbyVehicleLighting36::captures.load()
                << " receiver_matches=" << NearbyVehicleLighting36::matches.load()
                << " receiver_invalid_pool=" << NearbyVehicleLighting36::invalidPool.load()
                << " lookup_guard=" << ShadowLookupGuard::ready
                << " lamp_removed=" << PlayerShadowAllocation::lampRemoved.load()
                << " lamp_missing_input=" << PlayerShadowAllocation::lampMissingInput.load()
                << " lamp_dropped_present=" << PlayerShadowAllocation::lampDroppedPresent.load()
                << " rejected_dynamic_map=" << ShadowLookupGuard::rejectedDynamic.load()
                << " rejected_static_map=" << ShadowLookupGuard::rejectedStatic.load()
                << " lookup_buffer_changed=" << ShadowLookupGuard::changedBuffer.load()
                << " stale_keys_cleared=" << PlayerShadowAllocation::staleKeysCleared.load()
                << " rejected_adapter=" << PlayerShadowAllocation::rejectedAdapterChecks.load()
                << " rejected_input=" << PlayerShadowAllocation::rejectedPassReasons[2].load()
                << " rejected_duplicate=" << PlayerShadowAllocation::rejectedPassReasons[3].load()
                << " rejected_capacity=" << PlayerShadowAllocation::rejectedPassReasons[4].load()
                << " rejected_commit=" << PlayerShadowAllocation::rejectedPassReasons[5].load()
                << " rejected_snapshot=" << PlayerShadowAllocation::rejectedPassReasons[6].load()
                << " applied=" << PlayerShadowAllocation::appliedPasses.load()
                << " observed=" << PlayerShadowAllocation::observedPasses.load()
                << " fallback=" << PlayerShadowAllocation::fallbackPasses.load()
                << " passes=" << OwnHeadlightCaster::passes.load()
                << " deferred=" << OwnHeadlightCaster::deferredPasses.load()
                << " own_beam_passes=" << OwnHeadlightCaster::ownPasses.load()
                << " caster_visits=" << OwnHeadlightCaster::casterVisits.load()
                << " car_excluded=" << OwnHeadlightCaster::carExcluded.load()
                << " occupants_excluded=" << OwnHeadlightCaster::occupantsExcluded.load()
                << " driving_beam_yes=" << CShadows::drivingBeamAccepted.load()
                << " driving_beam_no=" << CShadows::drivingBeamRejected.load() << '\n';
        }
        catch (...) {}
    }
}






