#include "core/InteriorScenePriority211.hpp"
#include <cassert>
#include <limits>
using namespace fusionfix::shadows;
int main(){
 assert(InteriorQualified211(true,false,true,19719,1,0x6f));
 assert(!InteriorQualified211(false,false,true,19719,1,0x6f));
 assert(!InteriorQualified211(true,true,true,19719,1,0x6f));
 assert(!InteriorQualified211(true,false,false,19719,1,0x6f));
 assert(!InteriorQualified211(true,false,true,0,1,0x6f));
 assert(!InteriorQualified211(true,false,true,19719,0,0x6f));
 assert(!InteriorQualified211(true,false,true,19719,1,0x16f));
 assert(!InteriorQualified211(true,false,true,19719,1,0x46f));
 ShadowView view{};view.valid=true;for(int k=0;k<4;++k)view.view[k][k]=1;
 view.projection[0][0]=view.projection[1][1]=1;view.projection[2][2]=1;view.projection[2][3]=1;
 SceneReceiverConsequence44 initial{};initial.valid=true;initial.score=20;initial.consequence=.04f;
 const auto apply=[&](ShadowView v,Vec3 pos,float radius=3.f,float intensity=20.f){return InteriorScenePriority211(initial,v,pos,radius,intensity,{1,.2f,0},1.5f);};
 auto near=apply(view,{0,0,4}),far=apply(view,{0,0,8});
 assert(near.score<far.score&&far.score<initial.score&&near.consequence-initial.consequence<=1.5f);
 // A visible distant fixture receives support even outside Niko's radius.
 auto central=apply(view,{0,0,8}),edge=apply(view,{7,0,8});assert(central.score<edge.score);
 assert(apply(view,{0,0,-4}).score==initial.score);assert(apply(view,{100,0,4}).score==initial.score);
 view.valid=false;assert(apply(view,{0,0,4}).score==initial.score);view.valid=true;
 assert(apply(view,{0,0,4},0).score==initial.score);assert(apply(view,{0,0,4},3,0).score==initial.score);
 view.projection[0][0]=std::numeric_limits<float>::quiet_NaN();assert(apply(view,{0,0,4}).score==initial.score);
 // No eligibility/cache/range/radiance data are created or changed.
 assert(near.subjectSupport==initial.subjectSupport&&near.roadSupport==initial.roadSupport&&near.valid==initial.valid);
}
