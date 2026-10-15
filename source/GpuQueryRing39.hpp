#pragma once
#include <array>
#include <cstdint>
#include <cstddef>
namespace gpu39 {
struct Tag { uint64_t interval=0; uint32_t deviceGeneration=0,nativeFrame=0,beginTick=0; uint32_t stageKind=0,mapSlot=0,callbackOrdinal=0; };
struct Result { Tag tag{}; uint64_t start=0,end=0,frequency=0; bool disjoint=false; };
// Own returned query interfaces, never a mutable device wrapper. Backend uses
// the borrowed device only during synchronous Bind/Create; later calls use queries.
// Caller confines Issue/GetData/Create to the graphics submission thread.
template<class Backend,size_t Count=16> class Ring {
public:
 using Device=typename Backend::Device; using Query=typename Backend::Query;
 struct Stats { uint64_t issued=0,completed=0,pending=0,full=0,invalid=0,failures=0; } stats{};
private:
 struct Slot { Query begin{},end{},frequency{},disjoint{}; Tag tag{}; bool open=false,pending=false; };
 Backend backend{}; Device device{};std::array<Slot,Count> slots{};size_t cursor=0,pollCursor=0;
 void Clear(Slot& s){backend.Release(s.begin);backend.Release(s.end);backend.Release(s.frequency);backend.Release(s.disjoint);s={};}
 bool Create(Slot& s){return backend.Create(device,10,s.begin)&&backend.Create(device,10,s.end)&&backend.Create(device,12,s.frequency)&&backend.Create(device,11,s.disjoint);}
public:
 Ring()=default;Ring(const Ring&)=delete;Ring& operator=(const Ring&)=delete;
 // Explicit Stop, rather than destructor GPU calls on an arbitrary DLL teardown thread.
 bool Bound()const{return bool(device);} Device BoundDevice()const{return device;}
 void Stop(){for(auto& s:slots)Clear(s);device={};cursor=pollCursor=0;}
 bool Bind(Device d){Stop();if(!d)return false;
  device=d; // Borrow only during this Bind; thereafter device is an identity token.
  for(int type:{10,12,11})if(!backend.Supports(device,type)){++stats.failures;Stop();return false;}
  for(auto& s:slots)if(!Create(s)){++stats.failures;Stop();return false;}return true;
 }
 int Begin(Tag tag){if(!device)return -1;for(size_t n=0;n<Count;++n){auto i=(cursor+n)%Count;auto& s=slots[i];if(s.open||s.pending)continue;
  if(!backend.Issue(s.frequency,1)||!backend.Issue(s.disjoint,2)||!backend.Issue(s.begin,1)){++stats.failures;Stop();return -1;}
  s.tag=tag;s.open=true;cursor=(i+1)%Count;++stats.issued;return int(i);
 }++stats.full;return -1;}
 bool End(int i){if(i<0||size_t(i)>=Count||!device)return false;auto& s=slots[size_t(i)];if(!s.open)return false;
  if(!backend.Issue(s.end,1)||!backend.Issue(s.disjoint,1)){++stats.failures;Stop();return false;}s.open=false;s.pending=true;return true;}
 template<class Emit> void Poll(uint64_t interval,Emit emit){if(!device)return;
  // At most four slots (16 GetData calls) per frame. No spin, flush or overwrite.
  for(size_t n=0;n<Count&&n<4;++n){auto& s=slots[pollCursor];pollCursor=(pollCursor+1)%Count;if(!s.pending||interval<s.tag.interval||interval-s.tag.interval<3)continue;
   uint64_t b=0,e=0,f=0;int32_t dj=0;int rb=backend.Get(s.begin,&b,sizeof(b),0),re=backend.Get(s.end,&e,sizeof(e),0),rf=backend.Get(s.frequency,&f,sizeof(f),0),rd=backend.Get(s.disjoint,&dj,sizeof(dj),0);
   if(rb<0||re<0||rf<0||rd<0){++stats.failures;Stop();return;}
   if(rb||re||rf||rd){++stats.pending;continue;}s.pending=false;++stats.completed;
   bool invalid=dj||!f||e<b;if(invalid)++stats.invalid;emit(Result{s.tag,b,e,f,invalid});
  }
 }
};
}
