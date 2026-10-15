#include "FreshReadBatch30.hpp"
#include "fresh-page195-reference196.inc"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>

namespace current=traffic_signal::fresh_page;
namespace old=traffic_signal::fresh_page195;
namespace batch=traffic_signal::fresh_batch30;
unsigned queries{},fallbacks{},otherQueries{};
bool disturb{};
void Disturb() noexcept {
 if(!disturb)return;
 __asm fninit
 _mm_setcsr(0x1f80);SetLastError(123);
}
BOOL WINAPI Query(HANDLE process,PVOID buffer,DWORD bytes) {
 ++queries;const auto ok=QueryWorkingSetEx(process,buffer,bytes);Disturb();return ok;
}
BOOL WINAPI OtherQuery(HANDLE process,PVOID buffer,DWORD bytes) {++otherQueries;return Query(process,buffer,bytes);}
bool Region(uintptr_t address,size_t size) noexcept {
 ++fallbacks;MEMORY_BASIC_INFORMATION m{};
 const auto got=VirtualQuery(reinterpret_cast<void*>(address),&m,sizeof(m));
 const bool ok=got&&m.State==MEM_COMMIT&&!(m.Protect&(PAGE_GUARD|PAGE_NOACCESS))&&
  address>=uintptr_t(m.BaseAddress)&&address-uintptr_t(m.BaseAddress)<=m.RegionSize&&
  size<=m.RegionSize-(address-uintptr_t(m.BaseAddress));
 Disturb();return ok;
}
__declspec(noinline) void SeedState() noexcept {
 const unsigned short control=0x077f;
 __asm fninit
 __asm fldcw control
 __asm fld1
 __asm fldpi
 _mm_setcsr(0x3fa0);SetLastError(0x196abcd);
}
template<class F> bool State(F&& f) {
 alignas(16) unsigned char saved[512]{},before[512]{},after[512]{};
 _fxsave(saved);SeedState();_fxsave(before);
 const bool result=f();_fxsave(after);const auto error=GetLastError();
 _fxrstor(saved);
 // Legacy state/control/status/tag, MXCSR, and all eight x87 register slots.
 // XMM registers are caller-saved by the C++ ABI and tested by hook fixtures.
 assert(std::memcmp(before,after,160)==0);assert(error==0x196abcd);return result;
}
volatile unsigned sink{};
template<class F> double Time(F&& f) {
 LARGE_INTEGER frequency{},start{},end{};QueryPerformanceFrequency(&frequency);
 QueryPerformanceCounter(&start);
 for(unsigned i=0;i<500000;++i)sink+=unsigned(f(i));
 QueryPerformanceCounter(&end);
 return double(end.QuadPart-start.QuadPart)*1e9/double(frequency.QuadPart)/500000.;
}
void Benchmark(unsigned char* bytes) {
 disturb=false;memory_read46::Configure(false);shadow_cpu29::Configure(false);
 current::Operation44 now;old::Operation44 prior;
 assert(current::Readable(uintptr_t(bytes),4,Region,Query));assert(old::Readable(uintptr_t(bytes),4,Region,Query));
 std::array<double,9> a{},b{};
 for(unsigned trial=0;trial<a.size();++trial){
  auto oldTime=[&]{return Time([&](unsigned i){return old::Readable(uintptr_t(bytes)+(i&127),4,Region,Query);});};
  auto newTime=[&]{return Time([&](unsigned i){return current::Readable(uintptr_t(bytes)+(i&127),4,Region,Query);});};
  if(trial&1){b[trial]=newTime();a[trial]=oldTime();}else{a[trial]=oldTime();b[trial]=newTime();}
 }
 std::sort(a.begin(),a.end());std::sort(b.begin(),b.end());
 std::printf("known_page median_ns_per_call baseline195=%.3f candidate196=%.3f reduction_percent=%.2f trials=9 iterations=500000 no_game=1\n",a[4],b[4],100.*(a[4]-b[4])/a[4]);
 // Cold operations still perform the same real query and preservation.
 for(unsigned trial=0;trial<a.size();++trial){
  auto oldTime=[&]{return Time([&](unsigned){old::Operation44 call;return old::Readable(uintptr_t(bytes),4,Region,Query);});};
  auto newTime=[&]{return Time([&](unsigned){current::Operation44 call;return current::Readable(uintptr_t(bytes),4,Region,Query);});};
  if(trial&1){b[trial]=newTime();a[trial]=oldTime();}else{a[trial]=oldTime();b[trial]=newTime();}
 }
 std::sort(a.begin(),a.end());std::sort(b.begin(),b.end());
 std::printf("fresh_operation median_ns_per_call baseline195=%.3f candidate196=%.3f reduction_percent=%.2f trials=9 iterations=500000 no_game=1\n",a[4],b[4],100.*(a[4]-b[4])/a[4]);
 for(unsigned trial=0;trial<a.size();++trial){
  auto oldTime=[&]{return Time([&](unsigned){old::Operation44 call;unsigned result=0;for(unsigned n=0;n<8;++n)result+=unsigned(old::Readable(uintptr_t(bytes)+n*8,4,Region,Query));return result;});};
  auto newTime=[&]{return Time([&](unsigned){current::Operation44 call;unsigned result=0;for(unsigned n=0;n<8;++n)result+=unsigned(current::Readable(uintptr_t(bytes)+n*8,4,Region,Query));return result;});};
  if(trial&1){b[trial]=newTime();a[trial]=oldTime();}else{a[trial]=oldTime();b[trial]=newTime();}
 }
 std::sort(a.begin(),a.end());std::sort(b.begin(),b.end());
 std::printf("eight_reads_one_operation median_ns_per_operation baseline195=%.3f candidate196=%.3f reduction_percent=%.2f trials=9 iterations=500000 no_game=1\n",a[4],b[4],100.*(a[4]-b[4])/a[4]);
}
int main(int argc,char**) {
 auto* bytes=static_cast<unsigned char*>(VirtualAlloc(nullptr,4096*3,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(bytes);
 std::memset(bytes,42,4096*3);disturb=true;
 // Initial PageSize() initialization is guarded; then both exact old and new
 // paths preserve unusual rounding, sticky MXCSR, and nonempty x87 stack.
 assert(State([&]{return current::Readable(uintptr_t(bytes),4,Region,Query);}));
 assert(State([&]{return old::Readable(uintptr_t(bytes),4,Region,Query);}));
 queries=0;
 {
  current::Operation44 operation;old::Operation44 reference;
  assert(operation.pageSize196==0);
  assert(State([&]{return current::Readable(uintptr_t(bytes),4,Region,Query);}));
  assert(State([&]{return old::Readable(uintptr_t(bytes),4,Region,Query);}));assert(queries==2);
  assert(operation.pageSize196==4096);
  for(unsigned n=0;n<16;++n){
   assert(State([&]{return current::Readable(uintptr_t(bytes+n*8),8,Region,Query);}));
   assert(State([&]{return old::Readable(uintptr_t(bytes+n*8),8,Region,Query);}));
  }assert(queries==2);
  // The query identity still participates; nested scopes never borrow outer
  // permissions, and returning to the outer operation restores only it.
  assert(State([&]{return current::Readable(uintptr_t(bytes),4,Region,OtherQuery);}));assert(queries==3);
  {current::Operation44 nested;assert(State([&]{return current::Readable(uintptr_t(bytes),4,Region,Query);}));assert(queries==4);}
  assert(State([&]{return current::Readable(uintptr_t(bytes),4,Region,Query);}));assert(queries==4);
  // Enabled diagnostics take the complete original instrumented path.
  memory_read46::Configure(true);const auto before=memory_read46::Snapshot();
  assert(State([&]{return current::Readable(uintptr_t(bytes),4,Region,Query);}));const auto after=memory_read46::Snapshot();
  assert(after[unsigned(memory_read46::Metric::ScalarHelpers)]==before[unsigned(memory_read46::Metric::ScalarHelpers)]+1);
  assert(after[unsigned(memory_read46::Metric::ScalarCacheHits)]==before[unsigned(memory_read46::Metric::ScalarCacheHits)]+1);
  memory_read46::Configure(false);shadow_cpu29::Configure(true);
  const auto calls=shadow_cpu29::counters[unsigned(shadow_cpu29::Phase::PageCheck)].calls.load();
  assert(State([&]{return current::Readable(uintptr_t(bytes),4,Region,Query);}));
  assert(shadow_cpu29::counters[unsigned(shadow_cpu29::Phase::PageCheck)].calls.load()==calls+1);shadow_cpu29::Configure(false);
  current::Configure(true);const auto priorFallback=fallbacks;
  assert(State([&]{return current::Readable(uintptr_t(bytes),4,Region,Query);}));assert(fallbacks==priorFallback+1);current::Configure(false);
  assert(!State([&]{return current::Readable(0,4,Region,Query);}));
  assert(!State([&]{return current::Readable(uintptr_t(bytes),0,Region,Query);}));
  assert(!State([&]{return current::Readable(UINTPTR_MAX-2,4,Region,Query);}));
  // Existing approved memory can still be revoked before Copy. Keep SEH and
  // preservation on its actual read, and never publish a partly read packet.
  DWORD protection{};VirtualProtect(bytes+4096,4096,PAGE_NOACCESS,&protection);
  assert(!State([&]{return current::Readable(uintptr_t(bytes+4094),4,Region,Query);}));
  VirtualProtect(bytes+4096,4096,PAGE_READWRITE,&protection);
  bytes[4096]=42; // Make the restored mapping resident before caching it.
  unsigned char out[8];std::memset(out,99,sizeof(out));
  batch::Span spans[2]{{uintptr_t(bytes),out,4},{uintptr_t(bytes+4096),out+4,4}};
  assert(State([&]{return batch::Copy(spans,2,Region,Query);}));
  VirtualProtect(bytes+4096,4096,PAGE_NOACCESS,&protection);std::memset(out,99,sizeof(out));bool fault=false;
  assert(!State([&]{return batch::Copy(spans,2,Region,Query,&fault);}));assert(fault);
  for(const auto value:out)assert(value==99);
  VirtualProtect(bytes+4096,4096,PAGE_READWRITE,&protection);
 }
 const auto before=queries;assert(State([&]{return current::Readable(uintptr_t(bytes),4,Region,Query);}));assert(queries==before+1);
 if(argc>1)Benchmark(bytes);
 VirtualFree(bytes,0,MEM_RELEASE);
 std::puts("fresh-read-fast196: exact195 hit parity, query identity, nesting, fresh operation, counters, region strategy, range refusal, SEH race, x87/MXCSR/LastError passed");
}
