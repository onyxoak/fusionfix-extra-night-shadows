// Opt-in F8-leased numeric pole fields from the existing Caster callback.
// No hook, gameplay setting, draw, return decision or game-memory write.
#include "core/PoleCasterSnapshot134.hpp"
#include "core/PoleChildrenSnapshot135.hpp"
namespace PoleDrawWitness145 {template<class Map>void HintPole(const pole_caster134::Snapshot&,const Map&)noexcept;}
namespace PoleCasterDiagnostic134 {
bool requested=false;
struct Binding {uint32_t key{},slot{},kind{},frame{},clip{},thread{};bool valid{};};
thread_local Binding binding;
thread_local uint32_t budgetClip{},window{},used{},total{};
struct Preserve {DWORD error=GetLastError();fusionfix::shadows::FloatingPointState fp;~Preserve() noexcept{SetLastError(error);}};
class Scope {
 Binding previous;
public:
 explicit Scope(void* object) noexcept:previous(binding){
  binding={};
  if(!requested||!CaptureDiagnostic44::sampleSources.load(std::memory_order_relaxed)||!ShadowTrace34::enabled.load(std::memory_order_relaxed))return;
  const Preserve preserve;
  const uint32_t now=GetTickCount(),clip=CaptureDiagnostic44::lastStart;
  const auto thread=GetCurrentThreadId();uint8_t deferred{};uint32_t slot{};std::array<uint8_t,19> map{};
  if(!object||uint32_t(now-clip)>=10000u||!CShadows::pFrameCounter||thread!=PlayerShadowAllocation::ownerThread.load(std::memory_order_relaxed)||
     !Liberty20::Copy(game+0x1193312,&deferred,1)||deferred||!Liberty20::Copy(uintptr_t(object)+0x940,&slot,4)||slot<1||slot>7||
     !Liberty20::Copy(game+0xd9f1ed+slot*0x110,map.data(),map.size())||map[0]!=1)return;
  uint32_t key{},kind{};std::memcpy(&key,map.data()+11,4);std::memcpy(&kind,map.data()+15,4);
  if(!key||key==UINT32_MAX||(kind!=3&&kind!=4&&kind!=5))return;
  binding={key,slot,kind,*CShadows::pFrameCounter,clip,thread,true};
 }
 ~Scope() noexcept{binding=previous;}
 Scope(const Scope&)=delete;Scope& operator=(const Scope&)=delete;
};
void Observe(uintptr_t entity,uint32_t type,bool artificial,bool excluded) noexcept {
 if(!requested||type!=4||!binding.valid||!CaptureDiagnostic44::sampleSources.load(std::memory_order_relaxed)||!ShadowTrace34::enabled.load(std::memory_order_relaxed))return;
 const Preserve preserve;const auto now=GetTickCount();
 if(entity>UINT32_MAX||binding.thread!=GetCurrentThreadId()||binding.thread!=PlayerShadowAllocation::ownerThread.load(std::memory_order_relaxed)||
    binding.clip!=CaptureDiagnostic44::lastStart||uint32_t(now-binding.clip)>=10000u||!CShadows::pFrameCounter||binding.frame!=*CShadows::pFrameCounter)return;
 if(budgetClip!=binding.clip){budgetClip=binding.clip;window=now;used=total=0;}
 if(uint32_t(now-window)>=128u){window=now;used=0;}
 if(used>=8||total>=128)return;++used; // Read attempts, not only publications, are bounded.
 pole_caster134::Snapshot value{};
 const auto frame=[]() noexcept {uint32_t v{};return CShadows::pFrameCounter&&Liberty20::Copy(uintptr_t(CShadows::pFrameCounter),&v,4)?v:UINT32_MAX;};
 const auto read=[](uint64_t at,void* p,size_t n) noexcept {return pole_caster134::Address(at,n)&&Liberty20::Copy(uintptr_t(at),p,n);};
 if(!pole_caster134::Capture(read,frame,uint32_t(game),uint32_t(entity),binding.frame,value))return;
 pole_children135::Snapshot children{};
 const bool childrenValid=pole_children135::Capture(read,value.fragment8,children);
 pole_caster134::Snapshot finalBinding{};
 if(!pole_caster134::Capture(read,frame,uint32_t(game),uint32_t(entity),binding.frame,finalBinding)||
    std::memcmp(&value,&finalBinding,sizeof(value))!=0)return;
 // Recheck the actual selected map after the object reads; no stale map join.
 std::array<uint8_t,19> map{};uint32_t key{},kind{};
 if(!Liberty20::Copy(game+0xd9f1ed+binding.slot*0x110,map.data(),map.size())||map[0]!=1)return;
 std::memcpy(&key,map.data()+11,4);std::memcpy(&kind,map.data()+15,4);
 if(key!=binding.key||kind!=binding.kind||frame()!=binding.frame||!CaptureDiagnostic44::sampleSources.load(std::memory_order_relaxed))return;
 ++total;
 PoleDrawWitness145::HintPole(value,binding);
 const auto emit=[&](uint32_t stage,uint32_t a,uint32_t b,uint32_t c,uint32_t d) noexcept {
  ShadowTrace34::Emit({stage,binding.frame,now,value.entity,int32_t(a),int32_t(b),int32_t(c),int32_t(d),0,0,0,0});
 };
 emit(140,binding.key,binding.slot,binding.kind,(artificial?1u:0u)|(excluded?2u:0u));
 emit(141,value.modelIndex,value.modelHash,value.modelInfo,value.vtable);
 emit(142,value.flags210,value.associated1bc,value.associated28,value.instance280);
 emit(143,value.pool,value.slot,value.reference,binding.thread);
 emit(144,value.flags24,value.type28,value.renderInstance34,value.fragment8);
 emit(147,value.physical38,value.flags24,value.renderInstance34,value.fragment8);
 if(childrenValid){emit(145,children.root,children.children,children.alternate,children.count);
  for(uint32_t i=0;i<children.count;++i){const auto& c=children.child[i];emit(146,c.address,c.drawable90,c.drawable94,(i<<24)|(c.flags0c&0x00ffffffu));}
 }

}
}
