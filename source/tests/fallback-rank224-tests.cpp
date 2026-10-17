#include "core/VehicleBoxFallback216.hpp"
#include "fallback216-reference224.inc"
#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>

int main(){
 auto old=std::make_unique<reference216::State>();auto now=std::make_unique<vehicle_box216::State>();
 reference216::Settings a;vehicle_box216::Settings b;
 std::array<reference216::Car,160> before{};std::array<vehicle_box216::Car,160> after{};
 static_assert(sizeof(before)==sizeof(after));
 uint32_t seed=224;auto random=[&](){seed=seed*1664525u+1013904223u;return seed;};
 unsigned selections=0,exclusions=0,filtered=0;
 for(unsigned frame=1;frame<1800;++frame){
  work_avoidance224::mode.store(frame%3?1u:0u,std::memory_order_relaxed);
  const unsigned tick=frame*16+(frame>900?4000:0), count=frame%161;
  a.perCar=b.perCar=frame%3;a.perLight=b.perLight=1+frame%3;
  for(unsigned i=0;i<160;++i){
   const unsigned slot=(frame%13==0)?5000+i:i;
   const unsigned generation=(frame/80+i)%128;
   before[i]={{float(int(i%16)*4-32),float(int(i/16)*4-20),1},2,1,1,0,1,0x100000+i*0x320,slot,generation};
   if(frame%23==0&&i%3==0)before[i].owner=0;
  }
  std::memcpy(after.data(),before.data(),sizeof(before));
  old->Begin(frame,tick,a.perCar);now->Begin(frame,tick,b.perCar);
  for(unsigned l=0;l<32;++l){
   reference216::Light la{{float(int(l%8)*8-28),float(int(l/8)*8-12),8},25,8,100+l,4,l%2?2u:0u,1,1};
   vehicle_box216::Light lb{};static_assert(sizeof(la)==sizeof(lb));std::memcpy(&lb,&la,sizeof(lb));
   const bool mapped=(random()%5)==0;
   const auto va=[&](const auto& c,uint32_t entity,uint32_t type){return c.owner&&entity==c.owner&&type==2&&(c.generation&3u)!=0;};
   const auto oa=old->Select(la,before.data(),count,{},{},mapped,a,va);
   const auto ob=now->Select(lb,after.data(),count,{},{},mapped,b,va);
   static_assert(sizeof(oa)==sizeof(ob));
   assert(oa.count==ob.count&&oa.constants==ob.constants);
   for(unsigned i=0;i<oa.count;++i)assert(!std::memcmp(&oa.cars[i],&ob.cars[i],sizeof(oa.cars[i])));
   ++selections;
   for(unsigned i=0;i<4;++i){const unsigned index=random()%160;
    const bool maybe=!work_avoidance224::Enabled()||now->MayContainOwner224(after[index].owner);
    if(!maybe)++filtered;
    const bool ea=old->Exclude(la.key,before[index].owner,2,va),eb=maybe&&now->Exclude(lb.key,after[index].owner,2,va);
    assert(ea==eb);++exclusions;
    const uint32_t stranger=0x70000000u+(random()&0xffffu)*0x320u;
    const bool strangerMaybe=!work_avoidance224::Enabled()||now->MayContainOwner224(stranger);
    assert(old->Exclude(la.key,stranger,2,va)==(strangerMaybe&&now->Exclude(lb.key,stranger,2,va)));
   }
  }
 }
 // Clock/frame reversal clears all numeric lookup state too.
 old->Begin(1,1,2);now->Begin(1,1,2);
 for(unsigned key=100;key<140;++key){auto valid=[](const auto&,uint32_t,uint32_t){return true;};assert(old->Exclude(key,0x100000,2,valid)==now->Exclude(key,0x100000,2,valid));}
 std::cout<<"PASS fallback rank/filter equivalence: "<<selections<<" selections, "<<exclusions<<" exclusions, "<<filtered<<" negative-filter skips; same constants, limits, expiry and recycled-slot behavior. Not a game FPS result.\n";
}
