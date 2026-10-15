#pragma once
#include "FreshPageReadable.hpp"
#include <cstring>

// One callback's already-known independent spans only. Authorization is local
// to this invocation; no page permission, owner or lifetime survives the call.
namespace traffic_signal::fresh_batch30 {
struct Span {uintptr_t address;void* output;size_t size;};
constexpr unsigned Capacity=4,MaximumBytes=128;
__declspec(noinline) inline bool Stage(const Span* spans,unsigned count,
 unsigned char (*bytes)[MaximumBytes]) noexcept {
 __try {
  for(unsigned i=0;i<count;++i)std::memcpy(bytes[i],reinterpret_cast<const void*>(spans[i].address),spans[i].size);
  return true;
 } __except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION||
             GetExceptionCode()==EXCEPTION_IN_PAGE_ERROR||
             GetExceptionCode()==EXCEPTION_GUARD_PAGE?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){return false;}
}
inline bool Copy(const Span* spans,unsigned count,fresh_page::Check fallback,
 fresh_page::Query query=QueryWorkingSetEx,bool* readFault=nullptr) noexcept {
 if(readFault)*readFault=false;
 const fresh_page::Preserve preserve;
 memory_read46::Record(memory_read46::Metric::BatchHelpers);
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::PageBatch30,256);
 if(!spans||!count||count>Capacity)return false;
 const auto page=fresh_page::PageSize();
 PSAPI_WORKING_SET_EX_INFORMATION entries[Capacity]{};
 unsigned indices[Capacity]{},used=0;bool individual[Capacity]{},known[Capacity]{};
 for(unsigned i=0;i<count;++i){
  const auto& s=spans[i];
  if(!s.output||s.address<0x10000||!s.size||s.size>MaximumBytes||s.address>UINTPTR_MAX-s.size)return false;
  if(!page||(page&(page-1))||s.address/page!=(s.address+s.size-1)/page){individual[i]=true;continue;}
  if(fresh_page::Known44(s.address,s.size,page,query)){known[i]=true;memory_read46::Record(memory_read46::Metric::BatchSpanCacheHits);continue;}
  unsigned j=0;
  for(;j<used;++j)if(uintptr_t(entries[j].VirtualAddress)/page==s.address/page)break;
  if(j==used)entries[used++].VirtualAddress=reinterpret_cast<void*>(s.address);
  indices[i]=j;
 }
 if(used){memory_read46::Record(memory_read46::Metric::BatchOSQueries);memory_read46::Record(memory_read46::Metric::QueriedPages,used);}
 const bool queried=used&&query(GetCurrentProcess(),entries,used*sizeof(entries[0]));
 for(unsigned i=0;i<count;++i){
  const auto& s=spans[i];
  if(known[i])continue;
  if(individual[i]||!queried){memory_read46::Record(memory_read46::Metric::FallbackRequests);if(!fallback(s.address,s.size))return false;continue;}
  const auto attributes=entries[indices[i]].VirtualAttributes;
  if(!attributes.Valid){memory_read46::Record(memory_read46::Metric::FallbackRequests);if(!fallback(s.address,s.size))return false;continue;}
  const auto protection=attributes.Win32Protection;
  if(protection&(PAGE_GUARD|PAGE_NOACCESS))return false;
  if(attributes.Locked||attributes.LargePage||attributes.Bad){memory_read46::Record(memory_read46::Metric::FallbackRequests);if(!fallback(s.address,s.size))return false;continue;}
  switch(protection){
  case PAGE_READONLY:case PAGE_READWRITE:case PAGE_WRITECOPY:
  case PAGE_EXECUTE_READ:case PAGE_EXECUTE_READWRITE:case PAGE_EXECUTE_WRITECOPY:fresh_page::Remember44(s.address,page,query);break;
  default:memory_read46::Record(memory_read46::Metric::FallbackRequests);if(!fallback(s.address,s.size))return false;
  }
 }
 unsigned char bytes[Capacity][MaximumBytes];
 if(!Stage(spans,count,bytes)){if(readFault)*readFault=true;memory_read46::Record(memory_read46::Metric::BatchReadFaults);return false;}
 // Outputs belong to the hook's local sample. Publish only after EVERY read
 // succeeds; a raced mapping failure never exposes a partly updated packet.
 for(unsigned i=0;i<count;++i)std::memcpy(spans[i].output,bytes[i],spans[i].size);
 return true;
}
}
