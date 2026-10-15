#pragma once
#include <windows.h>
#include <cstdint>
namespace immediate_region33 {
// Permissions live only during one synchronous read-only validation. No native
// call, frame, owner result or data value is retained. Actual reads remain SEH
// contained because a mapping can change after even a fresh VirtualQuery.
struct Scope;
inline thread_local Scope* current=nullptr;
struct Scope {
 Scope* previous; MEMORY_BASIC_INFORMATION entries[8]{};unsigned count{};
 Scope() noexcept:previous(current){current=this;}
 ~Scope() noexcept{current=previous;}
 Scope(const Scope&)=delete;Scope& operator=(const Scope&)=delete;
};
inline bool Contains(const MEMORY_BASIC_INFORMATION& info,uintptr_t address,size_t size) noexcept {
 const auto base=reinterpret_cast<uintptr_t>(info.BaseAddress);
 return address>=base&&address-base<=info.RegionSize&&size<=info.RegionSize-(address-base);
}
inline bool Allowed(const MEMORY_BASIC_INFORMATION& info) noexcept {
 if(info.State!=MEM_COMMIT||(info.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return false;
 switch(info.Protect&0xffu){
 case PAGE_READONLY:case PAGE_READWRITE:case PAGE_WRITECOPY:
 case PAGE_EXECUTE_READ:case PAGE_EXECUTE_READWRITE:case PAGE_EXECUTE_WRITECOPY:return true;
 default:return false;
 }
}
using Query=SIZE_T(WINAPI*)(LPCVOID,PMEMORY_BASIC_INFORMATION,SIZE_T);
inline bool Readable(uintptr_t address,size_t size,Query query=VirtualQuery) noexcept {
 if(address<0x10000||!size||address>UINTPTR_MAX-size)return false;
 if(current)for(unsigned i=0;i<current->count;++i)
  if(Contains(current->entries[i],address,size))return Allowed(current->entries[i]);
 MEMORY_BASIC_INFORMATION info{};
 if(!query(reinterpret_cast<void*>(address),&info,sizeof(info)))return false;
 if(current&&current->count<8)current->entries[current->count++]=info;
 return Contains(info,address,size)&&Allowed(info);
}
}
