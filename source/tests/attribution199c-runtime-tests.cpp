#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <cassert>
#include <cstdio>
#include <span>
#include <memory>
#include <string>
#include "EffectAttribution199C.hpp"
#include "standalone-pe.hpp"
#include "safetyhook/context.hpp"
#include "core/ShadowFloatingPointState.hpp"
using SafetyHookContext=safetyhook::Context32;
struct SafetyHookMid {
 bool constructed=false,on=false;size_t stolen=5;
 explicit operator bool()const noexcept{return constructed;}
 bool enable()noexcept{on=constructed;return on;}
 bool disable()noexcept{on=false;return true;}
 std::span<const uint8_t> original_bytes()const noexcept {static uint8_t bytes[6]{};return {bytes,stolen};}
};
namespace ModelEffectObserver {struct LastErrorScope{DWORD value=GetLastError();~LastErrorScope(){SetLastError(value);}};}
std::atomic<bool> addonActive{true};
namespace CaptureDiagnostic44 {std::atomic<bool> sampleSources{true};}
namespace ShadowTrace34 {
 struct Event{uint32_t stage,frame,tick,key;int32_t a,b,c,d;float x,y,z,w;};
 std::atomic<bool>enabled{true};std::vector<Event>events;
 void Emit(Event e)noexcept{events.push_back(e);SetLastError(12345);_mm_setcsr(0x3f80);}
}
namespace rage {struct CLightSource{uint32_t words[32]{};};}
namespace NativeProjection66 {
 bool authenticated=true;
 struct Registry{bool volumetricMetadata199=false;}registry;
 using Observer=void(*)(unsigned,rage::CLightSource*,uint32_t,uint32_t,const effect_attribution199c::Packet&)noexcept;
 std::atomic<Observer>diagnosticPacket199c{nullptr};
}
uint32_t testFrame=42;
namespace CShadows{uint32_t*pFrameCounter=&testFrame;}
std::array<rage::CLightSource,2> bank{};
namespace PlayerShadowAllocation {
 rage::CLightSource*current=bank.data();uint32_t count=2;
 rage::CLightSource*CurrentLights(){return current;}
 uint32_t CurrentCount(){return count;}
}
std::unique_ptr<onyxoak::pe::Image> original;
uintptr_t game=0x440000;
std::vector<uint8_t> testLive;
alignas(16) std::array<uint8_t,0x400> stack{};
unsigned reads{},prepares{};bool failRead=false,changeFrame=false,mutatePacket=false;uintptr_t packetAddress{};
namespace Liberty20 {
 bool Copy(uintptr_t at,void*out,size_t n)noexcept {
  ++reads;SetLastError(98765);_mm_setcsr(0x5f80);
  if(failRead)return false;
  if(at==uintptr_t(&testFrame)&&n==4){memcpy(out,&testFrame,4);if(changeFrame)++testFrame;return true;}
  const auto valid=[&](const void*p,size_t size){return at>=uintptr_t(p)&&uint64_t(at-uintptr_t(p))+n<=size;};
  if(valid(stack.data(),stack.size())||valid(bank.data(),sizeof(bank))){
   memcpy(out,reinterpret_cast<void*>(at),n);
   if(mutatePacket&&at==packetAddress&&n==120)reinterpret_cast<uint32_t*>(out)[0]^=reads;
   return true;
  }
  if(at>=game&&uint64_t(at-game)+n<=testLive.size()){memcpy(out,testLive.data()+at-game,n);return true;}
  return false;
 }
}
std::vector<uint8_t>ReadMemory(uintptr_t at,size_t n){assert(at>=game&&at-game+n<=testLive.size());return {testLive.begin()+at-game,testLive.begin()+at-game+n};}
void Require(bool yes,const char*why){if(!yes)throw std::runtime_error(why);}
uintptr_t OriginalCall(uintptr_t rva){assert(original->bytes[rva]==0xe8);int32_t rel;memcpy(&rel,original->bytes.data()+rva+1,4);return game+rva+5+rel;}
void Prepare(SafetyHookMid&h,uintptr_t at,void(*)(SafetyHookContext&)noexcept){assert(at==game+0x6bd29f);++prepares;h.constructed=true;}
void Event(unsigned,const char*,const std::string&){}
#include "EffectAttribution199CRuntime.inc"

