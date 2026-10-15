#include "source/ShadowAllocationCE.hpp"
#include "source/NativeCacheDependencies44.hpp"
#include <windows.h>
#include <array>
#include <cassert>
#include <cstring>
#include <random>
#include <iostream>
// Run only the audited pure cache-choice instructions in this OFFLINE Win32
// process, over our own array. No GTA process, hooks, native addresses or files.
int main(){
 using namespace fusionfix::shadows;
 const auto& original=ce::allocation::detail::CachePickerBytes;
 auto* code=static_cast<unsigned char*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));
 assert(code && sizeof(void*)==4);std::memcpy(code,original.data(),original.size());
 // Replace the external "shadows enabled?" call with true; all other
 // selection instructions remain byte-for-byte identical to the native code.
 const unsigned char enabled[]{0xb0,1,0x90,0x90,0x90};std::memcpy(code,enabled,sizeof(enabled));
 alignas(16) std::array<unsigned char,0x840> storage{};
 const auto first=reinterpret_cast<uint32_t>(storage.data()+0x10);
 const auto end=first+0x800;
 std::memcpy(code+0x1f,&first,4);std::memcpy(code+0x4e,&end,4);
 DWORD prior;assert(VirtualProtect(code,4096,PAGE_EXECUTE_READ,&prior));
 FlushInstructionCache(GetCurrentProcess(),code,original.size());
 auto native=reinterpret_cast<int(__cdecl*)(float)>(code);
 std::mt19937 rng(4419);std::array<NativeCacheEntry44,8> entries{};
 for(unsigned scenario=0;scenario<100000;++scenario){
  for(unsigned i=0;i<8;++i){
   auto& e=entries[i];e={float(rng()%10000)/100,int(rng()%1000)-100,i+1,int(rng()%5)-1};
   auto* age=storage.data()+0x10+i*0x100;
   std::memcpy(age-12,&e.distance,4);std::memcpy(age,&e.age,4);
   std::memcpy(age+4,&e.key,4);std::memcpy(age+8,&e.state,4);
  }
  const float distance=float(rng()%10000)/100;
  assert(native(distance)==NativeCacheChoice44(distance,entries));
 }
 assert(VirtualFree(code,0,MEM_RELEASE));
 std::cout<<"PASS: policy matches audited native cache-picker instructions on100000 independent snapshots\n";
}
