#include "FreshPageReadable.hpp"
#include "FreshReadBatch30.hpp"
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <array>

bool FreshRegion(uintptr_t address,size_t size) noexcept {
 MEMORY_BASIC_INFORMATION info{};
 if(!VirtualQuery(reinterpret_cast<void*>(address),&info,sizeof(info))||info.State!=MEM_COMMIT||
    (info.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return false;
 const auto base=reinterpret_cast<uintptr_t>(info.BaseAddress);
 return address>=base&&address-base<=info.RegionSize&&size<=info.RegionSize-(address-base);
}
volatile unsigned sink;
template<class F> double Time(F f) {
 LARGE_INTEGER begin{},end{},frequency{};QueryPerformanceFrequency(&frequency);
 QueryPerformanceCounter(&begin);
 for(unsigned i=0;i<100000;++i)sink+=f(i);
 QueryPerformanceCounter(&end);
 return double(end.QuadPart-begin.QuadPart)*1e9/double(frequency.QuadPart)/100000.;
}
int main(int argc,char**) {
 const size_t ballastBytes=argc>1?600u*1024u*1024u:0;
 auto* ballast=ballastBytes?static_cast<unsigned char*>(VirtualAlloc(nullptr,ballastBytes,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE)):nullptr;
 if(ballastBytes&&!ballast)return 2;
 if(ballast)for(size_t i=0;i<ballastBytes;i+=4096)ballast[i]=1;
 std::printf("synthetic_touched_mib=%u not_game_process=1\n",unsigned(ballastBytes/1024/1024));
 auto* pages=static_cast<unsigned char*>(VirtualAlloc(nullptr,4096*8,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));
 if(!pages)return 1;std::memset(pages,1,4096*8);
 std::array<double,7> q{},v{},batch{};
 for(unsigned trial=0;trial<7;++trial){
  q[trial]=Time([&](unsigned i){return traffic_signal::fresh_page::Readable(uintptr_t(pages)+(i%8)*4096,12,FreshRegion);});
  v[trial]=Time([&](unsigned i){const traffic_signal::fresh_page::Preserve preserve;return FreshRegion(uintptr_t(pages)+(i%8)*4096,12);});
  batch[trial]=Time([&](unsigned i){const traffic_signal::fresh_page::Preserve preserve;PSAPI_WORKING_SET_EX_INFORMATION entries[4]{};
   for(unsigned j=0;j<4;++j)entries[j].VirtualAddress=pages+((i+j)%8)*4096;
   return QueryWorkingSetEx(GetCurrentProcess(),entries,sizeof(entries))!=FALSE;});
 }
 std::sort(q.begin(),q.end());std::sort(v.begin(),v.end());std::sort(batch.begin(),batch.end());
 std::printf("resident median_ns_per_call fresh_working_set=%.1f fresh_region=%.1f working_set_batch4=%.1f trials=7 iterations=100000 profiler_disabled=1\n",q[3],v[3],batch[3]);
 unsigned char out[4][12]{};traffic_signal::fresh_batch30::Span spans[4]{};
 for(unsigned i=0;i<4;++i)spans[i]={uintptr_t(pages)+i*4096,out[i],12};
 std::array<double,7> scalarCopy{},batchCopy{};
 for(unsigned trial=0;trial<7;++trial){
  scalarCopy[trial]=Time([&](unsigned){
   for(auto& s:spans){if(!traffic_signal::fresh_page::Readable(s.address,s.size,FreshRegion))return false;
    std::memcpy(s.output,reinterpret_cast<void*>(s.address),s.size);}return true;});
  batchCopy[trial]=Time([&](unsigned){return traffic_signal::fresh_batch30::Copy(spans,4,FreshRegion);});
 }
 std::sort(scalarCopy.begin(),scalarCopy.end());std::sort(batchCopy.begin(),batchCopy.end());
 std::printf("four_vectors median_ns_per_packet scalar_copy=%.1f batch_copy=%.1f reduction_percent=%.1f separate_pages=1\n",
  scalarCopy[3],batchCopy[3],100.*(scalarCopy[3]-batchCopy[3])/scalarCopy[3]);
 DWORD old{};VirtualProtect(pages,4096,PAGE_NOACCESS,&old);
 std::printf("noaccess rejected_working_set=%d rejected_region=%d\n",!traffic_signal::fresh_page::Readable(uintptr_t(pages),12,FreshRegion),!FreshRegion(uintptr_t(pages),12));
 VirtualProtect(pages,4096,PAGE_READWRITE|PAGE_GUARD,&old);
 std::printf("guard rejected_working_set=%d rejected_region=%d\n",!traffic_signal::fresh_page::Readable(uintptr_t(pages),12,FreshRegion),!FreshRegion(uintptr_t(pages),12));
 VirtualProtect(pages,4096,PAGE_READWRITE,&old);
 VirtualFree(pages,4096,MEM_DECOMMIT);
 std::printf("decommit rejected_working_set=%d rejected_region=%d\n",!traffic_signal::fresh_page::Readable(uintptr_t(pages),12,FreshRegion),!FreshRegion(uintptr_t(pages),12));
 VirtualFree(pages,0,MEM_RELEASE);if(ballast)VirtualFree(ballast,0,MEM_RELEASE);return 0;
}
