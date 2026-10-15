#pragma once
#include <array>
#include <cstdint>
#include <limits>
namespace fusionfix::shadows {
// Diagnostic classification only. These facts never grant rendering authority.
enum SelectionLossFact38 : std::uint32_t {
 LossSourcePresent38=1, LossCollected38=2, LossSameGeneration38=4,
 LossRelevant38=8, LossAmbiguous38=16, LossShadowEnabled38=32,
 LossCacheUnavailable38=64, LossReferenceChosen38=128
};
enum class SelectionLossReason38 : std::int32_t {
 MissingSource=1, AmbiguousIdentity, ChangedGeneration, ShadowFlagAbsent,
 CacheUnavailable, NotCollected, Irrelevant, EligibleNotSelected
};
inline SelectionLossReason38 ClassifySelectionLoss38(std::uint32_t facts) noexcept {
 if(!(facts&LossSourcePresent38))return SelectionLossReason38::MissingSource;
 if(facts&LossAmbiguous38)return SelectionLossReason38::AmbiguousIdentity;
 if(!(facts&LossShadowEnabled38))return SelectionLossReason38::ShadowFlagAbsent;
 if(facts&LossCacheUnavailable38)return SelectionLossReason38::CacheUnavailable;
 if(!(facts&LossCollected38))return SelectionLossReason38::NotCollected;
 if(!(facts&LossSameGeneration38))return SelectionLossReason38::ChangedGeneration;
 if(!(facts&LossRelevant38))return SelectionLossReason38::Irrelevant;
 return SelectionLossReason38::EligibleNotSelected;
}
struct SelectionScoreDiagnostic38 {
 std::uint32_t frame{};
 std::uint64_t generation{};
 float rawFocus=std::numeric_limits<float>::quiet_NaN();
 float viewWeight=std::numeric_limits<float>::quiet_NaN();
 float rawImportance=std::numeric_limits<float>::quiet_NaN();
 float stableImportance=std::numeric_limits<float>::quiet_NaN();
 float subjectCoverage=std::numeric_limits<float>::quiet_NaN();
 float roadCoverage=std::numeric_limits<float>::quiet_NaN();
};
// Select only actually chosen references. An empty record is not a winner.
// Same-slot replacement is a reference, not proof of causal eviction. Opaque
// key aliases still need the runtime multiplicity flag, never identity authority.
template<class Selection, class Slot>
inline int SelectionLossReference38(const Selection& chosen,const Slot& old,
 unsigned oldSlot) noexcept {
 if(oldSlot<7 && (chosen.validMask&(1u<<oldSlot)) && chosen.slots[oldSlot].key &&
    (chosen.slots[oldSlot].key!=old.key || chosen.slots[oldSlot].generation!=old.generation))return int(oldSlot);
 for(unsigned i=0;i<7;++i)if((chosen.validMask&(1u<<i)) && chosen.slots[i].key &&
    (chosen.slots[i].key!=old.key || chosen.slots[i].generation!=old.generation))return int(i);
 return -1;
}
}
