#include "VolumetricCompat199.hpp"
#include "NativeClassDomains105.hpp"
#include "standalone-pe.hpp"
#include <cassert>
#include <iostream>
#include <map>
using namespace volumetric199;
struct Fixture {
 static constexpr uint32_t game=0x440000,vl=0x57000000,ff=0x59000000;
 std::map<uint32_t,std::vector<uint8_t>> memory;
 std::vector<uint8_t> original,volumeCallback,fusionCallback;
 uint32_t vtr=0x410000,ftr=0x420000;
 static void Put(std::vector<uint8_t>& b,size_t at,uint32_t word){memcpy(b.data()+at,&word,4);}
 static std::vector<uint8_t> Jump(uint32_t at,uint32_t to){std::vector<uint8_t>b(5);b[0]=0xe9;Put(b,1,to-at-5);return b;}
 std::vector<uint8_t> Read(uint32_t at,size_t size) const {
  auto it=memory.upper_bound(at);if(it==memory.begin())throw std::runtime_error("missing");--it;
  if(uint64_t(at)-it->first+size>it->second.size())throw std::runtime_error("range");
  return {it->second.begin()+at-it->first,it->second.begin()+at-it->first+size};
 }
 void Layer(uint32_t trampoline,uint32_t callback,uint32_t alloc,bool first,uint32_t preceding){
  auto size=first?19u:15u;std::vector<uint8_t>b(size);
  if(first)std::copy_n(original.begin(),9,b.begin());else {auto j=Jump(trampoline,preceding);std::copy(j.begin(),j.end(),b.begin());}
  auto ret=Jump(trampoline+size-10,game+native_copy70::CopyRva+(first?9:5));std::copy(ret.begin(),ret.end(),b.begin()+size-10);
  auto relay=Jump(trampoline+size-5,callback);std::copy(relay.begin(),relay.end(),b.begin()+size-5);memory[trampoline]=b;
  std::vector<uint8_t>a(8);Put(a,0,trampoline);Put(a,4,size);memory[alloc]=a;
 }
 Fixture(const onyxoak::pe::Image& gameImage,const onyxoak::pe::Image& volumeImage,const onyxoak::pe::Image& fusionImage,bool volumeOuter,bool volumeOnly=false){
  original.assign(gameImage.bytes.begin()+native_copy70::CopyRva,gameImage.bytes.begin()+native_copy70::CopyRva+native_copy70::CopySize);
  volumeCallback.assign(volumeImage.bytes.begin()+Callback,volumeImage.bytes.begin()+Callback+CallbackSize);
  fusionCallback.assign(fusionImage.bytes.begin()+native_copy70::CallbackRva,fusionImage.bytes.begin()+native_copy70::CallbackRva+native_copy70::CallbackSize);
  memory[vl+Callback]=volumeCallback;memory[ff+native_copy70::CallbackRva]=fusionCallback;
  Layer(vtr,vl+Callback,vl+Allocation,!volumeOuter||volumeOnly,ftr+14);
  if(!volumeOnly)Layer(ftr,ff+native_copy70::CallbackRva,ff+native_copy70::AllocationRva,volumeOuter,vtr+14);
  auto live=original;auto jump=Jump(game+native_copy70::CopyRva,volumeOnly?vtr+14:(volumeOuter?vtr+10:ftr+10));
  std::copy(jump.begin(),jump.end(),live.begin());std::fill(live.begin()+5,live.begin()+9,uint8_t(0x90));memory[game+native_copy70::CopyRva]=live;
 }
 Chain Check(bool volumeOnly=false){Node volume{vl+Callback,vl+Allocation,volumeCallback},fusion{ff+native_copy70::CallbackRva,ff+native_copy70::AllocationRva,fusionCallback};
  return VerifyChain(game,original,volume,volumeOnly?nullptr:&fusion,[&](uint32_t a,size_t s){return Read(a,s);});}
 bool Refuses(){try{Check();return false;}catch(const std::runtime_error&){return true;}}
};
int main(int argc,char**argv){
 assert(argc==4);using onyxoak::pe::Image;using onyxoak::pe::Read;
 const auto vr=Read(argv[2]);assert(onyxoak::pe::Hash(vr)==Hash);
 const Image game(Read(argv[1]),Fixture::game),volume(vr,Fixture::vl),fusion(Read(argv[3]),Fixture::ff);
 size_t mutations=0;
 for(bool outer:{false,true}){
  Fixture f(game,volume,fusion,outer);auto proof=f.Check();assert(proof.volumetric&&proof.fusionFix);
  assert(proof.outer==(outer?Fixture::vl+Callback:Fixture::ff+native_copy70::CallbackRva));
  for(const auto& [address,bytes]:f.memory)for(unsigned i=0;i<bytes.size();++i){
   Fixture changed=f;changed.memory[address][i]^=1;assert(changed.Refuses());++mutations;
  }
  Fixture missing=f;missing.memory.erase(missing.vtr);assert(missing.Refuses());
 }
 {Fixture only(game,volume,fusion,false,true);assert(only.Check(true).volumetric);assert(only.Refuses());}
 auto values=game.bytes;
 std::map<uint32_t,uint32_t> settings{{HeadSize,std::bit_cast<uint32_t>(.37f)},{TailSize,std::bit_cast<uint32_t>(.19f)},
  {HeadIntensity,std::bit_cast<uint32_t>(.15f)},{TailIntensity,std::bit_cast<uint32_t>(.11f)}};
 auto setting=[&](uint32_t a,size_t n){assert(n==4);std::vector<uint8_t>b(4);Fixture::Put(b,0,settings.at(a-Fixture::vl));return b;};
 for(unsigned i=0;i<SizeOperands.size();++i)Fixture::Put(values,SizeOperands[i],settings[i<2?HeadSize:TailSize]);
 for(unsigned i=0;i<2;++i)Fixture::Put(values,IntensityOperands[i],Fixture::vl+(i?TailIntensity:HeadIntensity));
 {auto good=values;NormalizeOperands(good,game.bytes,Fixture::vl,setting);assert(good==game.bytes);}
 for(auto at:IntensityOperands)for(unsigned i=0;i<4;++i){auto bad=values;bad[at+i]^=1;bool rejected=false;
  try{NormalizeOperands(bad,game.bytes,Fixture::vl,setting);}catch(...){rejected=true;}assert(rejected);}
 for(auto at:SizeOperands){auto bad=values;bad[at]^=1;bool rejected=false;try{NormalizeOperands(bad,game.bytes,Fixture::vl,setting);}catch(...){rejected=true;}assert(rejected);}
 {auto bad=values;bad[0x63fe11]^=1;NormalizeOperands(bad,game.bytes,Fixture::vl,setting);assert(bad[0x63fe11]!=game.bytes[0x63fe11]);}
 using namespace native_projection66;
 for(uint32_t type:{0u,2u})for(uint32_t partition:{0u,0x100u,0x200u,0x300u}){
  Packet packet{};packet[17]=type;packet[18]=partition;packet[20]=0xdead;packet[21]=std::bit_cast<uint32_t>(30.f);packet[16]=std::bit_cast<uint32_t>(1.f);
  native_class_domains105::Registry<8> registry;registry.volumetricMetadata199=true;
  assert(registry.Init({0x10000000,0x11000000,0x12000000,0x13000000}));
  const auto key=registry.Observe(packet,1,[](uint32_t){return false;});assert(key);
  auto bank=packet;bank[24]=key;bank[18]|=12;bank[28]=std::bit_cast<uint32_t>(4.f);bank[29]=std::bit_cast<uint32_t>(.3f);
  assert(registry.Validate(key,bank));auto raw=bank;raw[24]=0;raw[18]&=~4u;
  assert(Values(packet,false,true)==Values(raw,false,true));assert(Values(packet)!=Values(raw));
  auto unmarked=packet;unmarked[20]=0;auto changed=unmarked;changed[29]=1;assert(Values(unmarked,false,true)!=Values(changed,false,true));
  // Every geometric/identity field, every unrelated flag, and the full current
  // bank packet witness remain exact. Volume changes never grant lifetime.
  for(unsigned field:Fields)if(field!=18&&field!=29){auto bad=bank;bad[field]^=1;assert(!registry.Validate(key,bad));}
  for(unsigned bit=0;bit<32;++bit)if(bit!=3){auto bad=bank;bad[18]^=1u<<bit;assert(!registry.Validate(key,bad));}
  Sidecar<4> sidecar;sidecar.Begin(0x777000,1,1);sidecar.Own(0,bank);sidecar.Accept(0,false);
  assert(sidecar.Current(0,bank,0x777000,1,1,0x5000)==Decision::Valid);
  for(unsigned field:WitnessFields){auto bad=bank;bad[field]^=1;assert(sidecar.Current(0,bad,0x777000,1,1,0x5000)==Decision::Invalid);}
  assert(sidecar.Current(0,bank,0x777000,1,2,0x5000)==Decision::Invalid);
  AliasPass<4> aliases;aliases.Begin();assert(aliases.Add(0,key,bank,1.f,false,true));assert(aliases.Add(1,key,bank,1.f,false,true));assert(aliases.IsAlias(1));
 }
 // MisterMansion's public195 failure, with the exact reported relocation and
 // pointer. All producer bytes must pass after authenticated normalization;
 // a second unrelated producer change must remain visible to the full guard.
 {
  constexpr uint32_t reportedModule=0x728b0000,reportedPointer=0x7293ea8c;
  static_assert(reportedModule+HeadIntensity==reportedPointer);
  const Image reportedGame(Read(argv[1]),0x160000);
  assert(Word(reportedGame.bytes.data()+0x63f8cc)==0x00d4879c);
  auto reported=reportedGame.bytes;
  for(unsigned i=0;i<SizeOperands.size();++i)
   Fixture::Put(reported,SizeOperands[i],settings[i<2?HeadSize:TailSize]);
  Fixture::Put(reported,IntensityOperands[0],reportedPointer);
  Fixture::Put(reported,IntensityOperands[1],reportedModule+TailIntensity);
  const auto changed=reported;
  constexpr size_t producerStart=0x63f5e0,producerSize=0x63ff1f-producerStart;
  assert(memcmp(reported.data()+producerStart,reportedGame.bytes.data()+producerStart,producerSize));
  auto reportedSetting=[&](uint32_t address,size_t count){
   assert(count==4);std::vector<uint8_t> bytes(4);
   Fixture::Put(bytes,0,settings.at(address-reportedModule));return bytes;
  };
  NormalizeOperands(reported,reportedGame.bytes,reportedModule,reportedSetting);
  assert(reported==reportedGame.bytes);
  assert(!memcmp(reported.data()+producerStart,reportedGame.bytes.data()+producerStart,producerSize));
  auto foreign=changed;foreign[0x63fe11]^=1;
  NormalizeOperands(foreign,reportedGame.bytes,reportedModule,reportedSetting);
  assert(memcmp(foreign.data()+producerStart,reportedGame.bytes.data()+producerStart,producerSize));
  auto wrongOwner=changed;Fixture::Put(wrongOwner,IntensityOperands[0],reportedPointer+4);
  bool refused=false;try{NormalizeOperands(wrongOwner,reportedGame.bytes,reportedModule,reportedSetting);}catch(...){refused=true;}
  assert(refused);
  std::cout<<"MisterMansion207: exact 0063F8CC / 00D4879C -> 7293EA8C regression passed; full producer restored only for owned operands; unrelated changes retained and wrong owner refused\n";
 }
 std::cout<<"Volumetric199: both copy orders, native-only chain, "<<mutations<<" byte mutations rejected; exact producer ownership and full packet witnesses passed\n";
}
