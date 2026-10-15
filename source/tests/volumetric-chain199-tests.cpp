#define NOMINMAX
#include <windows.h>
#include "VolumetricCompat199.hpp"
#include "standalone-pe.hpp"
#include "safetyhook.hpp"
#include <cassert>
#include <iostream>
namespace {
SafetyHookInline vh,fh;
unsigned vc{},fc{};void* expectedEdx{};void* expectedSource{};
void* __fastcall Volume(void* out,void* edx,void* source){
 assert(edx==expectedEdx&&source==expectedSource);++vc;
 auto* result=vh.unsafe_fastcall<uint32_t*>(out,edx,source);assert(result==out);
 result[18]|=8;result[28]=0x40800000;result[29]=0x3e99999a;return result;
}
void* __fastcall Fusion(void* out,void* edx,void* source){
 assert(edx==expectedEdx&&source==expectedSource);++fc;
 auto* result=fh.unsafe_fastcall<uint32_t*>(out,edx,source);assert(result==out);
 // Official snow handler may update the same three numeric volume fields.
 result[18]|=8;result[28]=0x40000000;result[29]=0x3e4ccccd;return result;
}
uint32_t Addr(const void* p){return uint32_t(reinterpret_cast<uintptr_t>(p));}
}
int main(int argc,char**argv){
 assert(argc==2);const onyxoak::pe::Image image(onyxoak::pe::Read(argv[1]));
 const auto* pristine=image.bytes.data()+native_copy70::CopyRva;
 auto* native=static_cast<uint8_t*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));assert(native);
 using Copy=void* (__fastcall*)(void*,void*,void*);
 for(bool volumeOuter:{false,true}){
  memcpy(native,pristine,native_copy70::CopySize);FlushInstructionCache(GetCurrentProcess(),native,native_copy70::CopySize);
  auto setupVolume=[&]{vh=safetyhook::create_inline(native,Volume);assert(vh);};
  auto setupFusion=[&]{fh=safetyhook::create_inline(native,Fusion);assert(fh);};
  if(volumeOuter){setupFusion();setupVolume();}else{setupVolume();setupFusion();}
  const std::array<uint32_t,2> va{Addr(vh.trampoline().data()),uint32_t(vh.trampoline().size())},fa{Addr(fh.trampoline().data()),uint32_t(fh.trampoline().size())};
  const volumetric199::Node v{Addr(reinterpret_cast<void*>(Volume)),Addr(va.data()),{reinterpret_cast<uint8_t*>(Volume),16}},
   f{Addr(reinterpret_cast<void*>(Fusion)),Addr(fa.data()),{reinterpret_cast<uint8_t*>(Fusion),16}};
  auto read=[](uint32_t a,size_t s){std::vector<uint8_t>b(s);SIZE_T got{};assert(ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(a),b.data(),s,&got)&&got==s);return b;};
  const auto chain=volumetric199::VerifyChain(Addr(native)-native_copy70::CopyRva,{pristine,native_copy70::CopySize},v,&f,read);
  assert(chain.outer==(volumeOuter?v.callback:f.callback));
  alignas(16) std::array<uint32_t,32> input{},output{};
  for(unsigned i=0;i<30;++i)input[i]=0x3f800000+i;
  expectedEdx=reinterpret_cast<void*>(0x12345678);expectedSource=input.data();vc=fc=0;
  SetLastError(0x156199);
  auto result=reinterpret_cast<Copy>(native)(output.data(),expectedEdx,input.data());
  assert(result==output.data()&&vc==1&&fc==1&&GetLastError()==0x156199);
  for(unsigned i=0;i<30;++i)if(i!=18&&i!=28&&i!=29)assert(output[i]==input[i]);
  assert(output[18]==(input[18]|8));assert(output[28]==(volumeOuter?0x40800000:0x40000000));
  assert(output[29]==(volumeOuter?0x3e99999a:0x3e4ccccd));
  if(volumeOuter){vh.reset();fh.reset();}else{fh.reset();vh.reset();}
  assert(!memcmp(native,pristine,native_copy70::CopySize));
 }
 assert(VirtualFree(native,0,MEM_RELEASE));
 std::cout<<"Executable native120-byte copier with real x86 SafetyHook chains: both load orders, one native copy, one callback each, ECX/EDX/source/RET4/EAX/LastError and rollback passed\n";
}
