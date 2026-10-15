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
namespace ModelEffectObserver {struct LastErrorScope {DWORD value=GetLastError();~LastErrorScope(){SetLastError(value);}};}
namespace CShadows {uint32_t frame=1;uint32_t* pFrameCounter=&frame;}
namespace ShadowTrace34 {struct Event{uint32_t stage,frame,tick,key;int32_t a,b,c,d;float x,y,z,w;};void Emit(Event){}}
std::atomic<bool> addonActive{true};uintptr_t game{};
namespace Liberty20 {unsigned reads=0;bool Copy(uintptr_t p,void* out,size_t n)noexcept{++reads;std::memcpy(out,reinterpret_cast<void*>(p),n);return true;}}
static uint32_t tick198=1000;static uint32_t Tick198(){return tick198;}
#define GetTickCount Tick198
#include "ConsumerReachRuntime136.inc"
#undef GetTickCount
static bool permit198=true,mutate198=false;static unsigned auth198=0;
static consumer_shadow75::Packet packet198{};
bool ConsumerReach136::Authorize(const consumer_shadow75::Packet&,uint32_t) noexcept {
 ++auth198;if(mutate198)packet198.words[21]=std::bit_cast<uint32_t>(11.f);return permit198;
}
static std::array<uint8_t,0x140> stack198{};static std::array<uint8_t,0x300> camera198{};static float scale198=1;
static uintptr_t Esp198(){return reinterpret_cast<uintptr_t>(stack198.data()+0x40);}
static void Reset198(){
 ConsumerReach136::applicationHistory198.Clear();ConsumerReach136::context={};ConsumerReach136::requested=true;ConsumerReach136::retentionRequested198=true;addonActive=true;
 packet198={};packet198.words[17]=0;packet198.words[18]=69;packet198.words[21]=std::bit_cast<uint32_t>(10.f);
 packet198.words[16]=std::bit_cast<uint32_t>(1.f);packet198.words[24]=123;
 for(unsigned i=12;i<15;++i)packet198.words[i]=std::bit_cast<uint32_t>(1.f);
 permit198=true;mutate198=false;tick198=1000;CShadows::frame=10;auth198=Liberty20::reads=0;
}
static bool Draw198(float distance,uint32_t elapsed=16,uint32_t lookup=1,bool available=true){
 tick198+=elapsed;++CShadows::frame;stack198.fill(0);camera198.fill(0);
 const uintptr_t s=Esp198();const uint32_t src=uint32_t(reinterpret_cast<uintptr_t>(&packet198)+0x28),cam=uint32_t(reinterpret_cast<uintptr_t>(camera198.data()));
 const float radius=10,threshold=1;std::memcpy(reinterpret_cast<void*>(s+0x1c),&src,4);std::memcpy(reinterpret_cast<void*>(s+0x28),&cam,4);
 std::memcpy(reinterpret_cast<void*>(s+0x14),&radius,4);*reinterpret_cast<uint8_t*>(s+0xd)=available;
 std::memcpy(camera198.data()+0x70,&distance,4);std::memcpy(camera198.data()+0x2cc,&threshold,4);
 game=reinterpret_cast<uintptr_t>(&scale198)-0xc3efdc;
 SafetyHookContext l{};l.esp=uint32_t(s-12);l.edi=src;l.eax=lookup;
 ConsumerReach136::Observe(consumer_shadow75::Stage::Lookup,l);
 SafetyHookContext g{};g.esp=uint32_t(s);g.xmm0.f32[0]=threshold;g.xmm1.f32[0]=radius/distance;
 ConsumerReach136::Observe(consumer_shadow75::Stage::Gate,g);
 return *reinterpret_cast<uint8_t*>(s+0xe)!=0;
}
int main(){
 // The real production callback removes ten threshold toggles in this bounded
 // oscillation. Baseline195 makes the geometric1.30 cutoff every other frame.
 Reset198();unsigned baselineToggles=0,candidateToggles=0;bool oldLast=false,newLast=false;
 for(unsigned n=0;n<20;++n){const float distance=n%2?13.2f:12.9f;const bool old=distance<=13.f,now=Draw198(distance);
  if(n){baselineToggles+=old!=oldLast;candidateToggles+=now!=newLast;}oldLast=old;newLast=now;assert(now);}
 assert(baselineToggles==19&&candidateToggles==0);
 Reset198();assert(!Draw198(13.2f)&&auth198==0);assert(Draw198(12.9f));assert(Draw198(14.f));assert(!Draw198(std::nextafter(14.f,15.f)));
 Reset198();assert(Draw198(12.9f));assert(Draw198(13.2f,180));assert(!Draw198(13.2f,1)); // no self renewal
 Reset198();assert(Draw198(12.9f));CShadows::frame+=13;assert(!Draw198(13.2f));
 Reset198();assert(Draw198(12.9f));CShadows::frame=0;assert(!Draw198(13.2f));
 Reset198();assert(Draw198(12.9f));assert(!Draw198(13.2f,16,UINT32_MAX));assert(!Draw198(13.2f,16,1,false)); // no map, no grant
 Reset198();assert(Draw198(12.9f));packet198.words[24]++;assert(!Draw198(13.2f)); // another light
 Reset198();assert(Draw198(12.9f));packet198.words[8]=std::bit_cast<uint32_t>(1.f);assert(!Draw198(13.2f)); // moved/recycled packet
 Reset198();assert(Draw198(12.9f));permit198=false;assert(!Draw198(13.2f)); // fresh owner refuses
 Reset198();assert(Draw198(12.9f));mutate198=true;assert(!Draw198(13.2f)); // final packet reread refuses
 for(auto flags:{0x104u,0x504u,0x404u}){Reset198();packet198.words[18]=flags;assert(!Draw198(12.9f)&&!Draw198(13.2f)&&auth198==0);}
 Reset198();assert(Draw198(12.9f));ConsumerReach136::retentionRequested198=false;assert(!Draw198(13.2f));
 ConsumerReach136::retentionRequested198=true;assert(!Draw198(13.2f)); // disabled interval cannot revive history
 Reset198();ConsumerReach136::retentionRequested198=false;assert(Draw198(12.9f));assert(!Draw198(13.2f)); // ordinary reach remains
 Reset198();assert(Draw198(12.9f));ConsumerReach136::requested=false;assert(!Draw198(13.2f));ConsumerReach136::requested=true;assert(!Draw198(13.2f));
 Reset198();assert(Draw198(12.9f));addonActive=false;assert(!Draw198(13.2f));addonActive=true;assert(!Draw198(13.2f));
 Reset198();SetLastError(198);const auto mxcsr=_mm_getcsr();assert(Draw198(12.9f));assert(GetLastError()==198&&_mm_getcsr()==mxcsr);
 // Failed writes must never seed a future extension.
 consumer_reach136::ApplicationHistory198 history;auto p=packet198;
 assert(!history.Retained(p,1,1));history.AppliedInside(p,1,1);assert(history.Retained(p,2,100));
 p.words[24]=124;assert(!history.Retained(p,2,100));assert(sizeof(history)<5000);
 std::cout<<"ACTUAL Observe: oscillating threshold toggles "<<baselineToggles<<" -> "<<candidateToggles<<"; acquisition unchanged; 180ms/1.40 max; map, owner, packet, frame, vehicle, disabled refusals PASS. No rendered fade/FPS proof.\n";
}
