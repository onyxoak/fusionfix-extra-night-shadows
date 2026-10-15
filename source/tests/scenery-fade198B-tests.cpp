#define NOMINMAX
#include <windows.h>
#include <map>
#include <vector>
#include <cassert>
#include <bit>
#include <iostream>
#include "core/SceneryFade198B.hpp"
#include "core/ShadowFloatingPointState.hpp"
#include "safetyhook/context.hpp"
using SafetyHookContext=safetyhook::Context32;
namespace ModelEffectObserver {struct LastErrorScope{DWORD value=GetLastError();~LastErrorScope(){SetLastError(value);}};}
namespace CShadows {uint32_t frame=10;uint32_t* pFrameCounter=&frame;}
static std::map<uintptr_t,std::vector<uint8_t>> memory198;
static uintptr_t frameAddress198(){return reinterpret_cast<uintptr_t>(CShadows::pFrameCounter);}
static bool Copy198(uintptr_t p,void* out,size_t n){
 if(p==frameAddress198()&&n==4){std::memcpy(out,&CShadows::frame,4);return true;}
 auto it=memory198.upper_bound(p);if(it==memory198.begin())return false;--it;
 if(p-it->first>it->second.size()||n>it->second.size()-(p-it->first))return false;
 std::memcpy(out,it->second.data()+p-it->first,n);return true;
}
template<class T> static void Put198(uintptr_t p,T value){auto it=memory198.upper_bound(p);assert(it!=memory198.begin());--it;
 assert(p-it->first+sizeof(T)<=it->second.size());std::memcpy(it->second.data()+p-it->first,&value,sizeof(T));}
