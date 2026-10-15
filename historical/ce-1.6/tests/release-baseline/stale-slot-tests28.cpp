#include "ShadowInactiveSlots.hpp"
#include "ShadowReceiver.hpp"
#include <array>
#include <cassert>
#include <iostream>
using namespace fusionfix::shadows;
int lookup(const std::array<unsigned char,0x880>& records,unsigned key) {
 for(unsigned slot=1;slot<=7;++slot) {
  unsigned stored;std::memcpy(&stored,records.data()+slot*0x110+0xF8,4);
  if(stored==key)return slot;
 }
 return -1;
}
int main(){
 for(unsigned mask=0;mask<128;++mask) {
  std::array<unsigned char,0x880> records;records.fill(0xA5);
  unsigned expected=0;
  for(unsigned slot=1;slot<=7;++slot) {
   records[slot*0x110+0xED]=(mask>>(slot-1))&1;
   unsigned key=100+slot;std::memcpy(records.data()+slot*0x110+0xF8,&key,4);
   assert(lookup(records,key)==int(slot));
   expected+=records[slot*0x110+0xED]==0;
  }
  const auto before=records;
  assert(ClearInactiveShadowKeys(records.data(),records.size())==expected);
  for(unsigned slot=1;slot<=7;++slot)assert(lookup(records,100+slot)==((mask&(1u<<(slot-1)))?int(slot):-1));
  for(unsigned byte=0;byte<records.size();++byte) {
   unsigned slot=byte/0x110,offset=byte%0x110;
   bool changed=slot>=1&&slot<=7&&offset>=0xF8&&offset<0xFC&&!(mask&(1u<<(slot-1)));
   assert(records[byte]==(changed?0:before[byte]));
  }
  assert(ClearInactiveShadowKeys(records.data(),records.size())==0);
  records=before;assert(ClearInactiveShadowKeys(records.data(),0x87F)==0&&records==before);
 }
 assert(ClearInactiveShadowKeys(nullptr,0x880)==0);
 // Earlier geometric admission at a cone edge, without changing illumination.
 Vec3 light{},forward{0,1,0};
 assert(!BeamTouchesReceiver({3,3,0},.75f,light,forward,.9f,20));
 assert(BeamTouchesReceiver({3,3,0},1.5f,light,forward,.9f,20));
 assert(!BeamTouchesReceiver({0,-3,0},1.5f,light,forward,.9f,20));
 std::cout<<"128 vacancy patterns: stale native lookup reproduced, dead keys cleared, live/static records and all other bytes preserved; receiver pre-entry passed\n";
}
