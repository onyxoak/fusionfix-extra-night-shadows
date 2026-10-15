#pragma once
#include "FusionBackendBytes130.hpp"
namespace fusion_backend130 {
inline uint32_t Word(const uint8_t* p) noexcept {uint32_t n{};std::memcpy(&n,p,4);return n;}
inline bool SameRelocated(std::span<const uint8_t> live,std::span<const uint8_t> original,uint32_t ff,std::span<const unsigned> relocs) noexcept {
 if(live.size()!=original.size())return false;
 for(size_t n=0;n<live.size();){
  bool reloc=false;for(auto offset:relocs)if(n==offset)reloc=true;
  if(reloc){if(n+4>live.size()||Word(live.data()+n)!=Word(original.data()+n)-PreferredFF+ff)return false;n+=4;}
  else {if(live[n]!=original[n])return false;++n;}
 }return true;
}
template<class Read>
bool CallbackBodies(uint32_t ff,Read read) noexcept {
 std::array<uint8_t,ExtraBody.size()> extra{};std::array<uint8_t,ListBody.size()> list{};
 constexpr std::array<unsigned,3> extraRelocs{0x21,0x26,0x2c};constexpr std::array<unsigned,1> listRelocs{0x10};
 return read(ff+ExtraCallbackRva,extra.data(),extra.size())&&SameRelocated(extra,ExtraBody,ff,extraRelocs)&&
  read(ff+ListCallbackRva,list.data(),list.size())&&SameRelocated(list,ListBody,ff,listRelocs);
}
// Fresh exact game->relay->known FF mid stub plus relocated native continuation.
// Vehicle's SHORT JE expands to NEAR JE in SafetyHook; no pristine memcpy waiver.
template<class Read>
bool MidOwner(uint32_t game,uint32_t ff,uint32_t rva,uint32_t callback,unsigned overwritten,
 std::span<const uint8_t> original,std::span<const uint8_t> stubTemplate,bool vehicle,Read read) noexcept {
 if(stubTemplate.size()!=171||original.size()<overwritten||overwritten<5||overwritten>8)return false;
 std::array<uint8_t,8> installed{};std::array<uint8_t,5> relay{};
 if(!read(game+rva,installed.data(),overwritten)||installed[0]!=0xe9)return false;
 for(unsigned n=5;n<overwritten;++n)if(installed[n]!=0x90)return false;
 const auto first=uint32_t(game+rva+5+Word(installed.data()+1));
 if(!read(first,relay.data(),relay.size())||relay[0]!=0xe9)return false;
 const auto at=uint32_t(first+5+Word(relay.data()+1));std::array<uint8_t,171> stub{};
 if(!read(at,stub.data(),stub.size()))return false;
 for(unsigned n=0;n<171;++n){if((n>=2&&n<6)||(n>=89&&n<93)||n>=163)continue;if(stub[n]!=stubTemplate[n])return false;}
 if(Word(stub.data()+2)!=at+167||Word(stub.data()+89)!=at+163||Word(stub.data()+163)!=ff+callback)return false;
 const auto trampoline=Word(stub.data()+167);std::array<uint8_t,16> code{};
 if(!read(trampoline,code.data(),code.size()))return false;
 unsigned displaced=overwritten;
 if(vehicle){
  constexpr uint8_t native[]{0x74,0x1d,0x53,0x6a,0x00};
  if(rva!=0x6e4236||overwritten!=5||std::memcmp(original.data(),native,5)||code[0]!=0x0f||code[1]!=0x84||
   uint32_t(trampoline+6+Word(code.data()+2))!=game+0x6e4255||code[6]!=0x53||code[7]!=0x6a||code[8]!=0)return false;
  displaced=9;
 }else if(std::memcmp(code.data(),original.data(),overwritten))return false;
 return code[displaced]==0xe9&&uint32_t(trampoline+displaced+5+Word(code.data()+displaced+1))==game+rva+overwritten;
}
}
