#pragma once
#define BOX216_SHADER_HASH "6f17f3f64a20d534d646c8c531198fbd3e019166b41aa6ce4729b4318e429ebb"


inline constexpr char BOX227_SHADER_HASH[]="86db809154f3b1249bb4bb6ea3adaa388ff40ae5c91bb232cedc215eb09e1ba4";

#include <string_view>
namespace box_shader216 {
inline bool SupportsFallback(std::string_view hash) noexcept {
 return hash==BOX216_SHADER_HASH||hash==BOX227_SHADER_HASH;
}
inline bool SupportsSceneryFade(std::string_view hash) noexcept {
 return hash=="2c2c8711e477d0ba2ceb9bdc493fc058f8f0bb3c0d497c8338d0e0e677631cfa"||SupportsFallback(hash);
}
}
