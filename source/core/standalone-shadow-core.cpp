// Standalone extraction of the CE44 policy/runtime, not a FusionFix module.
// No loader entry or installation occurs in this compilation unit yet.
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "ShadowCpuProfile29.hpp"
#include <safetyhook.hpp>
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>
#include <intrin.h>
#include "standalone-game-abi.hpp"

#include "StableHeadlightSelector.hpp"
#include "ShadowAdapterCE.hpp"
#include "ShadowAllocationCE.hpp"
#include "ShadowAllocationPass.hpp"
#include "ShadowFloatingPointState.hpp"
#include "ShadowLightGeometry.hpp"
#include "HeadlightCasterPolicy.hpp"
#include "ShadowCasterCE.hpp"
#include "ShadowGuardDiagnostics.hpp"
#include "CloseHeadlightRelevance.hpp"
#include "ShadowReach.hpp"
#include "ShadowReceiver.hpp"
#include "NearbyVehicleReceivers36.hpp"
#include "NativeLampDistance38.hpp"
#include "NativeLampContinuity41.hpp"
#include "NativeShadowContinuity42.hpp"
#include "ShadowVolumeVisibility43.hpp"
#include "NativeCacheDependencies44.hpp"
#include "ShadowInactiveSlots.hpp"
#include "ShadowViewPriority.hpp"
#include "ShadowDrivingFocus.hpp"
#include "ShadowLookupValidation.hpp"
#include "ShadowTrace34.hpp"
#include "SubmittedHeadlightHistory.hpp"
#include "ShadowLookupLayout.hpp"
#include "ShadowCrashTrace30.hpp"
#include <fstream>
#include <atomic>
#include <intrin.h>



bool bHighResolutionNightShadows = false;
static bool bCloseHeadlightRelevance = false;
static bool bTrafficSelfShadowFix = false;
#include "HeadlightEnhancementRuntime.inl"

namespace NearbyVehicleLighting36 {
    static void Update() noexcept;
    static bool Relevant(uintptr_t player,uint32_t frame,uint32_t now,
        fusionfix::shadows::Vec3 position,fusionfix::shadows::Vec3 source,
        fusionfix::shadows::Vec3 direction,float outerCos,float radius,uintptr_t key) noexcept;
    static float Score(uintptr_t player,uint32_t frame,uint32_t now,
        fusionfix::shadows::Vec3 position,fusionfix::shadows::Vec3 source,
        fusionfix::shadows::Vec3 direction,float outerCos,float radius,uintptr_t key) noexcept;
}

namespace CShadows
{
    // CE's submission adapter takes 16 stack words. Its final word is an opaque
    // stable key (including pointer+1 values), not a dereferenceable light object.
    // Official FusionFix preserved it through a tail jump. Our nontrivial
    // wrappers must explicitly forward it; no upstream flicker cause is claimed.
    using SubmitHeadlight = void(__cdecl*)(int, int, uint32_t, int, int, int, int, int,
                                          int, int, int, int, int, int, int, int);
    injector::hook_back<SubmitHeadlight> hbStoreStaticShadow;
    static uint32_t* pFrameCounter = nullptr;
    static std::atomic<uint32_t> drivingBeamAccepted{0}, drivingBeamRejected{0};

    struct StableHeadlightShadow
    {
        std::mutex stateMutex;
        fusionfix::shadows::StableHeadlightSelector selector;
        fusionfix::shadows::SubmittedHeadlightHistory submitted;
        fusionfix::shadows::Vec3 playerPosition{};
        uintptr_t occupiedVehicle = 0, lastVehicle = 0, playerSession = 0;
        uint32_t frame = 0;
        bool hasFrame = false;
        bool playerValid = false;

