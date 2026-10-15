// Included inside standalone, after traffic-signal-observer.inc. Native point/spot source catalog. Observation
// only: no writes to native arguments, descriptors, owners, keys or light lists.
#include "ModelLightSubmission20.hpp"
#include "FreshReadBatch30.hpp"
#include "ModelPrevalidation40.hpp"
#include "ModelOwnerTransformDiagnostic46.hpp"
#include "ModelEffectGetterDiagnostic62.hpp"
#include <optional>
namespace ModelEffectObserver {
using namespace model_effect_observer;
bool requested=false;
bool prepared=false;
std::atomic<bool> enabled{false};
SafetyHookMid preHook,postHook,resolveHook,wrapperHook,directHook,wrapperSpotHook,directSpotHook;
struct LastErrorScope {
 DWORD value=GetLastError();
 ~LastErrorScope(){SetLastError(value);}
};
struct Sample {
 bool diagnostic{};
 Invocation invocation{};
 traffic_signal::Identity identity{};
 uint32_t thread{},ownerKind{},site{},type{},flags{},key{},promotedKey{},status{},resolveMismatch{};
 bool ownerMatched{},modelInfoMatched{},descriptorCopied{},matrixCopied{},packetCopied{};
 std::array<uint8_t,0x6c> descriptor{};
 std::array<float,16> matrix{};std::array<uint8_t,68> ownerPose109{};
 model_owner_transform46::Snapshot ownerTransform{};
 model_effect_getter62::Snapshot getter62{};
 std::array<float,3> position{},color{},direction{},up{};
 float radius{},intensity{},innerCone{},outerCone{};
};
struct Local {
 model_light_submission20::State state;
 std::array<Sample,MaxDepth> samples{};
 uint32_t cadence{},quotaFrame{},quota{},lastSampleFrame{};
};
thread_local Local local;
std::atomic<uint32_t> serial{0},sampled{0},readRejected{0},contextRejected{0},depthRejected{0};
std::atomic<uint32_t> ringFull{0},ringContended{0};
std::atomic<uint32_t> nativeInactiveSkipped40{0};
std::array<Sample,256> ring{};
uint32_t head{},tail{};
std::atomic_flag ringLock=ATOMIC_FLAG_INIT;

bool Copy(uintptr_t address,void* output,size_t size) noexcept {
 return Liberty20::Copy(address,output,size);
}
uint32_t Frame() noexcept {return *CShadows::pFrameCounter;}
void Queue(const Sample& sample) noexcept {
 if(ringLock.test_and_set(std::memory_order_acquire)){++ringContended;return;}
 if(head-tail<ring.size()){ring[head%ring.size()]=sample;++head;}else ++ringFull;
 ringLock.clear(std::memory_order_release);
}
void Pre(SafetyHookContext& regs) noexcept {
 const LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::ModelPre,128);
 if(!enabled.load(std::memory_order_acquire))return;
 Invocation invocation{};
 if(regs.esp<PreStackAdjustment)return;
 invocation.preStack=regs.esp-PreStackAdjustment;invocation.owner=regs.esi;invocation.effect=regs.edi;
 invocation.frame=Frame();invocation.nonce=serial.fetch_add(1,std::memory_order_relaxed)+1;
 if(local.quotaFrame!=invocation.frame){local.quotaFrame=invocation.frame;local.quota=0;}
 uint32_t mix=++local.cadence^(invocation.frame*0x9e3779b9u);mix^=mix>>16;mix*=0x7feb352du;mix^=mix>>15;
 invocation.sampled=(diagnosticsEnabled||CaptureDiagnostic44::sampleSources.load(std::memory_order_relaxed))&&(mix&63u)==0&&uint32_t(invocation.frame-local.lastSampleFrame)>=10;
 const bool diagnostic=invocation.sampled;
 if(invocation.sampled)local.lastSampleFrame=invocation.frame;
 if(invocation.sampled){++local.quota;++sampled;}
 invocation.sampled=invocation.sampled||ModelLightShadow20::requested||CoronaLight20::requested;
 // Preserve a context for every invocation, even unsampled/read-rejected
 // calls, so nested returns cannot accidentally attach to an older sample.
 if(invocation.sampled){
  std::array<uint32_t,25> words{};
  if(!Copy(invocation.preStack,words.data(),sizeof(words))){invocation.invalid=true;++readRejected;}
  else {
   // The final PUSH EAX / PUSH EDI have not executed at PreSite. Their
   // authenticated registers supply those two words; no stack writes here.
   invocation.effect=regs.edi;invocation.matrix=regs.eax;invocation.proposedKey=words[PreKeyOffset/4];
   invocation.ordinal=words[PreOrdinalOffset/4];
   invocation.modelInfo=words[PreModelOffset/4];
   if(invocation.effect!=regs.edi)invocation.invalid=true;
  }
 }
 if(!local.state.Enter(invocation)){++depthRejected;return;}
 if(invocation.sampled){
  auto& sample=local.samples[local.state.Depth()-1];sample=Sample{};
  sample.diagnostic=diagnostic;
  sample.thread=GetCurrentThreadId();sample.invocation=*local.state.Current();
 }
}
bool InnerEvidence(const SafetyHookContext& regs,ResolveEvidence& evidence) noexcept {
 uint32_t arguments[8]{},saved[2]{};
 if(regs.ebp>UINT32_MAX-0x24)return false;
 const traffic_signal::fresh_batch30::Span spans[]{
  {regs.ebp+4,arguments,sizeof(arguments)},{regs.esp,saved,sizeof(saved)}};
 if(!traffic_signal::fresh_batch30::Copy(spans,2,TrafficSignal::ReadableOriginal))return false;
 evidence.frame=Frame();evidence.preStack=regs.ebp+8;
 evidence.returnRva=arguments[0]-uint32_t(game);evidence.effect=arguments[1];
 evidence.matrix=arguments[2];evidence.savedOwner=saved[1];evidence.savedEffect=saved[0];
 evidence.descriptor=regs.edi;evidence.proposedKey=arguments[7];
 return true;
}
#include "ModelPointFusion50Runtime.inc"
void ValidateSource46(const Invocation& current,Sample& sample) noexcept {
 std::optional<traffic_signal::fresh_page::Operation44> readOperation;
 if(!traffic_signal::fresh_page::operation44)readOperation.emplace();
 sample.ownerMatched=false;sample.modelInfoMatched=false;sample.descriptorCopied=false;sample.matrixCopied=false;
 TrafficSignal::Match match{};
 sample.ownerMatched=TrafficSignal::MatchOwnerWithDescriptor48(current.owner,match,
  current.descriptor,sample.descriptor.data(),sample.descriptor.size(),sample.descriptorCopied);
 if(sample.ownerMatched){
  sample.identity=match.identity;sample.ownerKind=match.kind;sample.ownerPose109=match.ownerPose109;
  // MatchOwner already freshly read and validated this model table entry.
  // Consume its operation-local evidence; never cache it across native calls.
  sample.modelInfoMatched=match.modelInfo==current.modelInfo;
  if(!sample.modelInfoMatched){local.state.Invalidate();++contextRejected;}
 }
 // Unsupported owners are recorded as such, never guessed or promoted.
 // The independent descriptor was freshly staged with the pool headers.
 // Promote still re-reads owner identity AND descriptor before publication.
 // Native point/spot promotion authenticates the matrix pointer and actual
 // emitted vectors, but needs the full matrix only for diagnostics or glow
 // reconstruction. Preserve corona validation independently of logging.
 const bool needsMatrix=sample.diagnostic||(CoronaLight20::requested&&sample.descriptorCopied&&sample.descriptor[0x5b]==0);
 if(needsMatrix)
  sample.matrixCopied=Copy(current.matrix,sample.matrix.data(),sizeof(sample.matrix));
 if(sample.diagnostic){
  sample.ownerTransform=model_owner_transform46::Read(current.owner,sample.ownerMatched&&sample.modelInfoMatched,
   [](uint32_t address,void* bytes,size_t size) noexcept {return Copy(address,bytes,size);},
   [&]() noexcept {
    TrafficSignal::Match fresh{};
    return TrafficSignal::MatchOwner(current.owner,fresh)&&fresh.identity==sample.identity&&fresh.modelInfo==current.modelInfo;
   });
  // Optional owner-pose read failure affects this diagnostic only. It must
  // neither invalidate native emission nor alter shadow-promotion authority.
 }
 if(!sample.descriptorCopied||(needsMatrix&&!sample.matrixCopied)){
  ++readRejected;local.state.Invalidate();
 }
}
void Resolve(SafetyHookContext& regs) noexcept {
 const LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::ModelResolve,128);
 if(!enabled.load(std::memory_order_acquire))return;
 const auto* current=local.state.Current();if(!current||!current->sampled)return;
 // This hook precedes the native JE, whose saved ZF already excludes every
 // active emission on this invocation. Preserve full diagnostic observations
 // and Pre/Post nesting; accepted sources retain every fresh check below.
 if(model_prevalidation40::NativeInactive(uint32_t(regs.eflags),local.samples[local.state.Depth()-1].diagnostic)){
  if(hotCountersEnabled)nativeInactiveSkipped40.fetch_add(1,std::memory_order_relaxed);
  return;
 }
 // All following reads are SEH-contained; no native emission/callback runs
 // before this scope ends. Values and owner generation are still reread fresh.
 const traffic_signal::fresh_page::Operation44 readOperation(32);
 // Region fallbacks may repeat for locked, nonresident or crossing pages.
 // Borrow permissions only within this callback; all data is read again.
 const immediate_region33::Scope regions196;
 ResolveEvidence evidence{};
 if(!InnerEvidence(regs,evidence)){local.state.Invalidate();++readRejected;return;}
 if(!local.state.Resolve(evidence)){
  auto& sample=local.samples[local.state.Depth()-1];
  sample.resolveMismatch=(current->invalid?1u:0u)|((evidence.preStack!=current->preStack)?2u:0u)|
    ((evidence.savedOwner!=current->owner)?4u:0u)|((evidence.savedEffect!=current->effect)?8u:0u)|
    ((evidence.effect!=current->effect)?16u:0u)|((evidence.matrix!=current->matrix)?32u:0u)|
    ((evidence.proposedKey!=current->proposedKey)?64u:0u)|((evidence.returnRva!=DispatchReturn)?128u:0u);
  ++contextRejected;return;
 }
 auto& sample=local.samples[local.state.Depth()-1];
 // Diagnostics and corona-only sources require metadata before their separate
 // callback. Point/spot metadata is freshly validated only after actual store
 // flags/key eligibility, avoiding work on packets carrying native shadow flags
 // or an existing key. Native renderer admission gates execute later.
 if(sample.diagnostic){
  sample.getter62=model_effect_getter62::Observe(current->effect,
   [](uint32_t address,void* bytes,size_t size) noexcept {return Copy(address,bytes,size);});
  ValidateSource46(*current,sample);return;
 }
 if(CoronaLight20::requested){
  uint8_t nativeType=0;
  if(current->descriptor>UINT32_MAX-0x5b||!Copy(current->descriptor+0x5b,&nativeType,1)){++readRejected;local.state.Invalidate();return;}
  if(nativeType==0)ValidateSource46(*current,sample);
 }

}
uint32_t FinishPoint68(SafetyHookContext& regs,uint32_t site,
 const std::array<uint32_t,18>& arguments,const PointEvidence& evidence,
 const Invocation* current,Sample& sample,
 std::array<uint32_t,32>* owned68=nullptr) noexcept {
 // Also reached from the deferred constructor fallback, after its native
 // call has returned. Never carry a permission proof through that call.
 std::optional<immediate_region33::Scope> regions196;
 if(!immediate_region33::current)regions196.emplace();
 if(!sample.diagnostic&&pointFusion50&&!traffic_signal::fresh_page::preferRegion){
  if(owned68)++ownedFusedAttempts79;
  if(FusePoint50(regs,*current,sample,site,arguments,evidence,owned68)==PointFusion50Result::Complete){
   if(owned68){++ownedFusedComplete79;if((*owned68)[24])++ownedFusedKeys79;return (*owned68)[24];}
   return 0;
  }
  if(owned68)++ownedFusedLegacy79;
 }
 if(!sample.diagnostic){ValidateSource46(*current,sample);if(current->invalid)return 0;}
 // Common first eleven arguments have the same layout in both store ABIs.
 const traffic_signal::fresh_batch30::Span vectorSpans[]{
  {arguments[5],sample.position.data(),sizeof(sample.position)},
  {arguments[6],sample.color.data(),sizeof(sample.color)},
  {arguments[3],sample.direction.data(),sizeof(sample.direction)},
  {arguments[4],sample.up.data(),sizeof(sample.up)}};
 sample.packetCopied=traffic_signal::fresh_batch30::Copy(vectorSpans,4,TrafficSignal::ReadableOriginal);
 std::memcpy(&sample.intensity,&arguments[7],4);std::memcpy(&sample.radius,&arguments[10],4);
 std::memcpy(&sample.innerCone,&arguments[11],4);std::memcpy(&sample.outerCone,&arguments[12],4);
 if(!sample.packetCopied){local.state.Invalidate();++readRejected;}
 if(sample.packetCopied&&sample.ownerMatched&&sample.modelInfoMatched&&sample.descriptorCopied){
  bool finite=std::isfinite(sample.radius)&&sample.radius>0&&std::isfinite(sample.intensity)&&sample.intensity>=0;
  for(unsigned i=0;i<3;++i)finite=finite&&std::isfinite(sample.position[i])&&std::isfinite(sample.color[i])&&std::isfinite(sample.direction[i])&&std::isfinite(sample.up[i]);
  if(evidence.type==2)finite=finite&&std::isfinite(sample.innerCone)&&std::isfinite(sample.outerCone);
  TrafficSignal::Match match{};match.identity=sample.identity;match.kind=sample.ownerKind;match.ownerPose109=sample.ownerPose109;
  if(owned68){const auto key=ModelLightShadow20::RegisterKey68(*current,match,sample.descriptor,site,arguments,finite);
   if(key){(*owned68)[24]=key;(*owned68)[18]|=4u;}return key;
  }
  const auto promotedKey=ModelLightShadow20::Promote(regs,*current,match,sample.descriptor,site,arguments,finite);
  if(sample.diagnostic)sample.promotedKey=promotedKey;
 }
 return 0;
}
#include "ModelPointValue68Runtime.inc"
void Point(SafetyHookContext& regs,uint32_t site) noexcept {
 const LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::ModelPoint,128);
 if(!enabled.load(std::memory_order_acquire))return;
 const auto* current=local.state.Current();if(!current||!current->sampled)return;
 ModelValue68::ClearDepth(local.state.Depth());
 // This synchronous hook returns before the native store executes. Reuse only
 // positive page-permission observations across context, metadata and final
 // promotion reads; all bytes and owner generations are still reread fresh.
 // SEH-contained copies reject an access revoked after the initial query.
 const traffic_signal::fresh_page::Operation44 pointReadOperation47(32);
 const immediate_region33::Scope regions196;
 const auto abi=model_light_submission20::Layout(site);const auto words=abi.words;
 std::array<uint32_t,18> arguments{};uint32_t returnAndEffect[3]{};
 const traffic_signal::fresh_batch30::Span contextSpans[]{
  {regs.esp,arguments.data(),words*4},{regs.ebp+4,returnAndEffect,sizeof(returnAndEffect)}};
 if(!words||regs.ebp>UINT32_MAX-0x10||
    !traffic_signal::fresh_batch30::Copy(contextSpans,2,TrafficSignal::ReadableOriginal)){
  local.state.Invalidate();++readRejected;return;
 }
 PointEvidence evidence{Frame(),regs.ebp+8,returnAndEffect[0]-uint32_t(game),arguments[0],
   uint32_t(regs.edi),site,arguments[1],arguments[2],arguments[abi.keyOffset/4]};
 if(returnAndEffect[1]!=arguments[0]||returnAndEffect[2]!=current->matrix||!local.state.Submission(evidence)){
  local.state.Invalidate();++contextRejected;return;
 }
 auto& sample=local.samples[local.state.Depth()-1];sample.site=site;sample.type=evidence.type;sample.flags=evidence.flags;sample.key=evidence.key;
 // An authenticated native shadow/keyed packet cannot acquire a private
 // model shadow identity. Its native submission still executes unchanged.
 // Keep full emitted-vector evidence for diagnostic samples; eligible
 // unshadowed packets retain every current validation below and in Promote.
 if(!sample.diagnostic&&((arguments[2]&6u)||arguments[abi.keyOffset/4]))return;
 // Region-preferring profiles retain their established full legacy path.
 if(ModelValue68::TryDefer(regs,site,arguments,*current,sample))return;
 (void)FinishPoint68(regs,site,arguments,evidence,current,sample);
}
void Wrapper(SafetyHookContext& regs) noexcept {Point(regs,WrapperPoint);}
void Direct(SafetyHookContext& regs) noexcept {Point(regs,DirectPoint);}
void WrapperSpot(SafetyHookContext& regs) noexcept {Point(regs,model_light_submission20::WrapperSpot);}
void DirectSpot(SafetyHookContext& regs) noexcept {Point(regs,model_light_submission20::DirectSpot);}
void Post(SafetyHookContext& regs) noexcept {
 const LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::ModelPost,128);
 if(!enabled.load(std::memory_order_acquire))return;
 const auto depth=local.state.Depth();Invocation finished{};
 if(regs.esp<PostStackAdjustment){ModelValue68::ClearDepth(depth);local.state.Invalidate();++contextRejected;return;}
 const auto result=local.state.Leave(regs.esp-PostStackAdjustment,Frame(),finished);
 if(result==LeaveResult::Missing||result==LeaveResult::Overflow)return;
 ModelValue68::ClearDepth(depth);
 if(!finished.sampled||!depth)return;
 if(result==LeaveResult::Invalid)++contextRejected;
 if(!local.samples[depth-1].diagnostic)return;
 auto sample=local.samples[depth-1];sample.invocation=finished;
 sample.status=result==LeaveResult::Invalid?1u:0u;
 Queue(sample);
}
void DisableHooks() noexcept {
 enabled.store(false,std::memory_order_release);
 for(auto* hook:{&directSpotHook,&wrapperSpotHook,&directHook,&wrapperHook,&resolveHook,&postHook,&preHook})if(*hook)(void)hook->disable();
 prepared=false;
}
void PrepareHooks() noexcept {
 if(!requested)return;
 try {
  Require(original!=nullptr,"model_observer_missing_image");
  bool officialFlags=false;
  if(!nativeBackend){
   // Version-specific official FlagsHook: callback, generated stub, six displaced bytes,
   // and continuation are authenticated. All other producer bytes stay exact.
   Require(fusionCallbacks213Authenticated&&fusionHooks213,"FusionFix_callback_code_changed");
   VerifyBranch(0x81e615,fusionHooks213->flagsCallback,true,6);officialFlags=true;
  }
  for(const auto& range:{std::pair<uint32_t,size_t>{0x81db60,0x81e1f1-0x81db60},
                         {0x81e200,0x81ede5-0x81e200}}){
   Require(range.first+range.second<=original->bytes.size(),"model_observer_image_range");
   auto actual=ReadMemory(game+range.first,range.second);
   if(officialFlags&&range.first==0x81e200)
    std::copy_n(original->bytes.data()+0x81e615,6,actual.data()+0x81e615-range.first);
   Require(!memcmp(actual.data(),original->bytes.data()+range.first,range.second),"model_observer_native_guard");
  }
  Require(OriginalCall(DispatchCall)==game+0x81e200,"model_observer_dispatch_target");
  Require(OriginalCall(WrapperPoint)==game+0x6bcc50,"model_observer_wrapper_target");
  Require(OriginalCall(DirectPoint)==game+0x6bccd0,"model_observer_direct_target");
  Require(OriginalCall(model_light_submission20::WrapperSpot)==game+0x6bcc50,"model_catalog_wrapper_spot_target");
  Require(OriginalCall(model_light_submission20::DirectSpot)==game+0x6bccd0,"model_catalog_direct_spot_target");
  const auto* after=original->bytes.data()+DispatchReturn;
  Require(after[0]==0x83&&after[1]==0xc4&&after[2]==0x34,"model_observer_return_guard");
  constexpr uint8_t preInstruction[]{0xf3,0x0f,0x11,0x1c,0x24};
  Require(!memcmp(original->bytes.data()+PreSite,preInstruction,sizeof(preInstruction)),"model_observer_pre_instruction");
  constexpr uint8_t postInstruction[]{0xc6,0x44,0x24,0x17,0x01};
  Require(!memcmp(original->bytes.data()+PostSite,postInstruction,sizeof(postInstruction)),"model_observer_post_instruction");
  Prepare(preHook,game+PreSite,Pre);Prepare(postHook,game+PostSite,Post);
  Require(preHook.original_bytes().size()==sizeof(preInstruction),"model_observer_pre_stole_call");
  Require(postHook.original_bytes().size()==sizeof(postInstruction),"model_observer_post_stole_cleanup");
  Prepare(resolveHook,game+ResolveSite,Resolve);Prepare(wrapperHook,game+WrapperPoint,Wrapper);
  Prepare(directHook,game+DirectPoint,Direct);
  Prepare(wrapperSpotHook,game+model_light_submission20::WrapperSpot,WrapperSpot);
  Prepare(directSpotHook,game+model_light_submission20::DirectSpot,DirectSpot);prepared=true;
 }catch(const std::exception& e){DisableHooks();Event(1,"MODEL_OBSERVER_REFUSED",e.what());}
 catch(...){DisableHooks();Event(1,"MODEL_OBSERVER_REFUSED","unknown_preparation_failure");}
}
void EnableHooks() noexcept {
 if(!requested||!prepared)return;
 try {
  // Enable entry last; no new tracked invocations until every exit is present.
  for(auto* hook:{&postHook,&resolveHook,&wrapperHook,&directHook,&wrapperSpotHook,&directSpotHook,&preHook})
   Require(bool(hook->enable()),"model_observer_hook_enable");
  enabled.store(true,std::memory_order_release);
  Event(1,"MODEL_OBSERVER_ACTIVE","sample_hash_mod=64 minimum_frame_interval=10 depth=8 ring=256 point_and_spot=1 authored_gates_preserved=1 promotion=0");
 }catch(const std::exception& e){DisableHooks();Event(1,"MODEL_OBSERVER_REFUSED",e.what());}
 catch(...){DisableHooks();Event(1,"MODEL_OBSERVER_REFUSED","unknown_enable_failure");}
}
uint32_t DescriptorWord(const Sample& sample,size_t offset) noexcept {
 uint32_t value{};std::memcpy(&value,sample.descriptor.data()+offset,4);return value;
}
float DescriptorFloat(const Sample& sample,size_t offset) noexcept {
 float value{};std::memcpy(&value,sample.descriptor.data()+offset,4);return value;
}
void Publish() noexcept {
 if(!requested||!enabled.load(std::memory_order_acquire))return;
 onyxoak::SupportLog::Batch150 batch150(log.get());
 const auto Event=[&](unsigned level,const char* code,const std::string& fields) noexcept {batch150.Event(level,code,fields);};
 try {
  static uint32_t previous{};const auto now=GetTickCount();if(now-previous<1000)return;previous=now;
  std::array<Sample,16> copy{};size_t count{};
  if(ringLock.test_and_set(std::memory_order_acquire))return;
  while(tail!=head&&count<copy.size()){copy[count++]=ring[tail%ring.size()];++tail;}
  ringLock.clear(std::memory_order_release);
  char text[500];
  sprintf_s(text,"sampled=%u read_rejected=%u context_rejected=%u depth_rejected=%u ring_full=%u ring_contention=%u drained=%u native_inactive_skipped40=%u observations_not_admissions=1",
   sampled.load(),readRejected.load(),contextRejected.load(),depthRejected.load(),ringFull.load(),ringContended.load(),unsigned(count),nativeInactiveSkipped40.load(std::memory_order_relaxed));
  Event(1,"MODEL_OBSERVER_COUNTS",text);
  sprintf_s(text,"deferred=%u matched_checks=%u fallback=%u fallback_association_rejected=%u match_rejected=%u",
   ModelValue68::deferred.load(),ModelValue68::matched77.load(),ModelValue68::legacyFallback.load(),
   ModelValue68::associationRejected.load(),ModelValue68::matchRejected77.load());
  Event(1,"MODEL_VALUE77_COUNTS",text);
  sprintf_s(text,"attempts=%u complete=%u legacy=%u keys=%u fresh_initial_and_final_identity=1 owned_packet_key_and_flags_only=1",
   ownedFusedAttempts79.load(),ownedFusedComplete79.load(),ownedFusedLegacy79.load(),ownedFusedKeys79.load());
  Event(1,"MODEL_FUSED79_COUNTS",text);
  sprintf_s(text,"invocation=%u caller=%u return_slot=%u destination=%u position=%u color=%u type=%u flags=%u key=%u native_classification_bits=96",
   ModelValue68::mismatch77[0].load(),ModelValue68::mismatch77[1].load(),ModelValue68::mismatch77[2].load(),
   ModelValue68::mismatch77[3].load(),ModelValue68::mismatch77[4].load(),ModelValue68::mismatch77[5].load(),
   ModelValue68::mismatch77[6].load(),ModelValue68::mismatch77[7].load(),ModelValue68::mismatch77[8].load());
  Event(1,"MODEL_VALUE77_MISMATCH",text);
  for(size_t i=0;i<count;++i){const auto& s=copy[i];const auto& c=s.invocation;
   sprintf_s(text,"id=%u frame=%u thread=%u invalid=%u resolved=%u light_submission_observed=%u site=%x type=%u owner_matched=%u model_info_matched=%u owner_kind=%u slot=%u reference=%u model=%x ordinal=%u descriptor_copy=%u matrix_copy=%u packet_copy=%u",
    c.nonce,c.frame,s.thread,s.status,unsigned(c.resolved),unsigned(c.submitted),s.site,s.type,unsigned(s.ownerMatched),unsigned(s.modelInfoMatched),s.ownerKind,s.identity.slot,s.identity.reference,s.identity.model,c.ordinal,unsigned(s.descriptorCopied),unsigned(s.matrixCopied),unsigned(s.packetCopied));
   Event(1,"MODEL_OBSERVER_SAMPLE",text);
   // Same bounded diagnostic sample as the pose/matrix events. The input key
   // is normally zero for promoted sources; record the actual written result
   // so source pose can be joined to queued light/map snapshots. No extra reads.
   sprintf_s(text,"id=%u frame=%u thread=%u site=%x input_key=%u promoted_key=%u pool=%x storage=%x flags_address=%x owner=%x slot=%u reference=%u model=%x ordinal=%u effect=%x descriptor=%x emitter_matrix=%x caller_owner=%x caller_model_info=%x observation_not_gpu_execution=1",
    c.nonce,c.frame,s.thread,s.site,s.key,s.promotedKey,s.identity.pool,s.identity.storage,s.identity.flags,s.identity.owner,s.identity.slot,s.identity.reference,s.identity.model,c.ordinal,c.effect,c.descriptor,c.matrix,c.owner,c.modelInfo);
   Event(1,"MODEL_SOURCE_IDENTITY38",text);
   if(s.diagnostic){
    if(model_effect_getter62::Format(s.getter62,c.nonce,c.frame,s.thread,c.descriptor,text))
     Event(1,"MODEL_EFFECT_GETTER62",text);
    if(model_effect_getter62::FormatOrigin(s.getter62,c.nonce,c.frame,s.thread,text))
     Event(1,"MODEL_EFFECT_ORIGIN62",text);
   }
   sprintf_s(text,"id=%u frame=%u thread=%u owner=%x model=%x ordinal=%u descriptor=%x emitter_matrix=%x caller_owner=%x caller_model_info=%x owner_matrix=%x copy_status=%u copy_valid=%u local_pose_decoded=0 local_pose_reason=descriptor_offsets_not_instruction_verified observation_not_gpu_execution=1",
    c.nonce,c.frame,s.thread,s.identity.owner,s.identity.model,c.ordinal,c.descriptor,c.matrix,c.owner,c.modelInfo,s.ownerTransform.address,
    unsigned(s.ownerTransform.status),unsigned(s.ownerTransform.status==model_owner_transform46::Status::Valid));
   Event(1,"MODEL_OWNER_TRANSFORM46",text);
   if(s.ownerTransform.status==model_owner_transform46::Status::Valid){
    for(unsigned row=0;row<4;++row){sprintf_s(text,"id=%u frame=%u thread=%u owner=%x model=%x row=%u values=%.6g,%.6g,%.6g,%.6g",c.nonce,c.frame,s.thread,s.identity.owner,s.identity.model,row,
      double(s.ownerTransform.matrix[row*4]),double(s.ownerTransform.matrix[row*4+1]),double(s.ownerTransform.matrix[row*4+2]),double(s.ownerTransform.matrix[row*4+3]));Event(1,"MODEL_OWNER_MATRIX46",text);}
   }
   if(s.resolveMismatch)Event(1,"MODEL_OBSERVER_MISMATCH","id="+std::to_string(c.nonce)+" reason_bits="+std::to_string(s.resolveMismatch));
   if(s.descriptorCopied){
    sprintf_s(text,"id=%u frame=%u thread=%u authored_type=%u authored_flags=%x authored_rgb=%u,%u,%u authored_radius=%.6g authored_intensity=%.6g corona=%x projection=%x packet_flags=%x packet_key_present=%u",
     c.nonce,c.frame,s.thread,unsigned(s.descriptor[0x5b]),DescriptorWord(s,0x4c),unsigned(s.descriptor[0x28]),unsigned(s.descriptor[0x29]),unsigned(s.descriptor[0x2a]),
     double(DescriptorFloat(s,0x38)),double(DescriptorFloat(s,0x3c)),DescriptorWord(s,0x50),DescriptorWord(s,0x54),s.flags,unsigned(s.key!=0));
    Event(1,"MODEL_OBSERVER_DESCRIPTOR",text);
   }
   if(s.packetCopied){
    sprintf_s(text,"id=%u frame=%u thread=%u pos=%.6g,%.6g,%.6g rgb=%.6g,%.6g,%.6g direction=%.6g,%.6g,%.6g radius=%.6g intensity=%.6g",
     c.nonce,c.frame,s.thread,double(s.position[0]),double(s.position[1]),double(s.position[2]),double(s.color[0]),double(s.color[1]),double(s.color[2]),double(s.direction[0]),double(s.direction[1]),double(s.direction[2]),double(s.radius),double(s.intensity));
    Event(1,"MODEL_OBSERVER_PACKET",text);
    sprintf_s(text,"id=%u type=%u up=%.6g,%.6g,%.6g inner_cone=%.6g outer_cone=%.6g native_authored_submission=1",c.nonce,s.type,double(s.up[0]),double(s.up[1]),double(s.up[2]),double(s.innerCone),double(s.outerCone));
    Event(1,"MODEL_CATALOG_ORIENTATION",text);
   }
   if(s.matrixCopied){
    for(unsigned row=0;row<4;++row){sprintf_s(text,"id=%u frame=%u thread=%u row=%u values=%.6g,%.6g,%.6g,%.6g",c.nonce,c.frame,s.thread,row,
      double(s.matrix[row*4]),double(s.matrix[row*4+1]),double(s.matrix[row*4+2]),double(s.matrix[row*4+3]));Event(1,"MODEL_OBSERVER_MATRIX",text);}
   }
  }
 }catch(...){/* Optional diagnostic publication cannot escape into the game. */}
}
}


