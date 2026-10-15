#include <array>
#include <atomic>
#include <cassert>
#include <cstring>
#include <bit>
#include "core/IndoorBubble212.hpp"
static std::array<unsigned char,0x30> ped212{};
static std::array<float,16> matrix212{};
static bool inCar212=false,deny212=false,move212=false;
static unsigned positionReads212=0;
namespace CPlayer {
 uintptr_t Ped212(){return reinterpret_cast<uintptr_t>(ped212.data());}
 uintptr_t Car212(){return inCar212?0x20000:0;}
 inline uintptr_t(*getLocalPlayerPed)()=Ped212;
 inline uintptr_t(*findPlayerCar)()=Car212;
}
namespace CShadows {uint32_t frame212=10;uint32_t* pFrameCounter=&frame212;}
std::atomic<bool> addonActive=true;
namespace Liberty20 {
 bool Copy(uintptr_t p,void* out,size_t n)noexcept {
  if(deny212)return false;
  const auto ped=reinterpret_cast<uintptr_t>(ped212.data()),matrix=reinterpret_cast<uintptr_t>(matrix212.data());
  if(p==ped+0x20&&n==4){std::memcpy(out,ped212.data()+0x20,4);return true;}
  if(p==matrix+0x30&&n==12){++positionReads212;if(move212&&positionReads212==2)matrix212[12]+=1;std::memcpy(out,matrix212.data()+12,12);return true;}
  return false;
 }
}
#include "IndoorBubble212Runtime.inc"
int main(){
 const auto matrix=uint32_t(reinterpret_cast<uintptr_t>(matrix212.data()));std::memcpy(ped212.data()+0x20,&matrix,4);
 consumer_shadow75::Packet p{};p.words[24]=7;p.words[26]=19719;p.words[27]=1;p.words[8]=std::bit_cast<uint32_t>(6.096f);
 assert(IndoorBubble212::Strength(p,10)<0);IndoorBubble212::requested=true;
 assert(IndoorBubble212::Strength(p,10)==1);assert(IndoorBubble212::Strength(p,11)<0);
 inCar212=true;assert(IndoorBubble212::Strength(p,10)<0);inCar212=false;
 p.words[26]=0;assert(IndoorBubble212::Strength(p,10)<0);p.words[26]=19719;
 p.words[18]=0x100;assert(IndoorBubble212::Strength(p,10)<0);p.words[18]=0;
 deny212=true;assert(IndoorBubble212::Strength(p,10)<0);deny212=false;
 positionReads212=0;move212=true;assert(IndoorBubble212::Strength(p,10)<0);move212=false;matrix212[12]=0;
 p.words[8]=std::bit_cast<uint32_t>(indoor212::Feet35);assert(IndoorBubble212::Strength(p,10)<0);
 addonActive=false;p.words[8]=0;assert(IndoorBubble212::Strength(p,10)<0);
}
