#include "ExperimentRuntimeOptions.hpp"
#include "TrafficSignalRegistry.hpp"
#include <array>
#include <atomic>
#include <cassert>
#include <iostream>
#include <vector>
using namespace experiment_perf;
using P=lab_siren::NearbyPolice;
std::vector<std::array<uint32_t,2>> Traffic(unsigned logging){
 const auto options=Configure(logging,true,false,false);assert(options.traffic);
 traffic_signal::Registry<256> r;assert(r.Configure(0x100000,0x200000));
 std::vector<std::array<uint32_t,2>> out;
 for(uint32_t f=1;f<500;++f){for(uint32_t owner=1;owner<6;++owner){
  traffic_signal::Identity id{1,2,3,owner*64,owner,f/40,0x4c60ad62};
  if(f%40==0)r.Free(1,id.owner);
  auto v=r.Observe(id,f,f%3,f%17!=0);uint32_t flags=0x200,key=0;
  if(v.index!=UINT32_MAX)r.Apply(v.index,id,f,0x921029,2,f%3,false,flags,key);
  out.push_back({flags,key});
 }if(f==450)r.Disable();}
 return out;
}
int main(){
 for(unsigned logging=0;logging<3;++logging)for(unsigned flags=0;flags<8;++flags){
  const auto o=Configure(logging,flags&1,flags&2,flags&4);
  assert(o.traffic==bool(flags&1));assert(o.detail==(logging==2));
  assert(o.counters==(logging==2&&bool(flags&2)));assert(o.ownerTelemetry==(logging==2&&bool(flags&4)));
 }
 assert(Traffic(0)==Traffic(1)&&Traffic(1)==Traffic(2));
 std::atomic<uint32_t> counter{0};Count(false,counter);assert(counter==0);Count(true,counter);assert(counter==1);
 // Exhaustive policy equivalence; prove visibility work is skipped only when unused.
 uint32_t comparisons=0;
 for(unsigned masks=0;masks<1024;++masks)for(unsigned ap=0;ap<2;++ap)for(unsigned bp=0;bp<2;++bp)
 for(unsigned ar=0;ar<3;++ar)for(unsigned br=0;br<3;++br)for(int native=0;native<3;++native){
  auto bit=[&](unsigned i){return bool(masks&(1u<<i));};unsigned evaluations=0;
  const int old=P::Compare(native,ap,bp,ar,br,bit(0),bit(1),bit(2),bit(3),bit(4),bit(5),bit(6),bit(7),bit(8),bit(9));
  const int next=CompareNearby(native,ap,bp,ar,br,bit(0),bit(1),bit(2),bit(3),bit(4),bit(5),bit(6),bit(7),[&]{++evaluations;return bit(8);},[&]{++evaluations;return bit(9);});
  assert(next==old&&evaluations==(ap==bp?0:2));++comparisons;
 }
 for(uint32_t key=0;key<0x40000;key+=7){constexpr uint32_t base=0x10000;constexpr unsigned tokens=64;
  const auto old=key<base||key-base>=tokens*64||(key-base)%64?tokens:(key-base)/64;
  assert(TokenIndex(key,base,tokens)==old);
 }
 for(unsigned token=0;token<64;++token)assert(TokenIndex(0x10000+token*64,0x10000,64)==token);
 // Same fixed pool/lease snapshot: visiting an entry repeatedly changes no final decision.
 for(unsigned used=0;used<90;++used){
  std::vector<bool> old(used,true),next(used,true);unsigned oldCalls=0,nextCalls=0;
  auto validate=[](unsigned i,bool& live){if(i%3==0)live=false;};
  if(used)for(unsigned n=0;n<32;++n){bool live=old[n%used];validate(n%used,live);old[n%used]=live;++oldCalls;}
  for(unsigned n=0;n<ValidationVisits(used);++n){bool live=next[n%used];validate(n%used,live);next[n%used]=live;++nextCalls;}
  assert(old==next&&nextCalls<=oldCalls&&nextCalls<=32);
 }
 std::cout<<"PASS performance equivalence: logging 0/1/2 identical traffic arguments/lifetimes/free/off; optional counters; "<<comparisons<<" emergency priority comparisons unchanged; token lookup bounds; duplicate validation visits removed without changed decisions\n";
}
