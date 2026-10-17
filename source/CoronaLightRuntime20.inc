// Included inside standalone AFTER ModelLightCatalogRuntime20.inc. GPL-3.0.
// Authored corona reconstruction is distinct from native illumination.
#include "CoronaLightPolicy20.hpp"
namespace CoronaLight20 {
bool requested=false,prepared=false;
std::atomic<bool> active{false};
corona_light20::Settings settings;
SafetyHookMid activeHook;
std::atomic<uint32_t> emitted{0},refused{0},approximated{0},collisions{0},contextRefused{0};
void Submit(SafetyHookContext& regs) noexcept {
 const ModelEffectObserver::LastErrorScope error;const fusionfix::shadows::FloatingPointState fp;
 if(!active.load(std::memory_order_acquire)||!addonActive.load(std::memory_order_acquire)||!PlayerShadowAllocation::Enabled()||
    !ModelEffectObserver::enabled.load(std::memory_order_acquire)||!ModelLightShadow20::active.load(std::memory_order_acquire))return;
 const auto* current=ModelEffectObserver::local.state.Current();
 if(!current||!current->sampled||current->invalid||!current->resolved||current->submitted||
    current->frame!=*CShadows::pFrameCounter||current->descriptor!=uint32_t(regs.edi)||
    !ModelEffectObserver::local.state.Depth()||regs.ebp>UINT32_MAX-12){++contextRefused;return;}
 const auto c=*current;
 const auto& sample=ModelEffectObserver::local.samples[ModelEffectObserver::local.state.Depth()-1];
 // Native point/spot descriptors already produce actual illumination. Their
 // corona never qualifies for reconstruction; skip redundant packet, matrix,
 // owner and model reads. This grants no authority to the copied descriptor:
 // a type0 candidate still revalidates every current byte before submission.
 if(!sample.descriptorCopied||sample.descriptor[0x5b]!=0){++refused;return;}
 uint32_t caller[2]{};
 if(!sample.ownerMatched||!sample.modelInfoMatched||!sample.descriptorCopied||!sample.matrixCopied||
    !Liberty20::Copy(regs.ebp+4,caller,sizeof(caller))||caller[0]!=game+model_effect_observer::DispatchReturn||
    caller[1]!=c.effect||regs.ebp+8!=c.preStack){++contextRefused;return;}
 corona_light20::Packet packet{};TrafficSignal::Match fresh{};std::array<float,16> matrix{};
 bool ok=Liberty20::Copy(regs.esp,packet.corona.data(),sizeof(packet.corona))&&
  Liberty20::Copy(c.preStack,packet.dispatch.data(),sizeof(packet.dispatch))&&
  Liberty20::Copy(packet.corona[5],&packet.position,sizeof(packet.position))&&
  Liberty20::Copy(c.matrix,matrix.data(),sizeof(matrix))&&
  Liberty20::Copy(c.descriptor,packet.descriptor.data(),packet.descriptor.size())&&packet.descriptor==sample.descriptor&&
  TrafficSignal::MatchOwner(c.owner,fresh)&&fresh.identity==sample.identity&&
  fresh.modelInfo==c.modelInfo;
 packet.matrixPosition={matrix[12],matrix[13],matrix[14]};
 corona_light20::Emission emission{};
 ok=ok&&packet.dispatch[0]==c.effect&&packet.dispatch[1]==c.matrix&&
  corona_light20::Reconstruct(corona_light20::ActiveSite,c.proposedKey,settings,packet,emission);
 uint32_t key{};
 if(ok){
  if(ModelLightShadow20::lock.test_and_set(std::memory_order_acquire))ok=false;
  else {
   const auto previousUsed=ModelLightShadow20::registry.used;
   auto* entry=ModelLightShadow20::registry.Observe(fresh.identity,c.ordinal,c.descriptor,c.modelInfo,packet.descriptor,c.frame);
   ModelLightShadow20::identitiesPublished32.store(ModelLightShadow20::registry.used,std::memory_order_relaxed);
   ok=entry!=nullptr;
   if(ok&&ModelLightShadow20::registry.used!=previousUsed&&Liberty20::Collision(entry->key)){
    entry->active=false;++collisions;ok=false;
   }
   if(ok)ok=ModelLightShadow20::registry.Publish(entry->key);
   if(ok)key=entry->key;
   ModelLightShadow20::lock.clear(std::memory_order_release);
  }
 }
 if(!ok||!active.load(std::memory_order_acquire)||!ModelLightShadow20::active.load(std::memory_order_acquire)){++refused;return;}
 const std::array<float,4> direction{0,0,-1,0},up{0,1,0,0};
 const std::array<float,4> position{emission.position.x,emission.position.y,emission.position.z,1};
 const std::array<float,4> color{emission.color.x,emission.color.y,emission.color.z,1};
 auto bits=[](float f){return int(std::bit_cast<uint32_t>(f));};
 // Original native wrapper copies these local vectors synchronously. Preserve
 // room/interior/native texture authority from the authenticated dispatch.
 // Real point, flags4 dynamic; no vehicle rank, cached key or double native
 // illumination for authored type1/2. Original corona call still runs intact.
 CShadows::hbStoreStaticShadow.fun(int(c.effect),0,4,
  int(reinterpret_cast<uintptr_t>(direction.data())),int(reinterpret_cast<uintptr_t>(up.data())),
  int(reinterpret_cast<uintptr_t>(position.data())),int(reinterpret_cast<uintptr_t>(color.data())),
  bits(emission.intensity),0,int(packet.dispatch[9]),bits(emission.radius),0,0,
  int(packet.dispatch[7]),int(packet.dispatch[8]),int(key));
 ++emitted;if(emission.approximateRadius||emission.approximateIntensity)++approximated;
}
void Disable() noexcept;
void Unavailable221(const char* stage,const char* reason) noexcept {
 Disable();prepared=false;
 Event(1,"OPTIONAL_FEATURE_UNAVAILABLE221",std::string("features=authored_corona_reconstruction stage=")+stage+
  " reason="+reason+" scope=optional core_disabled_by_this_failure=0 native_lighting_unchanged=1");
}
void PrepareHooks() noexcept {
 if(!requested)return;
 try {
 Require(corona_light20::Valid(settings),"corona_settings_invalid");
 Require(ModelEffectObserver::prepared,"corona_model_observer_prepare_unavailable");
 for(const auto& [rva,size]:{std::pair<uint32_t,size_t>{0x81ea5a,0x81eb3e-0x81ea5a},{0x81f020,0x81f163-0x81f020}}){
  Require(rva+size<=original->bytes.size(),"corona_image_range");
  auto actual=ReadMemory(game+rva,size);
  if(!nativeBackend&&rva==0x81f020){
   // FusionFix's IncreaseCoronaLimit changes only the capacity immediate
   // and displacements into its replacement corona array. It does not change
   // this call's arguments, stack cleanup, opcodes or control flow. Authenticate
   // one consistent buffer relocation; never ignore arbitrary body changes.
   constexpr uint32_t bufferOperands[]{0x81f04a,0x81f051,0x81f05b,0x81f065,
    0x81f073,0x81f07b,0x81f081,0x81f097,0x81f09f,0x81f0ad,0x81f0bb,
    0x81f0d1,0x81f0dc,0x81f0ef,0x81f100,0x81f106,0x81f11c,0x81f124,
    0x81f12c,0x81f136,0x81f13c,0x81f155};
   const auto oldBase=U32(original->bytes.data()+0x81f097);
   const auto newBase=U32(actual.data()+0x81f097-rva);
   const auto oldLimit=U32(original->bytes.data()+0x81f02c);
   const auto newLimit=U32(actual.data()+0x81f02c-rva);
   if(newBase!=oldBase||newLimit!=oldLimit){
    Require(oldLimit==768&&newLimit==49152&&newBase!=oldBase&&
     uint64_t(newBase)+uint64_t(newLimit)*64<=UINT32_MAX&&
     TrafficSignal::Readable(newBase,64),"corona_buffer_expansion_invalid");
    for(const auto operand:bufferOperands){
     const auto expected=U32(original->bytes.data()+operand);
     Require(expected>=oldBase&&expected-oldBase<=0x3c&&
      U32(actual.data()+operand-rva)==newBase+(expected-oldBase),"corona_buffer_relocation_inconsistent");
     std::copy_n(original->bytes.data()+operand,4,actual.data()+operand-rva);
    }
    std::copy_n(original->bytes.data()+0x81f02c,4,actual.data()+0x81f02c-rva);
    Event(1,"CORONA_BUFFER_EXPANSION_AUTHENTICATED","capacity=49152 consistent_operands=22 all_other_producer_bytes=exact");
   }
  }
  for(size_t n=0;n<size;++n)if(actual[n]!=original->bytes[rva+n]){
   Event(1,"CORONA_GUARD_MISMATCH","rva="+std::to_string(rva)+" offset="+std::to_string(n)+
    " expected="+std::to_string(original->bytes[rva+n])+" actual="+std::to_string(actual[n]));break;
  }
  Require(!memcmp(actual.data(),original->bytes.data()+rva,size),"corona_native_producer_modified");
 }
 Require(OriginalCall(corona_light20::ActiveSite)==game+corona_light20::NativeTarget,"corona_native_target_changed");
 Prepare(activeHook,game+corona_light20::ActiveSite,Submit);
 Require(activeHook.original_bytes().size()==5,"corona_hook_stole_cleanup");prepared=true;
 }catch(const std::exception& e){Unavailable221("prepare",e.what());}
 catch(...){Unavailable221("prepare","unknown_preparation_failure");}
}
void EnableHooks() noexcept {
 if(!requested||!prepared)return;
 try {
 Require(ModelEffectObserver::enabled.load(),"corona_model_observer_enable_unavailable");
 Require(ModelLightShadow20::active.load(),"corona_model_lifetime_unavailable");
 Require(bool(activeHook.enable()),"corona_hook_enable_failed");active=true;
 Event(1,"CORONA_LIGHT20_ENABLED","authored_type0_only=1 native_rgb_switches=1 approximate_falloff_configured=1 geographic_list=0 native_illumination_claim=0");
 }catch(const std::exception& e){Unavailable221("enable",e.what());}
 catch(...){Unavailable221("enable","unknown_enable_failure");}
}
void Disable() noexcept {active=false;if(activeHook)(void)activeHook.disable();}
void Publish() {
 if(!requested)return;static uint32_t last{};const auto now=GetTickCount();if(now-last<5000)return;last=now;
 Event(1,"CORONA_LIGHT20_COUNTS","active="+std::to_string(active.load())+" reconstructed="+std::to_string(emitted.load())+
  " approximated="+std::to_string(approximated.load())+" refused="+std::to_string(refused.load())+
  " context_refused="+std::to_string(contextRefused.load())+" collisions="+std::to_string(collisions.load())+
  " native_illumination=0 simultaneous_or_receiver_proof=0");
}
}
