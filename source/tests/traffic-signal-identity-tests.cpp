#include "TrafficSignalIdentity.hpp"
#include <cassert>
using namespace traffic_signal;
int main(){
 Identity id{1,2,3,4,5,6,7};Lifetime life;
 assert(!life.Bind({},10));assert(life.Bind(id,10));
 assert(life.Observe(id,11,0));assert(life.Observe(id,12,1));assert(life.Observe(id,13,2));assert(life.phaseMask==7);
 auto changed=id;changed.reference++;assert(!life.Observe(changed,14,0));assert(life.retired);assert(!life.Bind(id,15));assert(!life.Observe(id,15,0));
 Lifetime freed;assert(freed.Bind(id,20));freed.Retire();assert(!freed.Observe(id,21,1));assert(!freed.Bind(id,21));
 Lifetime streamed;assert(streamed.Bind(id,100));assert(!streamed.CheckGap(102));assert(streamed.CheckGap(103));assert(!streamed.Observe(id,104,2));
 Lifetime reset;assert(reset.Bind(id,1));changed=id;changed.storage++;assert(!reset.Observe(changed,2,0));
 Lifetime model;assert(model.Bind(id,1));changed=id;changed.model++;assert(!model.Observe(changed,2,0));
 Lifetime wrap;assert(wrap.Bind(id,0xfffffffeu));assert(!wrap.CheckGap(0));assert(wrap.CheckGap(1));
 // Continuous native submission must survive many frames and every phase, independently of log cadence.
 Lifetime continuous;assert(continuous.Bind(id,100));
 for(uint32_t frame=100;frame<=160;++frame){assert(continuous.Observe(id,frame,(frame-100)/21));assert(!continuous.CheckGap(frame+1));}
 assert(continuous.phaseMask==7);assert(continuous.CheckGap(163));assert(!continuous.Observe(id,164,0));
 // Recorded v5 rollback: the native restoration callback rebases first,
 // then continuous producer calls keep the same physical lifetime/phase mask.
 {Lifetime clock;assert(clock.Bind(id,5050813));clock.phaseMask=7;
  assert(clock.RebaseNativeTimer(id,5050775,5050775,true));
  assert(clock.lastFrame==5050775&&clock.phaseMask==7&&!clock.retired);
  assert(!clock.CheckGap(5050776));assert(clock.Observe(id,5050776,0));
  assert(clock.CheckGap(5050779));assert(!clock.RebaseNativeTimer(id,5050770,5050770,true));}
 // Ordinary backward readings still retire. Only an active native backup,
 // exact saved frame and all seven unchanged identity fields authorize rebasing.
 {Lifetime clock;assert(clock.Bind(id,100));assert(clock.CheckGap(99));}
 for(int gate=0;gate<10;++gate){Lifetime clock;assert(clock.Bind(id,100));auto current=id;
  if(gate==0)current.pool++;if(gate==1)current.storage++;if(gate==2)current.flags++;
  if(gate==3)current.owner++;if(gate==4)current.slot++;if(gate==5)current.reference++;if(gate==6)current.model++;
  assert(!clock.RebaseNativeTimer(current,gate==7?101:90,gate==8?89:(gate==7?101:90),gate!=9));
  assert(clock.lastFrame==100&&!clock.retired);}
 {Lifetime clock;assert(!clock.RebaseNativeTimer(id,90,90,true));}
}
