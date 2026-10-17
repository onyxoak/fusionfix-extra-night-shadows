#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#include <cassert>
#include <array>
#include <cstring>
#include <thread>
#include <cstdio>
static const auto NativeQuery236=&QueryWorkingSetEx;
static unsigned queries236{},fallback236{};
static BOOL WINAPI Query236(HANDLE p,PVOID out,DWORD bytes){++queries236;return NativeQuery236(p,out,bytes);}
#undef QueryWorkingSetEx
#define QueryWorkingSetEx Query236
#include "FreshReadGroup50.hpp"
#include "FreshReadWindow49.hpp"
static bool Region236(uintptr_t address,size_t size)noexcept{++fallback236;return immediate_region33::Readable(address,size);}
using traffic_signal::fresh_batch30::Span;
static LONG CALLBACK DisableAtGuard236(EXCEPTION_POINTERS* e){
 if(e->ExceptionRecord->ExceptionCode==EXCEPTION_GUARD_PAGE)contained_read236::enabled.store(false,std::memory_order_release);
 return EXCEPTION_CONTINUE_SEARCH;
}
int main(){
 auto* pages=static_cast<unsigned char*>(VirtualAlloc(nullptr,3*4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(pages);
 for(unsigned i=0;i<3*4096;++i)pages[i]=static_cast<unsigned char>(i);
 contained_read236::enabled=true;
 std::array<unsigned char,128> a{},b{},c{};
 SetLastError(0x236);
 assert(contained_read236::ReadInto236(uintptr_t(pages+4090),a.data(),a.size()));
 assert(GetLastError()==0x236&&std::memcmp(a.data(),pages+4090,a.size())==0);
 assert(!contained_read236::ReadInto236(0,a.data(),1));assert(!contained_read236::ReadInto236(UINTPTR_MAX-2,a.data(),8));
 const Span spans[]{{uintptr_t(pages+4090),a.data(),a.size()},{uintptr_t(pages+100),b.data(),b.size()},{uintptr_t(pages+8200),c.data(),c.size()}};
 assert(traffic_signal::fresh_batch30::Copy(spans,3,Region236));assert(queries236==0&&fallback236==0);
 assert(traffic_signal::fresh_group50::Copy(spans,3,Region236));assert(queries236==0&&fallback236==0);
 // Injected providers keep the legacy contract; no provider is silently bypassed.
 assert(traffic_signal::fresh_batch30::Copy(spans,3,Region236,NativeQuery236));
 DWORD old{};assert(VirtualProtect(pages+4096,4096,PAGE_NOACCESS,&old));
 a.fill(0xcc);b.fill(0xcc);c.fill(0xcc);bool fault=false;
 assert(!traffic_signal::fresh_batch30::Copy(spans,3,Region236,Query236,&fault)&&fault);
 for(auto x:a)assert(x==0xcc);for(auto x:b)assert(x==0xcc);for(auto x:c)assert(x==0xcc);
 assert(!contained_read236::ReadInto236(uintptr_t(pages+4090),a.data(),128));
 assert(VirtualProtect(pages+4096,4096,PAGE_READWRITE|PAGE_GUARD,&old));
 for(unsigned repeat=0;repeat<3;++repeat){
  a.fill(0x11);b.fill(0x11);fault=false;
  assert(!traffic_signal::fresh_group50::Copy(spans,3,Region236,Query236,&fault)&&fault);
  for(auto x:a)assert(x==0x11);for(auto x:b)assert(x==0x11);
  MEMORY_BASIC_INFORMATION m{};assert(VirtualQuery(pages+4096,&m,sizeof(m))&&(m.Protect&PAGE_GUARD));
 }
 assert(contained_read236::guardFaults==3&&contained_read236::guardRestoreFailures==0&&contained_read236::enabled);
 // Window groups fail independently; a denied middle group never publishes.
 Span denied{uintptr_t(pages+4096),b.data(),4};
 const traffic_signal::fresh_native_window115::Group groups[]{{spans+1,1},{&denied,1},{spans+2,1}};
 auto window=traffic_signal::fresh_native_window115::Copy(groups,3,Region236);
 assert(window.copied[0]&&!window.copied[1]&&window.faulted[1]&&window.copied[2]&&window.osQueries==0&&window.faults==1);
 assert(b[0]==pages[100]&&b[1]==pages[101]);
 // The failing group's destination deliberately overlaps an earlier accepted
 // output here; it must retain that earlier complete value, never partial data.
 assert(contained_read236::guardFaults==4);
 // A concurrent mode change cannot weaken an already-started copy's filter.
 auto handler=AddVectoredExceptionHandler(1,DisableAtGuard236);assert(handler);
 unsigned char staged[1][traffic_signal::fresh_batch30::MaximumBytes];
 assert(!traffic_signal::fresh_batch30::Stage(&denied,1,staged,false));
 assert(!contained_read236::enabled&&contained_read236::guardFaults==5);
 MEMORY_BASIC_INFORMATION guarded{};assert(VirtualQuery(pages+4096,&guarded,sizeof(guarded))&&(guarded.Protect&PAGE_GUARD));
 assert(RemoveVectoredExceptionHandler(handler));contained_read236::enabled=true;
 assert(VirtualProtect(pages+4096,4096,PAGE_READWRITE,&old));
 assert(contained_read236::ReadInto236(uintptr_t(pages+4090),a.data(),a.size()));
 // Changed contents are read again; the path never retains bytes or owners.
 pages[4090]^=0xff;assert(contained_read236::ReadInto236(uintptr_t(pages+4090),a.data(),a.size()));assert(a[0]==pages[4090]);
 auto* freed=static_cast<unsigned char*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(freed);assert(VirtualFree(freed,0,MEM_RELEASE));
 assert(!contained_read236::ReadInto236(uintptr_t(freed),a.data(),a.size()));
 std::thread worker([&]{std::array<unsigned char,128> result{};assert(contained_read236::ReadInto236(uintptr_t(pages+100),result.data(),result.size()));assert(result[0]==pages[100]);});worker.join();
 contained_read236::enabled=false;queries236=0;fallback236=0;
 assert(traffic_signal::fresh_batch30::Copy(spans,3,Region236));assert(queries236>0&&fallback236>0);
 assert(VirtualFree(pages,0,MEM_RELEASE));
 puts("contained236: fresh/cross-page/multi-thread/freed/no-access/guard-restoration/group-atomicity/provider and legacy rollback checks passed");
}

