#include "FreshPageReadable.hpp"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>
#include "core/ShadowFloatingPointState.hpp"
#include "core/HeadlightCasterPolicy.hpp"
#include "PerformanceWorkAvoidance224.hpp"

namespace standalone {
uintptr_t game=0x400000;
std::atomic<bool> addonActive{true};
bool bVehicleNightShadows=true;
struct SafetyHookContext {uintptr_t esi{},eax{},esp{},eip{},ebx{},ecx{},edx{},edi{},ebp{};};
namespace OwnHeadlightCaster {
bool diagnosticCounters=false;
unsigned visits=0;
fusionfix::shadows::caster::Context context{};
bool Exclude(uintptr_t entity,uint32_t type,bool artificial) noexcept {
 if(diagnosticCounters)++visits;
 return fusionfix::shadows::caster::Exclude(context,entity,type,artificial);
}
}
namespace PoleCasterDiagnostic134 {
bool requested=false;unsigned calls=0;
void Observe(uintptr_t,uint32_t type,bool,bool) noexcept {if(requested&&type==4)++calls;}
}
namespace PoleDrawWitness145 {
unsigned calls=0;
void SunCaster(uintptr_t,uint32_t type,uint32_t mode,uintptr_t,bool) noexcept {
 if(PoleCasterDiagnostic134::requested&&type==4&&mode==2)++calls;
}
}
namespace PoleStaticPartition141 {
bool requested=false;unsigned calls=0;
bool NeedsSlowAdmission224(uintptr_t,uint32_t type,uint32_t mode) noexcept {return requested&&type==4&&mode==0;}
bool Reclassify(uintptr_t,uint32_t type,uint32_t mode,uintptr_t,bool excluded,uint32_t* eligibility) noexcept {
 if(!requested||type!=4||mode!=0||excluded)return false;
 ++calls;*eligibility=1;return true;
}
}
bool (*receiverCull226)(uintptr_t,uint32_t) noexcept=nullptr;
#include "caster189-reference.inc"
#include "caster134-extracted.inc"
}

static unsigned short ControlWord() {unsigned short v{};__asm { fnstcw v } return v;}
int main(){
 using namespace standalone;
 // Actual policy: only vehicles and occupants can be excluded. Exercise
 // type, artificial/sun, feature guards, disabled addon and native routing.
 alignas(16) unsigned char entity[640]{};unsigned char stack[32]{};
 OwnHeadlightCaster::context.car=uintptr_t(entity);
 OwnHeadlightCaster::context.ownBeam=true;
 OwnHeadlightCaster::context.occupants[0]=uintptr_t(entity);
 for(unsigned type: {0u,1u,2u,3u,4u,5u,0xffffffffu})
 for(unsigned mask=0;mask<128;++mask){
  addonActive=(mask&1)!=0;bVehicleNightShadows=(mask&2)!=0;
  OwnHeadlightCaster::diagnosticCounters=(mask&4)!=0;
  PoleCasterDiagnostic134::requested=(mask&8)!=0;PoleStaticPartition141::requested=(mask&16)!=0;
  stack[11]=uint8_t((mask>>5)&1);entity[620]=uint8_t((mask&64)?4:0);
  SafetyHookContext a{uintptr_t(entity),type,uintptr_t(stack),123,11,22,(mask&32)?2u:0u,44,55},b=a;
  unsigned counts[2][4]{};
  for(unsigned run=0;run<2;++run){
   OwnHeadlightCaster::visits=PoleCasterDiagnostic134::calls=PoleDrawWitness145::calls=PoleStaticPartition141::calls=0;
   const auto mxcsr=_mm_getcsr();_mm_setcsr((mxcsr&~0x6000u)|0x4000u);
   unsigned short cw=0x077f;__asm fldcw cw
   const auto expectedCw=ControlWord();const auto expectedMxcsr=_mm_getcsr();SetLastError(1234);
   if(run==0)Caster189(a);else Caster(b);
   assert(ControlWord()==expectedCw&&_mm_getcsr()==expectedMxcsr&&GetLastError()==1234);
   _mm_setcsr(mxcsr);__asm fninit
   counts[run][0]=OwnHeadlightCaster::visits;counts[run][1]=PoleCasterDiagnostic134::calls;
   counts[run][2]=PoleDrawWitness145::calls;counts[run][3]=PoleStaticPartition141::calls;
  }
  assert(!std::memcmp(&a,&b,sizeof(a))&&!std::memcmp(counts[0],counts[1],sizeof(counts[0])));
 }
 // The ordinary path must not read the native stack or entity at all.
 addonActive=true;OwnHeadlightCaster::diagnosticCounters=false;
 PoleCasterDiagnostic134::requested=PoleStaticPartition141::requested=false;
 SafetyHookContext invalid{1,4,1,0};Caster(invalid);assert(invalid.eip==game+0x6e374f);
 // Isolated callback cost, not a game FPS benchmark. Indirect volatile calls
 // keep both extracted production bodies in the measured loop.
 unsigned char benchStack[32]{};SafetyHookContext regs{uintptr_t(entity),4,uintptr_t(benchStack),0};
 using Fn=void(*)(SafetyHookContext&);Fn volatile functions[]={Caster189,Caster};
 constexpr unsigned n=1000000;double elapsed[2]{};
 for(unsigned round=0;round<4;++round)for(unsigned j=0;j<2;++j){
  const unsigned which=(round&1)?1-j:j;
  auto start=std::chrono::steady_clock::now();
  for(unsigned i=0;i<n;++i)functions[which](regs);
  elapsed[which]+=std::chrono::duration<double,std::nano>(std::chrono::steady_clock::now()-start).count()/n;
 }
 std::cout<<"PASS195 896 production-callback equivalence cases, native registers, optional counters/routes, x87 control and MXCSR preserved; invalid stack avoided\n";
 std::cout<<"ISOLATED_CALLBACK_NS baseline="<<elapsed[0]/4<<" candidate="<<elapsed[1]/4<<" ratio="<<elapsed[0]/elapsed[1]<<" (not game FPS)\n";
}
