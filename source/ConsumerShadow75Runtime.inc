// Independent, default-off read-only per-light application diagnostics.
#include "core/ConsumerShadow75.hpp"
#include "core/ConsumerGuard80.hpp"
namespace ConsumerShadow75 {
bool requested=false,prepared=false;
std::atomic<bool> active{false};
SafetyHookMid lookupHook,gateHook,dispatchHook;
thread_local consumer_shadow75::Context context;
thread_local bool busy=false;
consumer_shadow75::FrameBudget budget,unkeyedBudget;
std::atomic<std::uint32_t> completed{0},refused{0},limited{0},sizeRejected{0};
void Emit(const consumer_shadow75::Sample& s) noexcept {
 ++completed;if(s.available && !s.sizeGate)++sizeRejected;
 ShadowTrace34::Emit({80,s.frame,s.tick,s.key,s.lookup,int(s.type),int(s.flags),int(s.mode),0,0,0,0});
 ShadowTrace34::Emit({81,s.frame,s.tick,s.key,int(s.technique),int(s.effectiveType),int(s.available),int(s.sizeGate),s.source[0],s.source[1],s.source[2],s.radius});
 ShadowTrace34::Emit({82,s.frame,s.tick,s.key,0,0,0,0,s.camera[0],s.camera[1],s.camera[2],s.cameraScale});
 ShadowTrace34::Emit({83,s.frame,s.tick,s.key,int(s.finite),int(s.reproduced),int(s.available),0,s.distance,s.ratio,s.threshold,s.globalScale});
 ShadowTrace34::Emit({84,s.frame,s.tick,s.key,0,0,0,0,s.gateSource[0],s.gateSource[1],s.gateSource[2],s.gateRadius});
 ShadowTrace34::Emit({85,s.frame,s.tick,s.key,0,0,0,0,s.recomputedRatio,s.recomputedThreshold,0,0});
}
void Observe(consumer_shadow75::Stage stage,SafetyHookContext& regs) noexcept {
 // Disabled diagnostics perform only atomic/integer checks and clear our
 // private context. Avoid saving/resetting FP state for every rendered light.
 if(!active.load(std::memory_order_acquire)||!addonActive.load(std::memory_order_acquire)||
  !CaptureDiagnostic44::sampleSources.load(std::memory_order_acquire)||
  !ShadowTrace34::enabled.load(std::memory_order_acquire)){context={};return;}
 // SafetyHook saves GPR/EFLAGS/XMM; active probing also preserves x87,
 // MXCSR and LastError, including every fault/reentrancy return below.
 const ModelEffectObserver::LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 if(busy)return;
 busy=true;struct Leave {~Leave(){busy=false;}} leave;
 // Diagnostic callback has no native call/write; each new stage starts fresh.
 const traffic_signal::fresh_page::Operation44 pages157(32);
 const immediate_region33::Scope regions157;
 const auto read=[](uintptr_t address,void* output,size_t size) noexcept {return Liberty20::Copy(address,output,size);};
 uint32_t frame{};
 if(!CShadows::pFrameCounter||!read(reinterpret_cast<uintptr_t>(CShadows::pFrameCounter),&frame,4)){context={};++refused;return;}
 if(stage==consumer_shadow75::Stage::Lookup){
  context={};uint32_t key{};
  if(regs.edi<0x10028 || regs.edi>UINT32_MAX-0x3c || !read(uintptr_t(regs.edi)+0x38,&key,4))return;
  if(key ? !ShadowTrace34::Tracked(key) : false)return;
  if(!(key ? budget.Claim(frame) : unkeyedBudget.Claim(frame,4))){++limited;return;}
 }else if(!context.pending||(context.sample.key && !ShadowTrace34::Tracked(context.sample.key))){context={};return;}
 const consumer_shadow75::Registers values{regs.esp,regs.edi,regs.eax,regs.esi,regs.xmm0.f32[0],regs.xmm1.f32[0]};
 if(!consumer_shadow75::Probe(context,stage,values,frame,GetTickCount(),game+0xc3efdc,read,Emit))++refused;
}
void Lookup(SafetyHookContext& r) noexcept {ConsumerReach136::Observe(consumer_shadow75::Stage::Lookup,r);Observe(consumer_shadow75::Stage::Lookup,r);}
void Gate(SafetyHookContext& r) noexcept {Observe(consumer_shadow75::Stage::Gate,r);ConsumerReach136::Observe(consumer_shadow75::Stage::Gate,r);}
void Dispatch(SafetyHookContext& r) noexcept {Observe(consumer_shadow75::Stage::Dispatch,r);}
void Disable() noexcept {active=false;for(auto* h:{&lookupHook,&gateHook,&dispatchHook})if(*h)(void)h->disable();prepared=false;context={};}
void PrepareHooks() noexcept {
 if(!requested)return;
 try {
  constexpr uint32_t begin=0x6c1030,length=0x5f5;
  const auto live=ReadMemory(game+begin,length);
  Require(original && begin+length<=original->bytes.size(),"consumer80_original_body_missing");
  bool knownFF511=false;
  if(const auto ff=GetModuleHandleW(L"GTAIV.EFLC.FusionFix.asi")){
   const auto raw=pe::Read(pe::ModulePath(reinterpret_cast<HMODULE>(ff)));
   knownFF511=pe::Hash(raw)==Layout511.hash;
  }
  const auto body=consumer_guard80::Verify(
   std::span<const uint8_t>(original->bytes.data()+begin,length),live,knownFF511);
  Require(body!=consumer_guard80::Body::Refused,"consumer80_full_native_body_changed");
  Event(1,"CONSUMER80_BODY_AUTHENTICATED",body==consumer_guard80::Body::FusionFix511?
   "official_ff511_tessellation_overlay=1 diagnostic_only=1":"pristine=1 diagnostic_only=1");
  Require(OriginalCall(0x6c1237)==game+0x525db0 && OriginalCall(0x6c124a)==game+0x5268b0 &&
   OriginalCall(0x6c15ce)==game+0x6c09a0,"consumer75_call_targets_changed");
  constexpr uint8_t lookup[]{0x83,0xc4,0x0c,0x83,0xf8,0xff};
  constexpr uint8_t dispatch[]{0x57,0xe8,0xcd,0xf3,0xff,0xff};
  Require(!memcmp(live.data()+0x20c,lookup,sizeof(lookup))&&
   !memcmp(live.data()+0x59d,dispatch,sizeof(dispatch)),"consumer75_site_abi_changed");
  Prepare(lookupHook,game+0x6c123c,Lookup);Prepare(gateHook,game+0x6c14ac,Gate);Prepare(dispatchHook,game+0x6c15cd,Dispatch);
  Require(lookupHook.original_bytes().size()==6 && gateHook.original_bytes().size()==7 &&
   dispatchHook.original_bytes().size()==6,"consumer75_stolen_span_changed");
  prepared=true;
 }catch(const std::exception& e){Disable();Event(1,"CONSUMER75_REFUSED",e.what());}
 catch(...){Disable();Event(1,"CONSUMER75_REFUSED","unknown_prepare_failure");}
}
void Enable() noexcept {
 if(!requested||!prepared)return;
 try {Require(bool(lookupHook.enable())&&bool(gateHook.enable())&&bool(dispatchHook.enable()),"consumer75_enable_failed");
  active=true;Event(1,"CONSUMER75_ACTIVE",ConsumerReach136::requested?"static_reach136=1 read_only=0 static_gate_distance_scale=1.30 stages=80_85_160 native_map_allocation_unchanged=1":"read_only=1 capture_lease_only=1 maximum_tracked_samples_per_frame=16 maximum_unkeyed_samples_per_frame=4 stages=80_85 native_changes=0");
 }catch(const std::exception& e){Disable();Event(1,"CONSUMER75_REFUSED",e.what());}
 catch(...){Disable();Event(1,"CONSUMER75_REFUSED","unknown_enable_failure");}
}
void Publish() {
 if(!requested)return;
 static uint32_t prior{};const auto now=GetTickCount();if(now-prior<5000)return;prior=now;
 Event(1,"CONSUMER75_COUNTS","requested="+std::to_string(requested)+" prepared="+std::to_string(prepared)+" active="+std::to_string(active.load())+" completed="+std::to_string(completed.load())+" refused="+std::to_string(refused.load())+
  " limited="+std::to_string(limited.load())+" size_rejected="+std::to_string(sizeRejected.load())+" diagnostic_not_clean_timing=1");
}
}


