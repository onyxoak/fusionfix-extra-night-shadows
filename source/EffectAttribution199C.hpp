#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <limits>
namespace effect_attribution199c {
using Packet=std::array<uint32_t,32>;
inline constexpr uint32_t CallRva=0x6bd29f,TargetRva=0x6bd2c0,WrapperRva=0x6bcc50,ProducerRva=0x6bccd0;
inline constexpr uint32_t SourceStage=61000,BankStage=61010,StatusStage=61020;
inline constexpr unsigned PerWindow=8,PerSession=2048;
struct Registers {uintptr_t esp{},ebp{};};
struct Source {Packet packet{};uintptr_t address{};uint32_t parent{},producer{},callee{};};
inline bool Candidate(const Packet& p) noexcept {return (p[17]==0||p[17]==2)&&(p[18]&0x300u)==0x200u;}
// The full guarded native6BCCD0 and6BCC50 bodies establish these stack
// relations. Code/return evidence is diagnostic, never effect-owner authority.
template<class Read,class Call> bool Decode(Registers r,uintptr_t game,Read&& read,Call&& call,Source& out) noexcept {
 if(r.esp<0x10000||r.esp>UINT32_MAX-0xe0||r.ebp>UINT32_MAX-0x54||
    (r.ebp&~uintptr_t(15))!=r.esp+0xe8)return false;
 uint32_t pointer{},parent{},producer{},callee=ProducerRva;
 if(!read(r.esp,&pointer,4)||pointer!=r.esp+0x68||!read(r.ebp+4,&parent,4))return false;
 producer=parent;
 if(parent==game+0x6bcccb){if(!read(r.ebp+0x50,&producer,4))return false;callee=WrapperRva;}
 if(producer<game+5||uint64_t(producer)-game>UINT32_MAX||!call(producer-5,game+callee))return false;
 Source s{};s.address=pointer;s.parent=parent-uint32_t(game);s.producer=producer-uint32_t(game);s.callee=callee;
 Packet again{};uint32_t pointerAgain{},parentAgain{},producerAgain{};
 if(!read(pointer,s.packet.data(),120)||!Candidate(s.packet)||!read(pointer,again.data(),120)||again!=s.packet||
    !read(r.esp,&pointerAgain,4)||pointerAgain!=pointer||!read(r.ebp+4,&parentAgain,4)||parentAgain!=parent)return false;
 if(callee==WrapperRva&&(!read(r.ebp+0x50,&producerAgain,4)||producerAgain!=producer))return false;
 out=s;return true;
}
struct Budget {
 const uint32_t perWindow,perSession;
 explicit Budget(uint32_t w=PerWindow,uint32_t s=PerSession)noexcept:perWindow(w),perSession(s){}
 std::atomic<uint64_t> window{0};std::atomic<uint32_t> total{0},limited{0};
 void Reset() noexcept {window=0;total=0;limited=0;}
 bool Claim(uint32_t tick) noexcept {
  if(total.load(std::memory_order_relaxed)>=perSession){++limited;return false;}
  const uint32_t bin=tick/128+1;auto prior=window.load(std::memory_order_relaxed);
  for(unsigned tries=0;tries<4;++tries){
   const auto old=uint32_t(prior>>32),used=uint32_t(prior);
   if(old==bin&&used>=perWindow){++limited;return false;}
   const uint64_t next=(uint64_t(bin)<<32)|(old==bin?used+1:1);
   if(window.compare_exchange_weak(prior,next,std::memory_order_relaxed)){
    const auto n=total.fetch_add(1,std::memory_order_relaxed);if(n<perSession)return true;++limited;return false;
   }
  }++limited;return false;
 }
};
// Exact120-byte copy comparison. Only an authenticated released VL callback
// on a point/spot0xDEAD packet may account for its three volume metadata fields.
// Full unmodified packets are always retained alongside this diagnostic label.
inline unsigned CopyMatch(const Packet& a,const Packet& b,bool volumetric) noexcept {
 if(!std::memcmp(a.data(),b.data(),120))return 1;
 if(!volumetric||(a[17]!=0&&a[17]!=2)||a[20]!=0xdead)return 0;
 auto x=a,y=b;x[18]&=~8u;y[18]&=~8u;x[28]=y[28]=x[29]=y[29]=0;
 return !std::memcmp(x.data(),y.data(),120)?2u:0u;
}
}
