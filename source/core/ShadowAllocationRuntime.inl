// OnyxOak modification project: Extra Night Shadows Fix and Better Headlights.
// Project direction, integration and visual testing by OnyxOak; Codex-assisted development.
// Modification notice: 2026-09-27. See ATTRIBUTION.md for upstream credits and GPL-3.0.
// Official release: https://www.nexusmods.com/gta4/mods/1459

// Included after module imports and CShadows declarations. This experimental
// adapter is opt-in and has not been run in GTA IV. No install occurs from the
// offline build; the game integration activates only on a later chosen launch.
#include "LabSirenLifetime.hpp"
#include "ShadowPrioritySmoothing20.hpp"
#include "ShadowMapOrdinals20.hpp"
#include "SignalShadowPriority20.hpp"
#include "SceneShadowPriority21.hpp"
#include "SceneScoreContinuity96.hpp"
#include "VisibleReceiverRanking133.hpp"
#include "ShadowKeyMultiplicity27.hpp"
#include "ShadowSelectionLoss38.hpp"
#include "DeferredModelShortlist48.hpp"
#include "ParallelSelectionMath61.hpp"
#include "ShadowTransitionDetector69.hpp"
#include <new>
#include <bit>
#include <type_traits>
namespace PlayerShadowAllocation
{
    namespace allocation = fusionfix::shadows::ce::allocation;
    namespace budget = fusionfix::shadows::budget;
    using fusionfix::shadows::Vec3;
    using fusionfix::shadows::FloatingPointState;

    static SafetyHookInline selectionHook; static thread_local std::array<uint8_t,4096> labRelevant{};
    static std::atomic<uint32_t> labCollected{0},labObserved{0},labNativeChosen{0},labFinalChosen{0};
    static std::array<std::atomic<uint32_t>,3> labComparisons{},labResults{}; static std::atomic<uint32_t> labClaimLoss{0},labOwnLoss{0};
    static int (*labCompareGrace)(int,int,int,int) noexcept=nullptr;
    // Startup-bound exact callback capability. It certifies only key-range
    // neutrality, never a borrowed owner or cached policy/permission decision.
    static bool (*labGracePairNeutral108)(int (*)(int,int,int,int) noexcept,uint32_t,uint32_t) noexcept=nullptr;
    static bool (*labPointSource)(const rage::CLightSource&) noexcept=nullptr;
    static uint64_t (*labLampGeneration)(const rage::CLightSource&) noexcept=nullptr;
    static void (*observeShadowResult)() noexcept=nullptr;
    static void (*observeSelected198C)() noexcept=nullptr;
    static thread_local bool freshView198C=false;
    static void (*observeSubmittedLights)(const rage::CLightSource*,uint32_t) noexcept = nullptr;
    static SafetyHookMid collectHook, finalizeHook, lampDistanceHook, compareResultHook, cacheResultHook, stableOrdinalsHook;
    static std::atomic<uint32_t> stableOrdinalPasses{0},stableOrdinalMoves{0},stableOrdinalRejected{0};
    static std::atomic<uint32_t> ownEligiblePasses{0},ownMissingSelectionPasses{0},retainedMapMoves{0};
    static std::atomic<uint32_t> ambiguousKeyPasses{0},ambiguousKeyInputs{0};
    static void (*prepareSources)(rage::CLightSource*,uint32_t) noexcept=nullptr;
    static void (*liveReachBoundary122)() noexcept=nullptr;
    static bool transitionDetectorEnabled69=false;
    static shadow_transition69::Ring<> transitionEvents69;
    // Owned normalized class only; callbacks never alter native light storage.
    static unsigned (*projectionCollect66)(unsigned,rage::CLightSource*,uint32_t,uint32_t,uintptr_t) noexcept=nullptr;
    static unsigned (*projectionClass66)(unsigned,uint32_t,rage::CLightSource*,uint32_t,uint32_t) noexcept=nullptr;
    static std::atomic<uint32_t> projectionZeroWrites66{0},projectionWouldWrites66{0};
    static bool (*projectionAlias66)(unsigned,uint32_t,rage::CLightSource*,uint32_t,uint32_t) noexcept=nullptr;
    // Startup immutable; false/null leaves the complete original early validator.
    static bool shortlistRequested48=false;
    static bool (*shortlistGetterSafe48)() noexcept=nullptr;
    static void (*bulkModelSources48)(rage::CLightSource*,uint32_t) noexcept=nullptr;
    static bool (*validateModelSource48)(rage::CLightSource&) noexcept=nullptr;
    static std::atomic<uint32_t> shortlistDeferred48{0},shortlistValidated48{0},shortlistInvalid48{0},shortlistFallback48{0},shortlistResident48{0},shortlistPasses48{0};
    static void FlushPending48() noexcept;
    static bool nativeLampPriority=false;
    static std::atomic<uint32_t> lampDistanceAdjusted{0};
    static std::atomic<uint32_t> cacheDependencyChecks{0}, cacheDependencyRedirected{0}, cacheDependencyDeferred{0}, cacheDependencyRejected{0};
    static std::atomic<uint32_t> continuityComparisons{0}, continuityOverrides{0}, continuityRejected{0};
    static std::atomic<bool> ready{false}, unsupportedThread{false};
    static std::atomic<DWORD> ownerThread{0};
    static uintptr_t gameBase = 0;
    static bool cameraPriority = false;
    static std::atomic<uint32_t> cameraPasses{0}, cameraFallbacks{0};
    static std::atomic<uint32_t> auxiliaryViewsRejected{0}, sceneCameraReads{0};
    static bool publicationEnabled = false; // Immutable after ready is published.
    static bool additionalFeatures = false;
    static float signalShadowPriority = 1.5f;
    static float scenePriorityStrength = 1.5f,scenePriorityReleaseMs = 450.f;
    static bool visibleReceiverExperiment133=false; // Startup-only default-OFF.
    static bool (*visibleReceiverSource133)(const rage::CLightSource&,unsigned,rage::CLightSource*,uint32_t,uint32_t) noexcept=nullptr;
    static std::string installStatus = "not_requested"; // Init-only; diagnostics reads after ready publication.
    // Diagnostics are counters only: no per-frame logging/allocations or I/O.
    static std::atomic<uint32_t> appliedPasses{0}, observedPasses{0}, fallbackPasses{0}, staleKeysCleared{0};

