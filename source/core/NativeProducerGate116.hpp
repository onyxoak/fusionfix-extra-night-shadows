#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <span>

namespace native_producer_gate116 {
inline constexpr std::uint32_t FirstRva=0x6bd068,SecondRva=0x6bd093;
inline constexpr std::uint32_t BodyRva=0x6bccd0,BodySize=0x5f0;
inline constexpr std::array<std::uint8_t,6> FirstBytes{0x0f,0x83,0x4b,0x02,0,0};
inline constexpr std::array<std::uint8_t,6> SecondBytes{0x0f,0x84,0x20,0x02,0,0};
struct Registers {
 std::uint32_t ebp{},esp{},eflags{},eax{},type{},flags{};
 std::array<float,3> source{};
 float radius{},planeDistance{},negativeRadius{};
};
struct Context {
 bool pending{};
 std::uint32_t ebp{},esp{},nonce{},type{},flags{},tick{};
 std::array<float,3> source{};
 float radius{};
};
struct Event {
 std::uint32_t stage{},nonce{},tick{};
 std::int32_t a{},b{},c{},d{};
 float x{},y{},z{},w{};
};
inline bool Verify(std::span<const std::uint8_t> original,std::span<const std::uint8_t> live) noexcept {
 return original.size()==BodySize && live.size()==BodySize &&
  !std::memcmp(original.data(),live.data(),BodySize) &&
  !std::memcmp(live.data()+FirstRva-BodyRva,FirstBytes.data(),6) &&
  !std::memcmp(live.data()+SecondRva-BodyRva,SecondBytes.data(),6);
}
// This budget is wall-tick based, not an invented native-frame authority.
class Budget {
 std::atomic<std::uint64_t> state_{0};
public:
 static constexpr unsigned Maximum=64;
 bool Claim(std::uint32_t tick) noexcept {
  const auto window=tick&~std::uint32_t(127);
  auto old=state_.load(std::memory_order_relaxed);
  for(unsigned n=0;n<4;++n){
   const auto priorWindow=std::uint32_t(old>>32),count=std::uint32_t(old);
   if(priorWindow==window && count>=Maximum)return false;
   if(count && priorWindow!=window && std::int32_t(window-priorWindow)<0)return false;
   const auto next=(std::uint64_t(window)<<32)|(priorWindow==window?count+1:1);
   if(state_.compare_exchange_weak(old,next,std::memory_order_relaxed))return true;
  }
  return false;
 }
};
template<class Sink>bool First(Context& context,const Registers& r,std::uint32_t nonce,std::uint32_t tick,Sink&& sink) noexcept {
 context={};
 if(!nonce || (r.type!=0 && r.type!=2) || r.ebp<0x100e0 ||
    (r.ebp&~15u)-0xe0u!=r.esp)return false;
 // Exact JAE following native COMISS: CF clear includes equality; do not
 // recompute this result or change native NaN/signed-zero behavior.
 const bool rejected=(r.eflags&1u)==0;
 sink(Event{90,nonce,tick,std::int32_t(r.type),std::int32_t(r.flags),int(rejected),0,r.source[0],r.source[1],r.source[2],r.radius});
 sink(Event{92,nonce,tick,std::int32_t(r.eflags&0x45u),0,0,0,r.planeDistance,r.negativeRadius,0,0});
 if(!rejected)context={true,r.ebp,r.esp,nonce,r.type,r.flags,tick,r.source,r.radius};
 return true;
}
template<class Sink>bool Second(Context& context,const Registers& r,std::uint32_t tick,Sink&& sink) noexcept {
 const auto current=context;context={};
 if(!current.pending || current.ebp!=r.ebp || current.esp!=r.esp ||
    std::uint32_t(tick-current.tick)>1000u || bool(r.eflags&0x40u)!=(r.eax==0))return false;
 sink(Event{91,current.nonce,tick,std::int32_t(current.type),std::int32_t(current.flags),int(bool(r.eflags&0x40u)),std::int32_t(r.eax),current.source[0],current.source[1],current.source[2],current.radius});
 return true;
}
}
