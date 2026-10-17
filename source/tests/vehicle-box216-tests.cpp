#include "core/VehicleBoxFallback216.hpp"
#include <cassert>
#include <limits>
using namespace vehicle_box216;
int main(){
 State state;Settings s;s.perCar=0;
 Car car{{0,0,1},2,1,1,0,1,100,2,3};
 Light lamp{{0,0,8},20,10,9,4,0,3,1};
 auto valid=[](const Car& c,uint32_t entity,uint32_t type){return c.generation==3&&entity==c.owner&&type==2;};
 state.Begin(1,100,0);auto first=state.Select(lamp,&car,1,{0,0,0},{0,0,0},false,s,valid);assert(first.count==0);
 for(unsigned i=2;i<60;++i){state.Begin(i,100+i*16,0);state.Select(lamp,&car,1,{0,0,0},{0,0,0},false,s,valid);}
 state.Begin(60,1060,0);auto boxed=state.Select(lamp,&car,1,{0,0,0},{0,0,0},false,s,valid);assert(boxed.count==1&&boxed.constants[3]>=16&&boxed.constants[27]>0);
 // A newly gained map is not double-shadowed: the caster bridge must run first.
 state.Begin(61,1076,0);assert(state.Select(lamp,&car,1,{0,0,0},{0,0,0},true,s,valid).count==0);
 assert(!state.Exclude(9,999,2,valid));assert(!state.Exclude(9,100,1,valid));
 assert(state.Exclude(9,100,2,valid));assert(state.Select(lamp,&car,1,{0,0,0},{0,0,0},true,s,valid).count==1);
 // Pool generation change rejects stale exclusions and boxes immediately.
 auto reject=[](const Car&,uint32_t,uint32_t){return false;};
 assert(!state.Exclude(9,100,2,reject));assert(state.Select(lamp,&car,1,{0,0,0},{0,0,0},false,s,reject).count==0);
 State real;real.Begin(1,100,0);assert(real.Select(lamp,&car,1,{0,0,0},{0,0,0},true,s,valid).count==0);
 real.Begin(2,116,0);assert(real.Select(lamp,&car,1,{0,0,0},{0,0,0},false,s,valid).count==1);
 lamp.flags|=0x100;assert(real.Select(lamp,&car,1,{0,0,0},{0,0,0},false,s,valid).count==0);lamp.flags&=~0x100u;
 lamp.intensity=0;assert(real.Select(lamp,&car,1,{0,0,0},{0,0,0},false,s,valid).count==0);lamp.intensity=10;
 lamp.position.x=std::numeric_limits<float>::quiet_NaN();assert(real.Select(lamp,&car,1,{0,0,0},{0,0,0},false,s,valid).count==0);lamp.position.x=0;
 car.centre.x=100;real.Begin(3,132,0);assert(real.Select(lamp,&car,1,{0,0,0},{0,0,0},false,s,reject).count==0);
 real.Begin(1,5000,0);assert(!real.Exclude(9,100,2,valid));
 // Competing lamps settle at the configured two-light limit for one car.
 State bounded;Settings limits;car.centre={0,0,1};Light other=lamp;other.position={2,0,8};other.key=10;
 Light third=lamp;third.position={-2,0,8};third.key=11;
 for(unsigned frame=1;frame<100;++frame){bounded.Begin(frame,100+frame*16,limits.perCar);
  auto a=bounded.Select(lamp,&car,1,{},{},false,limits,valid);
  auto b=bounded.Select(other,&car,1,{},{},false,limits,valid);
  auto c=bounded.Select(third,&car,1,{},{},false,limits,valid);
  if(frame>80)assert(a.count+b.count+c.count<=2);
 }
 // A disappearing vehicle cannot leave an unbound cached silhouette.
 bounded.Begin(100,1700,limits.perCar);Car absent{};
 assert(bounded.Select(lamp,&absent,0,{},{},false,limits,valid).count==0);
 // Exclusion lifetime expires even if a light is no longer submitted.
 bounded.Begin(101,5000,limits.perCar);assert(!bounded.Exclude(9,100,2,valid));

}
