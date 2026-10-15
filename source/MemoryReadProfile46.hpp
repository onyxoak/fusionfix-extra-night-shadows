#pragma once
#include <array>
#include <atomic>
#include <cstdint>
namespace memory_read46 {
enum class Metric:unsigned {ScalarHelpers,BatchHelpers,ScalarOSQueries,BatchOSQueries,QueriedPages,ScalarCacheHits,BatchSpanCacheHits,FallbackRequests,BatchReadFaults,Operations,NestedOperations,Count};
constexpr unsigned Count=unsigned(Metric::Count),FlushEvery=4096;
using Values=std::array<std::uint64_t,Count>;
inline bool enabled=false; // Startup-only immutable strategy; default has no hot atomics.
inline std::array<std::atomic<std::uint64_t>,Count> totals{};
struct Local {Values values{};unsigned helpers{};};
inline thread_local Local local;
inline void Configure(bool requested) noexcept {enabled=requested;}
inline void Flush() noexcept {
 if(!enabled)return;
 for(unsigned i=0;i<Count;++i)if(local.values[i]){totals[i].fetch_add(local.values[i],std::memory_order_relaxed);local.values[i]=0;}
 local.helpers=0;
}
inline void Record(Metric metric,std::uint64_t count=1) noexcept {
 if(!enabled)return;
 local.values[unsigned(metric)]+=count;
 if(metric==Metric::ScalarHelpers||metric==Metric::BatchHelpers)if(++local.helpers>=FlushEvery)Flush();
}
inline Values Snapshot() noexcept {
 Flush();Values result{};for(unsigned i=0;i<Count;++i)result[i]=totals[i].load(std::memory_order_relaxed);return result;
}
}
