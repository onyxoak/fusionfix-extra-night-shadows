#pragma once
#include <cstdint>
namespace model_prevalidation40 {
// Authenticated CE ResolveSite81E244: pending JE consumes preserved ZF from
// CMP[ESP+13],0 at81E23D; intervening MOVEDI,EAX preserves flags. Native
// inactive path has no active point/spot/corona submission. This predicate
// licenses no memory reads, owner identity or persistent eligibility.
constexpr bool NativeInactive(std::uint32_t savedFlags, bool diagnostic) noexcept {
    return !diagnostic && (savedFlags & 0x40u) != 0;
}
}
