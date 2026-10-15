#pragma once
#include <cstdint>
#include <span>
namespace consumer_guard80 {
enum class Body : uint8_t { Refused, Pristine, FusionFix511 };
// Full relocated body remains authoritative. The only supported overlay is
// official FF511's two adaptive-tessellation-state suppression branches.
// Accept both complete states only; never mask arbitrary bytes or partial edits.
inline Body Verify(std::span<const uint8_t> expected,
 std::span<const uint8_t> actual, bool authenticatedFF511) noexcept {
 constexpr unsigned length=0x5f5, first=0x3c6, second=0x5ce;
 if(expected.size()!=length || actual.size()!=length)return Body::Refused;
 bool pristine=true, overlay=authenticatedFF511 && expected[first]==0x74 && expected[second]==0x74;
 for(unsigned i=0;i<length;++i){
  pristine=pristine && actual[i]==expected[i];
  overlay=overlay && actual[i]==((i==first || i==second)?0xeb:expected[i]);
 }
 return pristine?Body::Pristine:overlay?Body::FusionFix511:Body::Refused;
}
}
