#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <intrin.h>
#include <iostream>
uintptr_t ff=0x50000000;
std::atomic<bool> addonActive{true};
namespace FusionBackend130{bool prepared=true;HMODULE pinnedModule=reinterpret_cast<HMODULE>(ff);struct Lease{bool owns=true;bool Owns()const noexcept{return owns;}}lease;}
namespace PlayerShadowAllocation{std::atomic<DWORD> ownerThread{GetCurrentThreadId()};}
namespace PoleStaticPartition141{
bool requested=true,prepared=true;
struct Binding{bool valid=true;DWORD thread=GetCurrentThreadId();}binding;
#include "caster-gate224.inc"
}
int main(){
 using namespace PoleStaticPartition141;
 unsigned cases=0;
 for(unsigned flags=0;flags<512;++flags)for(unsigned type:{0u,1u,2u,3u,4u,5u,0xffffffffu})for(unsigned mode:{0u,1u,2u}){
  requested=flags&1;prepared=flags&2;binding.valid=flags&4;addonActive=bool(flags&8);
  FusionBackend130::prepared=flags&16;FusionBackend130::lease.owns=flags&32;
  FusionBackend130::pinnedModule=reinterpret_cast<HMODULE>((flags&64)?ff:ff+4);
  binding.thread=(flags&128)?GetCurrentThreadId():GetCurrentThreadId()+1;
  PlayerShadowAllocation::ownerThread=(flags&256)?GetCurrentThreadId():GetCurrentThreadId()+2;
  alignas(16) unsigned char before[512]{},after[512]{};
  __asm fld1
  SetLastError(224);_fxsave(before);
  const bool result=NeedsSlowAdmission224(1,type,mode);
  _fxsave(after);__asm fstp st(0)
  assert(!std::memcmp(before,after,160));
  assert(GetLastError()==224);
  assert(result==(flags==511&&type==4&&mode==0));++cases;
 }
 std::cout<<"PASS production caster gate: "<<cases<<" feature/pass/thread/ownership cases; no object or stack reads, no native admission authorized.\n";
}
