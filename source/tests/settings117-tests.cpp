#define NOMINMAX
#include "PresetIni117.hpp"
#include "SettingsRuntime117.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>
namespace preset117 {ReadResult startup;}
using namespace preset117;
int main(){
 const auto low=*Defaults(Preset::Low),medium=*Defaults(Preset::Medium),high=*Defaults(Preset::High);
 assert(Requests(low).base&&!Requests(low).dual&&!Requests(low).model&&!Requests(low).projection&&!Requests(low).worldGeometry);
 assert(Requests(medium).traffic&&Requests(medium).emergency&&!Requests(medium).model&&!Requests(medium).tail);
 const auto p=Requests(high);assert(p.base&&p.dual&&p.tail&&p.traffic&&p.emergency&&p.model&&p.corona&&p.projection&&p.keyedWorld&&p.worldGeometry);
 for(auto state:{low,medium,high}){StartupLease lease;unsigned front{},shadows{};auto off=state;off.enabled=false;
  assert(!Requests(off).base&&!Requests(off).projection&&Implementation(off)==Engine::None);
  assert(RouteStartup117(off,lease,[&]{++front;return 17;},[&]{++shadows;return 19;})==17&&front==1&&shadows==0&&!lease.Active());
  assert(RouteStartup117(state,lease,[&]{++front;return 17;},[&]{++shadows;return 19;})==19&&front==1&&shadows==1&&lease.Active());
  assert(RouteStartup117(off,lease,[&]{++front;return 17;},[&]{++shadows;return 19;})==1&&front==1&&shadows==1);
 }
 StartupLease lease;std::atomic<unsigned> wins{};std::vector<std::thread> ts;for(unsigned i=0;i<20;++i)ts.emplace_back([&]{if(lease.Acquire())++wins;});for(auto& t:ts)t.join();assert(wins==1);
 wchar_t temp[32768]{};assert(GetTempPathW(32768,temp));auto f=std::filesystem::path(temp)/(L"ls-settings117-"+std::to_wstring(GetCurrentProcessId())+L".ini");assert(!std::filesystem::exists(f));
 assert(!Read(f.c_str()).present);assert(!UseOwnedBackend117(false)&&UseOwnedBackend117(true));
 assert(WritePrivateProfileStringW(L"SUPPORT",L"NativeOwnerBatch115",L"1",f.c_str()));
 for(auto s:{low,medium,high}){assert(Save(f,s));const auto r=Read(f.c_str());assert(r.present&&r.ownBackend&&!r.invalid&&r.state==s);startup=r;assert(UseOwnedBackend117(false));}
 assert(GetPrivateProfileIntW(L"SUPPORT",L"NativeOwnerBatch115",0,f.c_str())==1);
 assert(WritePrivateProfileStringW(liberty_settings93::Section,L"Preset",L"Low",f.c_str()));auto r=Read(f.c_str());assert(r.state==low&&!r.invalid);
 assert(WritePrivateProfileStringW(liberty_settings93::Section,L"Preset",L"Medium",f.c_str()));assert(Read(f.c_str()).state==medium);
 // Retired124 VeryHigh files migrate to ordinary High, never invalid/Off.
 static_assert(NamedPresetCount122==3);assert(!Implemented(Preset::VeryHigh)&&!Defaults(Preset::VeryHigh));
 for(auto version125:{L"2",L"3"})for(auto name125:{L"VeryHigh",L"Very High",L"vErYhIgH"})for(bool on125:{false,true}){
  auto old125=high;old125.enabled=on125;assert(Save(f,old125));
  assert(WritePrivateProfileStringW(liberty_settings93::Section,L"SchemaVersion",version125,f.c_str()));
  assert(WritePrivateProfileStringW(liberty_settings93::Section,L"Preset",name125,f.c_str()));
  assert(WritePrivateProfileStringW(liberty_settings93::Section,L"StreetlightReachFeet",L"300",f.c_str()));
  assert(WritePrivateProfileStringW(liberty_settings93::Section,L"HeadlightReachFeet",L"75",f.c_str()));
  auto migrated125=Read(f.c_str());assert(migrated125.present&&migrated125.migrated&&!migrated125.invalid&&!migrated125.unavailable&&migrated125.state==old125);
  assert(Save(f,migrated125.state)&&Read(f.c_str()).state==old125);
 }
 {auto custom125=high;ChangeReach(custom125,true,100);ChangeReach(custom125,false,100);assert(Save(f,custom125));
  assert(WritePrivateProfileStringW(liberty_settings93::Section,L"BasePreset",L"VeryHigh",f.c_str()));auto migrated125=Read(f.c_str());
  assert(migrated125.migrated&&!migrated125.invalid&&migrated125.state==custom125&&migrated125.state.preset==Preset::Custom);
 }
 assert(Save(f,high));
 auto custom=high;Change(custom,Feature::WorldLighting,false);assert(ChangeReach(custom,true,80)&&ChangeReach(custom,false,30));assert(Save(f,custom)&&Read(f.c_str()).state==custom);
 custom.enabled=false;assert(Save(f,custom));assert(Read(f.c_str()).state==custom);assert(WritePrivateProfileStringW(liberty_settings93::Section,L"Preset",L"Unknown",f.c_str()));r=Read(f.c_str());assert(r.invalid&&!r.state.enabled&&Implementation(r.state)==Engine::None);
 assert(WritePrivateProfileStringW(liberty_settings93::Section,L"SchemaVersion",L"999",f.c_str()));assert(!Read(f.c_str()).state.enabled&&Read(f.c_str()).invalid);
 assert(Save(f,high));assert(WritePrivateProfileStringW(liberty_settings93::Section,L"Preset",L"Off",f.c_str()));assert(!Read(f.c_str()).state.enabled);
 for(auto version:{L"2",L"3"})for(auto unsupported:{L"Max",L"Ultra",L"Extreme",L"Unknown"}){
  assert(Save(f,high));assert(WritePrivateProfileStringW(liberty_settings93::Section,L"SchemaVersion",version,f.c_str()));assert(WritePrivateProfileStringW(liberty_settings93::Section,L"Preset",unsupported,f.c_str()));r=Read(f.c_str());assert(r.unavailable&&r.invalid&&!r.state.enabled&&Implementation(r.state)==Engine::None);
 }
 // Legacy93 reader is preserved and is the migration authority, including
 // edited Ultra, old vehicle-only High, and Extreme's actual400ft reach.
 assert(WritePrivateProfileStringW(liberty_settings93::Section,nullptr,nullptr,f.c_str()));
 using Old=liberty_settings93::Preset;
 for(auto old:{Old::Lowest,Old::Medium,Old::High,Old::Ultra,Old::Extreme}){
  auto legacy=liberty_settings93::Defaults(old);assert(liberty_settings93::SavePublic(f,legacy));r=Read(f.c_str());assert(r.present&&r.migrated&&!r.ownBackend&&!r.invalid&&r.state.features==legacy.features&&r.state.lampReachSteps==legacy.lampReachSteps&&r.state.headlightReachSteps==legacy.headlightReachSteps);
  assert(r.state.preset==(old==Old::Lowest?Preset::Low:old==Old::Medium?Preset::Medium:old==Old::Ultra?Preset::High:Preset::Custom));
 }
 auto edited=liberty_settings93::Defaults(Old::Ultra);liberty_settings93::Change(edited,liberty_settings93::Feature::TailLights,false);liberty_settings93::ChangeReach(edited,false,99);assert(liberty_settings93::SavePublic(f,edited));r=Read(f.c_str());assert(r.state.preset==Preset::Custom&&!r.state.features[1]&&r.state.headlightReachSteps==99);assert(Save(f,r.state)&&Read(f.c_str()).state==r.state);
 assert(Save(f,high));assert(WritePrivateProfileStringW(liberty_settings93::Section,L"SchemaVersion",L"2",f.c_str()));r=Read(f.c_str());assert(r.migrated&&!r.ownBackend&&r.state==high);
 auto bad=high;bad.preset=Preset(255);assert(!Save(f,bad));bad=high;bad.lampReachSteps=101;assert(!Save(f,bad));assert(!ChangeReach(bad,false,101));
 // A hardlinked profile is replaced atomically, never mutated in the other install.
 auto link=f;link+=L".link";assert(CreateHardLinkW(link.c_str(),f.c_str(),nullptr));assert(Save(f,low));assert(Read(link.c_str()).state==high&&Read(f.c_str()).state==low);std::filesystem::remove(link);std::filesystem::remove(f);
 std::cout<<"PASS117 shared presets, realINI migration/atomic save/Off/single-manager/support preservation/backend authority\n";
}
