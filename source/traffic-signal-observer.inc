// Experimental dynamically discovered native traffic sources. No assets,
// synthetic lights, renderer allocation or source geometry/color changes.
#include "FreshPageReadable.hpp"
#include "ImmediateRegion33.hpp"
#include "FreshReadBatch30.hpp"
#include "FreshReadWindow49.hpp"
#include "FreshReadNativeWindow115.hpp"
#include "FreshReadGroup50.hpp"
#include "TrafficSignalRegistry.hpp"
#include "PoolDescriptorSnapshots.hpp"
#include "TrafficSignalDistanceGate.hpp"
#include "TrafficPoolHookGuard.hpp"
#include "TrafficSignalNativeQueue.hpp"
#include "TrafficSignalBrightness164.hpp"
#include "NativeOwnerClass109.hpp"
#include "NativeOwnerClass114.hpp"
namespace TrafficSignal {
constexpr uint32_t KeyCapacity=16384;
bool requested=false,authenticated=false;
std::atomic<bool> promotionRequested{false},enabled{false},fatal{false};
std::atomic<uint32_t> observerThread{0},issuedKeys{0};
alignas(64) uint8_t reservedIdentities[KeyCapacity][64]{};
traffic_signal::Registry<KeyCapacity> registry;
traffic_signal::PlayerPositionSnapshot playerPositionSnapshot;
SafetyHookMid freeHook,timerRestoreHook;
struct Snapshot {std::atomic<uint32_t> state{0},phase{0},lastAdmission{0},pool{0},owner{0};};
std::array<Snapshot,KeyCapacity> snapshots;
std::atomic<uint32_t> admitted{0},selected{0},renderVisits{0},collisions{0},collisionReason{0},maxActiveSlots{0};
std::atomic<uint32_t> siteCalls{0},membershipRejected{0},argumentRejected{0},leaseBreaks{0},capacityRejected{0},timerRebases{0},queuedAfterRetirement{0};
std::array<std::atomic<uint32_t>,3> phaseAdmissions{},phaseSelections{},phaseRenders{};
struct Record {uint32_t event{},index=UINT32_MAX,tick{},frame{},previous{},caller{},thread{},kind=UINT32_MAX;float entity[3]{};};
std::array<Record,1024> records;
std::atomic<uint32_t> head{0},tail{0},dropped{0};std::atomic_flag recordLock=ATOMIC_FLAG_INIT;
uint32_t validationCursor{};
bool ReadableOriginal(uintptr_t address,size_t size) noexcept {
 return immediate_region33::Readable(address,size);
}
bool Readable(uintptr_t address,size_t size) noexcept {
 return traffic_signal::fresh_page::Readable(address,size,ReadableOriginal);
}
void Emit(Record r) noexcept {
 if(!diagnosticsEnabled)return;
 if(recordLock.test_and_set(std::memory_order_acquire)){++dropped;return;}
 const auto h=head.load(std::memory_order_relaxed),t=tail.load(std::memory_order_acquire);
 if(h-t<records.size()){records[h%records.size()]=r;head.store(h+1,std::memory_order_release);}else ++dropped;
 recordLock.clear(std::memory_order_release);
}
struct Match {traffic_signal::Identity identity;uint32_t kind;float entity[3];uint32_t modelInfo{};std::array<uint8_t,68> ownerPose109{};};
bool PoolMatch(uintptr_t owner,uint32_t pointerRva,uint32_t stride,uint32_t kind,Match& result) noexcept {
 const auto pool=*reinterpret_cast<rage::VehiclePool**>(game+pointerRva);
 if(!Readable(reinterpret_cast<uintptr_t>(pool),0x1c))return false;
 if(pool->m_nSize<=0||pool->m_nSize>65536||pool->m_nStorageSize!=static_cast<int32_t>(stride))return false;
 const auto storage=reinterpret_cast<uintptr_t>(pool->m_aStorage),flags=reinterpret_cast<uintptr_t>(pool->m_aFlags);
 const uint64_t bytes=uint64_t(pool->m_nSize)*stride;
 if(owner<storage||uint64_t(owner-storage)>=bytes||(owner-storage)%stride||!Readable(owner,0x30))return false;
 const auto slot=uint32_t((owner-storage)/stride);
 if(!Readable(flags+slot,1))return false;const auto reference=pool->m_aFlags[slot];if(reference&0x80)return false;
 const auto index=*reinterpret_cast<const int16_t*>(owner+0x2e);if(index<0||index>=31000)return false;
 const auto info=*reinterpret_cast<const uintptr_t*>(game+0xe95cd8+index*4);
 if(!Readable(info,0x40))return false;const auto model=*reinterpret_cast<const uint32_t*>(info+0x3c);if(!model)return false;
 const auto matrix=*reinterpret_cast<const uintptr_t*>(owner+0x20);if(!Readable(matrix,64))return false;
 for(unsigned i=0;i<3;++i){result.entity[i]=reinterpret_cast<const float*>(matrix)[12+i];if(!std::isfinite(result.entity[i]))return false;}
 if(pool->m_aFlags[slot]!=reference)return false;
 result.identity={uint32_t(reinterpret_cast<uintptr_t>(pool)),uint32_t(storage),uint32_t(flags),uint32_t(owner),slot,reference,model};result.kind=kind;result.modelInfo=uint32_t(info);return true;
}
bool MatchOwnerOriginal(uintptr_t owner,Match& value) noexcept {
 Match b{},o{};const bool building=PoolMatch(owner,0xebd0e8,0x70,0,b),object=PoolMatch(owner,0x1232c60,0x320,1,o);
 if(building==object)return false;value=building?b:o;return true;
}
enum class PoolSnapshotOutcome {Rejected,Matched,Restart};
#ifndef CE_OWNER_BATCH_QUERY35
#define CE_OWNER_BATCH_QUERY35 QueryWorkingSetEx
#endif
bool OwnerCopy35(const traffic_signal::fresh_batch30::Span* spans,unsigned count,bool* readFault=nullptr) noexcept {
 return traffic_signal::fresh_batch30::Copy(spans,count,ReadableOriginal,CE_OWNER_BATCH_QUERY35,readFault);
}
#include "NativeOwnerClass109Runtime.inc"
#include "NativeOwnerClass114Runtime.inc"
PoolSnapshotOutcome PoolMatchSnapshot(uintptr_t owner,const traffic_signal::pool_descriptors::Header& header,uint32_t stride,uint32_t kind,Match& result) noexcept {
 if(!header.valid)return PoolSnapshotOutcome::Rejected;
 const auto pool=reinterpret_cast<rage::VehiclePool*>(header.pool);
 const auto storage=header.storage,flags=header.flags;
 const uint64_t bytes=uint64_t(header.size)*stride;
 if(owner<storage||uint64_t(owner-storage)>=bytes||(owner-storage)%stride)return PoolSnapshotOutcome::Rejected;
 const auto slot=uint32_t((owner-storage)/stride);
 std::array<uint8_t,0x30> ownerBytes{};uint8_t reference{};
 const traffic_signal::fresh_batch30::Span ownerSpans[]{
  {owner,ownerBytes.data(),ownerBytes.size()},{flags+slot,&reference,1}};
 if(!OwnerCopy35(ownerSpans,2)||(reference&0x80))return PoolSnapshotOutcome::Rejected;
 int16_t index{};uintptr_t matrix{};
 std::memcpy(&index,ownerBytes.data()+0x2e,2);std::memcpy(&matrix,ownerBytes.data()+0x20,4);
 if(index<0||index>=31000)return PoolSnapshotOutcome::Rejected;
 const auto info=*reinterpret_cast<const uintptr_t*>(game+0xe95cd8+index*4);
 std::array<uint8_t,0x40> modelBytes{};std::array<float,16> matrixBytes{};
 const traffic_signal::fresh_batch30::Span detailSpans[]{
  {info,modelBytes.data(),modelBytes.size()},{matrix,matrixBytes.data(),sizeof(matrixBytes)}};
 if(!OwnerCopy35(detailSpans,2))return PoolSnapshotOutcome::Rejected;
 uint32_t model{};std::memcpy(&model,modelBytes.data()+0x3c,4);if(!model)return PoolSnapshotOutcome::Rejected;
 for(unsigned i=0;i<3;++i){result.entity[i]=matrixBytes[12+i];if(!std::isfinite(result.entity[i]))return PoolSnapshotOutcome::Rejected;}
 const auto liveFlags=pool->m_aFlags;
 if(reinterpret_cast<uintptr_t>(liveFlags)!=flags)return PoolSnapshotOutcome::Restart;
 if(liveFlags[slot]!=reference)return PoolSnapshotOutcome::Rejected;
 result.identity={uint32_t(reinterpret_cast<uintptr_t>(pool)),uint32_t(storage),uint32_t(flags),uint32_t(owner),slot,reference,model};result.kind=kind;result.modelInfo=uint32_t(info);return PoolSnapshotOutcome::Matched;
}
// Optional operation-local independent bytes are batched with the pool headers.
// Generic owner callers pass no extra span and retain the original behavior.
bool MatchOwnerUnchecked32(uintptr_t owner,Match& value,
 const traffic_signal::fresh_batch30::Span* extra=nullptr,bool* extraCopied=nullptr,bool* descriptorFault=nullptr) noexcept {
 if(extraCopied)*extraCopied=false;
 const auto buildingPool=*reinterpret_cast<const uintptr_t*>(game+0xebd0e8);
 const auto objectPool=*reinterpret_cast<const uintptr_t*>(game+0x1232c60);
 rage::VehiclePool buildingBytes{},objectBytes{};
 const traffic_signal::fresh_batch30::Span headers[]{
  {buildingPool,&buildingBytes,sizeof(buildingBytes)},{objectPool,&objectBytes,sizeof(objectBytes)},
  extra?*extra:traffic_signal::fresh_batch30::Span{}};
 // General fresh header stage: building lights receive the same batched body
 // validation as object lights. No descriptor or permission survives this call.
 bool combinedReadFault49=false;
 if(!OwnerCopy35(headers,extra?3:2,&combinedReadFault49)){
  if(combinedReadFault49&&descriptorFault)*descriptorFault=true;
  // Failure of the optional descriptor must not make a formerly valid owner
  // unavailable. Keep original owner fallback; only permission refusals may
  // retry independent descriptor. A raw Stage fault is never re-authorized.
  if(extra&&extraCopied&&!combinedReadFault49)*extraCopied=OwnerCopy35(extra,1,descriptorFault);
  return MatchOwnerOriginal(owner,value);
 }
 if(extra&&extraCopied)*extraCopied=true;
 auto header=[](uintptr_t pool,const rage::VehiclePool& bytes,int32_t stride){
  traffic_signal::pool_descriptors::Header h{};h.pool=pool;
  if(bytes.m_nSize<=0||bytes.m_nSize>65536||bytes.m_nStorageSize!=stride)return h;
  h.storage=reinterpret_cast<uintptr_t>(bytes.m_aStorage);h.flags=reinterpret_cast<uintptr_t>(bytes.m_aFlags);h.size=bytes.m_nSize;h.valid=true;return h;
 };
 const auto bheader=header(buildingPool,buildingBytes,0x70),oheader=header(objectPool,objectBytes,0x320);
 Match b{},o{};
 const auto building=PoolMatchSnapshot(owner,bheader,0x70,0,b),object=PoolMatchSnapshot(owner,oheader,0x320,1,o);
 if(building==PoolSnapshotOutcome::Restart||object==PoolSnapshotOutcome::Restart)return MatchOwnerOriginal(owner,value);
 // Authenticate the descriptor observations again before publishing identity;
 // reject a replaced pool, changed range, or owner belonging to both pools.
 if(*reinterpret_cast<const uintptr_t*>(game+0xebd0e8)!=buildingPool||*reinterpret_cast<const uintptr_t*>(game+0x1232c60)!=objectPool)return false;
 auto unchanged=[](const traffic_signal::pool_descriptors::Header& h,int32_t stride){
  if(!h.valid)return true;
  const auto* pool=reinterpret_cast<const rage::VehiclePool*>(h.pool);
  return reinterpret_cast<uintptr_t>(pool->m_aStorage)==h.storage&&reinterpret_cast<uintptr_t>(pool->m_aFlags)==h.flags&&pool->m_nSize==h.size&&pool->m_nStorageSize==stride;
 };
 if(!unchanged(bheader,0x70)||!unchanged(oheader,0x320))return false;
 const bool bm=building==PoolSnapshotOutcome::Matched,om=object==PoolSnapshotOutcome::Matched;
 if(bm==om)return !bm&&(MatchOwnerClass109(owner,value)||MatchOwnerClass114(owner,value));value=bm?b:o;return true;
}
// No permission or lifetime cache: all checks above run anew on every call.
// Keep SEH in a separate leaf so profiling RAII unwinds normally.
std::atomic<uint32_t> ownerReadFaults32{0};
bool MatchOwnerContained32(uintptr_t owner,Match& value) noexcept {
 __try {return MatchOwnerUnchecked32(owner,value);}
 __except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION||
          GetExceptionCode()==EXCEPTION_IN_PAGE_ERROR||
          GetExceptionCode()==EXCEPTION_GUARD_PAGE?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){++ownerReadFaults32;return false;}
}
bool MatchOwner(uintptr_t owner,Match& value) noexcept {
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::OwnerMatch,128);
 const immediate_region33::Scope operation;
 return MatchOwnerContained32(owner,value);
}
// Model-only combined entry: no owner result or descriptor survives this call.
// Descriptor output is staged by Copy; false never publishes partial bytes.
bool MatchOwnerWithDescriptorContained48(uintptr_t owner,Match& value,
 const traffic_signal::fresh_batch30::Span& extra,bool& copied,bool& descriptorFault) noexcept {
 __try {return MatchOwnerUnchecked32(owner,value,&extra,&copied,&descriptorFault);}
 __except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION||
          GetExceptionCode()==EXCEPTION_IN_PAGE_ERROR||
          GetExceptionCode()==EXCEPTION_GUARD_PAGE?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){++ownerReadFaults32;return false;}
}
bool MatchOwnerWithDescriptor48(uintptr_t owner,Match& value,
 uintptr_t descriptor,void* output,size_t size,bool& copied) noexcept {
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::OwnerMatch,128);
 const immediate_region33::Scope operation;
 copied=false;
 const traffic_signal::fresh_batch30::Span extra{descriptor,output,size};
 bool descriptorFault49=false;
 const bool matched=MatchOwnerWithDescriptorContained48(owner,value,extra,copied,descriptorFault49);
 // Preserve the old independent descriptor read even when owner validation
 // faults before reaching the initial batch (unsupported diagnostics/coronas).
 // A Stage fault may consume PAGE_GUARD; never convert it into approval by
 // retrying that descriptor. Ordinary permission/early-owner faults retain fallback.
 if(!copied&&!descriptorFault49)copied=OwnerCopy35(&extra,1);
 return matched;
}
uint32_t PrivateKey() noexcept {return uint32_t(reinterpret_cast<uintptr_t>(reservedIdentities));}
#include "ModelOwnerWindow49Runtime.inc"
uint32_t KeyIndex(uint32_t key) noexcept {
 const auto base=PrivateKey();if(key<base||(key-base)%64)return UINT32_MAX;
 const auto index=(key-base)/64;return index<issuedKeys.load(std::memory_order_acquire)?index:UINT32_MAX;
}
uint64_t LampGeneration(const rage::CLightSource& light) noexcept {
 const auto key=uint32_t(light.mCastShadows),index=KeyIndex(key);
 if(index==UINT32_MAX||!enabled.load()||fatal.load()||registry.Disabled()||!CShadows::pFrameCounter||
    !traffic_signal::StoredPrivateTraffic(light.mType,light.mFlags,key,key))return 0;
 const auto& entry=registry.entries[index];const auto frame=*CShadows::pFrameCounter;
 return !entry.retired&&entry.leased&&entry.submitted&&snapshots[index].state.load()==1&&
  uint32_t(frame-entry.lastFrame)<=2&&uint32_t(frame-entry.lastAdmission)<=2?uint64_t(index)+1:0;
}
void ValidateQueued(rage::CLightSource* lights,uint32_t count) noexcept {
 if(!lights||count>4096||!CShadows::pFrameCounter)return;
 const auto frame=*CShadows::pFrameCounter;
 for(uint32_t i=0;i<count;++i){auto& light=lights[i];const auto index=KeyIndex(uint32_t(light.mCastShadows));
  if(index==UINT32_MAX||!(light.mFlags&4u))continue;
  const auto& entry=registry.entries[index];Match fresh{};
  const bool valid=enabled.load()&&!fatal.load()&&!registry.Disabled()&&promotionRequested.load()&&
   light.mType==2&&!entry.retired&&entry.leased&&entry.submitted&&snapshots[index].state.load()==1&&
   uint32_t(frame-entry.lastFrame)<=2&&uint32_t(frame-entry.lastAdmission)<=2&&
   MatchOwner(entry.id.owner,fresh)&&fresh.identity==entry.id;
  if(!valid){light.mFlags&=~4u;experiment_perf::Count(hotCountersEnabled,queuedAfterRetirement);}
 }
}
void RecordEvent(uint32_t event,uint32_t index,uint32_t frame,uint32_t kind=UINT32_MAX,const float* entity=nullptr) noexcept {
 if(!diagnosticsEnabled)return;
 Record r{};r.event=event;r.index=index;r.tick=GetTickCount();r.frame=frame;r.thread=GetCurrentThreadId();r.kind=kind;
 if(entity)std::memcpy(r.entity,entity,12);Emit(r);
}
void Retire(uint32_t index,traffic_signal::End reason,uint32_t event,uint32_t frame) noexcept {
 if(index>=registry.Used())return;
 if(registry.Retire(index,reason)){snapshots[index].state.store(2,std::memory_order_release);RecordEvent(event,index,frame);}
}
void FatalThread() noexcept {
 if(!fatal.exchange(true)){RecordEvent(7,UINT32_MAX,0);}
}
uint32_t KeyCollision(uint32_t key) noexcept {
 const auto count=*reinterpret_cast<const uint32_t*>(game+traffic_signal::SubmissionCountRva);
 const auto* lights=*reinterpret_cast<rage::CLightSource* const*>(game+traffic_signal::SubmissionPointerRva);
 const auto address=reinterpret_cast<uintptr_t>(lights),completed=reinterpret_cast<uintptr_t>(PlayerShadowAllocation::CurrentLights());
 const bool readable=count<=traffic_signal::SubmissionCapacity&&(!count||Readable(address,size_t(count)*sizeof(rage::CLightSource)));
 return traffic_signal::SubmissionCollision(count,address,completed,readable,key,[lights](uint32_t i){return uint32_t(lights[i].mCastShadows);});
}
bool PlayerPosition(uint32_t frame,float (&position)[3]) noexcept {
 auto& cachedFrame=playerPositionSnapshot.frame;auto& valid=playerPositionSnapshot.valid;auto& cached=playerPositionSnapshot.position;
 if(frame!=cachedFrame){cachedFrame=frame;valid=false;
  const auto ped=CPlayer::getLocalPlayerPed?CPlayer::getLocalPlayerPed():0;
  if(Readable(ped+0x20,4)){const auto matrix=*reinterpret_cast<const uintptr_t*>(ped+0x20);
   if(Readable(matrix,64)){valid=true;for(unsigned i=0;i<3;++i){cached[i]=reinterpret_cast<const float*>(matrix)[12+i];valid=valid&&std::isfinite(cached[i]);}}}
 }if(valid)std::memcpy(position,cached,12);return valid;
}
void Observe(SafetyHookContext& regs,uint32_t caller) noexcept {
 if(caller!=0x921029||!enabled.load()||fatal.load()||!promotionRequested.load()||!addonActive.load())return;
 if(GetCurrentThreadId()!=observerThread.load()){FatalThread();return;}experiment_perf::Count(hotCountersEnabled,siteCalls);
 if(!Readable(regs.esp,17*4))return;auto* args=reinterpret_cast<uint32_t*>(regs.esp);
 if(args[2]!=2||args[3]!=0x200||args[16]||regs.edi>2){experiment_perf::Count(hotCountersEnabled,argumentRejected);return;}
 if(!Readable(args[6],12)||!Readable(args[7],12))return;
 const auto* pos=reinterpret_cast<const float*>(args[6]);const auto* rgb=reinterpret_cast<const float*>(args[7]);
 for(unsigned i=0;i<3;++i)if(!std::isfinite(pos[i])||!std::isfinite(rgb[i])){experiment_perf::Count(hotCountersEnabled,argumentRejected);return;}
 const auto frame=*CShadows::pFrameCounter;
 const auto reach=float(std::clamp(FusionFixSettings.lampReach.load(),0,100));
 if(traffic_signal::RejectUntrackedFar(regs.esi,frame,pos,reach,playerPositionSnapshot,
    []{return PlayerShadowAllocation::Enabled();},[](uint32_t owner){return registry.TracksOwner(owner);}))return;
 Match value{};if(!MatchOwner(regs.esi,value)){experiment_perf::Count(hotCountersEnabled,membershipRejected);return;}
 float player[3]{};
 const bool relevant=PlayerShadowAllocation::Enabled()&&PlayerPosition(frame,player)&&traffic_signal::Nearby(pos,player,reach);
 const auto result=registry.Observe(value.identity,frame,regs.edi,relevant);
 if(result.replaced!=UINT32_MAX){snapshots[result.replaced].state=2;RecordEvent(6,result.replaced,frame);}
 if(result.visit==traffic_signal::Visit::Capacity){experiment_perf::Count(hotCountersEnabled,capacityRejected);return;}
 if(result.index==UINT32_MAX)return;const auto index=result.index;auto& entry=registry.entries[index];
 if(result.visit==traffic_signal::Visit::New){
  snapshots[index].state=0;snapshots[index].pool=value.identity.pool;snapshots[index].owner=value.identity.owner;
  issuedKeys.store(registry.Used(),std::memory_order_release);RecordEvent(1,index,frame,value.kind,value.entity);
 }else if(result.visit==traffic_signal::Visit::Retired){
  if(snapshots[index].state.exchange(2)!=2)RecordEvent(entry.reason==traffic_signal::End::Clock?14:6,index,frame);return;
 }else if(result.visit==traffic_signal::Visit::LeaseRestart){experiment_perf::Count(hotCountersEnabled,leaseBreaks);RecordEvent(2,index,frame);}
 if(!entry.leased){snapshots[index].state=0;return;}
 if(entry.submitted&&entry.lastAdmission==frame)return;
 const auto collision=KeyCollision(entry.key);
 if(traffic_signal::ApplySignalSpill164(registry,index,value.identity,frame,caller,regs.edi,collision!=0,args,rgb)){
  snapshots[index].phase=regs.edi;snapshots[index].lastAdmission=frame;snapshots[index].state.store(1,std::memory_order_release);
  if(hotCountersEnabled&&admitted.fetch_add(1)==0)RecordEvent(11,index,frame);experiment_perf::Count(hotCountersEnabled,phaseAdmissions[regs.edi]);
 }else if(collision){experiment_perf::Count(hotCountersEnabled,collisions);collisionReason=collision;snapshots[index].state=2;RecordEvent(12,index,frame);
  if(collision!=1)fatal=true;}
}
void Free(SafetyHookContext& regs) noexcept {
 const fusionfix::shadows::FloatingPointState fp;
 if(!enabled.load()||fatal.load()||!Readable(regs.esp+4,4))return;
 const auto owner=*reinterpret_cast<const uint32_t*>(regs.esp+4);
 if(GetCurrentThreadId()!=observerThread.load()){
  for(uint32_t i=0;i<issuedKeys.load(std::memory_order_acquire);++i)
   if(snapshots[i].state.load()!=2&&snapshots[i].pool.load()==regs.ecx&&snapshots[i].owner.load()==owner){FatalThread();break;}
  return;
 }
 const auto index=registry.Free(regs.ecx,owner);if(index==UINT32_MAX)return;
 snapshots[index].state=2;RecordEvent(3,index,*CShadows::pFrameCounter);
}
void NativeTimerRestore(SafetyHookContext& regs) noexcept {
 const fusionfix::shadows::FloatingPointState fp;
 if(!enabled.load()||fatal.load())return;
 if(GetCurrentThreadId()!=observerThread.load()){FatalThread();return;}
 const auto frame=*CShadows::pFrameCounter,saved=*reinterpret_cast<const uint32_t*>(game+0xd73684);
 if(*reinterpret_cast<const uint8_t*>(game+0xd73610)!=1||frame!=saved)return;
 uint32_t caller=0;if(Readable(regs.esp,4))caller=*reinterpret_cast<const uint32_t*>(regs.esp);
 for(uint32_t i=0;i<registry.Used();++i){auto& entry=registry.entries[i];if(entry.retired)continue;
  Match current{};if(!MatchOwner(entry.id.owner,current)||!(current.identity==entry.id)){Retire(i,traffic_signal::End::Identity,6,frame);continue;}
  const auto previous=entry.lastFrame;
  if(registry.Rebase(i,current.identity,frame,saved,true)){snapshots[i].state=0;experiment_perf::Count(hotCountersEnabled,timerRebases);
   Record r{};r.event=13;r.index=i;r.tick=GetTickCount();r.frame=frame;r.previous=previous;r.caller=caller;r.thread=GetCurrentThreadId();Emit(r);}
 }
}
void Result() noexcept {
 if(!issuedKeys.load()||fatal.load())return;
 if(GetCurrentThreadId()!=observerThread.load()){FatalThread();return;}
 uint32_t active=0;std::array<uint32_t,7> keys{};unsigned count=0;
 for(unsigned slot=0;slot<7;++slot){const auto record=game+fusionfix::shadows::ce::allocation::DynamicPassFirstPositionRva+slot*0x110;
  if(!*reinterpret_cast<const uint8_t*>(record+0x25))continue;++active;
  const auto key=*reinterpret_cast<const uint32_t*>(record+0x30),index=KeyIndex(key);if(index==UINT32_MAX)continue;
  bool duplicate=false;for(unsigned j=0;j<count;++j)duplicate=duplicate||keys[j]==key;keys[count++]=key;
  if(duplicate){experiment_perf::Count(hotCountersEnabled,collisions);collisionReason=4;Retire(index,traffic_signal::End::Collision,12,*CShadows::pFrameCounter);continue;}
  if(snapshots[index].state.load()==2){experiment_perf::Count(hotCountersEnabled,queuedAfterRetirement);continue;}
  experiment_perf::Count(hotCountersEnabled,selected);const auto phase=snapshots[index].phase.load();if(phase<3)experiment_perf::Count(hotCountersEnabled,phaseSelections[phase]);
 }auto old=maxActiveSlots.load();while(active>old&&!maxActiveSlots.compare_exchange_weak(old,active)){}
}
void Render(uint32_t key,uint32_t slot,uint32_t kind,bool active) noexcept {
 if(!active||slot<1||slot>7||kind!=4||fatal.load())return;const auto index=KeyIndex(key);if(index==UINT32_MAX)return;
 if(snapshots[index].state.load()==2){experiment_perf::Count(hotCountersEnabled,queuedAfterRetirement);return;}
 experiment_perf::Count(hotCountersEnabled,renderVisits);const auto phase=snapshots[index].phase.load();if(phase<3)experiment_perf::Count(hotCountersEnabled,phaseRenders[phase]);
}
void Poll() {
 if(enabled.load()&&GetCurrentThreadId()!=observerThread.load()){FatalThread();return;}
 static uint32_t settingsTick{},statsTick{};const auto now=GetTickCount();
 if(now-settingsTick>=500){settingsTick=now;
  const bool value=[&]{if(preset117::startup.ownBackend)return preset117::RuntimeTrafficPromotion125(nullptr,requested);
   const auto ini=pe::ModulePath(self).replace_extension(L".ini");return preset117::RuntimeTrafficPromotion125(ini.c_str(),requested);}();
  if(promotionRequested.exchange(value)&&!value){registry.Disable();for(uint32_t i=0;i<registry.Used();++i)snapshots[i].state=2;
   RecordEvent(10,UINT32_MAX,*CShadows::pFrameCounter);Event(1,"TRAFFIC_PROMOTION_OFF","all_signal_lifetimes_retired restart_required");}
 }
 if(fatal.load()&&!registry.Disabled()){registry.Disable(traffic_signal::End::Thread);for(uint32_t i=0;i<registry.Used();++i)snapshots[i].state=2;}
 static bool stopReported=false;if(fatal.load()&&!stopReported){stopReported=true;Event(1,"TRAFFIC_SAFETY_STOP","reason="+std::to_string(collisionReason.load())+" restart_required");}
 if(!registry.Disabled()&&enabled.load())for(unsigned n=0,visits=experiment_perf::ValidationVisits(registry.Used());n<visits;++n){
  if(validationCursor>=registry.Used())validationCursor=0;const auto index=validationCursor++;auto& entry=registry.entries[index];if(entry.retired)continue;
  Match current{};const auto frame=*CShadows::pFrameCounter;
  if(!MatchOwner(entry.id.owner,current)||!(current.identity==entry.id)){Retire(index,traffic_signal::End::Identity,6,frame);continue;}
  const auto age=uint32_t(frame-entry.lastFrame);
  if(age>=0x80000000u){Retire(index,traffic_signal::End::Clock,14,frame);continue;}
  if(age>2&&registry.Suspend(index)){snapshots[index].state=0;experiment_perf::Count(hotCountersEnabled,leaseBreaks);RecordEvent(2,index,frame);}
 }
 if(!diagnosticsEnabled)return; // Mandatory off/identity/lease work above runs at every logging level.
 if(hotCountersEnabled&&now-statsTick>=5000){statsTick=now;uint32_t alive=0,leased=0,retiredCount=0;
  for(uint32_t i=0;i<registry.Used();++i){const auto& e=registry.entries[i];if(e.retired)++retiredCount;else {++alive;if(e.leased)++leased;}}
  char fields[1024];sprintf_s(fields,
   "registered=%u alive=%u leased=%u retired_instances=%u admitted=%u admitted_phases=%u,%u,%u selected_passes=%u selected_phases=%u,%u,%u render_visits=%u render_phases=%u,%u,%u max_active_slots=%u collisions=%u collision_reason=%u global_retired=%u off=%u lease_breaks=%u capacity_rejected=%u unsupported_pool=%u argument_rejected=%u timer_rebases=%u queued_after_retirement=%u ff_extra=%u ff_headlight=%u ff_vehicle=%u allocator_enabled=%u key_capacity=16384 budget=7 no_time_expiry=1",
   registry.Used(),alive,leased,retiredCount,admitted.load(),phaseAdmissions[0].load(),phaseAdmissions[1].load(),phaseAdmissions[2].load(),
   selected.load(),phaseSelections[0].load(),phaseSelections[1].load(),phaseSelections[2].load(),renderVisits.load(),phaseRenders[0].load(),phaseRenders[1].load(),phaseRenders[2].load(),maxActiveSlots.load(),collisions.load(),collisionReason.load(),registry.Disabled()?1:0,promotionRequested.load()?0:1,leaseBreaks.load(),capacityRejected.load(),membershipRejected.load(),argumentRejected.load(),timerRebases.load(),queuedAfterRetirement.load(),bExtraNightShadows?1:0,bHeadlightShadows?1:0,bVehicleNightShadows?1:0,PlayerShadowAllocation::Enabled()?1:0);
  Event(2,"TRAFFIC_GENERAL_STATS",fields);
 }
 const auto h=head.load(std::memory_order_acquire);auto t=tail.load(std::memory_order_relaxed);
 while(t<h){const auto r=records[t%records.size()];char fields[768];
  if(r.index<registry.Used()){const auto& e=registry.entries[r.index];sprintf_s(fields,
   "event=%u token=%u key=%08x generation=%u tick=%u frame=%u previous_frame=%u timer_caller=%08x thread=%u pool_kind=%u pool=%08x owner=%08x slot=%u reference=%u model=%08x phases=%u retired=%u reason=%u entity=%.3f,%.3f,%.3f dropped=%u",
   r.event,r.index,e.key,e.generation,r.tick,r.frame,r.previous,r.caller,r.thread,r.kind,e.id.pool,e.id.owner,e.id.slot,e.id.reference,e.id.model,e.phaseMask,e.retired?1:0,uint32_t(e.reason),r.entity[0],r.entity[1],r.entity[2],dropped.load());
  }else sprintf_s(fields,"event=%u global=1 tick=%u frame=%u thread=%u dropped=%u",r.event,r.tick,r.frame,r.thread,dropped.load());
  Event(2,"TRAFFIC_INSTANCE",fields);++t;tail.store(t,std::memory_order_release);
 }
}
}
void TrafficObserve(SafetyHookContext& regs,uint32_t caller) noexcept {TrafficSignal::Observe(regs,caller);}
void TrafficPoll(){TrafficSignal::Poll();}
void PublishCombinedStatus() noexcept {
 if(!hotCountersEnabled)return;
 static uint32_t last{};const auto now=GetTickCount();if(now-last<5000)return;last=now;
 char fields[512];sprintf_s(fields,"enabled=%u requested=%u policy=%u vacancy=%u bound=%u promoted=%u selected=%u render_visits=%u max_police_selected=%u contentions=%u candidates=8 promotion_budget=2 native_slots=7",
  nearbyPoliceEnabled?1:0,proofRequested.load()?1:0,policePolicy.load(),vacancyRequested.load()?1:0,
  nearbyBound.load(),nearbyPromoted.load(),nearbySelected.load(),nearbyRendered.load(),nearbyMaxSelected.load(),nearbyContention.load());
 Event(2,"EMERGENCY_STATS",fields);
}
void PrivateResults() noexcept {if(policeTestEnabled)ObserveProofResult();TrafficSignal::Result();Liberty20::Result();}
void PrivateRender(uint32_t key,uint32_t slot,uint32_t kind,bool active) noexcept {if(policeTestEnabled)NineRender(key,slot,kind,active);TrafficSignal::Render(key,slot,kind,active);Liberty20::Render(key,slot,kind,active);}