    struct State
    {
        budget::ShadowAllocationPass pass;
        Vec3 player{}, drivingFocus{};
        fusionfix::shadows::ShadowDrivingFocus motionFocus;
        std::array<budget::PlayerShadowBudget::Slot,7> previousSelection{};
        uint32_t previousSelectionFrame=0;
        shadow_transition69::Detector transitionDetector69;
        fusionfix::shadows::NativeLampContinuity41 lampContinuity;
        fusionfix::shadows::NativeShadowContinuity42 continuity;
        fusionfix::shadows::ShadowPrioritySmoothing20 trafficPriority;
        fusionfix::shadows::SceneShadowHistory21 sceneHistory;
        fusionfix::shadows::SceneScoreContinuity96 sceneScoreHistory;
        fusionfix::shadows::ShadowKeyMultiplicity27 keyMultiplicity;
        std::array<fusionfix::shadows::NativeShadowContinuity42::Candidate,4096> nativeCandidates{};
        // Written/read only while diagnostics enabled; frame tags reject stale entries.
        std::array<fusionfix::shadows::SelectionScoreDiagnostic38,4096> scoreDiagnostics38{};
        unsigned parallelPhases61=0;
        bool continuityActive=false;
        bool tracedComparison=false;
        unsigned ownEligibleCount=0;
        fusionfix::shadows::ShadowView view{};
        fusionfix::shadows::ShadowVolumeFrustum20 volumeFrustum{};
        uintptr_t ped = 0, occupiedCar = 0, lastCar = 0;
        uint32_t frame = 0, viewFrame = 0;
        uintptr_t stackAnchor = 0;
        unsigned depth = 0;
        bool committed = false;
        bool deferModels48=false;
        rage::CLightSource* shortlistLights48=nullptr;
        uint32_t shortlistCount48=0;
        std::array<uint8_t,640> pending48{};
        std::array<uint32_t,16> resident48{};
    };
    static thread_local State state;
    // Requested only at startup. OFF does not allocate or create worker threads.
    static bool parallelRequested61=false,parallelMasterEnabled61=false;
    static std::atomic<bool> parallelAttempted61{false};
    static std::atomic<unsigned> parallelStartupStatus61{0}; // 0 disabled,1 starting,2 pin,3 allocation,4 thread,5 transfer failure,6 ready
#ifndef CE_PARALLEL_PIN61
#define CE_PARALLEL_PIN61(flags,address,module) GetModuleHandleExW(flags,address,module)
#endif
#ifndef CE_PARALLEL_ALLOC61
#define CE_PARALLEL_ALLOC61() new(std::nothrow) fusionfix::shadows::parallel61::Service
#endif
    static std::atomic<fusionfix::shadows::parallel61::Service*> parallelService61{nullptr};
    static fusionfix::shadows::parallel61::Context ParallelContext61() noexcept {
        fusionfix::shadows::parallel61::Context c{};c.view=state.view;c.player=state.player;c.focus=state.drivingFocus;
        c.driving=state.occupiedCar!=0;c.strength=scenePriorityStrength;
        c.visibleReceiver133=visibleReceiverExperiment133&&scenePriorityStrength>0;
        if(c.visibleReceiver133){const int feet=fusionfix::shadows::ShadowReachFeet(FusionFixSettings.Get("PREF_LAMP_REACH"));c.lampReach133=feet>0?feet*.3048f:60.f;}
        fusionfix::shadows::parallel61::CaptureControl(c);return c;
    }
    // Called once from standalone startup worker, never from a native hook.
    static void StartParallel61() noexcept {
        if(!parallelMasterEnabled61||!parallelRequested61||parallelAttempted61.exchange(true,std::memory_order_acq_rel))return;
        parallelStartupStatus61.store(1,std::memory_order_relaxed);
        const fusionfix::shadows::parallel61::Scope preserve;
        // Existing runtime has no safe dynamic unload contract. Pin before any
        // opt-in thread can run our code; process termination releases resources.
        HMODULE module{};
        if(!CE_PARALLEL_PIN61(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
            reinterpret_cast<LPCWSTR>(&StartParallel61),&module)){parallelStartupStatus61=2;return;}
        auto* service=CE_PARALLEL_ALLOC61();
        if(!service){parallelStartupStatus61=3;return;}
        if(!service->Start(2)){delete service;parallelStartupStatus61=4;return;}
        if(!service->ReleaseProducer()){delete service;parallelStartupStatus61=5;return;}
        parallelService61.store(service,std::memory_order_release); // first authenticated selection owner claims producer
        parallelStartupStatus61.store(6,std::memory_order_release);
    }

