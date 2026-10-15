#pragma once
#include "ShortlistHelperGuard48.hpp"
#include <windows.h>
#include <intrin.h>
#include <atomic>
namespace shortlist_helper48::runtime {
inline Binding binding; // Configured only during guarded startup, then immutable.
inline std::atomic<unsigned> lastReason{unsigned(Reason::Unconfigured)};
__declspec(noinline) inline bool ReadBytes(uintptr_t address,void* output,size_t size) noexcept {
 if(address<0x10000||!output||!size||address>UINTPTR_MAX-size)return false;
 __try {std::memcpy(output,reinterpret_cast<const void*>(address),size);return true;}
 __except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION||GetExceptionCode()==EXCEPTION_IN_PAGE_ERROR||GetExceptionCode()==EXCEPTION_GUARD_PAGE?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){return false;}
}
inline bool GetterSafe48() noexcept {
 const auto error=GetLastError();
 const auto reason=Check(binding,ReadBytes,uintptr_t(__readfsdword(0x2c)));
 lastReason.store(unsigned(reason),std::memory_order_relaxed);SetLastError(error);return reason==Reason::Safe;
}
}
