#include "FreshReadBatch30.hpp"
#include <cassert>
#include <array>
#include <cstdio>

unsigned queries,entriesSeen,fallbacks;void* revoke;
bool FreshRegion(uintptr_t address,size_t size) noexcept {
 ++fallbacks;MEMORY_BASIC_INFORMATION info{};
 if(address<0x10000||!size||address>UINTPTR_MAX-size||
    !VirtualQuery(reinterpret_cast<void*>(address),&info,sizeof(info))||info.State!=MEM_COMMIT||
    (info.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return false;
 const auto base=uintptr_t(info.BaseAddress);
 return address>=base&&address-base<=info.RegionSize&&size<=info.RegionSize-(address-base);
}
BOOL WINAPI CountQuery(HANDLE process,PVOID buffer,DWORD size){
 ++queries;entriesSeen=size/sizeof(PSAPI_WORKING_SET_EX_INFORMATION);
 const auto ok=QueryWorkingSetEx(process,buffer,size);
 if(revoke){DWORD old{};VirtualProtect(revoke,4096,PAGE_NOACCESS,&old);}
 SetLastError(1234);return ok;
}
BOOL WINAPI FailQuery(HANDLE,PVOID,DWORD){++queries;return FALSE;}
int main(){
 using namespace traffic_signal::fresh_batch30;
 auto* data=static_cast<unsigned char*>(VirtualAlloc(nullptr,4096*6,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(data);
 for(unsigned i=0;i<4096*6;++i)data[i]=static_cast<unsigned char>(i*7);
 std::array<std::array<unsigned char,12>,4> output{};
 Span spans[4]{};
 for(unsigned i=0;i<4;++i)spans[i]={uintptr_t(data)+i*4096,output[i].data(),12};
 const auto mxcsr=_mm_getcsr();SetLastError(8765);
 assert(Copy(spans,4,FreshRegion,CountQuery));assert(queries==1&&entriesSeen==4);
 assert(GetLastError()==8765&&_mm_getcsr()==mxcsr);
 for(unsigned i=0;i<4;++i)assert(!std::memcmp(output[i].data(),data+i*4096,12));
 for(unsigned i=0;i<4;++i)spans[i].address=uintptr_t(data)+i*24;
 queries=0;assert(Copy(spans,4,FreshRegion,CountQuery));assert(queries==1&&entriesSeen==1);
 // No authorization survives a call. A later protection change is refused.
 DWORD old{};VirtualProtect(data,4096,PAGE_NOACCESS,&old);
 for(auto& v:output)v.fill(99);
 assert(!Copy(spans,4,FreshRegion,CountQuery));for(auto& v:output)for(auto b:v)assert(b==99);
 VirtualProtect(data,4096,PAGE_READWRITE|PAGE_GUARD,&old);
 assert(!Copy(spans,4,FreshRegion,CountQuery));MEMORY_BASIC_INFORMATION mbi{};VirtualQuery(data,&mbi,sizeof(mbi));assert(mbi.Protect&PAGE_GUARD);
 VirtualProtect(data,4096,PAGE_READWRITE,&old);
 // Cross-page reads retain exact-span fallback; an inaccessible second page
 // cannot be authorized merely because the first page is readable.
 spans[0].address=uintptr_t(data)+4096-6;
 VirtualProtect(data+4096,4096,PAGE_NOACCESS,&old);
 assert(!Copy(spans,1,FreshRegion,CountQuery));
 VirtualProtect(data+4096,4096,PAGE_READWRITE,&old);
 assert(Copy(spans,1,FreshRegion,CountQuery));assert(!std::memcmp(output[0].data(),data+4096-6,12));
 // Query failure falls back to current region checks, never cached approval.
 spans[0].address=uintptr_t(data);fallbacks=0;
 assert(Copy(spans,4,FreshRegion,FailQuery));assert(fallbacks==4);
 // Mapping changes AFTER the query: SEH contains the second read fault and
 // leaves every output untouched despite staging the first read successfully.
 for(unsigned i=0;i<4;++i)spans[i].address=uintptr_t(data)+i*4096;
 for(auto& v:output)v.fill(88);revoke=data+4096;
 assert(!Copy(spans,4,FreshRegion,CountQuery));revoke=nullptr;
 for(auto& v:output)for(auto b:v)assert(b==88);
 VirtualProtect(data+4096,4096,PAGE_READWRITE,&old);
 VirtualFree(data+4096*3,4096,MEM_DECOMMIT);assert(!Copy(spans,4,FreshRegion,CountQuery));
 // Committed nonresident page is validated by fresh fallback and may fault in.
 auto* cold=VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);assert(cold);
 spans[0].address=uintptr_t(cold);assert(Copy(spans,1,FreshRegion,CountQuery));VirtualFree(cold,0,MEM_RELEASE);
 const auto before=queries;spans[0].address=UINTPTR_MAX-3;assert(!Copy(spans,1,FreshRegion,CountQuery));assert(queries==before);
 assert(!Copy(spans,5,FreshRegion,CountQuery));assert(!Copy(spans,0,FreshRegion,CountQuery));
 VirtualFree(data,0,MEM_RELEASE);
 std::puts("fresh batch: distinct/shared pages, exact span, current protection, guard preservation, fallback, raced fault, no partial outputs, LastError and FP passed");
}
