#include "ShadowReceiver.hpp"
#include "PlayerShadowBudget.hpp"
#include <cassert>
#include <limits>
#include <iostream>
using namespace fusionfix::shadows;
using namespace fusionfix::shadows::budget;
int main() {
    Vec3 source{0,0,0}, direction{0,1,0};
    // Walk continuously through a lit beam, across the old 10-foot threshold.
    for(int i=1;i<=300;++i) {
        float distance=i*0.05f;
        assert(BeamTouchesReceiver({0,distance,0},.75f,source,direction,.85f,20));
        assert(ReceiverDistanceSquared({0,distance,0},source,.75f)<=distance*distance);
    }
    assert(!BeamTouchesReceiver({0,22,0},.75f,source,direction,.85f,20));
    assert(!BeamTouchesReceiver({0,-5,0},.75f,source,direction,.85f,20));
    assert(!BeamTouchesReceiver({10,2,0},.75f,source,direction,.85f,20));
    assert(BeamTouchesReceiver({3,3,0},3,source,direction,.85f,20));
    assert(!BeamTouchesReceiver({3,3,0},.75f,source,direction,.85f,20));
    assert(!BeamTouchesReceiver({0,3,0},.75f,source,{},.85f,20));
    assert(!BeamTouchesReceiver({0,3,0},.75f,source,direction,.85f,std::numeric_limits<float>::quiet_NaN()));
    // A car centre well ahead of another bumper remains eligible, while
    // seven slots and two headlight allocations remain the hard budget.
    PlayerShadowBudget p;
    p.BeginPass({1,16,1,true});
    p.Add({1,0,Kind::PlayerBeam,100,false,true,0,1,15.24f*15.24f});
    p.Add({2,1,Kind::OtherBeam,ReceiverDistanceSquared({0,10,0},source,3),true,true,0,1,15.24f*15.24f});
    for(unsigned i=3;i<40;++i) p.Add({i,i-1,Kind::Lamp,float(i*20),i<5,true,0,3,60.96f*60.96f});
    auto result=p.Finalize();
    assert(result.safeToApply && result.count==7);
    unsigned beams=0;bool following=false;
    for(auto slot:result.slots) {beams+=slot.kind!=Kind::Lamp;following|=slot.key==2;}
    assert(beams==2 && following);
    // A far centred light must not beat every nearby source simply because
    // the old distance-compression expression saturated near a constant.
    p.Reset();p.BeginPass({1,16,1,false});
    for(unsigned i=1;i<=7;++i)p.Add({i,i-1,Kind::Lamp,100,false,true,0,1,4000});
    p.Add({99,7,Kind::Lamp,3600,false,true,0,3,4000});
    for(auto slot:p.Finalize().slots)assert(slot.key!=99);
    std::cout << "Receiver coverage, range continuity, vehicle footprint, invalid geometry, slot budget and bounded camera scoring passed\n";
}
