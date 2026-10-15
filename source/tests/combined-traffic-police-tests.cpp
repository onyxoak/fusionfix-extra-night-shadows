#include "TrafficSignalRegistry.hpp"
#include "core/NativeShadowContinuity42.hpp"
#include "core/LabPoliceClaim.hpp"
#include "core/LabSirenVacancy.hpp"
#include "core/NearbyPoliceShadows.hpp"
#include <cassert>
#include <iostream>
int main(){
 using C=fusionfix::shadows::NativeShadowContinuity42;
 using R=traffic_signal::Registry<32>;
 static_assert(C::Slots==7);
 constexpr uint32_t policeKey=0x200000,ownKey=0x300000;
 R signals;assert(signals.Configure(0x100000,policeKey));
 traffic_signal::Identity a{1,2,3,0x40,4,2,0x4c60ad62};
 auto b=a;b.owner=0x80;b.slot=8;b.model=0xe0494133;
 auto first=signals.Observe(a,10,0,true),second=signals.Observe(b,10,2,true);
 const auto ka=signals.entries[first.index].key,kb=signals.entries[second.index].key;
 assert(ka!=kb&&ka!=policeKey&&kb!=policeKey);
 lab_siren::Identity vehicle{0x4000,0x5000,12,2};
 // Legacy Proof9b fallback retains its original explicit-enable/lifetime rules.
 assert(lab_siren::Decide(false,false,false,true,{},vehicle)==lab_siren::Decision::Wait);
 assert(lab_siren::Decide(false,false,true,true,{},vehicle)==lab_siren::Decision::Bind);
 assert(lab_siren::Decide(true,false,true,true,vehicle,vehicle)==lab_siren::Decision::Keep);
 C continuity;continuity.Begin(1,10,100);
 std::array<C::Identity,7> chosen{{{policeKey,1},{ka,1},{kb,2},{ownKey,7},{0x310000,9},{0x320000,10},{0x330000,11}}};
 assert(continuity.Commit(chosen));continuity.Begin(1,11,110);
 auto other=a;other.owner=0xc0;other.slot=12;
 auto newcomer=signals.Observe(other,11,1,true);const auto kn=signals.entries[newcomer.index].key;
 auto traffic=continuity.Observe({kn,signals.entries[newcomer.index].generation},0x264,true,false);
 auto siren=continuity.Observe({policeKey,1},0x174,true,false);
 auto own=continuity.Observe({ownKey,7},0x164,true,true);
 own.stableOrder=1; // Authenticated physical forward private lamp.
 auto ownRear=own;ownRear.stableOrder=0;ownRear.focusTier=1;ownRear.focusDistance=100.f;
 auto impactfulSiren=siren;impactfulSiren.focusTier=1;impactfulSiren.focusDistance=1.f;
 assert(C::Compare(1,impactfulSiren,ownRear)==0&&C::Compare(1,ownRear,impactfulSiren)==2);
 auto special=continuity.Observe({0x340000,12},0x464,true,false);
 lab_siren::PoliceClaim claim;claim.Begin(true,vehicle,true,true,11,110);
 assert(claim.Observe(true,true));
 // Accepted police policy wins eligible unprotected comparisons; own/native priority stays.
 assert(lab_siren::PoliceClaim::Compare(0,false,true,true,traffic,siren)==2);
 assert(lab_siren::PoliceClaim::Compare(2,true,false,true,siren,traffic)==0);
 assert(lab_siren::PoliceClaim::Compare(C::Compare(2,siren,own),true,false,true,siren,own)==2);
 assert(lab_siren::PoliceClaim::Compare(2,true,false,true,siren,special)==2);
 lab_siren::Vacancy vacancy;std::array<lab_siren::VacancyEntry,7> slots{};
 for(unsigned i=0;i<7;++i)slots[i]={chosen[i].key,chosen[i].generation,i==3};
 assert(vacancy.Filter(11,110,true,true,true,true,0.1f,policeKey,slots)==-1);
 slots[0]={kn,signals.entries[newcomer.index].generation,false};
 // A known dark native frame may leave one empty slot, retaining the exact other six.
 assert(vacancy.Filter(12,120,true,true,true,true,0.0001f,policeKey,slots)==0);
 slots[0].protectedPriority=true;
 assert(vacancy.Filter(12,120,true,true,true,true,0.0001f,policeKey,slots)==-1);
 slots[0].protectedPriority=false;
 assert(vacancy.Filter(12,120,true,true,true,true,0.1f,policeKey,slots)==-1);
 assert(vacancy.Filter(12,120,true,true,true,false,0.0001f,policeKey,slots)==-1);
 auto oldGeneration=slots[1].generation;slots[1].generation++;
 assert(vacancy.Filter(12,120,true,true,true,true,0.0001f,policeKey,slots)==-1);
 slots[1].generation=oldGeneration;
 assert(vacancy.Filter(24,240,true,true,true,true,0.0001f,policeKey,slots)==-1);
 // Color/position changes do not change either physical source identity.
 for(uint32_t frame=11;frame<40;++frame){
  signals.Observe(a,frame,frame%3,true);uint32_t flags=0x200,key=0;
  assert(signals.Apply(first.index,a,frame,0x921029,2,frame%3,false,flags,key));
  assert(key==ka&&flags==0x204&&signals.entries[first.index].generation==1);
  claim.Begin(true,vehicle,true,true,frame,frame*10);assert(claim.Observe(true,true));
 }
 // Police off does not disable traffic; traffic off does not retire the police lifetime.
 claim.Begin(true,vehicle,false,true,40,400);assert(!claim.Active()&&!claim.Observe(true,true));
 signals.Observe(a,40,1,true);uint32_t flags=0x200,key=0;
 assert(signals.Apply(first.index,a,40,0x921029,2,1,false,flags,key));
 signals.Disable();flags=0x200;key=0;
 assert(!signals.Apply(first.index,a,40,0x921029,2,1,false,flags,key)&&flags==0x200&&key==0);
 claim.Begin(true,vehicle,true,true,41,410);assert(claim.Observe(true,true)&&!claim.Retired());
 auto changed=vehicle;changed.reference++;
 assert(lab_siren::Decide(true,false,true,true,vehicle,changed)==lab_siren::Decision::Retire);
 claim.Begin(true,changed,true,true,42,420);assert(claim.Retired()&&!claim.Observe(true,true));
 assert(lab_siren::Decide(true,true,true,true,vehicle,vehicle)==lab_siren::Decision::Retire);
 chosen[2]=chosen[1];assert(!continuity.Commit(chosen));
 // Delivered nearby mode: a player emergency vehicle and NPC can promote together;
 // two sources share the native set with several traffic fixtures and own headlights.
 using P=lab_siren::NearbyPolice;P nearby;
 auto valid=[](P::Owner&){return true;};auto clear=[](unsigned){return true;};
 nearby.Begin(100,1000,valid);
 auto* player=nearby.Touch(vehicle,lab_siren::ModelHash("police"),0x640d0a,1,true,true,clear);
 auto npcId=vehicle;npcId.owner+=64;npcId.slot++;
 auto* npc=nearby.Touch(npcId,lab_siren::ModelHash("ambulance"),0x6425ab,4,false,true,clear);
 assert(player&&npc&&player->token!=npc->token);
 nearby.Begin(101,1010,valid);assert(player->rank==0&&npc->rank==1);
 assert(nearby.Promote(*player,0x3f800000)&&nearby.Promote(*npc,0x3f800000));
 assert(!nearby.Promote(*player,0x3f800000)&&P::Promoted==2&&P::Candidates==8);
 // Secondary yields to a relevant visible traffic lamp; primary keeps accepted priority.
 assert(P::Compare(0,true,false,1,2,true,true,true,true,false,false,false,false,false,true)==2);
 assert(P::Compare(2,true,false,0,2,true,true,true,true,false,false,false,false,false,true)==0);
 assert(P::Compare(0,true,false,1,2,true,true,true,true,false,true,false,false,false,false)==2);
 const auto oldToken=player->token;nearby.Retire(*player);
 nearby.Begin(102,1020,valid);
 auto reentered=nearby.Touch(vehicle,lab_siren::ModelHash("police"),0x640d0a,1,true,true,clear);
 assert(reentered&&reentered->token!=oldToken); // Quarantined keys cannot be reused early.
 std::cout<<"PASS combined traffic/police: distinct stable keys, nearby player/NPC and re-entry, shared seven slots/two emergency promotions, police/own/native/visible-traffic priority, exact six-incumbent dark vacancy, generation/expiry guards and independent off switches\n";
}
