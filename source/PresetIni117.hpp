#pragma once
#include "PublicLightingIni93.hpp"
#include "PresetGeneration117.hpp"
namespace preset117 {
inline constexpr std::array<const wchar_t*,5> Keys{L"DualHeadlights",L"TaillightShadows",L"TrafficSignalShadows",L"EmergencyLightShadows",L"WorldLighting"};
struct ReadResult {State state{};bool present{},migrated{},unavailable{},ownBackend{};unsigned invalid{};};
inline bool RetiredVeryHigh125(const std::wstring& text) noexcept {return !_wcsicmp(text.c_str(),L"VeryHigh")||!_wcsicmp(text.c_str(),L"Very High");}
inline std::optional<Preset> Parse(const std::wstring& text){
 if(RetiredVeryHigh125(text))return Preset::High;
 for(auto p:{Preset::Low,Preset::Medium,Preset::High,Preset::Custom})if(!_wcsicmp(text.c_str(),Name(p)))return p;return {};
}
inline ReadResult Migrate93(const liberty_settings93::ReadResult& old) {
 ReadResult r{};r.present=old.present;r.invalid=old.invalid;r.migrated=old.present;if(!old.present)return r;
 using P=liberty_settings93::Preset;
 r.state.base=old.state.base==P::Lowest?Preset::Low:old.state.base==P::Medium?Preset::Medium:Preset::High;
 r.state.preset=old.state.preset==P::Lowest?Preset::Low:old.state.preset==P::Medium?Preset::Medium:old.state.preset==P::Ultra?Preset::High:Preset::Custom;
 r.state.features=old.state.features;r.state.lampReachSteps=old.state.lampReachSteps;r.state.headlightReachSteps=old.state.headlightReachSteps;
 return r;
}
inline ReadResult Read(const wchar_t* file) {
 using namespace liberty_settings93;ReadResult r{};
 const auto enabledText=ReadText(file,L"Enabled");const bool enabled=Number(enabledText,1,1,r.invalid)!=0;r.state.enabled=enabled;
 const auto version=ReadText(file,L"SchemaVersion");
 if(version.empty()){auto old=Migrate93(ReadPublic(file));old.state.enabled=enabled;old.present=old.present||!enabledText.empty();old.invalid+=r.invalid;return old;}
 r.present=true;
 if(version!=L"3"&&version!=L"2"){++r.invalid;r.unavailable=true;r.state.enabled=false;return r;}
 r.ownBackend=version==L"3";
 const auto text=ReadText(file,L"Preset");
 if(!_wcsicmp(text.c_str(),L"Off")){r.state.enabled=false;r.migrated=true;return r;}
 auto selected=Parse(text);
 if(!selected){++r.invalid;r.unavailable=true;r.state.enabled=false;return r;}
 bool retired125=RetiredVeryHigh125(text);auto base=*selected;if(*selected==Preset::Custom){const auto baseText=ReadText(file,L"BasePreset");retired125=retired125||RetiredVeryHigh125(baseText);const auto b=Parse(baseText);if(!b||!Implemented(*b)){++r.invalid;r.unavailable=true;r.state.enabled=false;return r;}base=*b;}
 r.state=*Defaults(base);r.state.preset=*selected;r.state.enabled=enabled;r.migrated=version==L"2"||retired125;
 // Named tiers are authoritative; only Custom reads the serialized snapshot.
 if(*selected!=Preset::Custom)return r;
 for(unsigned i=0;i<Keys.size();++i)r.state.features[i]=Number(ReadText(file,Keys[i]),r.state.features[i]?1u:0u,1,r.invalid)!=0;
 const auto street=Number(ReadText(file,L"StreetlightReachFeet"),r.state.lampReachSteps*5,500,r.invalid);
 const auto head=Number(ReadText(file,L"HeadlightReachFeet"),r.state.headlightReachSteps*5,500,r.invalid);
 if(street%5||head%5){++r.invalid;return r;}r.state.lampReachSteps=street/5;r.state.headlightReachSteps=head/5;
 return r;
}
inline bool Save(const std::filesystem::path& file,const State& s) noexcept {
 try {
  if(!Implemented(s.base)||(s.preset!=Preset::Custom&&!Implemented(s.preset))||s.lampReachSteps>100||s.headlightReachSteps>100)return false;
  // Reject noncanonical named states instead of saving a file with a different
  // interpretation. Controller edits always select Custom before serialization.
  if(s.preset!=Preset::Custom){auto canonical=*Defaults(s.preset);canonical.enabled=s.enabled;if(canonical!=s)return false;}
  auto tmp=file;tmp+=L".pending117-"+std::to_wstring(GetCurrentProcessId())+L"-"+std::to_wstring(GetTickCount64());if(std::filesystem::exists(tmp))return false;
  const bool exists=std::filesystem::exists(file);if(exists)std::filesystem::copy_file(file,tmp);
  auto write=[&](const wchar_t* key,const wchar_t* value){return WritePrivateProfileStringW(liberty_settings93::Section,key,value,tmp.c_str())!=0;};
  bool good=write(L"SchemaVersion",L"3")&&write(L"Enabled",s.enabled?L"1":L"0")&&write(L"Preset",Name(s.preset))&&write(L"BasePreset",Name(s.base));
  for(unsigned i=0;i<Keys.size();++i)good=write(Keys[i],s.features[i]?L"1":L"0")&&good;
  good=write(L"StreetlightReachFeet",std::to_wstring(s.lampReachSteps*5).c_str())&&write(L"HeadlightReachFeet",std::to_wstring(s.headlightReachSteps*5).c_str())&&good;
  WritePrivateProfileStringW(nullptr,nullptr,nullptr,tmp.c_str());if(good){const auto actual=Read(tmp.c_str());good=actual.present&&!actual.invalid&&actual.state==s;}
  if(good)good=exists?ReplaceFileW(file.c_str(),tmp.c_str(),nullptr,0,nullptr,nullptr)!=0:MoveFileExW(tmp.c_str(),file.c_str(),MOVEFILE_WRITE_THROUGH)!=0;
  if(!good&&std::filesystem::exists(tmp))std::filesystem::remove(tmp);return good;
 }catch(...){return false;}
}
}
