#pragma once
// Startup-only performance switch. Missing key preserves the reviewed32 path.
bool queuedScratch32Requested59=true;
std::atomic<uint32_t> scratchAttempted59{0},scratchCompleted59{0},scratchAllocationFallback59{0};
void ConfigureQueuedScratch59(const wchar_t* ini) noexcept {
 queuedScratch32Requested59=GetPrivateProfileIntW(L"SUPPORT",L"QueuedValidationScratch32",1,ini)==1;
}
struct QueuedScratch59 {
 static constexpr unsigned Limit=traffic_signal::fresh_window59::Groups;
 struct Snapshot {unsigned index{};uint32_t key{};decltype(registry)::Entry entry{};bool copied{};};
 Snapshot hints[Limit]{};TrafficSignal::OwnerRequest49 requests[Limit]{};TrafficSignal::OwnerResult49 results[Limit]{};
 TrafficSignal::Scratch59::OwnerScratch59 owner{};
 traffic_signal::fresh_window59::Span ranges[Limit][7]{};traffic_signal::fresh_window59::Group groups[Limit]{};
 traffic_signal::fresh_certificate59::Certificate certificate{};
};
#ifndef CE_ALLOCATE_SCRATCH59
#define CE_ALLOCATE_SCRATCH59(size) HeapAlloc(GetProcessHeap(),0,size)
#endif
#ifndef CE_RELEASE_SCRATCH59
#define CE_RELEASE_SCRATCH59(pointer) HeapFree(GetProcessHeap(),0,pointer)
#endif
__declspec(noinline) void ValidateQueuedScratch59(rage::CLightSource* lights,uint32_t count,QueuedScratch59& scratch) noexcept {
 constexpr unsigned Limit=traffic_signal::fresh_window59::Groups;

 const auto callbackFrame54=*CShadows::pFrameCounter;
 unsigned cursor=0;
 while(cursor<count){auto& hints=scratch.hints;auto& requests=scratch.requests;auto& results=scratch.results;unsigned used=0;
  for(unsigned i=0;i<Limit;++i){hints[i]={};requests[i]={};results[i]={};}
  while(cursor<count&&used<Limit){const unsigned i=cursor++;auto& l=lights[i];const auto key=uint32_t(l.mCastShadows);
   if(!registry.Contains(key)||!(l.mFlags&4u))continue;
   auto& h=hints[used];h.index=i;h.key=key;
   if(active.load(std::memory_order_acquire)&&!lock.test_and_set(std::memory_order_acquire)){
    if(auto* e=registry.Lookup(key);e&&e->active){h.entry=*e;h.copied=true;requests[used]={e->id.owner,e->descriptor};}
    lock.clear(std::memory_order_release);
   }
   ++used;
  }
  if(!used)continue;
  if(used==1){ValidateOne48(lights[hints[0].index]);continue;}
  ++queuedWindows49;queuedSources49.fetch_add(used,std::memory_order_relaxed);
  TrafficSignal::Scratch59::MatchOwnerWindow59(requests,results,used,scratch.owner);
  auto& certificateStorage54=scratch.certificate;
  traffic_signal::fresh_certificate59::Certificate* certificate54=nullptr;
  if(queuedCommitCertificate54&&!traffic_signal::fresh_page::preferRegion){
   auto& ranges54=scratch.ranges;auto& groups54=scratch.groups;uint8_t noData54{};
   for(unsigned i=0;i<Limit;++i){for(auto& s:ranges54[i])s={};groups54[i]={};}
   for(unsigned i=0;i<used;++i)if(hints[i].copied&&results[i].matched&&!results[i].faulted&&!results[i].serialFallback){
    TrafficSignal::Scratch59::OwnerCommitRanges59(results[i],hints[i].entry.descriptor,ranges54[i],&noData54);groups54[i]={ranges54[i],7};}
   traffic_signal::fresh_certificate59::Preflight(certificateStorage54,groups54,used,TrafficSignal::ReadableOriginal,CE_OWNER_BATCH_QUERY35);certificate54=&certificateStorage54;
   ++queuedPermissionWindows54;queuedPermissionQueries54.fetch_add(certificate54->osQueries,std::memory_order_relaxed);
  }
  for(unsigned i=0;i<used;++i){auto& h=hints[i];auto& l=lights[h.index];auto& result=results[i];
   if(uint32_t(l.mCastShadows)!=h.key||!(l.mFlags&4u)){if(certificate54)certificate54->active=false;ValidateOne48(l);continue;}
   if(result.faulted)++queuedWindowFaults49;
   if(h.copied&&result.serialFallback&&!result.faulted){++queuedFallback49;if(certificate54)certificate54->active=false;ValidateOne48(l);continue;}
   bool valid=false,relookupChanged=false;
   if(h.copied&&active.load(std::memory_order_acquire)&&!lock.test_and_set(std::memory_order_acquire)){
    auto* e=registry.Lookup(h.key);
    relookupChanged=e&&e->active&&(e->id!=h.entry.id||e->descriptor!=h.entry.descriptor||e->modelInfo!=h.entry.modelInfo);
    if(e&&e->active&&!relookupChanged&&result.matched&&result.match.modelInfo==e->modelInfo){
     // Final complete identity/model/matrix/descriptor is freshly rebound
     // under this source's registry lock; staged authority cannot authorize.
     std::array<uint8_t,0x6c> currentBytes{};TrafficSignal::Match current{};bool finalFault=false;
     valid=TrafficSignal::Scratch59::CommitOwnerWindow59(result,e->descriptor,current,currentBytes,finalFault,
      certificate54,i)&&active.load(std::memory_order_acquire)&&
      *CShadows::pFrameCounter==callbackFrame54&&registry.Validate(h.key,current.identity,currentBytes,callbackFrame54);
     valid=valid&&active.load(std::memory_order_acquire)&&*CShadows::pFrameCounter==callbackFrame54;
     if(finalFault){if(certificate54)certificate54->active=false;++queuedWindowFaults49;}
    }
    lock.clear(std::memory_order_release);
   }
   if(relookupChanged&&!result.faulted){++queuedFallback49;if(certificate54)certificate54->active=false;ValidateOne48(l);continue;}
   if(!valid){l.mFlags&=~4u;++retired;}
  }
  if(certificate54){queuedFreshStages54.fetch_add(certificate54->freshStages,std::memory_order_relaxed);queuedDomainFaults54.fetch_add(certificate54->stageFaults,std::memory_order_relaxed);certificate54->active=false;}
 }
}

// Allocation and release occur outside EVERY read/certificate domain.
// Each invocation owns its storage, including nested/reentrant invocations.
__declspec(noinline) bool TryValidateQueuedScratch59(rage::CLightSource* lights,uint32_t count) noexcept {
 scratchAttempted59.fetch_add(1,std::memory_order_relaxed);
 void* memory{};
 {const traffic_signal::fresh_page::Preserve preserve;memory=CE_ALLOCATE_SCRATCH59(sizeof(QueuedScratch59));}
 if(!memory){scratchAllocationFallback59.fetch_add(1,std::memory_order_relaxed);return false;} // original16 before reads/writes
 auto* scratch=new(memory) QueuedScratch59{};
 ValidateQueuedScratch59(lights,count,*scratch);
 scratchCompleted59.fetch_add(1,std::memory_order_relaxed);
 scratch->~QueuedScratch59();
 {const traffic_signal::fresh_page::Preserve preserve;(void)CE_RELEASE_SCRATCH59(memory);}
 return true;
}
