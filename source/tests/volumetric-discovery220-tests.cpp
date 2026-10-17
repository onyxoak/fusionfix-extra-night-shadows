#define NOMINMAX
#include "standalone-pe.hpp"
#include "FusionDiscovery218.hpp"
#include "CopyVersions213.hpp"
#include "VolumetricCompat199.hpp"
#include <cassert>
#include <iostream>
#include <map>
using namespace onyxoak;
struct ChainFixture220 {
 uint32_t game,volume,fusion;
 fusion_discovery218::Result layout;
 std::vector<uint8_t> native,volumeCode,fusionCode;
 std::map<uint32_t,std::vector<uint8_t>> memory;
 static constexpr uint32_t volumeTrampoline=0x210000,fusionTrampoline=0x220000;
 static void Put(std::vector<uint8_t>& b,size_t offset,uint32_t value){std::memcpy(b.data()+offset,&value,4);}
 static std::vector<uint8_t> Jump(uint32_t at,uint32_t to){std::vector<uint8_t> b{0xe9,0,0,0,0};Put(b,1,to-at-5);return b;}
 void Layer(uint32_t trampoline,uint32_t callback,uint32_t allocation,bool first,uint32_t preceding){
  const uint32_t size=first?19u:15u;std::vector<uint8_t> bytes(size);
  if(first)std::copy_n(native.begin(),9,bytes.begin());
  else {auto j=Jump(trampoline,preceding);std::copy(j.begin(),j.end(),bytes.begin());}
  auto j=Jump(trampoline+size-10,game+native_copy70::CopyRva+(first?9:5));std::copy(j.begin(),j.end(),bytes.begin()+size-10);
  j=Jump(trampoline+size-5,callback);std::copy(j.begin(),j.end(),bytes.begin()+size-5);memory[trampoline]=bytes;
  std::vector<uint8_t> storage(8);Put(storage,0,trampoline);Put(storage,4,size);memory[allocation]=storage;
 }
 ChainFixture220(const pe::Image& g,const pe::Image& v,const pe::Image& f,bool volumeOuter,bool volumeOnly=false):
  game(uint32_t(g.mappedBase)),volume(uint32_t(v.mappedBase)),fusion(uint32_t(f.mappedBase)),layout(fusion_discovery218::Discover(f)){
  native.assign(g.bytes.begin()+native_copy70::CopyRva,g.bytes.begin()+native_copy70::CopyRva+native_copy70::CopySize);
  volumeCode.assign(v.bytes.begin()+volumetric199::Callback,v.bytes.begin()+volumetric199::Callback+volumetric199::CallbackSize);
  fusionCode.assign(f.bytes.begin()+layout.copyCallback,f.bytes.begin()+layout.copyCallback+layout.copySize);
  assert(fusionCode[layout.copySlot]==0xa1&&native_copy70::Word(fusionCode.data()+layout.copySlot+1)==fusion+layout.allocation);
  const uint8_t invoke[]{0xff,0x75,0x08,0x8b,0xd6,0x8b,0xcf,0xff,0xd0,0x8b,0xf8};
  assert(!std::memcmp(fusionCode.data()+layout.copyInvoke,invoke,sizeof(invoke)));
  assert(volumeCode[0x6f]==0xa1&&native_copy70::Word(volumeCode.data()+0x70)==volume+volumetric199::Allocation);
  const uint8_t volumeInvoke[]{0xff,0x75,0x08,0x8b,0xd6,0x8b,0xcf,0xff,0xd0,0x8b,0xf0};
  assert(!std::memcmp(volumeCode.data()+0x81,volumeInvoke,sizeof(volumeInvoke)));
  memory[volume+volumetric199::Callback]=volumeCode;memory[fusion+layout.copyCallback]=fusionCode;
  Layer(volumeTrampoline,volume+volumetric199::Callback,volume+volumetric199::Allocation,!volumeOuter||volumeOnly,fusionTrampoline+14);
  if(!volumeOnly)Layer(fusionTrampoline,fusion+layout.copyCallback,fusion+layout.allocation,volumeOuter,volumeTrampoline+14);
  auto live=native;auto jump=Jump(game+native_copy70::CopyRva,volumeOnly?volumeTrampoline+14:(volumeOuter?volumeTrampoline+10:fusionTrampoline+10));
  std::copy(jump.begin(),jump.end(),live.begin());std::fill(live.begin()+5,live.begin()+9,uint8_t(0x90));memory[game+native_copy70::CopyRva]=live;
 }
 std::vector<uint8_t> Read(uint32_t at,size_t size)const{
  auto it=memory.upper_bound(at);if(it==memory.begin())throw std::runtime_error("unmapped test read");--it;
  if(uint64_t(at)-it->first+size>it->second.size())throw std::runtime_error("out-of-range test read");
  return {it->second.begin()+at-it->first,it->second.begin()+at-it->first+size};
 }
 volumetric199::Chain Check(bool volumeOnly=false)const{
  volumetric199::Node v{volume+volumetric199::Callback,volume+volumetric199::Allocation,volumeCode},f{fusion+layout.copyCallback,fusion+layout.allocation,fusionCode};
  return volumetric199::VerifyChain(game,native,v,volumeOnly?nullptr:&f,[&](uint32_t at,size_t size){return Read(at,size);});
 }
 bool Refuses()const {try{Check();return false;}catch(const std::exception&){return true;}}
};
int main(int argc,char**argv){
 assert(argc==4);
 const auto gameRaw=pe::Read(argv[1]),volumeRaw=pe::Read(argv[2]),fusionRaw=pe::Read(argv[3]);
 assert(pe::Hash(volumeRaw)==volumetric199::Hash);
 assert(copy_versions213::Find("5.1.2")==nullptr); // New release must use discovered addresses.
 size_t orders=0,mutations=0;
 for(uint32_t gameBase:{0x440000u,0x160000u})for(uint32_t fusionBase:{0x10000000u,0x73000000u}){
  const uint32_t volumeBase=fusionBase==0x10000000?0x57000000u:0x33000000u;
  const pe::Image game(gameRaw,gameBase),volume(volumeRaw,volumeBase),fusion(fusionRaw,fusionBase);
  const auto detected=fusion_discovery218::Discover(fusion);
  assert(detected.copyCallback==0x60500&&detected.allocation!=native_copy70::AllocationRva);
  for(bool outer:{false,true}){
   ChainFixture220 f(game,volume,fusion,outer);const auto proof=f.Check();
   assert(proof.volumetric&&proof.fusionFix&&proof.outer==(outer?volumeBase+volumetric199::Callback:fusionBase+detected.copyCallback));++orders;
   for(const auto& [address,bytes]:f.memory)for(size_t i=0;i<bytes.size();++i){
    auto changed=f;changed.memory[address][i]^=1;assert(changed.Refuses());++mutations;
   }
   auto missing=f;missing.memory.erase(ChainFixture220::volumeTrampoline);assert(missing.Refuses());
   auto cyclic=f;const auto loop=ChainFixture220::Jump(ChainFixture220::volumeTrampoline,ChainFixture220::volumeTrampoline);
   std::copy(loop.begin(),loop.end(),cyclic.memory[ChainFixture220::volumeTrampoline].begin());assert(cyclic.Refuses());
  }
  ChainFixture220 standalone(game,volume,fusion,false,true);assert(standalone.Check(true).volumetric);assert(standalone.Refuses());
 }
 std::cout<<"PASS actual official FusionFix5.1.2 discovered copier + official VolumetricLights2.00: "<<orders<<" two-layer orders/relocations; "<<mutations<<" byte mutations refused; missing/cyclic owners refused; volume-only native path preserved\n";
}
