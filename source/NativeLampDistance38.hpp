#pragma once
#include "ShadowViewPriority.hpp"
namespace fusionfix::shadows {
// Ranking distance only. Both native static-cache and dynamic selection read
// this value in the same invocation; physical light radius/intensity is intact.
inline float NativeLampPriorityDistance(float nativeDistance,float playerDistanceSquared,
                                        float viewWeight,float reach) noexcept {
    if(!std::isfinite(nativeDistance)||nativeDistance<0 || !std::isfinite(playerDistanceSquared)||playerDistanceSquared<0 ||
       !std::isfinite(viewWeight)||viewWeight<1||viewWeight>3 || !std::isfinite(reach)||reach<=0) return nativeDistance;
    const float distance=std::sqrt(playerDistanceSquared);
    if(distance>=reach || distance<=8 || viewWeight<=1) return nativeDistance;
    const float local=SmoothUnit((distance-8)/8);
    const float reachBlend=SmoothUnit((reach-distance)/6);
    const float benefit=(viewWeight-1)*0.5f;
    // At most35% preference, continuous at the local, camera and reach edges.
    return nativeDistance/(1.0f+0.35f*local*reachBlend*benefit);
}
}
