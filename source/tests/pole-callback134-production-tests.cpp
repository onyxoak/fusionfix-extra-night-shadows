#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#include "core/PoleCasterSnapshot134.hpp"
#include "ShadowFloatingPointState.hpp"
#include "FreshPageReadable.hpp"
#include "PerformanceWorkAvoidance224.hpp"
#include "core/ShadowReceiver.hpp"

namespace standalone {
uint32_t game=0x400000;
namespace CaptureDiagnostic44 {std::atomic<bool> sampleSources{true};uint32_t lastStart=1000;}
namespace ShadowTrace34 {
struct Event {uint32_t stage,frame,tick,key;int32_t a,b,c,d;float x,y,z,w;};
std::atomic<bool> enabled{true};std::vector<Event> events;
void Emit(Event e) noexcept{events.push_back(e);}
}
namespace PlayerShadowAllocation {std::atomic<DWORD> ownerThread{GetCurrentThreadId()};}
namespace CShadows {uint32_t frame=20;uint32_t* pFrameCounter=&frame;}
namespace fixture134 {
uint32_t tick=1050;unsigned reads=0;uint64_t failAddress=0,mutateAddress=0;unsigned mutateNth=0,mutateReads=0,mutateOffset=0;
std::map<uint64_t,std::vector<uint8_t>> memory;
void Bytes(uint64_t at,size_t n){memory[at]=std::vector<uint8_t>(n);}
template<class T> void Put(uint64_t at,T value){for(auto& [base,bytes]:memory)if(at>=base&&at+sizeof(T)<=base+bytes.size()){std::memcpy(bytes.data()+size_t(at-base),&value,sizeof(T));return;}assert(false);}
bool Read(uint64_t at,void* out,size_t n){
 ++reads;SetLastError(999);_mm_setcsr(_mm_getcsr()|0x2000);
 if(failAddress>=at&&failAddress<at+n)return false;
 if(at==uint64_t(uintptr_t(CShadows::pFrameCounter))&&n==4){std::memcpy(out,CShadows::pFrameCounter,4);return true;}
 for(auto& [base,bytes]:memory)if(at>=base&&at+n<=base+bytes.size()){
  std::memcpy(out,bytes.data()+size_t(at-base),n);
  if(mutateAddress>=at&&mutateAddress<at+n&&++mutateReads==mutateNth&&mutateAddress-at+mutateOffset<n)reinterpret_cast<uint8_t*>(out)[mutateAddress-at+mutateOffset]^=1;
  return true;
 }return false;
}
constexpr uint32_t render=0x20000000,entity=0x11000000,pool=0x30000000,flags=0x30001000,info=0x31000000,associated=0x32000000;
void Reset(){
 memory.clear();reads=mutateReads=mutateNth=mutateOffset=0;failAddress=mutateAddress=0;CShadows::frame=20;tick=1050;
 CaptureDiagnostic44::sampleSources=true;CaptureDiagnostic44::lastStart=1000;ShadowTrace34::enabled=true;ShadowTrace34::events.clear();PlayerShadowAllocation::ownerThread=GetCurrentThreadId();
 Bytes(game+0x1232c60,4);Put(game+0x1232c60,pool);Bytes(pool,24);Put(pool,entity);Put(pool+4,flags);Put(pool+8,int32_t(4));Put(pool+12,int32_t(0x320));Bytes(flags,4);Put(flags,uint8_t(7));
 Bytes(entity,0x320);Put(entity,uint32_t(game+0xaabb));Put(entity+0x24,uint32_t(0x123));Put(entity+0x28,uint32_t(4<<6));Put(entity+0x2e,int16_t(1905));Put(entity+0x34,uint32_t(0x33000000));Put(entity+0x38,uint32_t(0x36000100));
 Bytes(game+0xe95cd8+1905*4,4);Put(game+0xe95cd8+1905*4,info);Bytes(info,0x44);Put(info+0x3c,pole_caster134::PoleHash);Put(info+8,uint32_t(0x34000000));
 Put(entity+0x210,uint32_t(0x100800));Put(entity+0x1bc,associated);Put(entity+0x280,uint32_t(0x35000000));Bytes(associated,0x2c);Put(associated+0x28,uint32_t(0x800000));
 Bytes(render,0x944);Put(render+0x940,uint32_t(2));Bytes(game+0x1193312,1);Bytes(game+0xd9f1ed+2*0x110,19);Put(game+0xd9f1ed+2*0x110,uint8_t(1));Put(game+0xd9f1ed+2*0x110+11,uint32_t(7654));Put(game+0xd9f1ed+2*0x110+15,uint32_t(4));
}
DWORD Clock() noexcept{return tick;}
}
namespace Liberty20 {bool CopyLocal(uintptr_t at,void* out,size_t n) noexcept{return fixture134::Read(at,out,n);}
bool Copy(uintptr_t at,void* out,size_t n) noexcept{return fixture134::Read(at,out,n);}}
void Event(int,const char*,const char*) noexcept{}
std::atomic<bool> addonActive{true};
uintptr_t ff=0x50000000;
namespace FusionBackend130 {bool prepared=true;HMODULE pinnedModule=reinterpret_cast<HMODULE>(ff);struct Lease{bool owns=true;bool Owns()const noexcept{return owns;}}lease;}
HMODULE FakeModule141(LPCWSTR) noexcept{return FusionBackend130::pinnedModule;}
namespace pe {inline HMODULE ModuleAt(uintptr_t) noexcept{return FakeModule141(nullptr);}}
#define GetModuleHandleW FakeModule141
namespace native_projection66 {struct Permission142{bool allowed=true;};inline Permission142 Preflight(uintptr_t,size_t) noexcept{return {};}}
#define LIBERTY_POLE_TEST_PREFLIGHT142
#include "PoleStaticPartition141Runtime.inc"
#undef GetModuleHandleW

#define GetTickCount fixture134::Clock
namespace PoleDrawWitness145 {void SunCaster(uintptr_t,uint32_t,uint32_t,uintptr_t,bool)noexcept{} template<class Map>void HintPole(const pole_caster134::Snapshot&,const Map&)noexcept{}}
#include "PoleCasterDiagnostic134Runtime.inc"
#undef GetTickCount
struct SafetyHookContext {uintptr_t esi{},eax{},esp{},eip{},ebx{},ecx{},edx{},edi{},ebp{};};
bool bVehicleNightShadows=true;
namespace OwnHeadlightCaster {bool diagnosticCounters=true;bool skip=false;bool Exclude(uintptr_t,uint32_t,bool) noexcept{return skip;}}
bool (*receiverCull226)(uintptr_t,uint32_t) noexcept=nullptr;
#include "caster134-extracted.inc"
}

