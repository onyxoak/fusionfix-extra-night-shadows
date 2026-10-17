#define NOMINMAX
#include "standalone-pe.hpp"
#include "FusionDiscovery218.hpp"
#include "BackendVersions213.hpp"
#include "standalone-midstub.hpp"
#include "StartupConflict129.hpp"
#include "StartupGuidance210.hpp"
#include <cassert>
#include <iostream>
#include <map>
using namespace onyxoak;
int main(int argc,char** argv){
 assert(argc==3);
 for(unsigned n=1;n<3;++n)for(uint32_t base:{0x10000000u,0x73000000u}){
  auto raw=pe::Read(argv[n]);pe::Image image(raw,base);
  auto result=fusion_discovery218::Discover(image);
  auto read=[&](uint32_t at,size_t size){assert(at>=base&&uint64_t(at-base)+size<=image.bytes.size());return std::vector<uint8_t>(image.bytes.begin()+at-base,image.bytes.begin()+at-base+size);};
  fusion_discovery218::ValidateLive(image,result,read);
  std::cerr<<"fixture="<<n<<" process="<<std::hex<<result.process<<" player="<<result.player<<" frame="<<result.frame<<std::dec<<"\n";
  if(n==1){assert(result.process==0x18320&&result.player==0xa7be0&&result.flagsCallback==0xa6af0&&result.frame==0);}
  else assert(result.process==0x18b00&&result.player==0xa7c40);
  // Both actual timer arrangements: untouched counter in512 and installed
  // CE callback in511 (the DLL also contains an identical legacy callback).
  constexpr uint32_t game=0x400000,counter=0xd73604,site=game+0x4d0f82,relay=0x210000,stubAt=0x211000;
  std::vector<uint8_t> expected{0xff,0x05,0,0,0,0};uint32_t pointer=game+counter;std::memcpy(expected.data()+2,&pointer,4);
  std::map<uint32_t,std::vector<uint8_t>> memory;memory[site]=expected;
  auto word=[](auto& bytes,size_t at,uint32_t value){std::memcpy(bytes.data()+at,&value,4);};
  auto jump=[&](uint32_t at,uint32_t to){std::vector<uint8_t> b{0xe9,0,0,0,0};word(b,1,to-at-5);return b;};
  if(n==2){memory[site]=jump(site,relay);memory[site].push_back(0x90);memory[relay]=jump(relay,stubAt);
   memory[stubAt]={onyxoak::MidStubTemplate.begin(),onyxoak::MidStubTemplate.end()};
   word(memory[stubAt],2,stubAt+167);word(memory[stubAt],89,stubAt+163);word(memory[stubAt],163,base+0x83320);
   const auto storage=fusion_discovery218::Word(image.bytes,0x83324);memory[storage]=std::vector<uint8_t>(4);word(memory[storage],0,game+counter);
  }
  auto frameRead=[&](uint32_t at,size_t size){auto it=memory.find(at);if(it!=memory.end()){assert(it->second.size()>=size);return std::vector<uint8_t>(it->second.begin(),it->second.begin()+size);}return read(at,size);};
  fusion_discovery218::BindFrame(image,result,game,counter,expected,onyxoak::MidStubTemplate,frameRead);
  assert(n==1?result.frame==0:result.frame==0x83320);
  if(n==2){memory[stubAt][32]^=1;bool refused=false;try{fusion_discovery218::BindFrame(image,result,game,counter,expected,onyxoak::MidStubTemplate,frameRead);}catch(...){refused=true;}assert(refused);memory[stubAt][32]^=1;}
  backend_versions213::Layout backend{"no-version-used",result.extraCallback,result.admission,result.extra,result.head,result.vehicle,result.listCallback,result.vehicleCallback,result.flagsCallback,result.listIndex,result.listFunction};
  auto copy=[&](uintptr_t at,void* out,size_t size){if(at<base||uint64_t(at-base)+size>image.bytes.size())return false;std::memcpy(out,image.bytes.data()+at-base,size);return true;};
  assert(backend_versions213::CallbackBodies(backend,base,copy));
  // A new/edited release label and binary hash have no say in discovery.
  auto changed=image;for(const auto& section:changed.sections)if(!std::memcmp(section.Name,".rsrc",5)){changed.bytes[section.VirtualAddress+16]^=1;break;}
  auto same=fusion_discovery218::Discover(changed);assert(same.player==result.player&&same.extra==result.extra);
  // Required code modifications fail with the actual role, not a version error.
  changed=image;changed.bytes[result.player]^=1;bool refused=false;
  try{fusion_discovery218::Discover(changed);}catch(const std::exception& e){refused=std::string(e.what()).find("player_code_not_found")!=std::string::npos;}assert(refused);
  // Fresh current bytes must still agree with the discovered function body.
  changed=image;changed.bytes[result.copyCallback+32]^=1;refused=false;
  try{fusion_discovery218::ValidateLive(image,result,[&](uint32_t at,size_t size){return std::vector<uint8_t>(changed.bytes.begin()+at-base,changed.bytes.begin()+at-base+size);});}catch(const std::exception& e){refused=std::string(e.what())=="fusionfix_integration_required_code_modified";}assert(refused);
  // Outside-module pointer captures cannot be accepted as new addresses.
  changed=image;uint32_t bad=0x1234;std::memcpy(changed.bytes.data()+result.player+5,&bad,4);refused=false;
  try{fusion_discovery218::Discover(changed);}catch(...){refused=true;}assert(refused);
 }
 const std::string error="fusionfix_integration_player_code_not_found";
 assert(startup_conflict129::Fields("candidate218",error,{}).find("error="+error)!=std::string::npos);
 assert(startup_guidance210::Help(error,{}).find("not a FusionFix version allowlist")!=std::string::npos);
 std::cout<<"PASS actual official511/512 code-discovered at two bases, changed release identity accepted, missing/changed code and invalid references rejected\n";
}
