#include "../../modified-source/ShadowReach.hpp"
#include "../../modified-source/PlayerShadowBudget.hpp"
#include <iostream>
#include <limits>
#include <cstdlib>
using namespace fusionfix::shadows;
int main(){unsigned checks=0;auto check=[&](bool v){++checks;if(!v){std::cerr<<"FAIL "<<checks;std::exit(1);}};
check(ShadowReachFeet(-1)==0);check(ShadowReachFeet(99)==75);check(ShadowReachFeet(10)==50);
for(int step=1;step<=15;++step){float m=step*5*0.3048f;check(WithinShadowReach(m*m,step));check(!WithinShadowReach((m+0.1f)*(m+0.1f),step));check(WithinShadowReach(0,step));}
check(!WithinShadowReach(0,0));check(!WithinShadowReach(-1,10));check(!WithinShadowReach(std::numeric_limits<float>::infinity(),10));check(!WithinShadowReach(std::numeric_limits<float>::quiet_NaN(),10));
for(int step: {0,5,10,15}) {
 budget::PlayerShadowBudget p;p.BeginPass({1,16,100,false});
 // A 40-foot headlight source outside the player's actual beam volume.
 float d=40*0.3048f;bool relevant=WithinShadowReach(d*d,step);
 p.Add({1,0,budget::Kind::OtherBeam,d*d,relevant,true,0});
 auto result=p.Finalize();check(result.safeToApply);check(result.count==(step>=8?1:0));
}
budget::PlayerShadowBudget crowded;crowded.BeginPass({1,16,100,false});
for(unsigned i=1;i<=30;++i)crowded.Add({i,i,budget::Kind::OtherBeam,float(i),true,true,0});
auto r=crowded.Finalize();check(r.safeToApply);check(r.count==2);
std::cout<<checks<<" reach and shadow-budget checks passed\n";
}
