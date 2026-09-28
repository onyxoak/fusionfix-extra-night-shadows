#include "source/NativeLampContinuity41.hpp"
#include "source/NativeLampDistance38.hpp"
#include <cassert>
#include <iostream>
using namespace fusionfix::shadows;
int main(){
 NativeLampContinuity41 h;
 h.Begin(1,1,16);for(unsigned k=1;k<=7;++k)h.Selected(k,k*10);
 h.Begin(1,2,32);h.Selected(8,80);
 h.Begin(1,3,48);assert(h.Retained(1,10,true));assert(!h.Retained(8,80,true));
 h.Selected(1,10);
 for(unsigned f=4;f<1000;++f){
  h.Begin(1,f,f*16);
  // A returning lamp keeps its claim through repeated two-frame gaps.
  assert(h.Retained(1,10,true));
  if(f%3==0)h.Selected(1,10);
 }
 assert(!h.Retained(1,11,true)); // recycled identity
 h.Selected(1,11);assert(!h.Retained(1,11,false)); // leaves visible/range region
 h.Selected(1,11);h.Begin(2,1000,16000);assert(!h.Retained(1,11,true));
 h.Selected(2,20);h.Begin(2,1009,16144);assert(!h.Retained(2,20,true));
 h.Selected(2,20);h.Begin(2,1010,16270);assert(!h.Retained(2,20,true));
 // Incumbents resist modest approach competition; substantially nearer lights
 // still replace them. Acquisition and physical light properties are untouched.
 float held=NativeLampPriorityDistance(20,400,3,60,true);
 assert(held<NativeLampPriorityDistance(17,400,2,60,false));
 assert(NativeLampPriorityDistance(8,64,2,60,false)<held);
 assert(NativeLampPriorityDistance(20,3600,3,60,true)==20);
 std::cout<<"PASS lamp lease: two-frame loss, newcomer resistance, identity reuse, offscreen/range release, session and timeout reset, bounded priority\n";
}
