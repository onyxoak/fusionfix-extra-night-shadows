#pragma once
#include <atomic>
#include <cstdint>
// Test-only A/B switch for work avoidance. Both modes retain all lighting,
// fallback, native ownership checks and the same visible selection policy.
namespace work_avoidance224 {
inline std::atomic<uint32_t> mode{1};
inline bool Enabled() noexcept{return mode.load(std::memory_order_relaxed)==1;}
}