    struct Invocation
    {
        Invocation() noexcept { ++state.depth; if (state.depth == 1) state.committed = false; }
        ~Invocation() noexcept { if(state.depth==1){state.deferModels48=false;state.pending48.fill(0);} state.pass.EndInvocation(); --state.depth; }
        Invocation(const Invocation&) = delete;
        Invocation& operator=(const Invocation&) = delete;
    };
    static std::atomic<uint32_t> lampRemoved{0}, lampMissingInput{0}, lampDroppedPresent{0};
    static std::array<std::atomic<uint32_t>,8> rejectedPassReasons{};
    static std::atomic<uint32_t> rejectedAdapterChecks{0};
    static void RejectPass() noexcept
    {
        if (state.pass.Active()) { ++fallbackPasses; ++rejectedAdapterChecks; }
        FlushPending48();
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
        return addonActive.load(std::memory_order_acquire) && ready.load(std::memory_order_acquire) && !unsupportedThread.load(std::memory_order_relaxed) &&
            (bExtraNightShadows || (bHeadlightShadows && bVehicleNightShadows) || additionalFeatures);
    }
    static void FlushPending48() noexcept {
        if(!state.deferModels48)return;
        state.deferModels48=false;++shortlistFallback48;
        // Only unchecked zero-write inputs remain pending. Never clear a flag
        // on an already certified prefix occupant after native insertion.
        if(CurrentLights()!=state.shortlistLights48 || CurrentCount()!=state.shortlistCount48)return;
        for(unsigned i=0;i<state.shortlistCount48;++i)if(state.pending48[i]){
            state.pending48[i]=0;if(validateModelSource48)validateModelSource48(state.shortlistLights48[i]);
        }
    }
    static bool DeferModel48(rage::CLightSource& light,unsigned index) noexcept {
        if(!state.deferModels48 || index>=state.shortlistCount48 ||
           &light!=state.shortlistLights48+index || !(light.mFlags&4u) ||
           (light.mFlags&0x102u) || light.mShadowCacheIndex!=-1)return false;
        const auto key=uint32_t(light.mCastShadows);
        for(auto resident:state.resident48)if(key==resident){++shortlistResident48;return false;}
        state.pending48[index]=1;++shortlistDeferred48;return true;
    }
    static void BeginShortlist48() noexcept {
        state.deferModels48=false;state.pending48.fill(0);
        if(!shortlistRequested48 || !Enabled() || !publicationEnabled || !nativeLampPriority ||
           labCompareGrace || !validateModelSource48 || !shortlistGetterSafe48 || !shortlistGetterSafe48())return;
        auto* lights=CurrentLights();const auto count=CurrentCount();
        if(reinterpret_cast<uintptr_t>(lights)<0x10000 || count>639)return;
        // Native map record 0..7 and static cache 0..7 have different strides.
        // Even inactive records' keys stay early: update scheduling may reuse them.
        for(unsigned i=0;i<8;++i){
            state.resident48[i]=*reinterpret_cast<const uint32_t*>(gameBase+0xD9F1F8+i*0x110);
            state.resident48[8+i]=*reinterpret_cast<const uint32_t*>(gameBase+0xDA0BF0+i*0x100);
        }
        state.shortlistLights48=lights;state.shortlistCount48=count;state.deferModels48=true;++shortlistPasses48;
    }
    struct GracePrefix108 {
        struct Entry {int32_t index;uint32_t flags,key,distanceBits;std::array<uint32_t,9> candidate;};
        std::array<Entry,8> entries;
        uintptr_t stack,lights;
        uint32_t count,frame;
        int (*compare)(int,int,int,int) noexcept;
        bool (*neutral)(int (*)(int,int,int,int) noexcept,uint32_t,uint32_t) noexcept;
    };
    static_assert(std::is_trivially_default_constructible_v<GracePrefix108>);
    static std::array<uint32_t,9> GraceCandidateValues108(const fusionfix::shadows::NativeShadowContinuity42::Candidate& a) noexcept {
        // Named fields only: padding never supplies a stability certificate.
        return {a.flags,a.claim,uint32_t(a.ownBeam),uint32_t(a.observed),a.stableOrder,a.focusTier,a.identity,
            std::bit_cast<uint32_t>(a.focusDistance),std::bit_cast<uint32_t>(a.visualImportance)};
    }
    template<class Lights> static bool CaptureGracePrefix108(uintptr_t stack,uint32_t index,Lights lights,uint32_t count,GracePrefix108& proof) noexcept {
        if(!Enabled()||!labCompareGrace||!labGracePairNeutral108||state.depth!=1||!state.pass.Active()||
           !state.continuityActive||!CShadows::pFrameCounter||*CShadows::pFrameCounter!=state.frame||
           !stack||stack!=state.stackAnchor||lights!=CurrentLights()||count!=CurrentCount()||
           count>state.nativeCandidates.size()||index>=count)return false;
        // Fill only current evidence. A failed capture is never consumed;
        // ordinary failures need no initialization of the complete snapshot.
        proof.stack=stack;proof.lights=reinterpret_cast<uintptr_t>(lights);
        proof.count=count;proof.frame=state.frame;proof.compare=labCompareGrace;proof.neutral=labGracePairNeutral108;
        for(unsigned cell=0;cell<8;++cell){
            auto& entry=proof.entries[cell];
            entry.index=cell?*reinterpret_cast<const int32_t*>(stack+0x7C+(cell-1)*4):static_cast<int32_t>(index);
            if(entry.index<0||uint32_t(entry.index)>=count)return false;
            const auto& light=lights[entry.index];const auto& candidate=state.nativeCandidates[entry.index];
            entry.flags=cell?*reinterpret_cast<const uint32_t*>(stack+0x9C+(cell-1)*4):light.mFlags;
            entry.key=static_cast<uint32_t>(light.mCastShadows);
            entry.candidate=GraceCandidateValues108(candidate);
            entry.distanceBits=*reinterpret_cast<const uint32_t*>(stack+(cell?0x5C+(cell-1)*4:0x20));
            if(!entry.key||entry.key==UINT32_MAX||!candidate.observed||candidate.flags!=entry.flags||
               light.mFlags!=entry.flags||candidate.identity!=entry.key)return false;
            if(cell&&!proof.neutral(proof.compare,proof.entries[0].key,entry.key))return false;
        }
        return true;
    }
    template<class Lights> static bool CurrentGracePrefix108(uintptr_t stack,uint32_t index,Lights lights,uint32_t count,const GracePrefix108& proof) noexcept {
        if(!Enabled()||proof.compare!=labCompareGrace||proof.neutral!=labGracePairNeutral108||state.depth!=1||
           !state.pass.Active()||!state.continuityActive||!CShadows::pFrameCounter||
           *CShadows::pFrameCounter!=proof.frame||state.frame!=proof.frame||stack!=proof.stack||
           stack!=state.stackAnchor||reinterpret_cast<uintptr_t>(lights)!=proof.lights||
           lights!=CurrentLights()||count!=proof.count||count!=CurrentCount()||index>=count)return false;
        for(unsigned cell=0;cell<8;++cell){
            const auto& entry=proof.entries[cell];
            const auto currentIndex=cell?*reinterpret_cast<const int32_t*>(stack+0x7C+(cell-1)*4):static_cast<int32_t>(index);
            if(currentIndex!=entry.index||currentIndex<0||uint32_t(currentIndex)>=count)return false;
            const auto& light=lights[currentIndex];const auto& candidate=state.nativeCandidates[currentIndex];
            if(static_cast<uint32_t>(light.mCastShadows)!=entry.key||light.mFlags!=entry.flags||
               GraceCandidateValues108(candidate)!=entry.candidate||
               (cell&&*reinterpret_cast<const uint32_t*>(stack+0x9C+(cell-1)*4)!=entry.flags)||
               *reinterpret_cast<const uint32_t*>(stack+(cell?0x5C+(cell-1)*4:0x20))!=entry.distanceBits)return false;
            if(cell&&!proof.neutral(proof.compare,proof.entries[0].key,entry.key))return false;
        }
        return true;
    }
    template<class Lights> static bool PreviewWritesBank66(uintptr_t stack,uint32_t index,Lights lights,uint32_t count) noexcept {
        namespace plan=fusionfix::shadows::shortlist48;
        plan::Cell carry{static_cast<int32_t>(index),lights[index].mFlags,
            *reinterpret_cast<const float*>(stack+0x20)};
        bool wrote=false;
        for(unsigned row=0;row<7;++row){
            const unsigned offset=row*4;
            plan::Cell cell{*reinterpret_cast<const int32_t*>(stack+0x7C+offset),
                *reinterpret_cast<const uint32_t*>(stack+0x9C+offset),
                *reinterpret_cast<const float*>(stack+0x5C+offset)};
            int choice=plan::NativeFlags(carry.flags,cell.flags);
            if(carry.index>=0 && cell.index>=0 && uint32_t(carry.index)<count && uint32_t(cell.index)<count){
                const auto& a=state.nativeCandidates[carry.index];const auto& b=state.nativeCandidates[cell.index];
                if(a.observed&&b.observed&&a.flags==carry.flags&&b.flags==cell.flags)
                    choice=fusionfix::shadows::NativeShadowContinuity42::Compare(choice,a,b);
            }
            if(choice==0 || (choice==1&&cell.distance>carry.distance)){carry=cell;wrote=true;}
        }
        return wrote;
    }
    static bool PreviewWrites48(uintptr_t stack,uint32_t index) noexcept {
        return PreviewWritesBank66(stack,index,state.shortlistLights48,state.shortlistCount48);
    }
    static std::atomic<uint32_t> captureCalls{0}, captureValid{0}, perspectiveCalls{0};
    static std::atomic<int> captureWidth{0}, captureHeight{0}, activeWidth{0}, activeHeight{0};
    static SafetyHookMid cameraCaptureHook;
    static std::atomic_flag gameplayViewLock = ATOMIC_FLAG_INIT;
    static fusionfix::shadows::ShadowView gameplayView{};
    static uint32_t gameplayViewFrame = 0;
    static const rage::grcViewport* SceneViewport() noexcept
    {
        // The guarded native selection reads this exact table before touching
        // the same scene object. Never dereference a light/reflection viewport
        // simply because its resolution happens to match the back buffer.
        if(!ready.load(std::memory_order_acquire) || !gameBase) return nullptr;
        const auto* table=reinterpret_cast<const uint32_t*>(gameBase+allocation::SceneCameraTableRva);
        const auto index=table[0];
        if(index<1 || index>3) return nullptr;
        const auto camera=static_cast<uintptr_t>(table[index]);
        if(camera<0x10000 || camera>UINTPTR_MAX-allocation::SceneViewportOffset-sizeof(rage::grcViewport)) return nullptr;
        return reinterpret_cast<const rage::grcViewport*>(camera+allocation::SceneViewportOffset);
    }
    static_assert(offsetof(rage::grcViewport,mViewMatrix)==0x180);
    static_assert(offsetof(rage::grcViewport,mProjectionMatrix)==0x1C0);
    static_assert(offsetof(rage::grcViewport,mWidth)==0x2B0);
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
    static fusionfix::shadows::ShadowView ReadGameplayView() noexcept
    {
        auto view=ReadView(SceneViewport());
        if(view.valid)++sceneCameraReads;
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
            if(viewport!=SceneViewport()) {++auxiliaryViewsRejected;return;}
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
        if (ped != state.ped) {
            state.lastCar = 0; state.view = {};
            state.previousSelection = {}; state.previousSelectionFrame = 0;
        }
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
        state.lampContinuity.Begin(ped,state.frame,static_cast<uint32_t>(*CTimer::m_snTimeInMilliseconds));
        state.continuity.Begin(ped,state.frame,static_cast<uint32_t>(*CTimer::m_snTimeInMilliseconds));
        state.trafficPriority.Begin(ped);
        state.sceneHistory.Begin(ped,state.frame,static_cast<uint32_t>(*CTimer::m_snTimeInMilliseconds));
        state.sceneScoreHistory.Begin(ped,state.occupiedCar,state.frame,static_cast<uint32_t>(*CTimer::m_snTimeInMilliseconds));
        state.nativeCandidates.fill({}); labRelevant.fill(0);
        state.continuityActive=false;
        state.tracedComparison=false;
        state.ownEligibleCount=0;
        state.stackAnchor = 0;
        state.view = {}; freshView198C=false;
        if (cameraPriority && rage::pCurrentViewport) {
            state.view = ReadGameplayView(); freshView198C=state.view.valid;
            // Shadow passes replace the current viewport. Keep only a recent,
            // full-resolution perspective view. Selection can consume a view on a different
            // thread; publish a consistent snapshot without waiting in either hook.
            if (!state.view.valid && !gameplayViewLock.test_and_set(std::memory_order_acquire)) {
                if (gameplayView.valid && state.frame - gameplayViewFrame <= 1) state.view = gameplayView;
                gameplayViewLock.clear(std::memory_order_release);
            }
            if (state.view.valid) ++cameraPasses; else ++cameraFallbacks;
        }
        state.volumeFrustum=fusionfix::shadows::PrepareShadowVolumeFrustum20(state.view);
        if (reinterpret_cast<uintptr_t>(CurrentLights()) < 0x10000) return false;
        state.keyMultiplicity.Begin();
        const auto inputCount=CurrentCount();
        if(inputCount>budget::ShadowAllocationPass::Capacity) return false;
        for(unsigned i=0;i<inputCount;++i)
            if(!(projectionAlias66&&projectionAlias66(i,static_cast<uint32_t>(CurrentLights()[i].mCastShadows),CurrentLights(),inputCount,state.frame))&&
               !state.keyMultiplicity.Add(static_cast<uint32_t>(CurrentLights()[i].mCastShadows),i))return false;
        unsigned ambiguous=0;
        for(unsigned i=0;i<inputCount;++i)ambiguous+=state.keyMultiplicity.Ambiguous(i)?1u:0u;
        if(ambiguous){++ambiguousKeyPasses;ambiguousKeyInputs.fetch_add(ambiguous,std::memory_order_relaxed);}
        state.pass.Begin({state.frame, static_cast<uint32_t>(*CTimer::m_snTimeInMilliseconds),
                          ped, state.occupiedCar != 0}, CurrentLights(), CurrentCount(),
                          publicationEnabled && nativeLampPriority);
        state.continuityActive=publicationEnabled && nativeLampPriority && state.pass.Active();
        if(state.continuityActive && ShadowTrace34::enabled.load(std::memory_order_relaxed)) {
            const auto tick=GetTickCount();
            for(int row=0;row<4;++row) {
                const auto* v=state.view.view[row];const auto* p=state.view.projection[row];
                ShadowTrace34::Emit({8,state.frame,tick,static_cast<uint32_t>(row+1),row,0,0,0,v[0],v[1],v[2],v[3]});
                ShadowTrace34::Emit({9,state.frame,tick,static_cast<uint32_t>(row+1),row,0,0,0,p[0],p[1],p[2],p[3]});
            }
            ShadowTrace34::Emit({10,state.frame,tick,static_cast<uint32_t>(state.ped),state.occupiedCar?1:0,0,0,0,
                state.player.x,state.player.y,state.player.z,0});
        }
        state.parallelPhases61=0;
        if(state.pass.Active()&&parallelRequested61&&parallelService61.load(std::memory_order_acquire)&&!unsupportedThread.load(std::memory_order_relaxed))
            state.parallelPhases61=parallelService61.load(std::memory_order_acquire)->BeginStable(ParallelContext61());
        return state.pass.Active();
    }

