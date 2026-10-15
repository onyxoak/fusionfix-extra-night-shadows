#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <cassert>
#include <cstdio>
#include "EffectAttribution199C.hpp"
#include "standalone-pe.hpp"
#include "safetyhook.hpp"
#include "core/ShadowFloatingPointState.hpp"
unsigned callbacks{},copies{};
effect_attribution199c::Packet expected{};
void __cdecl Initialize(uint32_t*p){memcpy(p,expected.data(),120);}
void __cdecl NativeCopy(const uint32_t*p,float){++copies;assert(!memcmp(p,expected.data(),120));}
void Observe(SafetyHookContext&r){
 const auto incoming=r;const auto error=GetLastError();
 {const fusionfix::shadows::FloatingPointState fp;
  assert(r.esp==(r.ebp&~15u)-0xe8);auto*packet=*reinterpret_cast<const uint32_t**>(r.esp);
  assert(uintptr_t(packet)==r.esp+0x68&&!memcmp(packet,expected.data(),120));
  ++callbacks;_mm_setcsr(0x3f80);SetLastError(3);
 }
 SetLastError(error);assert(!memcmp(&r,&incoming,sizeof(r)));
}
int main(int argc,char**argv){
 assert(argc==2);onyxoak::pe::Image image(onyxoak::pe::Read(argv[1]));
 auto*code=static_cast<uint8_t*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));assert(code);
 size_t used=0;
 const auto native=[&](uint32_t rva,size_t size){memcpy(code+used,image.bytes.data()+rva,size);used+=size;};
 const auto bytes=[&](std::initializer_list<uint8_t> values){for(auto b:values)code[used++]=b;};
 const auto word=[&](uintptr_t value){uint32_t w=uint32_t(value);memcpy(code+used,&w,4);used+=4;};
 native(0x6bccd0,6);native(0x6bccde,6);native(0x6bcce8,2);
 bytes({0x8d,0x44,0x24,0x60,0x50,0xb8});word(uintptr_t(Initialize));bytes({0xff,0xd0,0x83,0xc4,0x04});
 native(0x6bd294,11);const auto call=used;bytes({0xe8});word(uintptr_t(NativeCopy)-uintptr_t(code+used+4));
 native(0x6bd2af,3);native(0x6bd2b9,6);
 FlushInstructionCache(GetCurrentProcess(),code,used);
 auto hook=SafetyHookMid::create(code+call,Observe,SafetyHookMid::StartDisabled);assert(hook&&hook->original_bytes().size()==5);
 expected[17]=0;expected[18]=0x261;expected[12]=0x3f800000;
 using Function=void(__cdecl*)();auto invoke=reinterpret_cast<Function>(code);
 invoke();assert(copies==1&&!callbacks);
 assert(hook->enable());for(unsigned i=0;i<16;++i){SetLastError(199);_mm_setcsr(0x5f80);invoke();assert(GetLastError()==199&&_mm_getcsr()==0x5f80);}
 assert(copies==17&&callbacks==16);assert(hook->disable());invoke();assert(copies==18&&callbacks==16);
 hook->reset();VirtualFree(code,0,MEM_RELEASE);
 puts("199c real x86 SafetyHookMid: native stack alignment/packet address, original call once, no extra copy, FP/LastError retained");
}
