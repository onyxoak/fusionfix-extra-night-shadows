#pragma once
#include "PresetGeneration117.hpp"
namespace preset117 {
// One numeric UI request, not emitter/owner/permission authority. No hook
// installation, key retirement or family/master change is performed here.
class LiveReach122 {
 std::atomic<uint64_t> desired_{0},acknowledged_{0};
 uint64_t committed_=0;uint32_t committedFrame_=0; // selection-owner only
 static constexpr uint32_t Encode(const State& s) noexcept {
  uint32_t bits=unsigned(s.preset)|(unsigned(s.base)<<3)|(unsigned(s.enabled)<<6);
  for(unsigned i=0;i<s.features.size();++i)bits|=unsigned(s.features[i])<<(7+i);
  return bits|(s.lampReachSteps<<12)|(s.headlightReachSteps<<19);
 }
 static constexpr State Decode(uint32_t bits) noexcept {
  State s{};s.preset=Preset(bits&7);s.base=Preset((bits>>3)&7);s.enabled=((bits>>6)&1)!=0;
  for(unsigned i=0;i<s.features.size();++i)s.features[i]=((bits>>(7+i))&1)!=0;
  s.lampReachSteps=(bits>>12)&127;s.headlightReachSteps=(bits>>19)&127;return s;
 }
public:
 static constexpr bool Valid(const State& s) noexcept {
  return Implemented(s.base)&&(Implemented(s.preset)||s.preset==Preset::Custom)&&s.lampReachSteps<=100&&s.headlightReachSteps<=100;
 }
 bool Request(const State& s) noexcept {
  if(!Valid(s))return false;const auto bits=Encode(s);auto old=desired_.load(std::memory_order_relaxed);
  for(;;){if(old&&uint32_t(old)==bits)return true;
   const auto revision=uint32_t(old>>32);if(revision==UINT32_MAX)return false;
   const auto next=(uint64_t(revision+1)<<32)|bits;
   if(desired_.compare_exchange_weak(old,next,std::memory_order_release,std::memory_order_relaxed))return true;
  }
 }
 // Commit only after the caller proves the same outer selection-owner thread.
 // Acknowledge on a later native frame so the headlight submission preceding
 // Select has also had an opportunity to consume the new reach. Nested calls
 // and same-frame repeats do not apply or acknowledge another request.
 template<class Store> bool Boundary(const State& installed,uint32_t frame,bool safe,Store store) noexcept {
  if(!safe)return false;
  if(committed_&&committedFrame_==frame)return false;
  if(committed_&&committedFrame_!=frame)acknowledged_.store(committed_,std::memory_order_release);
  const auto requested=desired_.load(std::memory_order_acquire);if(!requested)return false;
  const auto state=Decode(uint32_t(requested));
  if(!Valid(state)||!installed.enabled||state.enabled!=installed.enabled||state.features!=installed.features)return false;
  if(requested==committed_)return false;
  store(state.lampReachSteps,state.headlightReachSteps);
  committed_=requested;committedFrame_=frame;return true;
 }
 State Acknowledged(const State& installed) const noexcept {
  const auto word=acknowledged_.load(std::memory_order_acquire);return word?Decode(uint32_t(word)):installed;
 }
};
inline LiveReach122 liveReach122;
inline bool QueueLiveReach122(const State& desired) noexcept {return liveReach122.Request(desired);}
}
