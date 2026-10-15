// OnyxOak modification project: Extra Night Shadows Fix and Better Headlights.
// Project direction, integration and visual testing by OnyxOak; Codex-assisted development.
// Modification notice: 2026-09-27. See ATTRIBUTION.md for upstream credits and GPL-3.0.
// Official release: https://www.nexusmods.com/gta4/mods/1459

// Candidate 14: narrowly scoped immediate-render experiment. AE3310 may queue
// AE0690 instead: that path does not visit the entity hook and is NOT fixed here.
namespace Baseline143
{
    namespace policy = fusionfix::shadows::caster;
    namespace guard = fusionfix::shadows::ce::casterguard;
    static thread_local policy::Context context;
    static std::atomic<bool> enabled{false};
    static bool diagnosticCounters=false; // Startup-only; does not control exclusion policy.
    static uintptr_t base = 0; static void (*labRenderObserver)(uint32_t,uint32_t,uint32_t,bool) noexcept=nullptr;
    static std::atomic<uint32_t> passes{0}, deferredPasses{0}, ownPasses{0};
    static std::atomic<uint32_t> casterVisits{0}, carExcluded{0}, occupantsExcluded{0};
    static std::atomic<uint32_t> trafficCarExcluded{0};
    static uintptr_t (*additionalCasterOwner)(uintptr_t) noexcept=nullptr;

    static policy::Context Capture(void* renderPass) noexcept
    {
        policy::Context result{};
        if (!enabled.load(std::memory_order_acquire) || !renderPass ||
            ((!bHeadlightShadows || !bVehicleNightShadows) && !additionalCasterOwner))
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
        ShadowTrace34::Emit({5,CShadows::pFrameCounter?*CShadows::pFrameCounter:0,GetTickCount(),key,
            static_cast<int>(slot),static_cast<int>(kind),active?1:0,0,0,0,0,0});
        if(labRenderObserver)labRenderObserver(key,slot,kind,active);
        const bool headlights=bHeadlightShadows && bVehicleNightShadows;
        const auto car = CPlayer::findPlayerCar?CPlayer::findPlayerCar():0;
        // The inherited 64-key submission history is smaller than a busy
        // scene's two-per-car key set. Private lamp authority comes from the
        // fresh pool/model lifetime registry, not that lossy admission cache.
        if (headlights && bTrafficSelfShadowFix && kind == 4 && active &&
            fusionfix::shadows::ce::AdditionalVehicleBeamOwner)
            result.trafficBeamOwner = fusionfix::shadows::ce::AdditionalVehicleBeamOwner(key);
        if (bTrafficSelfShadowFix && kind == 4 && active && !result.trafficBeamOwner && additionalCasterOwner)
            result.trafficBeamOwner = additionalCasterOwner(key);
        // Exclude only the source vehicle from its own immediate
        // headlight pass. Confirm the opaque key was recently accepted by our submission
        // adapter; do not infer an owner pointer or dereference that key.
        if (headlights && bTrafficSelfShadowFix && kind == 4 && active &&
            CShadows::gStableHeadlightShadow.IsSubmittedBeam(key))
            result.trafficBeamKey = key;
        if (!headlights || !policy::OwnBeam(slot, kind, active, key, car)) return result;
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
        experiment_perf::Count(diagnosticCounters,casterVisits);
        if (((!bHeadlightShadows || !bVehicleNightShadows) && !context.trafficBeamOwner) ||
            !policy::Exclude(context, entity, type, artificial)) return false;
        if (type == 2) { experiment_perf::Count(diagnosticCounters,carExcluded); if (context.trafficBeamKey) experiment_perf::Count(diagnosticCounters,trafficCarExcluded); }
        else experiment_perf::Count(diagnosticCounters,occupantsExcluded);
        return true;
    }
}

