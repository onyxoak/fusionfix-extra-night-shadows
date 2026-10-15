#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>
#include "safetyhook/context.hpp"
#include "core/ShadowFloatingPointState.hpp"
#include "core/NativeProducerGate116.hpp"
using SafetyHookContext=safetyhook::Context32;
struct SafetyHookMid {
 bool constructed=false,enabled=false;
 explicit operator bool()const noexcept{return constructed;}
 bool enable()noexcept{enabled=constructed;return enabled;}
 bool disable()noexcept{enabled=false;return true;}
 std::span<const std::uint8_t>original_bytes()const noexcept{return native_producer_gate116::FirstBytes;}
};
namespace ModelEffectObserver{struct LastErrorScope{DWORD value=GetLastError();~LastErrorScope(){SetLastError(value);}};}
std::atomic<bool>addonActive{true};
namespace CaptureDiagnostic44{std::atomic<bool>sampleSources{true};}
namespace ShadowTrace34 {
 struct Record{std::uint32_t stage,frame,tick,key;std::int32_t a,b,c,d;float x,y,z,w;};
 std::atomic<bool>enabled{true};std::vector<Record>events;
 void Emit(Record r)noexcept{events.push_back(r);SetLastError(1234);_mm_setcsr(0x3f80);}
}
struct Original{std::vector<std::uint8_t>bytes;};
Original originalStorage;Original*original=&originalStorage;
std::uintptr_t game=0x440000;
unsigned readCalls{},prepareCalls{};bool mutateBody=false;
std::vector<std::uint8_t>ReadMemory(std::uintptr_t address,std::size_t size){
 ++readCalls;assert(address==game+native_producer_gate116::BodyRva&&size==native_producer_gate116::BodySize);
 std::vector<std::uint8_t>b(original->bytes.begin()+native_producer_gate116::BodyRva,original->bytes.end());
 if(mutateBody)b.back()^=1;return b;
}
void Require(bool value,const char*message){if(!value)throw std::runtime_error(message);}
std::uintptr_t OriginalCall(std::uintptr_t rva){assert(rva==0x6bd08c);return game+0x31e40;}
void Prepare(SafetyHookMid&h,std::uintptr_t address,void(*)(SafetyHookContext&)noexcept){
 assert(address==game+native_producer_gate116::FirstRva||address==game+native_producer_gate116::SecondRva);
 ++prepareCalls;h.constructed=true;
}
void Event(unsigned,const char*,const std::string&){SetLastError(9876);}
#include "NativeProducerGate116Runtime.inc"
__declspec(naked)void ActualNativeStack(std::uint32_t*){
 __asm{
  mov ecx,[esp+4]
  push ebp
  mov ebp,esp
  mov [ecx],ebp
  and esp,0fffffff0h
  sub esp,0d8h
  push esi
  push edi
  mov [ecx+4],esp
  mov esp,ebp
  pop ebp
  ret
 }
}
std::uint32_t NativePlaneFlags(float a,float b){
 std::uint32_t flags;
 __asm{
  movss xmm2,a
  movss xmm0,b
  comiss xmm2,xmm0
  pushfd
  pop eax
  mov flags,eax
 }
 return flags;
}
std::uint32_t NativeSphereFlags(std::uint32_t result){
 std::uint32_t flags;
 __asm{
  mov eax,result
  test eax,eax
  pushfd
  pop eax
  mov flags,eax
 }
 return flags;
}
void Preserved(bool first,SafetyHookContext&regs){
 const auto incoming=regs;SetLastError(0x7144116);_mm_setcsr(0x5f80);
 const auto csr=_mm_getcsr();unsigned short controlBefore,controlAfter;
 __asm{fnstcw controlBefore}
 NativeProducerGate116::Observe(first,regs);
 __asm{fnstcw controlAfter}
 assert(GetLastError()==0x7144116&&csr==_mm_getcsr()&&controlBefore==controlAfter);
 assert(!std::memcmp(&incoming,&regs,sizeof(regs)));
}
int main(){
 using namespace NativeProducerGate116;
 ShadowTrace34::events.reserve(2048);
 original->bytes.resize(native_producer_gate116::BodyRva+native_producer_gate116::BodySize);
 std::memcpy(original->bytes.data()+native_producer_gate116::FirstRva,native_producer_gate116::FirstBytes.data(),6);
 std::memcpy(original->bytes.data()+native_producer_gate116::SecondRva,native_producer_gate116::SecondBytes.data(),6);
 PrepareHooks();Enable();assert(!prepared&&!active&&readCalls==0&&prepareCalls==0);
 requested=true;mutateBody=true;PrepareHooks();assert(!prepared&&!active&&prepareCalls==0&&readCalls==1);
 mutateBody=false;PrepareHooks();Enable();assert(prepared&&active&&prepareCalls==2&&readCalls==2);
 SafetyHookContext regs{};std::array<std::uint32_t,2>stack{};ActualNativeStack(stack.data());
 regs.ebp=stack[0];regs.esp=stack[1];regs.esi=2;regs.ecx=0x41;regs.edx=0xdeadbeef;regs.eip=0x6bd068;
 regs.xmm6.f32[0]=528.4085f;regs.xmm5.f32[0]=195.7277f;regs.xmm4.f32[0]=27.3625f;
 regs.xmm1.f32[0]=50;regs.xmm2.f32[0]=-2;regs.xmm0.f32[0]=-50;
 for(auto pair:std::array<std::array<float,2>,4>{{{-51,-50},{-50,-50},{-2,-50},{0,-0.0f}}}){
  regs.xmm2.f32[0]=pair[0];regs.xmm0.f32[0]=pair[1];regs.eflags=NativePlaneFlags(pair[0],pair[1]);
  const auto count=ShadowTrace34::events.size();Preserved(true,regs);assert(ShadowTrace34::events.size()==count+2);
  assert(context.pending==bool(regs.eflags&1));
  if(context.pending){regs.eax=2;regs.eflags=NativeSphereFlags(2);Preserved(false,regs);assert(!context.pending&&ShadowTrace34::events.back().d==2);}
 }
 for(unsigned result:{0u,1u,2u}){
  regs.eflags=NativePlaneFlags(-51,-50);Preserved(true,regs);assert(context.pending);
  regs.eax=result;regs.eflags=NativeSphereFlags(result);Preserved(false,regs);
  assert(ShadowTrace34::events.back().stage==91&&ShadowTrace34::events.back().c==int(result==0));
 }
 const auto sampleCount=ShadowTrace34::events.size();
 for(unsigned gate=0;gate<4;++gate){
  if(gate==0)active=false;else if(gate==1)addonActive=false;else if(gate==2)CaptureDiagnostic44::sampleSources=false;else ShadowTrace34::enabled=false;
  context.pending=true;Preserved(true,regs);assert(!context.pending&&ShadowTrace34::events.size()==sampleCount);
  active=true;addonActive=true;CaptureDiagnostic44::sampleSources=true;ShadowTrace34::enabled=true;
 }
 busy=true;context.pending=true;Preserved(false,regs);assert(!context.pending&&ShadowTrace34::events.size()==sampleCount);busy=false;
 regs.esi=4;Preserved(true,regs);assert(ShadowTrace34::events.size()==sampleCount);
 regs.esi=2;regs.eflags=NativePlaneFlags(-51,-50);Preserved(true,regs);assert(context.pending);
 ++regs.esp;Preserved(false,regs);assert(!context.pending&&bindingRejected==1);
 assert(readCalls==2); // Startup body authentication only. Callbacks have no reader/query dependency.
 Disable();assert(!active&&!prepared&&!planeHook.enabled&&!sphereHook.enabled);
 puts("PASS116 real runtime callbacks: native x86 predicates/stack, unchanged registers/FP/LastError, OFF and lease, reentry and body guards; no callback reader calls.");
}
