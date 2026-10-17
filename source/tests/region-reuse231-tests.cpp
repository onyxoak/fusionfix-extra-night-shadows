#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>
#include <intrin.h>

static const auto SystemWorkingSet231=&QueryWorkingSetEx;
static std::atomic<unsigned> queryCount231{},fallbackCount231{},otherQueryCount231{};
static void(*queryAction231)()noexcept=nullptr;
static BOOL WINAPI WorkingSet231(HANDLE process,PVOID data,DWORD bytes){
 ++queryCount231;if(queryAction231)queryAction231();return SystemWorkingSet231(process,data,bytes);
}
static BOOL WINAPI OtherWorkingSet231(HANDLE process,PVOID data,DWORD bytes){
 ++queryCount231;++otherQueryCount231;return SystemWorkingSet231(process,data,bytes);
}
#ifdef QueryWorkingSetEx
#undef QueryWorkingSetEx
#endif
#define QueryWorkingSetEx WorkingSet231
#include "FreshReadGroup50.hpp"
#undef QueryWorkingSetEx
namespace TrafficSignal {
bool ReadableOriginal(uintptr_t at,size_t size)noexcept{
 ++fallbackCount231;return immediate_region33::Readable(at,size);
}
bool Readable(uintptr_t at,size_t size)noexcept{
 return traffic_signal::fresh_page::Readable(at,size,ReadableOriginal);
}
}
namespace Liberty20 {
#include "owned-copy224.inc"
}
static SIZE_T WINAPI CustomRegion231(LPCVOID p,PMEMORY_BASIC_INFORMATION info,SIZE_T bytes){
 return VirtualQuery(p,info,bytes);
}
static void Prime231(void* p,size_t size){assert(immediate_region33::Readable(uintptr_t(p),size));}
static void Reset231(){queryCount231=0;fallbackCount231=0;}
static DWORD Protection231(void* p){MEMORY_BASIC_INFORMATION info{};assert(VirtualQuery(p,&info,sizeof(info)));return info.Protect;}

