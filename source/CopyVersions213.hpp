#pragma once
#include <cstdint>
#include <cstddef>
#include <string_view>
namespace copy_versions213 {
struct Layout {std::string_view version; uint32_t callback,allocation; size_t size,slot,invoke;};
inline constexpr Layout Layouts[]{
 {"5.1.1",0x608b0,0x3325f8,0x113,0x57,0x70},
 {"5.1.0",0x60830,0x32b5f8,0x113,0x57,0x70},
 {"5.0.1",0x5cb30,0x33b028,0xf2,0x56,0x6f}};
inline const Layout* Find(std::string_view version){for(const auto& l:Layouts)if(l.version==version)return &l;return nullptr;}
}
