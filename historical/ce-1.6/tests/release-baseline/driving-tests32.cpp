#include "ShadowDrivingFocus.hpp"
#include "PlayerShadowBudget.hpp"
#include <cassert>
#include <iostream>
using namespace fusionfix::shadows;
using namespace fusionfix::shadows::budget;
bool has(const PlayerShadowBudget::Selection& s,unsigned id){for(auto x:s.slots)if(x.key==id)return true;return false;}
int main(){
 ShadowDrivingFocus f;
 auto p=f.Update({0,0,0},1,0);assert(p.x==0);
 for(unsigned n=1;n<=100;++n)p=f.Update({n*0.2f,0,0},1,n*10);
 assert(p.x>30 && p.x<=38); // anticipates steady 20m/s drive
 const auto same=f.Update({20,0,0},1,1000);assert(same.x==p.x); // repeated pass no extra smoothing
 p=f.Update({2000,0,0},1,1010);assert(p.x==2000); // teleport resets velocity
 p=f.Update({2000,0,0},2,1020);assert(p.x==2000); // ownership reset
 p=f.Update({2000,0,0},0,1030);assert(p.x==2000); // on foot no lookahead
 f.Reset();f.Update({0,0,0},1,0xfffffff0);p=f.Update({0.2f,0,0},1,4);assert(std::isfinite(p.x));
 assert(DrivingLampPriorityDistance({0,0,0},{18,0,0},{30,0,0},900)==144);
 assert(DrivingLampPriorityDistance({0,0,0},{18,0,0},{30,0,0},900,1)==900);
 assert(DrivingLampPriorityDistance({0,0,0},{18,0,0},{30,0,0},900,1.00001f)>899.99f);
 float last=900;
 for(unsigned step=0;step<=2000;++step){
  float now=DrivingLampPriorityDistance({0,0,0},{18,0,0},{30,0,0},900,1+step*.001f);
  assert(now<=last+.001f&&last-now<.4f);last=now;
 }
 PlayerShadowBudget b;
 auto fill=[&](bool driving,float npcDistance){
  b.Reset();b.BeginPass({1,16,100,driving});
  b.Add({1,0,Kind::Lamp,9,true,true});b.Add({2,1,Kind::Lamp,16,true,true});
  b.Add({3,2,Kind::PlayerBeam,4,true,true});
  b.Add({4,3,Kind::OtherBeam,npcDistance,true,true});
  for(unsigned n=5;n<11;++n)b.Add({n,n-1,Kind::Lamp,100,false,true,0,3,3600});
  return b.Finalize();
 };
 auto a=fill(false,25);assert(a.count==7&&has(a,3)&&has(a,4)); // on-foot contract preserved
 a=fill(true,25);assert(a.count==7&&has(a,3)&&!has(a,4)); // visible lamp wins driving contest
 a=fill(true,1);assert(a.count==7&&has(a,3)&&has(a,4)); // very close relevant NPC still wins
 b.Reset();b.BeginPass({1,16,100,true});
 b.Add({1,0,Kind::Lamp,5000,false,true,0,3,3600,1});
 assert(!has(b.Finalize(),1)); // priority never bypasses physical reach
 // Stable scene must not cycle lamps after hold expiry or shuffled submission.
 b.Reset();std::array<unsigned,7> first{};
 for(unsigned frame=1;frame<200;++frame){
  b.BeginPass({frame,frame*16,100,true});
  for(unsigned j=0;j<12;++j){unsigned id=frame%2?j+1:12-j;b.Add({id,id-1,Kind::Lamp,100.0f+id,false,true,0,2.8f,3600});}
  const auto& s=b.Finalize();assert(s.count==7&&s.safeToApply);
  if(frame==1)for(unsigned i=0;i<7;++i)first[i]=unsigned(s.slots[i].key);
  else for(unsigned i=0;i<7;++i)assert(first[i]==s.slots[i].key);
 }
 // Lookahead can rank a farther eligible lamp above a closer one while driving.
 b.Reset();b.BeginPass({1,16,100,true});
 for(unsigned i=1;i<8;++i)b.Add({i,i-1,Kind::Lamp,400,false,true,0,3,3600});
 b.Add({8,7,Kind::Lamp,900,false,true,0,3,3600,144});
 assert(has(b.Finalize(),8));
 std::cout<<"PASS driving lamp/headlight tradeoff, near NPC, physical limits, stable/shuffled traffic, lookahead/reset/wrap\n";
}
