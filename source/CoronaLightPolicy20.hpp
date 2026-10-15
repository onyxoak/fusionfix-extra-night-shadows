#pragma once
// OnyxOak / Liberty's Shadows private 2.0 candidate. GPL-3.0.
// Reconstruct a real point emitter from a currently active authored glow.
// A corona's draw distance is never interpreted as physical attenuation.
#include <array>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <algorithm>
namespace corona_light20 {
constexpr uint32_t ActiveSite=0x81eb27,OffSite=0x81e7fe,NativeTarget=0x81f020;
struct Vec3 {float x{},y{},z{};};
struct Settings {
 bool allowApproximation=true;
 float radiusPerAuthoredSize=12.f,fallbackIntensity=1.f,intensityScale=1.f;
 float minRadius=.1f,maxRadius=128.f,maxIntensity=64.f;
};
struct Packet {
 std::array<uint32_t,15> corona{};
 std::array<uint32_t,13> dispatch{};
 std::array<uint8_t,0x6c> descriptor{};
 Vec3 position{},matrixPosition{};
};
struct Emission {Vec3 position{},color{};float radius{},intensity{};bool approximateRadius{},approximateIntensity{};};
inline float Scalar(uint32_t bits) noexcept {float value;std::memcpy(&value,&bits,4);return value;}
inline float Authored(const Packet& p,unsigned offset) noexcept {float value;std::memcpy(&value,p.descriptor.data()+offset,4);return value;}
inline bool Finite(Vec3 p) noexcept {return std::isfinite(p.x)&&std::isfinite(p.y)&&std::isfinite(p.z);}
inline bool Valid(const Settings& s) noexcept {
 for(float v:{s.radiusPerAuthoredSize,s.fallbackIntensity,s.intensityScale,s.minRadius,s.maxRadius,s.maxIntensity})
  if(!std::isfinite(v)||v<=0.f)return false;
 return s.minRadius<=s.maxRadius&&s.maxRadius<=4096.f&&s.maxIntensity<=1024.f&&s.radiusPerAuthoredSize<=1024.f;
}
inline bool Reconstruct(uint32_t site,uint32_t nativeKey,const Settings& s,const Packet& p,Emission& result) noexcept {
 result={};
 // Exact active path only; never synthesize from a hidden/off submission, a
 // different producer, a native point/spot descriptor, or missing authority.
 if(site!=ActiveSite||!nativeKey||!Valid(s)||p.descriptor[0x5b]!=0||
    p.corona[0]!=nativeKey||p.dispatch[6]!=nativeKey||p.corona[13]||p.corona[14]!=1||
    !Finite(p.position)||!Finite(p.matrixPosition)||p.position.x!=p.matrixPosition.x||
    p.position.y!=p.matrixPosition.y||p.position.z!=p.matrixPosition.z)return false;
 for(unsigned i=1;i<=3;++i)if(p.corona[i]>255)return false;
 if(!(p.corona[1]|p.corona[2]|p.corona[3])||!std::isfinite(Scalar(p.corona[4]))||Scalar(p.corona[4])<=0.f)return false;
 const auto size=Authored(p,0x5c),authoredRadius=Authored(p,0x38),authoredIntensity=Authored(p,0x3c),brightness=Scalar(p.dispatch[2]);
 if(!std::isfinite(size)||size<=0.f||!std::isfinite(authoredRadius)||authoredRadius<0.f||
    !std::isfinite(authoredIntensity)||authoredIntensity<0.f||!std::isfinite(brightness)||brightness<=0.f)return false;
 const bool radiusMissing=authoredRadius==0.f,intensityMissing=authoredIntensity==0.f;
 if(!s.allowApproximation&&(radiusMissing||intensityMissing))return false;
 const auto radius=radiusMissing?size*s.radiusPerAuthoredSize:authoredRadius;
 const auto intensity=(intensityMissing?s.fallbackIntensity:authoredIntensity)*brightness*s.intensityScale;
 if(!std::isfinite(radius)||!std::isfinite(intensity)||radius<=0.f||intensity<=0.f)return false;
 result={p.position,{float(p.corona[1])/255.f,float(p.corona[2])/255.f,float(p.corona[3])/255.f},
  radiusMissing?std::clamp(radius,s.minRadius,s.maxRadius):radius,
  intensityMissing?std::min(intensity,s.maxIntensity):intensity,radiusMissing,intensityMissing};
 return true;
}
}
