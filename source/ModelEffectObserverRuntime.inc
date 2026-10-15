// Included inside standalone, after traffic-signal-observer.inc. Observation
// only: no writes to native arguments, descriptors, owners, keys or light lists.
#include "ModelEffectObserverState.hpp"
namespace ModelEffectObserver {
using namespace model_effect_observer;
bool requested=false;
bool prepared=false;
std::atomic<bool> enabled{false};
SafetyHookMid preHook,postHook,resolveHook,wrapperHook,directHook;
struct LastErrorScope {
 DWORD value=GetLastError();
 ~LastErrorScope(){SetLastError(value);}
};
struct Sample {
 Invocation invocation{};
 traffic_signal::Identity identity{};
 uint32_t thread{},ownerKind{},site{},flags{},key{},status{},resolveMismatch{};
 bool ownerMatched{},modelInfoMatched{},descriptorCopied{},matrixCopied{},packetCopied{};
 std::array<uint8_t,0x6c> descriptor{};
 std::array<float,16> matrix{};
 std::array<float,3> position{},color{},direction{};
 float radius{},intensity{};
};
struct Local {
 State state;
 std::array<Sample,MaxDepth> samples{};
 uint32_t cadence{},quotaFrame{},quota{},lastSampleFrame{};
};
thread_local Local local;
std::atomic<uint32_t> serial{0},sampled{0},readRejected{0},contextRejected{0},depthRejected{0};
std::atomic<uint32_t> ringFull{0},ringContended{0};
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
 if(!enabled.load(std::memory_order_acquire))return;
 Invocation invocation{};
 if(regs.esp<PreStackAdjustment)return;
 invocation.preStack=regs.esp-PreStackAdjustment;invocation.owner=regs.esi;invocation.effect=regs.edi;
 invocation.frame=Frame();invocation.nonce=serial.fetch_add(1,std::memory_order_relaxed)+1;
 if(local.quotaFrame!=invocation.frame){local.quotaFrame=invocation.frame;local.quota=0;}
 uint32_t mix=++local.cadence^(invocation.frame*0x9e3779b9u);mix^=mix>>16;mix*=0x7feb352du;mix^=mix>>15;
 invocation.sampled=(mix&63u)==0&&uint32_t(invocation.frame-local.lastSampleFrame)>=10;
 if(invocation.sampled)local.lastSampleFrame=invocation.frame;
 if(invocation.sampled){++local.quota;++sampled;}
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
  sample.thread=GetCurrentThreadId();sample.invocation=*local.state.Current();
 }
}
bool InnerEvidence(const SafetyHookContext& regs,ResolveEvidence& evidence) noexcept {
 uint32_t arguments[8]{},saved[2]{};
 if(regs.ebp>UINT32_MAX-0x24||!Copy(regs.ebp+4,arguments,sizeof(arguments))||
    !Copy(regs.esp,saved,sizeof(saved)))return false;
 evidence.frame=Frame();evidence.preStack=regs.ebp+8;
 evidence.returnRva=arguments[0]-uint32_t(game);evidence.effect=arguments[1];
 evidence.matrix=arguments[2];evidence.savedOwner=saved[1];evidence.savedEffect=saved[0];
 evidence.descriptor=regs.edi;evidence.proposedKey=arguments[7];
 return true;
}
void Resolve(SafetyHookContext& regs) noexcept {
 const LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 if(!enabled.load(std::memory_order_acquire))return;
 const auto* current=local.state.Current();if(!current||!current->sampled)return;
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
 TrafficSignal::Match match{};
 sample.ownerMatched=TrafficSignal::MatchOwner(current->owner,match);
 if(sample.ownerMatched){
  sample.identity=match.identity;sample.ownerKind=match.kind;
  int16_t modelIndex=-1;uint32_t modelInfo{};
  if(Copy(uintptr_t(current->owner)+0x2e,&modelIndex,sizeof(modelIndex))&&modelIndex>=0&&modelIndex<31000&&
     Copy(game+0xe95cd8+uint32_t(modelIndex)*4,&modelInfo,sizeof(modelInfo)))
   sample.modelInfoMatched=modelInfo==current->modelInfo;
  if(!sample.modelInfoMatched){local.state.Invalidate();++contextRejected;}
 }
 // Unsupported owners are recorded as such, never guessed or promoted.
 sample.descriptorCopied=Copy(evidence.descriptor,sample.descriptor.data(),sample.descriptor.size());
 sample.matrixCopied=Copy(evidence.matrix,sample.matrix.data(),sizeof(sample.matrix));
 if(!sample.descriptorCopied||!sample.matrixCopied){++readRejected;local.state.Invalidate();}
}
void Point(SafetyHookContext& regs,uint32_t site) noexcept {
 const LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 if(!enabled.load(std::memory_order_acquire))return;
 const auto* current=local.state.Current();if(!current||!current->sampled)return;
 const auto words=StoreWordCount(site);
 std::array<uint32_t,18> arguments{};uint32_t returnAndEffect[2]{};
 if(!words||regs.ebp>UINT32_MAX-0x0c||!Copy(regs.esp,arguments.data(),words*4)||
    !Copy(regs.ebp+4,returnAndEffect,sizeof(returnAndEffect))){
  local.state.Invalidate();++readRejected;return;
 }
 PointEvidence evidence{Frame(),regs.ebp+8,returnAndEffect[0]-uint32_t(game),arguments[0],
   uint32_t(regs.edi),site,arguments[1],arguments[2],arguments[StoreKeyOffset(site)/4]};
 if(returnAndEffect[1]!=arguments[0]||!local.state.Point(evidence)){
  local.state.Invalidate();++contextRejected;return;
 }
 auto& sample=local.samples[local.state.Depth()-1];sample.site=site;sample.flags=evidence.flags;sample.key=evidence.key;
 // Common first eleven arguments have the same layout in both store ABIs.
 sample.packetCopied=Copy(arguments[5],sample.position.data(),sizeof(sample.position))&&
   Copy(arguments[6],sample.color.data(),sizeof(sample.color))&&
   Copy(arguments[3],sample.direction.data(),sizeof(sample.direction));
 std::memcpy(&sample.intensity,&arguments[7],4);std::memcpy(&sample.radius,&arguments[10],4);
 if(!sample.packetCopied){local.state.Invalidate();++readRejected;}
}
void Wrapper(SafetyHookContext& regs) noexcept {Point(regs,WrapperPoint);}
void Direct(SafetyHookContext& regs) noexcept {Point(regs,DirectPoint);}
void Post(SafetyHookContext& regs) noexcept {
 const LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 if(!enabled.load(std::memory_order_acquire))return;
 const auto depth=local.state.Depth();Invocation finished{};
 if(regs.esp<PostStackAdjustment){local.state.Invalidate();++contextRejected;return;}
 const auto result=local.state.Leave(regs.esp-PostStackAdjustment,Frame(),finished);
 if(result==LeaveResult::Missing||result==LeaveResult::Overflow)return;
 if(!finished.sampled||!depth)return;
 auto sample=local.samples[depth-1];sample.invocation=finished;
 sample.status=result==LeaveResult::Invalid?1u:0u;
 if(sample.status)++contextRejected;
 Queue(sample);
}
void DisableHooks() noexcept {
 enabled.store(false,std::memory_order_release);
 for(auto* hook:{&directHook,&wrapperHook,&resolveHook,&postHook,&preHook})if(*hook)(void)hook->disable();
 prepared=false;
}
void PrepareHooks() noexcept {
 if(!requested)return;
 try {
  Require(original!=nullptr,"model_observer_missing_image");
  bool officialFlags=false;
  if(!nativeBackend){
   Require(layout==&Layout511,"model_observer_unsupported_flags_overlay");
   // Exact 5.1.1 FlagsHook: callback, generated stub, six displaced bytes,
   // and continuation are authenticated. All other producer bytes stay exact.
   VerifyBranch(0x81e615,0xa6b50,true,6);officialFlags=true;
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
  Prepare(directHook,game+DirectPoint,Direct);prepared=true;
 }catch(const std::exception& e){DisableHooks();Event(1,"MODEL_OBSERVER_REFUSED",e.what());}
 catch(...){DisableHooks();Event(1,"MODEL_OBSERVER_REFUSED","unknown_preparation_failure");}
}
void EnableHooks() noexcept {
 if(!requested||!prepared)return;
 try {
  // Enable entry last; no new tracked invocations until every exit is present.
  for(auto* hook:{&postHook,&resolveHook,&wrapperHook,&directHook,&preHook})
   Require(bool(hook->enable()),"model_observer_hook_enable");
  enabled.store(true,std::memory_order_release);
  Event(1,"MODEL_OBSERVER_ACTIVE","sample_hash_mod=64 minimum_frame_interval=10 depth=8 ring=256 point_only=1 promotion=0");
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
 try {
  static uint32_t previous{};const auto now=GetTickCount();if(now-previous<1000)return;previous=now;
  std::array<Sample,16> copy{};size_t count{};
  if(ringLock.test_and_set(std::memory_order_acquire))return;
  while(tail!=head&&count<copy.size()){copy[count++]=ring[tail%ring.size()];++tail;}
  ringLock.clear(std::memory_order_release);
  char text[500];
  sprintf_s(text,"sampled=%u read_rejected=%u context_rejected=%u depth_rejected=%u ring_full=%u ring_contention=%u drained=%u observations_not_admissions=1",
   sampled.load(),readRejected.load(),contextRejected.load(),depthRejected.load(),ringFull.load(),ringContended.load(),unsigned(count));
  Event(1,"MODEL_OBSERVER_COUNTS",text);
  for(size_t i=0;i<count;++i){const auto& s=copy[i];const auto& c=s.invocation;
   sprintf_s(text,"id=%u frame=%u thread=%u invalid=%u resolved=%u point_submission_observed=%u site=%x owner_matched=%u model_info_matched=%u owner_kind=%u slot=%u reference=%u model=%x ordinal=%u descriptor_copy=%u matrix_copy=%u packet_copy=%u",
    c.nonce,c.frame,s.thread,s.status,unsigned(c.resolved),unsigned(c.submitted),s.site,unsigned(s.ownerMatched),unsigned(s.modelInfoMatched),s.ownerKind,s.identity.slot,s.identity.reference,s.identity.model,c.ordinal,unsigned(s.descriptorCopied),unsigned(s.matrixCopied),unsigned(s.packetCopied));
   Event(1,"MODEL_OBSERVER_SAMPLE",text);
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
   }
   if(s.matrixCopied){
    for(unsigned row=0;row<4;++row){sprintf_s(text,"id=%u frame=%u thread=%u row=%u values=%.6g,%.6g,%.6g,%.6g",c.nonce,c.frame,s.thread,row,
      double(s.matrix[row*4]),double(s.matrix[row*4+1]),double(s.matrix[row*4+2]),double(s.matrix[row*4+3]));Event(1,"MODEL_OBSERVER_MATRIX",text);}
   }
  }
 }catch(...){/* Optional diagnostic publication cannot escape into the game. */}
}
}
