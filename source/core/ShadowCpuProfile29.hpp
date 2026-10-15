#pragma once
#include <windows.h>
#include <array>
#include <atomic>
#include <cstdint>

// Sample elapsed CPU-side call time, including native submission/blocking.
// No GPU queries, waits, allocations or file output in the measured hooks.
namespace shadow_cpu29 {
enum class Phase : unsigned { Selection, Sources, ModelPre, ModelResolve,
 ModelPoint, ModelPost, ModelPromote, ModelQueued, OwnerMatch,
 PageCheck, RenderOther, RenderStatic, RenderCachedDynamic, RenderUncached,
 PageBatch30, ProjectionConstruct69, ProjectionIntern69, ProjectionQueued69, ProjectionWinner69, Count };
constexpr std::array<const char*,unsigned(Phase::Count)> Names{
 "selection_inclusive","source_validation_inclusive","model_pre",
 "model_resolve_inclusive","model_point_inclusive","model_post",
 "model_promote_inclusive","model_queued_inclusive","owner_match_inclusive",
 "page_check","render_other_inclusive","render_static_inclusive",
 "render_cached_dynamic_inclusive","render_uncached_inclusive","page_batch30",
 "projection_constructor_inclusive69","projection_intern_inclusive69",
 "projection_queued_inclusive69","projection_winner_inclusive69"};
struct Counter {
 std::atomic<uint32_t> calls{0}, samples{0};
 std::atomic<uint64_t> ticks{0}, maximum{0};
};
inline std::array<Counter,unsigned(Phase::Count)> counters;
inline bool enabled=false; // Immutable after addon activation.
inline uint64_t frequency=0;
inline void Configure(bool requested) noexcept {
 LARGE_INTEGER f{};
 enabled=requested&&QueryPerformanceFrequency(&f)&&f.QuadPart>0;
 frequency=enabled?uint64_t(f.QuadPart):0;
}
struct Scope {
 Counter* counter=nullptr;uint64_t begin=0;
 explicit Scope(Phase phase,unsigned interval=64) noexcept {
  if(!enabled)return;
  auto& c=counters[unsigned(phase)];
  const auto call=c.calls.fetch_add(1,std::memory_order_relaxed);
  if(interval&&call%interval)return;
  const auto error=GetLastError();LARGE_INTEGER now{};
  const auto ok=QueryPerformanceCounter(&now);SetLastError(error);if(!ok)return;
  counter=&c;begin=uint64_t(now.QuadPart);
 }
 ~Scope() noexcept {
  if(!counter)return;
  const auto error=GetLastError();LARGE_INTEGER now{};
  const auto ok=QueryPerformanceCounter(&now);SetLastError(error);
  if(!ok||uint64_t(now.QuadPart)<begin)return;
  const auto ticks=uint64_t(now.QuadPart)-begin;
  counter->ticks.fetch_add(ticks,std::memory_order_relaxed);
  counter->samples.fetch_add(1,std::memory_order_relaxed);
  auto old=counter->maximum.load(std::memory_order_relaxed);
  while(ticks>old&&!counter->maximum.compare_exchange_weak(old,ticks,std::memory_order_relaxed)){}
 }
 Scope(const Scope&)=delete;Scope& operator=(const Scope&)=delete;
};
}
