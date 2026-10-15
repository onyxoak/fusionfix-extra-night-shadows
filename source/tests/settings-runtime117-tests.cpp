#define NOMINMAX
#include "SettingsRuntime117.hpp"
#include <cassert>
#include <iostream>
#include <new>
#include "core/ShadowFloatingPointState.hpp"
namespace preset117 {ReadResult startup;State applied;bool backendLimited{},vehicleDepthUnavailable119{},backendReadFailed119{};void (*refreshApplied119Hook)() noexcept{};HMODULE module{};StartupLease runtimeLease;unsigned frontendCalls{};DWORD FrontendOnly117(HMODULE) noexcept {++frontendCalls;return 17;}}
namespace rage {struct CLightSource{};}
namespace shadow_cpu29 {enum class Phase {Sources};struct Scope {Scope(Phase,unsigned){}};}
unsigned trafficCalls{},tailCalls{},modelCalls{},worldCalls{},projectionCalls{};
namespace TrafficSignal {bool requested{},promotionRequested{},enabled{};void ValidateQueued(rage::CLightSource*,uint32_t){++trafficCalls;}}
namespace TailLight20 {bool requested{};std::atomic<bool> active{};void ValidateQueued(rage::CLightSource*,uint32_t){++tailCalls;}}
namespace ModelLightShadow20 {bool requested{};std::atomic<bool> active{};void ValidateQueued(rage::CLightSource*,uint32_t){++modelCalls;}}
namespace CoronaLight20 {bool requested{};}
namespace NativeProducerGate116 {bool requested=true;}
namespace ModelEffectObserver {bool requested{};struct LastErrorScope {DWORD saved=GetLastError();~LastErrorScope(){SetLastError(saved);}};}
unsigned backendCopies119{};HMODULE observedModule119{};
namespace Liberty20 {
 bool dualRequested{},worldRequested{};std::atomic<bool> dualActive{};
 void PromoteNativeWorld(rage::CLightSource*,uint32_t){++worldCalls;}
 bool Copy(uintptr_t address,void* out,size_t size) noexcept {
  ++backendCopies119;MEMORY_BASIC_INFORMATION info{};
  if(!VirtualQuery(reinterpret_cast<void*>(address),&info,sizeof(info))||info.State!=MEM_COMMIT||(info.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;
  const auto mode=info.Protect&0xff;if(mode!=PAGE_READWRITE&&mode!=PAGE_READONLY)return false;
  if(uint64_t(address)+size>reinterpret_cast<uintptr_t>(info.BaseAddress)+info.RegionSize)return false;
  __try{std::memcpy(out,reinterpret_cast<void*>(address),size);}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
  _mm_setcsr((_mm_getcsr()&~0x6000u)|0x4000u);SetLastError(1192);return true;
 }
}
namespace NativeProjection66 {bool requested{},replacementRequested{};std::atomic<bool> active{};void ValidateQueued(rage::CLightSource*,uint32_t){++projectionCalls;}}
namespace WorldGeometry20 {bool requested{};}
namespace ConsumerShadow75 {bool requested{};}
namespace PlayerShadowAllocation {bool additionalFeatures{};}
namespace pe {std::filesystem::path path;std::filesystem::path ModulePath(HMODULE){return path;}}
HMODULE self{};unsigned shadowStarts{};std::atomic<bool> failed{};
DWORD StartShadow117(const std::filesystem::path&){++shadowStarts;return 19;}
bool ownExtra{},ownHeadlight{},ownVehicle{},policeTestEnabled{},nearbyPoliceEnabled{},nativeBackend{},ownerProbe{};std::atomic<bool> addonActive{};
struct Flag {const bool* value{};bool* configured{};explicit operator bool() const{return value&&*value&&(!configured||*configured);}};
Flag bExtraNightShadows,bHeadlightShadows,bVehicleNightShadows;
uintptr_t ff{};struct Layout {unsigned extra=0,headlight=1,vehicle=2;} layoutValue;Layout* layout=&layoutValue;
void Event(int,const char*,const std::string&){}
HMODULE FixtureModule119(LPCWSTR){SetLastError(1193);return observedModule119;}
#define GetModuleHandleW FixtureModule119
#include <settings-runtime117-extracted.inc>
#undef GetModuleHandleW
void Reset(){preset117::runtimeLease.~StartupLease();new(&preset117::runtimeLease)preset117::StartupLease;shadowStarts=preset117::frontendCalls=0;failed=false;trafficCalls=tailCalls=modelCalls=worldCalls=projectionCalls=0;ownExtra=ownHeadlight=ownVehicle=false;ConsumerShadow75::requested=true;}
int main(){
 using namespace preset117;wchar_t tmp[32768]{};assert(GetTempPathW(32768,tmp));pe::path=std::filesystem::path(tmp)/(L"ls-runtime117-"+std::to_wstring(GetCurrentProcessId())+L".ini");assert(!std::filesystem::exists(pe::path));
 for(auto p:{Preset::Low,Preset::Medium,Preset::High}){
  Reset();auto s=*Defaults(p);s.enabled=false;assert(Save(pe::path,s));assert(Start(nullptr)==17&&frontendCalls==1&&shadowStarts==0&&!runtimeLease.Active());
  s.enabled=true;assert(Save(pe::path,s));assert(Start(nullptr)==19&&frontendCalls==1&&shadowStarts==1&&runtimeLease.Active());assert(Start(nullptr)==1&&shadowStarts==1);
  NativeProducerGate116::requested=true;
  ApplySettings117();assert(ownExtra&&ownHeadlight&&ownVehicle);
  assert(NativeProducerGate116::requested==(p==Preset::High));
  PrepareLightSources(reinterpret_cast<rage::CLightSource*>(uintptr_t(1)),640);
  if(p==Preset::Low)assert(!trafficCalls&&!tailCalls&&!modelCalls&&!worldCalls&&!projectionCalls&&!PlayerShadowAllocation::additionalFeatures);
  if(p==Preset::Medium)assert(trafficCalls==1&&!tailCalls&&!modelCalls&&!worldCalls&&!projectionCalls&&policeTestEnabled&&PlayerShadowAllocation::additionalFeatures);
  if(p==Preset::High)assert(trafficCalls==1&&tailCalls==1&&modelCalls==1&&worldCalls==1&&projectionCalls==1&&Liberty20::dualRequested&&WorldGeometry20::requested);
 }
 // Schema3 authority is owned; neither existingFF value is written. Legacy
 // and absent settings bind exactly to the existing backend effective gates.
 bool foreign[3]{false,true,false};ff=reinterpret_cast<uintptr_t>(foreign);
 for(bool own:{false,true}){startup.ownBackend=own;nativeBackend=false;BindSettings117();assert(bExtraNightShadows.value==(own?&ownExtra:&foreign[0])&&bHeadlightShadows.value==(own?&ownHeadlight:&foreign[1])&&bVehicleNightShadows.value==(own?&ownVehicle:&foreign[2]));assert(!foreign[0]&&foreign[1]&&!foreign[2]);}
 startup.ownBackend=false;nativeBackend=true;BindSettings117();assert(bExtraNightShadows.value==&ownExtra);
 // Absent public settings keep every old source preparation call intact.
 startup.present=false;trafficCalls=tailCalls=modelCalls=worldCalls=projectionCalls=0;PrepareLightSources(nullptr,0);assert(trafficCalls==1&&tailCalls==1&&modelCalls==1&&worldCalls==1&&projectionCalls==1);
 // New master supersedes the olderSHADOWS gate, while schema2/absent paths
 // preserve it before entering even one shadow service.
 Reset();assert(Save(pe::path,*Defaults(Preset::High)));assert(WritePrivateProfileStringW(L"SHADOWS",L"Enabled",L"0",pe::path.c_str()));assert(Start(nullptr)==19&&shadowStarts==1&&applied.enabled);
 Reset();assert(WritePrivateProfileStringW(liberty_settings93::Section,L"SchemaVersion",L"2",pe::path.c_str()));assert(Start(nullptr)==17&&frontendCalls==1&&!shadowStarts&&!applied.enabled&&!runtimeLease.Active());
 for(auto version:{L"2",L"3"})for(auto unsupported:{L"Max",L"Ultra",L"Unknown"}){
  Reset();assert(Save(pe::path,*Defaults(Preset::High)));assert(WritePrivateProfileStringW(liberty_settings93::Section,L"SchemaVersion",version,pe::path.c_str()));assert(WritePrivateProfileStringW(liberty_settings93::Section,L"Preset",unsupported,pe::path.c_str()));assert(Start(nullptr)==17&&frontendCalls==1&&!shadowStarts&&!applied.enabled&&!runtimeLease.Active()&&startup.unavailable);
 }
 assert(WritePrivateProfileStringW(liberty_settings93::Section,nullptr,nullptr,pe::path.c_str()));Reset();assert(Start(nullptr)==17&&frontendCalls==1&&!shadowStarts&&!applied.enabled&&!startup.present&&!startup.ownBackend);
 std::filesystem::remove(pe::path);Reset();assert(Start(nullptr)==19&&shadowStarts==1&&startup.present&&startup.ownBackend&&startup.state==*Defaults(Preset::High));
 ApplySettings117();assert(Liberty20::dualRequested&&TailLight20::requested&&TrafficSignal::requested&&nearbyPoliceEnabled&&ModelLightShadow20::requested&&CoronaLight20::requested&&NativeProjection66::requested&&NativeProjection66::replacementRequested&&WorldGeometry20::requested);
 // Existing legacy Off was proved above. An existing read-locked file is not
 // absence and cannot silently gain public High authority.
 assert(WritePrivateProfileStringW(L"SHADOWS",L"Enabled",L"1",pe::path.c_str()));
 HANDLE locked124=CreateFileW(pe::path.c_str(),GENERIC_READ,0,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);assert(locked124!=INVALID_HANDLE_VALUE);
 Reset();assert(Start(nullptr)==17&&frontendCalls==1&&!shadowStarts&&!applied.enabled&&startup.unavailable&&!startup.ownBackend);assert(CloseHandle(locked124));std::filesystem::remove(pe::path);
 // Actual119 snapshot reads a validated0/1FFvehicle gate, never changes it,
 // preservesFP/LastError, and adds no backend reads to theOff frontend.
 auto* gate=static_cast<uint8_t*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(gate);
 startup.present=startup.ownBackend=true;startup.state=*Defaults(Preset::High);applied=startup.state;
 addonActive=true;Liberty20::dualActive=true;TailLight20::active=true;TrafficSignal::enabled=true;ModelLightShadow20::active=true;NativeProjection66::active=true;ownerProbe=nearbyPoliceEnabled=true;
 ownExtra=ownHeadlight=ownVehicle=true;nativeBackend=false;ff=reinterpret_cast<uintptr_t>(gate);observedModule119=reinterpret_cast<HMODULE>(ff);BindSettings117();
 const auto mxcsr119=_mm_getcsr();_mm_setcsr((mxcsr119&~0x6000u)|0x2000u);SetLastError(1191);
 gate[2]=1;RefreshApplied119State();assert(applied==startup.state&&!backendLimited&&!vehicleDepthUnavailable119&&!backendReadFailed119&&gate[2]==1&&GetLastError()==1191&&(_mm_getcsr()&0x6000u)==0x2000u);
 gate[2]=0;RefreshApplied119State();assert(backendLimited&&vehicleDepthUnavailable119&&!backendReadFailed119&&!applied.features[0]&&!applied.features[1]&&gate[2]==0&&startup.state==*Defaults(Preset::High));
 gate[2]=2;RefreshApplied119State();assert(backendLimited&&backendReadFailed119&&!vehicleDepthUnavailable119&&gate[2]==2);
 DWORD protect119{};assert(VirtualProtect(gate,4096,PAGE_READWRITE|PAGE_GUARD,&protect119));RefreshApplied119State();assert(backendReadFailed119);MEMORY_BASIC_INFORMATION info119{};assert(VirtualQuery(gate,&info119,sizeof(info119))&&(info119.Protect&PAGE_GUARD));
 assert(VirtualProtect(gate,4096,PAGE_NOACCESS,&protect119));RefreshApplied119State();assert(backendReadFailed119);
 const auto copies119=backendCopies119;observedModule119=nullptr;RefreshApplied119State();assert(backendReadFailed119&&backendCopies119==copies119);
 observedModule119=reinterpret_cast<HMODULE>(ff);nativeBackend=true;RefreshApplied119State();assert(backendReadFailed119&&backendCopies119==copies119);
 addonActive=false;applied=startup.state;applied.enabled=false;backendLimited=false;backendReadFailed119=vehicleDepthUnavailable119=false;RefreshApplied119State();assert(!applied.enabled&&!backendLimited&&backendCopies119==copies119);
 assert(VirtualFree(gate,0,MEM_RELEASE));_mm_setcsr(mxcsr119);
 std::cout<<"PASS117 actual startup/dispatch/binding plus119actualFFsnapshot/guard/FP/Off/no writes\n";
}
