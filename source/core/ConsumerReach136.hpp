#pragma once
#include "ConsumerShadow75.hpp"
#include "ConsumerTransition198.hpp"
namespace consumer_reach136 {
inline constexpr float StaticShoulder162=1.30f;
// Refusal-only prefilters142. Full current authorization still follows whenever
// a native gate decision could change; these cannot authorize a shadow.
inline bool NumericPotential142(float threshold,float ratio,float distanceScale=StaticShoulder162) noexcept {
 return std::isfinite(threshold)&&threshold>0&&std::isfinite(ratio)&&ratio>0&&
    std::isfinite(distanceScale)&&distanceScale>=StaticShoulder162&&distanceScale<=ApplicationHistory198::ReleaseScale&&
    ratio<threshold&&ratio>=threshold/distanceScale;
}
inline bool PacketPotential142(const consumer_shadow75::Packet& p) noexcept {
 if(!p.words[24]||p.words[24]==UINT32_MAX||(p.words[18]&0x500u)||
    (p.words[17]!=0&&p.words[17]!=2))return false;
 const auto radius=consumer_shadow75::Float(p.words[21]);
 const auto intensity=consumer_shadow75::Float(p.words[16]);
 if(!std::isfinite(radius)||radius<=0||!std::isfinite(intensity)||intensity<=0)return false;
 for(unsigned i=12;i<15;++i)if(!std::isfinite(consumer_shadow75::Float(p.words[i])))return false;
 return true; // Zero brightness/ownership/sidecar are still checked by Authorize.
}
// Extend only the existing selected-map application gate. This cannot allocate
// maps, widen emitted light, or authorize a missing/stale source.
inline bool Shoulder(const consumer_shadow75::Sample& s,float distanceScale=StaticShoulder162) noexcept {
 if(!s.available||s.sizeGate||!s.finite||!s.reproduced||s.flags&0x500u||
    (s.type!=0&&s.type!=2)||!std::isfinite(s.gateRadius)||s.gateRadius<=0||
    !std::isfinite(s.distance)||s.distance<=0||!std::isfinite(s.threshold)||s.threshold<=0||
    !std::isfinite(s.ratio)||s.ratio<=0)return false;
 // At most thirty percent beyond the existing apparent-size distance; no global setting write.
 return std::isfinite(distanceScale)&&distanceScale>=StaticShoulder162&&distanceScale<=ApplicationHistory198::ReleaseScale&&s.ratio>=s.threshold/distanceScale;
}
template<class Read,class Authorize,class Write>
bool Gate(bool enabled,consumer_shadow75::Context& c,const consumer_shadow75::Registers& r,
 uint32_t frame,uint32_t tick,uintptr_t globalScale,Read&& read,Authorize&& authorize,Write&& write,
 ApplicationHistory198* history=nullptr) noexcept {
 if(!enabled){c={};return false;}
 bool unused=false;
 if(!consumer_shadow75::Probe(c,consumer_shadow75::Stage::Gate,r,frame,tick,globalScale,read,
    [&](const consumer_shadow75::Sample&) noexcept {unused=true;})||unused)return false;
 const auto packet=c.packet;
 const float distanceScale=!Shoulder(c.sample)&&history&&history->Retained(packet,frame,tick)?ApplicationHistory198::ReleaseScale:StaticShoulder162;
 if(!Shoulder(c.sample,distanceScale))return false;
 if(!authorize(packet,frame))return false;
 // Re-read the complete current invocation AFTER source authorization, so a
// nested call or changing packet cannot acquire another light's gate decision.
 consumer_shadow75::Context fresh=c;fresh.gateSeen=false;
 if(!consumer_shadow75::Probe(fresh,consumer_shadow75::Stage::Gate,r,frame,tick,globalScale,read,
    [](const consumer_shadow75::Sample&) noexcept {})||
    std::memcmp(&fresh.packet,&packet,sizeof(packet))||!Shoulder(fresh.sample,distanceScale))return false;
 if(!write(r.esp+0xe,uint8_t(1)))return false;
 if(history&&Shoulder(fresh.sample))history->AppliedInside(packet,frame,tick);
 return true;
}
}