    static bool InfluencesPlayer(const rage::CLightSource& light) noexcept
    {
        const Vec3 position{light.mPosition.x,light.mPosition.y,light.mPosition.z};
        const Vec3 direction{light.mDirection.x,light.mDirection.y,light.mDirection.z};
        if ((light.mFlags & 0x100u) && !(labPointSource && labPointSource(light))) {
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
        if(labLampGeneration){const auto generation=labLampGeneration(light);if(generation)return generation;}
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
        return lab_siren::Generation(labPointSource && labPointSource(light),hash);
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
        float visibleWeight=fusionfix::shadows::ShadowViewWeight(state.view,position,
            {light.mDirection.x,light.mDirection.y,light.mDirection.z},light.mRadius,false);
        float viewWeight=1.0f+(visibleWeight-1.0f)*(state.occupiedCar?1.0f:0.5f);
        const int feet=fusionfix::shadows::ShadowReachFeet(FusionFixSettings.Get("PREF_LAMP_REACH"));
        const float reach=feet>0?feet*0.3048f:60.0f;
        const float original=regs.xmm0.f32[0];
        const float distanceSquared=fusionfix::shadows::EvaluateGeometry(state.player,position).distanceSquared;
        float receiverBenefit133=0.f;
        if(visibleReceiverExperiment133&&scenePriorityStrength>0&&
            fusionfix::shadows::VisibleReceiverEmission133(light.mIntensity,{light.mColor.x,light.mColor.y,light.mColor.z}))
            receiverBenefit133=fusionfix::shadows::VisibleReceiverBenefit133(true,true,state.view.valid,
                fusionfix::shadows::ShadowVolumeMayReachView(state.volumeFrustum,position,light.mRadius),
                distanceSquared,light.mRadius,reach,visibleWeight,uint32_t(light.mType),light.mOuterConeAngle);
        if(receiverBenefit133>0&&(!visibleReceiverSource133||
            !visibleReceiverSource133(light,index,const_cast<rage::CLightSource*>(lights),count,state.frame)))receiverBenefit133=0;
        if(receiverBenefit133>0) {
            const float forward163=fusionfix::shadows::SceneryReceiverFactor163(state.view,position,
                {light.mDirection.x,light.mDirection.y,light.mDirection.z},light.mRadius,uint32_t(light.mType));
            visibleWeight=1.f+(visibleWeight-1.f)*forward163;
            viewWeight=1.f+(visibleWeight-1.f)*(state.occupiedCar?1.f:.5f);
        }
        const bool retained=!state.keyMultiplicity.Ambiguous(index) && state.lampContinuity.Retained(static_cast<uint32_t>(light.mCastShadows),
            LampGeometry(light),fusionfix::shadows::ShadowVolumeMayReachView(state.volumeFrustum,position,light.mRadius) &&
                std::isfinite(distanceSquared) && fusionfix::shadows::VisibleReceiverRelevant133(distanceSquared<reach*reach,receiverBenefit133));
        // Keep the softer on-foot acquisition preference, but do not halve an
        // incumbent's visibility merely because the player is walking.
        const float effectiveWeight=retained ? visibleWeight : viewWeight;
        const float inherited=fusionfix::shadows::NativeLampPriorityDistance(original,
            distanceSquared,effectiveWeight,reach,retained);
        const float adjusted=receiverBenefit133>0?fusionfix::shadows::VisibleReceiverNativeDistance133(inherited,receiverBenefit133):inherited;
        if(adjusted!=original && std::isfinite(adjusted) && adjusted>=0) {
            regs.xmm0.f32[0]=adjusted; ++lampDistanceAdjusted;
        }
    }

    static void Collect(SafetyHookContext& regs) noexcept
    {
        const FloatingPointState fp;
        auto rejectCollect48=[&]() noexcept {
            RejectPass();
            // Bulk pending validation can reject the current packet after
            // native loaded its flags. Only the authenticated Collect layout
            // can redirect this one iteration; no guessed function epilogue.
            const auto i=uint32_t(regs.edx);
            if(state.shortlistLights48==CurrentLights() && state.shortlistCount48==CurrentCount() &&
               i<state.shortlistCount48 && regs.edi==reinterpret_cast<uintptr_t>(CurrentLights()) &&
               regs.esi==i*sizeof(rage::CLightSource) && !(CurrentLights()[i].mFlags&4u))
                regs.eip=gameBase+0x527DDC;
        };
        if (!Enabled() || state.depth != 1 || !state.pass.Active()) return;
        if (!state.stackAnchor) state.stackAnchor = regs.esp;
        if (state.stackAnchor != regs.esp) { rejectCollect48(); return; }
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
            rejectCollect48();
            return;
        }
        const auto& light = lights[index];
        const auto flags = *reinterpret_cast<const uint32_t*>(regs.esp + 0x1C);
        if (flags != light.mFlags || !(flags & 4u) || ((flags & 2u) && light.mShadowCacheIndex < 0))
        {
            rejectCollect48();
            return;
        }
        const auto projectionClass=projectionClass66?projectionClass66(index,uint32_t(light.mCastShadows),lights,count,state.frame):0u;
        if(labPointSource && labPointSource(light)) ++labCollected;
        const auto key = static_cast<uint32_t>(light.mCastShadows); // Audited opaque +60 key.
        auto geometry = fusionfix::shadows::EvaluateGeometry(state.player,
            {light.mPosition.x, light.mPosition.y, light.mPosition.z});
        auto kind = budget::Kind::Lamp;
        const bool vehicleSource=(flags&0x100u)&&!(labPointSource&&labPointSource(light));
        const auto beamSelection=vehicleSource?fusionfix::shadows::ce::ResolveBeamSelection32(key,state.occupiedCar):
            fusionfix::shadows::ce::BeamSelection32{};
        if(vehicleSource)
            kind = beamSelection.own ? budget::Kind::PlayerBeam : budget::Kind::OtherBeam;
        if(kind==budget::Kind::PlayerBeam)++state.ownEligibleCount;
        if ((flags & 0x100u) && !(labPointSource && labPointSource(light)))
            geometry.distanceSquared = fusionfix::shadows::ReceiverDistanceSquared(state.player,
                {light.mPosition.x,light.mPosition.y,light.mPosition.z},state.occupiedCar ? 3.0f : 1.5f);
        const int feet = fusionfix::shadows::ShadowReachFeet(FusionFixSettings.Get(
            kind == budget::Kind::Lamp ? "PREF_LAMP_REACH" : "PREF_HEADLIGHT_REACH"));
        const float configuredReach = feet > 0 ? feet * 0.3048f : (kind == budget::Kind::Lamp ? 60.0f : 35.0f);
        const float reach = kind == budget::Kind::OtherBeam && std::isfinite(light.mRadius) && light.mRadius > 0
            ? (std::max)(configuredReach,light.mRadius) : configuredReach;
        float viewWeight{};
        if((state.parallelPhases61&1u)&&parallelRequested61&&parallelService61.load(std::memory_order_acquire)&&!unsupportedThread.load(std::memory_order_relaxed)) {
            fusionfix::shadows::parallel61::Input in{};in.slot=index*2;in.key=key;in.phase=0;
            in.position={light.mPosition.x,light.mPosition.y,light.mPosition.z};
            in.direction={light.mDirection.x,light.mDirection.y,light.mDirection.z};in.radius=light.mRadius;
            in.beam=(flags&0x100u)!=0 && !(labPointSource && labPointSource(light));
            const auto context=ParallelContext61();fusionfix::shadows::parallel61::Output result{};
            if(parallelService61.load(std::memory_order_acquire)->TryGet(context,in,result))viewWeight=result.viewWeight;
            else viewWeight=fusionfix::shadows::ShadowViewWeight(state.view,
                {in.position[0],in.position[1],in.position[2]},{in.direction[0],in.direction[1],in.direction[2]},in.radius,in.beam!=0);
            (void)parallelService61.load(std::memory_order_acquire)->Record(context,in);
        } else {
        viewWeight=fusionfix::shadows::ShadowViewWeight(state.view,
            {light.mPosition.x,light.mPosition.y,light.mPosition.z},
            {light.mDirection.x,light.mDirection.y,light.mDirection.z},light.mRadius,(flags&0x100u)!=0 && !(labPointSource && labPointSource(light)));
        }
        const float priorityDistance=state.occupiedCar && kind==budget::Kind::Lamp && viewWeight>1.0f
            ? fusionfix::shadows::DrivingLampPriorityDistance(state.player,state.drivingFocus,
                {light.mPosition.x,light.mPosition.y,light.mPosition.z},geometry.distanceSquared,viewWeight)
            : -1.0f;
        const auto generation=kind==budget::Kind::Lamp?LampGeometry(light):0;
        const bool affectsPlayer=InfluencesPlayer(light);
        const bool deferredCurrent48=state.deferModels48&&index<640&&state.pending48[index];
        if(!deferredCurrent48){
            state.pass.Observe({key,index,kind,geometry.distanceSquared,affectsPlayer,true,
                generation,viewWeight,reach*reach,priorityDistance},&light);
            if(labPointSource&&labPointSource(light)&&state.pass.Active())++labObserved;
        }
        if(state.continuityActive && state.pass.Active()) {
            // Eligibility/cache checks above are native and current-frame.
            // Protect the whole visible influence volume, not screen center.
            const bool volumeVisible=fusionfix::shadows::ShadowVolumeMayReachView(state.volumeFrustum,
                {light.mPosition.x,light.mPosition.y,light.mPosition.z},light.mRadius);
            // RECEIVER133_RELEVANCE_BEGIN
            float receiverBenefit133=visibleReceiverExperiment133&&scenePriorityStrength>0&&kind==budget::Kind::Lamp&&
                fusionfix::shadows::VisibleReceiverEmission133(light.mIntensity,{light.mColor.x,light.mColor.y,light.mColor.z})?
                fusionfix::shadows::VisibleReceiverBenefit133(true,true,state.view.valid,volumeVisible,
                    geometry.distanceSquared,light.mRadius,reach,viewWeight,uint32_t(light.mType),light.mOuterConeAngle):0.f;
            const bool sourceSupported133=receiverBenefit133>0&&visibleReceiverSource133&&
                visibleReceiverSource133(light,index,lights,count,state.frame);
            if(!sourceSupported133)receiverBenefit133=0;
            const bool relevant=std::isfinite(geometry.distanceSquared) &&
                fusionfix::shadows::VisibleReceiverRelevant133(geometry.distanceSquared<=reach*reach,receiverBenefit133) &&
                (volumeVisible || kind==budget::Kind::PlayerBeam);
            // RECEIVER133_RELEVANCE_END
            labRelevant[index]=relevant?1:0; state.nativeCandidates[index]=state.continuity.Observe(
                {key,generation},flags,relevant&&!state.keyMultiplicity.Ambiguous(index),kind==budget::Kind::PlayerBeam);
            // Driving focus is a physical-distance prediction, not a camera
            // score. Apply the same camera scale to both branches; otherwise
            // a visible driving lamp loses up to9x priority against traffic.
            float focusDistance=(priorityDistance>=0?priorityDistance:
                  geometry.distanceSquared)/(viewWeight*viewWeight);
            const bool traceScore38=ShadowTrace34::enabled.load(std::memory_order_relaxed);
            fusionfix::shadows::SelectionScoreDiagnostic38* score38=nullptr;
            if(traceScore38) {
                score38=&state.scoreDiagnostics38[index];*score38={};
                score38->frame=state.frame;score38->generation=generation;
                score38->rawFocus=focusDistance;score38->viewWeight=viewWeight;
            }
            // Physical headlights retain native priority. All other current
            // eligible sources (including tiny own rear lamps) share receiver
            // consequence instead of repeated source-origin distance penalties.
            const bool protectedForward=beamSelection.own && (beamSelection.order>0 || (flags&0x400u));
            if(scenePriorityStrength>0&&relevant&&!protectedForward){
                const Vec3 source{light.mPosition.x,light.mPosition.y,light.mPosition.z};
                const Vec3 direction{light.mDirection.x,light.mDirection.y,light.mDirection.z};
                fusionfix::shadows::SceneReceiverConsequence44 consequence{};
                if((state.parallelPhases61&2u)&&parallelRequested61&&parallelService61.load(std::memory_order_acquire)&&!unsupportedThread.load(std::memory_order_relaxed)) {
                    fusionfix::shadows::parallel61::Input in{};in.slot=index*2+1;in.key=key;
                    in.generationLow=uint32_t(generation);in.generationHigh=uint32_t(generation>>32);in.phase=1;
                    in.position={source.x,source.y,source.z};in.direction={direction.x,direction.y,direction.z};
                    in.radius=light.mRadius;in.intensity=light.mIntensity;in.type=uint32_t(light.mType);in.outerCone=light.mOuterConeAngle;
                    in.rgb={light.mColor.x,light.mColor.y,light.mColor.z};in.viewWeight=viewWeight;in.volumeVisible=volumeVisible;
                    if(visibleReceiverExperiment133)in.beam=sourceSupported133?0u:1u; // Numeric exclusion, never a worker owner lookup.
                    const auto context=ParallelContext61();fusionfix::shadows::parallel61::Output result{};
                    if(!parallelService61.load(std::memory_order_acquire)->TryGet(context,in,result))result=fusionfix::shadows::parallel61::Compute(context,in);
                    consequence=result.consequence;(void)parallelService61.load(std::memory_order_acquire)->Record(context,in);
                } else {
                const float subjectDistance=fusionfix::shadows::EvaluateGeometry(state.player,source).distanceSquared;
                const float roadDistance=state.occupiedCar?fusionfix::shadows::EvaluateGeometry(state.drivingFocus,
                    source).distanceSquared:subjectDistance;
                const float playerCoverage=fusionfix::shadows::SceneReceiverCoverage21(state.player,source,direction,light.mType,light.mOuterConeAngle);
                const float roadCoverage=state.occupiedCar?fusionfix::shadows::SceneReceiverCoverage21(state.drivingFocus,source,direction,light.mType,light.mOuterConeAngle):playerCoverage;
                consequence=fusionfix::shadows::SceneConsequence44(subjectDistance,roadDistance,
                    light.mRadius,light.mIntensity,{light.mColor.x,light.mColor.y,light.mColor.z},viewWeight,
                    volumeVisible,state.occupiedCar!=0,playerCoverage,roadCoverage,scenePriorityStrength);
                if(sourceSupported133)consequence=fusionfix::shadows::VisibleReceiverScore133(consequence,receiverBenefit133,
                    subjectDistance,light.mRadius,light.mIntensity,{light.mColor.x,light.mColor.y,light.mColor.z},viewWeight,volumeVisible,
                    fusionfix::shadows::SceneryReceiverFactor163(state.view,source,direction,light.mRadius,uint32_t(light.mType)));
                }
                if(consequence.valid){
                    const float importance=1.f+std::min(4.f,consequence.consequence);
                    // Retention remains one scalar per candidate. No historical
                    // source can authorize a missing map or current geometry.
                    state.nativeCandidates[index].visualImportance=importance;
                    focusDistance=state.sceneScoreHistory.Current(key,generation,consequence.score,
                        state.nativeCandidates[index].claim<fusionfix::shadows::NativeShadowContinuity42::Slots,
                        scenePriorityReleaseMs);
                    if(score38){score38->rawImportance=importance;score38->stableImportance=importance;
                        score38->subjectCoverage=consequence.subjectSupport;score38->roadCoverage=consequence.roadSupport;}
                }
                // Invalid authored geometry/photometry keeps the inherited
                // current-frame ranking; never invent a high-impact source.
            }
            if(kind==budget::Kind::Lamp&&labLampGeneration&&labLampGeneration(light)!=0)
                focusDistance=fusionfix::shadows::SignalShadowPriorityDistance20(focusDistance,
                    true,relevant,affectsPlayer,viewWeight,signalShadowPriority);
            // Smooth only currently authenticated non-recycled traffic lamp keys.
            // Player beams and approaching world lamps acquire priority immediately.
            if(relevant&&!state.keyMultiplicity.Ambiguous(index)&&kind==budget::Kind::OtherBeam&&
               beamSelection.privateOwner)
                focusDistance=state.trafficPriority.Observe(key,focusDistance,state.frame,
                    static_cast<uint32_t>(*CTimer::m_snTimeInMilliseconds));
            if(std::isfinite(focusDistance)&&focusDistance>=0) {
                state.nativeCandidates[index].focusDistance=focusDistance;
                // Scene mode uses continuous receiver importance, avoiding the
                // hard near-lamp tier jump that could abruptly change winners.
                // Native special/player priority remains in Compare().
                state.nativeCandidates[index].focusTier=scenePriorityStrength>0?(relevant?1u:0u):
                    fusionfix::shadows::ShadowRelevanceTier(relevant,kind==budget::Kind::Lamp&&affectsPlayer,volumeVisible);
            }
            if(kind==budget::Kind::PlayerBeam)
                state.nativeCandidates[index].stableOrder=beamSelection.order;
            // Record why a prior choice loses its claim, independently of
            // whether native sorting eventually drops it. Bounded to 7/pass.
            for(const auto& old:state.previousSelection) if(old.key==key) {
                const bool sameGeneration=old.generation==generation;
                ShadowTrace34::Emit({7,state.frame,GetTickCount(),key,relevant?1:0,sameGeneration?1:0,
                    static_cast<int>(flags | (volumeVisible?0x10000u:0)),static_cast<int>(state.nativeCandidates[index].claim),
                    viewWeight,geometry.distanceSquared,reach,light.mRadius});
                break;
            }
        }
        if(projectionClass&&projectionCollect66) {
            // Same comparator/prefix that native will consume. Only this owned
            // value class can defer its full current packet permission witness.
            GracePrefix108 gracePrefix;
            const bool gracePreview=state.continuityActive&&labCompareGrace&&
                CaptureGracePrefix108(regs.esp,index,lights,count,gracePrefix);
            bool previewAvailable=state.continuityActive&&(!labCompareGrace||gracePreview);
            bool wouldWrite=!previewAvailable||PreviewWritesBank66(regs.esp,index,lights,count);
            // A changed callback, native prefix/envelope or current key cannot
            // borrow an earlier ordinary-pair certificate to omit its witness.
            if(gracePreview&&!CurrentGracePrefix108(regs.esp,index,lights,count,gracePrefix)){
                previewAvailable=false;wouldWrite=true;
            }
            const auto gate=native_projection66::GateInsertion(true,previewAvailable,wouldWrite,[&]() noexcept {
                ++projectionWouldWrites66;
                return projectionCollect66(index,lights,count,state.frame,regs.esp);
            });
            if(gate==native_projection66::InsertionDecision::Skipped) {
                ++projectionZeroWrites66;regs.eip=gameBase+0x527DDC;return;
            }
            if(gate==native_projection66::InsertionDecision::Rejected) {
                state.nativeCandidates[index]={};labRelevant[index]=0;
                regs.eip=gameBase+0x527DDC;return;
            }
        }
        if(state.deferModels48 && state.pending48[index]) {
            if(lights!=state.shortlistLights48 || count!=state.shortlistCount48 || !state.continuityActive){FlushPending48();}
            else if(PreviewWrites48(regs.esp,index)) {
                state.pending48[index]=0;++shortlistValidated48;
                if(!validateModelSource48(lights[index])) {
                    ++shortlistInvalid48;state.nativeCandidates[index]={};labRelevant[index]=0;
                    // At Collect EDI/ESI still hold array/byte offset; this skip
                    // reaches the original loop continuation without cell writes.
                    regs.eip=gameBase+0x527DDC;return;
                }
            }
        }
        if(deferredCurrent48){
            if(!(lights[index].mFlags&4u)){
                state.nativeCandidates[index]={};labRelevant[index]=0;regs.eip=gameBase+0x527DDC;return;
            }
            state.pass.Observe({key,index,kind,geometry.distanceSquared,affectsPlayer,true,
                generation,viewWeight,reach*reach,priorityDistance},&light);
            if(labPointSource&&labPointSource(light)&&state.pass.Active())++labObserved;
        }
        if (!state.pass.Active()) {
            FlushPending48();
            if(deferredCurrent48 && !(lights[index].mFlags&4u))regs.eip=gameBase+0x527DDC;
            ++fallbackPasses; ++rejectedPassReasons[state.pass.FailureCode() & 7];
        }
    }

