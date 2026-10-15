#pragma once
#include "ModelEffectObserverState.hpp"
// CE's model-effect dispatcher has two native lighting types and two store
// ABIs. The authored corona-only type 0 has no store path and is not inferred
// to be a point source. These scalars do not authorize native memory writes.
namespace model_light_submission20 {
inline constexpr std::uint32_t WrapperSpot=0x81ebab, DirectSpot=0x81ecfc;
struct Abi {
 std::size_t words{}, keyOffset{};
 std::uint32_t type{}, canonicalPointSite{};
 explicit constexpr operator bool() const noexcept{return words!=0;}
};
constexpr Abi Layout(std::uint32_t site) noexcept {
 using namespace model_effect_observer;
 if(site==WrapperPoint)return {16,0x3c,0,WrapperPoint};
 if(site==DirectPoint)return {18,0x44,0,DirectPoint};
 if(site==WrapperSpot)return {16,0x3c,2,WrapperPoint};
 if(site==DirectSpot)return {18,0x44,2,DirectPoint};
 return {};
}
class State : public model_effect_observer::State {
public:
 bool Submission(const model_effect_observer::PointEvidence& evidence) noexcept {
  const auto layout=Layout(evidence.site);
  if(!layout||evidence.type!=layout.type){Invalidate();return false;}
  auto normalized=evidence;
  normalized.site=layout.canonicalPointSite;normalized.type=0;
  // Reuse the tested frame/stack/descriptor/owner/key association without
  // granting type equivalence before authenticating the real native site.
  return Point(normalized);
 }
};
}
