#include "ShadowViewPriority.hpp"
#include "PlayerShadowBudget.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
using namespace fusionfix::shadows;
using namespace fusionfix::shadows::budget;
ShadowView Camera() {
    ShadowView c{};c.valid=true;
    for(int i=0;i<4;++i)c.view[i][i]=1;
    c.projection[0][0]=c.projection[1][1]=1;
    c.projection[2][2]=1.001f;c.projection[2][3]=1;c.projection[3][2]=-0.1f;
    return c;
}
int main() {
    // A shadow/reflection viewport must not pass merely because the temporary
    // render-window dimensions changed to the same size.
    assert(IsGameplayViewport(true,2560,1440,2560,1440));
    assert(!IsGameplayViewport(true,1280,512,2560,1440));
    assert(!IsGameplayViewport(true,1024,1024,2560,1440));
    assert(!IsGameplayViewport(false,2560,1440,2560,1440));
    assert(!IsGameplayViewport(true,0,0,0,0));
    assert(IsGameplayViewport(true,1920,1080,1920,1080));
    auto c=Camera();
    const auto center=ViewSample(c,{0,0,10});
    const auto edge=ViewSample(c,{10,0,10});
    assert(center>0.99f && edge>0.80f && edge<center);
    assert(ViewSample(c,{11,0,10})>0.5f); // enter before visible edge
    assert(ViewSample(c,{14,0,10})==0); // no unbounded offscreen preference
    float previous=1;
    for(int i=0;i<=15000;++i) {
        const float x=i*0.001f;
        const float value=ViewSample(c,{x,0,10});
        assert(value>=0&&value<=1&&value<=previous+0.000001f);
        assert(previous-value<0.001f); // continuous across shoulder/boundary
        assert(std::abs(value-ViewSample(c,{-x,0,10}))<0.000001f);
        assert(std::abs(value-ViewSample(c,{0,x,10}))<0.000001f);
        previous=value;
    }
    // Camera jitter must not repeatedly exchange equal-distance visible lamps.
    PlayerShadowBudget budget;
    PlayerShadowBudget::Selection prior{};
    unsigned swaps=0;
    for(unsigned frame=1;frame<=600;++frame) {
        budget.BeginPass({frame,frame*16,100,false});
        for(unsigned id=1;id<=10;++id) {
            const float x=(float(id)-5.5f)*2.0f+std::sin(frame*0.17f)*0.12f;
            const float weight=1+2*ViewSample(c,{x,0,10});
            budget.Add({id,id-1,Kind::Lamp,100,false,true,0,weight,400});
        }
        const auto selected=budget.Finalize();
        assert(selected.safeToApply&&selected.count==7);
        if(frame>1)for(unsigned slot=0;slot<7;++slot)
            swaps+=selected.slots[slot].key!=prior.slots[slot].key;
        prior=selected;
    }
    assert(swaps==0);
    std::cout<<"View29: display viewport filtering, broad visibility, continuous edges, and 600-frame jitter retention passed\n";
}
