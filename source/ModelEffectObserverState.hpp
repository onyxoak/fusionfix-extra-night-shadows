#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace model_effect_observer {
inline constexpr std::uint32_t DispatchCall = 0x81e1a4, DispatchReturn = 0x81e1a9;
// Hook one complete five-byte instruction before the two final PUSHes. A mid
// hook on CALL relocates that CALL and changes its inner return address.
inline constexpr std::uint32_t PreSite = 0x81e19d, PreStackAdjustment = 8;
// Observe after caller cleanup. Relocating ADD ESP together with its following
// instruction complicates stack-sensitive return hooks; keep cleanup native.
inline constexpr std::uint32_t PostSite=0x81e1ac,PostStackAdjustment=0x34;
// EDI has just received the resolved descriptor. Observe before time/weather,
// quality and radius gates, which can return before the former 81E5BE site.
inline constexpr std::uint32_t ResolveSite = 0x81e244;
inline constexpr std::uint32_t WrapperPoint = 0x81ec1e, DirectPoint = 0x81edd7;
inline constexpr std::size_t MaxDepth = 8;
// Offsets refer to ESP BEFORE call, not callee-entry ESP.
inline constexpr std::size_t PreOrdinalOffset = 0x58, PreModelOffset = 0x60;
// Outer local +0x28 (pre-call +0x5c) is overwritten with a fade float.
// It MUST NOT be read as a descriptor pointer. Bind descriptor at Resolve.
inline constexpr std::size_t PreKeyOffset = 0x18;
constexpr std::size_t StoreWordCount(std::uint32_t site) noexcept {
    return site == WrapperPoint ? 16 : site == DirectPoint ? 18 : 0;
}
constexpr std::size_t StoreKeyOffset(std::uint32_t site) noexcept {
    return site == WrapperPoint ? 0x3c : site == DirectPoint ? 0x44 : 0;
}
struct Invocation {
    std::uint32_t frame{}, nonce{}, preStack{}, owner{}, effect{}, descriptor{}, matrix{};
    std::uint32_t ordinal{}, modelInfo{}, proposedKey{};
    bool sampled{}, resolved{}, submitted{}, invalid{};
};
struct ResolveEvidence {
    std::uint32_t frame{}, preStack{}, returnRva{}, effect{}, matrix{}, savedOwner{}, savedEffect{};
    std::uint32_t descriptor{}, proposedKey{};
};
struct PointEvidence {
    std::uint32_t frame{}, preStack{}, returnRva{}, effect{}, descriptor{}, site{}, type{}, flags{}, key{};
};
enum class LeaveResult { Missing, Overflow, Valid, Invalid };
// Invocation correlation only; this type grants NO owner lifetime, eligibility,
// memory-read permission or persistent identity. Caller supplies copied scalars.
class State {
    std::array<Invocation, MaxDepth> stack_{};
    std::size_t depth_{};
    std::uint32_t overflow_{};
    bool poisoned_{};
public:
    bool Enter(Invocation value) noexcept {
        if (poisoned_) return false;
        if (overflow_ || depth_ == MaxDepth) {
            if (overflow_ == (std::numeric_limits<std::uint32_t>::max)()) poisoned_ = true;
            else ++overflow_;
            return false;
        }
        value.descriptor = 0;
        if (!value.owner || !value.effect || !value.matrix ||
            value.owner > UINT32_MAX - value.ordinal || value.proposedKey != value.owner + value.ordinal)
            value.invalid = true;
        stack_[depth_++] = value;
        return true;
    }
    std::size_t Depth() const noexcept { return depth_; }
    Invocation* Current() noexcept { return !poisoned_ && !overflow_ && depth_ ? &stack_[depth_-1] : nullptr; }
    void Invalidate() noexcept { if (auto* c = Current()) c->invalid = true; }
    bool Resolve(const ResolveEvidence& e) noexcept {
        auto* c = Current();
        if (!c || !c->sampled) return false;
        const bool ok = !c->invalid && !c->resolved && e.frame == c->frame &&
            e.preStack == c->preStack && e.returnRva == DispatchReturn && e.effect == c->effect &&
            e.matrix == c->matrix && e.savedOwner == c->owner && e.savedEffect == c->effect &&
            e.descriptor != 0 && e.proposedKey == c->proposedKey;
        if (!ok) { c->invalid = true; return false; }
        c->descriptor = e.descriptor;
        c->resolved = true;
        return true;
    }
    bool Point(const PointEvidence& e) noexcept {
        auto* c = Current();
        if (!c || !c->sampled) return false;
        const bool ok = !c->invalid && c->resolved && !c->submitted && e.frame == c->frame &&
            e.preStack == c->preStack && e.returnRva == DispatchReturn && e.effect == c->effect &&
            e.descriptor == c->descriptor && StoreWordCount(e.site) != 0 && e.type == 0 &&
            e.key == ((e.flags & 6u) ? c->proposedKey : 0u);
        if (!ok) { c->invalid = true; return false; }
        c->submitted = true;
        return true;
    }
    LeaveResult Leave(std::uint32_t preStack, std::uint32_t frame, Invocation& value) noexcept {
        if (poisoned_) return LeaveResult::Missing;
        if (overflow_) {
            // Overflow stores only a count. Never recover an outer context
            // from an outer/equal stack address or a changed frame.
            if (!depth_ || preStack >= stack_[depth_-1].preStack || frame != stack_[depth_-1].frame) {
                poisoned_ = true;
                return LeaveResult::Invalid;
            }
            --overflow_; return LeaveResult::Overflow;
        }
        if (!depth_) return LeaveResult::Missing;
        value = stack_[--depth_];
        if (value.preStack != preStack || value.frame != frame) {
            value.invalid = true;
            for (std::size_t i = 0; i < depth_; ++i) stack_[i].invalid = true;
        }
        return value.invalid ? LeaveResult::Invalid : LeaveResult::Valid;
    }
};
}
