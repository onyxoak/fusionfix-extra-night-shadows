#pragma once
#include <windows.h>
#include <cstdint>
#include <psapi.h>
#include <intrin.h>
#pragma comment(lib,"psapi.lib")

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
struct PacketPermission203 {bool allowed{};unsigned queries{},workingSetQueries{},regionQueries{},pages{};};
using WorkingSetQuery203=BOOL(WINAPI*)(HANDLE,PVOID,DWORD);
using PageSizeQuery203=DWORD(*)() noexcept;
struct PacketPreserve203 {
 alignas(16) unsigned char state[512];DWORD error;
 PacketPreserve203() noexcept {_fxsave(state);error=GetLastError();}
 ~PacketPreserve203() noexcept {SetLastError(error);_fxrstor(state);}
};
inline DWORD PacketPageSize203() noexcept {
 static const DWORD value=[]{SYSTEM_INFO info{};GetSystemInfo(&info);return info.dwPageSize;}();return value;
}
// Exactly one current 120-byte packet. No Operation44/certificate lookup or
// publication: every invocation queries every covered page anew. Whole-bank
// validation keeps the original committed-region walker above unchanged.
inline PacketPermission203 PacketPreflight203(uintptr_t address,
 WorkingSetQuery203 query=QueryWorkingSetEx,RegionQuery region=VirtualQuery,
 PageSizeQuery203 pageSize=PacketPageSize203) noexcept {
 const PacketPreserve203 preserve;PacketPermission203 result{};
 constexpr size_t bytes=120;
 if(address<0x10000||address>UINTPTR_MAX-bytes)return result;
 const auto fallback=[&]() noexcept {
  const auto original=Preflight(address,bytes,region);result.allowed=original.allowed;
  result.regionQueries+=original.queries;result.queries+=original.queries;return result;
 };
 const auto page=pageSize?pageSize():0;
 if(!query||page<bytes||(page&(page-1)))return fallback();
 const uintptr_t first=address&~uintptr_t(page-1),last=(address+bytes-1)&~uintptr_t(page-1);
 PSAPI_WORKING_SET_EX_INFORMATION info[2]{};
 info[0].VirtualAddress=reinterpret_cast<void*>(first);
 const unsigned count=first==last?1u:2u;
 if(count==2)info[1].VirtualAddress=reinterpret_cast<void*>(last);
 ++result.workingSetQueries;++result.queries;result.pages=count;
 if(!query(GetCurrentProcess(),info,count*sizeof(info[0])))return fallback();
 bool needRegion=false;
 for(unsigned i=0;i<count;++i){
  const auto a=info[i].VirtualAttributes;
  if(!a.Valid){needRegion=true;continue;}
  const auto protection=a.Win32Protection;
  if(protection&(PAGE_GUARD|PAGE_NOACCESS))return result;
  if(a.Locked||a.LargePage||a.Bad){needRegion=true;continue;}
  switch(protection){
  case PAGE_READONLY:case PAGE_READWRITE:case PAGE_WRITECOPY:
  case PAGE_EXECUTE_READ:case PAGE_EXECUTE_READWRITE:case PAGE_EXECUTE_WRITECOPY:break;
  default:needRegion=true;break;
  }
 }
 if(needRegion)return fallback();
 result.allowed=true;return result;
}
}
