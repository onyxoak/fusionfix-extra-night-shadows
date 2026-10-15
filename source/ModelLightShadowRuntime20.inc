// Included after the catalog declarations; actual native illumination only.
#include "ModelLightRegistry20.hpp"
#include "ModelLightSubmission20.hpp"
namespace ModelLightShadow20 {
bool requested=false;
std::atomic<bool> active{false};
alignas(64) uint8_t keyStorage[32768][64]{};
model_light20::Registry<> registry;
std::atomic_flag lock=ATOMIC_FLAG_INIT;
std::atomic<uint32_t> identitiesPublished32{0};
std::atomic<uint32_t> promoted{0},refused{0},retired{0},faults{0};
void LifecycleFault() noexcept {active=false;TrafficSignal::ownerClass109Active=false;TrafficSignal::ownerClass114Active=false;++faults;}
void OnFree(uint32_t pool,uint32_t owner) noexcept {
 if(!active.load(std::memory_order_acquire))return;
 if(lock.test_and_set(std::memory_order_acquire)){LifecycleFault();return;}
 registry.Free(pool,owner);lock.clear(std::memory_order_release);
}
#include "ModelQueuedWindow49Runtime.inc"
#include "ModelPointCommit50Runtime.inc"
template<class Publish68>
uint32_t RegisterValidated68(const model_effect_observer::Invocation& c,
 const TrafficSignal::Match& match,const std::array<uint8_t,0x6c>& descriptor,
 uint32_t site,const std::array<uint32_t,18>& arguments,bool packetValid,Publish68 publish68) noexcept {
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::ModelPromote,128);
 if(!requested||!active.load(std::memory_order_acquire)||!addonActive.load(std::memory_order_acquire)||!PlayerShadowAllocation::Enabled())return 0;
 const auto abi=model_light_submission20::Layout(site);
 // Retain native cached/static shadow identity; only previously unshadowed
 // native point/spot packets acquire our private dynamic map identity.
 if(!abi||(arguments[2]&6u)||arguments[abi.keyOffset/4]||!packetValid||c.invalid||!c.resolved||
    !std::isfinite(std::bit_cast<float>(arguments[10]))||std::bit_cast<float>(arguments[10])<=0){++refused;return 0;}
 TrafficSignal::Match fresh{};std::array<uint8_t,0x6c> again{};bool descriptorCopied49=false;
 // Re-read final descriptor bytes alongside fresh pool headers. This only
 // batches permission work: owner generations, model evidence and final byte
 // comparison remain authoritative at this callback, before key publication.
 if(!TrafficSignal::MatchOwnerWithDescriptor48(c.owner,fresh,c.descriptor,
     again.data(),again.size(),descriptorCopied49)||fresh.identity!=match.identity||
    fresh.modelInfo!=c.modelInfo||!descriptorCopied49||again!=descriptor||
    ((match.kind==2||match.kind==3)&&(fresh.kind!=match.kind||fresh.ownerPose109!=match.ownerPose109))){++refused;return 0;}
 if(lock.test_and_set(std::memory_order_acquire)){++refused;return 0;}
 const auto oldUsed=registry.used;
 auto* entry=registry.Observe(fresh.identity,c.ordinal,c.descriptor,c.modelInfo,descriptor,c.frame);
 identitiesPublished32.store(registry.used,std::memory_order_relaxed);
 bool ok=entry!=nullptr;
 if(ok&&registry.used!=oldUsed&&Liberty20::Collision(entry->key)){entry->active=false;ok=false;}
 uint32_t resultKey{};
 if(ok&&active.load(std::memory_order_acquire)&&registry.Publish(entry->key)){
  resultKey=entry->key;
  publish68(resultKey);++promoted;
 }else ++refused;
 lock.clear(std::memory_order_release);
 return resultKey;
}
// Constructor fallback obtains only a validated numeric key; the registry
// entry and ownership lock never escape this helper.
uint32_t RegisterKey68(const model_effect_observer::Invocation& c,
 const TrafficSignal::Match& match,const std::array<uint8_t,0x6c>& descriptor,
 uint32_t site,const std::array<uint32_t,18>& arguments,bool packetValid) noexcept {
 return RegisterValidated68(c,match,descriptor,site,arguments,packetValid,[](uint32_t) noexcept{});
}
// Legacy callers retain both native argument stores INSIDE the original lock.
// The normalized-constructor fallback performs no old-stack publication.
uint32_t Promote(SafetyHookContext& regs,const model_effect_observer::Invocation& c,
 const TrafficSignal::Match& match,const std::array<uint8_t,0x6c>& descriptor,
 uint32_t site,const std::array<uint32_t,18>& arguments,bool packetValid) noexcept {
 return RegisterValidated68(c,match,descriptor,site,arguments,packetValid,[&](uint32_t key) noexcept {
  const auto abi=model_light_submission20::Layout(site);
  auto* words=reinterpret_cast<uint32_t*>(regs.esp);
  words[abi.keyOffset/4]=key;words[2]=arguments[2]|4u;
 });
}
bool ValidateOne48(rage::CLightSource& l) noexcept {
 const auto key=uint32_t(l.mCastShadows);
 if(!registry.Contains(key)||!(l.mFlags&4u))return true;
 const traffic_signal::fresh_page::Operation44 queuedReadOperation47(32);
 bool valid=false;
 if(active.load(std::memory_order_acquire)&&!lock.test_and_set(std::memory_order_acquire)){
  auto* entry=registry.Lookup(key);TrafficSignal::Match fresh{};std::array<uint8_t,0x6c> bytes{};
  valid=entry&&entry->active&&TrafficSignal::MatchOwner(entry->id.owner,fresh)&&
   fresh.modelInfo==entry->modelInfo&&Liberty20::Copy(entry->descriptor,bytes.data(),bytes.size())&&
   registry.Validate(key,fresh.identity,bytes,*CShadows::pFrameCounter);
  lock.clear(std::memory_order_release);
 }
 if(!valid){l.mFlags&=~4u;++retired;}
 return valid;
}
void ValidateQueued(rage::CLightSource* lights,uint32_t count) noexcept {
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::ModelQueued,32);
 if(!lights||count>4096)return;
 if(queuedWindow49&&!PlayerShadowAllocation::shortlistRequested48){ValidateQueuedWindow49(lights,count);return;}
 for(unsigned i=0;i<count;++i){auto& l=lights[i];const auto key=uint32_t(l.mCastShadows);
  if(!registry.Contains(key)||!(l.mFlags&4u))continue;
  if(PlayerShadowAllocation::DeferModel48(l,i))continue;
  ValidateOne48(l);
 }
}
void Publish() {
 if(!requested&&!CoronaLight20::requested)return;static uint32_t last{};const auto now=GetTickCount();if(now-last<5000)return;last=now;
 // Diagnostics never acquire the ownership lock or contend with native frees.
 const auto identitiesUsed=identitiesPublished32.load(std::memory_order_relaxed);
 Event(1,"MODEL_SHADOW20_COUNTS","active="+std::to_string(active.load())+" promoted="+std::to_string(promoted.load())+
  " refused="+std::to_string(refused.load())+" stale_queued="+std::to_string(retired.load())+
  " owner_read_faults="+std::to_string(TrafficSignal::ownerReadFaults32.load())+" lifecycle_faults="+std::to_string(faults.load())+" identities_used="+std::to_string(identitiesUsed)+" identity_capacity=32768 native_colors_gates=1 corona_module_requested="+std::to_string(CoronaLight20::requested)+" simultaneous_coverage=0"+
 " queued49_windows="+std::to_string(queuedWindows49.load())+" queued49_sources="+std::to_string(queuedSources49.load())+
 " queued49_fallback="+std::to_string(queuedFallback49.load())+" queued49_faults="+std::to_string(queuedWindowFaults49.load())+
 " queued54_requested="+std::to_string(queuedCommitCertificate54)+" queued54_permission_windows="+std::to_string(queuedPermissionWindows54.load())+
 " queued54_permission_queries="+std::to_string(queuedPermissionQueries54.load())+" queued54_fresh_stages="+std::to_string(queuedFreshStages54.load())+
 " queued54_domain_faults="+std::to_string(queuedDomainFaults54.load())+
 " shortlist48_passes="+std::to_string(PlayerShadowAllocation::shortlistPasses48.load())+
 " shortlist48_deferred="+std::to_string(PlayerShadowAllocation::shortlistDeferred48.load())+
 " shortlist48_validated="+std::to_string(PlayerShadowAllocation::shortlistValidated48.load())+
 " shortlist48_invalid="+std::to_string(PlayerShadowAllocation::shortlistInvalid48.load())+
 " shortlist48_fallback="+std::to_string(PlayerShadowAllocation::shortlistFallback48.load())+
 " shortlist48_resident_early="+std::to_string(PlayerShadowAllocation::shortlistResident48.load()));
 Event(1,"MODEL_SCRATCH59_COUNTS",scratch_report59::Payload(queuedScratch32Requested59,
  scratchAttempted59.load(std::memory_order_relaxed),scratchCompleted59.load(std::memory_order_relaxed),
  scratchAllocationFallback59.load(std::memory_order_relaxed)));
}
}


