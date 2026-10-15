// Functional opt-in path; shares the existing authenticated consumer75 sites.
#include "core/ConsumerReach136.hpp"
#include "FreshPageReadable.hpp"
#include "ImmediateRegion33.hpp"
#include <optional>
namespace ConsumerReach136 {
inline bool requested=false,retentionRequested198=false;
inline thread_local consumer_shadow75::Context context;
inline thread_local bool busy=false;
inline thread_local consumer_reach136::ApplicationHistory198 applicationHistory198;
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
 if(!requested){context={};applicationHistory198.Clear();return;}
 if(stage==consumer_shadow75::Stage::Lookup&&regs.eax==UINT32_MAX){context={};return;}
 const ModelEffectObserver::LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 if(!requested||!addonActive.load(std::memory_order_acquire)){context={};applicationHistory198.Clear();return;}
 if(!retentionRequested198)applicationHistory198.Clear();
 if(busy)return;
 busy=true;struct Leave {~Leave(){busy=false;}} leave;
 if(stage==consumer_shadow75::Stage::Gate&&(!context.pending||
    (!consumer_reach136::NumericPotential142(regs.xmm0.f32[0],regs.xmm1.f32[0])&&
     (!retentionRequested198||!consumer_reach136::NumericPotential142(regs.xmm0.f32[0],regs.xmm1.f32[0],consumer_reach136::ApplicationHistory198::ReleaseScale)||
      !applicationHistory198.Retained(context.packet,context.frame,GetTickCount()))))){context={};return;}
 // Fresh positive exact-page permission proof only within this callback.
 // Every byte/generation read remains fresh; no proof crosses a native call.
 std::optional<traffic_signal::fresh_page::Operation44> pages157;
 std::optional<immediate_region33::Scope> regions157;
 pages157.emplace(32);regions157.emplace();
 const auto read=[](uintptr_t p,void* out,size_t size) noexcept{return Liberty20::Copy(p,out,size);};
 uint32_t frame{};
 if(!CShadows::pFrameCounter||!read(reinterpret_cast<uintptr_t>(CShadows::pFrameCounter),&frame,4)){context={};return;}
 const consumer_shadow75::Registers r{regs.esp,regs.edi,regs.eax,regs.esi,regs.xmm0.f32[0],regs.xmm1.f32[0]};
 if(stage==consumer_shadow75::Stage::Lookup){
  (void)consumer_shadow75::Probe(context,stage,r,frame,GetTickCount(),game+0xc3efdc,read,
     [](const consumer_shadow75::Sample&) noexcept {});
  if(context.pending&&!consumer_reach136::PacketPotential142(context.packet))context={};
 }else if(stage==consumer_shadow75::Stage::Gate){
  const auto write157=[&](uintptr_t address,uint8_t value) noexcept {
   // Release borrowed approvals before the existing final stack decision write.
   regions157.reset();pages157.reset();return WriteByte(address,value);
  };
  if(consumer_reach136::Gate(true,context,r,frame,GetTickCount(),game+0xc3efdc,read,Authorize,write157,retentionRequested198?&applicationHistory198:nullptr))
   ShadowTrace34::Emit({160,frame,GetTickCount(),context.packet.words[24],int(context.sample.type),int(context.sample.flags),1,0,r.ratio,r.threshold,r.threshold/consumer_reach136::StaticShoulder162,consumer_reach136::StaticShoulder162});
  context={};
 }else context={};
}
}




