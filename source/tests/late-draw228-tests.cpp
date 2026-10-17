#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "safetyhook.hpp"
namespace standalone {
uintptr_t game{};std::atomic<bool> addonActive{true};
namespace ReceiverCull226 {
std::atomic<bool> mode{false};bool prepared=true,experimental=true;
struct Binding{bool valid=true;}binding;
bool reject=false;unsigned visits=0;uintptr_t lastEntity=0;
bool Visit(uintptr_t p,uint32_t type)noexcept{assert(type==1);++visits;lastEntity=p;return reject;}
}
std::vector<uint8_t> ReadMemory(uintptr_t p,size_t n){return {reinterpret_cast<uint8_t*>(p),reinterpret_cast<uint8_t*>(p+n)};}
void Require(bool ok,const char* s){if(!ok)throw std::runtime_error(s);}
void Event(int,const char*,const std::string&){}
template<class Hook,class F>void Prepare(Hook& h,uintptr_t p,F f){h=safetyhook::create_mid(reinterpret_cast<void*>(p),f,safetyhook::MidHook::StartDisabled);Require(bool(h),"fixture_hook_create");}
#include "LateDrawCull228Runtime.inc"
}
unsigned drawn228{};std::array<uint32_t,4> args228{};
__declspec(naked) void Draw228(){__asm{
 mov eax,[esp+4]
 mov dword ptr [args228],eax
 mov eax,[esp+8]
 mov dword ptr [args228+4],eax
 mov eax,[esp+12]
 mov dword ptr [args228+8],eax
 mov eax,[esp+16]
 mov dword ptr [args228+12],eax
 inc dword ptr [drawn228]
 ret 16
}}
static void Jump228(uintptr_t p,uintptr_t target){*reinterpret_cast<uint8_t*>(p)=0xe9;const auto delta=uint32_t(target-p-5);memcpy(reinterpret_cast<void*>(p+1),&delta,4);}
static void Run228(unsigned i,uintptr_t* entity){
 using namespace standalone;using namespace LateDrawCull228;
 using Draw=void(__cdecl*)(uintptr_t*,uint32_t,uint32_t,uint32_t,uint32_t);
 const auto entry=game+0x6e3300;
 const std::array<uint8_t,7> prologue{0x55,0x8b,0xec,0x56,0x8b,0x75,0x08};
 memcpy(reinterpret_cast<void*>(entry),prologue.data(),prologue.size());Jump228(entry+7,game+Sites[i]);
 FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(game),0x700000);
 uintptr_t before{},after{};__asm mov before,esp
 reinterpret_cast<Draw>(entry)(entity,22,0x12345678,44,55);
 __asm mov after,esp
 assert(before==after&&GetLastError()==228);
}
int main(){
 using namespace standalone;using namespace LateDrawCull228;
 game=uintptr_t(VirtualAlloc(nullptr,0x700000,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));assert(game);
 std::array<uintptr_t,36> table{};table[35]=uintptr_t(Draw228);uintptr_t entity=uintptr_t(table.data());
 const std::array<uint8_t,13> draw{0x8b,0x06,0xff,0x75,0x10,0x8b,0xce,0xff,0x90,0x8c,0,0,0};
 memcpy(reinterpret_cast<void*>(game+0x6e3857),draw.data(),draw.size());
 const std::array<uint8_t,8> done{0x8b,0x55,0x14,0x5e,0x8b,0xe5,0x5d,0xc3};
 memcpy(reinterpret_cast<void*>(game+0x6e3864),done.data(),done.size());
 for(unsigned i=0;i<3;++i){
  const std::array<uint8_t,9> pushes{0x6a,0xff,0x68,0xff,0,0,0,0x6a,Modes[i]};
  memcpy(reinterpret_cast<void*>(game+Sites[i]),pushes.data(),pushes.size());
  if(Sites[i]+9!=0x6e3857)Jump228(game+Sites[i]+9,game+0x6e3857);
 }
 // Native reference first; then the actual production midhook callback,
 // including both an omitted virtual call and the displaced-push continuation.
 for(unsigned i=0;i<3;++i){SetLastError(228);Run228(i,&entity);assert((args228==std::array<uint32_t,4>{0x12345678,Modes[i],255,UINT32_MAX}));}
 PrepareHooks();assert(prepared);Enable();
 for(bool enabled:{false,true})for(bool rejected:{false,true})for(unsigned i=0;i<3;++i){
  ReceiverCull226::mode=enabled;ReceiverCull226::reject=rejected;ReceiverCull226::visits=0;
  const auto before=drawn228;SetLastError(228);Run228(i,&entity);
  assert(drawn228==before+unsigned(!(enabled&&rejected)));
  assert(ReceiverCull226::visits==unsigned(enabled));
  if(enabled)assert(ReceiverCull226::lastEntity==uintptr_t(&entity));
  if(!(enabled&&rejected))assert((args228==std::array<uint32_t,4>{0x12345678,Modes[i],255,UINT32_MAX}));
 }
 Disable();for(auto& h:hooks)h.reset();
 *reinterpret_cast<uint8_t*>(game+Sites[1])=0x90;
 ReceiverCull226::prepared=true;PrepareHooks();assert(!prepared&&!ReceiverCull226::prepared&&addonActive);
 assert(VirtualFree(reinterpret_cast<void*>(game),0,MEM_RELEASE));
 std::cout<<"PASS228 actual x86 production hooks: all three native pre-push sites, original RET16 arguments, disabled/accepted/rejected draws, unchanged stack/LastError, optional foreign-site refusal and rollback. Not FPS or pixel proof.\n";
}
