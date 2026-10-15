// Deterministic observer model: real sealed-v9/candidate registries and candidate
// gate, simulated native reads. Counts below are source-derived work, not FPS.
#define traffic_signal reference_traffic_signal
#include "../test-baseline-v9/TrafficSignalRegistry.hpp"
#undef traffic_signal
#include "TrafficSignalDistanceGate.hpp"
#include <array>
#include <bit>
#include <cassert>
#include <iostream>
#include <limits>
#include <vector>
struct Input {
 uint32_t frame=10,owner=0x100040,pool=1,reference=2,model=3,storage=4,flagsAddress=5,slot=1;
 uint32_t caller=0x921029,type=2,flags=0x200,key=0,phase=0,invalidOwnerQueries=1;
 float light[3]{},player[3]{},rgb[3]{1,1,1},reach=40;
 bool argsReadable=true,ownerValid=true,playerReadable=true,allocator=true,collision=false,cameraVisible=true;
};
struct Output {uint32_t flags{},key{};bool applied{};friend bool operator==(const Output&,const Output&)=default;};
template<class R,bool Fast> struct Harness {
 R r;traffic_signal::PlayerPositionSnapshot cached;
 uint64_t validations{},ownerQueries{},playerReads{},earlySkips{},trackingLookups{};
 std::vector<Output> outputs;
 Harness(){assert(r.Configure(0x400000,0x800000));}
 auto Id(const Input& i) const {using I=decltype(r.entries[0].id);return I{i.pool,i.storage,i.flagsAddress,i.owner,i.slot,i.reference,i.model};}
 bool Position(const Input& i,float (&out)[3]) {
  // Original per-frame cache semantics, including malformed/stale/UINT_MAX frames.
  if(i.frame!=cached.frame){cached.frame=i.frame;cached.valid=false;++playerReads;
   if(i.playerReadable){cached.valid=true;for(unsigned n=0;n<3;++n){cached.position[n]=i.player[n];cached.valid=cached.valid&&std::isfinite(cached.position[n]);}}
  }if(cached.valid)for(unsigned n=0;n<3;++n)out[n]=cached.position[n];return cached.valid;
 }
 bool Validate(const Input& i){++validations;ownerQueries+=i.ownerValid?6:i.invalidOwnerQueries;return i.ownerValid;}
 Output Produce(const Input& i){
  Output out{i.flags,i.key,false};
  auto finish=[&]{outputs.push_back(out);return out;};
  if(i.caller!=0x921029||!i.argsReadable||i.type!=2||i.flags!=0x200||i.key||i.phase>2)return finish();
  for(unsigned n=0;n<3;++n)if(!std::isfinite(i.light[n])||!std::isfinite(i.rgb[n]))return finish();
  if constexpr(Fast)if(traffic_signal::RejectUntrackedFar(i.owner,i.frame,i.light,i.reach,cached,
   [&]{return i.allocator;},[&](uint32_t owner){++trackingLookups;return r.TracksOwner(owner);})){++earlySkips;return finish();}
  if(!Validate(i))return finish();
  float player[3]{};const bool relevant=i.allocator&&Position(i,player)&&traffic_signal::Nearby(i.light,player,i.reach);
  const auto result=r.Observe(Id(i),i.frame,i.phase,relevant);
  if(result.index!=UINT32_MAX){auto& e=r.entries[result.index];
   if(e.leased&&!(e.submitted&&e.lastAdmission==i.frame))
    out.applied=r.Apply(result.index,Id(i),i.frame,i.caller,i.type,i.phase,i.collision,out.flags,out.key);
  }return finish();
 }
 void Free(const Input& i){r.Free(i.pool,i.owner);}
 void Timer(const Input& current,uint32_t restored,uint32_t saved,bool backup){
  if(!backup||restored!=saved)return;
  for(uint32_t n=0;n<r.Used();++n){auto& e=r.entries[n];if(e.retired||!e.leased)continue;
   Input i=current;i.owner=e.id.owner;
   if(!Validate(i)||!(Id(i)==e.id)){r.Retire(n,decltype(e.reason)::Identity);continue;}
   r.Rebase(n,Id(i),restored,saved,true);
  }
 }
 void Poll(const Input& current,uint32_t frame){
  for(uint32_t n=0;n<r.Used();++n){auto& e=r.entries[n];if(e.retired)continue;
   Input i=current;i.owner=e.id.owner;
   if(!Validate(i)||!(Id(i)==e.id)){r.Retire(n,decltype(e.reason)::Identity);continue;}
   const auto age=uint32_t(frame-e.lastFrame);
   if(age>=0x80000000u)r.Retire(n,decltype(e.reason)::Clock);else if(age>2)r.Suspend(n);
  }
 }
};
using Old=Harness<reference_traffic_signal::Registry<256>,false>;
using New=Harness<traffic_signal::Registry<256>,true>;
struct Pair {
 Old old;New next;uint64_t events{};
 void Check(){
  assert(old.outputs==next.outputs&&old.r.Used()==next.r.Used()&&old.r.Disabled()==next.r.Disabled());
  assert(old.cached.frame==next.cached.frame&&old.cached.valid==next.cached.valid);
  for(unsigned n=0;n<3;++n)assert(std::bit_cast<uint32_t>(old.cached.position[n])==std::bit_cast<uint32_t>(next.cached.position[n]));
  for(uint32_t n=0;n<old.r.Used();++n){const auto& a=old.r.entries[n];const auto& b=next.r.entries[n];
   assert(a.id.pool==b.id.pool&&a.id.storage==b.id.storage&&a.id.flags==b.id.flags&&a.id.owner==b.id.owner&&
    a.id.slot==b.id.slot&&a.id.reference==b.id.reference&&a.id.model==b.id.model);
   assert(a.key==b.key&&a.generation==b.generation&&a.lastFrame==b.lastFrame&&a.lastAdmission==b.lastAdmission&&
    a.phase==b.phase&&a.phaseMask==b.phaseMask&&a.retired==b.retired&&a.leased==b.leased&&a.submitted==b.submitted&&uint32_t(a.reason)==uint32_t(b.reason));
   for(uint32_t pool=1;pool<=3;++pool)assert(old.r.FindOwner(pool,a.id.owner)==next.r.FindOwner(pool,a.id.owner));
  }
 }
 Output Produce(Input i){++events;const auto a=old.Produce(i),b=next.Produce(i);assert(a==b);Check();return b;}
 void Free(Input i){++events;old.Free(i);next.Free(i);Check();}
 void Timer(Input i,uint32_t restored,uint32_t saved,bool backup){++events;old.Timer(i,restored,saved,backup);next.Timer(i,restored,saved,backup);Check();}
 void Poll(Input i,uint32_t frame){++events;old.Poll(i,frame);next.Poll(i,frame);Check();}
};
int main(){
 uint64_t totalEvents=0;
 // Exactly 40 m is retained, including diagonal/negative coordinates, independent of camera.
 for(const auto x:std::array<float,6>{40,std::nextafter(40.f,0.f),std::nextafter(40.f,100.f),-40,0,24}){
  Pair p;Input warm;warm.owner=0x111111;assert(p.Produce(warm).applied);
  Input i;i.light[0]=x;if(x==24)i.light[1]=32;
  i.cameraVisible=false;auto o=p.Produce(i);assert(o.applied==traffic_signal::Nearby(i.light,i.player,40));
  totalEvents+=p.events;
 }
 // Cold and stale caches must NOT be initialized by invalid native owners.
 {Pair p;Input i;i.ownerValid=false;i.playerReadable=false;i.light[0]=500;p.Produce(i);
  assert(p.next.cached.frame==UINT32_MAX&&p.next.playerReads==0&&p.next.earlySkips==0);
  i.ownerValid=true;i.playerReadable=true;i.light[0]=0;assert(p.Produce(i).applied);
  i.frame++;i.ownerValid=false;i.player[0]=1000;i.light[0]=500;p.Produce(i);
  assert(p.next.cached.frame==10);i.ownerValid=true;i.light[0]=1000;assert(p.Produce(i).applied);
  totalEvents+=p.events;
 }
 // Invalid cached position, failed player read, disabled allocator, malformed inputs.
 {Pair p;Input i;i.player[1]=std::numeric_limits<float>::quiet_NaN();p.Produce(i);
  i.owner++;i.light[0]=500;i.player[1]=0;p.Produce(i);assert(p.next.earlySkips==0);
  i.frame++;i.playerReadable=false;p.Produce(i);i.playerReadable=true;p.Produce(i);assert(p.next.earlySkips==0);
  i.frame++;i.light[0]=0;p.Produce(i);i.owner++;i.light[0]=500;i.allocator=false;p.Produce(i);assert(p.next.earlySkips==0);
  i.allocator=true;auto bad=i;bad.phase=3;p.Produce(bad);bad=i;bad.caller++;p.Produce(bad);bad=i;bad.type=1;p.Produce(bad);
  bad=i;bad.flags=0x204;p.Produce(bad);bad=i;bad.key=0x123;p.Produce(bad);bad=i;bad.argsReadable=false;p.Produce(bad);
  bad=i;bad.light[2]=std::numeric_limits<float>::infinity();p.Produce(bad);bad=i;bad.rgb[0]=std::numeric_limits<float>::quiet_NaN();p.Produce(bad);
  assert(p.next.earlySkips==0);totalEvents+=p.events;
 }
 // All identity replacements at a tracked address must validate/retire even far away.
 for(unsigned field=0;field<6;++field){Pair p;Input i;assert(p.Produce(i).applied);auto used=p.next.validations;
  i.light[0]=500;if(field==0)i.pool++;if(field==1)i.storage++;if(field==2)i.flagsAddress++;if(field==3)i.slot++;if(field==4)i.reference++;if(field==5)i.model++;
  p.Produce(i);assert(p.next.validations==used+1&&p.next.r.entries[0].retired&&p.next.r.Used()==1);
  i.light[0]=0;assert(p.Produce(i).applied&&p.next.r.Used()==2);totalEvents+=p.events;
 }
 {Pair p;Input i;i.collision=true;p.Produce(i);assert(p.next.r.TracksOwner(i.owner));
  i.collision=false;i.light[0]=500;const auto calls=p.next.validations;p.Produce(i);assert(p.next.validations==calls+1);
  p.Free(i);assert(!p.next.r.TracksOwner(i.owner));i.reference=130;i.light[0]=0;assert(p.Produce(i).applied);
  p.Free(i);i.reference&=127;assert(p.Produce(i).applied);totalEvents+=p.events;
 }
 // Untracked malformed far owner is safe to ignore only with warm valid cache.
 {Pair p;Input i;p.Produce(i);i.owner++;i.light[0]=500;i.ownerValid=false;const auto calls=p.next.validations;
  p.Produce(i);assert(p.next.validations==calls&&p.next.earlySkips==1);totalEvents+=p.events;
 }
 // Genuine free/stream-out tombstone and re-entry, stale lease, wrap/backward clock.
 {Pair p;Input i;i.frame=UINT32_MAX;p.Produce(i);assert(!p.next.cached.valid);
  i.frame=UINT32_MAX-2;p.Produce(i);p.Free(i);i.reference++;i.frame=0;p.Produce(i);
  i.frame=4;p.Produce(i);p.Poll(i,7);i.frame=8;p.Produce(i);
  p.Timer(i,6,6,true);i.frame=7;p.Produce(i);i.frame=5;p.Produce(i);totalEvents+=p.events;
 }
 // A timer scan can retire replaced identity without a backward-clock rebase.
 {Pair p;Input i;p.Produce(i);i.reference++;p.Timer(i,10,10,true);
  assert(p.next.r.entries[0].retired&&p.next.r.entries[0].reason==traffic_signal::End::Identity);
  traffic_signal::Registry<8> naive;assert(naive.Configure(0x400000,0x800000));
  const auto id=traffic_signal::Identity{1,4,5,0x100040,1,2,3};auto result=naive.Observe(id,10,0,true);
  // Proposed no-rebase shortcut omits validation and leaves the old owner alive.
  assert(uint32_t(10-naive.entries[result.index].lastFrame)<=0x80000000u&&!naive.entries[result.index].retired);
  totalEvents+=p.events;
 }
 // Deterministic long mixed trace: hash collisions, capacity, replacements, free,
 // malformed producers, camera visibility, all finite extremes and varied ranges.
 {Pair p;uint32_t seed=0xabcdef01;auto rnd=[&]{seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;};
  for(unsigned n=0;n<12000;++n){Input i;const auto bits=rnd();i.frame=n/40;i.owner=0x100000+(bits%400)*64;
   i.reference=(bits>>9)&127;i.model=3+((bits>>16)%4);i.pool=1+((bits>>18)%2);i.phase=(bits>>12)%3;
   i.light[0]=float(int((bits>>5)%201)-100);i.light[1]=float(int((bits>>17)%81)-40);i.player[0]=float(int(n/160)%20-10);
   i.cameraVisible=bool(bits&1);i.ownerValid=bits%17!=0;i.playerReadable=bits%23!=0;i.allocator=bits%29!=0;i.collision=bits%131==0;
   if(bits%19==0)i.reach=0;if(bits%37==0)i.reach=100;if(bits%43==0)i.light[0]=std::numeric_limits<float>::max();
   if(bits%47==0)i.rgb[1]=std::numeric_limits<float>::infinity();
   p.Produce(i);if(bits%31==0)p.Free(i);if(bits%101==0)p.Poll(i,i.frame);if(bits%107==0)p.Timer(i,i.frame,i.frame,true);
  }totalEvents+=p.events;
 }
 // Source-derived validation savings only: 1 cold valid far callback, then 1000
 // warm untracked far callbacks, vs 1001 full owner validations in baseline.
 {Pair p;Input i;i.light[0]=500;
  for(unsigned n=0;n<1001;++n){i.owner=0x100000+n*64;p.Produce(i);}
  assert(p.old.validations==1001&&p.next.validations==1&&p.next.earlySkips==1000);
  assert(p.old.ownerQueries-p.next.ownerQueries==6000&&p.next.r.Used()==0&&p.next.playerReads==1);
  std::cout<<"PASS source-derived work count: 1000/1001 far-owner validations eliminated, 6000 native metadata VirtualQuery calls avoided; argument checks and first-frame player read retained. No timing/FPS measurement.\n";
  totalEvents+=p.events;
 }
 std::cout<<"PASS "<<totalEvents<<" equivalence events: identical producer mutations, all registry lifetimes, cache reads/state; inclusive 40m, diagonal/negative/finite overflow, camera-independent near admission, malformed/cold/stale cache, tracked retired/reused identities, stream-out, wrap, capacity, collision. Timer no-rebase skip counterexample confirmed; timer unchanged.\n";
}
