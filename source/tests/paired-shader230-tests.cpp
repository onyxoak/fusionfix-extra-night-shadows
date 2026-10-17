#include "BoxShaderIdentity216.hpp"
#include <cassert>
#include <string>
#include <iostream>
int main(){
 const std::string fade="2c2c8711e477d0ba2ceb9bdc493fc058f8f0bb3c0d497c8338d0e0e677631cfa";
 assert(box_shader216::SupportsSceneryFade(fade));
 assert(!box_shader216::SupportsFallback(fade));
 for(std::string hash:{std::string(BOX216_SHADER_HASH),std::string(BOX227_SHADER_HASH)}){
  assert(box_shader216::SupportsFallback(hash));assert(box_shader216::SupportsSceneryFade(hash));
  for(size_t i=0;i<hash.size();++i){auto changed=hash;changed[i]=(changed[i]=='a'?'b':'a');
   assert(!box_shader216::SupportsSceneryFade(changed));assert(!box_shader216::SupportsFallback(changed));}
  assert(!box_shader216::SupportsSceneryFade(hash.substr(1)));
  assert(!box_shader216::SupportsSceneryFade(hash+"0"));
 }
 for(const auto hash:{"","unrecognized shader","86DB809154F3B1249BB4BB6EA3ADAA388FF40AE5C91BB232CEDC215EB09E1BA4"}){
  assert(!box_shader216::SupportsSceneryFade(hash));assert(!box_shader216::SupportsFallback(hash));
 }
 std::cout<<"Exact paired fallback shader identity admits original fade and both reviewed fallback shaders; every one-character mutation, truncation and unrecognized shader remains refused.\n";
}
