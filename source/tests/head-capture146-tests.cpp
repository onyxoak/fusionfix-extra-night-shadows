#include <windows.h>
#include <array>
#include <atomic>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <vector>
#include "core/HeadlightCasterPolicy.hpp"
#include "core/ShadowCasterCE.hpp"
namespace experiment_perf {template<class T>void Count(bool active,T& v){if(active)++v;}}
namespace ShadowTrace34 {struct Event{uint32_t a,b,c,d;int e,f,g,h;float i,j,k,l;};void Emit(Event){}}
bool bHeadlightShadows=true,bVehicleNightShadows=true,bTrafficSelfShadowFix=true;
namespace CShadows {uint32_t frame=9;uint32_t* pFrameCounter=&frame;struct Stable{bool submitted=true;bool IsSubmittedBeam(uintptr_t)const{return submitted;}}gStableHeadlightShadow;}
namespace CPlayer {uintptr_t current=0;uintptr_t Find(){return current;}uintptr_t(*findPlayerCar)()=Find;}
unsigned validations=0,fallcalls=0;uintptr_t witnessOwner=0,fallbackOwner=0;uint32_t privateKey=0x77777000;
uintptr_t Owner(uintptr_t key)noexcept{++validations;return key==privateKey?witnessOwner:0;}
bool Beam(uintptr_t key,uintptr_t owner)noexcept{++validations;return key==privateKey&&witnessOwner&&owner==witnessOwner;}
fusionfix::shadows::ce::BeamPriorityEvidence32 Priority(uintptr_t key)noexcept{++validations;return {key==privateKey?witnessOwner:0,7};}
volatile unsigned foreignVisits146=0;fusionfix::shadows::ce::BeamPriorityEvidence32 ForeignPriority(uintptr_t key)noexcept{foreignVisits146=foreignVisits146+1;return Priority(key);}
uintptr_t Fallback(uintptr_t)noexcept{++fallcalls;return fallbackOwner;}
#include "head-capture143-baseline.inc"
#include "head-capture146-extracted.inc"
using Context=fusionfix::shadows::caster::Context;
bool Equal(const Context&a,const Context&b){return a.car==b.car&&a.occupants==b.occupants&&a.ownBeam==b.ownBeam&&a.trafficBeamKey==b.trafficBeamKey&&a.trafficBeamOwner==b.trafficBeamOwner;}
int main(){
 namespace ce=fusionfix::shadows::ce;namespace g=ce::casterguard;
 std::vector<uint8_t> image(0x1200000),pass(0x944),car(0x2000);auto base=reinterpret_cast<uintptr_t>(image.data());auto carAt=reinterpret_cast<uintptr_t>(car.data());
 Baseline143::base=Candidate146::base=base;Baseline143::enabled=Candidate146::enabled=true;
 ce::AdditionalVehicleBeamOwner=Owner;ce::AdditionalVehicleBeam=Beam;ce::AdditionalVehicleBeamPriority32=Priority;
 Candidate146::singlePriority146=Priority;Candidate146::singleOwner146=Owner;Candidate146::singleBeam146=Beam;
 std::array<uintptr_t,9> occupants{1,2,3,4,5,6,7,8,9};std::memcpy(car.data()+0xf50,occupants.data(),sizeof(occupants));
 auto put=[&](size_t at,auto value){std::memcpy(image.data()+at,&value,sizeof(value));};auto passput=[&](size_t at,auto value){std::memcpy(pass.data()+at,&value,sizeof(value));};passput(0x938,int32_t(1));passput(0x940,uint32_t(1));
 unsigned cases=0,oldOwn=0,newOwn=0;
 for(unsigned flags=0;flags<16;++flags)for(unsigned occupied=0;occupied<2;++occupied)for(unsigned keyCase=0;keyCase<4;++keyCase)for(unsigned ownerCase=0;ownerCase<3;++ownerCase)for(unsigned foreign=0;foreign<2;++foreign){
  bHeadlightShadows=flags&1;bVehicleNightShadows=flags&2;bTrafficSelfShadowFix=flags&4;Baseline143::additionalCasterOwner=Candidate146::additionalCasterOwner=(flags&8)?Fallback:nullptr;
  fallbackOwner=0x99888000;CPlayer::current=occupied?carAt:0;witnessOwner=ownerCase==0?0:ownerCase==1?carAt:0x11222000;
  uint32_t key=keyCase==0?privateKey:keyCase==1?uint32_t(carAt):keyCase==2?uint32_t(carAt+1):0x55666000;
  put(g::DeferredFlagRva,uint8_t(0));put(g::SlotActiveRva+0x110,uint8_t(1));put(g::SlotKindRva+0x110,uint32_t(4));put(g::SlotKeyRva+0x110,key);
  ce::AdditionalVehicleBeamPriority32=foreign?ForeignPriority:Priority;
  validations=fallcalls=0;auto old=Baseline143::Capture(pass.data());const auto oldReads=validations,oldFallback=fallcalls;
  validations=fallcalls=0;auto next=Candidate146::Capture(pass.data());const auto newReads=validations;assert(Equal(old,next)&&oldFallback==fallcalls);assert(newReads<=oldReads);
  for(auto entity:{uintptr_t(0),carAt,uintptr_t(0x11222000),uintptr_t(1),uintptr_t(0xabcdef)})for(unsigned type=1;type<5;++type)for(bool artificial:{false,true})assert(fusionfix::shadows::caster::Exclude(old,entity,type,artificial)==fusionfix::shadows::caster::Exclude(next,entity,type,artificial));
  if(foreign)assert(oldReads==newReads);
  if(flags==7&&occupied&&keyCase==0&&ownerCase==1&&!foreign){oldOwn=oldReads;newOwn=newReads;assert(oldReads==2&&newReads==1);}++cases;
 }
 // Off/deferred/inactive and nonheadlight kinds retain original return values.
 for(unsigned kind:{3u,5u}){put(g::SlotKindRva+0x110,kind);assert(Equal(Baseline143::Capture(pass.data()),Candidate146::Capture(pass.data())));}
 put(g::DeferredFlagRva,uint8_t(1));assert(Equal(Baseline143::Capture(pass.data()),Candidate146::Capture(pass.data())));
 Baseline143::enabled=Candidate146::enabled=false;assert(Equal(Baseline143::Capture(pass.data()),Candidate146::Capture(pass.data())));
 std::cout<<"PASS146 actual old/new Capture "<<cases<<" states; own-private validation "<<oldOwn<<" -> "<<newOwn<<"; exclusions/occupants/foreign tuple fallback preserved\n";
}


