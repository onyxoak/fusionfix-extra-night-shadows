// Separate opt-in paired-shader experiment. Never enable with ordinary shaders.
#include "core/SceneryFade198B.hpp"
namespace SceneryFade198B {
inline bool requested=false,prepared=false;
inline std::atomic<bool> active=false;
inline SafetyHookMid binderHook;
inline thread_local bool busy=false;
__declspec(noinline) bool WriteFloat(uintptr_t address,float value) noexcept {
 MEMORY_BASIC_INFORMATION m{};
 if(!VirtualQuery(reinterpret_cast<void*>(address),&m,sizeof(m))||m.State!=MEM_COMMIT||
    (m.Protect&(PAGE_GUARD|PAGE_NOACCESS))||
    !(m.Protect&(PAGE_READWRITE|PAGE_WRITECOPY|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY))||
    address<reinterpret_cast<uintptr_t>(m.BaseAddress)||
    address-reinterpret_cast<uintptr_t>(m.BaseAddress)>m.RegionSize||
    m.RegionSize-(address-reinterpret_cast<uintptr_t>(m.BaseAddress))<4)return false;
 __try{*reinterpret_cast<volatile float*>(address)=value;return true;}
 __except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION||GetExceptionCode()==EXCEPTION_GUARD_PAGE?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){return false;}
}
void Binder(SafetyHookContext& regs) noexcept {
 if(!active.load(std::memory_order_acquire)||!addonActive.load(std::memory_order_acquire)||!ConsumerReach136::requested||busy)return;
 const ModelEffectObserver::LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 busy=true;struct Leave {~Leave(){busy=false;}} leave;
 const auto context=ConsumerReach136::context;
 if(!context.pending||!CShadows::pFrameCounter)return;
 const auto read=[](uintptr_t p,void* out,size_t size) noexcept {return Liberty20::Copy(p,out,size);};
 uint32_t frame{};if(!read(reinterpret_cast<uintptr_t>(CShadows::pFrameCounter),&frame,4))return;
 const auto authorize=[&](const consumer_shadow75::Packet& p,uint32_t f) noexcept {
  return ConsumerReach136::Authorize(p,f);
 };
 const auto write=[&](uintptr_t address,float value) noexcept {
  const auto& current=ConsumerReach136::context;uint32_t now{};
  if(!current.pending||current.anchor!=context.anchor||current.source!=context.source||current.frame!=frame||
     std::memcmp(&current.packet,&context.packet,sizeof(context.packet))||
     !read(reinterpret_cast<uintptr_t>(CShadows::pFrameCounter),&now,4)||now!=frame||!addonActive.load(std::memory_order_acquire))return false;
  return WriteFloat(address,value);
 };
 (void)scenery_fade198::Apply(context,{regs.esp,regs.ebp,regs.esi},game,frame,read,authorize,write);
}
void Disable() noexcept {active=false;if(binderHook)(void)binderHook.disable();prepared=false;}
void PrepareHooks(bool consumerPrepared) noexcept {
 if(!requested)return;
 try {
  Require(consumerPrepared&&ConsumerReach136::requested,"fade198_consumer_prerequisite");
  constexpr uint32_t begin=0x5268b0,length=0x264;
  const auto live=ReadMemory(game+begin,length);
  Require(original&&begin+length<=original->bytes.size()&&!memcmp(live.data(),original->bytes.data()+begin,length),"fade198_binder_body_changed");
  Require(OriginalCall(0x526ad9)==game+0x524a40,"fade198_binder_call_changed");
  // Both immediate/queued submission paths consume the local vector; the
  // queued constructor copies all four DWORDs before its RET0xC. Bind that
  // lifetime proof to the exact current wrapper+constructor body as well.
  constexpr uint32_t copyBegin=0x524a40,copyLength=0xb3;
  const auto copyLive=ReadMemory(game+copyBegin,copyLength);
  Require(copyBegin+copyLength<=original->bytes.size()&&!memcmp(copyLive.data(),original->bytes.data()+copyBegin,copyLength),"fade198_vector_copy_changed");
  const auto shader=pe::ModulePath(GetModuleHandleW(nullptr)).parent_path()/L"update/common/shaders/win32_30/deferred_lighting.fxc";
  Require(pe::Hash(pe::Read(shader))=="2c2c8711e477d0ba2ceb9bdc493fc058f8f0bb3c0d497c8338d0e0e677631cfa","fade198_paired_shader_missing");
  Prepare(binderHook,game+0x526ad9,Binder);
  Require(binderHook.original_bytes().size()==5,"fade198_stolen_span_changed");
  prepared=true;
 }catch(const std::exception& e){Disable();Event(1,"SCENERY_FADE198_REFUSED",e.what());}
 catch(...){Disable();Event(1,"SCENERY_FADE198_REFUSED","unknown_prepare_failure");}
}
void Enable(bool consumerActive) noexcept {
 if(!requested||!prepared)return;
 try {Require(consumerActive,"fade198_consumer_inactive");Require(bool(binderHook.enable()),"fade198_enable_failed");active=true;
  Event(1,"SCENERY_FADE198_ACTIVE","paired_shader=1 original_light_unchanged=1 static_distance_blend=1.10_to_1.30 map_replacement_fade=0 runtime_pixels_unverified=1");
 }catch(const std::exception& e){Disable();Event(1,"SCENERY_FADE198_REFUSED",e.what());}
 catch(...){Disable();Event(1,"SCENERY_FADE198_REFUSED","unknown_enable_failure");}
}
}
