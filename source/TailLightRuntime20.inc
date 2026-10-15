// Included in standalone after LibertyLighting20Runtime.inc. GPL-3.0.
// Authenticated native rear illumination only; no fabricated offsets/colors.
#include "TailLightPolicy20.hpp"
namespace TailLight20 {
bool requested=false,prepared=false;
std::atomic<bool> active{false};
SafetyHookMid combinedHook,singleHook;
alignas(64) uint8_t keyStorage[16384][64]{};
tail_light20::Registry registry;
std::atomic_flag lock=ATOMIC_FLAG_INIT;
std::atomic<uint32_t> promoted{0},refused{0},contention{0},collisions{0},staleQueued{0},freed{0},lifecycleFaults{0};
bool Contains(uint32_t key) noexcept {
 const auto base=uint32_t(reinterpret_cast<uintptr_t>(keyStorage));
 return key>=base&&uint64_t(key)-base<sizeof(keyStorage);
}
void LifecycleFault() noexcept {active.store(false,std::memory_order_release);++lifecycleFaults;}
// Root dispatches this from the EXISTING 70BA07 pool-free observer even when
// dual-headlight splitting is disabled. A missed event disables this family.
void OnFree(uint32_t pool,uint32_t owner,uint32_t slot) noexcept {
 if(!active.load(std::memory_order_acquire))return;
 if(lock.test_and_set(std::memory_order_acquire)){++contention;LifecycleFault();return;}
 registry.Free(pool,owner,slot);lock.clear(std::memory_order_release);++freed;
}
uintptr_t CasterOwner(uintptr_t key) noexcept {
 if(key>UINT32_MAX||!Contains(uint32_t(key))||!active.load(std::memory_order_acquire)||!CShadows::pFrameCounter)return 0;
 if(lock.test_and_set(std::memory_order_acquire)){++contention;return 0;}
 liberty20::VehicleIdentity id{},fresh{};
 const bool resolved=registry.Resolve(uint32_t(key),*CShadows::pFrameCounter,id);
 lock.clear(std::memory_order_release);
 return resolved&&Liberty20::Vehicle(id.owner,fresh)&&fresh==id?id.owner:0;
}
void Submit(SafetyHookContext& regs,uint32_t site) noexcept {
 const fusionfix::shadows::FloatingPointState fp;const auto error=GetLastError();
 if(active.load(std::memory_order_acquire)&&addonActive.load(std::memory_order_acquire)&&PlayerShadowAllocation::Enabled()&&CShadows::pFrameCounter){
  tail_light20::Packet packet{};liberty20::VehicleIdentity id{},fresh{};
  bool ok=Liberty20::Copy(regs.esp,packet.words.data(),sizeof(packet.words))&&
   Liberty20::Vehicle(uint32_t(regs.esi),id)&&
   Liberty20::Copy(packet.words[0],packet.matrix.data(),sizeof(packet.matrix))&&
   Liberty20::Copy(packet.words[1],&packet.position,sizeof(packet.position))&&
   Liberty20::Copy(packet.words[2],&packet.direction,sizeof(packet.direction))&&
   Liberty20::Copy(packet.words[3],&packet.color,sizeof(packet.color))&&
   tail_light20::Authorized(site,id.owner,uint32_t(game+tail_light20::NativeColorRva),packet)&&
   Liberty20::Vehicle(id.owner,fresh)&&fresh==id;
  uint32_t key{};
  if(ok){
   if(lock.test_and_set(std::memory_order_acquire)){++contention;ok=false;}
   else {
    const auto previousUsed=registry.used;
    const auto* pair=registry.Observe(id,*CShadows::pFrameCounter);
    ok=pair!=nullptr;
    if(ok&&registry.used!=previousUsed&&(Liberty20::Collision(pair->keys[0])||Liberty20::Collision(pair->keys[1]))){
     registry.Retire(id.slot);++collisions;ok=false;
    }
    if(ok)key=pair->keys[tail_light20::NativeChannel(id.owner,packet)];
    lock.clear(std::memory_order_release);
   }
  }
  // The authenticated native call immediately consumes this writable stack.
  // Preserve all other words, all switches before this call and RGB/intensity.
  if(ok&&active.load(std::memory_order_acquire)){
   auto* words=reinterpret_cast<uint32_t*>(regs.esp);words[10]=key;words[11]=1;++promoted;
  }else ++refused;
 }
 SetLastError(error);
}
void Combined(SafetyHookContext& regs) noexcept {Submit(regs,tail_light20::CombinedSite);}
void Single(SafetyHookContext& regs) noexcept {Submit(regs,tail_light20::SingleSite);}
void ValidateQueued(rage::CLightSource* lights,uint32_t count) noexcept {
 if(!lights||count>4096)return;
 for(uint32_t i=0;i<count;++i){auto& light=lights[i];
  if(Contains(uint32_t(light.mCastShadows))&&!CasterOwner(uint32_t(light.mCastShadows))&&(light.mFlags&4u)){
   light.mFlags&=~4u;++staleQueued;
  }
 }
}
void PrepareHooks(const std::vector<uint8_t>& image) {
 if(!requested)return;
 for(const auto& [rva,size]:{std::pair<uint32_t,size_t>{0x642b10,0x64343d-0x642b10},{0x63de90,0x63e1eb-0x63de90}}){
  Require(rva+size<=image.size()&&rva+size<=original->bytes.size()&&!memcmp(image.data()+rva,original->bytes.data()+rva,size),"tail_native_producer_modified");
 }
 Require(OriginalCall(tail_light20::CombinedSite)==game+0x63de90&&OriginalCall(tail_light20::SingleSite)==game+0x63de90,"tail_native_target_changed");
 Require(registry.Configure(uint32_t(reinterpret_cast<uintptr_t>(keyStorage))),"tail_key_storage_invalid");
 Prepare(combinedHook,game+tail_light20::CombinedSite,Combined);Prepare(singleHook,game+tail_light20::SingleSite,Single);
 Require(combinedHook.original_bytes().size()==5&&singleHook.original_bytes().size()==5,"tail_hook_stole_cleanup");
 prepared=true;
}
void EnableHooks(bool sharedFreeObserverEnabled) {
 if(!requested)return;
 Require(prepared&&sharedFreeObserverEnabled,"tail_shared_free_observer_required");
 Require(bool(singleHook.enable())&&bool(combinedHook.enable()),"tail_hook_enable_failed");
 active.store(true,std::memory_order_release);
 Event(1,"TAIL_LIGHT20_ENABLED","native_rear_beam=1 native_color_pose_switches=1 separate_headlight_rank=1 capacity_expansion=0");
}
void Disable() noexcept {
 active=false;for(auto* hook:{&combinedHook,&singleHook})if(*hook)(void)hook->disable();
}
void Publish() {
 static uint32_t last{};const auto now=GetTickCount();if(now-last<5000)return;last=now;
 Event(1,"TAIL_LIGHT20_COUNTS","active="+std::to_string(active.load())+" promoted="+std::to_string(promoted.load())+
  " refused="+std::to_string(refused.load())+" contention="+std::to_string(contention.load())+
  " collisions="+std::to_string(collisions.load())+" stale_queued="+std::to_string(staleQueued.load())+
  " free_events="+std::to_string(freed.load())+" lifecycle_faults="+std::to_string(lifecycleFaults.load())+" eligible_not_simultaneous_or_receiver_proof=1");
}
}
