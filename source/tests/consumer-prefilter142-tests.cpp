#define NOMINMAX
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <bit>
#include <cstring>
#include <iostream>
#include "core/ShadowFloatingPointState.hpp"
#include "safetyhook/context.hpp"
using SafetyHookContext=safetyhook::Context32;
namespace ModelEffectObserver {struct LastErrorScope{DWORD value=GetLastError();~LastErrorScope(){SetLastError(value);}};}
namespace CShadows {uint32_t frame=1;uint32_t* pFrameCounter=&frame;}
namespace ShadowTrace34 {struct Event{uint32_t stage,frame,tick,key;int32_t a,b,c,d;float x,y,z,w;};unsigned applied=0;void Emit(Event){++applied;}}
std::atomic<bool> addonActive{true};uintptr_t game{};
namespace Liberty20 {
unsigned reads=0;bool Copy(uintptr_t p,void* out,size_t n)noexcept{++reads;std::memcpy(out,reinterpret_cast<void*>(p),n);return true;}
}
#include "ConsumerReachRuntime136.inc"
#define ConsumerReach136 ConsumerBaseline136
#include "consumer136-baseline.inc"
#undef ConsumerReach136
unsigned authorized=0;bool permit=true;
bool ConsumerReach136::Authorize(const consumer_shadow75::Packet&,uint32_t)noexcept{++authorized;return permit;}
bool ConsumerBaseline136::Authorize(const consumer_shadow75::Packet&,uint32_t)noexcept{++authorized;return permit;}
static consumer_shadow75::Packet fixturePacket{};static std::array<uint8_t,0x140> stack{};static std::array<uint8_t,0x300> camera{};static float global=1;
static uintptr_t gateEsp(){return reinterpret_cast<uintptr_t>(stack.data()+0x40);}
static void Init(uint32_t flags=69,float ratio=10.f/31,uint32_t lookup=1){
 fixturePacket={};fixturePacket.words[17]=0;fixturePacket.words[18]=flags;fixturePacket.words[21]=std::bit_cast<uint32_t>(10.f);fixturePacket.words[16]=std::bit_cast<uint32_t>(1.f);fixturePacket.words[24]=123;
 for(unsigned i=12;i<15;++i)fixturePacket.words[i]=std::bit_cast<uint32_t>(1.f);
 stack.fill(0);camera.fill(0);const uint32_t src=uint32_t(reinterpret_cast<uintptr_t>(&fixturePacket)+0x28),cam=uint32_t(reinterpret_cast<uintptr_t>(camera.data()));const float radius=10,pos=31,scale=.339770854f;
 const uintptr_t s=gateEsp();std::memcpy(reinterpret_cast<void*>(s+0x1c),&src,4);std::memcpy(reinterpret_cast<void*>(s+0x28),&cam,4);std::memcpy(reinterpret_cast<void*>(s+0x14),&radius,4);*reinterpret_cast<uint8_t*>(s+0xd)=lookup!=UINT32_MAX;
 std::memcpy(camera.data()+0x70,&pos,4);std::memcpy(camera.data()+0x2cc,&scale,4);
 game=reinterpret_cast<uintptr_t>(&global)-0xc3efdc;ConsumerReach136::requested=ConsumerBaseline136::requested=true;ConsumerReach136::context={};ConsumerBaseline136::context={};authorized=Liberty20::reads=ShadowTrace34::applied=0;permit=true;
 (void)ratio;
}
static unsigned Run(bool baseline,uint32_t flags,float ratio,uint32_t lookup=1){
 Init(flags,ratio,lookup);SafetyHookContext l{};l.edi=uint32_t(reinterpret_cast<uintptr_t>(&fixturePacket)+0x28);l.esp=uint32_t(gateEsp()-12);l.eax=lookup;
 if(baseline)ConsumerBaseline136::Observe(consumer_shadow75::Stage::Lookup,l);else ConsumerReach136::Observe(consumer_shadow75::Stage::Lookup,l);
 SafetyHookContext g{};g.esp=uint32_t(gateEsp());g.xmm0.f32[0]=.339770854f;g.xmm1.f32[0]=ratio;
 if(baseline)ConsumerBaseline136::Observe(consumer_shadow75::Stage::Gate,g);else ConsumerReach136::Observe(consumer_shadow75::Stage::Gate,g);
 return Liberty20::reads;
}
int main(){
 const auto oldMissing=Run(true,69,10.f/31,UINT32_MAX);assert(authorized==0);const auto newMissing=Run(false,69,10.f/31,UINT32_MAX);assert(newMissing==0&&authorized==0);
 const auto oldFar=Run(true,69,.1f);assert(authorized==0);const auto newFar=Run(false,69,.1f);assert(newFar==2&&authorized==0);
 const auto oldBeam=Run(true,0x564,10.f/31);assert(authorized==0);const auto newBeam=Run(false,0x564,10.f/31);assert(newBeam==2&&authorized==0);
 Run(false,69,10.f/31);assert(authorized==1&&ShadowTrace34::applied==1&&*reinterpret_cast<uint8_t*>(gateEsp()+0xe)==1);
 Run(true,69,10.f/31);assert(authorized==1&&ShadowTrace34::applied==1&&*reinterpret_cast<uint8_t*>(gateEsp()+0xe)==1);
 Init();ConsumerReach136::requested=false;SafetyHookContext r{};ConsumerReach136::Observe(consumer_shadow75::Stage::Lookup,r);assert(Liberty20::reads==0&&authorized==0);
 auto p=fixturePacket;for(auto key:{0u,UINT32_MAX}){p.words[24]=key;assert(!consumer_reach136::PacketPotential142(p));}p=fixturePacket;p.words[16]=std::bit_cast<uint32_t>(0.f);assert(!consumer_reach136::PacketPotential142(p));
 for(float value:{0.f,-1.f,INFINITY,NAN})assert(!consumer_reach136::NumericPotential142(.339770854f,value));
 std::cout<<"ACTUAL callback readcounts no-map "<<oldMissing<<" -> "<<newMissing<<"; outside-shoulder "<<oldFar<<" -> "<<newFar<<"; vehicle "<<oldBeam<<" -> "<<newBeam<<"; candidate publication identical\n";
}

