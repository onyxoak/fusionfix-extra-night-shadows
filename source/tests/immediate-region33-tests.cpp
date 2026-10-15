#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <cassert>
#include <iostream>
#include "ImmediateRegion33.hpp"
unsigned queries{};
SIZE_T WINAPI CountQuery(LPCVOID p,PMEMORY_BASIC_INFORMATION i,SIZE_T n){++queries;return VirtualQuery(p,i,n);}
int main(){
 SYSTEM_INFO sys{};GetSystemInfo(&sys);auto page=sys.dwPageSize;
 auto memory=static_cast<unsigned char*>(VirtualAlloc(nullptr,2*page,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(memory);
 auto address=reinterpret_cast<uintptr_t>(memory);
 {
  immediate_region33::Scope operation;
  assert(immediate_region33::Readable(address,8,CountQuery));
  assert(immediate_region33::Readable(address+page,8,CountQuery));assert(queries==1);
  {immediate_region33::Scope nested;assert(immediate_region33::Readable(address,8,CountQuery));assert(queries==2);}
  assert(immediate_region33::Readable(address,8,CountQuery));assert(queries==2);
 }
 DWORD old{};assert(VirtualProtect(memory,page,PAGE_NOACCESS,&old));
 {immediate_region33::Scope fresh;assert(!immediate_region33::Readable(address,8,CountQuery));}
 assert(queries==3);assert(VirtualProtect(memory,page,PAGE_READWRITE|PAGE_GUARD,&old));
 {immediate_region33::Scope fresh;assert(!immediate_region33::Readable(address,8,CountQuery));}
 // A permission check must not consume the guard page.
 MEMORY_BASIC_INFORMATION info{};assert(VirtualQuery(memory,&info,sizeof(info)));assert(info.Protect&PAGE_GUARD);
 assert(!immediate_region33::Readable(address,2*page,CountQuery));
 assert(!immediate_region33::Readable(UINTPTR_MAX-2,8,CountQuery));
 assert(VirtualFree(memory,0,MEM_RELEASE));
 {immediate_region33::Scope fresh;assert(!immediate_region33::Readable(address,8,CountQuery));}
 assert(immediate_region33::current==nullptr);
 std::cout<<"Region reuse ends at call boundary; nested scope, revoked, guard, free, cross-region and overflow checks passed.\n";
}
