#include "TrafficSignalRegistry.hpp"
#include "TrafficSignalNativeQueue.hpp"
#include "core/NativeShadowContinuity42.hpp"
#include <cassert>
#include <iostream>
#include <limits>
using namespace traffic_signal;
Identity Id(uint32_t owner=4,uint32_t reference=2,uint32_t model=0x4c60ad62){return {1,2,3,owner,owner,reference,model};}
template<size_t N> bool Apply(Registry<N>& r,uint32_t index,uint32_t frame,bool collision=false){uint32_t flags=0x200,key=0;
 const auto okay=r.Apply(index,r.entries[index].id,frame,0x921029,2,frame%3,collision,flags,key);
 assert(okay?(flags==0x204&&key==r.entries[index].key):(flags==0x200&&key==0));return okay;}
int main(){
 {Registry<8> r;assert(r.Configure(0x100000,0x200000));
  auto a=r.Observe(Id(),10,0,true),b=r.Observe(Id(40,3,0xe0494133),10,2,true);
  assert(a.visit==Visit::New&&b.visit==Visit::New&&a.index!=b.index);
  assert(r.entries[a.index].key!=r.entries[b.index].key&&r.entries[a.index].generation!=r.entries[b.index].generation);
  assert(Apply(r,a.index,10)&&Apply(r,b.index,10));assert(!Apply(r,a.index,10));
  const auto key=r.entries[a.index].key,generation=r.entries[a.index].generation;
  for(uint32_t frame=11;frame<100;++frame){assert(r.Observe(Id(),frame,frame%3,true).visit==Visit::Continuous);
   assert(Apply(r,a.index,frame));assert(r.entries[a.index].key==key&&r.entries[a.index].generation==generation);}
  assert(r.entries[a.index].phaseMask==7); // No first-cycle wait; first valid phase worked.
  auto restarted=r.Observe(Id(),103,2,true);assert(restarted.visit==Visit::LeaseRestart&&Apply(r,a.index,103));
  assert(r.entries[a.index].key==key&&!r.entries[a.index].retired);
  assert(r.Observe(Id(),104,0,false).index==a.index&&!Apply(r,a.index,104));
  assert(r.Observe(Id(),105,1,true).visit==Visit::LeaseRestart&&Apply(r,a.index,105));
  assert(r.Free(1,4)==a.index&&r.entries[a.index].retired);
  auto replacement=r.Observe(Id(4,3),106,2,true);assert(replacement.visit==Visit::New&&Apply(r,replacement.index,106));
  assert(r.entries[replacement.index].key!=key&&!Apply(r,a.index,106));}
 // Native free authorizes a NEW physical lifetime even after 7-bit reference wrap.
 {Registry<256> r;assert(r.Configure(0x100000,0x200000));uint32_t previous=0;
  for(uint32_t life=0;life<130;++life){auto result=r.Observe(Id(4,life&127),life+10,life%3,true);
   assert(result.visit==Visit::New&&Apply(r,result.index,life+10));assert(r.entries[result.index].key!=previous);
   previous=r.entries[result.index].key;assert(r.Free(1,4)==result.index);}}
 // Every identity field replacement gets a new token; old keys remain retired.
 for(unsigned field=0;field<7;++field){Registry<8> r;assert(r.Configure(0x100000,0x200000));auto id=Id();auto first=r.Observe(id,10,0,true);
  if(field==0)id.pool++;if(field==1)id.storage++;if(field==2)id.flags++;if(field==3)id.owner++;
  if(field==4)id.slot++;if(field==5)id.reference++;if(field==6)id.model++;
  auto second=r.Observe(id,11,1,true);assert(second.visit==Visit::New&&second.index!=first.index);
  if(field!=3)assert(r.entries[first.index].retired); // Different owner is a simultaneous instance.
  if(field==0)assert(r.Free(1,4)==UINT32_MAX); // Old-pool callback cannot free the replacement.
  assert(r.entries[first.index].key!=r.entries[second.index].key);}
 {Registry<8> r;assert(r.Configure(0x100000,0x200000));auto a=r.Observe(Id(),100,0,true);
  assert(!Apply(r,a.index,100,true)&&r.entries[a.index].reason==End::Collision);
  assert(r.Observe(Id(),101,1,true).visit==Visit::Retired);assert(!Apply(r,a.index,101));
  auto b=r.Observe(Id(40),101,1,true);assert(Apply(r,b.index,101));}
 {Registry<8> r;assert(r.Configure(0x100000,0x200000));auto a=r.Observe(Id(),5050813,0,true);
  assert(r.Rebase(a.index,Id(),5050775,5050775,true));assert(!r.entries[a.index].leased);
  assert(r.Observe(Id(),5050776,1,true).visit==Visit::LeaseRestart&&Apply(r,a.index,5050776));
  assert(r.Observe(Id(),5050774,2,true).visit==Visit::Retired);
  assert(!r.Rebase(a.index,Id(),5050700,5050700,true));}
 {Registry<8> r;assert(r.Configure(0x100000,0x200000));auto a=r.Observe(Id(),100,0,true);
  assert(!r.Rebase(a.index,Id(),90,90,false));assert(!r.Rebase(a.index,Id(),90,89,true));
  assert(!r.Rebase(a.index,Id(4,3),90,90,true));r.Disable();assert(!Apply(r,a.index,100));
  assert(r.Observe(Id(40),101,0,true).visit==Visit::Rejected&&!r.Rebase(a.index,Id(),90,90,true));}
 {Registry<2> r;assert(r.Configure(0x100000,0x200000));assert(r.Observe(Id(),10,0,false).visit==Visit::Rejected);
  assert(r.Observe(Id(),10,0,true).visit==Visit::New);assert(r.Observe(Id(40),10,0,true).visit==Visit::New);
  assert(r.Observe(Id(80),10,0,true).visit==Visit::Capacity);assert(Apply(r,0,10)&&Apply(r,1,10));
  assert(r.IndexOfKey(0x100001)==UINT32_MAX&&r.IndexOfKey(0x100080)==UINT32_MAX);}
 {Registry<8> r;assert(!r.Configure(0x100000,0x100040));}
 {Registry<8> r;assert(r.Configure(0x100000,0x200000));auto a=r.Observe(Id(),10,0,true);
  std::array<uint32_t,17> words{},old{};for(unsigned i=0;i<words.size();++i)words[i]=100+i;
  words[2]=2;words[3]=0x200;words[16]=0;old=words;
  assert(r.Apply(a.index,Id(),10,0x921029,words[2],0,false,words[3],words[16]));
  for(unsigned i=0;i<words.size();++i)if(i!=3&&i!=16)assert(words[i]==old[i]);}
 const float player[]{0,0,0},near[]{40,0,0},far[]{40.01f,0,0},bad[]{std::numeric_limits<float>::quiet_NaN(),0,0};
 assert(Nearby(near,player,40)&&!Nearby(far,player,40)&&!Nearby(bad,player,40)&&!Nearby(near,player,0));
 // Integration with unchanged seven-slot continuity: no special signal priority,
 // no expired/irrelevant claim pinning, own headlights retain protected priority.
 {using C=fusionfix::shadows::NativeShadowContinuity42;Registry<16> r;assert(r.Configure(0x100000,0x200000));
  C continuity;continuity.Begin(1,10,100);std::array<C::Identity,7> chosen{};
  for(unsigned i=0;i<7;++i){auto result=r.Observe(Id(4+i*64),10,i%3,true);chosen[i]={r.entries[result.index].key,r.entries[result.index].generation};}
  assert(continuity.Commit(chosen));continuity.Begin(1,11,110);
  auto newcomer=r.Observe(Id(4+7*64),11,0,true);auto entrant=continuity.Observe({r.entries[newcomer.index].key,r.entries[newcomer.index].generation},0x264,true,false);
  auto retained=continuity.Observe(chosen[0],0x264,true,false);assert(C::Compare(0,entrant,retained)==2);
  auto irrelevant=continuity.Observe(chosen[0],0x264,false,false);assert(C::Compare(0,entrant,irrelevant)==0);
  auto own=continuity.Observe({0x200000,1},0x104,true,true);own.stableOrder=1; // Fresh physical-forward registry evidence.
  assert(C::Compare(0,entrant,own)==2&&C::Compare(2,own,entrant)==0);
  auto ownRear=own;ownRear.stableOrder=0;ownRear.focusTier=1;ownRear.focusDistance=100.f;
  auto impactfulSignal=entrant;impactfulSignal.focusTier=1;impactfulSignal.focusDistance=1.f;
  assert(C::Compare(1,impactfulSignal,ownRear)==0&&C::Compare(1,ownRear,impactfulSignal)==2);
  auto changed=chosen[0];changed.generation++;assert(continuity.Observe(changed,0x264,true,false).claim==C::Unclaimed);
  chosen[1]=chosen[0];assert(!continuity.Commit(chosen));assert(C::Slots==7);}
 std::cout<<"PASS generalized signals: multiple models/owners, immutable input, immediate phases/stable generation, nearby leases, free/128-reference reuse, all identity fields, foreign-key/capacity/off/reset guards; seven-slot stability and relevance/own-headlight priority transitions\n";
}