int main(){
 auto* bytes=static_cast<unsigned char*>(VirtualAlloc(nullptr,8192,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(bytes);
 auto* separate=static_cast<unsigned char*>(VirtualAlloc(nullptr,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(separate);
 uint32_t value=231,output=0;std::memcpy(bytes,&value,4);std::memcpy(bytes+4096,&value,4);std::memcpy(separate,&value,4);
 const auto address=uintptr_t(bytes);
 immediate_region33::borrowWorkingSet231=true;
 // Cold callbacks retain the ordinary native working-set path.
 Reset231();assert(Liberty20::Copy(address,&output,4)&&output==231);assert(queryCount231==1);
 {
  const immediate_region33::Scope scope;Prime231(bytes,8192);Reset231();
  SetLastError(231);const auto fp=_mm_getcsr();
  for(uint32_t i=0;i<10000;++i){std::memcpy(bytes,&i,4);assert(Liberty20::Copy(address,&output,4)&&output==i);}
  assert(queryCount231==0&&fallbackCount231==0&&GetLastError()==231&&_mm_getcsr()==fp);
  assert(!immediate_region33::KnownReadable231(1,4)&&!immediate_region33::KnownReadable231(address,0));
  assert(!immediate_region33::KnownReadable231(UINTPTR_MAX-1,4));
  assert(!immediate_region33::KnownReadable231(uintptr_t(separate),4));
  uint32_t a{},b{},c{};const traffic_signal::fresh_batch30::Span spans[]={{address,&a,4},{address+4096,&b,4}};
  Reset231();assert(traffic_signal::fresh_batch30::Copy(spans,2,TrafficSignal::ReadableOriginal));
  assert(a==9999&&b==231&&queryCount231==0&&fallbackCount231==0);
  assert(traffic_signal::fresh_group50::Copy(spans,2,TrafficSignal::ReadableOriginal));assert(queryCount231==0);
  const traffic_signal::fresh_batch30::Span mixed[]={{address,&a,4},{uintptr_t(separate),&c,4}};
  Reset231();assert(traffic_signal::fresh_group50::Copy(mixed,2,TrafficSignal::ReadableOriginal));assert(a==9999&&c==231&&queryCount231==1);
  // A different page-query provider cannot borrow native region authority.
  Reset231();assert(traffic_signal::fresh_batch30::Copy(spans,2,TrafficSignal::ReadableOriginal,OtherWorkingSet231));assert(queryCount231==1);
  {
   const immediate_region33::Scope inner;assert(!immediate_region33::KnownReadable231(address,4));
   Reset231();assert(Liberty20::Copy(address,&output,4)&&queryCount231==1);
  }
  assert(immediate_region33::KnownReadable231(address,4));Reset231();assert(Liberty20::Copy(address,&output,4)&&queryCount231==0);
  // TLS does not lend this scope's permission to another thread.
  Reset231();std::thread worker([&]{uint32_t v{};assert(!immediate_region33::KnownReadable231(address,4));assert(Liberty20::Copy(address,&v,4)&&v==9999);});worker.join();assert(queryCount231==1);
  // A permission race after preflight is contained; a guard remains armed.
  DWORD previous{};assert(VirtualProtect(bytes+4096,4096,PAGE_READWRITE|PAGE_GUARD,&previous));
  a=0x231; b=0x231;bool fault=false;
  assert(!traffic_signal::fresh_group50::Copy(spans,2,TrafficSignal::ReadableOriginal,WorkingSet231,&fault));
  assert(fault&&a==0x231&&b==0x231&&(Protection231(bytes+4096)&PAGE_GUARD));
  unsigned char crossing[8];std::memset(crossing,0x23,sizeof(crossing));
  const traffic_signal::fresh_batch30::Span cross[]={{address+4094,crossing,sizeof(crossing)}};
  fault=false;assert(!traffic_signal::fresh_batch30::Copy(cross,1,TrafficSignal::ReadableOriginal,WorkingSet231,&fault));
  assert(fault&&(Protection231(bytes+4096)&PAGE_GUARD));for(auto byte:crossing)assert(byte==0x23);
  // An in-flight copy retains guard handling if a later query changes mode.
  const traffic_signal::fresh_batch30::Span modeRace[]={{address+4096,&b,4},{uintptr_t(separate),&c,4}};
  queryAction231=+[]()noexcept{immediate_region33::borrowWorkingSet231=false;};
  b=c=0x231;fault=false;
  assert(!traffic_signal::fresh_group50::Copy(modeRace,2,TrafficSignal::ReadableOriginal,WorkingSet231,&fault));
  queryAction231=nullptr;
  assert(fault&&b==0x231&&c==0x231&&!immediate_region33::borrowWorkingSet231&&(Protection231(bytes+4096)&PAGE_GUARD));
  immediate_region33::borrowWorkingSet231=true;
  assert(VirtualProtect(bytes+4096,4096,PAGE_READWRITE,&previous));
  assert(VirtualProtect(bytes+4096,4096,PAGE_NOACCESS,&previous));
  a=0x231;b=0x231;fault=false;
  assert(!traffic_signal::fresh_batch30::Copy(spans,2,TrafficSignal::ReadableOriginal,WorkingSet231,&fault));
  assert(fault&&a==0x231&&b==0x231);
  output=0x231;assert(!Liberty20::Copy(address+4096,&output,4)&&output==0x231);
  assert(VirtualProtect(bytes+4096,4096,PAGE_READWRITE,&previous));
 }
 assert(immediate_region33::current==nullptr&&!immediate_region33::KnownReadable231(address,4));
 Reset231();assert(Liberty20::Copy(address,&output,4)&&queryCount231==1);
 {
  // Synthetic/non-native region providers are never borrowed by the new path.
  const immediate_region33::Scope scope;
  assert(immediate_region33::Readable(address,4,CustomRegion231));
  assert(!immediate_region33::KnownReadable231(address,4));Reset231();assert(Liberty20::Copy(address,&output,4)&&queryCount231==1);
 }
 {
  const immediate_region33::Scope scope;Prime231(bytes,8192);
  // The public/default-off path is unchanged until the private trial opts in.
  immediate_region33::borrowWorkingSet231=false;Reset231();assert(Liberty20::Copy(address,&output,4)&&queryCount231==1);
 }
 const unsigned iterations=50000;
 double times[2]{};unsigned calls[2]{};
 for(unsigned mode=0;mode<2;++mode){
  const immediate_region33::Scope scope;Prime231(bytes,8192);
  immediate_region33::borrowWorkingSet231=mode!=0;Reset231();
  const auto begin=std::chrono::steady_clock::now();
  for(unsigned i=0;i<iterations;++i){uint32_t v{};assert(Liberty20::Copy(address,&v,4)&&v==9999);}
  times[mode]=std::chrono::duration<double,std::nano>(std::chrono::steady_clock::now()-begin).count()/iterations;
  calls[mode]=queryCount231;
 }
 assert(calls[0]==iterations&&calls[1]==0);
 immediate_region33::borrowWorkingSet231=false;
 assert(VirtualFree(separate,0,MEM_RELEASE)&&VirtualFree(bytes,0,MEM_RELEASE));
 std::cout<<"PASS actual scalar/batch/group production readers: callback-local native permission reuse; fresh bytes; cold/custom/nested/TLS boundaries; guard restoration and atomic output on raced faults.\n";
 std::cout<<"Local fixture only, not game FPS: baseline_ns="<<times[0]<<" candidate_ns="<<times[1]<<" queries="<<calls[0]<<"->"<<calls[1]<<"\n";
}
