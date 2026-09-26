#include "fusionfix-source/source/ShadowViewPriority.hpp"
#include "fusionfix-source/source/PlayerShadowBudget.hpp"
#include <cassert>
#include <iostream>
#include <limits>
using namespace fusionfix::shadows;
using namespace fusionfix::shadows::budget;
bool Has(const PlayerShadowBudget::Selection& s, unsigned key) {for(auto x:s.slots) if(x.key==key)return true; return false;}
int main(){
 ShadowView c{}; c.valid=true;
 for(int i=0;i<4;++i)c.view[i][i]=1;
 c.projection[0][0]=c.projection[1][1]=1;
 c.projection[2][2]=1.001f;c.projection[2][3]=1;c.projection[3][2]=-0.1f;
 assert(ViewSample(c,{0,0,10})>0.99f);
 assert(ViewSample(c,{0,0,-10})==0);
 assert(ViewSample(c,{9,0,10})<ViewSample(c,{0,0,10}));
 assert(ShadowViewWeight(c,{0,0,-2},{0,0,1},20,true)>2.9f); // unseen source, visible beam
 assert(ShadowViewWeight(c,{0,0,-20},{0,0,-1},5,true)==1);
 c.valid=false;assert(ShadowViewWeight(c,{0,0,10},{},20,false)==1);
 c.valid=true; c.view[0][0]=std::numeric_limits<float>::quiet_NaN();
 assert(ShadowViewWeight(c,{0,0,10},{},20,false)==1);
 // Local lamp protections survive central distant competition.
 PlayerShadowBudget p;
 auto fill=[&](unsigned frame, bool newLight){
 p.BeginPass({frame,frame*16,100,false});
 p.Add({1,0,Kind::Lamp,400,true,true,0,1});
 p.Add({2,1,Kind::Lamp,400,true,true,0,1});
 for(unsigned i=3;i<=8;++i)p.Add({i,i-1,Kind::Lamp,100,false,true,0,i==8?3.0f:1.0f});
 if(newLight)p.Add({9,8,Kind::Lamp,40,false,true,0,3});
 return p.Finalize();};
 auto a=fill(1,false);assert(a.safeToApply&&a.count==7&&Has(a,1)&&Has(a,2)&&Has(a,8));
 auto b=fill(2,true);assert(!Has(b,9)); // don't switch at first camera movement
 auto d=fill(50,true);assert(Has(d,9)&&Has(d,1)&&Has(d,2));
 p.Reset(); p.BeginPass({1,16,100,true});
 p.Add({20,0,Kind::PlayerBeam,200,true,true,0,1});
 p.Add({21,1,Kind::OtherBeam,200,true,true,0,1});
 for(unsigned i=30;i<45;++i)p.Add({i,i-28,Kind::Lamp,50,false,true,0,3});
 auto e=p.Finalize(); assert(e.count==7&&Has(e,20)&&Has(e,21));
 p.Reset();p.BeginPass({1,16,100,false});p.Add({1,0,Kind::Lamp,1,true,true,0,100});assert(!p.Finalize().safeToApply);
 std::cout<<"Camera relevance, offscreen beams, player protections, hold/replacement and invalid-input checks passed\n";
}
