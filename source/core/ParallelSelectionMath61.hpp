#pragma once
// Parallel numeric hints only. No engine pointer, owner, permission or map authority.
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "SceneShadowPriority21.hpp"
#include "ShadowVolumeVisibility43.hpp"
#include "ShadowFloatingPointState.hpp"
#include <windows.h>
#include <atomic>
#include <array>
#include <cstring>
#include <cstdint>
#include <type_traits>
#ifndef CE_PARALLEL_CREATE_THREAD61
#define CE_PARALLEL_CREATE_THREAD61(function,context) CreateThread(nullptr,0,function,context,0,nullptr)
#endif
namespace fusionfix::shadows::parallel61 {
constexpr unsigned Slots=8192; // two independent pure phases per4096 native indices
struct ErrorScope {DWORD error=GetLastError();~ErrorScope(){SetLastError(error);}};
struct Scope {DWORD error=GetLastError();FloatingPointState fp;~Scope(){SetLastError(error);}};
struct Context {
 ShadowView view{};Vec3 player{},focus{};uint32_t driving{};float strength{};uint32_t mxcsr{};uint16_t control{};
};
inline void CaptureControl(Context& c) noexcept {c.mxcsr=_mm_getcsr()&~0x3fu;
#if defined(_M_IX86)
 uint16_t cw{};__asm fnstcw cw
 c.control=cw;
#else
 c.control=0;
#endif
}
inline bool Policy(const Context& c) noexcept{return c.mxcsr==0x1f80&&c.control==0x37f;}
inline bool Equal(const Context&a,const Context&b) noexcept {
 return a.view.valid==b.view.valid&&!std::memcmp(a.view.view,b.view.view,sizeof(a.view.view))&&
 !std::memcmp(a.view.projection,b.view.projection,sizeof(a.view.projection))&&
 !std::memcmp(&a.player,&b.player,sizeof(a.player))&&!std::memcmp(&a.focus,&b.focus,sizeof(a.focus))&&
 a.driving==b.driving&&!std::memcmp(&a.strength,&b.strength,4)&&a.mxcsr==b.mxcsr&&a.control==b.control;
}
// Match precisely the numeric dependencies of each Compute phase.
inline bool EqualPhase(const Context&a,const Context&b,unsigned phase) noexcept {
 if(a.mxcsr!=b.mxcsr||a.control!=b.control)return false;
 if(phase==0)return a.view.valid==b.view.valid&&!std::memcmp(a.view.view,b.view.view,sizeof(a.view.view))&&
  !std::memcmp(a.view.projection,b.view.projection,sizeof(a.view.projection));
 if(phase==1)return !std::memcmp(&a.player,&b.player,sizeof(a.player))&&
  !std::memcmp(&a.focus,&b.focus,sizeof(a.focus))&&a.driving==b.driving&&!std::memcmp(&a.strength,&b.strength,4);
 return false;
}
struct Input {
 uint32_t slot{},key{},generationLow{},generationHigh{},phase{},beam{},type{};
 std::array<float,3> position{},direction{},rgb{};
 float radius{},intensity{},outerCone{},viewWeight{};uint32_t volumeVisible{};
};
static_assert(sizeof(Input)==84&&std::is_trivially_copyable_v<Input>);
struct Output {float viewWeight{};SceneReceiverConsequence44 consequence{};};
inline Output Compute(const Context& c,const Input& in) noexcept {
 const Vec3 p{in.position[0],in.position[1],in.position[2]},d{in.direction[0],in.direction[1],in.direction[2]};Output out{};
 if(in.phase==0)out.viewWeight=ShadowViewWeight(c.view,p,d,in.radius,in.beam!=0);
 else {
  const float subject=EvaluateGeometry(c.player,p).distanceSquared;
  const float road=c.driving?EvaluateGeometry(c.focus,p).distanceSquared:subject;
  const float coverage=SceneReceiverCoverage21(c.player,p,d,int(in.type),in.outerCone);
  const float roadCoverage=c.driving?SceneReceiverCoverage21(c.focus,p,d,int(in.type),in.outerCone):coverage;
  out.consequence=SceneConsequence44(subject,road,in.radius,in.intensity,
   {in.rgb[0],in.rgb[1],in.rgb[2]},in.viewWeight,in.volumeVisible!=0,c.driving!=0,coverage,roadCoverage,c.strength);
 }
 return out;
}
// Caller-owned producer tape and two immutable jobs. No dynamic/TLS constructor.
class Service {
 enum State:unsigned {Empty,Writing,Queued,Running,Ready};
 struct Job {
  std::atomic<unsigned> state{Empty};uint64_t serial{};Context context{};unsigned count{};
  std::array<Input,Slots> input{};std::array<Output,Slots> output{};std::array<unsigned,Slots> lookup{};
 };
 std::array<Job,2> jobs_{};std::array<Input,Slots> tape_{};Context context_{};unsigned count_{};bool capture_{};Context stabilityContext_{};std::array<unsigned,2> stablePasses_{};
 std::atomic<DWORD> owner_{0};HANDLE semaphore_{};std::array<HANDLE,2> threads_{};unsigned workerCount_{};uint64_t serial_{};
 std::atomic<bool> stopping_{false};
 static DWORD WINAPI Thread(void* value) noexcept {
  auto& s=*static_cast<Service*>(value);
  for(;;){if(WaitForSingleObject(s.semaphore_,INFINITE)!=WAIT_OBJECT_0)return 1;if(s.stopping_.load(std::memory_order_acquire))return 0;
   for(auto& j:s.jobs_){unsigned expected=Queued;if(!j.state.compare_exchange_strong(expected,Running,std::memory_order_acq_rel))continue;
#ifdef CE_PARALLEL_WORKER_BEFORE61
    CE_PARALLEL_WORKER_BEFORE61();
#endif
    {const Scope preserve;for(unsigned i=0;i<j.count;++i)j.output[i]=Compute(j.context,j.input[i]);}
#ifdef CE_PARALLEL_WORKER_AFTER61
    CE_PARALLEL_WORKER_AFTER61();
#endif
    j.state.store(Ready,std::memory_order_release);break;
   }
  }
 }
 bool Owner() const noexcept{return owner_.load(std::memory_order_acquire)==GetCurrentThreadId();}
public:
 std::atomic<uint32_t> submitted{0},completedHits{0},serialMisses{0},busyDrops{0},captureDrops{0};
 Service()=default;Service(const Service&)=delete;Service& operator=(const Service&)=delete;
 ~Service(){if(semaphore_&&!Owner())__fastfail(7);Stop();}
 bool Start(unsigned workers=2) noexcept {
  const Scope preserve;if(semaphore_||workers<1||workers>2)return false;owner_=GetCurrentThreadId();stopping_=false;
  semaphore_=CreateSemaphoreW(nullptr,0,LONG_MAX,nullptr);if(!semaphore_){owner_=0;return false;}
  for(unsigned i=0;i<workers;++i){threads_[i]=CE_PARALLEL_CREATE_THREAD61(Thread,this);if(!threads_[i]){Stop();return false;}(void)SetThreadPriority(threads_[i],THREAD_PRIORITY_BELOW_NORMAL);++workerCount_;}
  return true;
 }
 void Stop() noexcept {
  const Scope preserve;if(!semaphore_)return;if(!Owner())return;capture_=false;stopping_.store(true,std::memory_order_release);
  (void)ReleaseSemaphore(semaphore_,LONG(workerCount_),nullptr);
  for(auto& t:threads_)if(t){(void)WaitForSingleObject(t,INFINITE);CloseHandle(t);t=nullptr;}
  CloseHandle(semaphore_);semaphore_=nullptr;workerCount_=0;for(auto&j:jobs_)j.state=Empty;
 }
 // Startup thread releases ownership before publishing this process-lifetime service.
 bool ReleaseProducer() noexcept {
  const ErrorScope preserve;if(!semaphore_||!Owner()||capture_)return false;owner_.store(0,std::memory_order_release);return true;
 }
 bool Begin(const Context& c) noexcept {
  const Scope preserve;if(!semaphore_||!Policy(c))return false;DWORD unbound=0;
  (void)owner_.compare_exchange_strong(unbound,GetCurrentThreadId(),std::memory_order_acq_rel);
  if(!Owner())return false;context_=c;count_=0;capture_=true;return true;
 }
 // Only stable phase dependencies are speculated; other phases stay serial.
 unsigned BeginStable(const Context& c) noexcept {
  const Scope preserve;if(!semaphore_||!Policy(c))return 0;DWORD unbound=0;
  (void)owner_.compare_exchange_strong(unbound,GetCurrentThreadId(),std::memory_order_acq_rel);if(!Owner())return 0;
  capture_=false;unsigned phases=0;
  for(unsigned phase=0;phase<2;++phase){
   if(!stablePasses_[phase]||!EqualPhase(stabilityContext_,c,phase))stablePasses_[phase]=1;
   else if(stablePasses_[phase]<3)++stablePasses_[phase];
   if(stablePasses_[phase]>=3)phases|=1u<<phase;
  }
  stabilityContext_=c;
  if(phases){context_=c;count_=0;capture_=true;}
  return phases;
 }
 bool Record(const Context& c,const Input& in) noexcept {
  const ErrorScope preserve;
  if(!Owner()||!capture_||!Equal(context_,c)||in.slot>=Slots||in.phase>1||count_==Slots){capture_=false;captureDrops.fetch_add(1,std::memory_order_relaxed);return false;}
  tape_[count_++]=in;return true;
 }
 bool TryGet(const Context& c,const Input& in,Output& out) noexcept {
  const ErrorScope preserve;
  if(!Owner()||!Policy(c)||in.slot>=Slots){serialMisses.fetch_add(1,std::memory_order_relaxed);return false;}
  Job* chosen=nullptr;
  for(auto&j:jobs_)if(j.state.load(std::memory_order_acquire)==Ready&&EqualPhase(j.context,c,in.phase)){
   const unsigned i=j.lookup[in.slot];if(i>=j.count||j.input[i].slot!=in.slot||std::memcmp(&j.input[i],&in,sizeof(in)))continue;
   if(!chosen||j.serial>chosen->serial)chosen=&j;
  }
  if(!chosen){serialMisses.fetch_add(1,std::memory_order_relaxed);return false;}out=chosen->output[chosen->lookup[in.slot]];completedHits.fetch_add(1,std::memory_order_relaxed);return true;
 }
 bool Dispatch() noexcept {
  const Scope preserve;if(!Owner()||!semaphore_||!capture_||!count_)return false;capture_=false;
  Job* chosen=nullptr;
  for(auto&j:jobs_){const auto state=j.state.load(std::memory_order_acquire);if(state==Empty){chosen=&j;break;}if(state==Ready&&(!chosen||j.serial<chosen->serial))chosen=&j;}
  if(!chosen){busyDrops.fetch_add(1,std::memory_order_relaxed);return false;}unsigned expected=chosen->state.load(std::memory_order_acquire);
  if((expected!=Empty&&expected!=Ready)||!chosen->state.compare_exchange_strong(expected,Writing,std::memory_order_acq_rel)){busyDrops.fetch_add(1,std::memory_order_relaxed);return false;}
  chosen->context=context_;chosen->count=count_;chosen->serial=++serial_;std::memcpy(chosen->input.data(),tape_.data(),sizeof(Input)*count_);
  for(unsigned i=0;i<count_;++i)chosen->lookup[chosen->input[i].slot]=i;
  chosen->state.store(Queued,std::memory_order_release);
  if(!ReleaseSemaphore(semaphore_,1,nullptr)){unsigned q=Queued;(void)chosen->state.compare_exchange_strong(q,Empty);return false;}
  submitted.fetch_add(1,std::memory_order_relaxed);return true;
 }
 unsigned Workers() const noexcept{return workerCount_;}
};
}
