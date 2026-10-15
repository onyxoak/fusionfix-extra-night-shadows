#pragma once
// OnyxOak / Liberty's Shadows private 2.0 candidate. GPL-3.0.
#include "VehicleLampIdentity20.hpp"
#include <cstring>
namespace tail_light20 {
constexpr uint32_t CombinedSite=0x643376,SingleSite=0x64342d,NativeColorRva=0xee2340;
struct Packet {
    std::array<uint32_t,13> words{};
    std::array<float,16> matrix{};
    liberty20::Vec3 position{},direction{},color{};
};
inline float Scalar(uint32_t bits) noexcept {float value;std::memcpy(&value,&bits,4);return value;}
inline bool Authorized(uint32_t site,uint32_t owner,uint32_t nativeColor,const Packet& p) noexcept {
    if((site!=CombinedSite&&site!=SingleSite)||!owner||owner==UINT32_MAX||
       !p.words[0]||!p.words[1]||!p.words[2]||p.words[3]!=nativeColor||
       (p.words[10]!=owner&&p.words[10]!=owner+1)||p.words[11]||p.words[12])return false;
    for(float value:p.matrix)if(!std::isfinite(value))return false;
    if(!liberty20::Finite(p.position)||!liberty20::Finite(p.direction)||!liberty20::Finite(p.color)||
       p.direction.x!=0.f||p.direction.y!=-1.f||p.direction.z!=0.f||
       p.color.x<0.f||p.color.y<0.f||p.color.z<0.f)return false;
    for(unsigned i=4;i<=7;++i){const auto value=Scalar(p.words[i]);if(!std::isfinite(value)||value<0.f)return false;}
    return Scalar(p.words[4])>0.f&&Scalar(p.words[5])>0.f;
}
inline unsigned NativeChannel(uint32_t owner,const Packet& p) noexcept {return p.words[10]==owner?0u:1u;}
// Separate family; these sources must never acquire player-headlight rank.
using Registry=liberty20::VehicleLampRegistry<>;
}
