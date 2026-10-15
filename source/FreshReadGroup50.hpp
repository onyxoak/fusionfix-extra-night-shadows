#pragma once
#include "FreshReadBatch30.hpp"
#include <cstring>

// Up to twelve already-known independent spans in the current callback. Authorization is local
// to this invocation; no page permission, owner or lifetime survives the call.
namespace traffic_signal::fresh_group50 {
using Span=fresh_batch30::Span;
constexpr unsigned Capacity=12,MaximumBytes=128;
inline bool Copy(const Span* spans,unsigned count,fresh_page::Check fallback,
 fresh_page::Query query=QueryWorkingSetEx,bool* readFault=nullptr,unsigned rememberMask=UINT32_MAX) noexcept {
 if(readFault)*readFault=false;
 const fresh_page::Preserve preserve;
 memory_read46::Record(memory_read46::Metric::BatchHelpers);
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::PageBatch30,256);
 if(!spans||!count||count>Capacity||!fallback||!query)return false;
 const auto page=fresh_page::PageSize();
 PSAPI_WORKING_SET_EX_INFORMATION entries[Capacity]{};
 unsigned indices[Capacity]{},used=0;bool individual[Capacity]{},known[Capacity]{};
 for(unsigned i=0;i<count;++i){
  const auto& s=spans[i];
  if(!s.output||s.address<0x10000||!s.size||s.size>MaximumBytes||s.address>UINTPTR_MAX-s.size)return false;
  // Respect the configured region strategy even when this primitive is
  // exercised directly; no QWS or borrowed page certificate in that mode.
  if(fresh_page::preferRegion){individual[i]=true;continue;}
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
  // Retention affects positive permission bookkeeping only. Every span
  // still receives preflight and a fresh contained copy, including one-use vectors.
  case PAGE_EXECUTE_READ:case PAGE_EXECUTE_READWRITE:case PAGE_EXECUTE_WRITECOPY:if(rememberMask&(1u<<i))fresh_page::Remember44(s.address,page,query);break;
  default:memory_read46::Record(memory_read46::Metric::FallbackRequests);if(!fallback(s.address,s.size))return false;
  }
 }
 unsigned char bytes[Capacity][MaximumBytes];
 if(!fresh_batch30::Stage(spans,count,bytes)){if(readFault)*readFault=true;memory_read46::Record(memory_read46::Metric::BatchReadFaults);return false;}
 // Outputs belong to the hook's local sample. Publish only after EVERY read
 // succeeds; a raced mapping failure never exposes a partly updated packet.
 for(unsigned i=0;i<count;++i)std::memcpy(spans[i].output,bytes[i],spans[i].size);
 return true;
}
}