    static void CompareNativeCandidates(SafetyHookContext& regs) noexcept
    {
        const FloatingPointState fp;
        if(!Enabled() || state.depth!=1 || !state.pass.Active() || !state.continuityActive) return;
        // The eighth insertion cell is native scratch; only the first seven
        // feed the dynamic pass. Do not read its uninitialized flags/index.
        if(regs.esi>=7*4 || (regs.esi&3u)) return;
        if(!state.stackAnchor || regs.esp!=state.stackAnchor) {++continuityRejected;return;}
        const auto challenger=*reinterpret_cast<const int32_t*>(regs.esp+0x24);
        const auto incumbent=*reinterpret_cast<const int32_t*>(regs.esp+0x7C+regs.esi);
        const auto count=CurrentCount();
        if(challenger<0 || incumbent<0) return; // Native inserts into empty cells.
        if(count>state.nativeCandidates.size() || static_cast<uint32_t>(challenger)>=count ||
           static_cast<uint32_t>(incumbent)>=count) {++continuityRejected;return;}
        const auto& a=state.nativeCandidates[challenger];
        const auto& b=state.nativeCandidates[incumbent];
        if(!a.observed || !b.observed || a.flags!=regs.edi ||
           b.flags!=*reinterpret_cast<const uint32_t*>(regs.esp+0x9C+regs.esi)) {++continuityRejected;return;}
        ++continuityComparisons;
        const int original=static_cast<int>(regs.eax);
        if(labPointSource && labPointSource(CurrentLights()[challenger]) && original>=0 && original<3) ++labComparisons[original];
        int result=fusionfix::shadows::NativeShadowContinuity42::Compare(original,a,b);
        if(labCompareGrace)result=labCompareGrace(original,result,challenger,incumbent);
        if(labPointSource && labPointSource(CurrentLights()[challenger]) && result>=0 && result<3){++labResults[result];if(result==2 && original!=2){if(a.ownBeam!=b.ownBeam)++labOwnLoss;else if(a.claim>b.claim)++labClaimLoss;}}
        if(result!=original) {
            regs.eax=static_cast<uint32_t>(result); ++continuityOverrides;
            // Decisions only, not render completion. No allocations/file I/O.
            if(!state.tracedComparison) {
                const auto* lights=CurrentLights();
                ShadowTrace34::Emit({6,state.frame,GetTickCount(),static_cast<uint32_t>(lights[challenger].mCastShadows),
                    original,result,static_cast<int>(lights[incumbent].mCastShadows),static_cast<int>(regs.esi/4),
                    static_cast<float>(a.claim),static_cast<float>(b.claim),a.ownBeam?1.0f:0.0f,b.ownBeam?1.0f:0.0f});
                state.tracedComparison=true;
            }
        }
    }

#include "ShadowCacheDependenciesRuntime44.inl"

