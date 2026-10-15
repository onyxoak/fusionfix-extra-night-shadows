#define NOMINMAX
#include <windows.h>
#include <atomic>
#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include "StartupConflict129.hpp"
#include "StartupGuidance210.hpp"
#include "standalone-support-log.hpp"
#include "standalone-ownership.hpp"
#include "core/ShadowFloatingPointState.hpp"
namespace fusionfix::shadows::ce {inline constexpr uint32_t FrameCounterRva=0x00D73604;}
using namespace onyxoak;
namespace startup129_fixture {
constexpr char CandidateBuild32[]="libertys-shadows-2.0-candidate129-startup-conflict-report";
namespace pe {
 struct Image {std::vector<uint8_t> bytes;std::vector<IMAGE_SECTION_HEADER> sections;};
 std::filesystem::path modulePath;
 std::filesystem::path ModulePath(HMODULE){return modulePath;}
}
std::unique_ptr<pe::Image> original;
std::unique_ptr<SupportLog> log;
HMODULE self{};
uintptr_t game=0x400000,ff=0x60000000;
void Require(bool result,const char* error){if(!result)throw std::runtime_error(error);}
#include "StartupConflict129Runtime.inc"
std::vector<uint8_t> capturedLive129;
struct Layout {uint32_t player=0xa7c40,npc=0xa7c30,admission{},lookup{},render{},caster{},contact{},frame{},frameData{};} layoutValue;
Layout* layout=&layoutValue;
bool nativeBackend{};unsigned verifiedBranches{};
std::vector<uint8_t> ReadMemory(uintptr_t address,size_t count){
 assert(address>=game&&address-game+count<=capturedLive129.size());
 return {capturedLive129.begin()+address-game,capturedLive129.begin()+address-game+count};
}
void VerifyBranch(uint32_t rva,uint32_t callback,bool mid,size_t length,uint8_t op){
 assert(!mid&&length==5&&op==0xe8&&capturedLive129[rva]==op);
 int32_t relative{};std::memcpy(&relative,capturedLive129.data()+rva+1,4);
 assert(game+rva+5+relative==ff+callback);++verifiedBranches;
}
uint32_t U32(const uint8_t* p){uint32_t value{};std::memcpy(&value,p,4);return value;}
#include "startup129-normalize.inc"
namespace TailLight20 {bool requested{};}
namespace ModelLightShadow20 {bool requested{};}
namespace CoronaLight20 {bool requested{};}
struct SafetyHookContext {};
struct Hook {unsigned prepared{};};
Hook freeObserver,dualPre,dualPost,singlePre;
bool dualRequested{},worldRequested{},worldActive{};
void Free(SafetyHookContext&){}void Pre(SafetyHookContext&){}void Post(SafetyHookContext&){}void Single(SafetyHookContext&){}
void Prepare(Hook& hook,uintptr_t,void(*)(SafetyHookContext&)){++hook.prepared;}
uintptr_t OriginalCall(uint32_t rva){assert(rva==0x63fe11);return game+0x63e070;}
alignas(64) uint8_t keyStorage[64]{};
struct Registry {bool Configure(uint32_t){return true;}} lampRegistry;
#include "startup129-prepare.inc"
std::string Read(const std::filesystem::path& path){std::ifstream f(path);return {std::istreambuf_iterator<char>(f),{}};}
void Reset(){startupDetails213.clear();startupMismatch129={};startupAdviceEvidence210={};startupFailureAttempt129.clear();freeObserver={};dualPre={};dualPost={};singlePre={};}
int Run(){
 const auto folder=std::filesystem::current_path()/"startup129-output";
 assert(!std::filesystem::exists(folder));std::filesystem::create_directory(folder);
 const auto path=folder/"quiet.log";
 original=std::make_unique<pe::Image>();original->bytes.resize(0x70bb00,0xcc);
 IMAGE_SECTION_HEADER section{};section.VirtualAddress=0x63de90;section.Misc.VirtualSize=0x70ba22-0x63de90;section.Characteristics=IMAGE_SCN_MEM_EXECUTE;original->sections.push_back(section);
 capturedLive129=original->bytes;
 // Exact official511 declared call overlays: production GuardedImage owns
 // these four RVAs/callbacks and uses the real normalization helper.
 for(const auto rva:{0x63dfeau,0x63e1cau,0x63dfffu,0x63e1dfu}){
  const auto callback=(rva==0x63dfea||rva==0x63e1ca)?layout->player:layout->npc;
  capturedLive129[rva]=0xe8;const auto relative=int32_t(ff+callback-(game+rva+5));std::memcpy(capturedLive129.data()+rva+1,&relative,4);
 }
 log=std::make_unique<SupportLog>(path,0);dualRequested=true;
 auto normalized=GuardedImage();assert(normalized==original->bytes&&verifiedBranches==4);
 PrepareHooks(normalized);assert(dualPre.prepared==1&&dualPost.prepared==1&&singlePre.prepared==1&&!startupMismatch129.valid);
 log->Event(1,"START","working=1");assert(!std::filesystem::exists(path));
 // Actual normalized producer guards run BEFORE dual preparation, preserving
 // the original throw code, full range and last-byte coverage.
 for(const auto rva:{0x63f5e0u,0x63ff1eu,0x63e1eau,0x70ba21u}){
  Reset();auto modified=normalized;modified[rva]^=1;bool rejected=false;
  try{PrepareHooks(modified);}catch(const std::runtime_error& e){
   rejected=true;assert(std::string(e.what())==(rva==0x70ba21?"vehicle_free_observer_modified":"dual_native_producer_modified"));
  }
  assert(rejected&&startupMismatch129.valid&&startupMismatch129.rva==rva&&dualPre.prepared==0&&dualPost.prepared==0&&singlePre.prepared==0);
  assert(startupMismatch129.expected[0]==0xcc&&startupMismatch129.actual[0]==0xcd);
  if(rva==0x63ff1e||rva==0x63e1ea||rva==0x70ba21)assert(startupMismatch129.count==1);
 }
 Reset();auto modified=normalized;modified[0x63f5e0]^=1;
 try{PrepareHooks(modified);}catch(const std::exception& e){
  const auto csr=_mm_getcsr();unsigned short originalControl129{},testControl129{},afterControl129{};
  __asm fnstcw originalControl129
  testControl129=static_cast<unsigned short>((originalControl129&~0xc00u)|0x400u);
  __asm fldcw testControl129
  SetLastError(12999);_mm_setcsr((csr&~0x6000u)|0x4000u);
  StartupFailure129(e.what());
  __asm fnstcw afterControl129
  assert(afterControl129==testControl129&&GetLastError()==12999&&(_mm_getcsr()&0x6000u)==0x4000u);
  __asm fldcw originalControl129
  _mm_setcsr(csr);
 }
 const auto text=Read(path);assert(text.size()<512&&text.find("STARTUP_FAILURE")!=text.npos&&text.find("dual_native_producer_modified")!=text.npos&&text.find("normalized_game_rva=0063F5E0 expected=CCCCCCCCCCCCCCCC actual=CDCCCCCCCCCCCCCC")!=text.npos);
 StartupFailure129("second_failure");log->StartupFailure("second_direct_attempt");log->Event(1,"ORDINARY","not_written");assert(Read(path)==text);
 // Unknown exception text never discloses a path, name or token.
 const auto safe=startup_conflict129::Fields(CandidateBuild32,"C:\\Users\\Private\\secret",{});
 assert(safe.find("Private")==safe.npos&&safe.find("secret")==safe.npos&&safe.find("error=startup_exception")!=safe.npos);
 assert(startup_conflict129::Fields(CandidateBuild32,"plain_ascii_user_token",{}).find("plain_ascii_user_token")==std::string::npos);
 std::array<uint8_t,3> bytes{1,2,3},different{1,2,4};
 assert(!startup_conflict129::FirstMismatch(bytes,bytes,0).valid);
 assert(!startup_conflict129::FirstMismatch(bytes,different,UINT32_MAX).valid);
 assert(!startup_conflict129::FirstMismatch(bytes,std::span<const uint8_t>(different.data(),2),0).valid);
 // Earliest Start fallback creates one report only; inaccessible output is
 // contained and cannot retry every frame or replace the native refusal.
 Reset();log.reset();pe::modulePath=folder/"earliest.asi";StartupFailure129("startup_fixed_error");assert(Read(folder/"earliest.log").find("normalized_game_rva=unavailable")!=std::string::npos);
 Reset();{std::ofstream blocker(folder/"blocked");blocker<<"file";}
 pe::modulePath=folder/"blocked"/"cannot.asi";SetLastError(12877);StartupFailure129("startup_fixed_error");assert(GetLastError()==12877&&!std::filesystem::exists(folder/"blocked"/"cannot.log"));
 assert(startupFailureAttempt129.test_and_set());
 // Fatal guidance is written even with ordinary logging disabled. Only
 // a detected module undergoing adapter validation is named as a VL failure.
 const std::string volumeHash210(64,'a');unsigned helpCase210=0;
 for(const char* error210:{"volumetric199_unsupported_binary","volumetric199_setting_range","volumetric199_outer_owner","volumetric199_unsupported_fusionfix","unreadable_code"}){
  Reset();const auto helpPath210=folder/("help210-"+std::to_string(helpCase210++)+".log");
  log=std::make_unique<SupportLog>(helpPath210,0);
  RecordVolumetric210(startup_guidance210::Volume::Checking,volumeHash210);
  StartupFailure129(error210);const auto advice210=Read(helpPath210);
  assert(advice210.find("Disable it temporarily: close GTA IV")!=std::string::npos);
  assert(advice210.find("VolumetricLights.asi and VolumetricLights.ini outside the GTA IV game folder")!=std::string::npos);
  assert(advice210.find("volumetric_sha256="+volumeHash210)!=std::string::npos);
  if(std::string_view(error210).starts_with("volumetric199_"))assert(advice210.find(std::string("error=")+error210)!=std::string::npos);
  assert(advice210.size()<512);StartupFailure129("second_failure");assert(Read(helpPath210)==advice210);
 }
 Reset();const auto acceptedPath210=folder/"accepted210.log";log=std::make_unique<SupportLog>(acceptedPath210,0);
 RecordVolumetric210(startup_guidance210::Volume::Checking,volumeHash210);
 RecordVolumetric210(startup_guidance210::Volume::Accepted);StartupFailure129("corona_context_or_settings_invalid");
 const auto accepted210=Read(acceptedPath210);assert(accepted210.find("VolumetricLights was accepted")!=std::string::npos);
 assert(accepted210.find("Disable it temporarily")==std::string::npos);
 assert(accepted210.find("error=corona_context_or_settings_invalid")!=std::string::npos);
 Reset();const auto unknownPath210=folder/"unknown210.log";log=std::make_unique<SupportLog>(unknownPath210,0);
 StartupFailure129("dual_native_producer_modified");const auto unknown210=Read(unknownPath210);
 assert(unknown210.find("VolumetricLights")==std::string::npos&&unknown210.find("ASI list")!=std::string::npos);
 startup_guidance210::Evidence unsafeHash210;unsafeHash210.SetHash("C:\\Users\\Private\\secret");
 assert(startup_guidance210::Help("user_secret_token",unsafeHash210).find("secret")==std::string::npos);
 unsafeHash210.SetHash(std::string(64,'z'));assert(startup_guidance210::Help("startup_exception",unsafeHash210).find("volumetric_sha256=")==std::string::npos);
 // Successful guard checks remain quiet; advice does not poll or open files.
 Reset();const auto quietPath210=folder/"success210.log";log=std::make_unique<SupportLog>(quietPath210,0);
 RecordVolumetric210(startup_guidance210::Volume::Accepted,volumeHash210);
 RequireNativeRange129(normalized,0x63f5e0,8,"dual_native_producer_modified");
 assert(!std::filesystem::exists(quietPath210));
 Reset();const auto details213=folder/"details213.log";log=std::make_unique<SupportLog>(details213,0);
 RecordStartupExtra213("fusionfix_version=5.1.0 callback_module=OtherLighting.asi");
 startupMismatch129={true,0x6bcc50,1,{0xff},{0xe9}};StartupFailure129("headlight_guard_failed");
 const auto detailText213=Read(details213);assert(detailText213.find("fusionfix_version=5.1.0")!=std::string::npos&&detailText213.find("normalized_game_rva=006BCC50")!=std::string::npos&&detailText213.find("OtherLighting.asi")!=std::string::npos);
 StartupFailure129("headlight_guard_failed");assert(Read(details213)==detailText213);
 std::cout<<"PASS210 exact VL error codes; fatal help with Logging0; disable-ASI-and-INI instructions; accepted and unknown-owner separation; fixed-format hash only; quiet success and one fatal report\n";
 std::cout<<"PASS actual GuardedImage511 normalization and full producer/free guards; first/last-byte mismatch before throw; quiet success no IO; one bounded fatal report; unsafe fields omitted; failure IO contained; FP and LastError preserved\n";
 return 0;
}
}
int main(){return startup129_fixture::Run();}
