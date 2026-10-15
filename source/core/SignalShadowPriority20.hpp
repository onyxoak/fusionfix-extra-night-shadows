#pragma once
#include <algorithm>
#include <cmath>

namespace fusionfix::shadows {
// Ranking only, after current eligibility and physical signal identity checks.
// A bounded, continuous preference helps visible signal receivers compete with
// newly admitted model/neon sources. It grants no map, lease or eligibility.
inline float SignalShadowPriorityDistance20(float distanceSquared,
    bool authenticatedSignal, bool relevant, bool influencesPlayer,
    float viewWeight, float priority) noexcept {
    if (!authenticatedSignal || !relevant || !std::isfinite(distanceSquared) ||
        distanceSquared < 0 || !std::isfinite(viewWeight) ||
        !std::isfinite(priority)) return distanceSquared;
    const float configured = std::clamp(priority, 1.0f, 4.0f);
    // ShadowViewWeight's range is 1..3. Reuse its smooth visible-influence
    // shoulder instead of treating the source point's screen edge as a switch.
    const float visibility = influencesPlayer ? 1.0f :
        std::clamp((viewWeight - 1.0f) * 0.5f, 0.0f, 1.0f);
    const float weight = 1.0f + (configured - 1.0f) * visibility;
    return distanceSquared / weight;
}
}
