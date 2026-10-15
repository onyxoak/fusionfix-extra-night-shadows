#include "core/SceneShadowPriority21.hpp"
#include "core/NativeShadowContinuity42.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <limits>
#include <cstdio>
using namespace fusionfix::shadows;
static SceneReceiverConsequence44 Impact(float d2,float r,float road=-1.f,float coverage=1.f,bool drive=true){
 return SceneConsequence44(d2,road<0?d2:road,r,20.f,{.37f,.91f,.92f},1.f,true,drive,coverage,coverage,1.5f);
}
int main(){
 // Tall floodlight within its authored influence: compare normalized geometry
 // rather than punishing height with both absolute local fade and final d2.
 auto tall=Impact(32.f*32.f,50.f),nearOutside=Impact(15.f*15.f,8.f);
 assert(tall.valid&&nearOutside.valid&&tall.subjectSupport>0&&nearOutside.subjectSupport==0);
 assert(tall.score<nearOutside.score);
 assert(SceneRadiusSupport44(25.f,2.f,1.5f)==0.f);
 // Same normalized radius relationship is comparable across fixture height.
 auto a=Impact(10.f*10.f,20.f),b=Impact(20.f*20.f,40.f);
 assert(a.subjectSupport>0&&b.subjectSupport>0&&b.score<a.score*1.5f);
 // Rear fixture origin need not be visible: central receiver support dominates
 // the camera factor. An incoming lamp cannot win solely from viewweight3.
 auto rear=SceneConsequence44(8.f*8.f,20.f*20.f,20.f,35.f,{1.f,.61f,.14f},1.f,true,true,1.f,1.f,1.5f);
 auto front=SceneConsequence44(25.f*25.f,12.f*12.f,20.f,35.f,{1.f,.61f,.14f},3.f,true,true,1.f,1.f,1.5f);
 assert(rear.score<front.score);
 auto oc=SceneConsequence44(25.f,25.f,20.f,35.f,{1,1,1},3.f,true,true,0.f,0.f,1.5f);
 assert(oc.subjectSupport==0.f&&oc.roadSupport==0.f&&oc.consequence==0.f);
 auto foot=Impact(25.f,20.f,0.f,1.f,false);assert(foot.roadSupport==0.f);
 assert(!Impact(1.f,0.f).valid);
 auto nan=std::numeric_limits<float>::quiet_NaN();assert(!Impact(nan,20.f).valid);
 assert(!SceneConsequence44(1,1,20,20,{nan,1,1},1,true,true,1,1,1.5f).valid);
 using C=NativeShadowContinuity42;
 auto candidate=[](unsigned key,float score,bool own=false,unsigned order=0){C::Candidate c{};c.flags=4;c.observed=true;c.identity=key;c.focusTier=1;c.focusDistance=score;c.ownBeam=own;c.stableOrder=order;return c;};
 auto tinyOwn=candidate(1,nearOutside.score,true),world=candidate(2,tall.score);
 assert(C::Compare(1,world,tinyOwn)==0&&C::Compare(1,tinyOwn,world)==2);
 auto physical=candidate(3,1000.f,true,1);assert(C::Compare(1,physical,world)==0);
 auto unobserved=world;unobserved.observed=false;assert(C::Compare(1,unobserved,tinyOwn)==1);
 auto native=world;native.flags|=0x400;assert(C::Compare(2,native,tinyOwn)==2);
 // Exhaust all six-element permutations. One candidate's score/retention is
 // intrinsic, independent of which challenger it was compared against.
 std::array<C::Candidate,6> base{{candidate(1,rear.score),candidate(2,front.score),candidate(3,tall.score),candidate(4,nearOutside.score),candidate(5,foot.score),candidate(6,oc.score)}};
 base[1].claim=0;base[1].visualImportance=2.f;
 std::array<unsigned,6> order{{0,1,2,3,4,5}},expected{};unsigned n=0;
 do{std::array<C::Candidate,6> current{};for(unsigned i=0;i<6;++i)current[i]=base[order[i]];
 std::sort(current.begin(),current.end(),[](auto x,auto y){return C::Compare(1,x,y)==0;});
 std::array<unsigned,6> got{};for(unsigned i=0;i<6;++i)got[i]=current[i].identity;
 if(n==0)expected=got;else assert(got==expected);++n;
 }while(std::next_permutation(order.begin(),order.end()));assert(n==720);
 for(auto x:base)for(auto y:base)for(auto z:base)
  if(C::Compare(1,x,y)==0&&C::Compare(1,y,z)==0)assert(C::Compare(1,x,z)==0);
 std::puts("receiver consequence44: tall floodlight, physical overlap, cone/radius exclusion, foot, own-source authority, invalid fallback and720 permutations passed");
}
