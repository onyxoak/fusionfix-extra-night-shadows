#pragma once
#include "core/ShadowAdapterCE.hpp"
#include "StartupConflict129.hpp"
// Failure-only evidence. This never changes the validator's acceptance.
namespace headlight_failure213 {
inline startup_conflict129::Mismatch Inspect(std::span<const uint8_t> image,uint32_t base){
 using namespace fusionfix::shadows::ce;
 auto check=[&](uint32_t at,std::span<const uint8_t> expected){
  if(uint64_t(at)+expected.size()>image.size())return startup_conflict129::Mismatch{};
  return startup_conflict129::FirstMismatch(expected,image.subspan(at,expected.size()),at);
 };
 if(image.size()<ImageSize||image.size()>UINT32_MAX)return {};
 const uint8_t mz[]{'M','Z'};auto m=check(0,mz);if(m.valid)return m;
 const auto pe=Read32(image.data()+0x3c);if(pe>image.size()-0x80)return {};
 const uint8_t signature[]{0x50,0x45,0,0},machine[]{0x4c,1},magic[]{0x0b,1};
 for(auto x:{check(pe,signature),check(pe+4,machine),check(pe+24,magic)})if(x.valid)return x;
 uint32_t size=uint32_t(image.size());m=check(pe+24+56,std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(&size),4));if(m.valid)return m;
 const uint8_t submit[]{0xff,0x74,0x24,0x40,0xf3,0x0f,0x10,0x44,0x24,0x38,0xff,0x74,0x24,0x40,0x8b,0x44,0x24,0x14,0xff,0x74,0x24,0x40,0x83,0xe0,0xf7};
 m=check(SubmitRva,submit);if(m.valid)return m;
 for(size_t i=0;i<CallRvas.size();++i){
  const auto call=CallRvas[i];const uint8_t prefix[]{0x68,4,uint8_t(i<2?5:1),0,0,0x6a,2,0x6a,0},opcode[]{0xe8},cleanup[]{0x83,0xc4,0x40};
  for(auto x:{check(call-9,prefix),check(call,opcode),check(call+5,cleanup)})if(x.valid)return x;
  uint32_t relative=SubmitRva-call-5;m=check(call+1,std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(&relative),4));if(m.valid)return m;
 }
 const uint8_t compare[]{0x3b,5},increment[]{0xff,5};uint32_t counter=base+FrameCounterRva;
 for(auto x:{check(0x769f26,compare),check(0x4d0f82,increment),check(0x769f28,std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(&counter),4)),check(0x4d0f84,std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(&counter),4))})if(x.valid)return x;
 return {};
}
}