    static void StabilizeMapOrdinals(SafetyHookContext& regs) noexcept {
        const FloatingPointState fp;
        if(!Enabled() || state.depth!=1 || !state.pass.Active() || !state.continuityActive)return;
        const auto* lights=CurrentLights();const auto count=CurrentCount();
        if(!state.stackAnchor || regs.esp!=state.stackAnchor || !CShadows::pFrameCounter ||
            *CShadows::pFrameCounter!=state.frame || CPlayer::getLocalPlayerPed()!=state.ped ||
            CPlayer::findPlayerCar()!=state.occupiedCar || !state.pass.InputsUnchanged(lights,count)) {
            ++stableOrdinalRejected;return;
        }
        auto* indices=reinterpret_cast<int32_t*>(regs.esp+allocation::SelectedIndicesStackOffset);
        std::array<fusionfix::shadows::ShadowMapOrdinal20,7> chosen{};
        for(unsigned i=0;i<7;++i) {
            const auto index=indices[i];chosen[i].index=index;
            if(index==-1)continue;
            if(index<0 || uint32_t(index)>=count || !state.nativeCandidates[index].observed ||
                state.nativeCandidates[index].flags!=lights[index].mFlags) {++stableOrdinalRejected;return;}
            chosen[i].claim=state.nativeCandidates[index].claim;
            chosen[i].flags=lights[index].mFlags;
        }
        std::array<int32_t,7> output{};
        if(!fusionfix::shadows::RestoreShadowMapOrdinals20(chosen,output)){++stableOrdinalRejected;return;}
        ++stableOrdinalPasses;
        for(unsigned i=0;i<7;++i)if(output[i]!=indices[i])++stableOrdinalMoves;
        // Cache dependency protection and map construction consume this same
        // exact set. Never perform this permutation at the later Finalize hook.
        std::memcpy(indices,output.data(),sizeof(output));
    }

