// Functional opt-in path; shares the existing authenticated consumer75 sites.
#include "core/ConsumerReach136.hpp"
namespace ConsumerReach136 {
inline bool requested=false;
inline thread_local consumer_shadow75::Context context;
inline thread_local bool busy=false;
bool Authorize(const consumer_shadow75::Packet&,uint32_t) noexcept;
__declspec(noinline) bool WriteByte(uintptr_t address,uint8_t value) noexcept {
 MEMORY_BASIC_INFORMATION m{};
 if(!VirtualQuery(reinterpret_cast<void*>(address),&m,sizeof(m))||m.State!=MEM_COMMIT||
    (m.Protect&(PAGE_GUARD|PAGE_NOACCESS))||
    !(m.Protect&(PAGE_READWRITE|PAGE_WRITECOPY|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY)))return false;
 __try{*reinterpret_cast<volatile uint8_t*>(address)=value;return true;}
 __except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION||GetExceptionCode()==EXCEPTION_GUARD_PAGE?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){return false;}
}
void Observe(consumer_shadow75::Stage stage,SafetyHookContext& regs) noexcept {
 if(!requested){context={};return;}
 if(stage==consumer_shadow75::Stage::Lookup&&regs.eax==UINT32_MAX){context={};return;}
 const ModelEffectObserver::LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 if(!requested||!addonActive.load(std::memory_order_acquire)){context={};return;}
 if(busy)return;
 busy=true;struct Leave {~Leave(){busy=false;}} leave;
 if(stage==consumer_shadow75::Stage::Gate&&(!context.pending||
    !consumer_reach136::NumericPotential142(regs.xmm0.f32[0],regs.xmm1.f32[0]))){context={};return;}
 const auto read=[](uintptr_t p,void* out,size_t size) noexcept{return Liberty20::Copy(p,out,size);};
 uint32_t frame{};
 if(!CShadows::pFrameCounter||!read(reinterpret_cast<uintptr_t>(CShadows::pFrameCounter),&frame,4)){context={};return;}
 const consumer_shadow75::Registers r{regs.esp,regs.edi,regs.eax,regs.esi,regs.xmm0.f32[0],regs.xmm1.f32[0]};
 if(stage==consumer_shadow75::Stage::Lookup){
  (void)consumer_shadow75::Probe(context,stage,r,frame,GetTickCount(),game+0xc3efdc,read,
     [](const consumer_shadow75::Sample&) noexcept {});
  if(context.pending&&!consumer_reach136::PacketPotential142(context.packet))context={};
 }else if(stage==consumer_shadow75::Stage::Gate){
  if(consumer_reach136::Gate(true,context,r,frame,GetTickCount(),game+0xc3efdc,read,Authorize,WriteByte))
   ShadowTrace34::Emit({160,frame,GetTickCount(),context.packet.words[24],int(context.sample.type),int(context.sample.flags),1,0,r.ratio,r.threshold,r.threshold/1.15f,1.15f});
  context={};
 }else context={};
}
}




