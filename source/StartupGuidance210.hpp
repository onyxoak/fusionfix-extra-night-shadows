#pragma once
#include <algorithm>
#include <array>
#include <string>
#include <string_view>
namespace startup_guidance210 {
enum class Volume {NotSeen,Checking,Accepted};
struct Evidence {
 Volume volume=Volume::NotSeen;
 std::array<char,65> hash{};
 void SetHash(std::string_view value) noexcept {
  hash.fill(0);
  if(value.size()!=64)return;
  for(char c:value)if(!((c>='0'&&c<='9')||(c>='a'&&c<='f')||(c>='A'&&c<='F')))return;
  std::copy(value.begin(),value.end(),hash.begin());
 }
};
inline std::string Help(std::string_view error,const Evidence& evidence) {
 if(error.starts_with("fusionfix_integration_"))return " Liberty's Shadows could not connect to the required function or binding named above. Detection uses actual code, not a FusionFix version allowlist. Game lighting is left unchanged. Send this log, your FusionFix ASI and ASI list so this specific integration can be supported.";
 std::string result;
 if(evidence.volume==Volume::Checking){
  if(error=="volumetric199_unsupported_binary")result=" Unsupported VolumetricLights build.";
  else if(error=="volumetric199_setting_range")result=" Unsupported VolumetricLights settings.";
  else if(error=="volumetric199_unsupported_fusionfix")result=" This VolumetricLights adapter needs supported FusionFix.";
  else result=" VolumetricLights could not be safely combined with this setup.";
  result+=" Disable it temporarily: close GTA IV, move VolumetricLights.asi and VolumetricLights.ini outside the GTA IV game folder, then restart.";
 }else if(evidence.volume==Volume::Accepted){
  result=" VolumetricLights was accepted. Another required startup check failed. Send this log, FusionFix version and ASI list from game and plugins folders.";
 }else{
  result=" Startup compatibility check failed. Send this log, FusionFix version and ASI list from game and plugins folders.";
 }
 if(evidence.hash[0])result+=" volumetric_sha256="+std::string(evidence.hash.data());
 return result;
}
}
