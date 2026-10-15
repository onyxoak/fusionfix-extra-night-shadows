// Private CE geometry experiment, traced from the authenticated native image.
// Keep native per-light frustum, room filtering, depth surfaces and techniques.
// No cache reuse for moving static geometry and no additional lighting packet.
#include "WorldCasterMask20.hpp"
namespace WorldGeometry20 {
bool requested=false,prepared=false;
std::atomic<bool> active{false};
SafetyHookMid collection,draw,mask;
thread_local bool dynamicScope=false;
std::atomic<uint32_t> collections{0},draws{0},staticMasks{0},deferredRefused{0},invalidRefused{0};
std::atomic<uint32_t> sampledLists{0},class7Entities{0};
std::atomic<uint32_t> resolvedStaticMasks{0},resolvedMissingStaticMasks{0};
std::array<std::atomic<uint32_t>,16> sampledTypes{};
std::array<std::atomic<uint32_t>,8> bucketVisits{},bucketEntities{};
std::array<std::array<std::atomic<uint32_t>,16>,8> bucketTypes{};
bool DynamicPass(uintptr_t object) noexcept {
 if(!object)return false;
 uint32_t slot{},key{},kind{};
 if(!Liberty20::Copy(object+0x940,&slot,4)||slot<1||slot>7)return false;
 const auto offset=slot*0x110u;
 // These selected/key/kind fields share one native map record. Copy their
 // current contiguous span once instead of querying each scalar separately.
 std::array<uint8_t,19> record{};
 if(!Liberty20::Copy(game+0xd9f1ed+offset,record.data(),record.size())||record[0]!=1)return false;
 std::memcpy(&key,record.data()+11,4);std::memcpy(&kind,record.data()+15,4);
 if(kind!=4||!key||key==UINT32_MAX)return false;
 return true;
}
void IncludeStaticMask(SafetyHookContext& regs) noexcept {
 const fusionfix::shadows::FloatingPointState fp;
 if(!active.load(std::memory_order_acquire)||!addonActive.load(std::memory_order_acquire))return;
 uint8_t deferred{};
 if(!Liberty20::Copy(game+0x1193312,&deferred,1)||deferred){++deferredRefused;return;}
 // At9784AE native EAX is the completed desired mask, before its exact
 // six-byte MOV to this view. Changing only EAX avoids unrelated views.
 if(!DynamicPass(regs.edi)||regs.eax!=world_caster_mask20::NativeDynamicMask)return;
 regs.eax=world_caster_mask20::IncludeStatic(regs.eax);++staticMasks;
}
class Scope {
 bool previous;
public:
 explicit Scope(void* object) noexcept:previous(dynamicScope){
  dynamicScope=false;
  if(!active.load(std::memory_order_acquire)||!addonActive.load(std::memory_order_acquire))return;
  uint8_t deferred{};
  if(!Liberty20::Copy(game+0x1193312,&deferred,1)||deferred){++deferredRefused;return;}
  dynamicScope=DynamicPass(reinterpret_cast<uintptr_t>(object));
  if(dynamicScope&&hotCountersEnabled){uint32_t resolved{};if(Liberty20::Copy(reinterpret_cast<uintptr_t>(object)+0x8e8,&resolved,4)){
   if(resolved&0x80u)++resolvedStaticMasks;else ++resolvedMissingStaticMasks;
  }}
 }
 ~Scope(){dynamicScope=previous;}
 Scope(const Scope&)=delete;
 Scope& operator=(const Scope&)=delete;
};
void Collect(SafetyHookContext& regs) noexcept {
 const fusionfix::shadows::FloatingPointState fp;
 if(!active.load(std::memory_order_acquire)||!addonActive.load(std::memory_order_acquire))return;
 // Native 978338 just wrote 0x0100 for a kind-4 light. Include the native
 // static collection byte as kind-5 does, keeping the dynamic byte intact.
 uint16_t flags{};
 if(!DynamicPass(regs.esi)||!Liberty20::Copy(regs.esi+0x15,&flags,2)||flags!=0x100){++invalidRefused;return;}
 *reinterpret_cast<uint16_t*>(regs.esi+0x15)=0x101;
 ++collections;
}
void Draw(SafetyHookContext& regs) noexcept {
 const fusionfix::shadows::FloatingPointState fp;
 if(!active.load(std::memory_order_acquire)||!addonActive.load(std::memory_order_acquire)||!dynamicScope||regs.ebx!=UINT32_MAX)return;
 // Preserve EBX=-1: the existing dynamic shader and mask remain authoritative.
 // Enter the native class-7 static-geometry call already used by cached maps.
 // FF's separate vehicle technique hook at 6E4236 stays untouched.
 regs.eip=game+0x6e426a;
 const auto visit=draws.fetch_add(1,std::memory_order_relaxed);
 if(!hotCountersEnabled||visit%120u)return;
 if(regs.ebp>=32){++invalidRefused;return;}
 const auto header=game+0x1214c90+regs.ebp*0x54;
 ++sampledLists;
 // The native scene has eight lists. Class 7 is a model-dependent special
 // path, not an exhaustive static building list. Read the same active native
 // pass's lists to identify actual building/prop buckets before adding draws.
 for(unsigned bucket=0;bucket<8;++bucket){
  uint32_t list{};uint16_t count{};
  if(!Liberty20::Copy(header+0x10+bucket*8,&list,4)||
     !Liberty20::Copy(header+0x14+bucket*8,&count,2)||count>4096||(!list&&count)){++invalidRefused;continue;}
  ++bucketVisits[bucket];bucketEntities[bucket]+=count;
  if(bucket==7)class7Entities+=count;
  for(unsigned i=0;i<std::min<unsigned>(count,32);++i){
   uint32_t entity{},type{};
   if(Liberty20::Copy(uintptr_t(list)+i*8,&entity,4)&&entity&&
      Liberty20::Copy(uintptr_t(entity)+0x28,&type,4)){
    const auto nativeType=(type>>6)&15;
    ++bucketTypes[bucket][nativeType];
    if(bucket==7)++sampledTypes[nativeType];
   }
  }
 }

}
void Disable() noexcept {
 active=false;dynamicScope=false;
 for(auto* hook:{&mask,&draw,&collection})if(*hook)(void)hook->disable();
 prepared=false;
}
void PrepareHooks() noexcept {
 if(!requested)return;
 try {
  // These ranges do not overlap the official FF vehicle/caster overlays.
  for(const auto& range:{std::pair<uint32_t,size_t>{0x9782e0,0xad},{0x978410,0xa4},{0x6e4265,0x15},{0x6e77f0,0x196}}){
   const auto actual=ReadMemory(game+range.first,range.second);
   Require(!memcmp(actual.data(),original->bytes.data()+range.first,range.second),"world_geometry_native_guard");
  }
  Require(OriginalCall(0x97835e)==game+0x7022e0&&OriginalCall(0x97837d)==game+0x71e880&&
          OriginalCall(0x6e4272)==game+0x6e3310,"world_geometry_call_target");
  constexpr uint8_t branch[]{0x83,0xfb,0xff,0x74,0x10};
  Require(!memcmp(original->bytes.data()+0x6e4265,branch,sizeof(branch)),"world_geometry_draw_branch");
  Prepare(collection,game+0x97833e,Collect);Prepare(draw,game+0x6e4265,Draw);
  constexpr uint8_t writeMask[]{0x89,0x87,0xd0,0x08,0x00,0x00};
  Require(!memcmp(original->bytes.data()+world_caster_mask20::MaskWriteRva,writeMask,sizeof(writeMask)),"world_geometry_mask_instruction");
  Prepare(mask,game+world_caster_mask20::MaskWriteRva,IncludeStaticMask);
  Require(mask.original_bytes().size()==sizeof(writeMask),"world_geometry_mask_stolen");
  Require(draw.original_bytes().size()==sizeof(branch),"world_geometry_branch_stolen");
  prepared=true;
 }catch(const std::exception& e){Disable();Event(1,"WORLD_GEOMETRY_REFUSED",e.what());}
 catch(...){Disable();Event(1,"WORLD_GEOMETRY_REFUSED","unknown_preparation_failure");}
}
void EnableHooks() noexcept {
 if(!requested||!prepared)return;
 try {
  Require(bool(draw.enable())&&bool(collection.enable())&&bool(mask.enable()),"world_geometry_enable");
  active=true;
  Event(1,"WORLD_GEOMETRY_ACTIVE","native_collection=static_and_dynamic draw_class=7 census_classes=8 static_building_view_mask=1 immediate_only=1 slots=7 visual_validation_pending=1");
 }catch(const std::exception& e){Disable();Event(1,"WORLD_GEOMETRY_REFUSED",e.what());}
 catch(...){Disable();Event(1,"WORLD_GEOMETRY_REFUSED","unknown_enable_failure");}
}
void Publish() {
 if(!requested)return;
 static uint32_t previous{};const auto now=GetTickCount();if(now-previous<5000)return;previous=now;
 Event(1,"WORLD_GEOMETRY_COUNTS","collections="+std::to_string(collections.load())+" draws="+std::to_string(draws.load())+" static_masks="+std::to_string(staticMasks.load())+
   " deferred_refused="+std::to_string(deferredRefused.load())+" invalid_refused="+std::to_string(invalidRefused.load())+
  " sampled_lists="+std::to_string(sampledLists.load())+" class7_entities="+std::to_string(class7Entities.load())+
   " resolved_static_masks="+std::to_string(resolvedStaticMasks.load())+" resolved_missing_static_masks="+std::to_string(resolvedMissingStaticMasks.load())+
   " sampled_buildings="+std::to_string(sampledTypes[1].load())+" sampled_vehicles="+std::to_string(sampledTypes[2].load())+
   " sampled_peds="+std::to_string(sampledTypes[3].load())+" sampled_objects="+std::to_string(sampledTypes[4].load())+
   " repeated_observations_not_receiver_proof=1");
 for(unsigned bucket=0;bucket<8;++bucket)
  Event(1,"WORLD_GEOMETRY_BUCKET","bucket="+std::to_string(bucket)+" sampled_lists="+std::to_string(bucketVisits[bucket].load())+
   " entity_entries="+std::to_string(bucketEntities[bucket].load())+
   " sampled_buildings="+std::to_string(bucketTypes[bucket][1].load())+
   " sampled_vehicles="+std::to_string(bucketTypes[bucket][2].load())+
   " sampled_peds="+std::to_string(bucketTypes[bucket][3].load())+
   " sampled_objects="+std::to_string(bucketTypes[bucket][4].load())+
   " repeated_observations_not_unique_or_receiver_proof=1");

}
}
