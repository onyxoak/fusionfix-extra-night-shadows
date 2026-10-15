// Repair only authenticated relevant static pole admission; retain native MODE0.
// Native6E3310 keeps room tests, virtual+28, actual object virtual+8C and child poses.
#include "core/PoleCasterSnapshot134.hpp"
#include "core/PoleSnapshotBatch144.hpp"
#include "core/PoleModeContract141.hpp"
#include "ImmediateRegion33.hpp"
#include "FreshPageReadable.hpp"
#include "PoleVolume142Runtime.inc"
namespace PoleStaticPartition141 {
struct Binding {uint32_t key{},slot{},kind{},frame{},thread{};bool valid{};PoleVolume142::Volume volume{};};
thread_local Binding binding;
bool requested=false,prepared=false;
struct Preserve {DWORD error=GetLastError();fusionfix::shadows::FloatingPointState fp;~Preserve() noexcept{SetLastError(error);}};
void Prepare(const std::vector<uint8_t>& image) noexcept {
 if(!requested){prepared=false;return;} // Deferred unaccepted pole override is internal opt-in only.
 const Preserve preserve;
 prepared=pole_mode141::Validate(image,uint32_t(game),[](uint64_t at,void* out,size_t n)noexcept{return pole_caster134::Address(at,n)&&Liberty20::Copy(uintptr_t(at),out,n);});
 Event(1,prepared?"POLE_PARTITION141_PREPARED":"POLE_PARTITION141_REFUSED",prepared?"native_MODE0=preserved argument0=preserved selected_light_volume=1 extra_maps=0":"native_MODE_branch_body_mismatch repair_disabled=1");
}
class Scope {
 Binding previous;
public:
 explicit Scope(void* object) noexcept:previous(binding){
  binding={};if(!requested||!prepared||!addonActive.load(std::memory_order_acquire)||!FusionBackend130::prepared||!FusionBackend130::lease.Owns())return;
  const Preserve preserve;
  const traffic_signal::fresh_page::Operation44 pages(32);const immediate_region33::Scope regions;
  if(!ff||reinterpret_cast<uintptr_t>(FusionBackend130::pinnedModule)!=ff||reinterpret_cast<uintptr_t>(GetModuleHandleW(L"GTAIV.EFLC.FusionFix.asi"))!=ff)return;
  const auto thread=GetCurrentThreadId();uint8_t deferred{};uint32_t slot{},frame{};std::array<uint8_t,19> map{};
  if(!object||!CShadows::pFrameCounter||thread!=PlayerShadowAllocation::ownerThread.load(std::memory_order_relaxed)||
   !Liberty20::Copy(uintptr_t(CShadows::pFrameCounter),&frame,4)||!Liberty20::Copy(game+0x1193312,&deferred,1)||deferred||
   !Liberty20::Copy(uintptr_t(object)+0x940,&slot,4)||slot<1||slot>7||!Liberty20::Copy(game+0xd9f1ed+slot*0x110,map.data(),map.size())||map[0]!=1)return;
  uint32_t key{},kind{};std::memcpy(&key,map.data()+11,4);std::memcpy(&kind,map.data()+15,4);
  if(!key||key==UINT32_MAX||(kind!=3&&kind!=4&&kind!=5))return;
  PoleVolume142::Volume volume;const auto read=[](uint64_t at,void* out,size_t n) noexcept{return pole_caster134::Address(at,n)&&Liberty20::Copy(uintptr_t(at),out,n);};
  if(!PoleVolume142::Capture(read,uint32_t(game),key,volume))return;
  uint32_t finalFrame{};std::array<uint8_t,19> finalMap{};
  if(!Liberty20::Copy(uintptr_t(CShadows::pFrameCounter),&finalFrame,4)||finalFrame!=frame||!Liberty20::Copy(game+0xd9f1ed+slot*0x110,finalMap.data(),19)||finalMap!=map)return;
  binding={key,slot,kind,frame,thread,true,volume};
 }
 ~Scope() noexcept{binding=previous;}
 Scope(const Scope&)=delete;Scope& operator=(const Scope&)=delete;
};
bool Reclassify(uintptr_t entity,uint32_t type,uint32_t mode,uintptr_t stackFrame,bool excluded,uint32_t* eligibility=nullptr) noexcept {
 if(!requested||!prepared||excluded||type!=4||mode!=0||!binding.valid||!addonActive.load(std::memory_order_acquire))return false;
 const Preserve preserve;
 const traffic_signal::fresh_page::Operation44 pages(32);
 const immediate_region33::Scope validation; // One synchronous SEH-contained read scope; ends before native drawing.
 if(binding.thread!=GetCurrentThreadId()||binding.thread!=PlayerShadowAllocation::ownerThread.load(std::memory_order_relaxed)||
  !FusionBackend130::prepared||!FusionBackend130::lease.Owns()||reinterpret_cast<uintptr_t>(FusionBackend130::pinnedModule)!=ff)return false;
 const auto frame=[]() noexcept {uint32_t v{};return CShadows::pFrameCounter&&Liberty20::Copy(uintptr_t(CShadows::pFrameCounter),&v,4)?v:UINT32_MAX;};
 const auto read=[](uint64_t at,void* p,size_t n) noexcept {return pole_caster134::Address(at,n)&&Liberty20::Copy(uintptr_t(at),p,n);};
 if(entity>UINT32_MAX||!pole_caster134::Address(uint64_t(stackFrame)+0x14,4))return false;
 //160 Reorder the EXISTING refusal-only three model reads before module/frame/
 //stack work. Same WorkingSet/SEH reader and per-operation permission scope;
 //no additional copies, radius/cone culling, or cached source authorization.
 // Model hints only reject unrelated props cheaply; full occupied-pool/model
 // authority is still freshly captured and repeated below for every actual fix.
 int16_t modelHint{};uint32_t infoHint{},hashHint{};
 if(!read(uint64_t(entity)+0x2e,&modelHint,2)||modelHint<0||modelHint>=31000||
  !read(uint64_t(game)+0xe95cd8u+uint32_t(modelHint)*4u,&infoHint,4)||
  !read(uint64_t(infoHint)+0x3c,&hashHint,4)||!pole_caster134::SupportedPole(hashHint))return false;
 if(reinterpret_cast<uintptr_t>(GetModuleHandleW(L"GTAIV.EFLC.FusionFix.asi"))!=ff||frame()!=binding.frame)return false;
 uint32_t actualMode{};if(!read(uint64_t(stackFrame)+0x14,&actualMode,4)||actualMode!=0)return false;
 uint32_t pose{};std::array<float,16> matrix{};
 if(!PoleVolume142::Touches(read,uint32_t(entity),binding.volume,pose,matrix))return false;
 pole_caster134::Snapshot value{};
 if(!pole_batch144::Capture(read,frame,uint32_t(game),uint32_t(entity),binding.frame,value)||
  value.vtable!=uint32_t(game)+0xaab0dcu||(value.flags210&(0x100000u|0x800u))||value.associated1bc||value.instance280||
  !(value.flags24&0x4000000u)||!value.physical38||!value.renderInstance34||!value.fragment8)return false;
 uint32_t predicate{};std::array<uint8_t,3> code{};
 if(!read(uint64_t(value.vtable)+0x28,&predicate,4)||predicate!=uint32_t(game)+0x16d0u||!read(predicate,code.data(),code.size())||
   code!=std::array<uint8_t,3>{0x32,0xc0,0xc3})return false;
 pole_caster134::Snapshot finalValue{};
 if(!pole_batch144::Capture(read,frame,uint32_t(game),uint32_t(entity),binding.frame,finalValue)||std::memcmp(&value,&finalValue,sizeof(value)))return false;
 // Reauthenticate exact predicate/stack/frame/selected map after all object reads.
 uint32_t againPredicate{},againMode{};std::array<uint8_t,3> againCode{};std::array<uint8_t,19> map{};uint32_t key{},kind{};uint8_t deferred{};
 if(!read(uint64_t(value.vtable)+0x28,&againPredicate,4)||againPredicate!=predicate||!read(predicate,againCode.data(),againCode.size())||againCode!=code||
  !read(uint64_t(stackFrame)+0x14,&againMode,4)||againMode!=actualMode||!read(uint64_t(game)+0x1193312u,&deferred,1)||deferred||
  !read(uint64_t(game)+0xd9f1edu+binding.slot*0x110u,map.data(),map.size())||map[0]!=1||frame()!=binding.frame)return false;
 std::memcpy(&key,map.data()+11,4);std::memcpy(&kind,map.data()+15,4);if(key!=binding.key||kind!=binding.kind)return false;
 uint32_t againPose{};std::array<float,16> againMatrix{};
 if(!PoleVolume142::Current(read,uint32_t(game),binding.volume)||!read(uint64_t(entity)+0x20,&againPose,4)||againPose!=pose||!read(pose,againMatrix.data(),64)||againMatrix!=matrix||frame()!=binding.frame)return false;
 ShadowTrace34::Emit({148,binding.frame,GetTickCount(),value.entity,0,0,int32_t(binding.slot),int32_t(binding.kind),0,0,0,0});
 if(eligibility)*eligibility=((value.flags210>>11)&0xffffff01u)|1u;
 return true; // Resume native local flag store at3767. EDX/stack MODE and entity flags unchanged.
}
}


