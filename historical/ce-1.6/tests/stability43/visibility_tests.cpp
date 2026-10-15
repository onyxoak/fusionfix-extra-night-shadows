#include "source/ShadowVolumeVisibility43.hpp"
#include "NativeShadowContinuity42.hpp"
#include <cassert>
#include <iostream>
using namespace fusionfix::shadows;
ShadowView View(){
    ShadowView v{};v.valid=true;
    for(int i=0;i<4;++i)v.view[i][i]=1;
    v.projection[0][0]=1;v.projection[1][1]=1;
    v.projection[2][2]=-100.0f/99.95f;v.projection[2][3]=-1;
    v.projection[3][2]=-5.0f/99.95f;return v;
}
int main(){
    auto v=View();
    // Niko/car in front can be illuminated by a source behind the camera.
    // None of the old half-radius samples enter view, yet the real volume does.
    Vec3 behind{0,0,5};assert(ShadowViewWeight(v,behind,{0,0,-1},10,false)==1);
    assert(ShadowVolumeMayReachView(v,behind,10));
    Vec3 side{11.0f,0,-5};assert(ShadowViewWeight(v,side,{0,0,-1},5,false)==1);
    assert(ShadowVolumeMayReachView(v,side,5));
    assert(ShadowVolumeMayReachView(v,{0,0,-5},1));
    assert(!ShadowVolumeMayReachView(v,{30,0,-5},2));
    assert(!ShadowVolumeMayReachView(v,{0,0,10},2));
    assert(!ShadowVolumeMayReachView(v,{0,0,-110},2));
    // Whole-frame and near-plane continuity, including objects crossing edges.
    for(int n=0;n<=1000;++n){float x=-5+10*n/1000.0f;assert(ShadowVolumeMayReachView(v,{x,0,-5},1));}
    assert(ShadowVolumeMayReachView(v,{0,0,0},1));
    auto unknown=v;unknown.valid=false;assert(ShadowVolumeMayReachView(unknown,side,5));
    assert(!ShadowVolumeMayReachView(v,side,-1));
    assert(!ShadowVolumeMayReachView(v,{NAN,0,0},2));
    // The former source-sample cull released this incumbent to a passing beam.
    // Receiver-volume retention prevents that exact release without a new slot.
    NativeShadowContinuity42 p;p.Begin(1,1,16);
    std::array<NativeShadowContinuity42::Identity,7> chosen{};chosen[0]={100,10};assert(p.Commit(chosen));
    auto incoming=p.Observe({200,0},0x164,true,false);
    auto old=p.Observe({100,10},0x67,ShadowViewWeight(v,behind,{0,0,-1},10,false)>1.1f,false);
    auto fixed=p.Observe({100,10},0x67,ShadowVolumeMayReachView(v,behind,10),false);
    assert(NativeShadowContinuity42::Compare(0,incoming,old)==0);
    assert(NativeShadowContinuity42::Compare(0,incoming,fixed)==2);
    std::cout<<"PASS off-camera source / visible receiver regression, full-frustum edges, near/far planes, invalid input and continuity integration\n";
}

