// Optional diagnostic only. No native packet/camera/owner reads or writes.
#include "core/NativeProducerGate116.hpp"
namespace NativeProducerGate116 {
bool requested=false,prepared=false;
std::atomic<bool> active{false};
SafetyHookMid planeHook,sphereHook;
thread_local native_producer_gate116::Context context;
thread_local bool busy=false;
native_producer_gate116::Budget budget;
std::atomic<std::uint32_t> sequence{0},firstObserved{0},sphereObserved{0},limited{0},bindingRejected{0};
void Emit(const native_producer_gate116::Event& e) noexcept {
 // Frame zero explicitly means no native-frame observation. The nonce is
 // diagnostic call association only, never a light key or emission identity.
 ShadowTrace34::Emit({e.stage,0,e.tick,e.nonce,e.a,e.b,e.c,e.d,e.x,e.y,e.z,e.w});
}
void Observe(bool first,SafetyHookContext& regs) noexcept {
 const ModelEffectObserver::LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 if(!active.load(std::memory_order_acquire)||!addonActive.load(std::memory_order_acquire)||
    !CaptureDiagnostic44::sampleSources.load(std::memory_order_acquire)||
    !ShadowTrace34::enabled.load(std::memory_order_acquire)){context={};return;}
 if(busy){context={};return;}
 busy=true;struct Leave{~Leave(){busy=false;}}leave;
 const auto tick=GetTickCount();
 native_producer_gate116::Registers r{};
 r.ebp=regs.ebp;r.esp=regs.esp;r.eflags=regs.eflags;r.eax=regs.eax;
 if(first){
  context={};if(regs.esi!=0 && regs.esi!=2)return;
  if(!budget.Claim(tick)){++limited;return;}
  r.type=regs.esi;r.flags=regs.ecx;
  r.source={regs.xmm6.f32[0],regs.xmm5.f32[0],regs.xmm4.f32[0]};
  r.radius=regs.xmm1.f32[0];r.planeDistance=regs.xmm2.f32[0];r.negativeRadius=regs.xmm0.f32[0];
  auto nonce=sequence.fetch_add(1,std::memory_order_relaxed)+1;
  if(!nonce)nonce=sequence.fetch_add(1,std::memory_order_relaxed)+1;
  if(native_producer_gate116::First(context,r,nonce,tick,Emit))++firstObserved;
 }else if(context.pending){
  if(native_producer_gate116::Second(context,r,tick,Emit))++sphereObserved;else ++bindingRejected;
 }
}
void Plane(SafetyHookContext& r) noexcept{Observe(true,r);}
void Sphere(SafetyHookContext& r) noexcept{Observe(false,r);}
void Disable() noexcept {
 active=false;for(auto* h:{&planeHook,&sphereHook})if(*h)(void)h->disable();prepared=false;context={};
}
void PrepareHooks() noexcept {
 if(!requested)return;
 try{
  constexpr auto begin=native_producer_gate116::BodyRva,length=native_producer_gate116::BodySize;
  Require(original && uint64_t(begin)+length<=original->bytes.size(),"producer116_original_body_missing");
  const auto live=ReadMemory(game+begin,length);
  Require(native_producer_gate116::Verify(std::span<const uint8_t>(original->bytes.data()+begin,length),live),"producer116_full_native_body_changed");
  Require(OriginalCall(0x6bd08c)==game+0x31e40,"producer116_sphere_call_changed");
  Prepare(planeHook,game+native_producer_gate116::FirstRva,Plane);
  Prepare(sphereHook,game+native_producer_gate116::SecondRva,Sphere);
  Require(planeHook.original_bytes().size()==6 && sphereHook.original_bytes().size()==6,"producer116_stolen_branch_span_changed");
  prepared=true;
 }catch(const std::exception& e){Disable();Event(1,"PRODUCER_GATE116_REFUSED",e.what());}
 catch(...){Disable();Event(1,"PRODUCER_GATE116_REFUSED","unknown_prepare_failure");}
}
void Enable() noexcept {
 if(!requested||!prepared)return;
 try{Require(bool(planeHook.enable())&&bool(sphereHook.enable()),"producer116_enable_failed");active=true;
  Event(1,"PRODUCER_GATE116_ACTIVE","diagnostic_only=1 capture_lease_only=1 register_reads_only=1 native_frame_unknown=1 maximum_calls_per_128ms=64 native_changes=0");
 }catch(const std::exception& e){Disable();Event(1,"PRODUCER_GATE116_REFUSED",e.what());}
 catch(...){Disable();Event(1,"PRODUCER_GATE116_REFUSED","unknown_enable_failure");}
}
void Publish(){
 if(!requested)return;
 static uint32_t prior{};const auto now=GetTickCount();if(now-prior<5000)return;prior=now;
 Event(1,"PRODUCER_GATE116_COUNTS","requested="+std::to_string(requested)+" prepared="+std::to_string(prepared)+" active="+std::to_string(active.load())+
  " plane_observed="+std::to_string(firstObserved.load())+" sphere_observed="+std::to_string(sphereObserved.load())+
  " limited="+std::to_string(limited.load())+" binding_rejected="+std::to_string(bindingRejected.load())+" borrowed_reads=0 os_permission_queries=0 frame_unknown=1");
}
}
