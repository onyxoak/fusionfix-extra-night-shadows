// Offline x86 instruction harness. Executes authenticated native dispatch bytes;
// external LOD/getter/owner leaves and GPU drawable methods are controlled stubs.
// This checks actual CPU routing, not a GPU draw or rendered shadow.
#include "native-dispatch197c-bytes.hpp"
namespace Native197c {
using namespace standalone;
struct Saved {uint32_t edi,esi,ebp,esp,ebx,edx,ecx,eax;};
bool repair{},forced=true,old157{};unsigned calls{},slot{},mask{},selectedLod{};
uint32_t callbackMode{},drawLod{},drawFilter{};
void* drawThis{};
void __cdecl Forward(Saved* saved) {
 if(!repair)return;
 SafetyHookContext r;r.edi=saved->edi;r.esi=saved->esi;r.ebp=saved->ebp;r.esp=saved->esp+8;
 r.ebx=saved->ebx;r.edx=saved->edx;r.ecx=saved->ecx;r.eax=saved->eax;
 const auto previous=fixture134::actualStack157;fixture134::actualStack157=r.esp;
 PoleDepthRepair157::Dispatch(r);
 if(old157&&*reinterpret_cast<uint8_t*>(r.esp+0x30))r.eax|=1;
 fixture134::actualStack157=previous;
 saved->eax=uint32_t(r.eax);saved->ebp=uint32_t(r.ebp);saved->ebx=uint32_t(r.ebx);
}
__declspec(naked) void Mid() {
 __asm {
  pushfd
  pushad
  mov eax,esp
  push eax
  call Forward
  add esp,4
  popad
  popfd
  mov edx,[esp+14h]
  mov ecx,[esp+2ch]
  ret
 }
}
void __stdcall Lod(void*,uint32_t* index,uint32_t* a,uint32_t* b,float) {*index=selectedLod;*a=0;*b=0;}
uint32_t __cdecl Getter(){return forced?1u:0u;}
void __fastcall Regular(void* self,void*,uint32_t,uint32_t,uint32_t filter,uint32_t lod) {++calls;slot=0x28;mask=0;drawThis=self;drawLod=lod;drawFilter=filter;}
void __fastcall Simple(void* self,void*,uint32_t,uint32_t filter,uint32_t lod,uint32_t) {++calls;slot=0x20;mask=0;drawThis=self;drawLod=lod;drawFilter=filter;}
void __fastcall DepthSimple(void* self,void*,uint32_t,uint32_t lod,uint32_t filter,uint32_t) {++calls;slot=0x2c;mask=2;drawThis=self;drawLod=lod;drawFilter=filter;}
void __fastcall Depth(void* self,void*,uint32_t,uint32_t,uint32_t lod,uint32_t bits,uint32_t filter) {++calls;slot=0x30;mask=bits;drawThis=self;drawLod=lod;drawFilter=filter;}
void __fastcall Owner(void*,void*,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t mode){++calls;slot=0x80;callbackMode=mode;}
// The byte tests below are the exact native2A2003/2A200D/2A21B2 operands.
// Native fourth drawable argument is [EBP+14], not the entity command MODE.
__declspec(naked) unsigned __cdecl GeometryBranch(const uint8_t*,unsigned,unsigned,unsigned) {
 __asm {
  push ebp
  mov ebp,esp
  mov eax,[ebp+8]
  cmp byte ptr[eax+19h],0
  je rigid
  test byte ptr[ebp+14h],1
  je rejected
  jmp accepted
 rigid:
  test byte ptr[ebp+14h],2
  je rejected
 accepted:
  mov eax,1
  pop ebp
  ret
 rejected:
  xor eax,eax
  pop ebp
  ret
 }
}
void RelCall(uint8_t* code,size_t at,uintptr_t target) {
 assert(code[at]==0xe8);const auto delta=uint32_t(target-uintptr_t(code+at+5));std::memcpy(code+at+1,&delta,4);
}
void Check(uint32_t command) {
 auto* code=static_cast<uint8_t*>(VirtualAlloc(nullptr,sizeof(NativeDispatchBytes197c),MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(code);
 std::memcpy(code,NativeDispatchBytes197c,sizeof(NativeDispatchBytes197c));
 RelCall(code,0x2c,uintptr_t(&Lod));RelCall(code,0x64,uintptr_t(&Getter));
 RelCall(code,0xcb,uintptr_t(&Owner));RelCall(code,0x156,uintptr_t(&Owner));
 assert(!std::memcmp(code+0x9a,"\x8b\x54\x24\x10\x8b\x4c\x24\x28",8));
 code[0x9a]=0xe8;RelCall(code,0x9a,uintptr_t(&Mid));code[0x9f]=code[0xa0]=code[0xa1]=0x90;
 DWORD previous{};assert(VirtualProtect(code,sizeof(NativeDispatchBytes197c),PAGE_EXECUTE_READ,&previous));assert(FlushInstructionCache(GetCurrentProcess(),code,sizeof(NativeDispatchBytes197c)));
 using Fn=uint32_t(__fastcall*)(void*,void*,void*,uint32_t,uint32_t,uint32_t,float);
 const auto run=reinterpret_cast<Fn>(code);
 std::array<uint32_t,13> vt{};vt[8]=uint32_t(uintptr_t(&Simple));vt[10]=uint32_t(uintptr_t(&Regular));vt[11]=uint32_t(uintptr_t(&DepthSimple));vt[12]=uint32_t(uintptr_t(&Depth));
 std::array<uint32_t,32> drawable{};drawable[0]=uint32_t(uintptr_t(vt.data()));for(unsigned i=16;i<20;++i)drawable[i]=0x12340000+i;
 std::array<uint32_t,4> context{};context[2]=11;
 std::array<uint8_t,32> model{};
 unsigned decisions=0;
 for(unsigned mode=0;mode<4;++mode)for(unsigned f=0;f<2;++f)for(unsigned pointers=0;pointers<3;++pointers){
  repair=false;forced=f!=0;context[3]=mode;calls=slot=mask=0;selectedLod=2;
  run(context.data(),nullptr,drawable.data(),0x1234,pointers==1?0x2345:0,pointers==2?0x3456:0,1.f);
  const bool depth=forced&&(mode&3),skin=depth&&(mode&2);
  if(!pointers&&skin){assert(calls==0);continue;}
  assert(calls==1&&drawThis==drawable.data()&&drawLod==2&&drawFilter==11);
  assert(slot==(depth?(pointers?0x30u:0x2cu):(pointers?0x28u:0x20u)));
  if(depth&&pointers)assert(mask==(skin?1u:2u));++decisions;
 }
 context[3]=0;forced=true;repair=true;
 for(unsigned pointers=0;pointers<3;++pointers){
  calls=slot=mask=0;selectedLod=1;
  run(context.data(),nullptr,drawable.data(),0x1234,pointers==1?0x2345:0,pointers==2?0x3456:0,1.f);
  assert(calls==1&&slot==(pointers?0x30u:0x2cu)&&mask==2&&context[3]==0&&drawLod==1&&drawFilter==11);
  uint32_t mode{};assert(fixture134::Read(command+0x54,&mode,4)&&mode==0);
  for(unsigned branch=0;branch<256;++branch){model[0x19]=uint8_t(branch);assert(GeometryBranch(model.data(),0,0,mask)==unsigned(branch==0));}
  // Historical157/197 reaches the wrong geometry class, or no method at all.
  calls=slot=mask=0;old157=true;
  run(context.data(),nullptr,drawable.data(),0x1234,pointers==1?0x2345:0,pointers==2?0x3456:0,1.f);
  old157=false;
  if(!pointers)assert(calls==0);else{assert(calls==1&&slot==0x30&&mask==1);model[0x19]=0;assert(!GeometryBranch(model.data(),0,0,mask));}
 }
 // Actual native LOD return occurs before the production MID/counters.
 const auto before=PoleDepthRepair157::dispatch197.load();selectedLod=3;drawable[19]=0;calls=0;
 run(context.data(),nullptr,drawable.data(),0x1234,0,0x3456,1.f);assert(calls==0&&PoleDepthRepair157::dispatch197.load()==before);
 // All branch-byte/argument combinations: masks are mutually exclusive for1/2.
 for(unsigned branch=0;branch<256;++branch)for(unsigned bits=0;bits<4;++bits){model[0x19]=uint8_t(branch);assert(GeometryBranch(model.data(),0,0,bits)==unsigned(bool(bits&(branch?1:2))));}
 repair=false;forced=true;assert(VirtualFree(code,0,MEM_RELEASE));
 std::cout<<"PASS197c native instruction dispatch: "<<decisions<<" original branches, repaired rigid/pointer routes, old157 skips, LOD early return,1024 geometry mask cases; no GPU proof\n";
}
}