void TrafficAuthenticate(){
 if(!TrafficSignal::requested&&!ModelLightShadow20::requested&&!CoronaLight20::requested)return;
 try {
  if(!nativeBackend){
  Require(ff,"traffic_pool_required_overlay_missing");
  Require(reinterpret_cast<uintptr_t>(GetModuleHandleW(L"GTAIV.EFLC.FusionFix.asi"))==ff,"traffic_pool_FusionFix_module_changed");
  const auto raw=pe::Read(pe::ModulePath(reinterpret_cast<HMODULE>(ff)));
  pe::Image expected(raw,ff);
  // VerifyOfficial already authenticated all executable sections. Recheck the
  // exact atPool entry and entire body here, before any private hooks activate.
  for(const auto& [rva,size]:{std::pair<uint32_t,size_t>{traffic_signal::PoolCallbackRva,5},
                             {traffic_signal::PoolCallbackBodyRva,0xae}}){
   const auto actual=ReadMemory(ff+rva,size);
   Require(!memcmp(actual.data(),expected.bytes.data()+rva,size),"traffic_pool_callback_code_changed");
  }
  }
  const std::pair<uint32_t,size_t> ranges[]={{0x9208f0,0x747},{0x921040,0x197},{0x70ba00,0x22},{0x60d370,0x32},{0x714fc0,0x35},
   {0x6bcf33,0x70},{0x6bd2c0,0x73},{0x6c2dd0,0x98},{0x4d0aa0,0xff},{0x4d0bc0,0xf2}};
  for(const auto& [rva,size]:ranges){
   const auto actual=ReadMemory(game+rva,size);
   for(size_t n=0;n<size;++n)if(actual[n]!=original->bytes[rva+n]){
    Event(1,"TRAFFIC_GUARD_MISMATCH","rva="+std::to_string(rva)+" offset="+std::to_string(n)+" expected="+std::to_string(original->bytes[rva+n])+" actual="+std::to_string(actual[n]));
    throw std::runtime_error("traffic_native_lifecycle_bytes_changed");
   }
  }
  auto readCode=[](uint32_t address,size_t size){
   MEMORY_BASIC_INFORMATION info{};
   Require(VirtualQuery(reinterpret_cast<void*>(address),&info,sizeof(info))&&info.State==MEM_COMMIT&&
    !(info.Protect&(PAGE_GUARD|PAGE_NOACCESS))&&
    (info.Protect&(PAGE_EXECUTE|PAGE_EXECUTE_READ|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY)),"traffic_pool_code_not_executable");
   Require(uint64_t(address)+size<=uint64_t(reinterpret_cast<uintptr_t>(info.BaseAddress))+info.RegionSize,"traffic_pool_code_crosses_region");
   return ReadMemory(address,size);
  };
  if(nativeBackend){
   traffic_signal::VerifyNativePoolCtor(uint32_t(game),
    std::span<const uint8_t>(original->bytes.data()+traffic_signal::PoolCtorRva,0x66),readCode);
   TrafficSignal::authenticated=true;
   Event(1,"TRAFFIC_NATIVE_POOL_AUTHENTICATED",
    "backend=native ctor_rva=0x86c5f0 ctor_bytes=102 entire_function=exact lifecycle_guards=exact pool_layout=unchanged pool_capacity_unchanged=1 no_pool_hook=1");
  }else{
  const auto evidence=traffic_signal::VerifyPoolHook(uint32_t(game),uint32_t(ff),true,
   std::span<const uint8_t>(original->bytes.data()+traffic_signal::PoolCtorRva,0x66),readCode);
  TrafficSignal::authenticated=true;
  Event(1,"TRAFFIC_POOL_HOOK_AUTHENTICATED","actual_code_capability=1 sha256="+pe::Hash(pe::Read(pe::ModulePath(reinterpret_cast<HMODULE>(ff))))+
   " ctor_rva=0x86c5fa callback_rva=0x98570 body_rva=0x9cae0 return_rva=0x86c5ff safetyhook_stub=exact continuation=5_nops all_other_lifecycle_bytes=exact pool_layout=unchanged");
  }
 }catch(const std::exception& e){
  TrafficSignal::authenticated=false;
  Event(1,"TRAFFIC_OBSERVER_REFUSED",e.what());
 }
}
void TrafficInstall(){
 if(!TrafficSignal::requested||!TrafficSignal::authenticated)return;
 try {
  // Repeat the free entry guard immediately before preparing its observer.
  const auto freeCode=ReadMemory(game+0x70ba00,0x22);
  Require(!memcmp(freeCode.data(),original->bytes.data()+0x70ba00,0x22),"traffic_free_entry_changed_after_authentication");
  Prepare(TrafficSignal::freeHook,game+0x70ba00,TrafficSignal::Free);
  const auto timerCode=ReadMemory(game+0x4d0aa0,0xff);
  Require(!memcmp(timerCode.data(),original->bytes.data()+0x4d0aa0,0xff),"traffic_timer_restore_entry_changed_after_authentication");
  Prepare(TrafficSignal::timerRestoreHook,game+0x4d0b97,TrafficSignal::NativeTimerRestore);
  Require(bool(TrafficSignal::freeHook.enable()),"traffic_free_observer_enable_failed");
  Require(bool(TrafficSignal::timerRestoreHook.enable()),"traffic_timer_restore_observer_enable_failed");
  TrafficSignal::observerThread.store(GetCurrentThreadId(),std::memory_order_release);
  TrafficSignal::enabled=true;Event(1,"TRAFFIC_OBSERVER_ACTIVE","build=private-distance-v10 dynamic_signals immediate_valid_phase no_time_expiry key_capacity=16384 budget=7 reserved_keys_no_reuse thread="+std::to_string(GetCurrentThreadId()));
 }catch(const std::exception& e){
  if(TrafficSignal::freeHook)(void)TrafficSignal::freeHook.disable();
  if(TrafficSignal::timerRestoreHook)(void)TrafficSignal::timerRestoreHook.disable();TrafficSignal::enabled=false;
  Event(1,"TRAFFIC_OBSERVER_REFUSED",e.what());
 }
}
