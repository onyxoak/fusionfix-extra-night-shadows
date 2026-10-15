#define NOMINMAX
#include "PublicStartupDefaults124.hpp"
#include <cassert>
#include <iostream>
using namespace preset117;
int main(){
 wchar_t temporary124[32768]{};assert(GetTempPathW(32768,temporary124));
 const auto file124=std::filesystem::path(temporary124)/(L"ls-public124-"+std::to_wstring(GetCurrentProcessId())+L".ini");assert(!std::filesystem::exists(file124));
 SetLastError(124);auto result124=ReadStartup124(file124.c_str());assert(GetLastError()==124);
 assert(result124.present&&result124.ownBackend&&!result124.unavailable&&!result124.invalid&&result124.state==*Defaults(Preset::High));assert(!std::filesystem::exists(file124));
 assert(!Read(file124.c_str()).present);assert(!ReadTransitionDiagnostics124(file124.c_str()));
 // Public High only for absence. Existing legacy key/value authority remains
 // unchanged, even when no public section or schema is present.
 assert(WritePrivateProfileStringW(L"SHADOWS",L"Enabled",L"0",file124.c_str()));
 assert(WritePrivateProfileStringW(L"LIGHTING20",L"ModelLightShadows",L"0",file124.c_str()));
 assert(WritePrivateProfileStringW(L"SHADOWS",L"LamppostShadowReach",L"17",file124.c_str()));
 result124=ReadStartup124(file124.c_str());assert(!result124.present&&!result124.ownBackend&&!result124.unavailable&&result124.state==Read(file124.c_str()).state);
 assert(GetPrivateProfileIntW(L"SHADOWS",L"Enabled",1,file124.c_str())==0&&GetPrivateProfileIntW(L"LIGHTING20",L"ModelLightShadows",1,file124.c_str())==0&&GetPrivateProfileIntW(L"SHADOWS",L"LamppostShadowReach",40,file124.c_str())==17);
 using Old124=liberty_settings93::Preset;auto legacy124=liberty_settings93::Defaults(Old124::Medium);assert(liberty_settings93::SavePublic(file124,legacy124));
 auto expected124=Read(file124.c_str());result124=ReadStartup124(file124.c_str());assert(result124.present&&!result124.ownBackend&&result124.migrated&&result124.state==expected124.state);
 assert(Save(file124,*Defaults(Preset::High)));assert(ReadStartup124(file124.c_str()).state==*Defaults(Preset::High));
 auto off124=*Defaults(Preset::High);off124.enabled=false;assert(Save(file124,off124));result124=ReadStartup124(file124.c_str());assert(result124.ownBackend&&!result124.state.enabled&&!result124.unavailable);
 assert(WritePrivateProfileStringW(liberty_settings93::Section,L"SchemaVersion",L"999",file124.c_str()));result124=ReadStartup124(file124.c_str());assert(result124.unavailable&&result124.invalid&&!result124.state.enabled);
 assert(Save(file124,*Defaults(Preset::High)));assert(WritePrivateProfileStringW(liberty_settings93::Section,L"Preset",L"Invalid",file124.c_str()));result124=ReadStartup124(file124.c_str());assert(result124.unavailable&&result124.invalid&&!result124.state.enabled);
 // Existing unreadable/shared-lock profiles refuse, never synthesize High.
 HANDLE lock124=CreateFileW(file124.c_str(),GENERIC_READ,0,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);assert(lock124!=INVALID_HANDLE_VALUE);
 SetLastError(124);result124=ReadStartup124(file124.c_str());assert(GetLastError()==124&&result124.unavailable&&result124.invalid&&!result124.ownBackend&&!result124.state.enabled);assert(CloseHandle(lock124));
 for(const wchar_t* value124:{L"1",L"0",L"2"}){assert(WritePrivateProfileStringW(L"DIAGNOSTICS",L"ShadowTransitions",value124,file124.c_str()));assert(ReadTransitionDiagnostics124(file124.c_str())==(value124[0]==L'1'));}
 assert(WritePrivateProfileStringW(L"DIAGNOSTICS",L"ShadowTransitions",nullptr,file124.c_str()));assert(!ReadTransitionDiagnostics124(file124.c_str()));
 assert(std::filesystem::remove(file124));assert(std::filesystem::create_directory(file124));result124=ReadStartup124(file124.c_str());assert(result124.unavailable&&!result124.state.enabled&&!result124.ownBackend);assert(std::filesystem::remove(file124));
 const auto missingParent124=file124/L"missing.ini";result124=ReadStartup124(missingParent124.c_str());assert(result124.unavailable&&!result124.ownBackend&&!result124.state.enabled);
 for(const wchar_t* invalid124:std::array<const wchar_t*,3>{nullptr,L"",L"?:\\invalid.ini"}){result124=ReadStartup124(invalid124);assert(result124.unavailable&&!result124.ownBackend&&!result124.state.enabled);}
 std::cout<<"PASS124 production parser/public absence High/legacy and explicit Off/malformed/read-lock refusal/quiet transitions/LastError/no file writes\n";
}
