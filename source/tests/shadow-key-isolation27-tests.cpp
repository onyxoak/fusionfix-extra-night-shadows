#include "core/ShadowAllocationPass.hpp"
#include "core/ShadowKeyMultiplicity27.hpp"
#include "core/NativeShadowContinuity42.hpp"
#include <cassert>
#include <algorithm>
int main() {
 using namespace fusionfix::shadows;
 ShadowKeyMultiplicity27 census;census.Begin();
 assert(census.Add(101,0)&&census.Add(101,1)&&census.Add(215,2));
 assert(census.Ambiguous(0)&&census.Ambiguous(1)&&!census.Ambiguous(2));
 assert(census.Add(101+2048,3)&&!census.Ambiguous(3)); // Hash collision != key alias.
 assert(!census.Add(20,640));census.Begin();assert(!census.Ambiguous(0));
 using namespace fusionfix::shadows::budget;
 std::array<std::array<unsigned char,128>,3> packets{};
 ShadowAllocationPass pass;
 auto observe=[&] {for(unsigned i=0;i<3;++i)
  pass.Observe({i<2?101u:215u,i,Kind::Lamp,10.f,true,true,1},packets[i].data());};
 std::array<int32_t,7> chosen{0,1,2,-1,-1,-1,-1};
 pass.Begin({1,1,100,false},packets.data(),3);observe();
 assert(!pass.Active()&&pass.FailureCode()==3); // Legacy policy remains strict.
 pass.Begin({2,2,100,false},packets.data(),3,true);observe();
 assert(pass.Active()&&pass.Commit(packets.data(),3,chosen.data(),true,true));
 assert(chosen[0]==0&&chosen[1]==1&&chosen[2]==2); // No light gets silently deleted.
 pass.Begin({3,3,100,false},packets.data(),3,true);observe();packets[2][0]=1;
 const auto before=chosen;
 assert(!pass.Commit(packets.data(),3,chosen.data(),true,true)&&chosen==before);
 pass.Begin({4,4,100,false},packets.data(),3,true);observe();
 assert(!pass.Commit(packets.data(),3,chosen.data(),false,false));
 NativeShadowContinuity42 continuity;continuity.Begin(100,1,10);
 std::array<NativeShadowContinuity42::Identity,7> ids{};ids[4]={215,1};
 assert(continuity.Commit(ids));
 assert(continuity.Observe({215,1},4,true,false).claim==4);
 assert(continuity.Observe({101,1},4,false,false).claim==7);
 // All aliases use the SAME score comparator as unique sources. Traversal
 // permutations cannot change membership through native/custom pair cycles.
 using C=NativeShadowContinuity42::Candidate;
 std::array<C,3> candidates{};
 for(unsigned i=0;i<3;++i){candidates[i].observed=true;candidates[i].identity=i<2?101:215;
  candidates[i].focusDistance=float(10+10*i);candidates[i].focusTier=1;}
 candidates[2].claim=4;
 auto impact=candidates[2];impact.visualImportance=2.5f;
 assert(NativeShadowContinuity42::RetainedScore(impact)<NativeShadowContinuity42::RetainedScore(candidates[2]));
 impact.claim=7;assert(NativeShadowContinuity42::RetainedScore(impact)==impact.focusDistance);
 std::array<unsigned,3> order{0,1,2};
 do {auto sorted=order;std::sort(sorted.begin(),sorted.end(),[&](unsigned a,unsigned b){
  return NativeShadowContinuity42::Compare(1,candidates[a],candidates[b])==0;});
  assert((sorted==std::array<unsigned,3>{0,1,2}));
 }while(std::next_permutation(order.begin(),order.end()));
}
