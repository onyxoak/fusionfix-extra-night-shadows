#pragma once
#include <windows.h>
#include <cstdint>

namespace native_projection66 {
struct ReadPermission {bool allowed{};unsigned queries{};};
using RegionQuery=SIZE_T(WINAPI*)(LPCVOID,PMEMORY_BASIC_INFORMATION,SIZE_T);
struct ErrorPreserve {DWORD saved=GetLastError();~ErrorPreserve() noexcept{SetLastError(saved);}};
inline bool AllowedRegion(const MEMORY_BASIC_INFORMATION& info) noexcept {
 if(info.State!=MEM_COMMIT||(info.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return false;
 switch(info.Protect&0xffu){
 case PAGE_READONLY:case PAGE_READWRITE:case PAGE_WRITECOPY:
 case PAGE_EXECUTE_READ:case PAGE_EXECUTE_READWRITE:case PAGE_EXECUTE_WRITECOPY:return true;
 default:return false;}
}
// One synchronous contiguous packet/bank operation, never a cross-callback
// permission certificate. Walk legal region boundaries and reject guard pages
// before any access, preserving their native observable protection state.
inline ReadPermission Preflight(uintptr_t address,size_t size,
 RegionQuery query=VirtualQuery) noexcept {
 const ErrorPreserve preserve;ReadPermission result{};
 if(address<0x10000||!size||size>81920||address>UINTPTR_MAX-size||!query)return result;
 const auto end=address+size;
 while(address<end&&result.queries<64){MEMORY_BASIC_INFORMATION info{};++result.queries;
  if(!query(reinterpret_cast<void*>(address),&info,sizeof(info))||!AllowedRegion(info))return result;
  const auto base=reinterpret_cast<uintptr_t>(info.BaseAddress);
  if(base>address||info.RegionSize>UINTPTR_MAX-base)return result;
  const auto next=base+info.RegionSize;if(next<=address)return result;address=next;
 }
 result.allowed=address>=end;return result;
}
}
