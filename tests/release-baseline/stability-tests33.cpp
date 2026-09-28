#include "PlayerShadowBudget.hpp"
#include "ShadowAllocationPass.hpp"
#include <cassert>
#include <iostream>
using namespace fusionfix::shadows::budget;
bool has(const PlayerShadowBudget::Selection& s,unsigned key){for(auto c:s.slots)if(c.key==key)return true;return false;}
PlayerShadowBudget::Selection scene(PlayerShadowBudget& b,unsigned time,bool local,bool missing=false,bool beam=false,unsigned generation=0,bool reverse=false,unsigned long long serial=0){
 b.BeginPass({serial?serial:time,time,123,true});
 for(unsigned j=1;j<=8;++j){unsigned i=reverse?9-j:j;if(missing&&i==7)continue;
  b.Add({i,i-1,Kind::Lamp,float(i*10),i==8&&local,true,i==8?generation:0,1,3600});}
 if(beam)b.Add({9,8,Kind::PlayerBeam,1,true,true});
 auto s=b.Finalize();assert(s.safeToApply&&s.count<=7);return s;
}
int main(){
 // Reproduce the protected-group boundary bypass in the previous policy.
 PlayerShadowBudget old;scene(old,0,false);assert(has(scene(old,16,true),8));
 PlayerShadowBudget::Policy p;p.replacementConfirmMs=80;PlayerShadowBudget b(p);
 auto s=scene(b,0,false);assert(has(s,7)&&!has(s,8));
 // A brief local-boundary crossing cannot evict a valid incumbent, even
 // though the protected local group picked it before the hold comparison.
 for(unsigned t=16;t<1000;t+=16){s=scene(b,t,(t/16)%2,false,false,0,t%32==0);assert(has(s,7)&&!has(s,8));}
 // A sustained requested replacement becomes available after confirmation.
 assert(!has(scene(b,1000,true),8));assert(!has(scene(b,1040,true),8));
 assert(has(scene(b,1080,true),8));
 // No stale ownership if the old light leaves native input.
 b.Reset();scene(b,0,false);assert(has(scene(b,16,true,true),8));
 // Player beam is immediate despite a pending lamp swap.
 b.Reset();scene(b,0,false);assert(has(scene(b,16,true,false,true),9));
 // Key reuse cannot inherit a previous generation's confirmation interval.
 b.Reset();scene(b,0,false);scene(b,16,true,false,false,1);
 assert(!has(scene(b,80,true,false,false,2),8));assert(has(scene(b,160,true,false,false,2),8));
 // Session switch clears pending and resident history; fresh inputs fill.
 b.BeginPass({200,200,456,true});b.Add({8,0,Kind::Lamp,80,true,true});assert(has(b.Finalize(),8));
 // A newly entering light never waits when there is spare atlas capacity.
 b.Reset();b.BeginPass({0,0,123,true});b.Add({1,0,Kind::Lamp,1,false,true});b.Finalize();
 b.BeginPass({16,16,123,true});b.Add({1,0,Kind::Lamp,1,false,true});b.Add({2,1,Kind::Lamp,2,false,true});assert(has(b.Finalize(),2));
 // Sustained movement with shuffled input: each retained identity must still
 // resolve to this pass's index, not a saved index from the last selection.
 b.Reset();for(unsigned frame=0;frame<3000;++frame){unsigned base=frame/4;
  b.BeginPass({frame,frame*16,123,true});
  for(unsigned j=0;j<20;++j){unsigned i=frame%2?19-j:j;unsigned id=base+i+1;
   b.Add({id,j,Kind::Lamp,float(i*i+1),i<2,true,0,2,3600});}
  const auto& out=b.Finalize();assert(out.count==7&&out.safeToApply);
  for(auto c:out.slots){assert(c.key>=base+1&&c.key<=base+20);unsigned i=unsigned(c.key)-base-1;assert(c.index==(frame%2?19-i:i));}
 }
 // A one-pass nearby NPC beam cannot flash into a full lamp allocation;
 // a sustained beam can, while loss of actual beam coverage removes it.
 b.Reset();scene(b,0,false);
 auto npc=[&](unsigned t,bool touches){b.BeginPass({t,t,123,true});
  for(unsigned i=1;i<=7;++i)b.Add({i,i-1,Kind::Lamp,float(i*10),false,true,0,1,3600});
  b.Add({9,8,Kind::OtherBeam,0,touches,true,0,1,3600});return b.Finalize();};
 assert(!has(npc(600,true),9));assert(!has(npc(632,true),9));assert(has(npc(680,true),9));assert(!has(npc(696,false),9));
 // Timer wrap cannot accidentally bypass the confirmation period.
 b.Reset();scene(b,0xffffffc0,false,false,false,0,false,1);assert(!has(scene(b,0xfffffff0,true,false,false,0,false,2),8));
 assert(!has(scene(b,0x10,true,false,false,0,false,3),8));assert(has(scene(b,0x40,true,false,false,0,false,4),8));
 std::cout<<"PASS old boundary bypass reproduced; transient vs sustained replacement, player beam, absent inputs, generation, session, spare capacity and 3000 fast shuffled passes\n";
}