    static void TraceSelectionLoss38(const budget::PlayerShadowBudget::Selection& chosen,
        const budget::PlayerShadowBudget::Slot& old,unsigned oldSlot,uint32_t tick) noexcept {
        // Maximum seven loss bundles/pass; no game calls, validation or file I/O.
        if(!ShadowTrace34::enabled.load(std::memory_order_relaxed))return;
        const auto count=CurrentCount();const auto* lights=CurrentLights();
        if(!lights || count>state.nativeCandidates.size())return;
        int index=-1;unsigned matches=0;
        for(unsigned i=0;i<count;++i)if(static_cast<uint32_t>(lights[i].mCastShadows)==old.key){
            ++matches;if(index<0)index=int(i);
            if(state.nativeCandidates[i].observed && state.scoreDiagnostics38[i].frame==state.frame &&
               state.scoreDiagnostics38[i].generation==old.generation)index=int(i);
        }
        uint32_t facts=0;
        fusionfix::shadows::SelectionScoreDiagnostic38 missing{};
        const auto* score=&missing;
        const fusionfix::shadows::NativeShadowContinuity42::Candidate* candidate=nullptr;
        if(index>=0){
            const auto& light=lights[index];facts|=fusionfix::shadows::LossSourcePresent38;
            if(matches>1 || state.keyMultiplicity.Ambiguous(index))facts|=fusionfix::shadows::LossAmbiguous38;
            if(light.mFlags&4u)facts|=fusionfix::shadows::LossShadowEnabled38;
            if((light.mFlags&2u)&&light.mShadowCacheIndex<0)facts|=fusionfix::shadows::LossCacheUnavailable38;
            if(state.nativeCandidates[index].observed){
                candidate=&state.nativeCandidates[index];facts|=fusionfix::shadows::LossCollected38;
                if(labRelevant[index])facts|=fusionfix::shadows::LossRelevant38;
                const auto& observed=state.scoreDiagnostics38[index];
                if(observed.frame==state.frame){score=&observed;
                    if(observed.generation==old.generation)facts|=fusionfix::shadows::LossSameGeneration38;}
            }
        }
        const int reference=fusionfix::shadows::SelectionLossReference38(chosen,old,oldSlot);
        if(reference>=0)facts|=fusionfix::shadows::LossReferenceChosen38;
        ShadowTrace34::Emit({30,state.frame,tick,uint32_t(old.key),int(oldSlot),index,
            int(fusionfix::shadows::ClassifySelectionLoss38(facts)),int(facts),0,0,0,0});
        ShadowTrace34::Emit({31,state.frame,tick,uint32_t(old.key),index,0,0,0,
            score->rawFocus,score->viewWeight,score->rawImportance,score->stableImportance});
        const float unavailable=std::numeric_limits<float>::quiet_NaN();
        ShadowTrace34::Emit({32,state.frame,tick,uint32_t(old.key),index,candidate?int(candidate->focusTier):-1,
            candidate?int(candidate->claim):-1,state.occupiedCar?1:0,score->subjectCoverage,score->roadCoverage,
            candidate?candidate->focusDistance:unavailable,
            candidate?fusionfix::shadows::NativeShadowContinuity42::RetainedScore(*candidate):unavailable});
        ShadowTrace34::Emit({34,state.frame,tick,uint32_t(old.key),int(uint32_t(old.generation)),
            int(uint32_t(old.generation>>32)),int(state.previousSelectionFrame),int(count),0,0,0,0});
        ShadowTrace34::Emit({38,state.frame,tick,uint32_t(old.key),int(uint32_t(score->generation)),
            int(uint32_t(score->generation>>32)),index,score==&missing?0:1,0,0,0,0});
        if(index>=0){const auto& light=lights[index];
            ShadowTrace34::Emit({35,state.frame,tick,uint32_t(old.key),index,int(light.mType),int(light.mFlags),
                light.mShadowCacheIndex,light.mPosition.x,light.mPosition.y,light.mPosition.z,light.mRadius});
            ShadowTrace34::Emit({36,state.frame,tick,uint32_t(old.key),index,0,0,0,
                light.mDirection.x,light.mDirection.y,light.mDirection.z,light.mOuterConeAngle});
            ShadowTrace34::Emit({37,state.frame,tick,uint32_t(old.key),index,0,0,0,
                light.mTangent.x,light.mTangent.y,light.mTangent.z,light.mInnerConeAngle});
        }
        // This is the selected same-slot replacement (or selected reference).
        // It is not asserted to be the comparator that causally evicted old.
        if(reference>=0){const auto& slot=chosen.slots[reference];
            if(slot.index<count){const auto& winner=state.nativeCandidates[slot.index];
                const auto& measured=state.scoreDiagnostics38[slot.index];
                const auto* current=winner.observed && measured.frame==state.frame?&measured:&missing;
                ShadowTrace34::Emit({33,state.frame,tick,uint32_t(old.key),int(uint32_t(slot.key)),int(slot.index),
                    reference,int(lights[slot.index].mFlags),winner.observed?
                    fusionfix::shadows::NativeShadowContinuity42::RetainedScore(winner):unavailable,
                    current->rawFocus,current->viewWeight,current->stableImportance});
                ShadowTrace34::Emit({39,state.frame,tick,uint32_t(old.key),int(uint32_t(slot.generation)),
                    int(uint32_t(slot.generation>>32)),winner.observed?int(winner.focusTier):-1,
                    winner.observed?int(winner.claim):-1,current->subjectCoverage,current->roadCoverage,
                    winner.observed?winner.focusDistance:unavailable,
                    state.keyMultiplicity.Ambiguous(slot.index)?1.f:0.f});
            }
        }
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
            if(labPointSource && labPointSource(light)){if(stage==1)++labNativeChosen;else if(stage==2)++labFinalChosen;}
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
        if (state.pass.Commit(CurrentLights(), CurrentCount(), publicationEnabled ? indices : observation.data(), true,
                              state.continuityActive))
        {
            const auto& traced=state.pass.LastSelection();
            for(unsigned slot=0;slot<7;++slot)
                if(traced.validMask&(1u<<slot)) traceChoice(2,slot,static_cast<int>(traced.slots[slot].index));
            if (publicationEnabled) {
                ++appliedPasses; state.committed = true;
                const auto& selection=state.pass.LastSelection();
                if(state.ownEligibleCount) {
                    ++ownEligiblePasses;unsigned chosenOwn=0;
                    for(unsigned i=0;i<7;++i)
                        if((selection.validMask&(1u<<i)) && selection.slots[i].kind==budget::Kind::PlayerBeam)++chosenOwn;
                    if(chosenOwn<state.ownEligibleCount)++ownMissingSelectionPasses;
                }
                for(unsigned oldSlot=0;oldSlot<7;++oldSlot) {
                    const auto& old=state.previousSelection[oldSlot];if(!old.key)continue;
                    for(unsigned nowSlot=0;nowSlot<7;++nowSlot)
                        if((selection.validMask&(1u<<nowSlot)) && !state.keyMultiplicity.Ambiguous(selection.slots[nowSlot].index) && nowSlot!=oldSlot &&
                            selection.slots[nowSlot].key==old.key && selection.slots[nowSlot].generation==old.generation)
                            ++retainedMapMoves;
                }
                for(unsigned oldSlot=0;oldSlot<7;++oldSlot) {
                    const auto& old=state.previousSelection[oldSlot];
                    if(!old.key) continue;
                    bool retained=false;
                    for(const auto& now:selection.slots)
                        if(now.key==old.key && now.generation==old.generation) retained=true;
                    if(!retained) {
                        TraceSelectionLoss38(selection,old,oldSlot,traceTick);
                        if(old.kind!=budget::Kind::Lamp)continue;
                        ++lampRemoved;
                        ShadowTrace34::Emit({3,state.frame,traceTick,static_cast<uint32_t>(old.key),
                            state.pass.WasObserved(old.key,old.generation)?1:0,0,0,0,0,0,0,0});
                        if(state.pass.WasObserved(old.key,old.generation)) ++lampDroppedPresent;
                        else ++lampMissingInput;
                    }
                }
                if(transitionDetectorEnabled69){
                    std::array<shadow_transition69::Identity,7> current{};
                    for(unsigned i=0;i<7;++i)if((selection.validMask&(1u<<i))&&!state.keyMultiplicity.Ambiguous(selection.slots[i].index))
                        current[i]={selection.slots[i].key,selection.slots[i].generation};
                    state.transitionDetector69.Observe(state.ped,state.occupiedCar,state.frame,traceTick,current,
                        [&](shadow_transition69::Identity id) noexcept {return state.pass.WasObserved(id.key,id.generation);},
                        [](const shadow_transition69::Event& event) noexcept {(void)transitionEvents69.Push(event);});
                }
                state.previousSelection=selection.slots;
                // Ambiguous native keys retain their CURRENT light/selection,
                // never identity history. Keep the same comparison rule for all
                // candidates, unlike mixed native/custom ordering in build26.
                for(auto& slot:state.previousSelection)
                    if(slot.key && state.keyMultiplicity.Ambiguous(slot.index))slot={};
                state.previousSelectionFrame=state.frame;
                std::array<fusionfix::shadows::NativeShadowContinuity42::Identity,7> identities{};
                for(unsigned i=0;i<7;++i) if((selection.validMask&(1u<<i)) && !state.keyMultiplicity.Ambiguous(selection.slots[i].index))
                    identities[i]={static_cast<uint32_t>(selection.slots[i].key),selection.slots[i].generation};
                state.continuity.Commit(identities);
                for(const auto& slot:selection.slots) if(slot.key && slot.kind==budget::Kind::Lamp && !state.keyMultiplicity.Ambiguous(slot.index))
                    state.lampContinuity.Selected(slot.key,slot.generation);
            }
            else ++observedPasses;
        }
        else { ++fallbackPasses; ++rejectedPassReasons[state.pass.FailureCode() & 7]; }
    }

