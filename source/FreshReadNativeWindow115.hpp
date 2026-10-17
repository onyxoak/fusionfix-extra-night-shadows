#pragma once
#include "FreshReadBatch30.hpp"
#include <array>
#include <new>
#include "ScratchCounterPayload59.hpp"
namespace traffic_signal::fresh_native_window115 {
using Span=fresh_batch30::Span;
constexpr unsigned Groups=32,Spans=16,Total=Groups*Spans;
struct Group{const Span* spans{};unsigned count{};};
struct Result{std::array<bool,Groups> copied{},faulted{};unsigned groups{},osQueries{},queriedPages{},fallbacks{},faults{},copiedBytes{};};
// One immediate independent read phase, not queued-owner authorization. All
// outputs must be hook-owned local buffers. No cached permissions/values are
// read from or published to Operation44; the next phase queries anew.
inline Result Copy(const Group* groups,unsigned count,fresh_page::Check fallback,
 fresh_page::Query query=QueryWorkingSetEx) noexcept {
 const fresh_page::Preserve preserve;
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::PageBatch30,256);
 memory_read46::Record(memory_read46::Metric::BatchHelpers);
 Result result{};if(!groups||count>Groups||!fallback||!query)return result;result.groups=count;
 if(contained_read236::enabled.load(std::memory_order_relaxed)&&query==QueryWorkingSetEx){fresh_batch30::CopyContainedGroups236<Spans>(groups,count,result);return result;}
 const auto page=fresh_page::PageSize();
 PSAPI_WORKING_SET_EX_INFORMATION pages[Total]{};
 struct Item{unsigned page{};bool individual{};};Item items[Groups][Spans]{};
 bool valid[Groups]{};unsigned used=0;
 for(unsigned g=0;g<count;++g){const auto& group=groups[g];
  if(!group.spans||!group.count||group.count>Spans)continue;
  bool good=true;
  // Validate this group's complete structure before adding page requests.
  for(unsigned j=0;j<group.count;++j){const auto& s=group.spans[j];
   if(!s.output||s.address<0x10000||!s.size||s.size>fresh_batch30::MaximumBytes||s.address>UINTPTR_MAX-s.size)good=false;
  }
  if(!good)continue;valid[g]=true;
  for(unsigned j=0;j<group.count;++j){const auto& s=group.spans[j];auto& item=items[g][j];
   if(!page||(page&(page-1))||s.address/page!=(s.address+s.size-1)/page){item.individual=true;continue;}
   unsigned n=0;for(;n<used;++n)if(uintptr_t(pages[n].VirtualAddress)/page==s.address/page)break;
   if(n==used)pages[used++].VirtualAddress=reinterpret_cast<void*>(s.address);item.page=n;
  }
 }
 if(used){++result.osQueries;result.queriedPages=used;memory_read46::Record(memory_read46::Metric::BatchOSQueries);memory_read46::Record(memory_read46::Metric::QueriedPages,used);}
 const bool queried=used&&query(GetCurrentProcess(),pages,used*sizeof(pages[0]));
 for(unsigned g=0;g<count;++g){if(!valid[g])continue;const auto& group=groups[g];bool permitted=true;
  for(unsigned j=0;j<group.count&&permitted;++j){const auto& s=group.spans[j];const auto& item=items[g][j];
   auto region=[&]() noexcept {++result.fallbacks;memory_read46::Record(memory_read46::Metric::FallbackRequests);return fallback(s.address,s.size);};
   if(item.individual||!queried){permitted=region();continue;}
   const auto a=pages[item.page].VirtualAttributes;
   if(!a.Valid){permitted=region();continue;}
   const auto protection=a.Win32Protection;
   if(protection&(PAGE_GUARD|PAGE_NOACCESS)){permitted=false;continue;}
   if(a.Locked||a.LargePage||a.Bad){permitted=region();continue;}
   switch(protection){case PAGE_READONLY:case PAGE_READWRITE:case PAGE_WRITECOPY:
    case PAGE_EXECUTE_READ:case PAGE_EXECUTE_READWRITE:case PAGE_EXECUTE_WRITECOPY:break;
    default:permitted=region();break;}
  }
  if(!permitted)continue;
  unsigned char bytes[Spans][fresh_batch30::MaximumBytes];
  // Same production SEH leaf as serial Copy. One group faults independently;
  // no retry can consume a PAGE_GUARD then reaccept this source.
  if(!fresh_batch30::Stage(group.spans,group.count,bytes)){result.faulted[g]=true;++result.faults;memory_read46::Record(memory_read46::Metric::BatchReadFaults);continue;}
  for(unsigned j=0;j<group.count;++j){std::memcpy(group.spans[j].output,bytes[j],group.spans[j].size);result.copiedBytes+=unsigned(group.spans[j].size);}
  result.copied[g]=true;
 }
 return result;
}
}

#include "FreshReadNativeCertificate115.hpp"
