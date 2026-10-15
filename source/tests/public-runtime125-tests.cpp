#define NOMINMAX
#include "SettingsRuntime117.hpp"
#include <cassert>
#include <iostream>
unsigned reads125{},pathReads125{},tick125{};
UINT ReadIni125(LPCWSTR section,LPCWSTR key,INT fallback,LPCWSTR file){++reads125;return ::GetPrivateProfileIntW(section,key,fallback,file);}
#define GetPrivateProfileIntW ReadIni125
#include "RuntimeFeaturePolicy125.hpp"
#undef GetPrivateProfileIntW
namespace preset117 {ReadResult startup;}
namespace pe {std::filesystem::path profile125;std::filesystem::path ModulePath(HMODULE){++pathReads125;return profile125;}}
HMODULE self{};bool ownExtra{},ownHeadlight{},ownVehicle{},policeTestEnabled{},nearbyPoliceEnabled{};
namespace Liberty20 {bool dualRequested{},worldRequested{};}
namespace TailLight20 {bool requested{};}
namespace WorldGeometry20 {bool requested{};}
namespace ModelLightShadow20 {bool requested{};}
namespace CoronaLight20 {bool requested{};}
namespace NativeProjection66 {bool requested{},replacementRequested{};}
namespace ModelEffectObserver {bool requested{};}
namespace ConsumerShadow75 {bool requested{};}
namespace NativeProducerGate116 {bool requested{};}
namespace PlayerShadowAllocation {bool additionalFeatures{};}
std::atomic<bool> proofRequested{},vacancyRequested{};std::atomic<uint32_t> policePolicy{};
void Event(int,const char*,const std::string&){}
namespace traffic_signal {enum class End {Thread};}
namespace CShadows {uint32_t counter125=1;uint32_t* pFrameCounter=&counter125;}
namespace TrafficSignal {
 bool requested{};std::atomic<bool> promotionRequested{},enabled{},fatal{};std::atomic<uint32_t> observerThread{};std::atomic<unsigned> collisionReason{};
 struct Registry125 {bool disabled{};unsigned count{},disables{};void Disable(){disabled=true;++disables;}void Disable(traffic_signal::End){Disable();}bool Disabled(){return disabled;}unsigned Used(){return count;}} registry;
 struct Snapshot125 {std::atomic<unsigned> state{0};};std::array<Snapshot125,4> snapshots{};
 unsigned events125{};void RecordEvent(int,unsigned,uint32_t){++events125;}void FatalThread(){fatal=true;}
}
DWORD Clock125(){return tick125;}
#define GetTickCount Clock125
#include <public-runtime125-extracted.inc>
#undef GetTickCount
void Reset125(){
 reads125=pathReads125=0;proofRequested=vacancyRequested=false;policePolicy=0;
 TrafficSignal::registry={};TrafficSignal::fatal=false;TrafficSignal::events125=0;TrafficSignal::enabled=true;TrafficSignal::observerThread=GetCurrentThreadId();
 for(auto& snapshot125:TrafficSignal::snapshots)snapshot125.state=0;
}
void Tick125(){tick125+=500;UpdateProof();TrafficSignal::Poll();}
int main(){
 using namespace preset117;wchar_t temporary125[32768]{};assert(GetTempPathW(32768,temporary125));
 pe::profile125=std::filesystem::path(temporary125)/(L"ls-runtime125-"+std::to_wstring(GetCurrentProcessId())+L".ini");assert(!std::filesystem::exists(pe::profile125));
 // Exact minimal public schema with no private LAB/LIGHTING20 keys.
 for(auto preset125:{Preset::Low,Preset::Medium,Preset::High}){
  assert(WritePrivateProfileStringW(L"LIBERTY_SHADOWS",L"SchemaVersion",L"3",pe::profile125.c_str()));assert(WritePrivateProfileStringW(L"LIBERTY_SHADOWS",L"Enabled",L"1",pe::profile125.c_str()));assert(WritePrivateProfileStringW(L"LIBERTY_SHADOWS",L"Preset",Name(preset125),pe::profile125.c_str()));
  startup=ReadStartup124(pe::profile125.c_str());assert(startup.ownBackend);Reset125();ConfigurePublic125();
  const bool wanted125=preset125!=Preset::Low;TrafficSignal::registry.count=wanted125?4u:0u;
  for(unsigned frame125=0;frame125<40;++frame125){Tick125();assert(TrafficSignal::promotionRequested==wanted125&&!TrafficSignal::registry.Disabled()&&proofRequested==wanted125&&policePolicy==(wanted125?1u:0u)&&vacancyRequested==wanted125);}
  assert(!reads125&&!pathReads125&&!TrafficSignal::events125&&!TrafficSignal::registry.disables);
  // Private stale switches cannot override installed schema3 authority.
  for(const wchar_t* value125:{L"0",L"1"}){
   for(const wchar_t* key125:{L"PoliceProof",L"PolicePolicy",L"PoliceVacancy",L"TrafficPromote"})assert(WritePrivateProfileStringW(L"LAB",key125,value125,pe::profile125.c_str()));assert(WritePrivateProfileStringW(L"LIGHTING20",L"TrafficSignalShadows",value125,pe::profile125.c_str()));
   Tick125();assert(TrafficSignal::promotionRequested==wanted125&&proofRequested==wanted125&&policePolicy==(wanted125?1u:0u)&&vacancyRequested==wanted125&&!TrafficSignal::registry.Disabled()&&!reads125&&!pathReads125);
  }
  assert(WritePrivateProfileStringW(L"LAB",nullptr,nullptr,pe::profile125.c_str()));assert(WritePrivateProfileStringW(L"LIGHTING20",nullptr,nullptr,pe::profile125.c_str()));
 }
 // Custom and Off use the installed feature array, including repeat rereads.
 auto custom125=*Defaults(Preset::High);Change(custom125,Feature::TrafficSignals,false);Change(custom125,Feature::EmergencyLights,false);assert(Save(pe::profile125,custom125));startup=ReadStartup124(pe::profile125.c_str());Reset125();ConfigurePublic125();for(unsigned frame125=0;frame125<20;++frame125){Tick125();assert(!TrafficSignal::promotionRequested&&!proofRequested&&!policePolicy&&!vacancyRequested);}
 auto off125=*Defaults(Preset::High);off125.enabled=false;assert(Save(pe::profile125,off125));startup=ReadStartup124(pe::profile125.c_str());Reset125();ConfigurePublic125();Tick125();assert(!PlayerShadowAllocation::additionalFeatures&&!TrafficSignal::requested&&!proofRequested&&!NativeProjection66::requested&&!ModelEffectObserver::requested&&!reads125&&!pathReads125);
 // The same recurring UpdateProof and UpdateNearby paths retain legacy keys.
 assert(WritePrivateProfileStringW(L"LIBERTY_SHADOWS",nullptr,nullptr,pe::profile125.c_str()));startup=ReadStartup124(pe::profile125.c_str());assert(!startup.ownBackend);Reset125();policeTestEnabled=nearbyPoliceEnabled=true;TrafficSignal::requested=true;TrafficSignal::promotionRequested=true;TrafficSignal::registry.count=4;
 Tick125();assert(!TrafficSignal::promotionRequested&&TrafficSignal::registry.Disabled()&&TrafficSignal::registry.disables==1&&TrafficSignal::events125==1&&!proofRequested&&!policePolicy&&!vacancyRequested&&reads125==5&&pathReads125==2);for(const auto& snapshot125:TrafficSignal::snapshots)assert(snapshot125.state==2);
 for(const wchar_t* key125:{L"PoliceProof",L"PolicePolicy",L"PoliceVacancy",L"TrafficPromote"})assert(WritePrivateProfileStringW(L"LAB",key125,L"1",pe::profile125.c_str()));Reset125();policeTestEnabled=nearbyPoliceEnabled=true;TrafficSignal::requested=true;TrafficSignal::promotionRequested=true;Tick125();assert(TrafficSignal::promotionRequested&&proofRequested&&policePolicy==1&&vacancyRequested&&!TrafficSignal::registry.Disabled());
 assert(WritePrivateProfileStringW(L"LIGHTING20",L"TrafficSignalShadows",L"0",pe::profile125.c_str()));Tick125();assert(!TrafficSignal::promotionRequested&&TrafficSignal::registry.Disabled());
 // Single-proof legacy route has the same actual recurring policy reads.
 nearbyPoliceEnabled=false;for(const wchar_t* value125:{L"0",L"1"}){for(const wchar_t* key125:{L"PoliceProof",L"PolicePolicy",L"PoliceVacancy"})assert(WritePrivateProfileStringW(L"LAB",key125,value125,pe::profile125.c_str()));tick125+=500;UpdateProof();assert(proofRequested==(value125[0]==L'1')&&policePolicy==(value125[0]==L'1'?1u:0u)&&vacancyRequested==(value125[0]==L'1'));}
 assert(std::filesystem::remove(pe::profile125));
 std::cout<<"PASS125 actual Configure/500ms UpdateProof+UpdateNearby+Poll prefixes: minimal schema3 persists/public stale private keys ignored/Low Custom Off staydisabled/legacy rereads+retirement unchanged/no public INI reads\n";
}
