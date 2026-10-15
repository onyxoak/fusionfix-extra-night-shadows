#include "core/IndoorBubble212.hpp"
#include <cassert>
#include <limits>
int main(){using namespace indoor212;using namespace fusionfix::shadows;
 assert(std::fabs(Feet35-10.668f)<.00001f);assert(Strength({0,0,0},{6.096f,0,0})==1);
 assert(Strength({0,0,0},{Feet30,0,0})==1);assert(Strength({0,0,0},{Feet35,0,0})<0);
 assert(Strength({0,0,0},{10,0,0})>0&&Strength({0,0,0},{10,0,0})<1);
 assert(Strength({0,0,0},{std::numeric_limits<float>::quiet_NaN(),0,0})<0);
 ShadowView v{};v.valid=true;for(int k=0;k<4;++k)v.view[k][k]=1;
 v.projection[0][0]=v.projection[1][1]=v.projection[2][2]=v.projection[2][3]=1;
 SceneReceiverConsequence44 c{};c.valid=true;c.score=40;c.consequence=0;
 auto central=Rank(c,v,{0,0,0},{0,0,6.096f},{0,0,-1},1,20,{1,.2f,0},0);
 auto edge=Rank(c,v,{0,0,0},{5,0,6.096f},{0,0,-1},1,20,{1,.2f,0},0);
 assert(central.score<c.score&&central.score<edge.score);
 assert(Rank(c,v,{0,0,0},{0,0,20},{0,0,-1},1,20,{1,.2f,0},0).score==c.score);
 consumer_shadow75::Packet p{};p.words[24]=7;p.words[26]=19719;p.words[27]=1;
 assert(Packet(p));p.words[18]=0x100;assert(!Packet(p));p.words[18]=0;p.words[26]=0;assert(!Packet(p));
}
