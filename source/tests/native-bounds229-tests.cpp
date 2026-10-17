#include "core/NativeBounds229.hpp"
#include <cassert>
#include <iostream>
#include <random>
template<class T>void Put229(auto& a,unsigned at,T v){memcpy(a.data()+at,&v,sizeof(v));}
int main(){
 using namespace receiver_bounds229;using receiver_cull226::V;
 Snapshot s{};Put229(s.body,0x20,uint32_t(1));
 for(unsigned i=0;i<3;++i){Put229(s.model,0x20+i*4,float(-2));Put229(s.model,0x30+i*4,float(2));}
 Put229(s.model,0x1c,float(3));s.physical=1;Put229(s.bound,8,float(4));
 Put229(s.bound,0x30,float(2));Put229(s.bound,0x34,float(-1));Put229(s.bound,0x38,float(3));s.margin=1;
 std::mt19937 rng(229);std::uniform_real_distribution<float> d(-8,8),unit(-1,1);
 unsigned checked=0;
 for(unsigned sample=0;sample<20000;++sample){
  for(unsigned i=0;i<4;++i)for(unsigned j=0;j<3;++j)s.pose[i*4+j]=d(rng);
  // Arbitrary unused native padding must not affect the native XYZ transform.
  for(unsigned i:{3u,7u,11u,15u})s.pose[i]=NAN;
  const auto sphere=Enclose(s);assert(sphere.valid);
  auto contains=[&](V p){
   V w{s.pose[12],s.pose[13],s.pose[14]};
   w.x+=s.pose[0]*p.x+s.pose[4]*p.y+s.pose[8]*p.z;
   w.y+=s.pose[1]*p.x+s.pose[5]*p.y+s.pose[9]*p.z;
   w.z+=s.pose[2]*p.x+s.pose[6]*p.y+s.pose[10]*p.z;
   const auto x=w.x-sphere.centre.x,y=w.y-sphere.centre.y,z=w.z-sphere.centre.z;
   assert(std::sqrt(x*x+y*y+z*z)<=sphere.radius+1e-5);++checked;
  };
  for(unsigned mask=0;mask<8;++mask)contains({(mask&1)?2.:-2.,(mask&2)?2.:-2.,(mask&4)?2.:-2.});
  V direction{unit(rng),unit(rng),unit(rng)};const auto n=std::sqrt(direction.x*direction.x+direction.y*direction.y+direction.z*direction.z);
  if(n>1e-6){contains({2+4*direction.x/n,-1+4*direction.y/n,3+4*direction.z/n});contains({3*direction.x/n,3*direction.y/n,3*direction.z/n});}
 }
 receiver_cull226::Volume v{};v.count=1;v.planes[0]={1,0,0,0};
 assert(Outside(v,{{-20,0,0},3,true}));assert(!Outside(v,{{-2,0,0},3,true}));
 s.margin=NAN;assert(!Enclose(s).valid);s.margin=0;Put229(s.bound,8,float(-1));assert(!Enclose(s).valid);
 std::cout<<"PASS229 conservative model/native-bound enclosure: "<<checked<<" transformed corners/sphere points, arbitrary shears/scales/reflections and native padding, crossing sphere retained. Not game FPS or universal attachment proof.\n";
}
