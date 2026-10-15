#pragma once
#include <windows.h>
#include <psapi.h>
#include <intrin.h>
#include <cstdint>
#include "core/ShadowCpuProfile29.hpp"
#include "MemoryReadProfile46.hpp"
#pragma comment(lib,"psapi.lib")
namespace traffic_signal::fresh_page {
using Check=bool(*)(uintptr_t,size_t) noexcept;
// Startup strategy only; no permission or owner result is cached.
inline bool preferRegion=false;
inline void Configure(bool requested) noexcept {preferRegion=requested;}
using Query=BOOL(WINAPI*)(HANDLE,PVOID,DWORD);
// Positive permissions only, borrowed within one SEH-contained read-only hook.
// No copied data, owner result or authorization survives return to native code.
struct Operation44;
inline thread_local Operation44* operation44=nullptr;
struct Operation44 {
 struct Entry {uintptr_t page{};Query query{};};
 static constexpr unsigned Maximum147=32;
 Operation44* previous;Entry entries[Maximum147]{};unsigned count{},capacity=8;
 explicit Operation44(unsigned requestedCapacity=8) noexcept:previous(operation44),capacity(requestedCapacity>Maximum147?Maximum147:requestedCapacity){memory_read46::Record(memory_read46::Metric::Operations);if(previous)memory_read46::Record(memory_read46::Metric::NestedOperations);operation44=this;}
 ~Operation44() noexcept{operation44=previous;}
 Operation44(const Operation44&)=delete;Operation44& operator=(const Operation44&)=delete;
};
inline bool Known44(uintptr_t address,size_t size,DWORD page,Query query) noexcept {
 if(!operation44||!page||(page&(page-1))||address/page!=(address+size-1)/page)return false;
 for(unsigned i=0;i<operation44->count;++i)if(operation44->entries[i].page==address/page&&operation44->entries[i].query==query)return true;
 return false;
}
inline void Remember44(uintptr_t address,DWORD page,Query query) noexcept {
 if(!operation44||operation44->count>=operation44->capacity)return;
 for(unsigned i=0;i<operation44->count;++i)if(operation44->entries[i].page==address/page&&operation44->entries[i].query==query)return;
 operation44->entries[operation44->count++]={address/page,query};
}
struct Preserve {
 alignas(16) unsigned char state[512];DWORD error;
 Preserve() noexcept {_fxsave(state);error=GetLastError();}
 ~Preserve() noexcept {SetLastError(error);_fxrstor(state);}
};
inline DWORD PageSize() noexcept {
 static const DWORD value=[]{SYSTEM_INFO info;GetSystemInfo(&info);return info.dwPageSize;}();
 return value;
}
__declspec(noinline) inline bool Readable(uintptr_t address,size_t size,Check fallback,Query query=QueryWorkingSetEx) noexcept {
 const Preserve preserve;
 memory_read46::Record(memory_read46::Metric::ScalarHelpers);
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::PageCheck,256);
 if(address<0x10000||!size||address>UINTPTR_MAX-size)return false;
 // Fresh committed-region query; owner/lifetime checks remain at callers.
 if(preferRegion){memory_read46::Record(memory_read46::Metric::FallbackRequests);return fallback(address,size);}
 const auto page=PageSize();
 if(!page||(page&(page-1))||address/page!=(address+size-1)/page){memory_read46::Record(memory_read46::Metric::FallbackRequests);return fallback(address,size);}
 if(Known44(address,size,page,query)){memory_read46::Record(memory_read46::Metric::ScalarCacheHits);return true;}
 PSAPI_WORKING_SET_EX_INFORMATION info{};info.VirtualAddress=reinterpret_cast<void*>(address);
 memory_read46::Record(memory_read46::Metric::ScalarOSQueries);memory_read46::Record(memory_read46::Metric::QueriedPages);
 if(!query(GetCurrentProcess(),&info,sizeof(info))||!info.VirtualAttributes.Valid){memory_read46::Record(memory_read46::Metric::FallbackRequests);return fallback(address,size);}
 const auto protection=info.VirtualAttributes.Win32Protection;
 if(protection&(PAGE_GUARD|PAGE_NOACCESS))return false;
 // Reserved bits are unspecified and must be ignored. Reject supported exotic flags.
 if(info.VirtualAttributes.Locked||info.VirtualAttributes.LargePage||info.VirtualAttributes.Bad){memory_read46::Record(memory_read46::Metric::FallbackRequests);return fallback(address,size);}
 switch(protection){
 case PAGE_READONLY:case PAGE_READWRITE:case PAGE_WRITECOPY:
 case PAGE_EXECUTE_READ:case PAGE_EXECUTE_READWRITE:case PAGE_EXECUTE_WRITECOPY:
  Remember44(address,page,query);return true;
 default:{memory_read46::Record(memory_read46::Metric::FallbackRequests);return fallback(address,size);} // Includes execute-only and modifiers.
 }
}
}

