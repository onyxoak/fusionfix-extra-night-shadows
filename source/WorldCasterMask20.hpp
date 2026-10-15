#pragma once
#include <cstdint>
namespace world_caster_mask20 {
inline constexpr std::uint32_t NativeDynamicMask=0x01800007;
inline constexpr std::uint32_t NativeBothMask=0x01800087;
inline constexpr std::uint32_t StaticBuildingBit=0x80;
inline constexpr std::uint32_t MaskWriteRva=0x9784ae;
// Native6E77F0 classifies entity type1 through6E7900. For a regular building
// (+24bit0x40 clear),6E7914 excludes the view unless mask+8E8 includes0x80.
// Native978410 supplies desired mask+8D0: kind4 omits0x80; kind5 includes it.
// Set only the stock bit shared by the native combined caster mask. Authored
// entity flags, room/frustum/material logic and the other view bits remain.
constexpr bool ShouldIncludeStatic(std::uint32_t slot,std::uint32_t kind,bool active,
 std::uint32_t key,std::uint32_t mask,bool deferred) noexcept {
 return !deferred&&slot>=1&&slot<=7&&kind==4&&active&&key&&key!=UINT32_MAX&&mask==NativeDynamicMask;
}
constexpr std::uint32_t IncludeStatic(std::uint32_t mask) noexcept{return mask|StaticBuildingBit;}
static_assert(IncludeStatic(NativeDynamicMask)==NativeBothMask);
}
