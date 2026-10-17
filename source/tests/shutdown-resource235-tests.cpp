#include "ShutdownResourceGuard235.hpp"
#include "standalone-pe.hpp"
#include <safetyhook.hpp>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <limits>
unsigned firstCalls{},secondCalls{};int firstIndex{},secondIndex{};int cookie=6;
extern "C" void __cdecl First(int index,int c){assert(c==6);++firstCalls;firstIndex=index;}
extern "C" void __cdecl Second(int index){++secondCalls;secondIndex=index;}
__declspec(naked) void __cdecl Invoke(void* record){
 __asm {
  push esi
  push edi
  mov edi,[esp+12]
  movsx ecx,word ptr [edi+52h]
  push cookie
  push ecx
  call First
  movsx eax,word ptr [edi+52h]
  push eax
  call Second
  add esp,0ch
  add esi,110h
  pop edi
  pop esi
  ret
 }
}
uintptr_t resume{};unsigned rejected{};
void Guard(SafetyHookContext& c){if(shutdown_resource235::Apply(c,resume))++rejected;}
int main(int argc,char**argv){
 using namespace shutdown_resource235;
 assert(Invalid(0xffffffffu)&&Invalid(0xffff8000u)&&!Invalid(0)&&!Invalid(32767));
 SafetyHookContext context{};context.ecx=0xffffffffu;context.esp=123;context.eflags=0x246;context.edi=456;context.eip=789;
 const auto before=context;assert(Apply(context,999));auto after=context;after.eip=before.eip;assert(!memcmp(&after,&before,sizeof(before)));
 context.ecx=32767;const auto valid=context;assert(!Apply(context,999));assert(!memcmp(&context,&valid,sizeof(valid)));
 assert(argc==2);
 onyxoak::pe::Image game(onyxoak::pe::Read(argv[1]));const auto rva=game.Find(Pattern);
 assert(Shape(game.bytes.data()+rva,BodyBytes));
 for(size_t i=0;i<BodyBytes;++i){auto bytes=std::array<unsigned char,BodyBytes>{};memcpy(bytes.data(),game.bytes.data()+rva,BodyBytes);bytes[i]^=1;assert(Shape(bytes.data(),bytes.size())==Wildcard(i));}
 // Execute the real x86 stack/calling-convention shape with and without the
 // mid hook; verify both native calls and balanced stack across edge indices.
 auto* code=reinterpret_cast<unsigned char*>(&Invoke);size_t at=0;
 for(size_t i=0;i<64;++i)if(!memcmp(code+i,"\x0f\xbf\x4f\x52\xff\x35",6)){at=i;break;}
 assert(at);resume=reinterpret_cast<uintptr_t>(code+at+ResumeOffset);
 assert(!memcmp(code+at+26,"\x83\xc4\x0c",3));
 alignas(16) unsigned char record[0x60]{};
 for(int i:{-1,0,32767}){*reinterpret_cast<short*>(record+0x52)=short(i);Invoke(record);assert(firstIndex==i&&secondIndex==i);}
 const auto initial=firstCalls;
 auto hook=safetyhook::create_mid(code+at+HookOffset,Guard);assert(hook);
 for(int i:{-32768,-1,0,1,32767}){*reinterpret_cast<short*>(record+0x52)=short(i);Invoke(record);}
 assert(rejected==2&&firstCalls==initial+3&&secondCalls==firstCalls&&firstIndex==32767&&secondIndex==32767);
 assert(hook.disable());
 *reinterpret_cast<short*>(record+0x52)=-1;Invoke(record);assert(firstCalls==initial+4&&secondCalls==firstCalls);
 puts("shutdown235 PASS: real executable shape, mutation rejection, signed indices, live x86 hook, native argument/stack preservation");
}
