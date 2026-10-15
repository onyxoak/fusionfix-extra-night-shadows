#include "ModelEffectGetterDiagnostic62.hpp"
#include <cassert>
#include <map>
#include <vector>
#include <algorithm>
using namespace model_effect_getter62;
struct Memory {
 std::map<std::uint32_t,std::vector<std::uint8_t>> bytes;
 unsigned calls{},fail{},changeEffect{},changeSlots{},changeOrigin{};
 bool Read(std::uint32_t p,void* out,std::size_t n) noexcept {
  ++calls;if(calls==fail)return false;
  auto it=bytes.find(p);if(it==bytes.end()||it->second.size()<n)return false;
  std::memcpy(out,it->second.data(),n);
  if(calls==changeEffect&&n==16)static_cast<std::uint8_t*>(out)[0]^=1;
  if(calls==changeOrigin&&n==16)static_cast<std::uint8_t*>(out)[4]^=1;
  if(calls==changeSlots&&n==16)static_cast<std::uint8_t*>(out)[0]^=1;
  return true;
 }
 template<class T>void Put(std::uint32_t p,const T& x){
  const auto* b=reinterpret_cast<const std::uint8_t*>(&x);bytes[p]={b,b+sizeof(x)};
 }
};
Memory Fixture(){
 Memory m;const std::array<std::uint32_t,4> header{0x2000,0x3f800000,0xc0000000,0x40400000};const std::uint32_t vtable=header[0];m.Put(0x1000,header);
 const std::array<std::uint32_t,4> slots{0x5000,0x6000,0x7000,0x8000};m.Put(vtable,slots);
 for(unsigned i=0;i<3;++i){std::array<std::uint8_t,16> b{};for(unsigned j=0;j<16;++j)b[j]=std::uint8_t(i*32+j);m.Put(slots[i+1],b);}
 return m;
}
Snapshot Run(Memory& m,std::uint32_t p=0x1000){return Observe(p,[&](auto a,void* b,auto n)noexcept{return m.Read(a,b,n);});}
int main(){
 assert(!Range(0,4)&&!Range(1,0)&&!Range(UINT32_MAX-2,4));assert(Range(UINT32_MAX-3,4));
 auto m=Fixture();const auto original=m.bytes;auto s=Run(m);assert(s.status==Status::Observed&&s.codeMask==7&&m.calls==7&&m.bytes==original);
 assert(s.effect==0x1000&&s.vtable==0x2000&&s.slots[3]==0x8000&&s.code[2][15]==79);
 assert(s.localOriginCopied&&s.localOriginFinite&&s.localOrigin==(std::array<float,3>{1,-2,3}));
 m=Fixture();s=Run(m,0);assert(s.status==Status::InvalidAddress&&m.calls==0);
 m=Fixture();s=Run(m,UINT32_MAX-2);assert(s.status==Status::InvalidAddress&&m.calls==0);
 m=Fixture();m.fail=1;s=Run(m);assert(s.status==Status::EffectUnreadable&&m.calls==1&&!s.localOriginCopied);
 m=Fixture();const std::array<std::uint32_t,4> bad{UINT32_MAX-14,0,0,0};m.Put(0x1000,bad);s=Run(m);assert(s.status==Status::InvalidAddress&&m.calls==1);
 m=Fixture();m.fail=2;s=Run(m);assert(s.status==Status::SlotsUnreadable&&m.calls==2);
 for(unsigned fail=3;fail<=5;++fail){m=Fixture();m.fail=fail;s=Run(m);assert(s.status==Status::CodeUnreadable&&s.codeMask==(7u^(1u<<(fail-3)))&&m.calls==7);}
 m=Fixture();m.fail=6;s=Run(m);assert(s.status==Status::EffectChanged&&m.calls==6);
 m=Fixture();m.changeEffect=6;s=Run(m);assert(s.status==Status::EffectChanged&&m.calls==6);
 m=Fixture();m.changeOrigin=6;s=Run(m);assert(s.status==Status::EffectChanged&&m.calls==6&&s.localOriginCopied);
 m=Fixture();m.fail=7;s=Run(m);assert(s.status==Status::SlotsChanged&&m.calls==7);
 m=Fixture();m.changeSlots=7;s=Run(m);assert(s.status==Status::SlotsChanged&&m.calls==7);
 m=Fixture();const std::array<std::uint32_t,4> badSlots{0x5000,0,UINT32_MAX-14,0x8000};m.Put(0x2000,badSlots);s=Run(m);assert(s.status==Status::CodeUnreadable&&s.codeMask==4&&m.calls==5);
 Snapshot maximal{};maximal.effect=maximal.vtable=UINT32_MAX;maximal.slots.fill(UINT32_MAX);maximal.codeMask=UINT32_MAX;maximal.status=Status::Observed;
 for(auto& code:maximal.code)code.fill(255);
 char text[500];assert(Format(maximal,UINT32_MAX,UINT32_MAX,UINT32_MAX,UINT32_MAX,text));
 assert(std::strlen(text)<500&&std::strstr(text,"observed_not_called=1 no_authority=1"));
 char shortText[16];assert(!Format(maximal,1,2,3,4,shortText)&&shortText[15]==0);
 m=Fixture();const std::array<std::uint32_t,4> nonfinite{0x2000,0x7fc00000,0x7f800000,0xff800000};m.Put(0x1000,nonfinite);s=Run(m);assert(s.localOriginCopied&&!s.localOriginFinite&&s.status==Status::Observed);
 assert(FormatOrigin(s,UINT32_MAX,UINT32_MAX,UINT32_MAX,text)&&std::strstr(text,"finite=0")&&std::strstr(text,"observation_only=1 no_authority=1"));
 std::puts("getter62 address/fault/race/code/log bounds passed; no native light changes");
}
