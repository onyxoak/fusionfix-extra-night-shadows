#include "../core/NativeShadowContinuity42.hpp"
#include "../core/ShadowVolumeVisibility43.hpp"
#include "../core/ShadowPrioritySmoothing20.hpp"
#include <cassert>
#include <limits>
#include <cstdio>
int main(){
 using C=fusionfix::shadows::NativeShadowContinuity42;
 using namespace fusionfix::shadows;
 ShadowView view{};view.valid=true;
 for(int i=0;i<4;++i)view.view[i][i]=1;
 view.projection[0][0]=view.projection[1][1]=1;
 view.projection[2][2]=100.f/99.f;view.projection[2][3]=1;
 view.projection[3][2]=-100.f/99.f;
 // Source and beam samples behind the camera: its volume still reaches visible receivers.
 const Vec3 behind{0,0,-2};
 assert(ShadowViewWeight(view,behind,{0,0,-1},8,true)==1);
 assert(ShadowVolumeMayReachView(view,behind,8));
 assert(ShadowRelevanceTier(true,false,ShadowVolumeMayReachView(view,behind,8))==1);
 assert(!ShadowVolumeMayReachView(view,{100,0,10},5));
 assert(!ShadowVolumeMayReachView(view,behind,.5f));
 // Sparse point samples miss this grazing volume; retain conservatively.
 assert(ShadowVolumeMayReachView(view,{14,14,10},4));
 auto missing=view;missing.valid=false;
 assert(ShadowVolumeMayReachView(missing,behind,8));
 assert(ShadowRelevanceTier(false,true,true)==0);
 ShadowPrioritySmoothing20 smooth;smooth.Begin(1);
 assert(smooth.Observe(64,100,1,1000)==100);
 const auto gradual=smooth.Observe(64,80,2,1016);
 assert(gradual>95&&gradual<100);
 assert(smooth.Observe(64,80,2,1016)==gradual);
 assert(smooth.Observe(64,1,3,1032)==1); // Large scene change bypasses lag.
 assert(smooth.Observe(64,80,30,1600)==80); // Old leases cannot survive gaps.
 assert(smooth.Observe(64,50,1,1616)==50); // Rewind resets.
 smooth.Begin(2);assert(smooth.Observe(64,80,2,1632)==80);
 assert(smooth.Observe(32832,20,2,1632)==20); // Hash collision never inherits another lamp.
 C::Candidate oldLamp{6,0,false,true,0,900,1};
 C::Candidate nearLamp{6,C::Unclaimed,false,true,0,36,2};
 // Moving into a light's influence must displace scenery before stopping.
 for(int native=0;native<3;++native){
  assert(C::Compare(native,nearLamp,oldLamp)==0);
  assert(C::Compare(native,oldLamp,nearLamp)==2);
 }
 // Similar scores retain the incumbent; a substantially better one can enter.
 oldLamp.focusTier=nearLamp.focusTier=1;oldLamp.focusDistance=100;
 nearLamp.focusDistance=90;assert(C::Compare(0,nearLamp,oldLamp)==2);
 nearLamp.focusDistance=70;assert(C::Compare(2,nearLamp,oldLamp)==0);
 // Offscreen origin and onscreen origin receive the same volume relevance.
 oldLamp.focusTier=ShadowRelevanceTier(true,false,true);
 nearLamp.focusTier=ShadowRelevanceTier(true,false,true);
 nearLamp.focusDistance=90;assert(C::Compare(0,nearLamp,oldLamp)==2);
 nearLamp.focusDistance=70;
 // Camera-visible volume beats an irrelevant close source.
 oldLamp.focusTier=0;oldLamp.focusDistance=4;nearLamp.focusDistance=70;
 assert(C::Compare(2,nearLamp,oldLamp)==0);
 // The same current score governs lamp, signal and traffic-beam categories.
 nearLamp.flags=0x104;assert(C::Compare(2,nearLamp,oldLamp)==0);
 // Membership is ranked by a transitive per-light score; the separate
 // pre-cache ordinal restorer prevents selected survivors exchanging maps.
 auto trafficA=C::Candidate{0x104,1,false,true,0,100,1};
 auto trafficB=C::Candidate{0x104,2,false,true,0,95,1};
 for(int native=0;native<3;++native){
  assert(C::Compare(native,trafficB,trafficA)==0);
  assert(C::Compare(native,trafficA,trafficB)==2);
 }
 // A newcomer needs a material improvement, but does not pin distant traffic forever.
 auto incoming=C::Candidate{0x104,C::Unclaimed,false,true,0,70,1};
 assert(C::Compare(0,incoming,trafficA)==2);
 incoming.focusDistance=60;assert(C::Compare(2,incoming,trafficA)==0);
 trafficB.focusDistance=25;assert(C::Compare(2,trafficB,trafficA)==0);
 // Former relative-band cycle: 100/81/65 each pair alone appeared stable,
 // but A>B, B>C, C>A changed selection with submission order.
 auto a=C::Candidate{0x264,0,false,true,0,100,1,100};
 auto b=C::Candidate{0x264,1,false,true,0,81,1,101};
 auto c=C::Candidate{0x264,2,false,true,0,65,1,102};
 assert(C::Compare(1,c,b)==0 && C::Compare(1,b,a)==0 && C::Compare(1,c,a)==0);
 // Equal-score newcomers use source identity rather than traversal order.
 a.claim=b.claim=C::Unclaimed;a.focusDistance=b.focusDistance=50;
 assert(C::Compare(1,a,b)==0 && C::Compare(1,b,a)==2);
 auto own=C::Candidate{0x504,0,true,true,1,25,2};
 assert(C::Compare(2,nearLamp,own)==2); // Special native priority remains.
 assert(C::Compare(0,own,nearLamp)==0);
 // No score evidence retains legacy comparison; NaN cannot acquire priority.
 nearLamp.focusDistance=std::numeric_limits<float>::quiet_NaN();
 assert(C::Compare(1,nearLamp,oldLamp)==2); // Legacy visible claim remains.
 nearLamp.focusDistance=-1;assert(C::Compare(1,nearLamp,oldLamp)==2);
 std::puts("moving player influence, camera relevance, bounded retention and headlight priority checks passed");
}
