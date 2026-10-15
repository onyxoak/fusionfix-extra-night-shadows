#pragma once
#include <array>
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace fusionfix::shadows {
// One ranking scalar per CURRENT, authenticated source. No retained packets,
// permissions, pointers, radiance, maps or geometry; no eligibility override.
class SceneScoreContinuity96 {
 struct Entry {uint32_t key{},frame{},time{};uint64_t generation{};float score{};};
 std::array<Entry,2048> entries_{};
 uintptr_t subject_{},vehicle_{};uint32_t frame_{},time_{};
public:
 void Begin(uintptr_t subject,uintptr_t vehicle,uint32_t frame,uint32_t now) noexcept {
  if(subject!=subject_||vehicle!=vehicle_||frame<frame_||uint32_t(now-time_)>2000u)entries_={};
  subject_=subject;vehicle_=vehicle;frame_=frame;time_=now;
 }
 float Current(uint32_t key,uint64_t generation,float raw,bool retained,float releaseMs) noexcept {
  if(!key||!std::isfinite(raw)||raw<0||!std::isfinite(releaseMs)||releaseMs<=0)return raw;
  auto hash=(key*0x9e3779b9u)^uint32_t(generation)^uint32_t(generation>>32);
  hash^=hash>>16;hash*=0x7feb352du;hash^=hash>>15;
  Entry* vacant=nullptr;
  for(unsigned n=0;n<4;++n){auto& e=entries_[(hash+n)&2047u];
   if(e.key==key&&e.generation==generation&&uint32_t(frame_-e.frame)<=16u&&uint32_t(time_-e.time)<=2000u){
    // New/improving receivers acquire immediately. Only a selected source's
    // loss of consequence releases gradually; this never pins missing sources.
    if(!retained||raw<=e.score)e.score=raw;
    else if(e.frame!=frame_){
     const float elapsed=float(uint32_t(time_-e.time));
     e.score+=(raw-e.score)*(elapsed/(releaseMs+elapsed));
    }
    e.frame=frame_;e.time=time_;return e.score;
   }
   if(!vacant&&(!e.key||uint32_t(frame_-e.frame)>16u||uint32_t(time_-e.time)>2000u))vacant=&e;
  }
  if(vacant)*vacant={key,frame_,time_,generation,raw};
  return raw; // Bounded collision pressure cannot borrow another source's history.
 }
};
}
