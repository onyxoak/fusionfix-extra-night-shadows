#include "FreshReadBatch30.hpp"
#include <cassert>
#include <thread>
namespace profile=memory_read46;namespace page=traffic_signal::fresh_page;namespace batch=traffic_signal::fresh_batch30;
unsigned queries=0;
BOOL WINAPI PositiveQuery(HANDLE,PVOID b,DWORD n){++queries;auto* entries=static_cast<PSAPI_WORKING_SET_EX_INFORMATION*>(b);for(unsigned i=0;i<n/sizeof(*entries);++i){entries[i].VirtualAttributes.Valid=1;entries[i].VirtualAttributes.Win32Protection=PAGE_READWRITE;}return TRUE;}
bool Reject(uintptr_t,size_t) noexcept{return false;}
int main(){
 for(unsigned i=0;i<8192;++i)profile::Record(profile::Metric::ScalarHelpers);assert(profile::local.helpers==0);for(auto& v:profile::totals)assert(v.load()==0);
 profile::Configure(true);auto* data=static_cast<unsigned char*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(data);data[0]=9;unsigned char out=0;batch::Span span{uintptr_t(data),&out,1};
 {page::Operation44 operation;assert(page::Readable(uintptr_t(data),1,Reject,PositiveQuery));assert(page::Readable(uintptr_t(data+8),1,Reject,PositiveQuery));assert(batch::Copy(&span,1,Reject,PositiveQuery));assert(out==9&&queries==1);}
 auto counts=profile::Snapshot();auto get=[&](profile::Metric m){return counts[unsigned(m)];};assert(get(profile::Metric::ScalarHelpers)==2&&get(profile::Metric::BatchHelpers)==1);assert(get(profile::Metric::ScalarOSQueries)==1&&get(profile::Metric::BatchOSQueries)==0&&get(profile::Metric::QueriedPages)==1);assert(get(profile::Metric::ScalarCacheHits)==1&&get(profile::Metric::BatchSpanCacheHits)==1&&get(profile::Metric::Operations)==1);
 // Actual failed staged read is counted; helper entries are not called OSqueries.
 {page::Operation44 operation;assert(batch::Copy(&span,1,Reject,PositiveQuery));DWORD old{};VirtualProtect(data,4096,PAGE_NOACCESS,&old);out=99;assert(!batch::Copy(&span,1,Reject,PositiveQuery));assert(out==99);VirtualProtect(data,4096,PAGE_READWRITE,&old);}
 counts=profile::Snapshot();assert(get(profile::Metric::BatchReadFaults)==1&&get(profile::Metric::BatchOSQueries)==1);
 // Thread-local hot increments aggregate only on bounded flush, not per entry.
 const auto before=counts[unsigned(profile::Metric::ScalarHelpers)];
 std::thread other([]{for(unsigned i=0;i<profile::FlushEvery;++i)profile::Record(profile::Metric::ScalarHelpers);assert(profile::local.helpers==0);});other.join();counts=profile::Snapshot();assert(get(profile::Metric::ScalarHelpers)==before+profile::FlushEvery);
 page::Configure(true);assert(!page::Readable(uintptr_t(data),1,Reject,PositiveQuery));page::Configure(false);counts=profile::Snapshot();assert(get(profile::Metric::FallbackRequests)==1);
 VirtualFree(data,0,MEM_RELEASE);
}
