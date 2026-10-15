#include "core/ShadowTransitionDetector69.hpp"
#include <cassert>
#include <vector>
#include <algorithm>
int main(){
 using namespace shadow_transition69;
 Detector detector;std::vector<Event> events;
 auto emit=[&](Event event){events.push_back(event);};
 auto present=[](Identity){return true;};
 std::array<Identity,7> a{};a[0]={10,1};a[1]={20,1};
 detector.Observe(1,2,10,100,a,present,emit);
 auto b=a;std::swap(b[0],b[1]);detector.Observe(1,2,11,110,b,present,emit);assert(events.empty());
 b[1]={30,1};detector.Observe(1,2,12,120,b,present,emit);
 assert(events.size()==1&&events.back().kind==Kind::PresentSourceDropped&&events.back().source==a[0]);
 detector.Observe(1,2,12,121,a,present,emit);assert(events.size()==1);
 detector.Observe(1,2,13,140,a,present,emit);
 assert(events.size()==3&&events.back().kind==Kind::RapidReturn&&events.back().elapsed==20);
 // A changed generation must not be called the same returning shadow.
 auto c=a;c[0]={10,2};detector.Observe(1,2,14,150,c,present,emit);
 assert(events.back().kind==Kind::PresentSourceDropped&&events.back().source.generation==1);
 const auto before=events.size();
 detector.Observe(1,3,15,160,b,present,emit);assert(events.size()==before);
 detector.Observe(1,3,16,5000,a,present,emit);assert(events.size()==before);
 detector.Observe(1,3,1,5010,b,present,emit);assert(events.size()==before);
 detector.Observe(0,3,2,5020,a,present,emit);assert(events.size()==before);
 detector.Observe(1,3,3,5030,a,present,emit);
 detector.Observe(1,3,4,5040,b,[](Identity){return false;},emit);
 detector.Observe(1,3,5,5050,a,[](Identity){return false;},emit);assert(events.size()==before);
 detector.Reset();events.clear();
 detector.Observe(1,2,100,0xfffffff0u,a,present,emit);
 detector.Observe(1,2,101,5,b,present,emit);
 detector.Observe(1,2,102,20,a,present,emit);assert(events.back().kind==Kind::RapidReturn&&events.back().elapsed==15);
 Ring<2> queue;Event event{};assert(queue.Push(events[0])&&queue.Push(events[1]));
 assert(!queue.Push(events[2])&&queue.dropped.load()==1);
 assert(queue.Pop(event)&&event.kind==events[0].kind);
 assert(queue.Pop(event)&&event.source==events[1].source);assert(!queue.Pop(event));
}