int main(){
 using namespace standalone;using namespace fixture134;using namespace PoleCasterDiagnostic134;
 const auto reset=[](){Reset();binding={};budgetClip=window=used=total=0;};
 //162 DefaultOFF skips all pole override reads/scopes and preserves native Caster continuation.
 assert(!PoleStaticPartition141::requested);reset();CaptureDiagnostic44::sampleSources=false;OwnHeadlightCaster::skip=false;
 PoleStaticPartition141::prepared=true;PoleStaticPartition141::binding.valid=true;
 const auto offReads162=reads;std::vector<uint8_t> offImage162;PoleStaticPartition141::Prepare(offImage162);assert(!PoleStaticPartition141::prepared&&reads==offReads162);
 {PoleStaticPartition141::Scope inactive(reinterpret_cast<void*>(uintptr_t(1)));assert(!PoleStaticPartition141::binding.valid);}assert(reads==offReads162);
 std::array<uint8_t,32> nativeStack162{};SafetyHookContext offRegs162{entity,4,uintptr_t(nativeStack162.data()),0};offRegs162.edx=0;offRegs162.ebp=0x44000000;offRegs162.ebx=17;offRegs162.ecx=18;offRegs162.edi=19;const auto expected162=offRegs162;
 Caster(offRegs162);assert(reads==offReads162&&offRegs162.eip==game+0x6e374f);auto wanted162=expected162;wanted162.eip=game+0x6e374f;assert(!std::memcmp(&wanted162,&offRegs162,sizeof(wanted162)));
 for(unsigned type162:{1u,3u,4u})for(unsigned mode162:{0u,2u})for(bool skip162:{false,true}){auto q=expected162;q.eax=type162;q.edx=mode162;OwnHeadlightCaster::skip=skip162;Caster(q);assert(q.eax==type162&&q.edx==mode162&&q.eip==game+(skip162?0x6e3867:0x6e374f)&&reads==offReads162);}OwnHeadlightCaster::skip=false;

 // Real helper exercises current child relation, changed pointer and bounded count refusal.
 reset();constexpr uint32_t frag=0x34000000,root=0x36000000,array=0x37000000,child=0x38000000;
 Bytes(frag,0x200);Put(frag+0xb4,root);Put(frag+0xd4,array);Put(frag+0x1f3,uint8_t(1));Bytes(array,4);Put(array,child);Bytes(child,0x98);Put(child+0x90,uint32_t(0x39000000));Put(child+0xc,uint32_t(2));
 pole_children135::Snapshot cs{};assert(pole_children135::Capture(Read,frag,cs)&&cs.count==1&&cs.child[0].drawable90==0x39000000);
 mutateAddress=child+0x90;mutateNth=2;mutateReads=0;assert(!pole_children135::Capture(Read,frag,cs)&&cs.fragment==0);
 mutateAddress=0;Put(frag+0x1f3,uint8_t(17));assert(!pole_children135::Capture(Read,frag,cs));
 requested=true;reset();uint8_t stack[12]{};SafetyHookContext regs{entity,4,uintptr_t(stack),0};
 regs.ebx=101;regs.ecx=102;regs.edx=103;regs.edi=104;regs.ebp=105;const auto inputs=regs;
 const uint32_t incoming=_mm_getcsr();SetLastError(71);
 {Scope scope(reinterpret_cast<void*>(render));assert(binding.valid);assert(GetLastError()==71&&_mm_getcsr()==incoming);Caster(regs);assert(GetLastError()==71&&_mm_getcsr()==incoming);}
 assert(!binding.valid);assert(regs.eip==game+0x6e374f);assert(ShadowTrace34::events.size()==6);
 assert(regs.esi==inputs.esi&&regs.eax==inputs.eax&&regs.esp==inputs.esp&&regs.ebx==inputs.ebx&&regs.ecx==inputs.ecx&&regs.edx==inputs.edx&&regs.edi==inputs.edi&&regs.ebp==inputs.ebp);
 assert(ShadowTrace34::events[0].stage==140&&uint32_t(ShadowTrace34::events[0].a)==7654&&ShadowTrace34::events[0].key==entity);
 assert(ShadowTrace34::events[1].a==1905&&uint32_t(ShadowTrace34::events[1].b)==pole_caster134::PoleHash);
 assert(uint32_t(ShadowTrace34::events[2].a)==0x100800&&uint32_t(ShadowTrace34::events[2].b)==associated&&uint32_t(ShadowTrace34::events[2].c)==0x800000&&uint32_t(ShadowTrace34::events[2].d)==0x35000000);
 assert(ShadowTrace34::events[5].stage==147&&uint32_t(ShadowTrace34::events[5].a)==0x36000100);
 reset();mutateAddress=entity;mutateOffset=0x38;mutateNth=2;{Scope scope(reinterpret_cast<void*>(render));Observe(entity,4,true,false);assert(ShadowTrace34::events.empty());}
 // Disabled, inactive capture, expired clip, deferred or foreign-thread paths do no observer reads.
 for(unsigned test=0;test<5;++test){reset();if(test==0)requested=false;else if(test==1)CaptureDiagnostic44::sampleSources=false;else if(test==2)tick=11000;else if(test==3)Put(game+0x1193312,uint8_t(1));else PlayerShadowAllocation::ownerThread=GetCurrentThreadId()+1;
  {Scope scope(reinterpret_cast<void*>(render));const auto before=reads;Observe(entity,4,true,false);assert(reads==before&&ShadowTrace34::events.empty());}requested=true;}
 // Exact hash/type/membership and repeat refusal; no partial diagnostic publication.
 for(unsigned test=0;test<6;++test){reset();if(test==0)Put(info+0x3c,uint32_t(1));else if(test==1)Put(entity+0x28,uint32_t(3<<6));else if(test==2)Put(flags,uint8_t(0x80));else if(test==3)failAddress=entity+0x210;else if(test==4){mutateAddress=entity+0x1bc;mutateNth=2;}else {mutateAddress=flags;mutateNth=2;}
  {Scope scope(reinterpret_cast<void*>(render));Observe(entity,4,true,false);assert(ShadowTrace34::events.empty());}}
 reset();{Scope outer(reinterpret_cast<void*>(render));const auto old=binding;{Scope nested(nullptr);assert(!binding.valid);}assert(binding.valid&&binding.key==old.key);CShadows::frame=21;const auto before=reads;Observe(entity,4,true,false);assert(reads==before&&ShadowTrace34::events.empty());}
 reset();{Scope scope(reinterpret_cast<void*>(render));OwnHeadlightCaster::skip=true;Caster(regs);assert(regs.eip==game+0x6e3867&&(ShadowTrace34::events[0].d&2));OwnHeadlightCaster::skip=false;}
 reset();{Scope scope(reinterpret_cast<void*>(render));for(unsigned i=0;i<25;++i)Observe(entity,4,true,false);assert(used==8&&ShadowTrace34::events.size()==48);
  tick+=128;Observe(entity,4,true,false);assert(used==1&&total==9&&ShadowTrace34::events.size()==54);
 }
 //128 successful publications fit16 windows inside the10second lease.
 reset();{Scope scope(reinterpret_cast<void*>(render));for(unsigned windowIndex=0;windowIndex<16;++windowIndex){
   for(unsigned sample=0;sample<8;++sample)Observe(entity,4,true,false);
   tick+=128;
  }
  assert(total==128&&ShadowTrace34::events.size()==768);const auto before=reads;
  Observe(entity,4,true,false);assert(total==128&&reads==before&&ShadowTrace34::events.size()==768);
  CaptureDiagnostic44::lastStart=tick;Observe(entity,4,true,false);assert(reads==before); //stale clip scope refused.
 }
 {Scope newClip(reinterpret_cast<void*>(render));Observe(entity,4,true,false);assert(used==1&&total==1&&budgetClip==CaptureDiagnostic44::lastStart&&ShadowTrace34::events.size()==774);}

 // Actual Caster body changes onlyEDX for the authenticated native MODE0 static pole.
 const auto prepare141=[&](){reset();addonActive=true;FusionBackend130::prepared=true;FusionBackend130::lease.owns=true;FusionBackend130::pinnedModule=reinterpret_cast<HMODULE>(ff);PoleStaticPartition141::binding={};PoleStaticPartition141::requested=true;PoleStaticPartition141::prepared=true;
  Put(entity,uint32_t(game+0xaab0dc));Put(entity+0x24,uint32_t(0x86808021));Put(entity+0x210,uint32_t(0x2000));Put(entity+0x1bc,uint32_t(0));Put(entity+0x280,uint32_t(0));
  Bytes(game+0xaab0dc+0x28,4);Put(game+0xaab0dc+0x28,uint32_t(game+0x16d0));Bytes(game+0x16d0,3);Put(game+0x16d0,uint8_t(0x32));Put(game+0x16d1,uint8_t(0xc0));Put(game+0x16d2,uint8_t(0xc3));
  Bytes(0x44000000,0x20);Put(0x44000014,uint32_t(0));
  Bytes(game+0xc3eed8,4);Put(game+0xc3eed8,uint32_t(0x45000000));Bytes(game+0x110e240,4);Put(game+0x110e240,uint32_t(1));Bytes(0x45000000,128);Put(0x45000060,uint32_t(7654));Put(0x45000054,float(20));Put(0x45000040,float(1));Put(0x45000030,float(1));
  Bytes(0x46000000,64);Put(0x46000000,float(1));Put(0x46000014,float(1));Put(0x46000028,float(1));Put(entity+0x20,uint32_t(0x46000000));Put(0x46000030,float(0));Put(0x46000034,float(0));Put(0x46000038,float(0));
 };
 prepare141();std::vector<uint8_t> image141(size_t(pole_mode141::Rva)+pole_mode141::Bytes.size());std::memcpy(image141.data()+pole_mode141::Rva,pole_mode141::Bytes.data(),pole_mode141::Bytes.size());Bytes(game+pole_mode141::Rva,pole_mode141::Bytes.size());std::memcpy(memory[game+pole_mode141::Rva].data(),pole_mode141::Bytes.data(),pole_mode141::Bytes.size());
 PoleStaticPartition141::Prepare(image141);assert(PoleStaticPartition141::prepared);
 image141[pole_mode141::Rva+0xc4]^=1;PoleStaticPartition141::Prepare(image141);assert(!PoleStaticPartition141::prepared);image141[pole_mode141::Rva+0xc4]^=1;
 Put(game+pole_mode141::Rva+0xc4,uint8_t(0x82));PoleStaticPartition141::Prepare(image141);assert(!PoleStaticPartition141::prepared);
 prepare141();SafetyHookContext poleRegs{entity,4,uintptr_t(stack),0};poleRegs.edx=0;poleRegs.ebp=0x44000000;poleRegs.ebx=17;poleRegs.ecx=18;poleRegs.edi=19;
 const auto before141=poleRegs;SetLastError(73);const auto fp141=_mm_getcsr();
 {PoleStaticPartition141::Scope scope(reinterpret_cast<void*>(render));assert(PoleStaticPartition141::binding.valid);Caster(poleRegs);}
 assert(poleRegs.edx==0&&poleRegs.eax==1&&poleRegs.eip==game+0x6e3767&&poleRegs.esi==before141.esi&&poleRegs.ebx==before141.ebx&&poleRegs.ecx==before141.ecx&&poleRegs.edi==before141.edi&&poleRegs.ebp==before141.ebp);
 assert(GetLastError()==73&&_mm_getcsr()==fp141);uint32_t stackMode141{};assert(Read(0x44000014,&stackMode141,4)&&stackMode141==0);assert(ShadowTrace34::events.size()==1&&ShadowTrace34::events[0].stage==148);
 // Native83FA01 is CMP EDX,1. Native3864 reloads unchangedstackMODE eachentity.
 const auto nativeAdmission141=[](uint32_t mode,bool bit11,bool predicate,bool room){return room&&(mode==0?(bit11||predicate):(mode==1&&!bit11&&!predicate));};
 assert(!nativeAdmission141(0,false,false,true)&&nativeAdmission141(poleRegs.edx,true,false,true));
 assert(!nativeAdmission141(poleRegs.edx,true,false,false));assert(nativeAdmission141(0,true,false,true)&&!nativeAdmission141(1,true,false,true));
 assert(stackMode141==0&&!nativeAdmission141(stackMode141,false,false,true));
 for(unsigned test=0;test<11;++test){prepare141();SafetyHookContext q=before141;
  if(test==0)addonActive=false;else if(test==1)PlayerShadowAllocation::ownerThread=GetCurrentThreadId()+1;else if(test==2)Put(flags,uint8_t(0x80));else if(test==3)Put(entity+0x210,uint32_t(0x2800));else if(test==4)Put(entity+0x210,uint32_t(0x102000));else if(test==5)Put(entity+0x38,uint32_t(0));else if(test==6)Put(game+0x16d0,uint8_t(0x33));else if(test==7)Put(0x44000014,uint32_t(1));else if(test==8)Put(info+0x3c,uint32_t(1));else if(test==9)q.eax=3;else OwnHeadlightCaster::skip=true;
  const auto expectedType=q.eax;{PoleStaticPartition141::Scope scope(reinterpret_cast<void*>(render));const auto readsBefore=reads;Caster(q);assert(q.edx==0&&q.eax==expectedType);if(addonActive)assert(q.eip==game+(test==10?0x6e3867:0x6e374f));if(test==0)assert(reads==readsBefore);}
  OwnHeadlightCaster::skip=false;
 }
 prepare141();{PoleStaticPartition141::Scope scope(reinterpret_cast<void*>(render));CShadows::frame=21;assert(!PoleStaticPartition141::Reclassify(entity,4,0,0x44000000,false));}
 prepare141();{PoleStaticPartition141::Scope scope(reinterpret_cast<void*>(render));mutateAddress=entity+0x1bc;mutateNth=2;assert(!PoleStaticPartition141::Reclassify(entity,4,0,0x44000000,false));}
 prepare141();{PoleStaticPartition141::Scope scope(reinterpret_cast<void*>(render));mutateAddress=entity;mutateOffset=0x38;mutateNth=3;assert(!PoleStaticPartition141::Reclassify(entity,4,0,0x44000000,false));}
 prepare141();Put(game+0xd9f1ed+2*0x110+15,uint32_t(2));{PoleStaticPartition141::Scope scope(reinterpret_cast<void*>(render));assert(!PoleStaticPartition141::binding.valid);}
 prepare141();Put(game+0x1193312,uint8_t(1));{PoleStaticPartition141::Scope scope(reinterpret_cast<void*>(render));assert(!PoleStaticPartition141::binding.valid);}
 prepare141();Put(0x45000040,float(0));{PoleStaticPartition141::Scope scope(reinterpret_cast<void*>(render));assert(!PoleStaticPartition141::binding.valid);}
 prepare141();{PoleStaticPartition141::Scope outer(reinterpret_cast<void*>(render));{PoleStaticPartition141::Scope nested(nullptr);assert(!PoleStaticPartition141::binding.valid);}assert(PoleStaticPartition141::binding.valid);}
 prepare141();{PoleStaticPartition141::Scope scope(reinterpret_cast<void*>(render));pole_caster134::Snapshot baselineA{},baselineB{};const auto frame142=[]() noexcept{return CShadows::frame;};const auto baselineStart=reads;assert(pole_caster134::Capture(Read,frame142,game,entity,20,baselineA)&&pole_caster134::Capture(Read,frame142,game,entity,20,baselineB));const auto baselineReads=reads-baselineStart;Put(0x46000030,float(500));const auto before=reads;uint32_t result{};assert(!PoleStaticPartition141::Reclassify(entity,4,0,0x44000000,false,&result));assert(reads-before<=7);std::cout<<"COST142 outside-volume reads="<<reads-before<<" vs old repeated-identity reads="<<baselineReads<<"\n";}
 prepare141();{PoleStaticPartition141::Scope scope(reinterpret_cast<void*>(render));Put(0x45000054,float(21));assert(!PoleStaticPartition141::Reclassify(entity,4,0,0x44000000,false));}
 std::cout<<"PASS134 actual Caster extraction + snapshot runtime: exact pool/hash/raw gates, repeats, capture/10s/thread/frame guards, native decisions and LastError/FP preserved\n";
}


