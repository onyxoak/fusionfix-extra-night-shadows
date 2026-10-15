#pragma once
#include <optional>
// Permission-only evidence for one synchronous queued-validation window.
// No owner/model/descriptor values, output pointers or cross-callback authority.
namespace traffic_signal::fresh_certificate59 {
using Span=fresh_window59::Span;using Group=fresh_window59::Group;
constexpr unsigned Groups=fresh_window59::Groups,Spans=fresh_window59::Spans,Total=Groups*Spans;
struct Certificate {
 struct Range {uintptr_t address{};size_t size{};};
 std::array<std::array<Range,Spans>,Groups> ranges{};
 std::array<unsigned,Groups> counts{};
 std::array<bool,Groups> allowed{},reusable{};
 unsigned groups{},osQueries{},queriedPages{},fallbacks{},freshStages{},stageFaults{};bool active{};
};
inline void Preflight(Certificate& result,const Group* groups,unsigned count,fresh_page::Check fallback,
 fresh_page::Query query=QueryWorkingSetEx) noexcept {
 const fresh_page::Preserve preserve;
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::PageBatch30,256);
 memory_read46::Record(memory_read46::Metric::BatchHelpers);
 result={};if(!groups||count>Groups||!fallback||!query||fresh_page::preferRegion)return;
 result.groups=count;result.active=true;
 for(unsigned g=0;g<count;++g)if(groups[g].spans&&groups[g].count<=Spans){result.counts[g]=groups[g].count;
  for(unsigned j=0;j<groups[g].count;++j)result.ranges[g][j]={groups[g].spans[j].address,groups[g].spans[j].size};}
 const auto page=fresh_page::PageSize();
 // A validated <=128-byte span can touch at most two ordinary pages.
 PSAPI_WORKING_SET_EX_INFORMATION pages[Total*2]{};
 struct Item{unsigned pages[2]{},count{};};Item items[Groups][Spans]{};
 bool valid[Groups]{};unsigned used=0;
 for(unsigned g=0;g<count;++g){const auto& group=groups[g];
  if(!group.spans||!group.count||group.count>Spans)continue;
  bool good=true;result.reusable[g]=true;
  // Validate this group's complete structure before adding page requests.
  for(unsigned j=0;j<group.count;++j){const auto& s=group.spans[j];
   if(!s.output||s.address<0x10000||!s.size||s.size>fresh_batch30::MaximumBytes||s.address>UINTPTR_MAX-s.size)good=false;
  }
  if(!good)continue;valid[g]=true;
  for(unsigned j=0;j<group.count;++j){const auto& s=group.spans[j];auto& item=items[g][j];
   if(page<fresh_batch30::MaximumBytes||(page&(page-1)))continue;
   const uintptr_t first=s.address/page,last=(s.address+s.size-1)/page;
   for(uintptr_t number=first;number<=last;++number){
    unsigned n=0;for(;n<used;++n)if(uintptr_t(pages[n].VirtualAddress)/page==number)break;
    if(n==used)pages[used++].VirtualAddress=reinterpret_cast<void*>(number==first?s.address:number*page);
    item.pages[item.count++]=n;
   }
  }
 }
 if(used){++result.osQueries;result.queriedPages=used;memory_read46::Record(memory_read46::Metric::BatchOSQueries);memory_read46::Record(memory_read46::Metric::QueriedPages,used);}
 const bool queried=used&&query(GetCurrentProcess(),pages,used*sizeof(pages[0]));
 for(unsigned g=0;g<count;++g){if(!valid[g])continue;const auto& group=groups[g];bool permitted=true;
  for(unsigned j=0;j<group.count&&permitted;++j){const auto& s=group.spans[j];const auto& item=items[g][j];
   auto region=[&]() noexcept {result.reusable[g]=false;++result.fallbacks;memory_read46::Record(memory_read46::Metric::FallbackRequests);return fallback(s.address,s.size);};
   if(!item.count||!queried){permitted=region();continue;}
   bool needsRegion=false;
   // Inspect BOTH pages before fallback: a known denial is never hidden by
   // a nonresident/exotic peer. Only ordinary positives become reusable.
   for(unsigned k=0;k<item.count;++k){const auto a=pages[item.pages[k]].VirtualAttributes;
    if(!a.Valid){needsRegion=true;continue;}
    const auto protection=a.Win32Protection;
    if(protection&(PAGE_GUARD|PAGE_NOACCESS)){permitted=false;break;}
    if(a.Locked||a.LargePage||a.Bad){needsRegion=true;continue;}
    switch(protection){case PAGE_READONLY:case PAGE_READWRITE:case PAGE_WRITECOPY:
     case PAGE_EXECUTE_READ:case PAGE_EXECUTE_READWRITE:case PAGE_EXECUTE_WRITECOPY:break;
     default:needsRegion=true;break;}
   }
   if(permitted&&needsRegion)permitted=region();
  }
  if(!permitted)continue;
  result.allowed[g]=true;
 }

 return;
}
// false means no reusable domain/span binding: perform an entirely fresh
// original commit. true means this request was handled (including denial).
inline bool TryCopy(Certificate& certificate,unsigned group,const Span* spans,unsigned count,
 bool& copied,bool& faulted) noexcept {
 copied=false;faulted=false;
 if(!certificate.active)return false;
 if(group>=certificate.groups||!spans||count!=certificate.counts[group]){certificate.active=false;return false;}
 for(unsigned j=0;j<count;++j)if(!spans[j].output||spans[j].address!=certificate.ranges[group][j].address||
  spans[j].size!=certificate.ranges[group][j].size){certificate.active=false;return false;}
 if(!certificate.allowed[group]){certificate.active=false;return false;}
 if(!certificate.reusable[group]){certificate.active=false;return false;}
 const fresh_page::Preserve preserve;
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::PageBatch30,256);
 memory_read46::Record(memory_read46::Metric::BatchHelpers);++certificate.freshStages;
 #ifdef CE_CERTIFICATE_BEFORE_STAGE54
 CE_CERTIFICATE_BEFORE_STAGE54(group);
#endif
 unsigned char bytes[Spans][fresh_batch30::MaximumBytes];
 if(!fresh_batch30::Stage(spans,count,bytes)){
  faulted=true;++certificate.stageFaults;certificate.active=false;memory_read46::Record(memory_read46::Metric::BatchReadFaults);return true;
 }
 for(unsigned j=0;j<count;++j)std::memcpy(spans[j].output,bytes[j],spans[j].size);
 copied=true;return true;
}
}
