#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <cassert>
#include <array>
#include <vector>
#include <iostream>
#include "LocalShaderBinding244.hpp"
#include "standalone-pe.hpp"
int main(int argc,char** argv){
 using namespace local_shader244;
 assert(argc>=2);
 for(int n=1;n<argc;++n){
  const onyxoak::pe::Image image(onyxoak::pe::Read(argv[n]),0x60000000);
  std::vector<Region> regions;for(const auto& s:image.sections)regions.push_back({s.VirtualAddress,s.Misc.VirtualSize,s.Characteristics});
  const auto binding=Discover(image.bytes,0x60000000,regions);assert(binding);
  assert(binding.epoch!=binding.group);
  auto changed=image.bytes;changed[binding.name]='x';assert(!Discover(changed,0x60000000,regions));
  changed=image.bytes;changed[binding.registration+9]=0x90;assert(!Discover(changed,0x60000000,regions));
  auto wrong=regions;for(auto& s:wrong)s.flags&=~0x80000000u;assert(!Discover(image.bytes,0x60000000,wrong));
  // A second structurally valid registration must not silently pick one.
  changed=image.bytes;
  std::copy_n(changed.begin()+binding.registration,21,changed.begin()+binding.registration+64);
  assert(!Discover(changed,0x60000000,regions));
  std::cout<<"PASS244 registered local technique: "<<argv[n]<<" registration="<<std::hex<<binding.registration<<" epoch="<<binding.epoch<<" group="<<binding.group<<std::dec<<"\n";
 }
}
