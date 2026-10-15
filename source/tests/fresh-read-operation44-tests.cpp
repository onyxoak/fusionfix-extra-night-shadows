#include "FreshReadBatch30.hpp"
#include <cassert>
#include <array>
#include <atomic>
#include <thread>
#include <cstdio>
namespace fp=traffic_signal::fresh_page;
namespace batch=traffic_signal::fresh_batch30;
std::atomic<unsigned> queries{0};
BOOL WINAPI CountQuery(HANDLE p,PVOID b,DWORD n){++queries;auto ok=QueryWorkingSetEx(p,b,n);SetLastError(55);return ok;}
BOOL WINAPI PositiveQuery(HANDLE,PVOID b,DWORD n){++queries;auto* entries=static_cast<PSAPI_WORKING_SET_EX_INFORMATION*>(b);for(unsigned i=0;i<n/sizeof(*entries);++i){entries[i].VirtualAttributes.Valid=1;entries[i].VirtualAttributes.Win32Protection=PAGE_READWRITE;}return TRUE;}
bool Region(uintptr_t a,size_t n) noexcept {
 MEMORY_BASIC_INFORMATION m{};if(a<0x10000||!n||a>UINTPTR_MAX-n||!VirtualQuery(reinterpret_cast<void*>(a),&m,sizeof(m)))return false;
 return m.State==MEM_COMMIT&&!(m.Protect&(PAGE_GUARD|PAGE_NOACCESS))&&a>=uintptr_t(m.BaseAddress)&&n<=m.RegionSize-(a-uintptr_t(m.BaseAddress));
}
int main(){
 auto* bytes=static_cast<unsigned char*>(VirtualAlloc(nullptr,4096*10,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(bytes);
 for(unsigned i=0;i<4096*10;++i)bytes[i]=static_cast<unsigned char>(i);
 unsigned char out[4]{};batch::Span pair[]{{uintptr_t(bytes+8),out,2},{uintptr_t(bytes+32),out+2,2}};
 // Accepted fixture: four scalar/batch stages on one page fall from four
 // permission queries to one; copied values are still fresh at every stage.
 queries=0;assert(fp::Readable(uintptr_t(bytes),4,Region,CountQuery));assert(batch::Copy(pair,2,Region,CountQuery));assert(fp::Readable(uintptr_t(bytes+64),4,Region,CountQuery));assert(batch::Copy(pair,2,Region,CountQuery));assert(queries==4);
 queries=0;const auto mxcsr=_mm_getcsr();SetLastError(77);
 {fp::Operation44 operation;assert(fp::Readable(uintptr_t(bytes),4,Region,CountQuery));assert(batch::Copy(pair,2,Region,CountQuery));bytes[8]=111;
  assert(fp::Readable(uintptr_t(bytes+64),4,Region,CountQuery));assert(batch::Copy(pair,2,Region,CountQuery));assert(out[0]==111&&queries==1);
  // Independent nested scope never borrows outer approvals; outer restores.
  {fp::Operation44 nested;assert(fp::Readable(uintptr_t(bytes),4,Region,CountQuery));assert(queries==2);}
  assert(fp::Readable(uintptr_t(bytes),4,Region,CountQuery));assert(queries==2);
  // Another thread obtains its own fresh permission even while outer exists.
  std::thread other([&]{assert(fp::operation44==nullptr);assert(fp::Readable(uintptr_t(bytes),4,Region,CountQuery));});other.join();assert(queries==3);
 }
 assert(fp::operation44==nullptr&&GetLastError()==77&&_mm_getcsr()==mxcsr);
 assert(fp::Readable(uintptr_t(bytes),4,Region,CountQuery));assert(queries==4);
 // Fresh negative/guard observations are never cached or consumed.
 DWORD old{};VirtualProtect(bytes,4096,PAGE_READWRITE|PAGE_GUARD,&old);
 {fp::Operation44 operation;assert(!batch::Copy(pair,2,Region,CountQuery));MEMORY_BASIC_INFORMATION m{};VirtualQuery(bytes,&m,sizeof(m));assert(m.Protect&PAGE_GUARD);assert(operation.count==0);}
 VirtualProtect(bytes,4096,PAGE_READWRITE,&old);
 // A mapping revoked after a positive approval faults in the actual SEH read,
 // rejects the entire batch, and never publishes partially copied output.
 {fp::Operation44 operation;assert(batch::Copy(pair,2,Region,CountQuery));VirtualProtect(bytes,4096,PAGE_NOACCESS,&old);for(auto& v:out)v=99;
  assert(!batch::Copy(pair,2,Region,CountQuery));for(auto v:out)assert(v==99);
 }
 VirtualProtect(bytes,4096,PAGE_READWRITE,&old);
 // Cross-page input never gains authorization from its first cached page.
 {fp::Operation44 operation;assert(fp::Readable(uintptr_t(bytes),4,Region,CountQuery));VirtualProtect(bytes+4096,4096,PAGE_NOACCESS,&old);
  batch::Span cross{uintptr_t(bytes+4094),out,4};assert(!batch::Copy(&cross,1,Region,CountQuery));
 }
 VirtualProtect(bytes+4096,4096,PAGE_READWRITE,&old);
 // Capacity is bounded; overflow returns to fresh queries, never guessed approval.
 queries=0;{fp::Operation44 operation;for(unsigned i=0;i<9;++i)assert(fp::Readable(uintptr_t(bytes+i*4096),4,Region,PositiveQuery));assert(operation.count==8&&queries==9);
  assert(fp::Readable(uintptr_t(bytes+8*4096),4,Region,PositiveQuery));assert(queries==10);}
 VirtualFree(bytes,0,MEM_RELEASE);std::puts("read-operation44: accepted duplicate-query reduction, fresh values, race rejection, guard, nesting, thread isolation, capacity and state preservation passed");
}
