#include <windows.h>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include "ModelNativeAssociation77.hpp"
#include "NativeBytes77.hpp"
// This x86 fixture executes the authenticated native wrapper, actual generic
// prologue/flags-writer/constructor-argument fragments, and native epilogue.
// It records the actual call stack; it does not manufacture a Binding with
// the production ReturnSlot formula. Native culling helpers are not executed.
static uint32_t measuredArguments77,measuredSlot77,measuredDestination77;
static uint32_t measuredFlags77,measuredPosition77,measuredColor77,measuredType77,measuredKey77;
__declspec(naked) static void Probe77() {
 __asm {
  mov eax,esp
  mov measuredSlot77,eax
  mov measuredDestination77,ecx
  mov eax,[esp+4]
  mov measuredType77,eax
  mov eax,[esp+8]
  mov measuredFlags77,eax
  mov eax,[esp+14h]
  mov measuredPosition77,eax
  mov eax,[esp+18h]
  mov measuredColor77,eax
  mov eax,[esp+44h]
  mov measuredKey77,eax
  ret 44h
 }
}
template<size_t N> static void Append77(std::vector<uint8_t>& code,const std::array<uint8_t,N>& bytes){code.insert(code.end(),bytes.begin(),bytes.end());}
static void Word77(std::vector<uint8_t>& code,uint32_t value){for(unsigned i=0;i<4;++i)code.push_back(uint8_t(value>>(i*8)));}
static void Capture77(std::vector<uint8_t>& code){
 // MOV EAX,ESP / ADD EAX,4 / MOV [measuredArguments77],EAX.
 code.insert(code.end(),{0x8b,0xc4,0x83,0xc0,0x04,0xa3});Word77(code,uint32_t(reinterpret_cast<uintptr_t>(&measuredArguments77)));
}
static uint8_t* Seal77(const std::vector<uint8_t>& code){
 auto* out=static_cast<uint8_t*>(VirtualAlloc(nullptr,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(out&&code.size()<4096);
 std::memcpy(out,code.data(),code.size());return out;
}
static void RelativeCall77(uint8_t* code,size_t offset,const void* target){
 assert(code[offset]==0xe8);const auto relative=uint32_t(reinterpret_cast<uintptr_t>(target)-reinterpret_cast<uintptr_t>(code+offset+5));std::memcpy(code+offset+1,&relative,4);
}
static void Protect77(uint8_t* code){DWORD old{};assert(VirtualProtect(code,4096,PAGE_EXECUTE_READ,&old));assert(FlushInstructionCache(GetCurrentProcess(),code,4096));}
static uint8_t* Generic77(bool capture,unsigned classification){
 using namespace native_bytes77;std::vector<uint8_t> code;if(capture)Capture77(code);
 Append77(code,Prologue);Append77(code,Allocate);Append77(code,SaveRegs);Append77(code,FlagsLoad);
 if(classification==0x20)Append77(code,Class20);else if(classification==0x40)Append77(code,Class40);else {assert(classification==0x60);Append77(code,Class60);}
 Append77(code,FlagsStore);Append77(code,LoadPosition);Append77(code,LoadFlags);Append77(code,LoadRoom);
 const auto start=code.size();Append77(code,CtorArgs);Append77(code,Epilogue);
 auto* out=Seal77(code);RelativeCall77(out,start+CtorArgs.size()-5,reinterpret_cast<void*>(Probe77));Protect77(out);return out;
}
static uint8_t* Wrapper77(uint8_t* generic){
 std::vector<uint8_t> code;Capture77(code);const auto prefix=code.size();Append77(code,native_bytes77::Wrapper);
 auto* out=Seal77(code);RelativeCall77(out,prefix+0x76,generic);Protect77(out);return out;
}
static void Invoke77(const uint32_t* args,unsigned words,unsigned padding,void* target){
 __asm {
  mov eax,padding
  sub esp,eax
  mov ecx,words
  mov edx,args
 pushLoop77:
  dec ecx
  push dword ptr [edx+ecx*4]
  test ecx,ecx
  jnz pushLoop77
  mov eax,target
  call eax
  mov eax,words
  shl eax,2
  add esp,eax
  mov eax,padding
  add esp,eax
 }
}
int main(){
 unsigned cases{};
 for(bool wrapper:{false,true})for(unsigned classification:{0x20u,0x40u,0x60u}){
  auto* generic=Generic77(!wrapper,classification);auto* entry=wrapper?Wrapper77(generic):generic;
  for(uint32_t type:{0u,2u})for(unsigned padding:{0u,4u,8u,12u})for(uint32_t flags:{1u,17u,65u,81u,9u,0x104u}){
   std::array<uint32_t,18> args{};args[1]=type;args[2]=flags;args[5]=0x11223344;args[6]=0x55667788;
   const auto words=wrapper?16u:18u;args[words-1]=0x12340000;
   Invoke77(args.data(),words,padding,entry);
   const auto expected=model_native_association77::ReturnSlot(measuredArguments77,wrapper);
   assert(measuredSlot77==expected&&measuredDestination77==expected+0xa8);
   assert(measuredPosition77==args[5]&&measuredColor77==args[6]&&measuredType77==type&&measuredKey77==args[words-1]);
   assert(model_native_association77::SameFlags(flags,measuredFlags77,wrapper));
   const auto preNormalized=wrapper?(flags&~8u):flags;
   if((preNormalized&0x60u)!=classification)assert(measuredFlags77!=preNormalized); // Reproduces old strict-match rejection.
   for(unsigned bit=0;bit<32;++bit)if(!(model_native_association77::ClassificationBits&(1u<<bit)))
    assert(!model_native_association77::SameFlags(flags,measuredFlags77^(1u<<bit),wrapper));
   ++cases;
  }
  if(wrapper)assert(VirtualFree(entry,0,MEM_RELEASE));assert(VirtualFree(generic,0,MEM_RELEASE));
 }
 std::printf("77 actual native x86 stack/flags fragments: %u cases passed; original strict flag match reproduced; full renderer/live validation pending\n",cases);
}
