#include "FreshPageReadable.hpp"
#include <cassert>
#include <cstdio>
static unsigned queries=0,regions=0;
BOOL WINAPI CountQuery(HANDLE h,PVOID b,DWORD n){++queries;return QueryWorkingSetEx(h,b,n);}
bool Region(uintptr_t a,size_t n) noexcept {
 ++regions;MEMORY_BASIC_INFORMATION info{};
 if(a<0x10000||!n||a>UINTPTR_MAX-n||!VirtualQuery(reinterpret_cast<void*>(a),&info,sizeof(info))||info.State!=MEM_COMMIT||(info.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;
 switch(info.Protect&0xffu){case PAGE_READONLY:case PAGE_READWRITE:case PAGE_WRITECOPY:case PAGE_EXECUTE_READ:case PAGE_EXECUTE_READWRITE:case PAGE_EXECUTE_WRITECOPY:break;default:return false;}
 const auto base=uintptr_t(info.BaseAddress);
 return a>=base&&a-base<=info.RegionSize&&n<=info.RegionSize-(a-base);
}
int main(){
 using namespace traffic_signal::fresh_page;
 auto* data=static_cast<unsigned char*>(VirtualAlloc(nullptr,8192,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(data);data[0]=1;
 Configure(true);const auto fp=_mm_getcsr();SetLastError(923);
 assert(Readable(uintptr_t(data),16,Region,CountQuery));assert(regions==1&&queries==0);assert(_mm_getcsr()==fp&&GetLastError()==923);
 DWORD old{};assert(VirtualProtect(data,4096,PAGE_NOACCESS,&old));assert(!Readable(uintptr_t(data),16,Region,CountQuery));
 assert(VirtualProtect(data,4096,PAGE_READWRITE|PAGE_GUARD,&old));assert(!Readable(uintptr_t(data),16,Region,CountQuery));MEMORY_BASIC_INFORMATION info{};VirtualQuery(data,&info,sizeof(info));assert(info.Protect&PAGE_GUARD);
 assert(VirtualProtect(data,4096,PAGE_EXECUTE,&old));assert(!Readable(uintptr_t(data),16,Region,CountQuery));
 assert(VirtualProtect(data,4096,PAGE_READWRITE,&old));assert(VirtualProtect(data+4096,4096,PAGE_NOACCESS,&old));assert(!Readable(uintptr_t(data)+4090,16,Region,CountQuery));
 assert(VirtualProtect(data+4096,4096,PAGE_READWRITE,&old));assert(Readable(uintptr_t(data)+4090,16,Region,CountQuery));
 assert(VirtualFree(data+4096,4096,MEM_DECOMMIT));assert(!Readable(uintptr_t(data)+4096,16,Region,CountQuery));
 const auto before=regions;assert(!Readable(UINTPTR_MAX-7,16,Region,CountQuery));assert(!Readable(uintptr_t(data),0,Region,CountQuery));assert(regions==before);
 Configure(false);assert(Readable(uintptr_t(data),16,Region,CountQuery));assert(queries==1);
 VirtualFree(data,0,MEM_RELEASE);
 std::puts("fresh region31: current protection, guard preservation, execute-only, crossing, decommit, overflow, FP/LastError and previous-path override passed");
}
