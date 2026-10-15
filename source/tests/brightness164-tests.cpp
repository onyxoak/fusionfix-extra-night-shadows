#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <atomic>
#include <cassert>
#include <cmath>
#include <limits>
#include <initializer_list>
#include <cstring>
#include <cstdio>
#include "safetyhook/context.hpp"
#include "core/standalone-game-abi.hpp"
#include "core/ShadowFloatingPointState.hpp"
using SafetyHookContext=safetyhook::Context32;
namespace HeadlightEnhancement {
uintptr_t imageBase{};std::atomic<uint64_t> lastDrivenToken{0};std::atomic<uint32_t> retainedSubmissions{0};
#include "headlight164-retain.inc"
}
#include "headlight164-hook.inc"
void Checked164(SafetyHookContext& r){
 const auto before=r;const DWORD error=0x164164;SetLastError(error);
 const auto mxcsr=_mm_getcsr();unsigned short cw{},sw{},afterCw{},afterSw{};
 __asm fnstcw cw
 __asm fnstsw sw
 Brightness(r);
 __asm fnstcw afterCw
 __asm fnstsw afterSw
 assert(GetLastError()==error&&_mm_getcsr()==mxcsr&&cw==afterCw&&sw==afterSw);
 auto actual=r;actual.xmm1=before.xmm1;actual.xmm4=before.xmm4;assert(!memcmp(&actual,&before,sizeof(actual)));
}
int main(){
 addonActive=true;
 using namespace HeadlightEnhancement;
 auto* body=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x2000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(body);
 auto* driver=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x1000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(driver);
 imageBase=reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr,0xc40000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(imageBase);
 uint8_t reference=7;rage::VehiclePool pool{body,&reference,1,0x2000,0,0};auto* poolPtr=&pool;CVehicle::pool=&poolPtr;
 *reinterpret_cast<uintptr_t*>(body+0xf50)=reinterpret_cast<uintptr_t>(driver);driver[0x218]=0;driver[0x219]=1;
 *reinterpret_cast<uint8_t*>(imageBase+0xc3cc4a)=1;
 *reinterpret_cast<float*>(imageBase+0xc3cc74)=2;
 *reinterpret_cast<float*>(imageBase+0xc3cc78)=3;
 SafetyHookContext r{};r.esi=reinterpret_cast<uintptr_t>(body);r.xmm1.f32[0]=8;r.xmm4.f32[0]=30;
 Checked164(r);const float occupied=r.xmm1.f32[0];assert(occupied==8*.85f&&r.xmm4.f32[0]==30&&lastDrivenToken);
 *reinterpret_cast<uintptr_t*>(body+0xf50)=0;r.xmm1.f32[0]=4;r.xmm4.f32[0]=10;
 Checked164(r);assert(r.xmm1.f32[0]==occupied&&r.xmm4.f32[0]==30&&retainedSubmissions==1);
 // Same parked car emits the same final intensity on each new native call.
 r.xmm1.f32[0]=4;r.xmm4.f32[0]=10;Checked164(r);assert(r.xmm1.f32[0]==occupied&&retainedSubmissions==2);
 reference=8;r.xmm1.f32[0]=4;r.xmm4.f32[0]=10;Checked164(r);assert(r.xmm1.f32[0]==4*.85f&&r.xmm4.f32[0]==10);
 reference=7;*reinterpret_cast<uint8_t*>(imageBase+0xc3cc4a)=0;r.xmm1.f32[0]=4;Checked164(r);assert(r.xmm1.f32[0]==4*.85f);
 // Unrelated occupied NPC preserves native multiplier and receives one dim.
 *reinterpret_cast<uintptr_t*>(body+0xf50)=reinterpret_cast<uintptr_t>(driver);driver[0x218]=1;r.xmm1.f32[0]=12;r.xmm4.f32[0]=15;Checked164(r);assert(r.xmm1.f32[0]==12*.85f&&!lastDrivenToken&&r.xmm4.f32[0]==15);
 // High-beam scaling remains downstream and commutes with one dim.
 assert(occupied*1.5f==(8*.85f)*1.5f);
 for(float invalid:{-2.f,std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()}){r.xmm1.f32[0]=invalid;const auto beforeInvalid=r;Checked164(r);assert(!memcmp(&r,&beforeInvalid,sizeof(r)));}
 addonActive=false;const auto before=r;Checked164(r);assert(!memcmp(&r,&before,sizeof(r)));
 CVehicle::pool=nullptr;VirtualFree(body,0,MEM_RELEASE);VirtualFree(driver,0,MEM_RELEASE);VirtualFree(reinterpret_cast<void*>(imageBase),0,MEM_RELEASE);
 puts("actual164 Brightness and Retain production: occupied/exit/NPC/generation/off PASS");
}