        bool PrepareFrame()
        {
            if (!bHeadlightShadows || !pFrameCounter || !CTimer::m_snTimeInMilliseconds ||
                !CPlayer::getLocalPlayerPed || !CPlayer::findPlayerCar)
            {
                selector.Reset();
                submitted.Reset();
                hasFrame = false;
                return false;
            }

            // Use the game's verified frame counter. A timer tick does not
            // identify a frame when time is paused, slowed or reset.
            const uint32_t nextFrame = *pFrameCounter;
            if (hasFrame && nextFrame == frame)
                return playerValid;
            frame = nextFrame;
            hasFrame = true;
            playerValid = false;

            const uintptr_t ped = CPlayer::getLocalPlayerPed();
            if (!ped)
            {
                selector.Reset();
                submitted.Reset();
                return false;
            }
            const auto matrix = *reinterpret_cast<const float* const*>(ped + 0x20);
            if (!matrix)
            {
                selector.Reset();
                submitted.Reset();
                return false;
            }
            playerPosition = {matrix[12], matrix[13], matrix[14]};
            if (!std::isfinite(playerPosition.x) || !std::isfinite(playerPosition.y) ||
                !std::isfinite(playerPosition.z))
            {
                selector.Reset();
                submitted.Reset();
                return false;
            }
            const uintptr_t car = CPlayer::findPlayerCar();
            if (ped != playerSession) { lastVehicle = 0; submitted.Reset(); }
            playerSession = ped;
            occupiedVehicle = car;
            if (car) {
                lastVehicle = car;
                const auto carMatrix = *reinterpret_cast<const float* const*>(car + 0x20);
                if (carMatrix && std::isfinite(carMatrix[12]) && std::isfinite(carMatrix[13]) && std::isfinite(carMatrix[14]))
                    playerPosition = {carMatrix[12], carMatrix[13], carMatrix[14]};
            }
            selector.BeginFrame({nextFrame, static_cast<uint32_t>(*CTimer::m_snTimeInMilliseconds),
                                 ped, car != 0, car});
            playerValid = true;
            return true;
        }

        bool IsSubmittedBeam(uintptr_t key)
        {
            const std::lock_guard<std::mutex> lock(stateMutex);
            if (!key || !hasFrame || !playerValid) return false;
            return pFrameCounter && submitted.Contains(key, *pFrameCounter);
        }

        bool ShouldCast(int directionAddress, int positionAddress, int stableKey, int radiusBits)
        {
            // Keep policy bookkeeping coherent if render submissions are made
            // on more than one thread. The engine call occurs after unlock.
            const std::lock_guard<std::mutex> lock(stateMutex);
            if (!PrepareFrame() || !positionAddress || !stableKey)
                return false;
            // Audited ABCC50/ABCCD0: argument 4 is direction, argument 6 position.
            // Copy values now; these vectors may live on the caller's stack.
            const auto position = reinterpret_cast<const float*>(positionAddress);
            const fusionfix::shadows::Vec3 lightPosition{position[0], position[1], position[2]};
            fusionfix::shadows::Vec3 forward{};
            if (directionAddress)
            {
                const auto direction = reinterpret_cast<const float*>(directionAddress);
                forward = {direction[0], direction[1], direction[2]};
            }
            auto geometry = fusionfix::shadows::EvaluateGeometry(
                playerPosition, lightPosition, directionAddress ? &forward : nullptr);
            float sourceRadius;
            std::memcpy(&sourceRadius, &radiusBits, sizeof(sourceRadius));
            const float receiverExtent = occupiedVehicle ? 3.0f : 1.5f;
            geometry.distanceSquared = fusionfix::shadows::ReceiverDistanceSquared(playerPosition, lightPosition, receiverExtent);
            // Broad submission gate; the allocator checks the native cone.
            geometry.aimedAtPlayer = directionAddress && fusionfix::shadows::BeamTouchesReceiver(
                playerPosition, receiverExtent, lightPosition, forward, 0.70710678f, sourceRadius);
            float receiverWeight=1.0f;
            if (!occupiedVehicle) {
                if(geometry.aimedAtPlayer) receiverWeight=1.5f;
                else if(directionAddress) {
                    const float benefit=NearbyVehicleLighting36::Score(playerSession,frame,
                        static_cast<uint32_t>(*CTimer::m_snTimeInMilliseconds),playerPosition,lightPosition,
                        forward,0.70710678f,sourceRadius,static_cast<uintptr_t>(static_cast<uint32_t>(stableKey)));
                    geometry.aimedAtPlayer=benefit>0;
                    receiverWeight=1.0f+0.5f*benefit;
                }
            }
            const auto identity = static_cast<uintptr_t>(static_cast<uint32_t>(stableKey));
            const bool playerHeadlight = fusionfix::shadows::ce::IsVehicleBeam(identity,
                occupiedVehicle ? occupiedVehicle : lastVehicle);
            // NPC beams must reach the receiver; source-to-ped distance is not a ten-foot gate.
            // The current/recent car may light scenery beyond Niko instead.
            const int feet = fusionfix::shadows::ShadowReachFeet(FusionFixSettings.Get("PREF_HEADLIGHT_REACH"));
            const float configuredReach = feet > 0 ? feet * 0.3048f : 35.0f;
            const float reach = !playerHeadlight && std::isfinite(sourceRadius) && sourceRadius > 0
                ? (std::max)(configuredReach, sourceRadius) : configuredReach;
            const bool accepted = selector.Consider({identity, geometry, playerHeadlight,
                reach * reach, !playerHeadlight, receiverWeight});
            if (accepted) submitted.Record(identity, frame);
            if (playerHeadlight)
            {
                if (accepted) ++drivingBeamAccepted;
                else ++drivingBeamRejected;
            }
            return accepted;
        }
    };
    static StableHeadlightShadow gStableHeadlightShadow;

