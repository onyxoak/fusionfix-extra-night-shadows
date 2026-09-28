#include "source/NativeLampDistance38.hpp"
#include "NativeLampSelection35.hpp"
#include <cassert>
#include <iostream>
using namespace fusionfix::shadows;
int main() {
    ShadowView v{}; v.valid=true;
    for(int i=0;i<4;++i)v.view[i][i]=1;
    v.projection[0][0]=v.projection[1][1]=1;
    v.projection[2][2]=1.001f;v.projection[2][3]=1;v.projection[3][2]=-.1f;
    // Screen center, edges and corners receive the same relevance.
    for(float x:{-10.f,-9.f,0.f,9.f,10.f})
        for(float y:{-10.f,0.f,10.f}) assert(ViewSample(v,{x,y,10})==1);
    assert(ViewSample(v,{0,0,-10})==0);
    assert(ViewSample(v,{14,0,10})==0);
    float last=1;
    for(int i=0;i<=350;++i) {
        float value=ViewSample(v,{10+i*.01f,0,10});
        assert(value<=last+.00001f && last-value<.005f);last=value;
    }
    // Model the last contested native slot: near-equal competitors oscillate
    // around its distance. Retention should avoid alternation, but a materially
    // closer newcomer must still win. No fixed time delay or extra slots.
    unsigned active=1,swaps=0;
    for(unsigned f=0;f<1000;++f) {
        float a=NativeLampPriorityDistance(20,400,3,60,active==1);
        float b=NativeLampPriorityDistance(f%2?19.f:21.f,400,3,60,active==2);
        unsigned next=a<=b?1:2;
        swaps+=next!=active;active=next;
    }
    assert(swaps==0);
    assert(NativeLampPriorityDistance(12,400,3,60,false)<NativeLampPriorityDistance(20,400,3,60,true));
    // No retention outside view/range, no invalid-distance changes.
    assert(NativeLampPriorityDistance(20,400,1,60,true)==20);
    assert(NativeLampPriorityDistance(20,3600,3,60,true)==20);
    assert(NativeLampPriorityDistance(-1,400,3,60,true)==-1);
    assert(NativeLampPriorityDistance(5,25,3,60,true)<5);
    assert(NativeLampPriorityDistance(5,25,3,60,false)==5);
    for(float boundary:{8.f,16.f,54.f,60.f}) {
        float a=boundary-.001f,b=boundary+.001f;
        assert(std::abs(NativeLampPriorityDistance(30,a*a,3,60,true)-NativeLampPriorityDistance(30,b*b,3,60,true))<.01f);
    }
    std::cout<<"PASS full-frame priority, smooth offscreen boundary, contested-slot stability, meaningful replacement, range/invalid safety\n";
}
