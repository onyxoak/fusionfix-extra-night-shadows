#include "D:/GTA-IV-Collection-Lab/view-priority-candidate26/fusionfix-source/source/NativeLampDistance38.hpp"
#include "D:/GTA-IV-Collection-Lab/view-priority-candidate26/fusionfix-source/source/NearbyVehicleReceivers36.hpp"
#include <cassert>
#include <iostream>
#include <limits>
using namespace fusionfix::shadows;
int main() {
    // Lamp changes are bounded ranking preference, not a new physical radius.
    assert(NativeLampPriorityDistance(20,100,1,60)==20);
    assert(NativeLampPriorityDistance(5,25,3,60)==5);
    assert(NativeLampPriorityDistance(70,4900,3,60)==70);
    assert(NativeLampPriorityDistance(20,400,3,60)<20);
    assert(NativeLampPriorityDistance(20,400,3,60)<NativeLampPriorityDistance(20,400,2,60));
    float last=NativeLampPriorityDistance(30,400,1,60);
    for(unsigned i=1;i<=2000;++i) {
        const float now=NativeLampPriorityDistance(30,400,1+float(i)/1000,60);
        assert(now<=last+0.0001f);assert(last-now<0.02f);assert(now>=30/1.35001f);last=now;
    }
    for(float boundary:{8.0f,16.0f,54.0f,60.0f}) {
        const float a=boundary-0.001f,b=boundary+0.001f;
        assert(std::abs(NativeLampPriorityDistance(30,a*a,3,60)-NativeLampPriorityDistance(30,b*b,3,60))<0.01f);
    }
    assert(NativeLampPriorityDistance(20,-1,3,60)==20);
    assert(NativeLampPriorityDistance(20,400,std::numeric_limits<float>::quiet_NaN(),60)==20);
    // New headlight score chooses a visibly useful receiver over a slightly
    // nearer competing source, keeping existing own-car priority and budget.
    StableHeadlightSelector selector;
    for(unsigned frame=1;frame<=6;++frame) {
        selector.BeginFrame({frame,frame*16,123,false,0});
        selector.Consider({1,{100,true,true},false,400,true,1.0f});
        selector.Consider({2,{110,true,true},false,400,true,1.0f});
        selector.Consider({3,{120,true,true},false,400,true,1.5f});
    }
    auto ids=selector.ActiveIdentities();
    assert((ids[0]==3||ids[1]==3));assert(ids[0]!=ids[1]);
    // A brief visibility fluctuation cannot defeat the existing minimum hold.
    for(unsigned frame=7;frame<=12;++frame) {
        selector.BeginFrame({frame,frame*16,123,false,0});
        selector.Consider({1,{100,true,true},false,400,true,1.5f});
        selector.Consider({2,{110,true,true},false,400,true,1.5f});
        selector.Consider({3,{120,true,true},false,400,true,1.0f});
        auto current=selector.ActiveIdentities();assert(current[0]==ids[0]&&current[1]==ids[1]);
    }
    auto bad=HeadlightCandidate{9,{1,true,true},false,400,true,2.0f};assert(!selector.Consider(bad));
    // Canonical source header must contain the intended80-foot discovery limit.
    static_assert(NearbyVehicleReceivers36::Reach>24.38f&&NearbyVehicleReceivers36::Reach<24.39f);
    NearbyVehicleReceivers36 receivers{};receivers.origin={2,-2,0};
    receivers.Add({2,20.86f,0},20);assert(receivers.count==1);
    receivers.Add({2,24,0},21);assert(receivers.count==1);
    std::cout<<"PASS coherent native lamp ranking bounds/continuity, on-foot receiver priorities, retained headlight choices and80-foot canonical receiver header\n";
}
