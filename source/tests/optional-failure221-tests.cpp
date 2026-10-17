#define NOMINMAX
#include "standalone-pe.hpp"
#include "standalone-ownership.hpp"
#include "ModelLightSubmission20.hpp"
#include "CoronaLightPolicy20.hpp"
#include <atomic>
#include <cassert>
#include <iostream>
#include <memory>
#include <limits>
using namespace onyxoak;
struct SafetyHookContext {};
unsigned prepares221{},enables221{},reads221{},throwPrepare221{},failEnable221{};
bool wrongSpan221{},foreignBranch221{},unknownRead221{};
struct SafetyHookMid {
 bool valid{},enabled{};uintptr_t target{};std::array<uint8_t,6> bytes{};size_t length=5;
 explicit operator bool()const noexcept{return valid;}
 bool enable()noexcept {++enables221;enabled=valid&&enables221!=failEnable221;return enabled;}
 bool disable()noexcept {enabled=false;return true;}
 std::span<const uint8_t> original_bytes()const noexcept{return {bytes.data(),length};}
 uintptr_t target_address()const noexcept{return target;}
};
uintptr_t game=0x440000;bool nativeBackend=true,fusionCallbacks213Authenticated=true;
struct FusionHooks221{uint32_t flagsCallback=0xa6af0;} hooks221;
FusionHooks221* fusionHooks213=&hooks221;
std::unique_ptr<pe::Image> original;std::vector<uint8_t> live221;
struct Logged221{std::string event,fields;};std::vector<Logged221> logged221;
void Event(unsigned,const char* event,const std::string& fields){logged221.push_back({event,fields});}
void Require(bool value,const char* reason){if(!value)throw std::runtime_error(reason);}
uint32_t U32(const uint8_t* b){uint32_t value;std::memcpy(&value,b,4);return value;}
std::vector<uint8_t> ReadMemory(uintptr_t at,size_t size){
 ++reads221;if(unknownRead221)throw 42;
 assert(at>=game&&uint64_t(at-game)+size<=live221.size());
 return {live221.begin()+at-game,live221.begin()+at-game+size};
}
uintptr_t OriginalCall(uint32_t rva){assert(original->bytes[rva]==0xe8);return game+rva+5+U32(original->bytes.data()+rva+1);}
void VerifyBranch(uint32_t,uint32_t,bool,unsigned){if(foreignBranch221)throw std::runtime_error("foreign_mid_callback");}
void Prepare(SafetyHookMid& hook,uintptr_t at,void(*)(SafetyHookContext&)noexcept){
 ++prepares221;hook.valid=true;hook.target=at;hook.length=wrongSpan221?6u:5u;
 if(prepares221==throwPrepare221)throw std::runtime_error("mid_prepare_failed");
}
namespace TrafficSignal{bool Readable(uint32_t,size_t){return true;}}
namespace ModelLightShadow20{std::atomic<bool> active{true};}
namespace ModelEffectObserver {
 using namespace model_effect_observer;
 bool requested=true,prepared=false;std::atomic<bool> enabled{false};
 SafetyHookMid preHook,postHook,resolveHook,wrapperHook,directHook,wrapperSpotHook,directSpotHook;
 void Pre(SafetyHookContext&)noexcept{} void Post(SafetyHookContext&)noexcept{} void Resolve(SafetyHookContext&)noexcept{}
 void Wrapper(SafetyHookContext&)noexcept{} void Direct(SafetyHookContext&)noexcept{}
 void WrapperSpot(SafetyHookContext&)noexcept{} void DirectSpot(SafetyHookContext&)noexcept{}
 #include "model-lifecycle221.inc"
}
namespace CoronaLight20 {
 bool requested=true,prepared=false;std::atomic<bool> active{false};SafetyHookMid activeHook;
 corona_light20::Settings settings;
 void Submit(SafetyHookContext&)noexcept{}
 #include "corona-lifecycle221.inc"
}
#include "optional-ranges221.inc"
bool Has221(std::string_view text){for(const auto& e:logged221)if(e.fields.find(text)!=std::string::npos)return true;return false;}
void Reset221(){
 using namespace ModelEffectObserver;
 preHook={};postHook={};resolveHook={};wrapperHook={};directHook={};wrapperSpotHook={};directSpotHook={};
 requested=true;prepared=false;enabled=false;
 CoronaLight20::requested=true;CoronaLight20::prepared=false;CoronaLight20::active=false;CoronaLight20::activeHook={};CoronaLight20::settings={};
 prepares221=enables221=reads221=throwPrepare221=failEnable221=0;wrongSpan221=foreignBranch221=unknownRead221=false;
 nativeBackend=true;ModelLightShadow20::active=true;logged221.clear();live221=original->bytes;
}
void AssertDormant221(){
 assert(!ModelEffectObserver::enabled&&!CoronaLight20::active);
 for(const auto* h:{&ModelEffectObserver::preHook,&ModelEffectObserver::postHook,&ModelEffectObserver::resolveHook,&ModelEffectObserver::wrapperHook,&ModelEffectObserver::directHook,&ModelEffectObserver::wrapperSpotHook,&ModelEffectObserver::directSpotHook,&CoronaLight20::activeHook})assert(!h->enabled);
 std::vector<PreparedHookRange> ranges{{game+0x63e1ca,5}};AppendPreparedOptionalRanges221(ranges);
 assert(ranges.size()==1&&DisjointPreparedHooks(ranges));
}
int main(int argc,char**argv){
 assert(argc==2);const auto raw=pe::Read(argv[1]);unsigned cases=0;
 for(uint32_t base:{0x440000u,0x160000u}){
  game=base;original=std::make_unique<pe::Image>(raw,base);
  Reset221();ModelEffectObserver::PrepareHooks();assert(ModelEffectObserver::prepared&&prepares221==7);
  CoronaLight20::PrepareHooks();assert(CoronaLight20::prepared&&prepares221==8);
  ModelEffectObserver::EnableHooks();CoronaLight20::EnableHooks();assert(ModelEffectObserver::enabled&&CoronaLight20::active&&enables221==8);
  {std::vector<PreparedHookRange> ranges;AppendPreparedOptionalRanges221(ranges);assert(ranges.size()==8&&DisjointPreparedHooks(ranges));}++cases;

  // Exact original failure chain: foreign FlagsHook owner is refused locally,
  // then its corona dependent also stays off without throwing through Install.
  Reset221();nativeBackend=false;foreignBranch221=true;
  ModelEffectObserver::PrepareHooks();CoronaLight20::PrepareHooks();ModelEffectObserver::EnableHooks();CoronaLight20::EnableHooks();
  assert(Has221("foreign_mid_callback")&&Has221("features_unavailable=model_attached_provenance,authored_corona_reconstruction")&&Has221("corona_model_observer_prepare_unavailable"));
  assert(prepares221==0&&reads221==0);AssertDormant221();++cases;

  // Invalid user settings are distinguished from a missing dependency.
  Reset221();CoronaLight20::settings.maxRadius=std::numeric_limits<float>::quiet_NaN();CoronaLight20::PrepareHooks();
  assert(Has221("corona_settings_invalid")&&!Has221("corona_model_observer_prepare_unavailable")&&reads221==0&&prepares221==0);AssertDormant221();++cases;

  // Every partially prepared model hook and every partial enable is disabled.
  for(unsigned failed=1;failed<=7;++failed){
   Reset221();throwPrepare221=failed;ModelEffectObserver::PrepareHooks();CoronaLight20::PrepareHooks();
   assert(prepares221==failed&&Has221("mid_prepare_failed")&&Has221("corona_model_observer_prepare_unavailable"));AssertDormant221();++cases;
   Reset221();ModelEffectObserver::PrepareHooks();CoronaLight20::PrepareHooks();failEnable221=failed;
   ModelEffectObserver::EnableHooks();CoronaLight20::EnableHooks();
   assert(Has221("model_observer_hook_enable")&&Has221("corona_model_observer_enable_unavailable"));AssertDormant221();++cases;
  }
  Reset221();ModelEffectObserver::prepared=true;live221[0x81ea60]^=1;CoronaLight20::PrepareHooks();
  assert(!CoronaLight20::prepared&&!CoronaLight20::active&&prepares221==0&&Has221("corona_native_producer_modified"));++cases;
  Reset221();ModelEffectObserver::prepared=true;wrongSpan221=true;CoronaLight20::PrepareHooks();
  assert(!CoronaLight20::prepared&&!CoronaLight20::active&&CoronaLight20::activeHook.valid&&!CoronaLight20::activeHook.enabled&&Has221("corona_hook_stole_cleanup"));
  {ModelEffectObserver::prepared=false;std::vector<PreparedHookRange> ranges{{CoronaLight20::activeHook.target_address(),5}};AppendPreparedOptionalRanges221(ranges);assert(ranges.size()==1&&DisjointPreparedHooks(ranges));}++cases;
  Reset221();ModelEffectObserver::prepared=true;unknownRead221=true;CoronaLight20::PrepareHooks();
  assert(!CoronaLight20::prepared&&Has221("unknown_preparation_failure"));++cases;
  Reset221();ModelEffectObserver::prepared=true;ModelEffectObserver::enabled=true;CoronaLight20::PrepareHooks();ModelLightShadow20::active=false;CoronaLight20::EnableHooks();
  assert(!CoronaLight20::active&&!CoronaLight20::prepared&&Has221("corona_model_lifetime_unavailable"));++cases;
  Reset221();ModelEffectObserver::prepared=true;ModelEffectObserver::enabled=true;CoronaLight20::PrepareHooks();failEnable221=1;CoronaLight20::EnableHooks();
  assert(!CoronaLight20::active&&!CoronaLight20::prepared&&!CoronaLight20::activeHook.enabled&&Has221("corona_hook_enable_failed"));++cases;
  Reset221();ModelEffectObserver::requested=false;CoronaLight20::requested=false;ModelEffectObserver::PrepareHooks();CoronaLight20::PrepareHooks();ModelEffectObserver::EnableHooks();CoronaLight20::EnableHooks();
  assert(prepares221==0&&reads221==0&&enables221==0&&logged221.empty());++cases;
 }
 std::cout<<"PASS "<<cases<<" production lifecycle221 cases at two game bases: normal path, foreign callback dependency, invalid settings, seven partial prepare/enable failures, changed native producer, wrong span, unknown read failure, missing lifetime, failed corona enable, dormant range cleanup and disabled no-op\n";
}
