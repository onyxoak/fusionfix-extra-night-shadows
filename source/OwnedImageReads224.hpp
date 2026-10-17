#pragma once
// Borrow only readable, non-discardable data mapped by the primary executable.
// That module lives for the process lifetime. No heap/pool ownership, pointer,
// copied value or shadow authorization is retained here. Every copy is fresh
// and remains SEH-contained, including protection changes after startup.
#include <windows.h>
#include <array>
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <span>
#include "PerformanceWorkAvoidance224.hpp"
namespace owned_image224 {
struct Range {uintptr_t begin{},end{};};
struct Table {std::array<Range,64> ranges{};unsigned count{};};
inline Table table;
inline std::atomic<bool> configured{false};
inline bool ReadProtection(DWORD p) noexcept {
 if(p&(PAGE_GUARD|PAGE_NOACCESS))return false;
 switch(p&0xffu){case PAGE_READONLY:case PAGE_READWRITE:case PAGE_WRITECOPY:
 case PAGE_EXECUTE_READ:case PAGE_EXECUTE_READWRITE:case PAGE_EXECUTE_WRITECOPY:return true;default:return false;}
}
inline unsigned Configure(uintptr_t module,size_t imageSize,std::span<const IMAGE_SECTION_HEADER> sections) noexcept {
 if(configured.load(std::memory_order_acquire))return table.count;
 Table next{};
 if(module<0x10000||!imageSize||module>UINTPTR_MAX-imageSize)return 0;
 for(const auto& s:sections){
  if(!(s.Characteristics&IMAGE_SCN_MEM_READ)||(s.Characteristics&(IMAGE_SCN_MEM_EXECUTE|IMAGE_SCN_MEM_DISCARDABLE)))continue;
  const auto length=size_t(s.Misc.VirtualSize);
  if(!length||s.VirtualAddress>=imageSize||length>imageSize-s.VirtualAddress)continue;
  uintptr_t at=module+s.VirtualAddress;const auto end=at+length;
  while(at<end&&next.count<next.ranges.size()){
   MEMORY_BASIC_INFORMATION m{};
   if(!VirtualQuery(reinterpret_cast<void*>(at),&m,sizeof(m)))break;
   const auto base=reinterpret_cast<uintptr_t>(m.BaseAddress);
   if(!m.RegionSize||base>UINTPTR_MAX-m.RegionSize||base+m.RegionSize<=at)break;
   const uintptr_t regionEnd=base+uintptr_t(m.RegionSize);
   const uintptr_t stop=end<regionEnd?end:regionEnd;
   if(m.State==MEM_COMMIT&&m.Type==MEM_IMAGE&&reinterpret_cast<uintptr_t>(m.AllocationBase)==module&&ReadProtection(m.Protect)){
    if(next.count&&next.ranges[next.count-1].end==at)next.ranges[next.count-1].end=stop;
    else next.ranges[next.count++]={at,stop};
   }
   at=stop;
  }
 }
 table=next;configured.store(true,std::memory_order_release);return table.count;
}
inline bool Contains(uintptr_t address,size_t size) noexcept {
 if(!work_avoidance224::Enabled()||!size||address<0x10000||address>UINTPTR_MAX-size||!configured.load(std::memory_order_acquire))return false;
 for(unsigned i=0;i<table.count;++i){const auto& r=table.ranges[i];if(address>=r.begin&&address<r.end&&size<=r.end-address)return true;}
 return false;
}
inline int CopyException(EXCEPTION_POINTERS* e) noexcept {
 const auto code=e->ExceptionRecord->ExceptionCode;
 if(code==EXCEPTION_GUARD_PAGE){
  // A guarded read consumes PAGE_GUARD before its exception is delivered.
  // Refuse the copy and restore that guard rather than silently clearing it.
  const auto previous=GetLastError();MEMORY_BASIC_INFORMATION m{};DWORD ignored{};
  const auto at=reinterpret_cast<void*>(e->ExceptionRecord->ExceptionInformation[1]);
  if(VirtualQuery(at,&m,sizeof(m))&&m.State==MEM_COMMIT){
   SYSTEM_INFO system{};GetSystemInfo(&system);
   if(system.dwPageSize){const auto page=uintptr_t(at)&~uintptr_t(system.dwPageSize-1);
    (void)VirtualProtect(reinterpret_cast<void*>(page),system.dwPageSize,m.Protect|PAGE_GUARD,&ignored);}
  }
  SetLastError(previous);return EXCEPTION_EXECUTE_HANDLER;
 }
 return code==EXCEPTION_ACCESS_VIOLATION||code==EXCEPTION_IN_PAGE_ERROR?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH;
}
}