    static void ApplyLiveReachAtSelection122(DWORD currentThread) noexcept {
        if(state.depth==1&&Enabled()&&!unsupportedThread.load(std::memory_order_relaxed)&&
            ownerThread.load(std::memory_order_relaxed)==currentThread&&liveReachBoundary122)liveReachBoundary122();
    }
    static void __cdecl Select()
    {
        const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::Selection,16);
        const Invocation invocation;
        {
            const FloatingPointState fp;
            DWORD expected = 0;
            const DWORD current = GetCurrentThreadId();
            ownerThread.compare_exchange_strong(expected, current, std::memory_order_relaxed);
            if (ownerThread.load(std::memory_order_relaxed) != current)
                unsupportedThread.store(true, std::memory_order_relaxed);
            ApplyLiveReachAtSelection122(current);
            if(state.depth==1)BeginShortlist48();
            if(state.depth==1 && Enabled() && prepareSources)prepareSources(CurrentLights(),CurrentCount());
            if (state.depth != 1 || !Prepare()) RejectPass();
            else if(observeSubmittedLights && !unsupportedThread.load(std::memory_order_relaxed)) observeSubmittedLights(CurrentLights(),CurrentCount());
        }
        // Hooks are immutable once published. The unsafe call avoids the
        // hook-object mutex around native execution (including recursion).
        if(state.deferModels48 && (CurrentLights()!=state.shortlistLights48 || CurrentCount()!=state.shortlistCount48)){
            state.deferModels48=false;state.pending48.fill(0);++shortlistFallback48;
            if(bulkModelSources48)bulkModelSources48(CurrentLights(),CurrentCount());
            RejectPass();
        }
        if(state.deferModels48 && (!state.continuityActive || !shortlistGetterSafe48()))FlushPending48();
        selectionHook.unsafe_ccall<void>();
        if(observeShadowResult && state.depth==1)observeShadowResult();
        if(observeSelected198C && state.depth==1)observeSelected198C();
        // Native copying marks holes inactive but leaves their previous keys.
        // Clear only those dead identities before native frame-buffer copying.
        // This prevents lighting lookup from finding a stale, unrendered map.
        if(state.depth==1&&state.pass.Active()&&state.parallelPhases61&&parallelRequested61&&!unsupportedThread.load(std::memory_order_relaxed)) {
            // Submit copied numeric hints after native selection, never wait.
            if(parallelService61.load(std::memory_order_acquire))(void)parallelService61.load(std::memory_order_acquire)->Dispatch();
        }
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
        auto compare = safetyhook::MidHook::create(gameBase + allocation::CompareResultRva,
            CompareNativeCandidates, safetyhook::MidHook::StartDisabled);
        auto cacheResult = safetyhook::MidHook::create(gameBase + allocation::CacheResultRva,
            ProtectCacheDependencies, safetyhook::MidHook::StartDisabled);
        auto stable = safetyhook::MidHook::create(gameBase + allocation::StableOrdinalsRva,
            StabilizeMapOrdinals, safetyhook::MidHook::StartDisabled);
        if (!begin || !collect || !finish || !lampDistance || !compare || !cacheResult || !stable)
        {
            installStatus = std::string("hook_creation_failed begin=") + (begin ? "1" : "0") +
                " collect=" + (collect ? "1" : "0") + " finish=" + (finish ? "1" : "0") + " compare=" + (compare ? "1" : "0") + " cache=" + (cacheResult ? "1" : "0");
            return false;
        }
        selectionHook = std::move(*begin);
        collectHook = std::move(*collect);
        finalizeHook = std::move(*finish);
        lampDistanceHook = std::move(*lampDistance);
        compareResultHook = std::move(*compare);
        cacheResultHook = std::move(*cacheResult);
        stableOrdinalsHook = std::move(*stable);
        if (!stableOrdinalsHook.enable() || !cacheResultHook.enable() || !compareResultHook.enable() || !lampDistanceHook.enable() || !collectHook.enable() || !finalizeHook.enable() || !selectionHook.enable())
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






