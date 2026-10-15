#pragma once
namespace onyxoak {
// The stock warning remains appropriate whenever the full correction is not
// active, including a runtime thread-safety fallback or a reduced settings mode.
constexpr bool SuppressStockShadowWarning(bool active,bool allocationReady,bool publishes,
 bool unsupportedThread,bool casterEnabled,bool extra,bool headlights,bool vehicles) noexcept {
 return active&&allocationReady&&publishes&&!unsupportedThread&&casterEnabled&&extra&&headlights&&vehicles;
}
}
