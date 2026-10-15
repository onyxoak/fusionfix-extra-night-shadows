#include "core/SceneScoreContinuity96.hpp"
#include "core/NativeShadowContinuity42.hpp"
#include <cassert>
#include <cmath>
#include <limits>
using namespace fusionfix::shadows;
int main(){
 SceneScoreContinuity96 history;
 auto begin=[&](uint32_t frame,uint32_t now){history.Begin(10,20,frame,now);};
 begin(1,100);assert(history.Current(64,7,1.f,false,450)==1.f);
 NativeShadowContinuity42::Candidate old{},next{};
 old.observed=next.observed=true;old.claim=0;old.visualImportance=3;
 next.focusTier=old.focusTier=1;next.focusDistance=.9f;
 // Measured policy failure: a short loss in consequence would eject the map.
 old.focusDistance=1.8f;assert(NativeShadowContinuity42::Compare(1,next,old)==0);
 begin(2,116);old.focusDistance=history.Current(64,7,1.8f,true,450);
 assert(old.focusDistance>1&&old.focusDistance<1.1f);
 assert(NativeShadowContinuity42::Compare(1,next,old)==2);
 // A materially stronger new central receiver still takes its slot immediately.
 next.focusDistance=.4f;assert(NativeShadowContinuity42::Compare(1,next,old)==0);
 begin(3,132);assert(history.Current(64,7,.5f,true,450)==.5f);
 // Same numeric address with a different generation cannot inherit history.
 begin(4,148);assert(history.Current(64,8,2.f,true,450)==2.f);
 assert(history.Current(128,7,3.f,true,450)==3.f);
 // Deselected sources reacquire from their present consequence; no old pin.
 assert(history.Current(64,7,4.f,false,450)==4.f);
 // Pass gaps, load/time restoration and entering/exiting vehicles reset authority.
 begin(30,164);assert(history.Current(64,7,8.f,true,450)==8.f);
 history.Begin(10,0,31,180);assert(history.Current(64,7,9.f,true,450)==9.f);
 history.Begin(10,0,1,0);assert(history.Current(64,7,11.f,true,450)==11.f);
 assert(history.Current(64,7,12.f,true,0)==12.f);
 assert(std::isnan(history.Current(64,7,std::numeric_limits<float>::quiet_NaN(),true,450)));
 // Sustained loss eventually releases; temporal continuity is not permanent priority.
 history.Begin(10,0,2,16);float result=0;
 for(unsigned frame=2;frame<400;++frame){history.Begin(10,0,frame,frame*16);result=history.Current(64,7,100.f,true,450);}
 assert(result>99.9f);
}
