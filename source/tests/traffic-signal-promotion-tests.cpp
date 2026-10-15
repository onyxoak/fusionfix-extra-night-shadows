#include "TrafficSignalPromotion.hpp"
#include "TrafficSignalNativeQueue.hpp"
#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
using namespace traffic_signal;
struct Input {uint32_t caller=0x921029,type=2,phase=0,flags=0x200,key=0;float rgb[3]{0.1f,0.4f,0.1f};};
struct Fixture {
 Identity id{1,2,3,4,5,6,0x4c60ad62};Lifetime life;Promotion p;
 Fixture(){assert(p.Configure(0x100000,0x200000));assert(life.Bind(id,10));assert(life.Observe(id,10,0));assert(life.Observe(id,11,1));assert(life.Observe(id,12,2));}
 bool Apply(Input& in,uint32_t frame=12,bool sameThread=true,bool retired=false,bool requested=true,bool collision=false){
  return p.Apply(life,id,frame,in.caller,in.type,in.phase,sameThread,retired,requested,collision,in.flags,in.key);
 }
};
int main(){
 // Native triple-buffer rotation retains our previous submission in the
 // completed renderer input while the next producer buffer starts empty.
 {Fixture f;std::array<std::array<uint32_t,4>,3> buffers{};std::array<uint32_t,3> counts{};
  unsigned producer=0,completed=2;
  for(uint32_t frame=12;frame<100;++frame){
   Input in;in.phase=frame%3;assert(f.life.Observe(f.id,frame,in.phase));
   auto check=[&]{return SubmissionCollision(counts[producer],0x300000+producer*0x14000,
    0x300000+completed*0x14000,true,f.p.Key(),[&](uint32_t i){return buffers[producer][i];});};
   assert(check()==0);assert(f.Apply(in,frame,true,false,true,check()!=0));
   buffers[producer][counts[producer]++]=in.key;
   assert(check()==1); // A genuine duplicate/current foreign key still refuses.
   completed=producer;producer=(producer+1)%3;counts[producer]=0;
   assert(buffers[completed][0]==f.p.Key()); // Prior key must not retire us.
  }assert(!f.p.Retired());}
 {Fixture f;Input in;const auto old=in;
  const auto collision=SubmissionCollision(1,0x300000,0x314000,true,f.p.Key(),[&](uint32_t){return f.p.Key();});
  assert(collision==1);assert(!f.Apply(in,12,true,false,true,collision!=0));
  assert(f.p.Retired()&&!memcmp(&in,&old,sizeof(in)));}
 {auto unexpected=[](uint32_t)->uint32_t{assert(false);return 0;};
  assert(SubmissionCollision(0,0x300000,0x314000,false,0x100000,unexpected)==0);
  assert(SubmissionCollision(641,0x300000,0x314000,true,0x100000,unexpected)==2);
  assert(SubmissionCollision(1,0x300000,0x314000,false,0x100000,unexpected)==3);
  assert(SubmissionCollision(0,0,0x314000,true,0x100000,unexpected)==3);
  assert(SubmissionCollision(0,0x300000,0x300000,true,0x100000,unexpected)==5);}
 // 0x6BCF33 tests input 0x300, then ORs 0x60 before storing traffic flags.
 {Fixture f;Input in;assert(f.Apply(in));assert(in.flags==0x204);
  const auto nativeFlags=in.flags|0x60;
  assert(nativeFlags==0x264&&StoredPrivateTraffic(2,nativeFlags,in.key,f.p.Key()));
  assert(!StoredPrivateTraffic(2,0x204,in.key,f.p.Key()));
  assert(!StoredPrivateTraffic(1,nativeFlags,in.key,f.p.Key()));
  assert(!StoredPrivateTraffic(2,nativeFlags,in.key+64,f.p.Key()));
  assert(!StoredPrivateTraffic(2,nativeFlags|0x100,in.key,f.p.Key()));
  assert(Promotion::LampGeneration(StoredPrivateTraffic(2,nativeFlags,in.key,f.p.Key()),123)==1);}
 const float target[]{116.875f,833.670f,13.755f};assert(ProvenTarget(1,0x4c60ad62,target));
 assert(!ProvenTarget(0,0x4c60ad62,target));assert(!ProvenTarget(1,1,target));
 float moved[]{116.975f,833.670f,13.755f};assert(!ProvenTarget(1,0x4c60ad62,moved));
 for(auto key:{0u,1u,0xffffu,UINT32_MAX,0x100001u,0x200000u}){Promotion p;assert(!p.Configure(key,0x200000));assert(p.Retired());assert(!p.Configure(0x100000,0x200000));}
 {Promotion p;assert(p.Configure(0x100000,0x200000));assert(!p.Configure(0x300000,0x200000));}
 {Fixture f;Input in;const auto before=in;assert(f.Apply(in));assert(in.flags==0x204&&in.key==0x100000);assert(in.caller==before.caller&&in.type==before.type&&in.phase==before.phase&&!memcmp(in.rgb,before.rgb,12));Input duplicate;const auto old=duplicate;assert(!f.Apply(duplicate));assert(!memcmp(&duplicate,&old,sizeof(old)));assert(!f.p.Retired());}
 // Continuous phases keep the SAME key and generation while native RGB changes.
 {Fixture f;for(uint32_t frame=12;frame<100;++frame){Input in;in.phase=(frame/7)%3;in.rgb[0]=float(in.phase);assert(f.life.Observe(f.id,frame,in.phase));const auto before=in;assert(f.Apply(in,frame));assert(in.key==0x100000&&in.flags==0x204&&!memcmp(in.rgb,before.rgb,12));assert(Promotion::LampGeneration(true,frame)==1);}}
 for(int field=0;field<5;++field){Fixture f;Input in;if(field==0)in.caller++;if(field==1)in.type++;if(field==2)in.phase=3;if(field==3)in.flags++;if(field==4)in.key++;const auto old=in;assert(!f.Apply(in));assert(!memcmp(&in,&old,sizeof(in)));}
 {Fixture f;f.life.phaseMask=3;Input in;const auto old=in;assert(!f.Apply(in));assert(!memcmp(&in,&old,sizeof(in)));}
 for(int gate=0;gate<4;++gate){Fixture f;Input in;const auto old=in;assert(!f.Apply(in,12,gate!=0,gate==1,gate!=2,gate==3));assert(f.p.Retired());assert(!memcmp(&in,&old,sizeof(in)));assert(!f.Apply(in,13));assert(!f.p.Configure(0x300000,0x200000));}
 {Fixture f;Input in;assert(!f.Apply(in,15));assert(f.p.Retired());}
 for(int field=0;field<7;++field){Fixture f;Input in;if(field==0)f.id.pool++;if(field==1)f.id.storage++;if(field==2)f.id.flags++;if(field==3)f.id.owner++;if(field==4)f.id.slot++;if(field==5)f.id.reference++;if(field==6)f.id.model++;const auto old=in;assert(!f.Apply(in));assert(f.p.Retired());assert(!memcmp(&in,&old,sizeof(in)));}
 {Fixture f;Input in;assert(f.Apply(in));f.life.Retire();Input later;assert(!f.Apply(later,13));assert(f.p.Retired());}
 {Fixture f;assert(!f.life.Observe(f.id,15,0));assert(f.life.retired);Input in;assert(!f.Apply(in,15));}
 {Fixture f;Input in;assert(f.Apply(in));Input sameFrame;assert(!f.Apply(sameFrame,12,true,false,true,true));assert(!f.p.Retired());}
 {Fixture f;Input in;assert(!f.Apply(in,12,true,false,false));assert(!f.Apply(in,13,true,false,true));assert(f.p.Retired());}
 assert(Promotion::LampGeneration(false,0xabcdef)==0xabcdef);
 {Fixture f;Input first;assert(f.Apply(first));
  assert(f.life.RebaseNativeTimer(f.id,5,5,true));Input next;
  assert(f.Apply(next,5));assert(next.key==first.key&&!f.p.Retired());
  assert(f.life.CheckGap(8));Input later;assert(!f.Apply(later,8));assert(f.p.Retired());}
 std::cout<<"PASS one-signal admission: immutable RGB/type, stable phase key/generation, exact target, seven identity fields, duplicate/collision/thread/gap/free/off rejection and permanent retirement; other lamp generations unchanged\n";
}
