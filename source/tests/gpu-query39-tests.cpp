#include "GpuProfiling39.hpp"
#include "GpuResetGuard39.hpp"
#include "GpuResetGate39.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include <vector>
#include <algorithm>
struct Device {int references=0,calls=0;};
struct Query {int type=0,index=0;uint64_t value=0;Device* owner=nullptr;};
struct Fake {
 using Device=::Device*;using Query=::Query*;
 static inline int created=0,released=0,failCreate=-1,failIssue=-1,issues=0,reads=0,support=-1;
 static inline bool pending=false,disjoint=false,reverse=false,readFail=false;
 static inline uint64_t clock=0;
 static void Reset(){assert(created==released);created=released=issues=reads=0;failCreate=failIssue=support=-1;pending=disjoint=reverse=readFail=false;clock=0;}
 bool Supports(Device d,int type){++d->calls;return support!=type;}
 bool Create(Device d,int type,Query& q){++d->calls;if(created==failCreate)return false;q=new ::Query{type,created++,0,d};++d->references;return true;}
 void Release(Query& q){if(q){++released;--q->owner->references;delete q;q=nullptr;}}
 bool Issue(Query q,unsigned flags){assert(flags==1||flags==2);if(issues++==failIssue)return false;if(q->type==10)q->value=(clock+=100);return true;}
 int Get(Query q,void* output,unsigned size,unsigned flags){assert(flags==0);++reads;if(readFail)return -1;if(pending)return 1;
  if(q->type==11){assert(size==4);*static_cast<int32_t*>(output)=disjoint;}
  else {assert(size==8);*static_cast<uint64_t*>(output)=q->type==12?1000:reverse?1000-q->value:q->value;}return 0;}
};
int main(){
 Device device{};using Ring=gpu39::Ring<Fake,2>;
 {Fake::Reset();Fake::support=11;Ring ring;assert(!ring.Bind(&device));assert(device.references==0&&Fake::created==0);}
 {Fake::Reset();Fake::failCreate=3;Ring ring;assert(!ring.Bind(&device));assert(device.references==0&&Fake::created==Fake::released);}
 {Fake::Reset();Ring ring;assert(ring.Bind(&device));assert(device.references==8);Fake::pending=true;
  auto a=ring.Begin({1,1});assert(a>=0&&ring.End(a));auto b=ring.Begin({2,1});assert(b>=0&&ring.End(b));assert(ring.Begin({3,1})==-1&&ring.stats.full==1);
  int emitted=0;ring.Poll(3,[&](auto){++emitted;});assert(Fake::reads==0);ring.Poll(6,[&](auto){++emitted;});assert(emitted==0&&ring.Bound());
  Fake::pending=false;ring.Poll(7,[&](auto result){++emitted;assert(!result.disjoint&&result.end>result.start&&result.frequency==1000);});assert(emitted==2);
  assert(ring.Begin({8,1})>=0);ring.Stop();assert(device.references==0&&Fake::created==Fake::released); // release BEFORE original Reset
 }
 {Fake::Reset();Ring ring;assert(ring.Bind(&device));Fake::disjoint=true;auto i=ring.Begin({1,2});assert(ring.End(i));int bad=0;ring.Poll(5,[&](auto r){bad+=r.disjoint;});assert(bad==1&&ring.stats.invalid==1);ring.Stop();}
 {Fake::Reset();Ring ring;assert(ring.Bind(&device));Fake::reverse=true;auto i=ring.Begin({1,2});assert(ring.End(i));int bad=0;ring.Poll(5,[&](auto r){bad+=r.disjoint;});assert(bad==1);ring.Stop();}
 {Fake::Reset();Ring ring;assert(ring.Bind(&device));Fake::failIssue=0;assert(ring.Begin({1,1})==-1);assert(!ring.Bound()&&device.references==0&&ring.stats.failures==1);}
 {Fake::Reset();Ring ring;assert(ring.Bind(&device));auto i=ring.Begin({1,1});assert(ring.End(i));Fake::readFail=true;ring.Poll(5,[](auto){assert(false);});assert(!ring.Bound()&&Fake::created==Fake::released&&device.references==0);}
 {Fake::Reset();Ring ring;assert(ring.Bind(&device));auto i=ring.Begin({1,1});assert(ring.End(i));ring.Stop();assert(device.references==0);assert(ring.Bind(&device));i=ring.Begin({2,2});assert(ring.End(i));int generation=0;ring.Poll(6,[&](auto r){generation=r.tag.deviceGeneration;});assert(generation==2);ring.Stop();}
 // The native facade forwards AddRef/Release through a mutable inner field.
 // Query interfaces own their original device; wrapper is merely an identity.
 {Fake::Reset();Device oldProxy{},newProxy{};Device* mutableFacade=&oldProxy;Ring ring;
  assert(ring.Bind(mutableFacade));auto i=ring.Begin({1,3,88,123});assert(ring.End(i));
  mutableFacade=&newProxy;assert(ring.BoundDevice()!=mutableFacade);const auto oldCalls=oldProxy.calls;
  ring.Stop();assert(oldProxy.calls==oldCalls&&oldProxy.references==0&&newProxy.references==0&&Fake::created==Fake::released);
  assert(ring.Bind(mutableFacade));assert(newProxy.references==8&&oldProxy.references==0);
  i=ring.Begin({2,4,89,124});assert(ring.End(i));int emitted=0;
  ring.Poll(6,[&](auto result){++emitted;assert(result.tag.deviceGeneration==4&&result.tag.nativeFrame==89&&result.tag.beginTick==124);});
  assert(emitted==1);ring.Stop();assert(oldProxy.references==0&&newProxy.references==0&&Fake::created==Fake::released);
 }
 // Identify ScriptHook non-owning reference wrapper; do not use it for retention.
 std::vector<uint8_t> referenceCode{0x8b,0x44,0x24,4,0x8b,0x40,4,0x8b,8,0x89,0x44,0x24,4,0x8b,0x51,4,0xff,0xe2};
 assert(gpu39::NonOwningReferenceForwarder(referenceCode,4));
 assert(!gpu39::NonOwningReferenceForwarder(referenceCode,8));referenceCode[15]=8;
 assert(gpu39::NonOwningReferenceForwarder(referenceCode,8));referenceCode[6]=8;
 assert(!gpu39::NonOwningReferenceForwarder(referenceCode,8));
 // Exact native forwarder guards reject altered proxy offsets and calling ABI.
 std::vector<uint8_t> code{0x8b,0x44,0x24,0x04,0x8b,0x80,0xac,0x11,0,0,0x8b,8,0x89,0x44,0x24,4,0xff,0xa1,0xa4,0,0,0};
 assert(gpu39::Forwarder(code,0xa4));assert(!gpu39::Forwarder(code,0xa8));code[8]=1;assert(!gpu39::Forwarder(code,0xa4));code[8]=0;code[3]=8;assert(!gpu39::Forwarder(code,0xa4));
 assert(gpu39::Forwarder(std::span(gpu39::ResetBytes).subspan(190),0x40));
 for(auto offset:gpu39::ResetRelocs)assert(offset+4<=sizeof(gpu39::ResetBytes));
 assert(Fake::created==Fake::released&&device.references==0);
 // Reset gate covers original Reset AFTER bookkeeping lock is released.
 {gpu39::ResetGate gate;std::mutex mutex;std::condition_variable condition;bool originalEntered=false,releaseOriginal=false;std::atomic<int> calls{0};
  std::thread reset([&]{ {std::lock_guard lock(mutex);gate.Begin();originalEntered=true;}condition.notify_one();
   {std::unique_lock lock(mutex);condition.wait(lock,[&]{return releaseOriginal;});}gate.End(); });
  {std::unique_lock lock(mutex);condition.wait(lock,[&]{return originalEntered;});}
  for(int i=0;i<100;++i){std::lock_guard lock(mutex);if(!gate.Blocked())++calls;}
  assert(calls==0);{std::lock_guard lock(mutex);releaseOriginal=true;}condition.notify_one();reset.join();assert(!gate.Blocked());
  gate.Begin();gate.Begin();gate.End();assert(gate.Blocked());gate.End();assert(!gate.Blocked());
 }
}
