#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "core/PoleCasterSnapshot134.hpp"
#include "ShadowFloatingPointState.hpp"
#include "FreshPageReadable.hpp"
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
std::map<uint64_t,std::vector<uint8_t>> memory;uintptr_t actualStack157{};
void Bytes(uint64_t at,size_t n){memory[at]=std::vector<uint8_t>(n);}
template<class T> void Put(uint64_t at,T value){for(auto& [base,bytes]:memory)if(at>=base&&at+sizeof(T)<=base+bytes.size()){std::memcpy(bytes.data()+size_t(at-base),&value,sizeof(T));return;}assert(false);}
bool Read(uint64_t at,void* out,size_t n){
 ++reads;SetLastError(999);_mm_setcsr(_mm_getcsr()|0x2000);
 if(at==failAddress)return false;
 if(actualStack157&&at>=actualStack157&&at+n<=actualStack157+128){std::memcpy(out,reinterpret_cast<const void*>(uintptr_t(at)),n);return true;}
 if(at==uint64_t(uintptr_t(CShadows::pFrameCounter))&&n==4){std::memcpy(out,CShadows::pFrameCounter,4);return true;}
 for(auto& [base,bytes]:memory)if(at>=base&&at+n<=base+bytes.size()){
  std::memcpy(out,bytes.data()+size_t(at-base),n);
  if(at==mutateAddress&&++mutateReads==mutateNth&&n>mutateOffset)reinterpret_cast<uint8_t*>(out)[mutateOffset]^=1;
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
void Event(int,const char*,const std::string& = {}) noexcept{}
std::atomic<bool> addonActive{true};
uintptr_t ff=0x50000000;
namespace FusionBackend130 {bool prepared=true;HMODULE pinnedModule=reinterpret_cast<HMODULE>(ff);struct Lease{bool owns=true;bool Owns()const noexcept{return owns;}}lease;}
HMODULE FakeModule141(LPCWSTR) noexcept{return FusionBackend130::pinnedModule;}
#define GetModuleHandleW FakeModule141
namespace native_projection66 {struct Permission142{bool allowed=true;};inline Permission142 Preflight(uintptr_t,size_t) noexcept{return {};}}
#define LIBERTY_POLE_TEST_PREFLIGHT142
#include "PoleStaticPartition141Runtime.inc"
#undef GetModuleHandleW

#define GetTickCount fixture134::Clock
#include "PoleCasterDiagnostic134Runtime.inc"
#undef GetTickCount
struct SafetyHookContext {uintptr_t esi{},eax{},esp{},eip{},ebx{},ecx{},edx{},edi{},ebp{};};
bool bVehicleNightShadows=true;
namespace OwnHeadlightCaster {bool diagnosticCounters=true;bool skip=false;bool Exclude(uintptr_t,uint32_t,bool) noexcept{return skip;}}
namespace PoleDrawWitness145 {void SunCaster(uintptr_t,uint32_t,uint32_t,uintptr_t,bool)noexcept;}
#include "caster134-extracted.inc"
namespace fixture145 {unsigned calls{},inputFP{},outputFP{};DWORD inputError{};uint32_t Native(void*) noexcept;uint32_t Call(void*);}
struct SafetyHookMid {bool valid{};std::vector<uint8_t> bytes;std::vector<uint8_t> original_bytes()const{return bytes;}};
struct SafetyHookInline {bool valid{};std::vector<uint8_t> bytes;std::vector<uint8_t> original_bytes()const{return bytes;} template<class Ret> Ret thiscall(void* p){return Ret(fixture145::Native(p));}};
template<class Hook,class Fn>void Prepare(Hook& h,uintptr_t at,Fn){h.valid=true;const size_t n=at==game+0x6ec84a?8:at==game+0x4dd0b0?8:at==game+0x4dae60?5:6;h.bytes.resize(n);assert(fixture134::Read(at,h.bytes.data(),n));}
struct Image145{std::vector<uint8_t> bytes;};Image145 image145;Image145* original=&image145;
std::vector<uint8_t> ReadMemory(uintptr_t p,size_t n){std::vector<uint8_t> out(n);if(!fixture134::Read(p,out.data(),n))throw 1;return out;}
void Require(bool value,const char*){if(!value)throw 1;}
#define GetTickCount fixture134::Clock
#include "PoleDrawWitness145Runtime.inc"
#define LIBERTY_POLE_DEPTH_TEST157
#define GetModuleHandleW FakeModule141
#include "PoleDepthRepair157Runtime.inc"
#undef GetModuleHandleW
#undef LIBERTY_POLE_DEPTH_TEST157
#undef GetTickCount
namespace fixture145 {
uint32_t Native(void*) noexcept {assert(GetLastError()==inputError&&_mm_getcsr()==inputFP);++calls;SafetyHookContext regs;regs.ecx=0x4a000000;regs.eax=0x49000000;PoleDrawWitness145::Child(regs);outputFP=inputFP^0x2000;SetLastError(314);_mm_setcsr(outputFP);return 35;}
uint32_t Call(void* object){inputError=GetLastError();inputFP=_mm_getcsr();const auto value=PoleDrawWitness145::Consumer(object,nullptr);assert(GetLastError()==314&&_mm_getcsr()==outputFP);return value;}
}

}


#include "native-rigid197c-harness.hpp"

int main(){
 using namespace standalone;using namespace fixture134;using namespace PoleDrawWitness145;
 // Failed physical158 experiment must remain durably off in a normal startup.
 assert(!PoleDepthRepair157::requested);const auto inactiveReads160=reads;
 PoleDepthRepair157::PrepareHooks();assert(!PoleDepthRepair157::prepared&&!PoleDepthRepair157::dispatchHook.valid);
 {PoleDepthRepair157::Scope inactive(reinterpret_cast<void*>(uintptr_t(1)));assert(!PoleDepthRepair157::current.valid);}
 PoleCasterDiagnostic134::requested=false;PoleDrawWitness145::PrepareHooks();assert(!PoleDrawWitness145::consumerHook.valid&&reads==inactiveReads160);

 Reset();actualClip155=CaptureDiagnostic44::lastStart;actualCommands155=128;actualEntries155=16;PoleCasterDiagnostic134::requested=true;Bytes(0x46000000,64);Put(entity+0x20,uint32_t(0x46000000));Put(0x46000000,float(1));Put(0x46000014,float(1));Put(0x46000028,float(1));
 pole_caster134::Snapshot owner{};assert(pole_caster134::Capture(PoleDrawWitness145::Read,PoleDrawWitness145::Frame,game,entity,20,owner));
 PoleCasterDiagnostic134::Binding map{7654,2,4,20,1000,GetCurrentThreadId(),true};HintPole(owner,map);
 constexpr uint32_t stack=0x47000000,command=0x48000000;Bytes(stack,0x80);Put(stack+0x34,uint32_t(game+0x713087));Put(stack+0x60,uint32_t(game+0x6e3864));Put(stack+0x68,uint32_t(0));Put(stack,game+0x7104f2);Put(stack+4,uint32_t(1905));Put(stack+8,uint32_t(255));Put(stack+12,uint32_t(254));Put(stack+16,uint32_t(0));Put(stack+20,uint32_t(0x46000000));
 Bytes(command,0x60);Put(command+4,uint32_t(0xabcd0000));Bytes(game+0xc327a0,4);Put(game+0xc327a0,uint32_t(77));
 SafetyHookContext regs;regs.esp=stack;regs.ecx=command;regs.esi=entity;const auto before=regs;SafetyHookContext objectRegs;objectRegs.ecx=entity;objectRegs.esp=stack+0x60;Object(objectRegs);assert(objects==1&&objectEntry.valid);SetLastError(73);const auto fp=_mm_getcsr();Put(entity+0x24,uint32_t(0x123|0x800));Constructor(regs);Put(entity+0x24,uint32_t(0x123));assert(published==1&&records[0].valid&&regs.esp==before.esp&&regs.ecx==before.ecx&&regs.esi==before.esi);assert(GetLastError()==73&&_mm_getcsr()==fp);
 Put(command+4,records[0].expectedTag);Put(command+0x58,uint16_t(1905));Put(command+0x5a,uint8_t(255));Put(command+0x5b,uint8_t(254));for(unsigned i=0;i<16;++i)if(i%4!=3)Put(command+0x10+i*4,records[0].pose[i]);
 Bytes(game+0xc3f260,16);Put(game+0xc3f260,float(1));
 Bytes(0x49000000,16);Put(0x4900000c,uint32_t(game+0x1fc490));Bytes(0x4a000000,12);Put(0x4a000008,uint32_t(0x4b000000));
 assert(fixture145::Call(reinterpret_cast<void*>(command))==35&&fixture145::calls==1&&!current.valid);
 assert(ShadowTrace34::events.size()==8&&ShadowTrace34::events[4].stage==192&&ShadowTrace34::events[5].stage==193);
 Constructor(regs);assert(published==1&&!objectEntry.valid);
 Put(command+4,uint32_t(3));const auto count=ShadowTrace34::events.size();assert(fixture145::Call(reinterpret_cast<void*>(command))==35&&fixture145::calls==2&&ShadowTrace34::events.size()==count);
 Put(command+4,records[0].expectedTag);Put(flags,uint8_t(128));assert(fixture145::Call(reinterpret_cast<void*>(command))==35&&ShadowTrace34::events.size()==count);Put(flags,uint8_t(7));
 CShadows::frame=21;assert(fixture145::Call(reinterpret_cast<void*>(command))==35&&ShadowTrace34::events.size()==count);CShadows::frame=20;
 CaptureDiagnostic44::sampleSources=false;const auto readsBefore=reads;assert(fixture145::Call(reinterpret_cast<void*>(command))==35&&reads==readsBefore);CaptureDiagnostic44::sampleSources=true;
 regs.esi=entity+0x320;Constructor(regs);assert(published==1);regs.esi=entity;
 Put(stack,uint32_t(game+0x7104f3));Constructor(regs);assert(published==1);Put(stack,uint32_t(game+0x7104f2));
 published=128;const auto cappedReads=reads;Constructor(regs);assert(reads==cappedReads);
 Put(stack,uint32_t(game+0x6e3864));Put(stack+8,uint32_t(0));regs.ecx=entity;Object(regs);assert(objects==2&&ShadowTrace34::events.back().stage==194);Put(stack,uint32_t(game+0x6e3865));Object(regs);assert(objects==2);
 Bytes(0x4c000000,0x20);Put(0x4c000014,uint32_t(2));SunCaster(entity,4,2,0x4c000000,false);assert(hint.sun&&hint.map.kind==2);Put(game+0xd9f1ed+2*0x110,uint8_t(0));assert(CurrentOwner(hint));Put(stack,uint32_t(game+0x6e3864));Put(stack+8,uint32_t(2));Object(regs);assert(objects==3&&ShadowTrace34::events.back().b==2);Put(stack+8,uint32_t(0));Object(regs);assert(objects==3&&!objectEntry.valid);
 //155 Actual executing commands deliberately survive loss of historical entity,
 //owner thread and submission frame attribution. They prove command context only.
 actualCommands155=actualChildren155=actualEntries155=0;records={};hint={};current={};CShadows::frame=23;PlayerShadowAllocation::ownerThread=123;
 const auto stages=[](unsigned stage){unsigned n=0;for(const auto& e:ShadowTrace34::events)if(e.stage==stage)++n;return n;};
 Put(command+0x54,uint32_t(0));Put(command+0x50,uint32_t(9));const auto n198=stages(198),n201=stages(201);
 assert(fixture145::Call(reinterpret_cast<void*>(command))==35);assert(stages(198)==n198+1&&stages(201)==n201+1&&!actual155.valid&&stages(203)>0);
 Put(command+0x54,uint32_t(2));assert(fixture145::Call(reinterpret_cast<void*>(command))==35&&stages(198)==n198+2&&stages(201)==n201+2);
 const auto accepted=stages(198);Put(info+0x3c,uint32_t(123));assert(fixture145::Call(reinterpret_cast<void*>(command))==35&&stages(198)==accepted);Put(info+0x3c,pole_caster134::PoleHash);
 mutateAddress=command;mutateNth=2;mutateReads=0;mutateOffset=4;assert(fixture145::Call(reinterpret_cast<void*>(command))==35&&stages(198)==accepted);mutateAddress=0;
 actualCommands155=128;const auto beforeCap=reads;assert(fixture145::Call(reinterpret_cast<void*>(command))==35&&stages(198)==accepted);assert(reads-beforeCap<=2);
 actualCommands155=0;actualAttempts155=4096;actualEntries155=16;const auto beforeAttemptCap=reads;assert(fixture145::Call(reinterpret_cast<void*>(command))==35&&reads-beforeAttemptCap<=1);assert(stages(204)>0);
 actualAttempts155=0;actualWindowUsed155=64;const auto beforeWindowCap=reads;assert(fixture145::Call(reinterpret_cast<void*>(command))==35&&reads-beforeWindowCap<=1);
 actualCommands155=0;CaptureDiagnostic44::sampleSources=false;const auto beforeOff=reads;assert(fixture145::Call(reinterpret_cast<void*>(command))==35&&reads==beforeOff);
 //157 Production functional callback: independent of F8 and historical145 receipts.
 CaptureDiagnostic44::sampleSources=false;tick=20000;PoleDepthRepair157::requested=true;PoleDepthRepair157::prepared=true;PoleDepthRepair157::registeredLocal=0;
 Bytes(ff+0x3366f0,4);Put(ff+0x3366f0,int32_t(-2147483591));Bytes(ff+0x3366ec,4);Put(ff+0x3366ec,uint32_t(0));Bytes(game+0xc6b308,4);Put(game+0xc6b308,int32_t(0));
 Bytes(game+0x13f59d4,4);Put(game+0x13f59d4,uint32_t(0x39000000));Bytes(game+0x11dbdbc,4);Bytes(game+0x11dbdc4,4);
 Put(command+0x54,uint32_t(0));Put(info+0x3c,pole_caster134::PoleHash);
 std::array<uint8_t,128> dispatchStack{};actualStack157=uintptr_t(dispatchStack.data());SafetyHookContext dispatch;dispatch.esp=actualStack157;dispatch.eax=0xaabbcc00;dispatch.ecx=987;dispatch.edx=654;dispatch.esi=0x4a000000;dispatch.edi=3;
 {PoleDepthRepair157::Scope active(reinterpret_cast<void*>(command));assert(PoleDepthRepair157::current.valid);const auto originalRegs=dispatch;const auto originalStack=dispatchStack;SetLastError(73);const auto originalFp=_mm_getcsr();PoleDepthRepair157::Dispatch(dispatch);
  assert(dispatch.eax==0xaabbcc00&&dispatchStack[0x30]==1&&dispatch.ecx==originalRegs.ecx&&dispatch.edx==originalRegs.edx&&dispatch.esi==originalRegs.esi&&dispatch.edi==originalRegs.edi&&dispatch.ebp==0);
  auto expectedRegs=originalRegs;assert(!std::memcmp(&dispatch,&expectedRegs,sizeof(dispatch)));auto expectedStack=originalStack;expectedStack[0x30]=1;assert(dispatchStack==expectedStack);
  assert(GetLastError()==73&&_mm_getcsr()==originalFp);uint32_t originalMode{};assert(PoleDepthRepair157::Read(command+0x54,&originalMode,4)&&originalMode==0);
  Native197c::Check(command);
  dispatch.eax=0xaabbcc00;dispatchStack[0x30]=0;Put(game+0x13f59d4,uint32_t(0));PoleDepthRepair157::Dispatch(dispatch);assert(dispatch.eax==0xaabbcc00&&!dispatchStack[0x30]);Put(game+0x13f59d4,uint32_t(0x39000000));
  Put(game+0xc6b308,int32_t(5));PoleDepthRepair157::Dispatch(dispatch);assert(dispatch.eax==0xaabbcc00&&!dispatchStack[0x30]);Put(game+0xc6b308,int32_t(0));
  ++CShadows::frame;PoleDepthRepair157::Dispatch(dispatch);assert(dispatch.eax==0xaabbcc00&&!dispatchStack[0x30]);--CShadows::frame;
  dispatch.ebx=1;PoleDepthRepair157::Dispatch(dispatch);assert(dispatch.eax==0xaabbcc00&&!dispatchStack[0x30]);dispatch.ebx=0;
  uint32_t originalTag157{};std::memcpy(&originalTag157,PoleDepthRepair157::current.bytes.data()+4,4);Put(command+4,uint32_t(777));PoleDepthRepair157::Dispatch(dispatch);assert(dispatch.eax==0xaabbcc00&&!dispatchStack[0x30]);Put(command+4,originalTag157);
  //197 Native owner-class parity: only class0x80 takes the separate owner
  //draw route. TYPE4 objects remain generic even with a nonnull context owner.
  Put(game+0x11dbdbc,entity);
  for(uint32_t type=0;type<16;++type){
   Put(entity+0x28,uint32_t(0xa500003fu|(type<<6)));
   dispatch.eax=0xaabbcc00;dispatchStack[0x30]=0;
   SetLastError(73);const auto before197=_mm_getcsr();PoleDepthRepair157::Dispatch(dispatch);
   assert(dispatch.eax==0xaabbcc00);assert((dispatchStack[0x30]==1)==(type!=2));
   assert(GetLastError()==73&&_mm_getcsr()==before197);
  }
  Put(entity+0x28,uint32_t(4<<6));dispatch.eax=0xaabbcc00;dispatchStack[0x30]=0;
  failAddress=entity+0x28;PoleDepthRepair157::Dispatch(dispatch);assert(dispatch.eax==0xaabbcc00&&!dispatchStack[0x30]);failAddress=0;
  for(const auto at:{uint64_t(game+0x11dbdbc),uint64_t(entity+0x28)}){
   mutateAddress=at;mutateNth=2;mutateReads=0;mutateOffset=0;
   PoleDepthRepair157::Dispatch(dispatch);assert(dispatch.eax==0xaabbcc00&&!dispatchStack[0x30]);mutateAddress=0;
  }
  Put(game+0x11dbdbc,uint32_t(0));
 }
 assert(!PoleDepthRepair157::current.valid);Put(info+0x3c,uint32_t(123));{PoleDepthRepair157::Scope rejected(reinterpret_cast<void*>(command));assert(!PoleDepthRepair157::current.valid);}Put(info+0x3c,pole_caster134::PoleHash);
 Put(ff+0x3366f0,int32_t(-1));{PoleDepthRepair157::Scope rejected(reinterpret_cast<void*>(command));assert(!PoleDepthRepair157::current.valid);}Put(ff+0x3366f0,int32_t(-2147483591));
 addonActive=false;const auto off157=reads;{PoleDepthRepair157::Scope rejected(reinterpret_cast<void*>(command));assert(!PoleDepthRepair157::current.valid);}assert(reads==off157);addonActive=true;
 //Actual production Arm->Prepare->Commit and quiet hook lifecycle, with
 //memory/hook boundary substitutes. Required native exact bytes are retained.
 original->bytes.assign(0x714000,0);Bytes(game+0x6ec7b0,0x203);const std::array<uint8_t,8> stolen157{0x8b,0x54,0x24,0x10,0x8b,0x4c,0x24,0x28};for(unsigned i=0;i<8;++i){Put(game+0x6ec84a+i,stolen157[i]);original->bytes[0x6ec84a+i]=stolen157[i];}
 Bytes(game+0x44940,6);Put(game+0x44940,uint8_t(0xa1));Put(game+0x44941,uint32_t(game+0x13f59d4));Put(game+0x44945,uint8_t(0xc3));
 Bytes(game+0x2f0b0,6);Put(game+0x2f0b0,uint8_t(0xa1));Put(game+0x2f0b1,uint32_t(game+0xc6b308));Put(game+0x2f0b5,uint8_t(0xc3));
 Bytes(ff+0x266868,9);const char localName157[]="wd_local";for(unsigned i=0;i<9;++i)Put(ff+0x266868+i,uint8_t(localName157[i]));Bytes(ff+0xa7693,21);
 const std::array<uint8_t,21> init157{0x68,0,0,0,0,0xc6,0x45,0xfc,0x08,0xff,0xd7,0x68,0,0,0,0,0xa3,0,0,0,0};for(unsigned i=0;i<21;++i)Put(ff+0xa7693+i,init157[i]);Put(ff+0xa7694,uint32_t(ff+0x266868));Put(ff+0xa769f,uint32_t(ff+0x3366f0));Put(ff+0xa76a4,uint32_t(ff+0x3366ec));
 FusionBackend130::lease.owns=false;Put(ff+0x3366f0,int32_t(-1));PoleDepthRepair157::prepared=false;PoleDepthRepair157::PrepareHooks();assert(PoleDepthRepair157::prepared&&PoleDepthRepair157::dispatchHook.valid);
 {PoleDepthRepair157::Scope denied(reinterpret_cast<void*>(command));assert(!PoleDepthRepair157::current.valid);}
 FusionBackend130::lease.owns=true;{PoleDepthRepair157::Scope pending(reinterpret_cast<void*>(command));assert(!PoleDepthRepair157::current.valid);}Put(ff+0x3366f0,int32_t(-2147483591));
 {PoleDepthRepair157::Scope recovered(reinterpret_cast<void*>(command));assert(PoleDepthRepair157::current.valid);}
 Bytes(game+0x4dd0b0,0xbf);const std::array<uint8_t,8> consumer157{0x51,0x56,0x8b,0xf1,0x0f,0xb7,0x46,0x58};for(unsigned i=0;i<8;++i){Put(game+0x4dd0b0+i,consumer157[i]);original->bytes[0x4dd0b0+i]=consumer157[i];}
 PoleCasterDiagnostic134::requested=false;objectHook={};constructorHook={};childHook={};consumerHook={};PrepareHooks();assert(consumerHook.valid&&!objectHook.valid&&!constructorHook.valid&&!childHook.valid);
 actualStack157=0;
 std::cout<<"PASS145 actual observer runtime: constructor caller/ESI identity, command tag/pose/model match, original consumer always called, child receipt scoped, stale/free/frame/capture refusal and bounded reads\n";
}

