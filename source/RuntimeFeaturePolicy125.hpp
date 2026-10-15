#pragma once
#include "SettingsRuntime117.hpp"
namespace preset117 {
struct EmergencyPolicy125 {bool proof{};uint32_t policy{};bool vacancy{};};
inline bool RuntimeTrafficPromotion125(const wchar_t* file,bool requested) noexcept {
 // Owned public settings are the installed startup authority. Pending UI/INI
 // feature edits still require restart; obsolete private keys cannot retire it.
 if(startup.ownBackend)return requested&&Requests(startup.state).traffic;
 return requested&&GetPrivateProfileIntW(L"LIGHTING20",L"TrafficSignalShadows",
  GetPrivateProfileIntW(L"LAB",L"TrafficPromote",0,file),file)==1;
}
inline EmergencyPolicy125 RuntimeEmergencyPolicy125(const wchar_t* file,bool requested) noexcept {
 if(startup.ownBackend){const bool enabled=requested&&Requests(startup.state).emergency;return {enabled,enabled?1u:0u,enabled};}
 // Legacy explicit proof/policy/vacancy toggles keep their historical meaning.
 return {GetPrivateProfileIntW(L"LAB",L"PoliceProof",0,file)==1,
  GetPrivateProfileIntW(L"LAB",L"PolicePolicy",0,file)==1?1u:0u,
  GetPrivateProfileIntW(L"LAB",L"PoliceVacancy",0,file)==1};
}
}
