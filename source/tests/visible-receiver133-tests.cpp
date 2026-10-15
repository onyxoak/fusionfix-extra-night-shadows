#include "core/ParallelSelectionMath61.hpp"
#include "core/NativeShadowContinuity42.hpp"
#include <cassert>
#include <limits>
using namespace fusionfix::shadows;
static void Same133(const SceneReceiverConsequence44& a,const SceneReceiverConsequence44& b) {
 assert(a.valid==b.valid&&a.subjectSupport==b.subjectSupport&&a.roadSupport==b.roadSupport&&
        a.consequence==b.consequence&&a.score==b.score);
}
int main() {
 const FloatingPointState fp;
 const float reach=60.96f;
 // Actual production scenery sampler: screen-center preference is continuous,
 // while the existing vehicle sampling remains exactly uniform in the frame.
 ShadowView forward{};forward.valid=true;for(unsigned i=0;i<4;++i)forward.view[i][i]=forward.projection[i][i]=1.f;
 assert(ViewSample(forward,{0,0,.5f})==ViewSample(forward,{1,0,.5f}));
 assert(ViewSample(forward,{0,0,.5f},true)==1.f&&ViewSample(forward,{1,0,.5f},true)==.5f);
 const float center163=SceneryReceiverFactor163(forward,{0,0,.5f},{0,0,1},.1f,0);
 const float edge163=SceneryReceiverFactor163(forward,{1,0,.5f},{0,0,1},.1f,0);
 assert(center163==1.f&&edge163>=.5f&&edge163<.51f);
 float old163=center163;for(unsigned i=0;i<=100;++i){const float next163=SceneryReceiverFactor163(forward,{float(i)/100,0,.5f},{0,0,1},.1f,0);assert(next163>=.5f&&next163<=1.f&&std::abs(next163-old163)<.02f);old163=next163;}
 // Sparse misses retain half the peripheral preference and all inherited
 // central terms; no zero-sample boundary jumps back to full preference.
 assert(SceneryReceiverFactor163(forward,{0,0,-10},{0,0,1},.1f,0)==.5f);
 ShadowView missing{};assert(SceneryReceiverFactor163(missing,{0,0,.5f},{0,0,1},.1f,0)==1.f);
 missing.valid=true;assert(SceneryReceiverFactor163(missing,{0,0,.5f},{0,0,1},.1f,0)==1.f);
 old163=SceneryReceiverFactor163(forward,{1,0,.5f},{0,0,1},.1f,0);
 for(unsigned i=1;i<=100;++i){const float factor163=SceneryReceiverFactor163(forward,{1.f+float(i)/100,0,.5f},{0,0,1},.1f,0);assert(factor163>=.5f&&factor163<=1.f&&std::abs(factor163-old163)<.02f);old163=factor163;}
 assert(old163==.5f);
 const auto central163=SceneConsequence44(100,100,15,8,{1,1,1},3,true,true,1,1,1.5f);
 const auto front163=VisibleReceiverScore133(central163,.8f,100,15,8,{1,1,1},3,true,center163);
 const auto side163=VisibleReceiverScore133(central163,.8f,100,15,8,{1,1,1},3,true,edge163);
 assert(front163.score<side163.score&&front163.subjectSupport==side163.subjectSupport&&front163.roadSupport==side163.roadSupport&&front163.consequence==side163.consequence);
 assert(VisibleReceiverEmission133(8,{1,1,1}));
 assert(!VisibleReceiverEmission133(0,{1,1,1})&&!VisibleReceiverEmission133(8,{0,0,0}));
 assert(!VisibleReceiverEmission133(std::numeric_limits<float>::quiet_NaN(),{1,1,1}));
 assert(!VisibleReceiverEmission133(8,{std::numeric_limits<float>::quiet_NaN(),1,1}));
 for(unsigned i=0;i<256;++i){
  const float d=float(i),radius=5.f+float(i%40);
  auto baseline=SceneConsequence44(d*d,d*d,radius,8,{.2f,.5f,1},3,true,false,1,1,1.5f);
  const float off=VisibleReceiverBenefit133(false,true,true,true,d*d,radius,reach,3,0,0);
  assert(off==0);Same133(baseline,VisibleReceiverScore133(baseline,off,d*d,radius,8,{.2f,.5f,1},3,true));
  assert(VisibleReceiverNativeDistance133(d*d,off)==d*d);
 }
 // Far source, visible authored influence: potential receiver benefit extends
 // ranking support only. The source radius and native illumination stay exact.
 const float benefit=VisibleReceiverBenefit133(true,true,true,true,80.f*80.f,30,reach,3,0,0);
 assert(benefit>0&&benefit<1&&VisibleReceiverRelevant133(false,benefit));
 auto base=SceneConsequence44(6400,6400,30,8,{.2f,.5f,1},3,true,false,1,1,1.5f);
 auto changed=VisibleReceiverScore133(base,benefit,6400,30,8,{.2f,.5f,1},3,true);
 assert(changed.score<base.score&&changed.consequence==base.consequence&&changed.subjectSupport==base.subjectSupport);
 assert(VisibleReceiverNativeDistance133(6400,benefit)>=6400.f/1.15f);
 // Exact no-benefit boundaries and malformed numeric input do not invent support.
 for(float d:{100.f,1000.f})assert(VisibleReceiverBenefit133(true,true,true,true,d*d,30,reach,3,0,0)==0);
 const float nan=std::numeric_limits<float>::quiet_NaN();
 assert(VisibleReceiverBenefit133(true,false,true,true,400,30,reach,3,0,0)==0);
 assert(VisibleReceiverBenefit133(true,true,false,true,400,30,reach,3,0,0)==0);
 assert(VisibleReceiverBenefit133(true,true,true,false,400,30,reach,3,0,0)==0);
 assert(VisibleReceiverBenefit133(true,true,true,true,nan,30,reach,3,0,0)==0);
 assert(VisibleReceiverBenefit133(true,true,true,true,400,0,reach,3,0,0)==0);
 assert(VisibleReceiverBenefit133(true,true,true,true,400,30,0,3,0,0)==0);
 assert(VisibleReceiverBenefit133(true,true,true,true,400,30,reach,nan,0,0)==0);
 assert(VisibleReceiverBenefit133(true,true,true,true,400,30,reach,3,2,0)==0);
 assert(VisibleReceiverBenefit133(true,true,true,true,400,30,reach,3,1,.5f)==0);
 // The shoulder reaches zero continuously rather than adding a timed map lease.
 float previous=1;
 for(unsigned i=0;i<=100;++i){float next=VisibleReceiverBenefit133(true,true,true,true,
   (30+reach*float(i)/100)*(30+reach*float(i)/100),30,reach,3,0,0);assert(next<=previous+1e-6f);previous=next;}
 assert(previous<1e-6f);
 // Same exact production math in serial and worker phases; no worker registry,
 // source pointer, receiver discovery or permission cache is introduced.
 parallel61::Context c{};c.view.valid=true;c.player={0,0,0};c.strength=1.5f;c.visibleReceiver133=1;c.lampReach133=reach;
 parallel61::Input in{};in.phase=1;in.position={80,0,0};in.radius=30;in.type=0;in.intensity=8;in.rgb={.2f,.5f,1};in.viewWeight=3;in.volumeVisible=1;
 Same133(changed,parallel61::Compute(c,in).consequence);
 for(unsigned dark=0;dark<4;++dark){auto input=in;if(dark==0)input.intensity=0;if(dark==1)input.rgb={0,0,0};if(dark==2)input.intensity=std::numeric_limits<float>::quiet_NaN();if(dark==3)input.rgb[0]=std::numeric_limits<float>::quiet_NaN();auto off=c;off.visibleReceiver133=0;auto a=parallel61::Compute(c,input).consequence,b=parallel61::Compute(off,input).consequence;assert(a.valid==b.valid);if(a.valid)Same133(a,b);}
 auto excluded=in;excluded.beam=1;Same133(base,parallel61::Compute(c,excluded).consequence);
 // Actual worker Compute uses the same current-view scenery factor as serial,
 // with its vehicle-exclusion input preserving the full inherited score.
 for(Vec3 point163:{Vec3{0,0,.5f},Vec3{1,0,.5f}}){
  auto context163=c;context163.view=forward;auto input163=in;input163.position={point163.x,point163.y,point163.z};input163.radius=.1f;
  const float distance163=EvaluateGeometry(context163.player,point163).distanceSquared;
  const auto inherited163=SceneConsequence44(distance163,distance163,input163.radius,input163.intensity,
   {input163.rgb[0],input163.rgb[1],input163.rgb[2]},input163.viewWeight,true,false,1,1,context163.strength);
  const float benefit163=VisibleReceiverBenefit133(true,true,true,true,distance163,input163.radius,reach,input163.viewWeight,0,0);
  const auto serial163=VisibleReceiverScore133(inherited163,benefit163,distance163,input163.radius,input163.intensity,
   {input163.rgb[0],input163.rgb[1],input163.rgb[2]},input163.viewWeight,true,SceneryReceiverFactor163(forward,point163,{0,0,0},input163.radius,0));
  Same133(serial163,parallel61::Compute(context163,input163).consequence);
  input163.beam=1;Same133(inherited163,parallel61::Compute(context163,input163).consequence);
 }
 auto offContext=c;offContext.visibleReceiver133=0;Same133(base,parallel61::Compute(offContext,in).consequence);
 assert(!parallel61::EqualPhase(c,offContext,1)&&parallel61::EqualPhase(c,offContext,0));
 auto moved=c;moved.lampReach133=50;assert(!parallel61::EqualPhase(c,moved,1));
 moved=c;moved.view.valid=false;assert(!parallel61::EqualPhase(c,moved,1));
 // Brighter duplicate projector remains monotone for the existing alias policy.
 auto dim=SceneConsequence44(6400,6400,30,1,{.2f,.5f,1},3,true,false,1,1,1.5f);
 dim=VisibleReceiverScore133(dim,benefit,6400,30,1,{.2f,.5f,1},3,true);assert(changed.score<dim.score);
 // Actual native comparator's forward/special priority is unchanged.
 NativeShadowContinuity42::Candidate player{},lamp{};player.observed=lamp.observed=true;
 player.ownBeam=true;player.stableOrder=1;player.focusDistance=100;lamp.focusDistance=changed.score;
 assert(NativeShadowContinuity42::Compare(1,lamp,player)==2);
 player.flags=0x400;assert(NativeShadowContinuity42::Compare(2,lamp,player)==2);
 return 0;
}
