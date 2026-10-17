// Prototype default OFF. Constructor-scoped normalized projection authority.
// No async emission, borrowed owner lifetime, token recycling or lighting merge.
#include "NativeClassDomains105.hpp"
#include "NativeProjectionRead66.hpp"
#include "NativeCopyGuard70.hpp"
#include "ArcadePlacement197.hpp"
#include <intrin.h>
namespace NativeProjection66 {
bool requested=false,replacementRequested=false,authenticated=false;
std::atomic<bool> active{false};
SafetyHookInline constructorHook;
alignas(64) uint8_t tokens[4096][64]{};
alignas(64) uint8_t classTokens105[3][4096][64]{};
native_class_domains105::Registry<> registry;
native_class_domains105::IssuedKeys<> issuedKeys;
arcade_placement197::Projections privateProjections197;
thread_local native_projection66::AliasPass<> aliases;
thread_local native_projection66::Sidecar<> sidecar;
std::atomic_flag lock=ATOMIC_FLAG_INIT;
std::atomic<uint32_t> constructed{0},promoted{0},fallback{0},modelDeferred{0},modelFallback{0},
 accepted{0},invalidQueued{0},aliasPackets{0},contended{0},capacityFallback{0},bankPreflights{0},bankDenied{0},packetFaults{0},permissionQueries{0},winnerPreflights{0},winnerDenied{0},tokensUsed74{0};
std::array<std::atomic<uint32_t>,4> classTokensUsed105{};
using native_projection66::Packet;
// Integration68 supplies a synchronous, exact current-submission fallback.
// No stored owner/argument pointers or pending source scope are owned here.
bool (*matchModelConstructor66)(const native_projection66::ConstructorContext&) noexcept=nullptr;
uint32_t (*legacyFallback66)(Packet&,uint32_t,const native_projection66::ConstructorContext&) noexcept=nullptr;
void (*constructorCompleted66)(bool,const native_projection66::ConstructorContext&) noexcept=nullptr;
bool (*placeModel197)(Packet&,Packet&,uint32_t,const native_projection66::ConstructorContext&) noexcept=nullptr;
bool (*finishPlacement197)(Packet&,uint32_t,const native_projection66::ConstructorContext&,bool) noexcept=nullptr;
bool (*placementBank197)(rage::CLightSource&,Packet&,uint32_t) noexcept=nullptr;
bool Enabled() noexcept {return requested&&active.load(std::memory_order_acquire)&&addonActive.load(std::memory_order_acquire)&&PlayerShadowAllocation::Enabled()&&bool(bExtraNightShadows);}
bool CopyPacket(const rage::CLightSource& light,Packet& p,bool fresh=true,unsigned* queryCount133=nullptr) noexcept {
 if(queryCount133)*queryCount133=0;
 p={};if(fresh){++winnerPreflights;const auto permission=native_projection66::PacketPreflight203(reinterpret_cast<uintptr_t>(&light));
  permissionQueries.fetch_add(permission.queries,std::memory_order_relaxed);
  if(queryCount133)*queryCount133=permission.queries;
  if(!permission.allowed){++winnerDenied;return false;}
 }
 const auto copied=Liberty20::CopyLocal(reinterpret_cast<uintptr_t>(&light),p.data(),0x78);
 if(!copied)++packetFaults;return copied;
}
bool OwnKey(uint32_t key) noexcept {
 return privateProjections197.Owns(key)||issuedKeys.Contains(registry,key);
}
bool CanReplaceModelSubmission(uint32_t type,uint32_t flags,uint32_t key) noexcept {
 return replacementRequested&&authenticated&&Enabled()&&(type==0||type==2)&&!(flags&6u)&&!key;
}
uint32_t Intern(Packet& packet,uint32_t frame) noexcept {
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::ProjectionIntern69,128);
 if(lock.test_and_set(std::memory_order_acquire)){++contended;return 0;}
 const bool full105=registry.Full(packet);
 const auto key=registry.Observe(packet,frame,[](uint32_t candidate) noexcept {return Liberty20::Collision(candidate);});
 tokensUsed74.store(registry.Used(),std::memory_order_relaxed);
 for(unsigned domain=0;domain<4;++domain)classTokensUsed105[domain].store(registry.domains[domain].used,std::memory_order_relaxed);
 if(!key&&full105)++capacityFallback;
 lock.clear(std::memory_order_release);
 if(key){
  if(!issuedKeys.Publish(registry,key))return 0;
  packet[24]=key;packet[18]|=4u;
 }return key;
}
uint32_t InternPlacement197(Packet& packet,uint32_t frame) noexcept {
 if(lock.test_and_set(std::memory_order_acquire)){++contended;return 0;}
 const auto key=privateProjections197.Intern(packet,frame,[](uint32_t candidate)noexcept{return Liberty20::Collision(candidate);});
 lock.clear(std::memory_order_release);return key;
}
// Exact native thiscall signature: ECXdestination,17 stack DWORDs,ret44.
// The inline hook has a fastcall dummy EDX and the same stack cleanup. Only the
// first authenticated constructor caller is adapted; alternate6BD23B retains
// its intentional shadow suppression and every other caller passes through.
__declspec(noinline) uint32_t __fastcall Constructor(void* destination,void*,uint32_t type,uint32_t flags,
 const void* direction,const void* tangent,const void* position,const void* color,
 float intensity,uint32_t texture50,uint32_t texture4c,float radius,float inner,float outer,
 float volumeIntensity,float volumeScale,uint32_t interior,uint32_t room,uint32_t key) noexcept {
 const auto caller=reinterpret_cast<uintptr_t>(_ReturnAddress());
 const native_projection66::ConstructorContext context{caller,reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()),
  reinterpret_cast<uintptr_t>(destination),reinterpret_cast<uintptr_t>(direction),reinterpret_cast<uintptr_t>(tangent),
  reinterpret_cast<uintptr_t>(position),reinterpret_cast<uintptr_t>(color),type,flags,key,interior,room};
 auto original=[&](void* output) noexcept {
  return constructorHook.unsafe_thiscall<uint32_t>(output,type,flags,direction,tangent,position,color,intensity,
   texture50,texture4c,radius,inner,outer,volumeIntensity,volumeScale,interior,room,key);
 };
 if(caller!=game+0x6bd194||(type!=0&&type!=2)||(flags&6u)||key)return original(destination);
 bool currentModel=false;
 {const ModelEffectObserver::LastErrorScope error;const fusionfix::shadows::FloatingPointState fp;
  currentModel=matchModelConstructor66&&matchModelConstructor66(context);
 }
 if(!Enabled()&&!currentModel)return original(destination);
 ++constructed;
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::ProjectionConstruct69,128);
 return native_projection66::ConstructOwned(destination,original,[&](Packet& packet) noexcept {
  const ModelEffectObserver::LastErrorScope error;
  const fusionfix::shadows::FloatingPointState fp;
  const auto frame=*CShadows::pFrameCounter;
  const bool success=arcade_placement197::Adapt(packet,currentModel&&placeModel197,
   [&](Packet& p,Packet& before) noexcept{return placeModel197(p,before,frame,context);},
   [&](Packet& p,bool relocated) noexcept{return Enabled()&&(relocated?InternPlacement197(p,frame):Intern(p,frame))!=0;},
   [&](Packet& p,bool issued) noexcept{return finishPlacement197&&finishPlacement197(p,frame,context,issued);});
  if(success)++promoted;
  else {++fallback;if(currentModel&&legacyFallback66)(void)legacyFallback66(packet,frame,context);}
  if(currentModel&&constructorCompleted66)constructorCompleted66(success,context);
 },[](void* output,const void* packet) noexcept {
  // The authenticated original copier is exactly120 bytes of bitwise copies.
  // This destination is the caller's temporary, not the native lighting bank.
  // Preserve FusionFix's after-copy event solely on its original bank copy.
  std::memcpy(output,packet,0x78);
 });
}
void PrepareHooks() {
 if(!requested)return;
 // Authentication covers both constructor callpaths and exact constructor/copy
 // bodies. Relocation-bearing store range uses mapped original, not diskbytes.
 for(const auto [rva,size]:std::array<std::pair<uint32_t,size_t>,3>{{{0x6bcc50,0x7f},{0x6bccd0,0x900},{0x1b8580,0x150}}}){
  const auto bytes=ReadMemory(game+rva,size);
  for(size_t n=0;n<size;++n)if(bytes[n]!=original->bytes[rva+n]){
   Event(1,"PROJECTION_GUARD70_MISMATCH","rva="+std::to_string(rva)+" offset="+std::to_string(n));break;
  }
  Require(!memcmp(bytes.data(),original->bytes.data()+rva,size),"projection66_native_body_guard_failed");
 }
 std::vector<uint8_t> callback;
 const auto* copyProfile213=(!nativeBackend&&layout)?CopyProfile218():nullptr;
 const bool knownVersion213=copyProfile213&&fusionModule213Authenticated;
 if(knownVersion213){
  const auto raw=pe::Read(pe::ModulePath(reinterpret_cast<HMODULE>(ff)));
  Require(reinterpret_cast<uintptr_t>(pe::ModuleAt(ff))==ff,"copy70_hook_owner_unavailable");
  const pe::Image expected(raw,ff);
  callback.assign(expected.bytes.begin()+copyProfile213->callback,
   expected.bytes.begin()+copyProfile213->callback+copyProfile213->size);
 }
 const auto copyEvidence=VolumetricCompat199::authenticated?native_copy70::Evidence{VolumetricCompat199::copyChain.fusionFix,0,0}:native_copy70::Verify(uint32_t(game),uint32_t(ff),knownVersion213,
  std::span<const uint8_t>(original->bytes.data()+native_copy70::CopyRva,native_copy70::CopySize),callback,
  [](uint32_t address,size_t size){return ReadMemory(address,size);},
  copyProfile213?copyProfile213->callback:native_copy70::CallbackRva,
  copyProfile213?copyProfile213->allocation:native_copy70::AllocationRva,
  copyProfile213?copyProfile213->size:native_copy70::CallbackSize,
  copyProfile213?copyProfile213->slot:0x57,copyProfile213?copyProfile213->invoke:0x70);
 registry.snowMetadata70=knownVersion213&&copyEvidence.fusionFixOverlay;
 registry.volumetricMetadata199=VolumetricCompat199::authenticated;
 Event(1,"PROJECTION_COPY70_AUTHENTICATED","fusionfix_overlay="+std::to_string(copyEvidence.fusionFixOverlay)+
  " scratch_copy_bytes=120 native_bank_callback_preserved=1 duplicate_callback=0");
 Require(OriginalCall(0x6bd18f)==game+0x1b8580&&OriginalCall(0x6bd23b)==game+0x1b8580,"projection66_ctor_call_guard_failed");
 auto prepared=SafetyHookInline::create(game+0x1b8580,Constructor,SafetyHookInline::StartDisabled);
 Require(bool(prepared),"projection66_ctor_prepare_failed");constructorHook=std::move(*prepared);
 Require(registry.Init({uint32_t(reinterpret_cast<uintptr_t>(classTokens105[0])),uint32_t(reinterpret_cast<uintptr_t>(tokens)),uint32_t(reinterpret_cast<uintptr_t>(classTokens105[1])),uint32_t(reinterpret_cast<uintptr_t>(classTokens105[2]))}),"projection105_key_domains_invalid");authenticated=true;
}
void Enable() {if(requested&&authenticated){Require(bool(constructorHook.enable()),"projection66_ctor_enable_failed");active.store(true,std::memory_order_release);}}
void Disable() noexcept {active=false;if(constructorHook)(void)constructorHook.disable();}
using DiagnosticPacket199C=void(*)(unsigned,rage::CLightSource*,uint32_t,uint32_t,const Packet&) noexcept;
std::atomic<DiagnosticPacket199C> diagnosticPacket199c{nullptr};
void ValidateQueued(rage::CLightSource* lights,uint32_t count) noexcept {
 const auto frame=*CShadows::pFrameCounter;
 aliases.Begin();sidecar.Begin(reinterpret_cast<uintptr_t>(lights),count,frame);
 // Disabled or failed hooks must add no native-bank permission/read work.
 // Reset first so a formerly active pass cannot retain authorization.
 if(!active.load(std::memory_order_acquire))return;
 if(!lights||count>640)return;
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::ProjectionQueued69,32);
 if(count){++bankPreflights;
  // Current native list is numeric game-owned packet storage, not a borrowed
  // model/owner. One fresh whole-bank committed-region preflight; no cached
  // permissions. Current values are still freshly SEH-copied at each consumer.
  const auto permission=native_projection66::Preflight(reinterpret_cast<uintptr_t>(lights),size_t(count)*sizeof(rage::CLightSource));
  permissionQueries.fetch_add(permission.queries,std::memory_order_relaxed);
  if(!permission.allowed){++bankDenied;return;}
 }
 // Snapshot every own-domain entry as invalid before attempting registry lock.
 // A failed bounded read cannot authorize any insertion in this pass.
 for(unsigned i=0;i<count;++i){Packet packet{};
  if(CopyPacket(lights[i],packet,false)){
   if(const auto observer=diagnosticPacket199c.load(std::memory_order_acquire))observer(i,lights,count,frame,packet);
   if(OwnKey(packet[24])){
    // Observe raw current packets before the exact placement-retirement check.
    // A retired placement never receives sidecar authority for its old packet.
    if(placementBank197&&!placementBank197(lights[i],packet,frame))continue;
    sidecar.Own(i,packet);
   }
  }
 }
 if(lock.test_and_set(std::memory_order_acquire)){++contended;return;}
 for(unsigned i=0;i<count;++i){
  if(sidecar.decisions[i]!=native_projection66::Decision::Invalid)continue;
  const auto& packet=sidecar.packets[i];const auto key=packet[24];
  const float weight=fusionfix::shadows::SceneBrightness44(std::bit_cast<float>(packet[16]),
   {std::bit_cast<float>(packet[12]),std::bit_cast<float>(packet[13]),std::bit_cast<float>(packet[14])});
  const bool valid197=privateProjections197.Owns(key)?privateProjections197.Validate(key,packet):registry.Validate(key,packet);
  if(!valid197||!aliases.Add(i,key,packet,weight,registry.snowMetadata70,registry.volumetricMetadata199)){++invalidQueued;continue;}
  sidecar.Accept(i,false);++accepted;
 }
 for(unsigned i=0;i<count;++i)if(sidecar.decisions[i]==native_projection66::Decision::Valid){
  sidecar.Accept(i,aliases.IsAlias(i));if(aliases.IsAlias(i))++aliasPackets;
 }
 lock.clear(std::memory_order_release);
}
// 0 ordinary,1 valid owned projection,2 same-projection shadow alias,3 invalid.
// The consumer checks exact native index/list/stride/stack flags before using
// this numeric sidecar. Invalid/alias bypass ONLY shadow insertion via EIP;
// separate RGB/intensity packets remain in the original native lighting bank.
unsigned CollectDecision(unsigned index,rage::CLightSource* lights,uint32_t count,uint32_t frame,uintptr_t stack) noexcept {
 if(!active.load(std::memory_order_acquire))return 3;
 if(!lights||index>=count||count>640)return 3;
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::ProjectionWinner69,128);
 Packet packet{};
 if(!CopyPacket(lights[index],packet)) {
  return sidecar.Envelope(reinterpret_cast<uintptr_t>(lights),count,frame)&&
   sidecar.decisions[index]!=native_projection66::Decision::Ordinary?3u:0u;
 }
 if(!OwnKey(packet[24]))return 0;
 if(placementBank197&&!placementBank197(lights[index],packet,frame))return 3;
 if(!active.load(std::memory_order_acquire))return 3;
 const auto decision=sidecar.Current(index,packet,reinterpret_cast<uintptr_t>(lights),count,frame,stack);
 return decision==native_projection66::Decision::Valid?1u:decision==native_projection66::Decision::Alias?2u:3u;
}
unsigned Classify(unsigned index,uint32_t key,rage::CLightSource* lights,uint32_t count,uint32_t frame) noexcept {
 if(!OwnKey(key))return 0;
 if(!active.load(std::memory_order_acquire)||!sidecar.Envelope(reinterpret_cast<uintptr_t>(lights),count,frame)||index>=count)return 3;
 const auto d=sidecar.decisions[index];
 return sidecar.packets[index][24]==key&&(d==native_projection66::Decision::Valid||d==native_projection66::Decision::Alias)?1u:3u;
}
bool Alias(unsigned index,uint32_t key,rage::CLightSource* lights,uint32_t count,uint32_t frame) noexcept {
 // Multiplicity hint only: never authorizes native insertion or map reuse.
 return Classify(index,key,lights,count,frame)==1&&sidecar.decisions[index]==native_projection66::Decision::Alias;
}
void Publish() {
 static uint32_t last{};if(!requested)return;const auto now=GetTickCount();if(uint32_t(now-last)<5000)return;last=now;
 Event(1,"NATIVE_PROJECTION66_COUNTS","requested="+std::to_string(requested)+" active="+std::to_string(active.load())+
  " replacement="+std::to_string(replacementRequested)+" constructed="+std::to_string(constructed.load())+
  " promoted="+std::to_string(promoted.load())+" fallback="+std::to_string(fallback.load())+
  " model_deferred="+std::to_string(modelDeferred.load())+" model_fallback="+std::to_string(modelFallback.load())+
  " accepted_packets="+std::to_string(accepted.load())+" invalid_queued="+std::to_string(invalidQueued.load())+
  " alias_packets="+std::to_string(aliasPackets.load())+" contended="+std::to_string(contended.load())+
  " capacity_fallback="+std::to_string(capacityFallback.load())+
  " bank_preflights="+std::to_string(bankPreflights.load())+" bank_denied="+std::to_string(bankDenied.load())+
  " packet_faults="+std::to_string(packetFaults.load())+
  " permission_queries="+std::to_string(permissionQueries.load())+" winner_preflights="+std::to_string(winnerPreflights.load())+
  " winner_denied="+std::to_string(winnerDenied.load())+" tokens_used="+std::to_string(tokensUsed74.load())+
  " tokens_capacity=16384 partition_mask105=768 native_shadow_slots=7 projection_identity_bytes=76 current_witness_bytes=112 reuse=0 independent_illumination=1");
 Event(1,"NATIVE_PROJECTION105_DOMAINS","class0_used="+std::to_string(classTokensUsed105[0].load())+
  " class100_used="+std::to_string(classTokensUsed105[1].load())+
  " class200_used="+std::to_string(classTokensUsed105[2].load())+
  " class300_used="+std::to_string(classTokensUsed105[3].load())+
  " total_used="+std::to_string(tokensUsed74.load())+
  " per_class_capacity=4096 total_capacity=16384 retained_native_mask=768 native_shadow_slots=7 reuse=0");
}
}
