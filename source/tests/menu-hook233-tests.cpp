#define NOMINMAX
#include "standalone-pe.hpp"
#include "MenuChainGuard117.hpp"
#include "VolumetricCompat199.hpp"
#include "menu233-live-fixture.hpp"
#include <cassert>
#include <iostream>
#include <map>
using namespace onyxoak;
struct Reader233 {
 std::map<uint32_t,std::vector<uint8_t>> blocks;bool denied{},executable=true;
 bool Executable(uint32_t at,size_t n)const{return !denied&&executable&&blocks.contains(at)&&blocks.at(at).size()>=n;}
 bool Copy(uint32_t at,void* out,size_t n)const{if(!Executable(at,n))return false;std::memcpy(out,blocks.at(at).data(),n);return true;}
};
std::vector<uint8_t> Hex233(const char* s){std::vector<uint8_t> out;while(*s){char b[]{s[0],s[1],0};out.push_back(uint8_t(std::strtoul(b,nullptr,16)));s+=2;}return out;}
std::vector<uint8_t> Jump233(uint32_t from,uint32_t to){std::vector<uint8_t> b{0xe9,0,0,0,0};const auto d=to-from-5;std::memcpy(b.data()+1,&d,4);return b;}
bool Section233(const pe::Image& p,uint32_t at,size_t n){for(const auto& s:p.sections)if((s.Characteristics&IMAGE_SCN_MEM_EXECUTE)&&at>=s.VirtualAddress&&uint64_t(at)+n<=uint64_t(s.VirtualAddress)+s.Misc.VirtualSize)return true;return false;}
int main(int argc,char**argv){
 assert(argc==4);const auto rawGame=pe::Read(argv[1]);
 constexpr unsigned patches[]{6,5,8,6};unsigned verified{};
 // Replay all four ACTUAL captured live512 chains against the real relocated
 // game and owner image. No process writes, DLL load or game execution.
 {
  const pe::Image game(rawGame,LiveGame233),owner(pe::Read(argv[3]),LiveOwner233);
  for(unsigned i=0;i<4;++i){const auto& row=LiveMenus233[i];Reader233 r;
   r.blocks[LiveGame233+row.native]=Hex233(row.entry);r.blocks[row.relay]=Hex233(row.jump);r.blocks[row.target]=Hex233(row.body);
   const auto native=std::span(game.bytes).subspan(row.native,32);
   auto check=[&]{return menu_chain117::Discover233(r,LiveGame233+row.native,native,LiveOwner233,owner.bytes,[&](uint32_t at,size_t n){return Section233(owner,at,n);},patches[i]);};
   auto result=check();assert(result.owner==menu_chain117::Owner::FusionFix&&result.destination==row.target);++verified;
   for(unsigned n=0;n<32;++n){r.blocks[row.target][n]^=1;assert(check().owner==menu_chain117::Owner::Refused);r.blocks[row.target][n]^=1;}
   for(unsigned n=5;n<32;++n){r.blocks[LiveGame233+row.native][n]^=1;assert(check().owner==menu_chain117::Owner::Refused);r.blocks[LiveGame233+row.native][n]^=1;}
   r.blocks[row.relay]=Jump233(row.relay,row.relay);assert(check().owner==menu_chain117::Owner::Refused);
   r.blocks[row.relay]=Hex233(row.jump);r.denied=true;assert(check().owner==menu_chain117::Owner::Refused);
  }
 }
 // Different module bases, callback RVAs and binary metadata remain usable.
 for(unsigned fixture=2;fixture<4;++fixture)for(uint32_t base:{0x10000000u,0x73000000u}){
  auto raw=pe::Read(argv[fixture]);pe::Image owner(raw,base),game(rawGame,0x400000);
  const uint32_t callbacksOld[]{0xd1410,0xd6e90,0xd6f80,0xd6860};
  for(unsigned i=0;i<4;++i){const auto& row=LiveMenus233[i];const uint32_t at=0x400000+row.native,relay=0x210000+i*0x100;
   const uint32_t callback=fixture==2?callbacksOld[i]:row.target-LiveOwner233;
   Reader233 r;auto native=std::span(game.bytes).subspan(row.native,32);auto patched=std::vector<uint8_t>(native.begin(),native.end());
   const auto jump=Jump233(at,relay);std::copy(jump.begin(),jump.end(),patched.begin());std::fill(patched.begin()+5,patched.begin()+patches[i],uint8_t(0x90));
   r.blocks[at]=patched;r.blocks[relay]=Jump233(relay,base+callback);r.blocks[base+callback]={owner.bytes.begin()+callback,owner.bytes.begin()+callback+32};
   auto check=[&](const pe::Image& image){return menu_chain117::Discover233(r,at,native,base,image.bytes,[&](uint32_t offset,size_t n){return Section233(image,offset,n);},patches[i]);};
   assert(check(owner).owner==menu_chain117::Owner::FusionFix);++verified;
   auto changed=owner;for(const auto& section:changed.sections)if(!std::memcmp(section.Name,".rsrc",5)){changed.bytes[section.VirtualAddress+16]^=1;break;}
   assert(check(changed).owner==menu_chain117::Owner::FusionFix);
   r.blocks[at]={native.begin(),native.end()};assert(check(changed).owner==menu_chain117::Owner::Native);
   assert(menu_chain117::Discover233(r,at,native,0,{},[](uint32_t,size_t){return false;},patches[i]).owner==menu_chain117::Owner::Native);
  }
 }
 // Current producer operands and their data owner are sufficient; settings
 // addresses may move without changing the game's light argument contract.
 auto image=pe::Image(rawGame,0x400000).bytes;const auto original=image;
 auto put=[&](size_t at,uint32_t value){std::memcpy(image.data()+at,&value,4);};
 for(unsigned i=0;i<volumetric199::SizeOperands.size();++i)put(volumetric199::SizeOperands[i],std::bit_cast<uint32_t>(i<2?.3f:.4f));
 constexpr uint32_t module=0x54000000;std::map<uint32_t,uint32_t> values;
 for(unsigned i=0;i<volumetric199::IntensityOperands.size();++i){put(volumetric199::IntensityOperands[i],module+0x900+i*4);values[module+0x900+i*4]=std::bit_cast<uint32_t>(.5f);}
 const auto modified=image;
 auto read=[&](uint32_t at,size_t n){assert(n==4&&values.contains(at));std::vector<uint8_t> b(4);const auto value=values.at(at);std::memcpy(b.data(),&value,4);return b;};
 volumetric199::NormalizeCurrentOperands233(image,original,module,0x1000,read);assert(image==original);
 for(auto at:volumetric199::SizeOperands){image=modified;image[at-2]^=1;bool refused=false;try{volumetric199::NormalizeCurrentOperands233(image,original,module,0x1000,read);}catch(...){refused=true;}assert(refused);}
 image=modified;put(volumetric199::IntensityOperands[0],0x1234);bool refused=false;
 try{volumetric199::NormalizeCurrentOperands233(image,original,module,0x1000,read);}catch(...){refused=true;}assert(refused);
 std::cout<<"PASS actual live menu replay, "<<verified<<" hooks across relocated official fixtures, new metadata accepted, modified/foreign hooks refused, moved producer settings accepted\n";
}
