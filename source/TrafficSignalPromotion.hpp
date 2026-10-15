#pragma once
#include "TrafficSignalIdentity.hpp"
#include <cmath>
namespace traffic_signal {
// The proven physical signal, rather than its wind-moving bulb coordinates.
inline bool ProvenTarget(uint32_t kind,uint32_t model,const float* entity) noexcept {
 constexpr float target[]{116.875f,833.670f,13.755f};
 if(kind!=1||model!=0x4c60ad62||!entity)return false;
 for(unsigned i=0;i<3;++i)if(!std::isfinite(entity[i])||std::abs(entity[i]-target[i])>0.05f)return false;
 return true;
}
// No key recycling, lease extension, renderer-pool changes or synthetic lights.
// Only the bound producer's original flags and opaque key can be modified.
class Promotion {
 uint32_t key_{},lastFrame_{};bool submitted_{},retired_{};
public:
 bool Configure(uint32_t reservedKey,uint32_t otherPrivateKey) noexcept {
  if(key_||retired_||reservedKey<0x10000||reservedKey==UINT32_MAX||
     (reservedKey&63)||reservedKey==otherPrivateKey){retired_=true;return false;}
  key_=reservedKey;return true;
 }
 uint32_t Key() const noexcept{return key_;}
 bool Retired() const noexcept{return retired_;}
 bool SubmittedIn(uint32_t frame) const noexcept{return submitted_&&lastFrame_==frame;}
 static constexpr uint64_t Generation=1; // Unique never-reused key owns one lifetime.
 bool Apply(const Lifetime& life,Identity current,uint32_t frame,uint32_t caller,
  uint32_t type,uint32_t phase,bool sameThread,bool externalRetired,bool requested,
  bool collision,uint32_t& flags,uint32_t& nativeKey) noexcept {
  if(retired_||!key_)return false;
  if(!requested){retired_=true;return false;}
  if(!sameThread||externalRetired||life.retired){retired_=true;return false;}
  if(!life.bound)return false;
  if(!(current==life.identity)||uint32_t(frame-life.lastFrame)>2){retired_=true;return false;}
  if(caller!=0x921029||type!=2||phase>2||flags!=0x200||nativeKey!=0||life.phaseMask!=7)return false;
  if(SubmittedIn(frame))return false;
  if(collision){retired_=true;return false;}
  flags|=4;nativeKey=key_;lastFrame_=frame;submitted_=true;return true;
 }
 static uint64_t LampGeneration(bool privateSource,uint64_t natural) noexcept {
  return privateSource?Generation:natural;
 }
};
}
