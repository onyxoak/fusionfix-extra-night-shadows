#include "D:/GTA-IV-Collection-Lab/view-priority-candidate26/fusionfix-source/source/NearbyVehicleReceivers36.hpp"
#include <cassert>
#include <iostream>
#include <random>
#include <vector>
using namespace fusionfix::shadows;
static ShadowView View() {
    ShadowView v{};v.valid=true;
    v.view[0][0]=1;v.view[2][1]=1;v.view[1][2]=1;v.view[3][3]=1;
    v.projection[0][0]=1;v.projection[1][1]=1;v.projection[2][2]=0.5f;v.projection[2][3]=1;
    return v;
}
static NearbyVehicleReceivers36 Scene() {
    NearbyVehicleReceivers36 s{};s.session=123;s.frame=10;s.timeMs=1000;s.origin={2,-2,0};s.view=View();return s;
}
static bool Match(const NearbyVehicleReceivers36& s,Vec3 source={},Vec3 direction={0,1,0},float radius=20,uintptr_t key=100) {
    return s.Touches(123,10,1000,{2,-2,0},source,direction,0.70710678f,radius,key);
}
int main() {
    auto s=Scene();s.Add({0,8,0},200);
    // Door position is outside Niko's body/beam test; the car ahead is lit.
    assert(!BeamTouchesReceiver({2,-2,0},1.5f,{0,0,0},{0,1,0},0.70710678f,20));
    assert(Match(s));
    // Hood position and side position now both qualify, independent of Niko
    // stepping across the cone boundary. Run the real headlight selector too.
    StableHeadlightSelector selector;
    for(unsigned i=1;i<=8;++i) {
        selector.BeginFrame({i,i*16,123,false,0});
        auto g=EvaluateGeometry({2,-2,0},{0,0,0});g.directionKnown=true;g.aimedAtPlayer=Match(s);
        const bool selected=selector.Consider({100,g,false,400,true});
        if(i>=2)assert(selected);
    }
    auto own=Scene();own.Add({0,1,0},100);assert(!Match(own));assert(!Match(own,{}, {0,1,0},20,101));
    auto empty=Scene();assert(!Match(empty));
    auto behind=Scene();behind.Add({0,-10,0},200);assert(!Match(behind));
    auto side=Scene();side.Add({12,2,0},200);assert(!Match(side));
    auto offscreen=Scene();offscreen.Add({16,1,0},200);assert(!Match(offscreen,{}, {16,1,0}));
    auto beyond=Scene();beyond.Add({0,50,0},200);assert(beyond.count==0);assert(!Match(beyond));
    auto shortBeam=Scene();shortBeam.Add({0,12,0},200);assert(!Match(shortBeam,{}, {0,1,0},5));
    auto partial=Scene();partial.Add({8,8,0},200);assert(Match(partial));
    assert(!Match(s,{},{}));assert(!Match(s,{}, {0,1,0},-1));
    assert(!s.Touches(999,10,1000,{2,-2,0},{},{0,1,0},0.70710678f,20,100));
    assert(!s.Touches(123,13,1000,{2,-2,0},{},{0,1,0},0.70710678f,20,100));
    assert(!s.Touches(123,10,1101,{2,-2,0},{},{0,1,0},0.70710678f,20,100));
    assert(!s.Touches(123,10,1000,{100,100,0},{},{0,1,0},0.70710678f,20,100));
    auto missingCamera=s;missingCamera.view.valid=false;assert(!Match(missingCamera));
    // Pool iteration order cannot alter the bounded nearest receiver set.
    std::vector<unsigned> ids;for(unsigned i=1;i<=80;++i)ids.push_back(i);
    auto baseline=Scene();for(auto i:ids)baseline.Add({float(i%10),float(i%8),0},1000+i);
    assert(baseline.count==32);
    std::mt19937 rng(3601);
    for(unsigned iteration=0;iteration<1000;++iteration) {
        std::shuffle(ids.begin(),ids.end(),rng);auto current=Scene();
        for(auto i:ids)current.Add({float(i%10),float(i%8),0},1000+i);
        assert(current.count==32);
        for(unsigned i=0;i<32;++i)assert(current.receivers[i].identity==baseline.receivers[i].identity);
    }
    std::cout<<"PASS door-to-hood receiver regression, real headlight admission, source-car exclusion, frustum/range gates, stale snapshots and 1000 shuffled bounded receiver sets\n";
}
