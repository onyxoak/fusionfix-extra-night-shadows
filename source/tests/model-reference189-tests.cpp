#include "core/ModelReferenceQueue189.hpp"
#include <cassert>
#include <vector>
#include <memory>
#include <algorithm>
using namespace model_reference189;
int main(){
 // Four independently draining rings: native AddRef/Release must balance,
 // including repeated draws and the same model held by several rings.
 std::array<Seen,4> state{};std::array<std::vector<uint16_t>,4> storage;
 std::array<std::vector<uint16_t>,4> spill;
 std::array<uint16_t,4> counts{};std::array<int,65536> refs{};
 for(auto& v:storage)v.resize(16384);
 auto retain=[&](unsigned ring,uint16_t model){
  auto* data=storage[ring].data();uint16_t cap=uint16_t(storage[ring].size());
  return Retain(state[ring],model,data,counts[ring],cap,
   [&](uint16_t id){++refs[id];spill[ring].push_back(id);return true;},
   [&]{++refs[model];data[counts[ring]++]=model;});
 };
 auto drain=[&](unsigned ring){for(unsigned j=0;j<counts[ring];++j)--refs[storage[ring][j]];for(auto id:spill[ring])--refs[id];spill[ring].clear();counts[ring]=0;state[ring].known=false;};
 for(unsigned frame=0;frame<16;++frame){
  const auto ring=frame%4;drain(ring);
  for(unsigned pass=0;pass<7;++pass)for(uint16_t model=0;model<694;++model)
   assert(retain(ring,model)==(pass?Result::AlreadyHeld:Result::Added));
  assert(counts[ring]==694);
 }
 for(unsigned ring=0;ring<4;++ring)drain(ring);
 assert(std::all_of(refs.begin(),refs.end(),[](int n){return n==0;}));
 // Every native model can remain retained: capacity stays fixed without losing or
 // duplicating references. Overflow uses a separate balanced hold and never
 // changes the native buffer pointer, capacity or allocator.
 for(uint16_t model=0;model<31000;++model)assert(retain(0,model)==Result::Added);
 assert(counts[0]==16384&&storage[0].size()==16384&&spill[0].size()==14616);
 for(uint16_t model=0;model<31000;++model)assert(retain(0,model)==Result::AlreadyHeld);
 drain(0);assert(std::all_of(refs.begin(),refs.end(),[](int n){return n==0;}));
 // Activation into a nonempty native queue preserves its preexisting release
 // obligations and recognizes its already-held models.
 std::array<uint16_t,3> prior{9,9,12};uint16_t* p=prior.data();uint16_t n=3,c=3;Seen s;
 unsigned appended=0;auto noReserve=[](uint16_t){return false;};auto append=[&]{++appended;};
 assert(Retain(s,9,p,n,c,noReserve,append)==Result::AlreadyHeld&&appended==0);
 assert(Retain(s,42,p,n,c,noReserve,append)==Result::Invalid&&appended==0);
 n=4;assert(Retain(s,9,p,n,c,noReserve,append)==Result::Invalid);
 // Model indices at the full uint16 boundary cannot alias a different bit.
 n=0;c=3;s={};assert(Retain(s,65535,p,n,c,noReserve,[&]{p[n++]=65535;})==Result::Added);
 assert(Retain(s,65535,p,n,c,noReserve,append)==Result::AlreadyHeld);
}
