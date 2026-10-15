#pragma once
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <atomic>

namespace consumer_shadow75 {
enum class Stage : unsigned { Lookup, Gate, Dispatch };
struct Registers { std::uintptr_t esp{},edi{};std::uint32_t eax{},esi{};float threshold{},ratio{}; };
struct Packet { std::array<std::uint32_t,30> words{}; };
struct Sample {
 std::uint32_t frame{},key{},flags{},type{},tick{};
 std::int32_t lookup{};
 std::uint8_t available{},sizeGate{};
 std::uint32_t mode{},technique{},effectiveType{};
 float radius{},gateRadius{},cameraScale{},globalScale{},distance{},ratio{},threshold{},recomputedRatio{},recomputedThreshold{};
 std::array<float,3> source{},gateSource{},camera{};
 bool finite{},reproduced{};
};
struct Context {
 bool pending{},gateSeen{};
 std::uintptr_t anchor{},source{};
 std::uint32_t frame{};
 Packet packet{};
 Sample sample{};
};
inline float Float(std::uint32_t v) noexcept {float f;std::memcpy(&f,&v,4);return f;}
inline bool PacketSupported(const Packet& p) noexcept {
 return (p.words[17]==0 || p.words[17]==2) && p.words[24]!=UINT32_MAX;
}
inline void Calculate(Sample& s) noexcept {
 const float dx=s.camera[0]-s.gateSource[0],dy=s.camera[1]-s.gateSource[1],dz=s.camera[2]-s.gateSource[2];
 // Match native MULSS/ADDSS lower-lane order: y^2+x^2, then z^2.
 const float a=dy*dy+dx*dx,b=dz*dz+a;
 s.distance=std::sqrt(b);s.recomputedRatio=s.gateRadius/s.distance;s.recomputedThreshold=s.cameraScale*s.globalScale;
 s.finite=s.available!=0 && std::isfinite(s.distance)&&s.distance>0&&std::isfinite(s.ratio)&&
  std::isfinite(s.threshold)&&std::isfinite(s.radius);
 // This does not authorize rendering or alter the native SETBE decision.
 s.reproduced=s.finite && bool(s.sizeGate)==(s.threshold<=s.ratio);
}
class FrameBudget {
 std::atomic<std::uint64_t> state_{0};
public:
 static constexpr unsigned Maximum=16;
 bool Claim(std::uint32_t frame,unsigned maximum=Maximum) noexcept {
  auto prior=state_.load(std::memory_order_relaxed);
  for(unsigned n=0;n<4;++n){
   const auto oldFrame=std::uint32_t(prior>>32),count=std::uint32_t(prior);
   if(oldFrame==frame && count>=maximum)return false;
   if(oldFrame!=frame && std::int32_t(frame-oldFrame)<=0)return false;
   const auto next=(std::uint64_t(frame)<<32)|(oldFrame==frame?count+1:1);
   if(state_.compare_exchange_weak(prior,next,std::memory_order_relaxed))return true;
  }return false;
 }
};
// Production decoder: reader performs fresh permission checks and contained
// copies. Every address is from this exact native invocation, never retained
// as authority across calls. Sink receives copied numeric data only.
template<class Read,class Sink>
bool Probe(Context& c,Stage stage,const Registers& r,std::uint32_t frame,
 std::uint32_t tick,std::uintptr_t globalScale,Read&& read,Sink&& sink) noexcept {
 if(stage==Stage::Lookup){
  c={};if(r.edi<0x10028 || r.esp<0x10000 || r.esp>UINTPTR_MAX-0x58)return false;
  Packet p{};if(!read(r.edi-0x28,&p,sizeof(p))||!PacketSupported(p))return false;
  c.pending=true;c.source=r.edi-0x28;c.anchor=r.esp+12;c.frame=frame;c.packet=p;
  c.sample.frame=frame;c.sample.key=p.words[24];c.sample.lookup=std::int32_t(r.eax);
  return true;
 }
 if(!c.pending || c.frame!=frame){c={};return false;}
 const auto extra=stage==Stage::Dispatch?20u:0u;
 if(r.esp>UINTPTR_MAX-extra || r.esp+extra!=c.anchor){c={};return false;}
 std::uint32_t sourcePointer{};Packet p{};
 if(!read(r.esp+extra+0x1c,&sourcePointer,4)||sourcePointer<0x10028 ||
  std::uintptr_t(sourcePointer)-0x28!=c.source || !read(c.source,&p,sizeof(p)) ||
  std::memcmp(&p,&c.packet,sizeof(p))){c={};return false;}
 if(stage==Stage::Gate){
  auto& s=c.sample;std::uint32_t cameraPointer{};
  std::array<std::uint8_t,2> decisions{};
  if(!read(r.esp+0xd,decisions.data(),2)||decisions[0]>1||decisions[1]>1 ||
   !read(r.esp+0x28,&cameraPointer,4)||cameraPointer<0x10000 || cameraPointer>UINTPTR_MAX-0x2d0 ||
   !read(std::uintptr_t(cameraPointer)+0x70,s.camera.data(),12)||
   !read(std::uintptr_t(cameraPointer)+0x2cc,&s.cameraScale,4)||
   !read(globalScale,&s.globalScale,4)||!read(r.esp+0x14,&s.gateRadius,4)||
   !read(r.esp+0x40,s.gateSource.data(),12)){c={};return false;}
  s.available=decisions[0];s.sizeGate=decisions[1];s.flags=p.words[18];s.type=p.words[17];
  s.radius=Float(p.words[21]);for(unsigned n=0;n<3;++n)s.source[n]=Float(p.words[8+n]);
  s.ratio=r.ratio;s.threshold=r.threshold;s.tick=tick;Calculate(s);c.gateSeen=true;return true;
 }
 if(!c.gateSeen){c={};return false;}
 std::uint32_t mode{};if(!read(r.esp,&mode,4)||mode>1){c={};return false;}
 c.sample.tick=tick;c.sample.mode=mode;c.sample.technique=r.esi;c.sample.effectiveType=std::uint32_t(r.edi);
 const auto s=c.sample;c={};sink(s);return true;
}
}
