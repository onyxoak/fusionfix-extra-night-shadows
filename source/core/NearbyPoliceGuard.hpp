#pragma once
#include "NearbyPoliceShadows.hpp"
#include <cstring>
#include <span>
namespace lab_siren {
inline bool PoliceImageGuard(std::span<const std::uint8_t> live,std::span<const std::uint8_t> expected,std::uint32_t base) noexcept {
 constexpr std::array<std::pair<std::uint32_t,unsigned>,5> ranges{{{0x6bcc50,0x7f},{0x640790,0x1e20},{0x644b36,0x35},{0x640620,0x167},{0x63eb30,17}}};
 if(live.size()!=expected.size())return false;
 for(auto [rva,size]:ranges)if(rva>live.size()||size>live.size()-rva||std::memcmp(live.data()+rva,expected.data()+rva,size))return false;
 const auto word=[&](unsigned i){std::uint32_t value{};std::memcpy(&value,live.data()+i,4);return value;};
 if(word(0x63eb30+7)!=base+0xe95cd8)return false;
 for(auto site:PoliceSites)if(live[site]!=0xe8||site+5+word(site+1)!=0x6bcc50)return false;
 return true;
}
}