SafetyHookContext Fixture(bool wrapper){
 stack.fill(0);SafetyHookContext r{};r.ebp=uint32_t(uintptr_t(stack.data())+0x200+12);r.esp=(r.ebp&~15u)-0xe8;
 packetAddress=r.esp+0x68;*reinterpret_cast<uint32_t*>(r.esp)=uint32_t(packetAddress);
 const auto target=wrapper?0x6bcc50u:0x6bccd0u;
 uint32_t caller=0;
 for(uint32_t at=0x1000;at+5<original->bytes.size();++at){
  if(original->bytes[at]==0xe8&&OriginalCall(at)==game+target&&at!=0x6bccc6){caller=uint32_t(game+at+5);break;}
 }
 assert(caller);
 *reinterpret_cast<uint32_t*>(r.ebp+4)=wrapper?uint32_t(game+0x6bcccb):caller;
 if(wrapper)*reinterpret_cast<uint32_t*>(r.ebp+0x50)=caller;
 effect_attribution199c::Packet p{};p[17]=0;p[18]=0x261;p[12]=0x3f800000;p[15]=0x3f800000;
 memcpy(reinterpret_cast<void*>(packetAddress),p.data(),120);memcpy(bank.data(),p.data(),128);
 r.eax=0x12345678;r.edx=0xdeadbeef;r.esi=0x11111111;r.eflags=0x246;
 return r;
}
void CheckInsertion(SafetyHookContext&r){
 const auto saved=r;const auto before=stack;SetLastError(77777);_mm_setcsr(0x5f80);unsigned short cw1,cw2;
 __asm{fnstcw cw1}
 EffectAttribution199C::Insertion(r);
 __asm{fnstcw cw2}
 assert(GetLastError()==77777&&_mm_getcsr()==0x5f80&&cw1==cw2);
 assert(!memcmp(&saved,&r,sizeof(r))&&stack==before);
}
int main(int argc,char**argv){
 assert(argc==2);auto raw=onyxoak::pe::Read(argv[1]);assert(onyxoak::pe::Hash(raw)=="08759a5516f9837920ea504436236bbab89d0826a8e4d04ff106345177b5345d");
 original=std::make_unique<onyxoak::pe::Image>(raw,game);testLive=original->bytes;ShadowTrace34::events.reserve(30000);
 using namespace EffectAttribution199C;
 PrepareHooks();Enable();assert(!active&&!prepared&&!prepares);
 requested=true;
 for(uint32_t rva:{0x6bcc50u,0x6bccceu,0x6bccd0u,0x6bd29fu,0x6bd332u}){
  testLive[rva]^=1;PrepareHooks();assert(!prepared&&!active&&!prepares);testLive[rva]^=1;
 }
 PrepareHooks();Enable();assert(prepared&&active&&prepares==1&&NativeProjection66::diagnosticPacket199c.load()==Bank);
 auto r=Fixture(true);auto startReads=reads;CheckInsertion(r);assert(reads==startReads&&ShadowTrace34::events.empty());
 CaptureTransition(1);assert(captureEpoch==1&&ShadowTrace34::events.size()==1);
 CheckInsertion(r);assert(ShadowTrace34::events.size()==6&&records==1);
 assert(ShadowTrace34::events[1].stage==61000&&uint32_t(ShadowTrace34::events[1].c)==0x6bcccb);
 effect_attribution199c::Packet p{};memcpy(p.data(),bank.data(),128);
 auto bankBefore=bank;SetLastError(8888);_mm_setcsr(0x5f80);Bank(0,bank.data(),2,42,p);
 assert(ShadowTrace34::events.size()==11&&records==2&&GetLastError()==8888&&_mm_getcsr()==0x5f80&&!memcmp(bankBefore.data(),bank.data(),sizeof(bank)));
 r=Fixture(false);CheckInsertion(r);assert(records==3);
 auto saved=r;r.ebp+=16;auto count=records.load();CheckInsertion(r);assert(records==count);r=saved;
 *reinterpret_cast<uint32_t*>(r.esp)+=4;CheckInsertion(r);assert(records==count);r=Fixture(true);
 *reinterpret_cast<uint32_t*>(r.ebp+0x50)=uint32_t(game+0x12345);CheckInsertion(r);assert(records==count);r=Fixture(true);
 mutatePacket=true;CheckInsertion(r);assert(records==count);mutatePacket=false;
 failRead=true;CheckInsertion(r);assert(records==count);failRead=false;
 changeFrame=true;CheckInsertion(r);assert(records==count);changeFrame=false;testFrame=42;
 ++bank[0].words[0];Bank(0,bank.data(),2,42,p);assert(records==count);--bank[0].words[0];
 PlayerShadowAllocation::current=nullptr;Bank(0,bank.data(),2,42,p);assert(records==count);PlayerShadowAllocation::current=bank.data();
 Bank(0,bank.data(),2,43,p);assert(records==count);
 startReads=reads;attemptBudget.total=32768;CheckInsertion(r);assert(reads==startReads&&attemptBudget.limited>0);
 sourceBudget.total=2048;bankBudget.total=2048;startReads=reads;Bank(0,bank.data(),2,42,p);assert(reads==startReads&&bankBudget.limited>0);
 CaptureDiagnostic44::sampleSources=false;startReads=reads;CheckInsertion(r);Bank(0,bank.data(),2,42,p);assert(reads==startReads);CaptureDiagnostic44::sampleSources=true;
 recordingLock.test_and_set();CaptureTransition(1);recordingLock.clear();assert(captureEpoch==0);startReads=reads;CheckInsertion(r);assert(reads==startReads);
 CaptureTransition(1);assert(captureEpoch==2&&attemptBudget.total==0);CheckInsertion(r);assert(records==1);
 sequence=UINT32_MAX;CheckInsertion(r);assert(exhausted&&!Capturing());
 Disable();assert(!active&&!insertionHook.on&&!NativeProjection66::diagnosticPacket199c.load());
 effect_attribution199c::Budget b(8,16);for(unsigned i=0;i<8;++i)assert(b.Claim(1));assert(!b.Claim(1));for(unsigned i=0;i<8;++i)assert(b.Claim(129));assert(!b.Claim(257)&&b.total==16);
 p[20]=0xdead;auto q=p;q[18]|=8;q[28]=1;q[29]=2;
 assert(effect_attribution199c::CopyMatch(p,p,false)==1&&effect_attribution199c::CopyMatch(p,q,false)==0&&effect_attribution199c::CopyMatch(p,q,true)==2);
 for(unsigned i=0;i<30;++i){if(i==18||i==28||i==29)continue;auto changed=q;changed[i]^=1;assert(!effect_attribution199c::CopyMatch(p,changed,true));}
 puts("199c actual runtime: exact CE guards, native stack, default-off/lease/caps, raw witnesses, unchanged bytes/registers/FP/LastError and unknown cases passed");
}
