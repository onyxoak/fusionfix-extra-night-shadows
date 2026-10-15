// Opt-in only; native facade COM proxy chain is preserved.
namespace GpuProfiling39 {
bool requested=false,prepared=false,shadowStages=false;
gpu48::StageSchedule stageSchedule;
std::atomic<uint64_t> stageObserved{0},stageNested{0},stageBudgetSkipped{0},stageRefused{0};
SafetyHookInline beginHook,endHook,presentHook,resetHook;
std::atomic<bool> enabled{false},stopRequested{false},unavailable{false};
std::atomic<uint32_t> ownerThread{0},reason{0};
std::atomic<uint64_t> samples{0},sumNs{0},maxNs{0},invalid{0},pending{0},ringFull{0},queryFailures{0},threadRefusals{0},contention{0},presents{0},intervalsPublished{0},resets{0};
std::mutex measurementMutex;gpu39::ResetGate resetGate;
std::array<gpu39::Result,64> completedResults{};std::atomic<uint32_t> resultWrite{0},resultRead{0};std::atomic<uint64_t> resultDrops{0};
std::atomic_flag publishing=ATOMIC_FLAG_INIT;
gpu39::Ring<gpu39::NativeBackend> ring;
uint64_t interval=0;uint32_t generation=0;int open=-1;bool insideScene=false;IDirect3DDevice9* boundFacade=nullptr;
struct StateScope {DWORD error=GetLastError();fusionfix::shadows::FloatingPointState fp;~StateScope(){SetLastError(error);}};
const std::array<SafetyHookInline*,4> Hooks{&beginHook,&endHook,&presentHook,&resetHook};
bool Current(IDirect3DDevice9* device) noexcept {uint32_t live=0;return device&&Liberty20::Copy(game+0x13ed8d8,&live,4)&&live==uint32_t(reinterpret_cast<uintptr_t>(device));}
bool DeviceSlots(IDirect3DDevice9* d,IDirect3DDevice9*& validatedProxy) noexcept {
 validatedProxy=nullptr;
 uint32_t vt{};if(!Liberty20::Copy(uintptr_t(d),&vt,4))return false;
 constexpr std::array<uint32_t,8> slots{41,42,17,16,118,9,1,2},targets{0x21750,0x21770,0x212f0,0x2c3a0,0x228a0,0x21210,0x21110,0x21130};
 for(size_t i=0;i<slots.size();++i){uint32_t got{};if(!Liberty20::Copy(vt+slots[i]*4,&got,4)||got!=game+targets[i])return false;}
 uint32_t inner{},innerVt{},query{};
 if(!Liberty20::Copy(uintptr_t(d)+0x11ac,&inner,4)||!inner||!Liberty20::Copy(inner,&innerVt,4)||!Liberty20::Copy(innerVt+118*4,&query,4))return false;
 HMODULE owner{};if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<const wchar_t*>(uintptr_t(query)),&owner))return false;
 validatedProxy=reinterpret_cast<IDirect3DDevice9*>(uintptr_t(inner));return true;
}
void StopOnOwner() noexcept {ring.Stop();stageSchedule.Reset();boundFacade=nullptr;open=-1;insideScene=false;if(!enabled.load())stopRequested=false;}
void Capture(gpu39::Result result) noexcept {
 const auto w=resultWrite.load(std::memory_order_relaxed),r=resultRead.load(std::memory_order_acquire);
 if(uint32_t(w-r)<completedResults.size()){completedResults[w%completedResults.size()]=result;resultWrite.store(w+1,std::memory_order_release);}else ++resultDrops;
 if(result.disjoint){++invalid;return;}
 const double ns=double(result.end-result.start)*1000000000.0/double(result.frequency);
 if(!std::isfinite(ns)||ns<0||ns>=double(UINT64_MAX)){++invalid;return;}
 const auto value=uint64_t(ns);++samples;sumNs.fetch_add(value,std::memory_order_relaxed);
 if(value>maxNs.load(std::memory_order_relaxed))maxNs.store(value,std::memory_order_relaxed);
}
void Stats() noexcept {pending=ring.stats.pending;ringFull=ring.stats.full;queryFailures=ring.stats.failures;}
bool Thread(bool claim=true) noexcept {const auto current=GetCurrentThreadId();auto expected=uint32_t(0);if(claim)ownerThread.compare_exchange_strong(expected,current);else if(ownerThread.load()==0)return false;if(ownerThread.load()!=current){++threadRefusals;stopRequested=true;reason=4;return false;}return true;}
void MeasureBegin(IDirect3DDevice9* d,HRESULT hr) noexcept {
 if(!enabled.load(std::memory_order_acquire)||resetGate.Blocked()||!Current(d))return;
 const std::unique_lock lock(measurementMutex,std::try_to_lock);if(!lock.owns_lock()){++contention;return;}
 if(resetGate.Blocked())return;
 uint32_t proxyValue=0;if(!Liberty20::Copy(uintptr_t(d)+0x11ac,&proxyValue,4)||!proxyValue){stopRequested=true;reason=10;return;}
 auto* proxy=reinterpret_cast<IDirect3DDevice9*>(uintptr_t(proxyValue));
 if(ring.Bound()&&(boundFacade!=d||ring.BoundDevice()!=proxy)){StopOnOwner();ownerThread=0;stopRequested=false;unavailable=false;++generation;}
 if(!Thread())return;
 if(stopRequested.load()){StopOnOwner();return;}
 if(FAILED(hr)){StopOnOwner();unavailable=true;reason=5;return;}
 if(unavailable.load())return;
  if(!ring.Bound()){
  IDirect3DDevice9* validatedProxy=nullptr;if(!DeviceSlots(d,validatedProxy)){unavailable=true;if(reason.load()!=11)reason=10;return;}
  D3DDEVICE_CREATION_PARAMETERS parameters{};
  if(FAILED(d->GetCreationParameters(&parameters))||!(parameters.BehaviorFlags&D3DCREATE_MULTITHREADED)){unavailable=true;reason=6;return;}
  uint32_t confirmedProxy=0;if(!Liberty20::Copy(uintptr_t(d)+0x11ac,&confirmedProxy,4)||validatedProxy!=proxy||confirmedProxy!=proxyValue){unavailable=true;reason=10;return;}
  if(!ring.Bind(validatedProxy)){unavailable=true;reason=1;Stats();return;}boundFacade=d;++generation;
 }
 ++interval;intervalsPublished=interval;
 if(insideScene){StopOnOwner();stopRequested=true;reason=9;++invalid;return;}
 ring.Poll(interval,Capture);Stats();if(!ring.Bound()){unavailable=true;reason=2;return;}
 insideScene=true;stageSchedule.BeginScene(interval,generation);if(!shadowStages&&(interval%16)==0){uint32_t frame=0;(void)Liberty20::Copy(uintptr_t(CShadows::pFrameCounter),&frame,4);open=ring.Begin({interval,generation,frame,GetTickCount()});}else open=-1;Stats();
}
void MeasureEnd(IDirect3DDevice9* d) noexcept {
 if(!enabled.load()||resetGate.Blocked()||!Current(d)||!Thread(false))return;
 const std::unique_lock lock(measurementMutex,std::try_to_lock);if(!lock.owns_lock()){++contention;stopRequested=true;reason=7;return;}
 if(resetGate.Blocked())return;
 if(stopRequested.load()){StopOnOwner();return;}
 uint32_t liveProxy=0;if(!Liberty20::Copy(uintptr_t(d)+0x11ac,&liveProxy,4)||boundFacade!=d||ring.BoundDevice()!=reinterpret_cast<IDirect3DDevice9*>(uintptr_t(liveProxy))){StopOnOwner();ownerThread=0;stopRequested=false;unavailable=false;++generation;return;}
 if(!insideScene)return;
 if(shadowStages&&stageSchedule.Open()){StopOnOwner();stopRequested=true;reason=12;++invalid;return;}
 stageSchedule.Reset();
 if(open>=0)ring.End(open);open=-1;insideScene=false;Stats();
 if(!ring.Bound()&&!unavailable.load()){unavailable=true;reason=2;}
}
struct StageScope {
 gpu48::StageSchedule::Token token{};int query=-1;
 explicit StageScope(void* object) noexcept {
  // Default off: no memory reads, mutex, query work or object dereference.
  if(!shadowStages||!enabled.load(std::memory_order_acquire))return;
  const StateScope state;
  try {
   if(resetGate.Blocked()||ownerThread.load()!=GetCurrentThreadId()){++stageRefused;return;}
   const std::unique_lock lock(measurementMutex,std::try_to_lock);
   if(!lock.owns_lock()){++contention;++stageRefused;return;}
   if(resetGate.Blocked()||stopRequested.load()||unavailable.load()||!insideScene||!ring.Bound()){++stageRefused;return;}
   // Must remain the exact current facade/proxy epoch, including same-address
   // rebinding. This read is opt-in; no calls through persistent wrapper tokens.
   uint32_t liveProxy=0;
   if(!Current(boundFacade)||!Liberty20::Copy(uintptr_t(boundFacade)+0x11ac,&liveProxy,4)||ring.BoundDevice()!=reinterpret_cast<IDirect3DDevice9*>(uintptr_t(liveProxy))){stopRequested=true;reason=10;++stageRefused;return;}
   token=stageSchedule.Begin();stageObserved=stageSchedule.observed;stageNested=stageSchedule.nested;stageBudgetSkipped=stageSchedule.budgetSkipped;
   if(!token.sample)return;
   uint32_t slot=0,kind=0,frame=0;
   if(object&&Liberty20::Copy(uintptr_t(object)+0x940,&slot,4)&&slot>=1&&slot<=7){
    if(!Liberty20::Copy(game+0xd9f1fc+slot*0x110,&kind,4)||(kind!=3&&kind!=4&&kind!=5))kind=0;
   }else slot=0;
   (void)Liberty20::Copy(uintptr_t(CShadows::pFrameCounter),&frame,4);
   query=ring.Begin({interval,generation,frame,GetTickCount(),kind,slot,token.ordinal});Stats();
   if(!ring.Bound()){stopRequested=true;unavailable=true;reason=2;}
  }catch(...){stopRequested=true;reason=8;++stageRefused;}
 }
 ~StageScope() noexcept {
  if(!token.registered)return;
  const StateScope state;
  try {
   if(resetGate.Blocked()||ownerThread.load()!=GetCurrentThreadId()){++stageRefused;return;}
   const std::unique_lock lock(measurementMutex,std::try_to_lock);
   if(!lock.owns_lock()){++contention;stopRequested=true;reason=7;return;}
   if(resetGate.Blocked())return;
   const bool current=stageSchedule.End(token);
   if(!current||query<0)return;
   uint32_t liveProxy=0;
   if(!enabled.load()||stopRequested.load()||unavailable.load()||!insideScene||!ring.Bound()||generation!=token.generation||interval!=token.interval||!Current(boundFacade)||!Liberty20::Copy(uintptr_t(boundFacade)+0x11ac,&liveProxy,4)||ring.BoundDevice()!=reinterpret_cast<IDirect3DDevice9*>(uintptr_t(liveProxy))){stopRequested=true;++stageRefused;return;}
   if(!ring.End(query)){stopRequested=true;reason=2;}Stats();
  }catch(...){stopRequested=true;reason=8;++stageRefused;}
 }
 StageScope(const StageScope&)=delete;StageScope& operator=(const StageScope&)=delete;
};
HRESULT STDMETHODCALLTYPE Begin(IDirect3DDevice9* d) {
 auto original=beginHook.original<HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*)>();
 const auto hr=original(d);if(!enabled.load())return hr;{const StateScope state;try{MeasureBegin(d,hr);}catch(...){stopRequested=true;reason=8;}}return hr;
}
HRESULT STDMETHODCALLTYPE End(IDirect3DDevice9* d) {
 if(!enabled.load())return endHook.original<HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*)>()(d);
 {const StateScope state;try{MeasureEnd(d);}catch(...){stopRequested=true;reason=8;}}
 return endHook.original<HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*)>()(d);
}
HRESULT STDMETHODCALLTYPE Present(IDirect3DDevice9* d,const RECT* source,const RECT* destination,HWND window,const RGNDATA* dirty) {
 const auto hr=presentHook.original<HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,const RECT*,const RECT*,HWND,const RGNDATA*)>()(d,source,destination,window,dirty);
 if(!enabled.load()&&!stopRequested.load())return hr;
 {const StateScope state;try{
  if(!resetGate.Blocked()&&(enabled.load()||stopRequested.load())&&Current(d)&&Thread(false)){
   ++presents;const std::unique_lock lock(measurementMutex,std::try_to_lock);
   if(lock.owns_lock()&&!resetGate.Blocked()){if(stopRequested.load()||FAILED(hr)){StopOnOwner();if(FAILED(hr)){unavailable=true;reason=5;}}}
   else ++contention;
  }
 }catch(...){stopRequested=true;reason=8;}}return hr;
}
HRESULT STDMETHODCALLTYPE Reset(IDirect3DDevice9* d,D3DPRESENT_PARAMETERS* parameters) {
 // The authenticated native Reset body still clears all engine state caches.
 // MULTITHREADED is required before Bind, so releasing at a foreign reset thread
 // is safe; no Issue/GetData runs there. Serialize against bounded CPU bookkeeping.
 bool current=false;{const StateScope state;current=Current(d);}
 bool affected=false;
 {const StateScope state;const std::lock_guard lock(measurementMutex);affected=current||boundFacade==d;if(affected){resetGate.Begin();StopOnOwner();++resets;}}
 const auto hr=resetHook.original<HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,D3DPRESENT_PARAMETERS*)>()(d,parameters);
 {const StateScope state;if(affected){const std::lock_guard lock(measurementMutex);unavailable=FAILED(hr);reason=FAILED(hr)?5:0;ownerThread=0;if(SUCCEEDED(hr)&&enabled.load())stopRequested=false;++generation;resetGate.End();}}
 return hr;
}
void Disable() noexcept {
 enabled=false;stopRequested=true;
 // In-flight COM resources are released only by graphics/reset callbacks. Keep
 // owned pass-through hooks and trampolines until that cleanup is possible.
 // If no device ever bound, preparation failure is safe to unwind immediately.
 if(ownerThread.load()==0)for(auto* hook:Hooks)if(*hook)(void)hook->disable();
}
void PrepareHooks(const std::vector<PreparedHookRange>& other) noexcept {
 if(!requested)return;
 try{
  const std::array<uint32_t,6> rv{0x21750,0x21770,0x212f0,0x21210,0x21110,0x21130};const std::array<uint32_t,6> methods{0xa4,0xa8,0x44,0x24,4,8};
  for(size_t i=0;i<rv.size();++i)Require(gpu39::Forwarder(ReadMemory(game+rv[i],22),methods[i]),"gpu39_facade_forwarder_guard_failed");
  auto expected=std::vector<uint8_t>(std::begin(gpu39::ResetBytes),std::end(gpu39::ResetBytes));
  for(auto offset:gpu39::ResetRelocs){uint32_t value=U32(expected.data()+offset)+uint32_t(game)-0x400000;std::memcpy(expected.data()+offset,&value,4);}
  Require(ReadMemory(game+0x2c3a0,expected.size())==expected,"gpu39_reset_guard_failed");
  Require(gpu39::Forwarder(ReadMemory(game+0x228a0,22),0x1d8),"gpu39_create_query_guard_failed");
  uint32_t device{},vt{};Require(Liberty20::Copy(game+0x13ed8d8,&device,4)&&device&&Liberty20::Copy(device,&vt,4),"gpu39_device_missing");
  const std::array<uint32_t,5> slots{41,42,17,16,118},targets{0x21750,0x21770,0x212f0,0x2c3a0,0x228a0};
  for(size_t i=0;i<slots.size();++i){uint32_t value{};Require(Liberty20::Copy(vt+slots[i]*4,&value,4)&&value==game+targets[i],"gpu39_facade_vtable_ownership_changed");}
  // Native exact forwarding code is owned. Retain the exact inner COM proxy
  // only during synchronous query creation; own returned query COM interfaces.
  uint32_t inner{},innerVt{},query{};Require(Liberty20::Copy(device+0x11ac,&inner,4)&&inner&&Liberty20::Copy(inner,&innerVt,4)&&Liberty20::Copy(innerVt+118*4,&query,4),"gpu39_proxy_chain_missing");
  HMODULE owner{};Require(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<const wchar_t*>(uintptr_t(query)),&owner)!=0,"gpu39_proxy_query_not_module_owned");
  Prepare(beginHook,game+0x21750,Begin);Prepare(endHook,game+0x21770,End);Prepare(presentHook,game+0x212f0,Present);Prepare(resetHook,game+0x2c3a0,Reset);
  auto ranges=other;for(auto* hook:Hooks)ranges.push_back({hook->target_address(),hook->original_bytes().size()});
  Require(DisjointPreparedHooks(ranges),"gpu39_hook_range_overlap");prepared=true;
 }catch(const std::exception& e){Disable();prepared=false;reason=3;Event(1,"GPU41_UNAVAILABLE",e.what());}
}
unsigned EnableHooks() noexcept {
 if(!prepared)return 0;
 for(auto* hook:Hooks)if(!hook->enable()){Disable();prepared=false;reason=3;Event(1,"GPU41_UNAVAILABLE","optional_hook_enable_failed lighting_unchanged=1");return 0;}
 enabled=true;Event(1,"GPU41_READY",shadowStages?"opt_in=1 required_hooks=4 runtime_query_support_unverified=1 span=shadow_command_span proxy_chain_preserved=1 per_shadow_total_cost_unverified=1":"opt_in=1 required_hooks=4 runtime_query_support_unverified=1 span=BeginScene_EndScene proxy_chain_preserved=1");return 4;
}
void Publish() noexcept {
 if(!requested||publishing.test_and_set(std::memory_order_acquire))return;
 struct PublishExit{~PublishExit(){publishing.clear(std::memory_order_release);}} exit;
 try {auto r=resultRead.load(std::memory_order_relaxed),w=resultWrite.load(std::memory_order_acquire);
  for(unsigned count=0;r!=w&&count<64;++r,++count){const auto value=completedResults[r%completedResults.size()];char text[512];sprintf_s(text,"interval=%llu native_frame_at_begin=%u begin_tick=%u device_generation=%u start_gpu_tick=%llu end_gpu_tick=%llu frequency=%llu disjoint_or_invalid=%u gpu_ms=%.6f span=%s stage_kind=%u map_slot=%u callback_ordinal=%u presented_fps=0 command_span_only=1 per_shadow_total_cost_unverified=1",value.tag.interval,value.tag.nativeFrame,value.tag.beginTick,value.tag.deviceGeneration,value.start,value.end,value.frequency,unsigned(value.disjoint),!value.disjoint&&value.frequency?double(value.end-value.start)*1000.0/double(value.frequency):0.0,value.tag.callbackOrdinal?"shadow_command_span":"BeginScene_EndScene",value.tag.stageKind,value.tag.mapSlot,value.tag.callbackOrdinal);Event(1,value.tag.callbackOrdinal?"GPU_STAGE_SAMPLE48":"GPU_SAMPLE41",text);resultRead.store(r+1,std::memory_order_release);}
 }catch(...){}
 static uint32_t last{};auto now=GetTickCount();if(last&&uint32_t(now-last)<5000)return;last=now;
 try{const auto n=samples.load(),ns=sumNs.load();char text[900];sprintf_s(text,"span=%s cumulative_samples=%llu cumulative_mean_gpu_ms=%.6f maximum_gpu_ms=%.6f invalid=%llu pending_polls=%llu ring_full=%llu result_queue_drops=%llu query_failures=%llu thread_refusals=%llu cpu_lock_contention=%llu intervals=%llu presents=%llu resets=%llu enabled=%u unavailable=%u reason=%u sample_every=16 presented_fps=0 all_gpu_work_in_span_unverified=1 observed_stage_callbacks=%llu nested_skips=%llu budget_skips=%llu stage_refusals=%llu maximum_scopes_per_sampled_scene=7",shadowStages?"shadow_command_span":"BeginScene_EndScene",n,n?double(ns)/double(n)/1000000.0:0.0,double(maxNs.load())/1000000.0,invalid.load(),pending.load(),ringFull.load(),resultDrops.load(),queryFailures.load(),threadRefusals.load(),contention.load(),intervalsPublished.load(),presents.load(),resets.load(),unsigned(enabled.load()),unsigned(unavailable.load()),reason.load(),stageObserved.load(),stageNested.load(),stageBudgetSkipped.load(),stageRefused.load());Event(1,"GPU_PROFILE41",text);}catch(...){}
}
}
