// Private implementation; included after the inherited traffic owner validator.
// Features are explicitly selected by candidate profiles; no publication.
#include "VehicleLampIdentity20.hpp"
#include "DualIssuedKeys127.hpp"
#include "FreshReadBatch30.hpp"
#include "OwnedImageReads224.hpp"
namespace Liberty20 {
bool dualRequested=false,worldRequested=false;
std::atomic<bool> dualActive{false},worldActive{false};
SafetyHookMid dualPre,dualPost,singlePre,freeObserver;
alignas(64) uint8_t keyStorage[16384][64]{};
liberty20::VehicleLampRegistry<> lampRegistry;
dual_keys127::PublishedKeys<> publishedKeys127;
std::atomic_flag registryLock=ATOMIC_FLAG_INIT;
std::atomic<uint32_t> splitPairs{0},fallbacks{0},contentions{0},collisions{0},worldEligible{0};
std::atomic<uint32_t> playerPairs{0},selectedLamps{0},renderVisits{0},maxSelectedLamps{0},maxSelectedPairs{0},staleQueued{0};
std::array<std::atomic<uint32_t>,4> worldFlags{};
std::atomic<uint32_t> worldKeyed{0},worldCached{0},worldCacheMatched{0},sourcePasses{0};
std::array<liberty20::Vec3,2> lastPositions{};
uint32_t lastSlot{},lastReference{};
struct Invocation {bool valid{};uint32_t owner{},frame{},stack{};std::array<liberty20::Vec3,2> positions{};liberty20::VehicleIdentity identity{};};
thread_local Invocation invocation;

// Copies remain fault-contained and fresh in both strategies. Candidate236
// obtains readability from the data copy; the legacy strategy preflights it.
// Neither path authorizes owner identity or later use of copied data.
__declspec(noinline) bool CopyLocal(uintptr_t address,void* output,size_t size) noexcept {
 __try {std::memcpy(output,reinterpret_cast<const void*>(address),size);return true;}
 __except(owned_image224::CopyException(GetExceptionInformation())){return false;}
}
bool Copy(uintptr_t address,void* output,size_t size) noexcept {
 if(contained_read236::enabled.load(std::memory_order_relaxed))return contained_read236::ReadInto236(address,output,size);
 if(owned_image224::Contains(address,size))return CopyLocal(address,output,size);
 if(!TrafficSignal::Readable(address,size))return false;
 return CopyLocal(address,output,size);
}
#ifndef CE_VEHICLE_BATCH_QUERY36
#define CE_VEHICLE_BATCH_QUERY36 QueryWorkingSetEx
#endif
bool Vehicle(uint32_t owner,liberty20::VehicleIdentity& id) noexcept {
 const immediate_region33::Scope operation;
 std::optional<traffic_signal::fresh_page::Operation44> vehiclePages233;
 if(traffic_signal::fresh_page::readScopes233.load(std::memory_order_relaxed)&&!traffic_signal::fresh_page::operation44)vehiclePages233.emplace(32);
 rage::VehiclePool header{};rage::VehiclePool* pool{};
 if(!Copy(reinterpret_cast<uintptr_t>(CVehicle::pool),&pool,sizeof(pool)))return false;
 if(!Copy(reinterpret_cast<uintptr_t>(pool),&header,sizeof(header))||header.m_nSize<=0||header.m_nSize>4096||header.m_nStorageSize<0x1308||header.m_nStorageSize>0x10000)return false;
 const auto storage=uint32_t(reinterpret_cast<uintptr_t>(header.m_aStorage)),flags=uint32_t(reinterpret_cast<uintptr_t>(header.m_aFlags));
 if(owner<storage||uint64_t(owner-storage)>=uint64_t(header.m_nSize)*header.m_nStorageSize||(owner-storage)%header.m_nStorageSize)return false;
 const auto slot=(owner-storage)/header.m_nStorageSize;uint8_t reference{},again{};int16_t modelIndex{};uint32_t info{},model{},type{};
 const traffic_signal::fresh_batch30::Span first[]={{flags+slot,&reference,1},{owner+0x2e,&modelIndex,2},{owner+0x1304,&type,4}};
 if(!traffic_signal::fresh_batch30::Copy(first,3,TrafficSignal::ReadableOriginal,CE_VEHICLE_BATCH_QUERY36)||(reference&0x80)||modelIndex<0||modelIndex>=31000||type!=0||
    !Copy(game+0xe95cd8+uint32_t(modelIndex)*4,&info,4))return false;
 rage::VehiclePool current{};
 const traffic_signal::fresh_batch30::Span final[]={{info+0x3c,&model,4},{flags+slot,&again,1},{reinterpret_cast<uintptr_t>(pool),&current,sizeof(current)}};
 if(!traffic_signal::fresh_batch30::Copy(final,3,TrafficSignal::ReadableOriginal,CE_VEHICLE_BATCH_QUERY36)||!model||again!=reference||current.m_aStorage!=header.m_aStorage||current.m_aFlags!=header.m_aFlags||current.m_nStorageSize!=header.m_nStorageSize||current.m_nSize!=header.m_nSize)return false;
 id={uint32_t(reinterpret_cast<uintptr_t>(pool)),storage,flags,owner,slot,reference,info,model};return true;
}
bool VehicleBeam(uintptr_t key,uintptr_t owner) noexcept {
 if(!dualActive.load(std::memory_order_acquire)||key>UINT32_MAX||owner>UINT32_MAX||!owner||!CShadows::pFrameCounter||
    !publishedKeys127.Contains(uint32_t(reinterpret_cast<uintptr_t>(keyStorage)),uint32_t(key)))return false;
 if(registryLock.test_and_set(std::memory_order_acquire)){++contentions;return false;}
 liberty20::VehicleIdentity id{};const bool resolved=lampRegistry.Resolve(uint32_t(key),*CShadows::pFrameCounter,id);
 registryLock.clear(std::memory_order_release);
 liberty20::VehicleIdentity fresh{};return resolved&&id.owner==owner&&Vehicle(uint32_t(owner),fresh)&&fresh==id;
}
fusionfix::shadows::ce::BeamPriorityEvidence32 BeamPriority32(uintptr_t key) noexcept {
 if(!dualActive.load(std::memory_order_acquire)||key>UINT32_MAX||!CShadows::pFrameCounter||
    !publishedKeys127.Contains(uint32_t(reinterpret_cast<uintptr_t>(keyStorage)),uint32_t(key)))return {};
 if(registryLock.test_and_set(std::memory_order_acquire)){++contentions;return {};}
 liberty20::VehicleIdentity id{};const bool resolved=lampRegistry.Resolve(uint32_t(key),*CShadows::pFrameCounter,id);
 registryLock.clear(std::memory_order_release);
 liberty20::VehicleIdentity fresh{};
 if(!resolved||!Vehicle(id.owner,fresh)||fresh!=id)return {};
 return {id.owner,uint32_t((key-reinterpret_cast<uintptr_t>(keyStorage))/64)+1};
}
uintptr_t BeamOwner(uintptr_t key) noexcept {return BeamPriority32(key).owner;}
uint32_t BeamOrder(uintptr_t key) noexcept {return BeamPriority32(key).order;}
void Pre(SafetyHookContext& regs) noexcept {
 const fusionfix::shadows::FloatingPointState fp;const auto error=GetLastError();
 invocation={};
 if(dualActive.load(std::memory_order_acquire)&&addonActive.load(std::memory_order_acquire)){
  std::array<uint32_t,12> words{};std::array<float,16> matrix{};liberty20::Vec3 a{},b{};
  liberty20::VehicleIdentity id{};
  if(Copy(regs.esp,words.data(),sizeof(words))&&words[10]==regs.esi&&Vehicle(uint32_t(regs.esi),id)&&
     Copy(words[0],matrix.data(),sizeof(matrix))&&Copy(regs.esp+0xa0,&a,sizeof(a))&&Copy(regs.esp+0x90,&b,sizeof(b))&&
     liberty20::LampPositions(matrix,a,b,invocation.positions)){
   invocation.valid=true;invocation.owner=uint32_t(regs.esi);invocation.frame=*CShadows::pFrameCounter;invocation.stack=uint32_t(regs.esp);invocation.identity=id;
  }else ++fallbacks;
 }
 SetLastError(error);
}
void Post(SafetyHookContext&) noexcept {invocation={};}
void Single(SafetyHookContext& regs) noexcept {
 const fusionfix::shadows::FloatingPointState fp;const auto error=GetLastError();
 if(dualActive.load(std::memory_order_acquire)){
  liberty20::VehicleIdentity id{};
  if(Vehicle(uint32_t(regs.esi),id)){
   if(registryLock.test_and_set(std::memory_order_acquire)){dualActive=false;++contentions;}
   else {lampRegistry.Retire(id.slot);registryLock.clear(std::memory_order_release);}
  }
 }
 invocation={};SetLastError(error);
}
void Free(SafetyHookContext& regs) noexcept {
 const fusionfix::shadows::FloatingPointState fp;const auto error=GetLastError();uint32_t owner{};
 // At 70BA07: ESI is pool; duplicated native owner argument is at ESP.
 rage::VehiclePool* vehiclePool{};
 if(ModelLightShadow20::requested||CoronaLight20::requested){
  if(Copy(regs.esp,&owner,4))ModelLightShadow20::OnFree(uint32_t(regs.esi),owner);
  else ModelLightShadow20::LifecycleFault();
 }
 if((dualRequested||TailLight20::requested)&&!Copy(reinterpret_cast<uintptr_t>(CVehicle::pool),&vehiclePool,sizeof(vehiclePool))){
  dualActive=false;TailLight20::LifecycleFault();SetLastError(error);return;
 }
 if((dualRequested||TailLight20::requested)&&Copy(reinterpret_cast<uintptr_t>(CVehicle::pool),&vehiclePool,sizeof(vehiclePool))&&uint32_t(regs.esi)==uint32_t(reinterpret_cast<uintptr_t>(vehiclePool))){
  rage::VehiclePool header{};
  if(!Copy(regs.esp,&owner,4)||!Copy(regs.esi,&header,sizeof(header))||header.m_nSize<=0||header.m_nSize>4096||header.m_nStorageSize<=0){dualActive=false;TailLight20::LifecycleFault();SetLastError(error);return;}
  const auto storage=uint32_t(reinterpret_cast<uintptr_t>(header.m_aStorage));
  if(owner<storage||uint64_t(owner-storage)>=uint64_t(header.m_nSize)*header.m_nStorageSize||(owner-storage)%header.m_nStorageSize){dualActive=false;TailLight20::LifecycleFault();SetLastError(error);return;}
  const auto slot=(owner-storage)/header.m_nStorageSize;
  TailLight20::OnFree(uint32_t(regs.esi),owner,slot);
  if(registryLock.test_and_set(std::memory_order_acquire)){
   // A missed destruction cannot silently keep identities live.
   dualActive.store(false,std::memory_order_release);++contentions;
  }else {lampRegistry.Free(uint32_t(regs.esi),owner,slot);registryLock.clear(std::memory_order_release);}
 }
 SetLastError(error);
}
bool Collision(uint32_t key) noexcept {
 if(TrafficSignal::KeyCollision(key)!=0)return true;
 for(unsigned i=0;i<8;++i)if(*reinterpret_cast<const uint32_t*>(game+fusionfix::shadows::ce::allocation::CacheAge0Rva+i*0x100+4)==key)return true;
 for(unsigned i=0;i<7;++i)if(*reinterpret_cast<const uint32_t*>(game+fusionfix::shadows::ce::allocation::DynamicPassFirstPositionRva+i*0x110+0x30)==key)return true;
 return false;
}
bool Split(const std::array<int,16>& input) noexcept {
 if(!dualActive.load(std::memory_order_acquire)||!invocation.valid||invocation.frame!=*CShadows::pFrameCounter||
    uint32_t(input[15])!=invocation.owner||input[1]!=2||(uint32_t(input[2])!=0x504&&uint32_t(input[2])!=0x104))return false;
 liberty20::VehicleIdentity fresh{};
 if(!Vehicle(invocation.owner,fresh)||!(fresh==invocation.identity)){++fallbacks;return false;}
 if(registryLock.test_and_set(std::memory_order_acquire)){++contentions;return false;}
 const auto* pair=lampRegistry.Observe(fresh,invocation.frame);
 std::array<uint32_t,2> keys{};bool ok=pair!=nullptr;
 if(ok){keys=pair->keys;
  // An allocated pair can remain unpublished after a refused input. Recheck
  // collision authority at its FIRST successful publication, not only allocation.
  if(!publishedKeys127.ContainsPair(uint32_t(reinterpret_cast<uintptr_t>(keyStorage)),keys)&&
     (Collision(keys[0])||Collision(keys[1]))){lampRegistry.Retire(fresh.slot);++collisions;ok=false;}
  if(ok&&uint32_t(input[2])==0x504){lastPositions=invocation.positions;lastSlot=fresh.slot;lastReference=fresh.reference;}
 }
 registryLock.clear(std::memory_order_release);
 if(!ok){++fallbacks;return false;}
 float intensity{};std::memcpy(&intensity,&input[7],4);if(!std::isfinite(intensity)||intensity<0){++fallbacks;return false;}
 const float half=intensity*.5f;int bits{};std::memcpy(&bits,&half,4);
 // Successful collision-checked ownership precedes both native calls. Freed
 // published keys remain owned forever; burned/unused addresses remain foreign.
 if(!publishedKeys127.PublishPair(uint32_t(reinterpret_cast<uintptr_t>(keyStorage)),keys)){++fallbacks;return false;}
 // Replace the combined packet, preserving cone, color, mask, room and range.
 // Actual attachment-derived native positions; two half-intensity sources.
 for(unsigned side=0;side<2;++side){
  auto words=input;words[5]=int(reinterpret_cast<uintptr_t>(&invocation.positions[side]));words[7]=bits;words[15]=int(keys[side]);
  if(!bHeadlightShadows)words[2]&=~4;
  CShadows::hbStoreStaticShadow.fun(words[0],words[1],words[2],words[3],words[4],words[5],words[6],words[7],words[8],words[9],words[10],words[11],words[12],words[13],words[14],words[15]);
  {const std::lock_guard<std::mutex> lock(CShadows::gStableHeadlightShadow.stateMutex);
   CShadows::gStableHeadlightShadow.PrepareFrame();CShadows::gStableHeadlightShadow.submitted.Record(keys[side],invocation.frame);}
 }
 ++splitPairs;if(uint32_t(input[2])==0x504)++playerPairs;return true;
}
void PromoteNativeWorld(rage::CLightSource* lights,uint32_t count) noexcept {
 if(count>4096||!lights)return;
 std::array<uint32_t,4> flags{};uint32_t keyed=0,cachedCount=0,matchedCount=0;
 for(uint32_t i=0;i<count;++i){auto& light=lights[i];
  // Issued private keys can never gain shadow authority from a stale packet.
  // Keep illumination, but refuse queued shadows after free/reuse/rewind.
  const auto key=uint32_t(light.mCastShadows);
  if(publishedKeys127.Contains(uint32_t(reinterpret_cast<uintptr_t>(keyStorage)),key)){
   liberty20::VehicleIdentity id{},fresh{};bool valid=false;
   if(dualActive.load(std::memory_order_acquire)&&!registryLock.test_and_set(std::memory_order_acquire)){
    valid=lampRegistry.Resolve(key,*CShadows::pFrameCounter,id);registryLock.clear(std::memory_order_release);
    valid=valid&&Vehicle(id.owner,fresh)&&fresh==id;
   }
   if(!valid&&(light.mFlags&4)){light.mFlags&=~4u;++staleQueued;}
   continue;
  }
  if(!worldActive.load(std::memory_order_acquire))continue;
  if((light.mType==rage::LT_POINT||light.mType==rage::LT_SPOT)&&!(light.mFlags&0x100u)){
   ++flags[(light.mFlags&6u)>>1];
   if(light.mCastShadows&&uint32_t(light.mCastShadows)!=UINT32_MAX){
    ++keyed;if(light.mShadowCacheIndex>=0&&light.mShadowCacheIndex<8){
     ++cachedCount;const auto cache=*reinterpret_cast<const uint32_t*>(game+fusionfix::shadows::ce::allocation::CacheAge0Rva+uint32_t(light.mShadowCacheIndex)*0x100+4);
     if(cache==uint32_t(light.mCastShadows))++matchedCount;
    }
   }
  }
  // Native lifetime/key/cache authority is retained. Unkeyed effects are
  // outside this gate and require authenticated producer promotion separately.
  if((light.mType!=rage::LT_POINT&&light.mType!=rage::LT_SPOT)||(light.mFlags&6)!=2||
     !light.mCastShadows||uint32_t(light.mCastShadows)==UINT32_MAX||light.mShadowCacheIndex<0||light.mShadowCacheIndex>=8||
     !std::isfinite(light.mRadius)||light.mRadius<=0)continue;
  const auto cached=*reinterpret_cast<const uint32_t*>(game+fusionfix::shadows::ce::allocation::CacheAge0Rva+uint32_t(light.mShadowCacheIndex)*0x100+4);
  if(cached!=uint32_t(light.mCastShadows))continue;
  light.mFlags|=4u;++worldEligible;
 }
 for(unsigned i=0;i<4;++i)worldFlags[i]+=flags[i];worldKeyed+=keyed;worldCached+=cachedCount;worldCacheMatched+=matchedCount;++sourcePasses;
}
void Max(std::atomic<uint32_t>& target,uint32_t value) noexcept {auto old=target.load();while(value>old&&!target.compare_exchange_weak(old,value)){};}
void Result() noexcept {
 if(!dualRequested||registryLock.test_and_set(std::memory_order_acquire))return;
 unsigned count=0;std::array<uint32_t,7> owners{};std::array<unsigned,7> ownerCounts{};unsigned ownerUsed=0;
 for(unsigned slot=0;slot<7;++slot){const auto record=game+fusionfix::shadows::ce::allocation::DynamicPassFirstPositionRva+slot*0x110;
  if(!*reinterpret_cast<const uint8_t*>(record+0x25))continue;
  const auto key=*reinterpret_cast<const uint32_t*>(record+0x30);liberty20::VehicleIdentity id{};
  if(!lampRegistry.Resolve(key,*CShadows::pFrameCounter,id))continue;
  ++count;unsigned i=0;while(i<ownerUsed&&owners[i]!=id.owner)++i;
  if(i==ownerUsed)owners[ownerUsed++]=id.owner;++ownerCounts[i];
 }
 registryLock.clear(std::memory_order_release);
 unsigned pairs=0;for(unsigned i=0;i<ownerUsed;++i)if(ownerCounts[i]==2)++pairs;
 selectedLamps+=count;Max(maxSelectedLamps,count);Max(maxSelectedPairs,pairs);
}
void Render(uint32_t key,uint32_t slot,uint32_t kind,bool active) noexcept {
 if(!active||slot<1||slot>7||kind!=4||registryLock.test_and_set(std::memory_order_acquire))return;
 liberty20::VehicleIdentity id{};const bool valid=lampRegistry.Resolve(key,*CShadows::pFrameCounter,id);
 registryLock.clear(std::memory_order_release);if(valid)++renderVisits;
}
void Disable() noexcept {
 dualActive=false;worldActive=false;
 for(auto* hook:{&dualPre,&dualPost,&singlePre,&freeObserver})if(*hook)(void)hook->disable();
}
void PrepareHooks(const std::vector<uint8_t>& image) {
 if(!dualRequested&&!worldRequested&&!TailLight20::requested&&!ModelLightShadow20::requested&&!CoronaLight20::requested)return;
 if(worldRequested)worldActive=true;
 if(dualRequested||TailLight20::requested||ModelLightShadow20::requested||CoronaLight20::requested){
  RequireNativeRange129(image,0x70ba00,0x22,"vehicle_free_observer_modified");
  Prepare(freeObserver,game+0x70ba07,Free);
 }
 if(!dualRequested)return;
 // Authenticate full producer and both transforms BEFORE inherited brightness
 // hooks are installed. Unknown graphics/headlight patches are refused.
 for(const auto& [rva,size]:{std::pair<uint32_t,size_t>{0x63f5e0,0x63ff1f-0x63f5e0},{0x63de90,0x63e1eb-0x63de90},{0x70ba00,0x22}}){
  RequireNativeRange129(image,rva,size,"dual_native_producer_modified");
 }
 Require(OriginalCall(0x63fe11)==game+0x63e070,"dual_combined_target_changed");
 Require(lampRegistry.Configure(uint32_t(reinterpret_cast<uintptr_t>(keyStorage))),"dual_key_storage_invalid");
 Prepare(dualPre,game+0x63fe11,Pre);Prepare(dualPost,game+0x63fe16,Post);Prepare(singlePre,game+0x63ff0f,Single);
}
void EnableHooks() {
 if(freeObserver)Require(bool(freeObserver.enable()),"vehicle_free_enable_failed");
 if(dualRequested){for(auto* hook:{&dualPost,&singlePre,&dualPre})Require(bool(hook->enable()),"dual_hook_enable_failed");dualActive=true;
  fusionfix::shadows::ce::AdditionalVehicleBeam=VehicleBeam;
  fusionfix::shadows::ce::AdditionalVehicleBeamOwner=BeamOwner;
  OwnHeadlightCaster::singlePriority146=BeamPriority32;
  OwnHeadlightCaster::singleOwner146=BeamOwner;
  OwnHeadlightCaster::singleBeam146=VehicleBeam;
  fusionfix::shadows::ce::AdditionalVehicleBeamOrder=BeamOrder;
  fusionfix::shadows::ce::AdditionalVehicleBeamPriority32=BeamPriority32;
 }
 Event(1,"LIGHTING20_FEATURES","dual="+std::to_string(dualActive.load())+" native_keyed_world="+std::to_string(worldActive.load())+" native_slots=7 unkeyed_world_promotion=0 capacity_expansion=0");
}
void Publish() {
 static uint32_t last{};const auto now=GetTickCount();if(now-last<5000)return;last=now;
 Event(1,"LIGHTING20_COUNTS","split_pairs="+std::to_string(splitPairs.load())+" fallback="+std::to_string(fallbacks.load())+" contention="+std::to_string(contentions.load())+" collisions="+std::to_string(collisions.load())+" keyed_world_eligible="+std::to_string(worldEligible.load())+" submissions_not_visual_proof=1");
 Event(1,"LIGHTING20_ALLOCATION","applied="+std::to_string(PlayerShadowAllocation::appliedPasses.load())+
  " fallback="+std::to_string(PlayerShadowAllocation::fallbackPasses.load())+
  " isolated_key_passes="+std::to_string(PlayerShadowAllocation::ambiguousKeyPasses.load())+
  " isolated_key_inputs="+std::to_string(PlayerShadowAllocation::ambiguousKeyInputs.load())+
  " stable_passes="+std::to_string(PlayerShadowAllocation::stableOrdinalPasses.load())+
  " stable_moves="+std::to_string(PlayerShadowAllocation::stableOrdinalMoves.load())+
  " stable_rejected="+std::to_string(PlayerShadowAllocation::stableOrdinalRejected.load())+
  " retained_map_moves="+std::to_string(PlayerShadowAllocation::retainedMapMoves.load())+
  " own_eligible_passes="+std::to_string(PlayerShadowAllocation::ownEligiblePasses.load())+
  " own_missing_selection_passes="+std::to_string(PlayerShadowAllocation::ownMissingSelectionPasses.load())+
  " lookup_dynamic_rejected="+std::to_string(ShadowLookupGuard::rejectedDynamic.load())+
  " lookup_static_rejected="+std::to_string(ShadowLookupGuard::rejectedStatic.load())+
  " lookup_buffer_changed="+std::to_string(ShadowLookupGuard::changedBuffer.load())+
  " thread_block="+std::to_string(PlayerShadowAllocation::unsupportedThread.load())+
  " selection_not_receiver_proof=1");
 Event(1,"LIGHTING20_ALLOCATION_LOSS","lamp_removed="+std::to_string(PlayerShadowAllocation::lampRemoved.load())+
  " lamp_missing_input="+std::to_string(PlayerShadowAllocation::lampMissingInput.load())+
  " lamp_dropped_present="+std::to_string(PlayerShadowAllocation::lampDroppedPresent.load())+
  " adapter_rejected="+std::to_string(PlayerShadowAllocation::rejectedAdapterChecks.load())+
  " rejected_begin="+std::to_string(PlayerShadowAllocation::rejectedPassReasons[1].load())+
  " rejected_input="+std::to_string(PlayerShadowAllocation::rejectedPassReasons[2].load())+
  " rejected_duplicate="+std::to_string(PlayerShadowAllocation::rejectedPassReasons[3].load())+
  " rejected_capacity="+std::to_string(PlayerShadowAllocation::rejectedPassReasons[4].load())+
  " rejected_commit="+std::to_string(PlayerShadowAllocation::rejectedPassReasons[5].load())+
  " rejected_snapshot="+std::to_string(PlayerShadowAllocation::rejectedPassReasons[6].load())+
  " candidate_capacity=640 native_shadow_slots=7 cumulative_counts=1");
 Event(1,"LIGHTING20_RENDER","player_pairs="+std::to_string(playerPairs.load())+" selected_lamps="+std::to_string(selectedLamps.load())+" render_visits="+std::to_string(renderVisits.load())+" max_selected_lamps="+std::to_string(maxSelectedLamps.load())+" max_selected_pairs="+std::to_string(maxSelectedPairs.load())+" stale_queued_refused="+std::to_string(staleQueued.load())+" rendering_visits_not_receiver_proof=1");
 uint8_t modelLights=0;Copy(game+0xc48244,&modelLights,1);
 Event(1,"LIGHTING20_WORLD_CENSUS","passes="+std::to_string(sourcePasses.load())+" flags0="+std::to_string(worldFlags[0].load())+" flags2="+std::to_string(worldFlags[1].load())+" flags4="+std::to_string(worldFlags[2].load())+" flags6="+std::to_string(worldFlags[3].load())+" keyed="+std::to_string(worldKeyed.load())+" cached="+std::to_string(worldCached.load())+" cache_match="+std::to_string(worldCacheMatched.load())+" native_model_light_gate="+std::to_string(modelLights)+" counts_are_repeated_observations=1");
 if(!registryLock.test_and_set(std::memory_order_acquire)){
  auto positions=lastPositions;const auto slot=lastSlot,reference=lastReference,used=lampRegistry.used;
  registryLock.clear(std::memory_order_release);
  if(playerPairs.load()){char fields[350];sprintf_s(fields,"slot=%u reference=%u issued_pairs=%u left=%.6g,%.6g,%.6g right=%.6g,%.6g,%.6g actual_native_attachments=1",slot,reference,used/2,double(positions[0].x),double(positions[0].y),double(positions[0].z),double(positions[1].x),double(positions[1].y),double(positions[1].z));Event(1,"LIGHTING20_PLAYER_GEOMETRY",fields);}
 }
}
}