namespace Liberty20 {bool Copy(uintptr_t p,void* out,size_t n)noexcept{return Copy198(p,out,n);}}
std::atomic<bool> addonActive=true;uintptr_t game=0x400000;
static unsigned authorized198{},writes198{};static bool permit198=true,mutate198=false,write198=true;
namespace ConsumerReach136 {
 bool requested=true;consumer_shadow75::Context context;
 bool Authorize(const consumer_shadow75::Packet&,uint32_t) noexcept {++authorized198;if(mutate198)Put198(context.source,uint32_t(8));return permit198;}
}
namespace SceneryFade198B {
 std::atomic<bool> active=true;bool busy=false;
 bool WriteFloat(uintptr_t p,float v){++writes198;if(!write198)return false;Put198(p,v);return true;}
 #include "binder198B-extracted.inc"
}
static SafetyHookContext Reset198(float distance=12.f,uint32_t index=8){
 memory198.clear();memory198[0x300000]=std::vector<uint8_t>(0x200);memory198[0x500000]=std::vector<uint8_t>(120);
 memory198[0x600000]=std::vector<uint8_t>(0x300);memory198[game+0xd74794]=std::vector<uint8_t>(4);
 memory198[game+0xc3efdc]=std::vector<uint8_t>(4);
 const auto header=index<8?game+0xd9fa00+index*0x110:game+0xd9d0f0+(index-8)*0x100;
 memory198[header]=std::vector<uint8_t>(0x38);
 ConsumerReach136::context={};auto& c=ConsumerReach136::context;c.pending=true;c.frame=10;c.anchor=0x300100;c.source=0x500000;c.sample.lookup=int32_t(index);
 c.packet.words[17]=0;c.packet.words[18]=69;c.packet.words[21]=std::bit_cast<uint32_t>(10.f);c.packet.words[16]=std::bit_cast<uint32_t>(1.f);c.packet.words[24]=123;
 for(unsigned i=12;i<15;++i)c.packet.words[i]=std::bit_cast<uint32_t>(1.f);
 std::memcpy(memory198[c.source].data(),&c.packet,sizeof(c.packet));
 SafetyHookContext r{};r.ebp=0x3000f4;r.esp=0x3000b4;r.esi=uint32_t(header);
 Put198(r.ebp+4,uint32_t(game+0x6c124f));Put198(r.ebp+8,index);Put198(r.esp+8,uint32_t(r.esp+0x28));
 Put198(c.anchor+0x1c,uint32_t(c.source+0x28));Put198(c.anchor+0x28,uint32_t(0x600000));Put198(c.anchor+0x14,10.f);
 Put198(0x600070,distance);Put198(0x6002cc,1.f);Put198(game+0xc3efdc,1.f);
 Put198(header+8,uint32_t(512));Put198(header+0x10,256.f);Put198(header+0x14,10.f);
 Put198(r.esp+0x2c,512.f);Put198(r.esp+0x30,256.f);Put198(r.esp+0x34,10.f);
 CShadows::frame=10;ConsumerReach136::requested=true;SceneryFade198B::active=true;addonActive=true;SceneryFade198B::busy=false;
 authorized198=writes198=0;permit198=write198=true;mutate198=false;return r;
}
int main(){
 for(auto index:{0u,7u,8u,15u}){
  auto r=Reset198(12,index);const auto before=memory198;SetLastError(198);const auto mxcsr=_mm_getcsr();SceneryFade198B::Binder(r);
  assert(writes198==1&&authorized198==1&&GetLastError()==198&&_mm_getcsr()==mxcsr);
  float marker{};assert(Copy198(r.esp+0x28,&marker,4));assert(std::abs(marker+198.5f)<.0001f);
  auto expected=before;std::memcpy(expected[0x300000].data()+r.esp+0x28-0x300000,&marker,4);assert(memory198==expected);
 }
 for(unsigned scenario=0;scenario<16;++scenario){auto r=Reset198();auto& c=ConsumerReach136::context;
  if(scenario==0)SceneryFade198B::active=false;if(scenario==1)addonActive=false;if(scenario==2)ConsumerReach136::requested=false;
  if(scenario==3)c.sample.lookup=-1;if(scenario==4)Put198(r.ebp+4,uint32_t(1));if(scenario==5)++r.ebp;
  if(scenario==6)Put198(r.esp+8,uint32_t(r.esp+0x2c));if(scenario==7)++r.esi;
  if(scenario==8)Put198(r.esp+0x28,-198.f);if(scenario==9)permit198=false;if(scenario==10)mutate198=true;
  if(scenario==11)++CShadows::frame;if(scenario==12)Put198(0x600070,INFINITY);if(scenario==13)Put198(0x600070,10.9f);
  if(scenario==14)memory198.erase(0x600000);if(scenario==15){c.packet.words[18]=0x104;Put198(c.source+18*4,uint32_t(0x104));}
  SceneryFade198B::Binder(r);assert(writes198==0);
 }
 auto r=Reset198();write198=false;SceneryFade198B::Binder(r);float marker{};Copy198(r.esp+0x28,&marker,4);assert(marker==0);
 for(float bad:{0.f,-1.f,INFINITY,NAN})assert(scenery_fade198::Strength(10,bad,1)<0);
 assert(scenery_fade198::Strength(10,11,1)>.999f);assert(scenery_fade198::Strength(10,13,1)<.001f);
 r=Reset198(13.5f);SceneryFade198B::Binder(r);Copy198(r.esp+0x28,&marker,4);assert(writes198==1&&marker==-198.f); //198A outer hold stays invisible

 scenery_fade198::indoorStrength212=+[](const consumer_shadow75::Packet&,uint32_t)noexcept{return 1.f;};
 r=Reset198(40.f);SceneryFade198B::Binder(r);Copy198(r.esp+0x28,&marker,4);assert(writes198==1&&marker==-199.f);
 r=Reset198(40.f);permit198=false;SceneryFade198B::Binder(r);assert(writes198==0);
 r=Reset198(40.f);mutate198=true;SceneryFade198B::Binder(r);assert(writes198==0);
 scenery_fade198::indoorStrength212=nullptr;
 std::cout<<"ACTUAL Binder callback: four native map index paths; only current local scalar changed; shadow-only smoothstep;16 refusal cases and failed write; native light packet intact. Shader GPU/runtime pixels unverified.\n";
}
