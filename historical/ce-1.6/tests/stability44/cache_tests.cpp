#include "source/NativeCacheDependencies44.hpp"
#include <cassert>
#include <iostream>
#include <random>
#include <limits>
using namespace fusionfix::shadows;
int main(){
 std::array<NativeCacheEntry44,8> c{};
 for(unsigned i=0;i<8;++i)c[i]={20.0f+i,0,100+i,1};
 // Observed failure: a currently selected lamp's far cache is evicted for a
 // newcomer. Seven dynamic outputs depend on slots1..7. Slot0 can be replaced.
 c[7].distance=80;
 const auto before=c;
 assert(NativeCacheChoice44(10,c)==7);
 assert(ProtectNativeCacheChoice44(7,10,c,0xfe)==0);
 assert(c[7].key==107 && c[7].distance==80);
 // Leave non-conflicting native decisions exactly alone.
 assert(ProtectNativeCacheChoice44(7,10,c,0x7f)==7);
 assert(ProtectNativeCacheChoice44(-1,10,c,0xff)==-1);
 assert(ProtectNativeCacheChoice44(8,10,c,0xff)==8);
 assert(ProtectNativeCacheChoice44(7,10,c,0xff)==-1);
 // A free but closer cache is not overwritten gratuitously. Native refresh
 // can continue when no eligible replacement is available.
 c[0].distance=5;
 assert(ProtectNativeCacheChoice44(7,10,c,0xfe)==-1);
 c[0].state=-1;c[0].age=20;
 assert(ProtectNativeCacheChoice44(7,10,c,0xfe)==0);
 // Native unused/age preference and deterministic ties.
 c[1].state=-1;c[1].age=30;
 assert(NativeCacheChoice44(100,c)==1);
 c[0].age=30;assert(NativeCacheChoice44(100,c)==0);
 c=before; c[0].distance=std::numeric_limits<float>::quiet_NaN();
 assert(NativeCacheChoice44(10,c)==7);
 // Across all masks and changing scenes, replacements never destroy a
 // selected dependency; no protected entries still matches native behavior.
 std::mt19937 rng(440027);unsigned checks=0;
 for(unsigned scene=0;scene<5000;++scene){
  for(unsigned i=0;i<8;++i)c[i]={float(rng()%1000)/10,int(rng()%100),i+1,(rng()%5==0)?-1:1};
  float d=float(rng()%900)/10;
  const int native=NativeCacheChoice44(d,c);
  for(unsigned mask=0;mask<256;++mask){
   int result=ProtectNativeCacheChoice44(native,d,c,static_cast<uint8_t>(mask));
   assert(result>=-1 && result<8);
   assert(result<0 || !(mask&(1u<<result)));
   if(native>=0 && !(mask&(1u<<native)))assert(result==native);
   if(result>=0)assert(c[result].state==-1 || c[result].distance>d);
   ++checks;
  }
 }
 std::cout<<"PASS recorded static-cache dependency loss, native tie/age order, "<<checks<<" cache masks and replacement cases\n";
}
