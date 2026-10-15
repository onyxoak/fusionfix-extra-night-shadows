#pragma once
#include "ConsumerShadow75.hpp"

namespace consumer_reach136 {
// Policy history only: callers must still prove the CURRENT source, available
// map and complete invocation. No packet stored here can authorize a draw.
class ApplicationHistory198 {
 struct Entry {consumer_shadow75::Packet packet{};uint32_t frame{},tick{};bool occupied=false;};
 std::array<Entry,32> entries_{};
 uint32_t frame_{},tick_{};
 bool begun_=false;
 static unsigned Slot(uint32_t key) noexcept {return ((key*0x9e3779b9u)^(key>>16))&31u;}
 static bool Same(const Entry& e,const consumer_shadow75::Packet& p) noexcept {
  return e.occupied&&!std::memcmp(&e.packet,&p,sizeof(p));
 }
public:
 static constexpr uint32_t HoldMs=180,MaximumFrameGap=12;
 static constexpr float ReleaseScale=1.40f;
 void Clear() noexcept {entries_={};begun_=false;}
 void Begin(uint32_t frame,uint32_t tick) noexcept {
  if(begun_&&(frame<frame_||uint32_t(tick-tick_)>HoldMs||uint32_t(frame-frame_)>MaximumFrameGap))entries_={};
  frame_=frame;tick_=tick;begun_=true;
 }
 bool Retained(const consumer_shadow75::Packet& p,uint32_t frame,uint32_t tick) noexcept {
  Begin(frame,tick);
  if(!p.words[24]||p.words[24]==UINT32_MAX)return false;
  for(unsigned n=0;n<4;++n){const auto& e=entries_[(Slot(p.words[24])+n)&31u];
   if(Same(e,p))return uint32_t(tick-e.tick)<=HoldMs&&uint32_t(frame-e.frame)<=MaximumFrameGap;
  }
  return false;
 }
 // Call only AFTER a successful native decision write inside the inherited
 //1.30 band. Outer-band application never refreshes its own hold deadline.
 void AppliedInside(const consumer_shadow75::Packet& p,uint32_t frame,uint32_t tick) noexcept {
  Begin(frame,tick);
  if(!p.words[24]||p.words[24]==UINT32_MAX)return;
  Entry* vacant=nullptr;
  for(unsigned n=0;n<4;++n){auto& e=entries_[(Slot(p.words[24])+n)&31u];
   if(Same(e,p)){e.frame=frame;e.tick=tick;return;}
   if(!vacant&&(!e.occupied||uint32_t(tick-e.tick)>HoldMs||uint32_t(frame-e.frame)>MaximumFrameGap))vacant=&e;
  }
  if(vacant)*vacant={p,frame,tick,true}; // Collision pressure falls back to195.
 }
};
}
