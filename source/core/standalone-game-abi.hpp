#pragma once
// Minimal CE 1.2.0.59 game layout, independent of FusionFix's internal objects.
// Offsets derive from the previously audited CE44 integration. No CSettings ABI.
#include <cstdint>
#include <cstddef>
#include <atomic>
#include <string_view>
namespace CPlayer { inline uintptr_t (*getLocalPlayerPed)(){}; inline uintptr_t (*findPlayerCar)(){}; }
namespace CTimer { inline int32_t* m_snTimeInMilliseconds{}; }
namespace rage {
struct Vector3 { float x,y,z; };
struct Vector4 { float x,y,z,w; };
enum eLightType : uint32_t { LT_POINT,LT_DIR,LT_SPOT,LT_AMBOCC,LT_CLAMPED };
struct CLightSource {
 Vector3 mDirection;float field_C;Vector3 mTangent;float field_1C;
 Vector3 mPosition;float field_2C;Vector4 mColor;float mIntensity;
 eLightType mType;uint32_t mFlags;int32_t mCoronaTexHash,mProjTexHash;
 float mRadius,mInnerConeAngle,mOuterConeAngle;
 int32_t mCastShadows,mShadowCacheIndex,mInteriorIndex,mRoomIndex;
 float mVolumeIntensity,mVolumeScale;uint8_t padding[8];
};
static_assert(sizeof(CLightSource)==0x80);
struct grcViewport {
 uint8_t initial[0x180];float mViewMatrix[4][4];float mProjectionMatrix[4][4];
 uint8_t middle[0xb0];int32_t mWidth,mHeight;float mFov,mAspect,mNearClip,mFarClip;
 uint8_t tail[0x28];bool mIsPerspective;uint8_t remainder[0xff];
};
static_assert(sizeof(grcViewport)==0x3f0);
static_assert(offsetof(grcViewport,mIsPerspective)==0x2f0);
inline grcViewport** pCurrentViewport{};
namespace grcDevice {inline int32_t *ms_nActiveWidth{},*ms_nActiveHeight{};}
struct VehiclePool {
 uint8_t *m_aStorage,*m_aFlags;int32_t m_nSize,m_nStorageSize,m_nFirstFreeIndex,m_numSlotsUsed;
 bool GetIsFree(int32_t i) const noexcept {return (m_aFlags[i]&0x80)!=0;}
 uint8_t GetReference(int32_t i) const noexcept{return m_aFlags[i]&0x7f;}
 void* GetSlot(int32_t i) const noexcept{return i>=0&&i<m_nSize&&!GetIsFree(i)?m_aStorage+size_t(i)*m_nStorageSize:nullptr;}
};
}
namespace CVehicle {inline rage::VehiclePool** pool{};inline auto GetVehiclePool(){return pool?*pool:nullptr;}}
// This is the add-on's own configuration facade, NOT FusionFix's CSettings.
#include "ReachSettings122.hpp"
inline AddonSettings FusionFixSettings;
inline std::atomic<bool> addonActive{false};
struct LiveFlag { const volatile bool* value{}; const bool* configured{}; operator bool() const noexcept {return (!configured||*configured)&&value&&*value;} };
inline LiveFlag bExtraNightShadows,bHeadlightShadows,bVehicleNightShadows;
namespace injector {template<class T> struct hook_back{T fun{};};}

