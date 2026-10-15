#include "core/PresentFrameTime151.hpp"
#include <sstream>
#include <iomanip>
namespace FrameTime151 {
bool requested=false,prepared=false,sharedGpuPresent=false;
std::atomic<bool> enabled{false};SafetyHookInline presentHook;
present_time151::State<> state;
std::array<present_time151::Record,4096> drained;
std::filesystem::path outputPath;
std::atomic_flag publishing=ATOMIC_FLAG_INIT;
uint32_t lastPublish{};uint64_t written{},ioFailures{},ioDroppedRows{};bool headerWritten{};
using NativePresent=HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,const RECT*,const RECT*,HWND,const RGNDATA*);
struct Ops {
 using Token=present_time151::Token;
 struct Guard {DWORD error=GetLastError();fusionfix::shadows::FloatingPointState fp;~Guard()noexcept{SetLastError(error);}};
 Token Before(IDirect3DDevice9* device) noexcept {
  LARGE_INTEGER start{};if(!QueryPerformanceCounter(&start)||start.QuadPart<=0){++state.clockRefusals;return {};}
  const traffic_signal::fresh_page::Operation44 pages;const immediate_region33::Scope regions;
  uint32_t current{},vtable{},target{},proxy{},frame=UINT32_MAX;
  const auto address=uintptr_t(device);
  if(!device||!Liberty20::Copy(game+0x13ed8d8,&current,4)||current!=address||
   !Liberty20::Copy(address,&vtable,4)||!vtable||!Liberty20::Copy(uintptr_t(vtable)+17*4,&target,4)||target!=game+0x212f0||
   !Liberty20::Copy(address+0x11ac,&proxy,4)||!proxy){++state.identityRefusals;return {};}
  if(CShadows::pFrameCounter)(void)Liberty20::Copy(uintptr_t(CShadows::pFrameCounter),&frame,4);
  auto token=state.Begin(GetCurrentThreadId(),current,proxy,frame,uint64_t(start.QuadPart));
  LARGE_INTEGER nativeStart{};if(token.valid&&QueryPerformanceCounter(&nativeStart)&&nativeStart.QuadPart>0)token.nativeStart=uint64_t(nativeStart.QuadPart);
  return token;
 }
 void After(Token token,HRESULT result) noexcept {
  if(!token.valid)return;LARGE_INTEGER stop{};const auto ok=QueryPerformanceCounter(&stop);
  state.End(token,ok&&stop.QuadPart>0?uint64_t(stop.QuadPart):0,int32_t(result));
 }
};
HRESULT Call(NativePresent original,IDirect3DDevice9* device,const RECT* source,const RECT* destination,HWND window,const RGNDATA* dirty) {
 Ops ops;return present_time151::Invoke(enabled.load(std::memory_order_acquire),ops,original,device,source,destination,window,dirty);
}
HRESULT STDMETHODCALLTYPE Present(IDirect3DDevice9* device,const RECT* source,const RECT* destination,HWND window,const RGNDATA* dirty) {
 return Call(presentHook.original<NativePresent>(),device,source,destination,window,dirty);
}
void PrepareHooks(const std::vector<PreparedHookRange>& other,bool gpuPrepared) noexcept {
 if(!requested)return;
 try {
  LARGE_INTEGER frequency{};Require(QueryPerformanceFrequency(&frequency)&&frequency.QuadPart>0,"frame151_qpc_frequency");state.Configure(uint64_t(frequency.QuadPart));
  Require(gpu39::Forwarder(ReadMemory(game+0x212f0,22),0x44),"frame151_present_forwarder");
  uint32_t device{},vtable{},target{};Require(Liberty20::Copy(game+0x13ed8d8,&device,4)&&device&&Liberty20::Copy(device,&vtable,4)&&vtable&&Liberty20::Copy(uintptr_t(vtable)+17*4,&target,4)&&target==game+0x212f0,"frame151_present_vtable");
  sharedGpuPresent=gpuPrepared;
  if(!sharedGpuPresent){
   Prepare(presentHook,game+0x212f0,Present);
   constexpr std::array<uint8_t,10> stolen{0x8b,0x44,0x24,0x04,0x8b,0x80,0xac,0x11,0,0};
   const auto originalBytes=presentHook.original_bytes();Require(originalBytes.size()==stolen.size()&&!std::memcmp(originalBytes.data(),stolen.data(),stolen.size()),"frame151_present_stolen");
   auto ranges=other;ranges.push_back({presentHook.target_address(),presentHook.original_bytes().size()});Require(DisjointPreparedHooks(ranges),"frame151_present_overlap");
  }
  const auto run=std::to_wstring(GetCurrentProcessId())+L"-"+std::to_wstring(GetTickCount64());
  outputPath=std::filesystem::path(L"D:\\Codex-Work\\Libertys-Shadows-2.0\\evidence\\frame-time151")/run/L"cpu-present.csv";
  prepared=true;Event(1,"FRAME_TIME151_PREPARED","CPU_API_only=1 GPU_queries=0 bounded4096=1 output_private_D=1 reset_full_coverage=0");
 }catch(...){presentHook={};prepared=false;Event(1,"FRAME_TIME151_REFUSED","optional_present_contract_or_output_failed lighting_unchanged=1");}
}
unsigned Enable(bool gpuEnabled) noexcept {
 if(!requested||!prepared)return 0;
 if(sharedGpuPresent){enabled=gpuEnabled;if(!gpuEnabled)Event(1,"FRAME_TIME151_REFUSED","shared_gpu_present_hook_unavailable=1");return 0;}
 if(!presentHook.enable()){prepared=false;Event(1,"FRAME_TIME151_REFUSED","optional_present_enable_failed lighting_unchanged=1");return 0;}
 enabled=true;return 1;
}
void Disable() noexcept {enabled=false;if(presentHook)(void)presentHook.disable();}
void ObserveReset() noexcept {if(enabled.load(std::memory_order_acquire))state.ObserveReset(GetCurrentThreadId());}
void Publish() noexcept {
 if(!enabled.load(std::memory_order_acquire)||publishing.test_and_set(std::memory_order_acquire))return;
 struct Exit{~Exit(){publishing.clear(std::memory_order_release);}}exit;
 const Ops::Guard preserve;
 if(present_time151::PublicationCapReached153(written)){
  enabled=false;Event(1,"FRAME_TIME151_COMPLETE","bounded_rows="+std::to_string(written)+" CPU_API_boundary=1 native_present_passthrough=1");return;
 }
 const auto now=GetTickCount();if(lastPublish&&uint32_t(now-lastPublish)<1000)return;lastPublish=now;
 const auto count=state.Drain(drained.data(),unsigned(drained.size()));if(!count)return;
 try {
  // One bounded chunk open/write/flush per second, outside Present. No
  // persistent queue/thread or GPU call; failures are reported, not retried hot.
  std::filesystem::create_directories(outputPath.parent_path());
  std::ostringstream lines;lines<<std::setprecision(9);
  if(!headerWritten)lines<<"present_ordinal,native_frame,thread,device_generation,api_start_qpc,api_start_interval_ms,native_present_elapsed_ms,flags,hresult,qpc_frequency,ring_drops,thread_refusals,identity_refusals,clock_refusals,nested_refusals\n";
  const auto frequency=state.Frequency();
  for(unsigned i=0;i<count;++i){const auto& r=drained[i];const auto& t=r.token;
   const auto interval=1000.0*double(t.interval)/double(frequency);
   const auto native=(t.flags&present_time151::NativeClockInvalid)?0.0:1000.0*double(r.nativeEnd-t.nativeStart)/double(frequency);
   lines<<t.ordinal<<','<<t.frame<<','<<t.thread<<','<<t.epoch<<','<<t.start<<','<<interval<<','<<native<<','<<t.flags<<','<<r.result<<','<<frequency<<','<<state.drops.load()<<','<<state.threadRefusals.load()<<','<<state.identityRefusals.load()<<','<<state.clockRefusals.load()<<','<<state.nestedRefusals.load()<<'\n';
  }
  std::ofstream out(outputPath,std::ios::app|std::ios::binary);out<<lines.str();out.flush();if(!out.good()){++ioFailures;ioDroppedRows+=count;return;}
  headerWritten=true;written+=count;
  Event(1,"FRAME_TIME151_CSV","rows="+std::to_string(written)+" ring_drops="+std::to_string(state.drops.load())+" io_failures="+std::to_string(ioFailures)+" io_dropped_rows="+std::to_string(ioDroppedRows)+" CPU_API_boundary=1 native_present_includes_driver_wait=1 GPU_display_completion_measured=0");
 }catch(...){++ioFailures;ioDroppedRows+=count;}
}
}

