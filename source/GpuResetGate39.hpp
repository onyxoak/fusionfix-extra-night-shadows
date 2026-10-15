#pragma once
#include <atomic>
namespace gpu39 {
class ResetGate {
 std::atomic<unsigned> depth{0};
public:
 void Begin() noexcept {depth.fetch_add(1,std::memory_order_acq_rel);}
 void End() noexcept {depth.fetch_sub(1,std::memory_order_release);}
 bool Blocked()const noexcept {return depth.load(std::memory_order_acquire)!=0;}
};
}
