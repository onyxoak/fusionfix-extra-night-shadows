#pragma once
// Authenticated VolumetricLights v2.00 coexistence. No installed code is written.
#include "NativeCopyGuard70.hpp"
#include <array>
#include <vector>
#include <bit>
namespace volumetric199 {
inline constexpr char Hash[]="61fb3710301d880d94ed245d74d936d4180c61a7b428fd3d382e95b1613974b7";
inline constexpr uint32_t Callback=0x12100,CallbackSize=0xba,Allocation=0x8eaf0;
inline constexpr uint32_t HeadSize=0x8ea50,HeadIntensity=0x8ea8c,TailSize=0x8ead0,TailIntensity=0x8ea64;
inline constexpr std::array<uint32_t,5> SizeOperands{0x63f985,0x63fa3f,0x642c47,0x642de3,0x642f76};
inline constexpr std::array<uint32_t,2> IntensityOperands{0x63f8cc,0x642ba9};
using native_copy70::Need;using native_copy70::Word;
struct Node {uint32_t callback{},allocation{};std::span<const uint8_t> expected;};
struct Chain {bool fusionFix{},volumetric{};uint32_t outer{};};
// Each of the two authentic callbacks forwards ECX/EDX/one stack DWORD once,
// then applies its own event once. The first hook displaced nine native bytes;
// the second displaced only the first hook's E9. Authenticate every edge and
// unreachable SafetyHook continuation too, so there is no unknown owner/cycle.
template<class Reader> Chain VerifyChain(uint32_t game,std::span<const uint8_t> original,
 const Node& volume,const Node* fusion,Reader read){
 Need(original.size()==native_copy70::CopySize,"volumetric199_native_size");
 constexpr uint8_t displaced[]{0x8b,0x54,0x24,0x04,0xf3,0x0f,0x10,0x42,0x04};
 Need(!memcmp(original.data(),displaced,9),"volumetric199_native_contract");
 const auto live=read(game+native_copy70::CopyRva,original.size());
 Need(live.size()==original.size()&&live[0]==0xe9,"volumetric199_native_hook");
 for(unsigned i=5;i<9;++i)Need(live[i]==0x90,"volumetric199_native_padding");
 Need(!memcmp(live.data()+9,original.data()+9,original.size()-9),"volumetric199_native_suffix");
 auto branch=[&](uint32_t address){const auto bytes=read(address,5);
  Need(bytes.size()==5&&bytes[0]==0xe9,"volumetric199_chain_branch");return uint32_t(address+5+Word(bytes.data()+1));};
 std::array<const Node*,2> nodes{&volume,fusion};
 std::array<uint32_t,2> tramp{},sizes{},relays{};
 const unsigned count=fusion?2u:1u;
 for(unsigned i=0;i<count;++i){const auto& n=*nodes[i];
  const auto callback=read(n.callback,n.expected.size());
  Need(!n.expected.empty()&&callback.size()==n.expected.size()&&!memcmp(callback.data(),n.expected.data(),callback.size()),"volumetric199_callback_changed");
  const auto allocation=read(n.allocation,8);Need(allocation.size()==8,"volumetric199_allocation_read");
  tramp[i]=Word(allocation.data());sizes[i]=Word(allocation.data()+4);
  Need(tramp[i]>=0x10000&&(sizes[i]==19||sizes[i]==15),"volumetric199_allocation_layout");
  relays[i]=tramp[i]+sizes[i]-5;
  Need(branch(relays[i])==n.callback,"volumetric199_callback_owner");
 }
 const auto outerRelay=uint32_t(game+native_copy70::CopyRva+5+Word(live.data()+1));
 unsigned outer=outerRelay==relays[0]?0:1;
 Need(outer<count&&outerRelay==relays[outer],"volumetric199_outer_owner");
 unsigned inner=outer;
 if(count==2){inner=1-outer;Need(sizes[outer]==15,"volumetric199_outer_layout");
  Need(branch(tramp[outer])==relays[inner],"volumetric199_previous_owner");
  Need(branch(tramp[outer]+5)==game+native_copy70::CopyRva+5,"volumetric199_outer_continuation");
 }
 Need(sizes[inner]==19,"volumetric199_inner_layout");
 const auto body=read(tramp[inner],9);Need(body.size()==9&&!memcmp(body.data(),displaced,9),"volumetric199_inner_native");
 Need(branch(tramp[inner]+9)==game+native_copy70::CopyRva+9,"volumetric199_inner_continuation");
 return {fusion!=nullptr,true,nodes[outer]->callback};
}
template<class Reader> void NormalizeOperands(std::vector<uint8_t>& image,std::span<const uint8_t> original,uint32_t module,Reader read){
 auto value=[&](uint32_t rva){auto b=read(module+rva,4);Need(b.size()==4,"volumetric199_setting_read");
  const auto bits=Word(b.data());const auto f=std::bit_cast<float>(bits);
  Need(f>=0.f&&f<=1.f,"volumetric199_setting_range");return bits;};
 const uint32_t head=value(HeadSize),tail=value(TailSize);(void)value(HeadIntensity);(void)value(TailIntensity);
 // Compare complete instruction forms and every installed value before
 // normalizing only seven four-byte operands in the private guard snapshot.
 for(unsigned i=0;i<SizeOperands.size();++i){const auto at=SizeOperands[i];
  Need(uint64_t(at)+4<=image.size()&&image.size()==original.size(),"volumetric199_operand_bounds");
  Need(!memcmp(image.data()+at-4,original.data()+at-4,4)&&image[at-4]==0xc7&&image[at-3]==0x44&&image[at-2]==0x24,"volumetric199_size_instruction");
  Need(Word(image.data()+at)==(i<2?head:tail),"volumetric199_size_value");
 }
 for(unsigned i=0;i<IntensityOperands.size();++i){const auto at=IntensityOperands[i];
  Need(uint64_t(at)+4<=image.size()&&image.size()==original.size(),"volumetric199_operand_bounds");
  constexpr uint8_t mul[]{0xf3,0x0f,0x59,0x15};
  Need(!memcmp(image.data()+at-4,mul,4)&&!memcmp(original.data()+at-4,mul,4),"volumetric199_intensity_instruction");
  Need(Word(image.data()+at)==module+(i?TailIntensity:HeadIntensity),"volumetric199_intensity_owner");
 }
 for(auto at:SizeOperands)std::copy_n(original.data()+at,4,image.data()+at);
 for(auto at:IntensityOperands)std::copy_n(original.data()+at,4,image.data()+at);
}
}
