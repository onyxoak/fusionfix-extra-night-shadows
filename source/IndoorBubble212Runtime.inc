#include "core/IndoorBubble212.hpp"
namespace IndoorBubble212 {
inline bool requested=false;
float Strength(const consumer_shadow75::Packet& packet,uint32_t frame) noexcept {
 if(!requested||!addonActive.load(std::memory_order_acquire)||!indoor212::Packet(packet)||
    !CShadows::pFrameCounter||*CShadows::pFrameCounter!=frame||
    !CPlayer::getLocalPlayerPed||!CPlayer::findPlayerCar||CPlayer::findPlayerCar())return -1;
 const auto ped=CPlayer::getLocalPlayerPed();uintptr_t matrix{};fusionfix::shadows::Vec3 position{};
 if(ped<0x10000||ped>UINT32_MAX-0x24||!Liberty20::Copy(ped+0x20,&matrix,4)||
    matrix<0x10000||matrix>UINT32_MAX-0x40||!Liberty20::Copy(matrix+0x30,&position,12))return -1;
 const fusionfix::shadows::Vec3 source{consumer_shadow75::Float(packet.words[8]),consumer_shadow75::Float(packet.words[9]),consumer_shadow75::Float(packet.words[10])};
 const float strength=indoor212::Strength(position,source);uintptr_t freshMatrix{};fusionfix::shadows::Vec3 fresh{};
 if(strength<0||*CShadows::pFrameCounter!=frame||CPlayer::findPlayerCar()||CPlayer::getLocalPlayerPed()!=ped||
    !Liberty20::Copy(ped+0x20,&freshMatrix,4)||freshMatrix!=matrix||
    !Liberty20::Copy(matrix+0x30,&fresh,12)||std::memcmp(&fresh,&position,12))return -1;
 return strength;
}
}
