#include "GpuStageSchedule48.hpp"
#include <cassert>
#include <cstdio>
int main(){
 gpu48::StageSchedule stage;
 assert(!stage.Begin().registered);
 stage.BeginScene(16,1);
 auto outer=stage.Begin();assert(outer.sample&&outer.ordinal==1);
 auto nested=stage.Begin();assert(nested.registered&&!nested.sample&&stage.nested==1);
 assert(!stage.End(nested));assert(stage.Open());assert(stage.End(outer));assert(!stage.Open());
 for(unsigned i=2;i<=7;++i){auto t=stage.Begin();assert(t.sample&&t.ordinal==i);assert(stage.End(t));}
 auto eighth=stage.Begin();assert(!eighth.sample&&eighth.ordinal==8&&stage.budgetSkipped==1);assert(!stage.End(eighth));
 auto old=stage.Begin();stage.Reset();assert(!stage.End(old));assert(!stage.Begin().registered);
 stage.BeginScene(17,1);auto unsampled=stage.Begin();assert(!unsampled.sample);assert(!stage.End(unsampled));
 stage.BeginScene(32,2);auto oldEpoch=stage.Begin();assert(oldEpoch.sample);
 stage.BeginScene(32,2);auto current=stage.Begin();assert(current.sample);assert(!stage.End(oldEpoch));assert(stage.Open());assert(stage.End(current));
 stage.BeginScene(48,3);auto reset=stage.Begin();stage.Reset();stage.BeginScene(64,4);auto rebound=stage.Begin();assert(!stage.End(reset));assert(stage.Open());assert(stage.End(rebound));
 std::puts("gpu-stage48 schedule: nested/budget/unsampled/reset/same-frame epoch checks passed");
}
