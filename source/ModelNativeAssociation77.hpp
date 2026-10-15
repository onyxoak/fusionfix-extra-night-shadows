#pragma once
#include <cstdint>
namespace model_native_association77 {
// Authenticated CE Store6BCCD0 classification changes ONLY these two input
// flag bits at6BCF49/6BCF98/6BCFA0, then stores them at6BCFB3. The constructor
// sees that native result, not the original model callsite scalar. This does
// not alter either scalar or authorize a different caller/invocation.
inline constexpr uint32_t ClassificationBits=0x60u;
inline uintptr_t ReturnSlot(uintptr_t argumentBase,bool wrapper) noexcept {
 const uintptr_t adjustment=wrapper?0x54u:8u;
 if(argumentBase<adjustment+0x128u)return 0;
 return ((argumentBase-adjustment)&~uintptr_t(15))-0x128u;
}
inline bool SameFlags(uint32_t original,uint32_t normalized,bool wrapper) noexcept {
 if(wrapper)original&=~8u; // Existing6BCC66 wrapper normalization.
 return ((original^normalized)&~ClassificationBits)==0;
}
}
