#include "fusionfix-source/source/StableHeadlightSelector.hpp"
#include "fusionfix-source/source/PlayerShadowBudget.hpp"
#include <cassert>
#include <iostream>
using namespace fusionfix::shadows;
using namespace fusionfix::shadows::budget;
bool has(const PlayerShadowBudget::Selection& s, unsigned id){for(auto x:s.slots)if(x.key==id)return true;return false;}
int main(){
 StableHeadlightSelector s;
 HeadlightCandidate npc{1,{9,true,true},false,3.048f*3.048f,true};
 s.BeginFrame({1,16,100,false,0}); assert(!s.Consider(npc));
 s.BeginFrame({2,32,100,false,0}); assert(s.Consider(npc));
 npc.geometry.distanceSquared=12; assert(s.Consider(npc)); // retained up to twelve feet
 npc.geometry.distanceSquared=14; assert(!s.Consider(npc));
 s.BeginFrame({3,48,100,false,0}); assert(s.ActiveIdentities()[0]==0);
 npc.geometry.distanceSquared=12; assert(!s.Consider(npc));
 s.BeginFrame({4,64,100,false,0}); assert(s.ActiveIdentities()[0]==0); // cannot newly enter at eleven feet
 npc.geometry.distanceSquared=4; npc.geometry.aimedAtPlayer=false; assert(!s.Consider(npc));
 s.BeginFrame({5,80,100,false,0}); assert(s.ActiveIdentities()[0]==0);
 HeadlightCandidate own{2,{200,true,false},true,15.24f*15.24f,false};
 assert(!s.Consider(own));
 s.BeginFrame({6,96,100,false,0}); assert(s.Consider(own)); // parked beam pointing away still useful
 npc.geometry.aimedAtPlayer=true; assert(!s.Consider(npc));
 s.BeginFrame({7,112,100,false,0}); assert(s.Consider(own)&&s.Consider(npc));
 s.BeginFrame({8,128,100,true,300}); assert(s.ActiveIdentities()[0]==0&&s.ActiveIdentities()[1]==0);
 own.identity=300; assert(!s.Consider(own));
 s.BeginFrame({9,144,100,true,300}); assert(s.Consider(own));
 for(unsigned f=10;f<20;++f)s.BeginFrame({f,f*16,100,false,0});
 assert(s.ActiveIdentities()[0]==0&&s.ActiveIdentities()[1]==0); // lights off: no reserved dead slots
 PlayerShadowBudget b;
 auto fill=[&](unsigned f){
  b.BeginPass({f,f*16,100,false});
  b.Add({1,0,Kind::Lamp,25,true,true,0,1,3600});
  b.Add({2,1,Kind::Lamp,36,true,true,0,1,3600});
  b.Add({3,2,Kind::PlayerBeam,225,false,true,0,1,232.26f});
  b.Add({4,3,Kind::OtherBeam,4,true,true,0,1,9.2904f});
  for(unsigned i=5;i<15;++i)b.Add({i,i-1,Kind::Lamp,i==14?3600.0f:100.0f,false,true,0,i==14?3.0f:1.0f,3716.122f});
  return b.Finalize();
 };
 auto a=fill(1); assert(a.safeToApply&&a.count==7&&has(a,1)&&has(a,2)&&has(a,3)&&has(a,4)&&has(a,14));
 b.Reset(); b.BeginPass({1,16,100,false});
 b.Add({1,0,Kind::Lamp,3700,false,true,0,3,3716.122f}); assert(has(b.Finalize(),1)); // actual 200 feet, no hidden 60m cap
 b.BeginPass({2,32,100,false}); b.Add({1,0,Kind::Lamp,4000,false,true,0,3,3716.122f}); assert(has(b.Finalize(),1));
 b.BeginPass({3,48,100,false}); b.Add({1,0,Kind::Lamp,5500,false,true,0,3,3716.122f}); assert(!has(b.Finalize(),1));
 std::cout<<"Priority26: NPC radius/direction, exit hysteresis, parked-car priority, ownership reset, lights-off expiry, lamp share and 200-foot reach passed\n";
}
