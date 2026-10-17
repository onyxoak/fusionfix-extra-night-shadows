#define NOMINMAX
#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include "core/ReceiverCasterCull226.hpp"
using receiver_cull226::V;
static fusionfix::shadows::ShadowView View(){
 fusionfix::shadows::ShadowView v{};v.valid=true;
 for(unsigned i=0;i<4;++i)v.view[i][i]=1;
 v.projection[0][0]=1;v.projection[1][1]=1;v.projection[2][2]=100.f/99.f;
 v.projection[2][3]=1;v.projection[3][2]=-100.f/99.f;return v;
}
static std::array<float,16> Pose(V p){std::array<float,16> m{};for(unsigned i=0;i<4;++i)m[5*i]=1; m[12]=float(p.x);m[13]=float(p.y);m[14]=float(p.z);return m;}
int main(){
 using namespace receiver_cull226;auto view=View();auto inside=Prepare(view,{0,0,10});
 assert(inside.count==6);
 assert(!OutsideBox(inside,{-1,-1,-1},{1,1,1},Pose({0,0,10})));
 assert(OutsideBox(inside,{-1,-1,-1},{1,1,1},Pose({100,0,10})));
 assert(!OutsideBox(inside,{-100,-1,-1},{100,1,1},Pose({80,0,10})));
 assert(!OutsideBox(inside,{-1,-1,-1},{1,1,1},Pose({0,0,-.5})));
 // A caster outside the camera can still lie between an outside light and a
 // visible receiver. The wrong camera-only cull would remove this object.
 auto outside=Prepare(view,{200,0,10});assert(outside.count<inside.count);
 assert(!OutsideBox(outside,{0,0,0},{0,0,0},Pose({100,0,10})));
 auto invalid=view;invalid.valid=false;assert(!Prepare(invalid,{0,0,10}).count);
 assert(!Prepare(view,{NAN,0,10}).count);invalid=view;invalid.projection[0][0]=NAN;assert(!Prepare(invalid,{0,0,10}).count);
 assert(!OutsideBox(inside,{2,0,0},{1,1,1},Pose({100,0,10})));
 auto m=Pose({100,0,10});m[0]=NAN;assert(!OutsideBox(inside,{-1,-1,-1},{1,1,1},m));
 m=Pose({100,0,10});m[15]=0;assert(!OutsideBox(inside,{-1,-1,-1},{1,1,1},m));
 assert(OutsideRotatedOrigin(inside,{-1,-1,-1},{1,1,1},{100,0,10}));
 assert(!OutsideRotatedOrigin(inside,{-100,-1,-1},{100,1,1},{80,0,10}));
 // Independent segment containment oracle: every possible occluder between
 // each visible receiver and its light must survive, including offscreen
 // lights and occluders, lights behind/above/beyond the camera, huge scales,
 // reflected/sheared affine model poses and bounds containing the origin.
 std::mt19937 rng(226);std::uniform_real_distribution<double> unit(0,1),coord(-500,500),shear(-3,3);
 unsigned checks=0;
 for(unsigned n=0;n<100000;++n){
  const V light{coord(rng),coord(rng),coord(rng)};
  const double rz=1.+98.*unit(rng);const V receiver{(unit(rng)*2-1)*rz*.999,(unit(rng)*2-1)*rz*.999,rz};
  const double t=unit(rng);const V point{light.x+(receiver.x-light.x)*t,light.y+(receiver.y-light.y)*t,light.z+(receiver.z-light.z)*t};
  const auto volume=Prepare(view,light);auto pose=Pose(point);
  assert(!OutsideBox(volume,{0,0,0},{0,0,0},pose));
  for(unsigned i=0;i<3;++i)for(unsigned j=0;j<3;++j)pose[i*4+j]=float(shear(rng));
  assert(!OutsideBox(volume,{-1,-2,-3},{1,2,3},pose));
  assert(!OutsideRotatedOrigin(volume,{-1,-2,-3},{1,2,3},point));checks+=3;
 }
 // Check exact support against an independent eight-corner affine transform.
 for(unsigned n=0;n<10000;++n){
  auto pose=Pose({coord(rng),coord(rng),coord(rng)});
  for(unsigned i=0;i<3;++i)for(unsigned j=0;j<3;++j)pose[i*4+j]=float(shear(rng));
  const V lo{-3,-4,-5},hi{6,7,8};bool definitelyOutside=false;
  for(unsigned i=0;i<inside.count;++i){const auto& p=inside.planes[i];double high=-1e100;
   for(unsigned mask=0;mask<8;++mask){const double x=mask&1?hi.x:lo.x,y=mask&2?hi.y:lo.y,z=mask&4?hi.z:lo.z;
    const V world{x*pose[0]+y*pose[4]+z*pose[8]+pose[12],x*pose[1]+y*pose[5]+z*pose[9]+pose[13],x*pose[2]+y*pose[6]+z*pose[10]+pose[14]};
    high=std::max(high,p[0]*world.x+p[1]*world.y+p[2]*world.z+p[3]);}
   if(high<-.1)definitelyOutside=true;
  }
  if(OutsideBox(inside,lo,hi,pose))assert(definitelyOutside);
  ++checks;
 }
 std::cout<<"PASS226 "<<checks<<" segment/corner containment checks plus invalid inputs and offscreen occluders; math only, not an FPS or renderer proof\n";
}
