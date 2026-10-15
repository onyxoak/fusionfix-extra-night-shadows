#include "../core/NativeShadowContinuity42.hpp"
#include "../core/HeadlightCasterPolicy.hpp"
#include "../core/SubmittedHeadlightHistory.hpp"
#include "../core/ShadowMapOrdinals20.hpp"
#include <algorithm>
#include <cassert>
#include <cstdio>
int main(){
 using C=fusionfix::shadows::NativeShadowContinuity42;
 C::Candidate left{0x504,0,true,true,1},right{0x504,1,true,true,2};
 // Sorting may see either lamp first and all native distance outcomes.
 for(int native=0;native<3;++native){
  assert(C::Compare(native,left,right)==0);assert(C::Compare(native,right,left)==2);
  auto npc=C::Candidate{0x104,2,false,true,3};
  assert(C::Compare(native,left,npc)==native);assert(C::Compare(native,npc,left)==native);
 }
 left.claim=right.claim=C::Unclaimed;
 assert(C::Compare(0,right,left)==2);assert(C::Compare(2,left,right)==0);
 right.stableOrder=0;assert(C::Compare(0,right,left)==0); // Unknown keys retain native behavior.
 C history;std::array<C::Identity,7> chosen{};chosen[0]={100,0};chosen[1]={101,0};
 history.Begin(1,1,10);assert(history.Commit(chosen));history.Begin(1,2,20);
 assert(history.Observe({100,0},0x504,true,true).claim==0);
 assert(history.Observe({101,0},0x504,true,true).claim==1);
 assert(history.Observe({100,0},0x504,false,true).claim==C::Unclaimed);
 history.Begin(1,3,420); // Slow frames cannot erase a currently valid source's ordinal.
 assert(history.Observe({100,0},0x504,true,true).claim==0);
 assert(history.Observe({100,1},0x504,true,true).claim==C::Unclaimed);
 history.Begin(1,20,440); // Extended missing native updates abandon history.
 assert(history.Observe({100,0},0x504,true,true).claim==C::Unclaimed);
 history.Begin(2,3,30);assert(history.Observe({100,0},0x504,true,true).claim==C::Unclaimed);
 using namespace fusionfix::shadows;
 SubmittedHeadlightHistory lossy;
 for(unsigned key=1;key<=80;++key)lossy.Record(key,10);
 unsigned missing=0;for(unsigned key=1;key<=80;++key)missing+=!lossy.Contains(key,10);
 assert(missing>0); // Real all-car scene exceeds the inherited provenance cache.
 caster::Context context{};context.trafficBeamOwner=0x100000;
 assert(caster::Exclude(context,0x100000,2,true));
 assert(!caster::Exclude(context,0x100100,2,true));
 assert(!caster::Exclude(context,0x100000,1,true)); // Buildings remain eligible casters.
 assert(!caster::Exclude(context,0x100000,2,false));
 context={};assert(!caster::Exclude(context,0x100000,2,true));
 // Every incoming-order permutation retains the same seven native winners
 // and all prior ordinals. This includes nearby signals and moving traffic.
 std::array<ShadowMapOrdinal20,7> picked{};
 for(unsigned i=0;i<7;++i)picked[i]={int32_t(i+10),i,i<2?0x504u:0x264u};
 std::array<int32_t,7> output{};
 unsigned permutations=0;
 do {
  // Native special order is itself established by authenticated lamp sorting.
  if(picked[0].index==10 && picked[1].index==11) {
   assert(RestoreShadowMapOrdinals20(picked,output));
   for(unsigned i=0;i<7;++i)assert(output[i]==int32_t(i+10));
   ++permutations;
  }
 } while(std::next_permutation(picked.begin(),picked.end(),[](auto a,auto b){return a.index<b.index;}));
 assert(permutations==120);
 // A stronger newcomer replaces one loser, without shifting five survivors.
 picked={{{10,0,0x504},{11,1,0x504},{100,7,0x104},{12,2,0x264},{13,3,0x264},{15,5,0x264},{16,6,0x264}}};
 assert(RestoreShadowMapOrdinals20(picked,output));
 assert((output==std::array<int32_t,7>{10,11,12,13,100,15,16}));
 // Missing native candidates leave holes rather than resurrecting an old map.
 picked[2]={};assert(RestoreShadowMapOrdinals20(picked,output));assert(output[4]==-1);
 // Player entry takes native special prefix immediately; old claims vacate it.
 picked={{{100,7,0x504},{101,7,0x504},{10,0,0x264},{11,1,0x264},{12,2,0x264},{13,3,0x264},{14,4,0x264}}};
 assert(RestoreShadowMapOrdinals20(picked,output));assert(output[0]==100&&output[1]==101);
 auto expected=output;std::sort(expected.begin(),expected.end());
 assert((expected==std::array<int32_t,7>{10,11,12,13,14,100,101}));
 // Collision/duplicate/bad native inputs must leave caller output untouched.
 auto sentinel=output;picked[5].claim=2;
 assert(!RestoreShadowMapOrdinals20(picked,output));assert(output==sentinel);
 picked[5].claim=7;picked[5].index=12;
 assert(!RestoreShadowMapOrdinals20(picked,output));assert(output==sentinel);
 picked[5].index=-2;assert(!RestoreShadowMapOrdinals20(picked,output));assert(output==sentinel);
 std::puts("physical lamp ordering, priority boundaries, reset and dense-scene self-caster authority checks passed");
}
