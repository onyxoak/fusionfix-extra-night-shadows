#pragma once
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>

namespace fixture_light244 {
inline constexpr uint32_t IndustrialFourHead=1117016355u; // CJ_IND_LIGHT_1
inline constexpr float IndustrialRadius=32.f;

// Authored heads are roughly eleven metres above the base. A ten-metre
// sphere never reaches its ground. Keep position, colour and power unchanged;
// extend only this model's downward native point source, before native culling.
inline bool Range(uint32_t model,uint32_t type,float radius,
 const std::array<float,3>& direction,float& corrected) noexcept {
 corrected=radius;
 if(model!=IndustrialFourHead||type!=0||!std::isfinite(radius)||radius<=0.f||radius>=IndustrialRadius)return false;
 for(float lane:direction)if(!std::isfinite(lane))return false;
 if(std::abs(direction[0])>0.01f||std::abs(direction[1])>0.01f||direction[2]>-0.99f||direction[2]<-1.01f)return false;
 corrected=IndustrialRadius;return true;
}
}
