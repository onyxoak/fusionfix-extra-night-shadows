#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <utility>
namespace present_time151 {
inline constexpr uint64_t PublicationLimit153=65536;
inline bool PublicationCapReached153(uint64_t written) noexcept {return written>=PublicationLimit153;}
enum Flag : uint32_t { First=1, EpochChanged=2, ClockDiscontinuity=4, NativeClockInvalid=8, PresentFailed=16, LostIndication=32, RecoveryIndication=64, LongGap=128, ResetUnknown=256, ExplicitReset=512 };
struct Token {uint64_t ordinal{},start{},interval{},nativeStart{};uint32_t frame{},thread{},epoch{},flags{};bool valid{};};
struct Record {Token token{};uint64_t nativeEnd{};int32_t result{};};
template<unsigned Capacity=4096> class State {
 std::array<Record,Capacity> rows{};std::atomic<uint64_t> write{0},read{0};
 uint64_t lastStart{},ordinal{},frequency{};std::atomic<uint32_t> owner{0};uint32_t device{},proxy{},epoch{};bool flight{},lost{},reset{};
public:
 std::atomic<uint64_t> drops{0},threadRefusals{0},nestedRefusals{0},clockRefusals{0},identityRefusals{0};
 void Configure(uint64_t f) noexcept {frequency=f;}
 Token Begin(uint32_t thread,uint32_t dev,uint32_t inner,uint32_t frame,uint64_t now) noexcept {
  if(!frequency||!now){++clockRefusals;return {};}
  if(!dev||!inner){++identityRefusals;return {};}
  if(!thread){++threadRefusals;return {};}
  uint32_t empty=0;owner.compare_exchange_strong(empty,thread,std::memory_order_acq_rel);
  if(owner.load(std::memory_order_acquire)!=thread){++threadRefusals;return {};}
  if(flight){++nestedRefusals;return {};}
  uint32_t flags=ResetUnknown;
  if(dev!=device||inner!=proxy||reset){device=dev;proxy=inner;++epoch;lastStart=0;flags|=EpochChanged;if(reset)flags|=ExplicitReset;reset=false;}
  uint64_t interval=0;
  if(!lastStart)flags|=First;
  else if(now<=lastStart)flags|=ClockDiscontinuity;
  else {interval=now-lastStart;if(interval/frequency>=10)flags|=LongGap;}
  lastStart=now;flight=true;return {++ordinal,now,interval,0,frame,thread,epoch,flags,true};
 }
 void End(Token t,uint64_t nativeEnd,int32_t result) noexcept {
  if(!t.valid)return;flight=false;
  if(!t.nativeStart||t.nativeStart<t.start||nativeEnd<t.nativeStart)t.flags|=NativeClockInvalid;
  if(result<0)t.flags|=PresentFailed;
  const bool indication=uint32_t(result)==0x88760868u||uint32_t(result)==0x88760869u;
  if(indication){t.flags|=LostIndication;lost=true;}
  else if(result>=0&&lost){t.flags|=RecoveryIndication;lost=false;lastStart=0;++epoch;}
  const auto w=write.load(std::memory_order_relaxed),r=read.load(std::memory_order_acquire);
  if(w-r>=Capacity){++drops;return;}rows[w%Capacity]={t,nativeEnd,result};write.store(w+1,std::memory_order_release);
 }
 unsigned Drain(Record* out,unsigned maximum) noexcept {
  auto r=read.load(std::memory_order_relaxed);const auto w=write.load(std::memory_order_acquire);unsigned n=0;
  while(r<w&&n<maximum){out[n++]=rows[r%Capacity];++r;}read.store(r,std::memory_order_release);return n;
 }
 // Optional existing GPU Reset callback, on owner only; no extra Reset hook.
 void ObserveReset(uint32_t thread) noexcept {if(thread==owner.load(std::memory_order_acquire))reset=true;}
 uint64_t Frequency()const noexcept{return frequency;}
};
// Guard is deliberately split before/after original: preserve BOTH native
// caller input and callee output FP/LastError. Never reset native output to input.
template<class Ops,class Native,class Device,class... Args>
auto Invoke(bool active,Ops& ops,Native&& native,Device device,Args... args) {
 if(!active)return std::forward<Native>(native)(device,args...);
 typename Ops::Token token{};
 {const typename Ops::Guard preserve;token=ops.Before(device);}
 auto result=std::forward<Native>(native)(device,args...);
 {const typename Ops::Guard preserve;ops.After(token,result);}
 return result;
}
}
