// Default-off, lease-only read-only attribution. No emitter/key/flag changes.
#include "EffectAttribution199C.hpp"
namespace EffectAttribution199C {
using effect_attribution199c::Packet;
bool requested=false,prepared=false;
std::atomic<bool> active{false};
std::atomic<bool> exhausted{false};
std::atomic_flag recordingLock=ATOMIC_FLAG_INIT;
SafetyHookMid insertionHook;
std::atomic<uint32_t> captureEpoch{0},nextEpoch{0},sequence{0},rejected{0},records{0};
effect_attribution199c::Budget sourceBudget,bankBudget;
effect_attribution199c::Budget attemptBudget{128,32768};
thread_local bool busy=false;
bool Capturing() noexcept {return !exhausted.load(std::memory_order_relaxed)&&active.load(std::memory_order_acquire)&&addonActive.load(std::memory_order_acquire)&&captureEpoch.load(std::memory_order_acquire)&&
 CaptureDiagnostic44::sampleSources.load(std::memory_order_acquire)&&ShadowTrace34::enabled.load(std::memory_order_acquire);}
bool CallMatches(uintptr_t address,uintptr_t target) noexcept {
 if(!original||address<game||address-game>original->bytes.size()||original->bytes.size()-(address-game)<5)return false;
 uint8_t bytes[5]{};int32_t relative{};
 if(!Liberty20::Copy(address,bytes,5)||bytes[0]!=0xe8||std::memcmp(bytes,original->bytes.data()+address-game,5))return false;
 std::memcpy(&relative,bytes+1,4);return uint32_t(address+5+relative)==target;
}
void Emit(uint32_t stage,uint32_t epoch,uint32_t frame,uint32_t tick,const std::array<uint32_t,8>& meta,const Packet& packet) noexcept {
 if(!Capturing()||captureEpoch.load(std::memory_order_acquire)!=epoch)return;
 const auto id=sequence.fetch_add(1,std::memory_order_relaxed)+1;if(!id){exhausted=true;++rejected;return;}
 ShadowTrace34::Event e{};e.frame=frame;e.tick=tick;e.key=id;e.stage=stage;
 static_assert(sizeof(e)-offsetof(ShadowTrace34::Event,a)==32);
 std::memcpy(&e.a,meta.data(),32);ShadowTrace34::Emit(e);
 for(unsigned i=0;i<4;++i){
  if(!Capturing()||captureEpoch.load(std::memory_order_acquire)!=epoch)return;
  e.stage=stage+1+i;std::memcpy(&e.a,packet.data()+i*8,32);ShadowTrace34::Emit(e);
 }
 ++records;
}
// CALLBACK199C_BEGIN
void Insertion(SafetyHookContext& regs) noexcept {
 if(!Capturing()||busy)return;
 const ModelEffectObserver::LastErrorScope error;
 if(recordingLock.test_and_set(std::memory_order_acquire)){++rejected;return;}
 busy=true;struct Leave{~Leave(){busy=false;recordingLock.clear(std::memory_order_release);}}leave;
 const auto epoch=captureEpoch.load(std::memory_order_acquire);const auto tick=GetTickCount();
 if(!attemptBudget.Claim(tick))return;
 const fusionfix::shadows::FloatingPointState fp;
 uint32_t frame{},again{};
 const auto read=[](uintptr_t p,void* out,size_t n)noexcept{return Liberty20::Copy(p,out,n);};
 if(!CShadows::pFrameCounter||!read(reinterpret_cast<uintptr_t>(CShadows::pFrameCounter),&frame,4))return;
 effect_attribution199c::Source source{};
 if(!effect_attribution199c::Decode({regs.esp,regs.ebp},game,read,CallMatches,source)){++rejected;return;}
 if(!sourceBudget.Claim(tick)||!read(reinterpret_cast<uintptr_t>(CShadows::pFrameCounter),&again,4)||again!=frame)return;
 Emit(effect_attribution199c::SourceStage,epoch,frame,tick,
  {GetCurrentThreadId(),source.producer,source.parent,uint32_t(source.address),source.callee,0,epoch,NativeProjection66::registry.volumetricMetadata199?1u:0u},source.packet);
}
void Bank(unsigned index,rage::CLightSource* lights,uint32_t count,uint32_t frame,const Packet& packet) noexcept {
 if(!Capturing()||busy||!effect_attribution199c::Candidate(packet))return;
 const ModelEffectObserver::LastErrorScope error;
 if(recordingLock.test_and_set(std::memory_order_acquire)){++rejected;return;}
 busy=true;struct Leave{~Leave(){busy=false;recordingLock.clear(std::memory_order_release);}}leave;
 const auto epoch=captureEpoch.load(std::memory_order_acquire);const auto tick=GetTickCount();
 if(!lights||index>=count||count>640||!bankBudget.Claim(tick))return;
 const fusionfix::shadows::FloatingPointState fp;
 uint32_t now{};Packet again{};
 if(!CShadows::pFrameCounter||!Liberty20::Copy(reinterpret_cast<uintptr_t>(CShadows::pFrameCounter),&now,4)||now!=frame||
    PlayerShadowAllocation::CurrentLights()!=lights||PlayerShadowAllocation::CurrentCount()!=count||
    !Liberty20::Copy(reinterpret_cast<uintptr_t>(lights+index),again.data(),120)||std::memcmp(again.data(),packet.data(),120)||
    !Liberty20::Copy(reinterpret_cast<uintptr_t>(CShadows::pFrameCounter),&now,4)||now!=frame||
    PlayerShadowAllocation::CurrentLights()!=lights||PlayerShadowAllocation::CurrentCount()!=count){++rejected;return;}
 Emit(effect_attribution199c::BankStage,epoch,frame,tick,
  {GetCurrentThreadId(),uint32_t(reinterpret_cast<uintptr_t>(lights)),count,index,1,0,epoch,NativeProjection66::registry.volumetricMetadata199?1u:0u},again);
}
// CALLBACK199C_END
void CaptureTransition(int transition) noexcept {
 if(!requested||!active.load(std::memory_order_acquire)||!transition)return;
 captureEpoch.store(0,std::memory_order_release);
 if(recordingLock.test_and_set(std::memory_order_acquire)){
  Event(1,"EFFECT_ATTRIBUTION199C_CAPTURE","armed=0 transition_contended=1 records_are_unknown=1");return;
 }
 struct Leave{~Leave(){recordingLock.clear(std::memory_order_release);}}leave;
 if(transition==1){sourceBudget.Reset();bankBudget.Reset();attemptBudget.Reset();rejected=0;records=0;
  auto epoch=nextEpoch.fetch_add(1,std::memory_order_relaxed)+1;
  if(!epoch){exhausted=true;return;}
  ShadowTrace34::Event marker{};marker.stage=effect_attribution199c::StatusStage;marker.tick=GetTickCount();marker.key=epoch;
  const std::array<uint32_t,8> meta{epoch,1,effect_attribution199c::CallRva,128,effect_attribution199c::PerWindow,effect_attribution199c::PerSession,0,0};
  std::memcpy(&marker.a,meta.data(),32);ShadowTrace34::Emit(marker);
  captureEpoch.store(epoch,std::memory_order_release);
  Event(1,"EFFECT_ATTRIBUTION199C_CAPTURE","started=1 diagnostic_only=1 attempts_per_128ms=128 attempts_per_capture=32768 per_stage_per_128ms=8 per_stage_per_capture=2048 timing_comparison_invalid=1");
 }else Event(1,"EFFECT_ATTRIBUTION199C_CAPTURE","stopped=1 records="+std::to_string(records.load())+" rejected="+std::to_string(rejected.load())+
  " attempts="+std::to_string(attemptBudget.total.load())+" attempt_limited="+std::to_string(attemptBudget.limited.load())+
  " source_limited="+std::to_string(sourceBudget.limited.load())+" bank_limited="+std::to_string(bankBudget.limited.load())+" incomplete_ambiguous_missing_are_unknown=1");
}
void Disable() noexcept {captureEpoch=0;active=false;if(insertionHook)(void)insertionHook.disable();prepared=false;NativeProjection66::diagnosticPacket199c=nullptr;}
void PrepareHooks() noexcept {
 if(!requested)return;
 try {
  Require(NativeProjection66::authenticated,"effect_attribution199c_projection_prerequisite");
  for(const auto [rva,size]:std::array<std::pair<uint32_t,size_t>,2>{{{0x6bcc50,0x7f},{0x6bccd0,0x663}}}){
   const auto live=ReadMemory(game+rva,size);Require(original&&rva+size<=original->bytes.size()&&!memcmp(live.data(),original->bytes.data()+rva,size),"effect_attribution199c_native_body_changed");
  }
  Require(OriginalCall(effect_attribution199c::CallRva)==game+effect_attribution199c::TargetRva&&OriginalCall(0x6bccc6)==game+0x6bccd0,"effect_attribution199c_call_changed");
  Prepare(insertionHook,game+effect_attribution199c::CallRva,Insertion);
  Require(insertionHook.original_bytes().size()==5,"effect_attribution199c_stolen_span_changed");prepared=true;
 }catch(const std::exception& e){Disable();Event(1,"EFFECT_ATTRIBUTION199C_REFUSED",e.what());}
 catch(...){Disable();Event(1,"EFFECT_ATTRIBUTION199C_REFUSED","unknown_prepare_failure");}
}
void Enable() noexcept {
 if(!requested||!prepared)return;
 try{Require(bool(insertionHook.enable()),"effect_attribution199c_enable_failed");NativeProjection66::diagnosticPacket199c=Bank;active=true;
  Event(1,"EFFECT_ATTRIBUTION199C_READY","default_off=1 capture_lease_only=1 native_packet_writes=0 owner_authority=0 fire_attribution_unproved=1");
 }catch(const std::exception& e){Disable();Event(1,"EFFECT_ATTRIBUTION199C_REFUSED",e.what());}
 catch(...){Disable();Event(1,"EFFECT_ATTRIBUTION199C_REFUSED","unknown_enable_failure");}
}
}