    void __cdecl StoreStaticShadowPlayerDriving(int a1, int a2, uint32_t flags,
        int direction, int tangent, int position, int a7, int a8, int a9, int a10,
        int a11, int a12, int a13, int a14, int a15, int stableKey)
    {
        a9 = HeadlightEnhancement::SelectMask(a9, stableKey);
        if (!gStableHeadlightShadow.ShouldCast(direction, position, stableKey, a11))
            flags &= ~4u;
        hbStoreStaticShadow.fun(a1, a2, flags, direction, tangent, position,
                                a7, a8, a9, a10, a11, a12, a13, a14, a15, stableKey);
    }

    void __cdecl StoreStaticShadowNPC(int a1, int a2, uint32_t flags,
        int direction, int tangent, int position, int a7, int a8, int a9, int a10,
        int a11, int a12, int a13, int a14, int a15, int stableKey)
    {
        a9 = HeadlightEnhancement::SelectMask(a9, stableKey);
        if (!gStableHeadlightShadow.ShouldCast(direction, position, stableKey, a11))
            flags &= ~4u;
        hbStoreStaticShadow.fun(a1, a2, flags, direction, tangent, position,
                                a7, a8, a9, a10, a11, a12, a13, a14, a15, stableKey);
    }

    static bool ValidateAdapter()
    {
        const auto image = reinterpret_cast<const uint8_t*>(GetModuleHandleW(nullptr));
        const auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image);
        if (!image || dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < 0 ||
            dos->e_lfanew > 0x100000)
            return false;
        const auto nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(image + dos->e_lfanew);
        if (!fusionfix::shadows::ce::ValidateMappedImage(
                image, nt->OptionalHeader.SizeOfImage, reinterpret_cast<uintptr_t>(image)))
            return false;
        pFrameCounter = reinterpret_cast<uint32_t*>(
            reinterpret_cast<uintptr_t>(image) + fusionfix::shadows::ce::FrameCounterRva);
        hbStoreStaticShadow.fun = reinterpret_cast<SubmitHeadlight>(
            reinterpret_cast<uintptr_t>(image) + fusionfix::shadows::ce::SubmitRva);
        return true;
    }
}

#include "ShadowAllocationRuntime.inl"
#include "NearbyVehicleRuntime36.inl"
#include "ShadowLookupRuntime.inl"
#include "ShadowCasterRuntime.inl"


static inline SafetyHookInline shsub_925DB0{};
static inline SafetyHookInline shsub_D77A00{};

// Lamppost shadows workaround 1
static int __cdecl sub_925DB0(int a1, int a2, int flags)
{


    const int buffer = ShadowLookupGuard::ReadBuffer();
    const int result = shsub_925DB0.ccall<int>(a1, a2, flags);
    return ShadowLookupGuard::Filter(result, static_cast<uint32_t>(a1), a2, buffer);
}

// Lamppost shadows workaround 2
static void __fastcall sub_D77A00(void* _this, void* edx)
{


    const fusionfix::shadows::caster::Scope scope(OwnHeadlightCaster::context,
                                                OwnHeadlightCaster::Capture(_this));
    return shsub_D77A00.unsafe_fastcall(_this, edx);
}
