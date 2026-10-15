#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <cassert>
#include <cstdio>
#include "PublicLightingIni93.hpp"
using namespace liberty_settings93;
int main(){
 assert(State{}==Defaults(Preset::Ultra));
 const auto low=Requests(Defaults(Preset::Lowest)),medium=Requests(Defaults(Preset::Medium));
 assert(!low.dual&&!low.tail&&!low.traffic&&!low.emergency&&!low.model&&!low.corona&&!low.projection&&!low.keyedWorld&&!low.worldGeometry);
 assert(medium.traffic&&medium.emergency&&!medium.dual&&!medium.tail&&!medium.model&&!medium.corona&&!medium.projection);
 const auto high=Requests(Defaults(Preset::High)),ultra=Requests(Defaults(Preset::Ultra)),extreme=Requests(Defaults(Preset::Extreme));
 assert(high.dual&&high.tail&&high.traffic&&high.emergency&&high.worldGeometry&&!high.projection);
 assert(ultra.dual&&ultra.tail&&ultra.traffic&&ultra.emergency&&ultra.model&&ultra.corona&&ultra.projection&&ultra.keyedWorld&&ultra.worldGeometry);
 assert(extreme.lampReach>ultra.lampReach&&extreme.headlightReach>ultra.headlightReach);
 State state=Defaults(Preset::Medium);Change(state,Feature::TailLights,true);assert(state.preset==Preset::Custom&&state.base==Preset::Medium);
 assert(RestartRequired(Defaults(Preset::Medium),state));RestorePreset(state);assert(state==Defaults(Preset::Medium));
 ResetDefaults(state);assert(state==Defaults(Preset::Ultra));
 wchar_t exe[MAX_PATH]{};assert(GetModuleFileNameW(nullptr,exe,MAX_PATH));const auto path=std::filesystem::path(exe).concat(L".settings93-test.ini");
 assert(!std::filesystem::exists(path));assert(!ReadPublic(path.c_str()).present);
 assert(WritePrivateProfileStringW(L"SUPPORT",L"UnrelatedSetting",L"keep",path.c_str()));
 for(const auto& preset:Presets){state=Defaults(preset.value);assert(SavePublic(path,state));auto actual=ReadPublic(path.c_str());assert(actual.present&&actual.invalid==0&&actual.state==state);}
 state=Defaults(Preset::Ultra);Change(state,Feature::WorldLighting,false);assert(SavePublic(path,state));assert(ReadPublic(path.c_str()).state==state);assert(!Requests(state).projection&&!Requests(state).model&&!Requests(state).corona);
 assert(GetPrivateProfileIntW(L"SUPPORT",L"UnrelatedSetting",0,path.c_str())==0);wchar_t saved[16]{};assert(GetPrivateProfileStringW(L"SUPPORT",L"UnrelatedSetting",L"",saved,16,path.c_str()));assert(std::wstring(saved)==L"keep");
 assert(WritePrivateProfileStringW(Section,L"Preset",L"InvalidPreset",path.c_str()));assert(ReadPublic(path.c_str()).invalid>0);
 assert(SavePublic(path,Defaults(Preset::Ultra)));assert(WritePrivateProfileStringW(Section,L"WorldLighting",L"invalid",path.c_str()));auto invalid=ReadPublic(path.c_str());assert(invalid.invalid>0&&Enabled(invalid.state,Feature::WorldLighting));
 assert(WritePrivateProfileStringW(Section,L"StreetlightReachFeet",L"-10",path.c_str()));assert(ReadPublic(path.c_str()).state.lampReachSteps==40);
 assert(SavePublic(path,Defaults(Preset::Ultra)));auto invalidState=Defaults(Preset::Ultra);invalidState.lampReachSteps=101;assert(!SavePublic(path,invalidState));assert(ReadPublic(path.c_str()).state==Defaults(Preset::Ultra));
 assert(std::filesystem::remove(path));
 std::puts("Published preset scopes, grouped world producer control, Custom/reset/restart, atomic INI roundtrip, legacy preservation and invalid-input safety passed. No live menu proof.");
}
