#pragma once
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cstring>

// Readability is established by the fresh, fault-contained data copy itself.
// This never certifies owner identity, pool membership, generation, model,
// descriptor equality, hook ownership, a write target, or a later native call.
// Raw dereferences and executable-hook validation retain their preflight path.
namespace contained_read236 {
inline std::atomic<bool> enabled{false};
inline bool requested=true;
inline std::atomic<uint32_t> accessFaults{},guardFaults{},guardRestoreFailures{};
inline bool Span(uintptr_t address,void* output,size_t size) noexcept {
 return output&&address>=0x10000&&size&&address<=UINTPTR_MAX-size;
}
inline int Exception(EXCEPTION_POINTERS* e) noexcept {
 const auto code=e->ExceptionRecord->ExceptionCode;
 if(code==EXCEPTION_GUARD_PAGE){
  guardFaults.fetch_add(1,std::memory_order_relaxed);
  const auto previous=GetLastError();MEMORY_BASIC_INFORMATION m{};DWORD ignored{};bool restored=false;
  if(e->ExceptionRecord->NumberParameters>=2){
   const auto at=reinterpret_cast<void*>(e->ExceptionRecord->ExceptionInformation[1]);
   if(VirtualQuery(at,&m,sizeof(m))&&m.State==MEM_COMMIT){
    SYSTEM_INFO system{};GetSystemInfo(&system);
    if(system.dwPageSize){const auto page=uintptr_t(at)&~uintptr_t(system.dwPageSize-1);
     restored=VirtualProtect(reinterpret_cast<void*>(page),system.dwPageSize,m.Protect|PAGE_GUARD,&ignored)!=FALSE;
    }
   }
  }
  // A guard fault refuses this copy even when restoration succeeds. Never
  // retry and accept the same source after consuming its one-shot guard.
  if(!restored){guardRestoreFailures.fetch_add(1,std::memory_order_relaxed);enabled.store(false,std::memory_order_release);}
  SetLastError(previous);return EXCEPTION_EXECUTE_HANDLER;
 }
 if(code==EXCEPTION_ACCESS_VIOLATION||code==EXCEPTION_IN_PAGE_ERROR){accessFaults.fetch_add(1,std::memory_order_relaxed);return EXCEPTION_EXECUTE_HANDLER;}
 return EXCEPTION_CONTINUE_SEARCH;
}
__declspec(noinline) inline bool ReadInto236(uintptr_t address,void* output,size_t size) noexcept {
 if(!Span(address,output,size))return false;
 __try{std::memcpy(output,reinterpret_cast<const void*>(address),size);return true;}
 __except(Exception(GetExceptionInformation())){return false;}
}
}
