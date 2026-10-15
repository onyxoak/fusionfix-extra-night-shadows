#pragma once
#include "ConsumerShadow75.hpp"
#include "ConsumerReach136.hpp"
#include <algorithm>

namespace scenery_fade198 {
inline float (*indoorStrength212)(const consumer_shadow75::Packet&,uint32_t) noexcept=nullptr;
struct Registers {uintptr_t esp{},ebp{},esi{};};
struct Plan {uintptr_t address{};consumer_shadow75::Packet packet{};std::array<float,4> parameters{};float marker{},strength{};};
inline float Strength(float radius,float distance,float threshold) noexcept {
 if(!std::isfinite(radius)||radius<=0||!std::isfinite(distance)||distance<=0||!std::isfinite(threshold)||threshold<=0)return -1;
 const float scaled=distance*threshold/radius;
 if(!std::isfinite(scaled)||scaled<1.10f)return -1;
 // Stay fully unshadowed beyond the fade boundary if another authenticated
 // application policy briefly holds this same map farther out (e.g.198A).
 const float t=std::clamp((1.30f-scaled)/.20f,0.f,1.f);
 return t*t*(3.f-2.f*t);
}
// Decode only the already looked-up map's native binder invocation. No native
// function is called here; the original binder submits its own local vector.
template<class Read> bool Decode(const consumer_shadow75::Context& c,Registers r,
 uintptr_t game,uint32_t frame,Read&& read,Plan& out) noexcept {
 if(!c.pending||c.frame!=frame||c.sample.lookup<0||c.sample.lookup>=16||
    r.ebp<0x10000||r.ebp>UINTPTR_MAX-12||r.ebp+12!=c.anchor||
    r.esp<0x10000||r.esp>UINTPTR_MAX-0x38||r.esi<0x10000||
    !consumer_reach136::PacketPotential142(c.packet))return false;
 uint32_t caller{},index{},vector{},source{},camera{},bank{};
 if(!read(r.ebp+4,&caller,4)||caller!=game+0x6c124f||
    !read(r.ebp+8,&index,4)||index!=uint32_t(c.sample.lookup)||
    !read(r.esp+8,&vector,4)||vector!=r.esp+0x28||
    !read(c.anchor+0x1c,&source,4)||source!=c.source+0x28||
    !read(game+0xd74794,&bank,4)||bank>1)return false;
 const uintptr_t header=index<8?game+0xd9fa00+(index+bank*8)*0x110:
    game+0xd9d0f0+(index-8+bank*16)*0x100;
 if(r.esi!=header)return false;
 Plan p{};p.address=vector;
 if(!read(c.source,&p.packet,sizeof(p.packet))||std::memcmp(&p.packet,&c.packet,sizeof(p.packet))||
    !read(vector,p.parameters.data(),16)||p.parameters[0]!=0.f)return false;
 // Authenticate all four incoming lanes against this exact native map header.
 float nativeX{},nativeZ{},nativeW{};uint32_t nativeY{};
 if(!read(header+0x28,&nativeX,4)||!read(header+8,&nativeY,4)||
    !read(header+0x10,&nativeZ,4)||!read(header+0x14,&nativeW,4)||
    p.parameters[0]!=nativeX||p.parameters[1]!=float(nativeY)||
    p.parameters[2]!=nativeZ||p.parameters[3]!=nativeW)return false;
 float radius{},scale{},global{};std::array<float,3> position{},eye{};
 if(!read(c.anchor+0x28,&camera,4)||camera<0x10000||camera>UINT32_MAX-0x2d0||
    !read(c.anchor+0x14,&radius,4)||!read(c.anchor+0x40,position.data(),12)||
    !read(uintptr_t(camera)+0x70,eye.data(),12)||!read(uintptr_t(camera)+0x2cc,&scale,4)||
    !read(game+0xc3efdc,&global,4))return false;
 const float dx=eye[0]-position[0],dy=eye[1]-position[1],dz=eye[2]-position[2];
 const float distance=std::sqrt((dy*dy+dx*dx)+dz*dz);
 p.strength=Strength(radius,distance,scale*global);
 if(indoorStrength212){const float strength=indoorStrength212(p.packet,frame);
  if(std::isfinite(strength)&&strength>=0&&strength<=1)p.strength=std::fmax(p.strength,strength);}
 if(p.strength<0)return false;
 p.marker=-198.f-p.strength;out=p;return true;
}
template<class Read,class Authorize,class Write> bool Apply(const consumer_shadow75::Context& c,Registers r,
 uintptr_t game,uint32_t frame,Read&& read,Authorize&& authorize,Write&& write) noexcept {
 Plan first{},fresh{};
 if(!Decode(c,r,game,frame,read,first)||!authorize(first.packet,frame)||
    !Decode(c,r,game,frame,read,fresh)||first.address!=fresh.address||
    std::memcmp(&first.packet,&fresh.packet,sizeof(first.packet))||first.parameters!=fresh.parameters||first.marker!=fresh.marker)return false;
 return write(fresh.address,fresh.marker);
}
}
