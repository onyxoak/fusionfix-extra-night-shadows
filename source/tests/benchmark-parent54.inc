// Hook-owned snapshots are address hints only. Locks remain per source;
// final relookup authenticates the live entry and complete fresh identity.
bool queuedWindow49=true;
// Reviewed ordinary-positive permission batching; fresh locked contents remain mandatory.
bool queuedCommitCertificate54=true;
std::atomic<uint32_t> queuedPermissionWindows54{0},queuedPermissionQueries54{0},queuedFreshStages54{0},queuedDomainFaults54{0};
std::atomic<uint32_t> queuedWindows49{0},queuedSources49{0},queuedFallback49{0},queuedWindowFaults49{0};
bool ValidateOne48(rage::CLightSource& l) noexcept;
void ValidateQueuedWindow49(rage::CLightSource* lights,uint32_t count) noexcept {
 constexpr unsigned Limit=traffic_signal::fresh_window49::Groups;
 struct Snapshot {unsigned index{};uint32_t key{};decltype(registry)::Entry entry{};bool copied{};};
 const auto callbackFrame54=*CShadows::pFrameCounter;
 unsigned cursor=0;
 while(cursor<count){Snapshot hints[Limit]{};TrafficSignal::OwnerRequest49 requests[Limit]{};TrafficSignal::OwnerResult49 results[Limit]{};unsigned used=0;
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
  TrafficSignal::MatchOwnerWindow49(requests,results,used);
  std::optional<traffic_signal::fresh_certificate54::Certificate> certificateStorage54;
  traffic_signal::fresh_certificate54::Certificate* certificate54=nullptr;
  if(queuedCommitCertificate54&&!traffic_signal::fresh_page::preferRegion){
   traffic_signal::fresh_window49::Span ranges54[Limit][7]{};traffic_signal::fresh_window49::Group groups54[Limit]{};uint8_t noData54{};
   for(unsigned i=0;i<used;++i)if(hints[i].copied&&results[i].matched&&!results[i].faulted&&!results[i].serialFallback){
    TrafficSignal::OwnerCommitRanges54(results[i],hints[i].entry.descriptor,ranges54[i],&noData54);groups54[i]={ranges54[i],7};}
   certificateStorage54.emplace(traffic_signal::fresh_certificate54::Preflight(groups54,used,TrafficSignal::ReadableOriginal,CE_OWNER_BATCH_QUERY35));certificate54=&*certificateStorage54;
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
     valid=TrafficSignal::CommitOwnerWindow49(result,e->descriptor,current,currentBytes,finalFault,
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
  if(certificate54){queuedFreshStages54.fetch_add(certificate54->freshStages,std::memory_order_relaxed);queuedDomainFaults54.fetch_add(certificate54->stageFaults,std::memory_order_relaxed);}
 }
}
