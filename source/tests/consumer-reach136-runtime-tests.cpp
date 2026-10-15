#define NOMINMAX
#include <windows.h>
#include <cassert>
#include <cstring>
#include "core/standalone-game-abi.hpp"
#include "core/SceneShadowPriority21.hpp"
#include "core/ShadowVolumeVisibility43.hpp"
#include "core/ConsumerShadow75.hpp"
#include "core/VisibleReceiverRanking133.hpp"
namespace CShadows {inline uint32_t frame=1;inline uint32_t* pFrameCounter=&frame;}
namespace Liberty20 {inline unsigned calls=0,failRead=0;bool Copy(uintptr_t address,void* out,size_t size)noexcept{++calls;if(failRead&&calls==failRead)return false;std::memcpy(out,reinterpret_cast<void*>(address),size);return true;}}
namespace PlayerShadowAllocation {
 inline bool visibleReceiverExperiment133=true;inline rage::CLightSource* bank{};inline uint32_t count{};inline rage::grcViewport* viewport{};inline bool validView=true;
 rage::CLightSource* CurrentLights(){return bank;}uint32_t CurrentCount(){return count;}const rage::grcViewport* SceneViewport(){return viewport;}
 fusionfix::shadows::ShadowView ReadView(const rage::grcViewport*){fusionfix::shadows::ShadowView v{};v.valid=validView;for(unsigned i=0;i<4;++i)v.view[i][i]=v.projection[i][i]=1;return v;}
}
inline unsigned ownerCalls=0,denyOwnerAt=0;inline bool staleFrame=false;
bool VisibleReceiverSource133(const rage::CLightSource& l,unsigned,rage::CLightSource*,uint32_t,uint32_t)noexcept{++ownerCalls;if(staleFrame)++CShadows::frame;return (l.mFlags&0x500)==0&&ownerCalls!=denyOwnerAt;}
namespace ConsumerReach136 {inline bool requested=true;bool Authorize(const consumer_shadow75::Packet&,uint32_t)noexcept;}
#include "consumer136-auth.inc"
int main(){
 static std::array<rage::CLightSource,2> bank{};static rage::grcViewport viewport{};
 using namespace PlayerShadowAllocation;PlayerShadowAllocation::bank=bank.data();PlayerShadowAllocation::count=1;PlayerShadowAllocation::viewport=&viewport;
 auto& l=bank[0];l.mCastShadows=123;l.mFlags=69;l.mType=rage::LT_POINT;l.mRadius=10;l.mIntensity=1;l.mColor={1,1,1,1};
 consumer_shadow75::Packet p{};std::memcpy(&p,&l,sizeof(p));addonActive=true;
 auto reset=[&]{++CShadows::frame;ConsumerReach136::requested=true;visibleReceiverExperiment133=true;Liberty20::calls=Liberty20::failRead=0;ownerCalls=denyOwnerAt=0;staleFrame=false;validView=true;PlayerShadowAllocation::count=1;l.mFlags=69;};
 reset();assert(ConsumerReach136::Authorize(p,CShadows::frame)&&ownerCalls==2);
 reset();ConsumerReach136::requested=false;assert(!ConsumerReach136::Authorize(p,CShadows::frame)&&Liberty20::calls==0);
 reset();visibleReceiverExperiment133=false;assert(!ConsumerReach136::Authorize(p,CShadows::frame)&&Liberty20::calls==0);
 reset();assert(!ConsumerReach136::Authorize(p,CShadows::frame+1)&&Liberty20::calls==0);
 reset();bank[1]=bank[0];PlayerShadowAllocation::count=2;assert(!ConsumerReach136::Authorize(p,CShadows::frame)&&ownerCalls==0);
 reset();l.mPosition.x=1;assert(!ConsumerReach136::Authorize(p,CShadows::frame)&&ownerCalls==0);l.mPosition.x=0;
 reset();denyOwnerAt=1;assert(!ConsumerReach136::Authorize(p,CShadows::frame));
 reset();denyOwnerAt=2;assert(!ConsumerReach136::Authorize(p,CShadows::frame));
 reset();staleFrame=true;assert(!ConsumerReach136::Authorize(p,CShadows::frame));
 reset();validView=false;assert(!ConsumerReach136::Authorize(p,CShadows::frame));
 for(unsigned i=1;i<=4;++i){reset();Liberty20::failRead=i;assert(!ConsumerReach136::Authorize(p,CShadows::frame));}
 reset();l.mFlags=0x104;std::memcpy(&p,&l,sizeof(p));assert(!ConsumerReach136::Authorize(p,CShadows::frame));
}

