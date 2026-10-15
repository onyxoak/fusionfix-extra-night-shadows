#pragma once
#include <windows.h>
#include <string>
#include <filesystem>
#include <cwctype>
#include <cerrno>
#include "PublicLightingSettings93.hpp"
namespace liberty_settings93 {
inline constexpr const wchar_t* Section=L"LIBERTY_SHADOWS";
struct ReadResult {State state=Defaults(Preset::Ultra);bool present{};unsigned invalid{};};
inline std::wstring Trim(std::wstring text){
 const auto a=text.find_first_not_of(L" \t\r\n");if(a==text.npos)return {};
 return text.substr(a,text.find_last_not_of(L" \t\r\n")-a+1);
}
inline std::wstring ReadText(const wchar_t* path,const wchar_t* key){
 wchar_t value[256]{};const auto length=GetPrivateProfileStringW(Section,key,L"",value,256,path);
 if(length>=255)return L"INVALID_VALUE_TOO_LONG";return Trim(value);
}
inline Preset ParsePreset(const std::wstring& text,bool& ok){
 for(const auto& p:Presets)if(!_wcsicmp(text.c_str(),p.name.data())){ok=true;return p.value;}
 if(!_wcsicmp(text.c_str(),L"Custom")){ok=true;return Preset::Custom;}
 ok=false;return Preset::Ultra;
}
inline unsigned Number(const std::wstring& text,unsigned fallback,unsigned maximum,unsigned& invalid){
 if(text.empty()||!_wcsicmp(text.c_str(),L"Auto"))return fallback;
 if(text[0]==L'-'){++invalid;return fallback;}
 wchar_t* end{};errno=0;const auto value=wcstoul(text.c_str(),&end,10);
 if(errno||end==text.c_str()||*end||value>maximum){++invalid;return fallback;}
 return unsigned(value);
}
inline ReadResult ReadPublic(const wchar_t* path){
 ReadResult result{};auto text=ReadText(path,L"Preset");if(text.empty())return result;
 result.present=true;bool ok{};const auto selected=ParsePreset(text,ok);if(!ok)++result.invalid;
 auto base=selected;
 if(selected==Preset::Custom){base=ParsePreset(ReadText(path,L"BasePreset"),ok);if(!ok||base==Preset::Custom){++result.invalid;base=Preset::Ultra;}}
 result.state=Defaults(base);result.state.preset=selected;
 for(const auto& f:Features){const auto enabled=Number(ReadText(path,f.key.data()),unsigned(Enabled(result.state,f.value)),1,result.invalid)!=0;
  if(enabled!=Enabled(result.state,f.value))Change(result.state,f.value,enabled);
 }
 const auto street=Number(ReadText(path,L"StreetlightReachFeet"),result.state.lampReachSteps*5,500,result.invalid);
 const auto headlights=Number(ReadText(path,L"HeadlightReachFeet"),result.state.headlightReachSteps*5,500,result.invalid);
 if(street%5||headlights%5){++result.invalid;}else {
  if(street/5!=result.state.lampReachSteps)ChangeReach(result.state,true,street/5);
  if(headlights/5!=result.state.headlightReachSteps)ChangeReach(result.state,false,headlights/5);
 }
 return result;
}
inline const wchar_t* Name(Preset value) noexcept {if(value==Preset::Custom)return L"Custom";for(const auto& p:Presets)if(p.value==value)return p.name.data();return L"Ultra";}
inline bool SavePublic(const std::filesystem::path& path,const State& state) noexcept {
 try {
  if(state.base>Preset::Extreme||state.preset>Preset::Custom||state.lampReachSteps>100||state.headlightReachSteps>100)return false;
  // Save an owned new file, preserving all legacy/support sections. Atomic
  // replacement breaks any file hardlink instead of mutating another install.
  auto temp=path;temp+=L".pending93-"+std::to_wstring(GetCurrentProcessId())+L"-"+std::to_wstring(GetTickCount64());
  if(std::filesystem::exists(temp))return false;
  const bool existed=std::filesystem::exists(path);
  if(existed)std::filesystem::copy_file(path,temp);
  auto write=[&](const wchar_t* key,const wchar_t* value){return WritePrivateProfileStringW(Section,key,value,temp.c_str())!=0;};
  bool good=write(L"Preset",Name(state.preset))&&write(L"BasePreset",Name(state.base));
  for(const auto& f:Features)good=write(f.key.data(),Enabled(state,f.value)?L"1":L"0")&&good;
  good=write(L"StreetlightReachFeet",std::to_wstring(state.lampReachSteps*5).c_str())&&good;
  good=write(L"HeadlightReachFeet",std::to_wstring(state.headlightReachSteps*5).c_str())&&good;
  // The all-null form flushes the profile cache; its return value is not
  // evidence that each previous write failed. Verify the saved state below.
  WritePrivateProfileStringW(nullptr,nullptr,nullptr,temp.c_str());
  if(good){const auto actual=ReadPublic(temp.c_str());good=actual.present&&actual.invalid==0&&actual.state==state;}
  if(good)good=existed?ReplaceFileW(path.c_str(),temp.c_str(),nullptr,0,nullptr,nullptr)!=0:MoveFileExW(temp.c_str(),path.c_str(),MOVEFILE_WRITE_THROUGH)!=0;
  if(!good&&std::filesystem::exists(temp))std::filesystem::remove(temp);
  return good;
 }catch(...){return false;}
}
}
