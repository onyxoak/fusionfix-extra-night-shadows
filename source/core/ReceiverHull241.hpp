#pragma once
#include <algorithm>

namespace receiver_cull226 {
// Add only supporting planes of the convex hull of all camera-frustum corners
// and the light. Every segment joining the light to a visible receiver survives.
// Singular/infinite projections retain the original larger volume.
inline void Tighten241(Volume& volume,const fusionfix::shadows::ShadowView& view,V light) noexcept {
 double a[4][8]{};
 for(unsigned i=0;i<4;++i){
  for(unsigned j=0;j<4;++j)for(unsigned k=0;k<4;++k)a[i][j]+=double(view.view[i][k])*view.projection[k][j];
  a[i][i+4]=1;
 }
 for(unsigned col=0;col<4;++col){
  unsigned pivot=col;for(unsigned row=col+1;row<4;++row)if(std::abs(a[row][col])>std::abs(a[pivot][col]))pivot=row;
  if(!std::isfinite(a[pivot][col])||std::abs(a[pivot][col])<1e-12)return;
  for(unsigned k=0;k<8;++k)std::swap(a[col][k],a[pivot][k]);
  const auto divisor=a[col][col];for(double& v:a[col])v/=divisor;
  for(unsigned row=0;row<4;++row)if(row!=col){const auto factor=a[row][col];for(unsigned k=0;k<8;++k)a[row][k]-=factor*a[col][k];}
 }
 std::array<V,8> corners{};double scale=1;
 for(unsigned n=0;n<8;++n){
  const double clip[4]{n&1?1.:-1.,n&2?1.:-1.,n&4?1.:0.,1.};double world[4]{};
  for(unsigned i=0;i<4;++i)for(unsigned k=0;k<4;++k)world[i]+=clip[k]*a[k][i+4];
  if(!std::isfinite(world[3])||world[3]<=1e-10)return;
  corners[n]={world[0]/world[3],world[1]/world[3],world[2]/world[3]};
  if(!Finite(corners[n]))return;
  scale=std::max({scale,std::abs(corners[n].x),std::abs(corners[n].y),std::abs(corners[n].z)});
 }
 scale=std::max({scale,std::abs(light.x),std::abs(light.y),std::abs(light.z)});
 const double tolerance=1e-9*scale;
 for(unsigned first=0;first<8;++first)for(unsigned bit=0;bit<3;++bit){
  const unsigned second=first^(1u<<bit);if(second<first)continue;
  const auto x=corners[first],y=corners[second];
  const V e{y.x-x.x,y.y-x.y,y.z-x.z},l{light.x-x.x,light.y-x.y,light.z-x.z};
  V normal{e.y*l.z-e.z*l.y,e.z*l.x-e.x*l.z,e.x*l.y-e.y*l.x};
  const double length=std::sqrt(normal.x*normal.x+normal.y*normal.y+normal.z*normal.z);
  if(!std::isfinite(length)||length<1e-9)continue;
  std::array<double,4> p{normal.x/length,normal.y/length,normal.z/length,0};
  p[3]=-(p[0]*x.x+p[1]*x.y+p[2]*x.z);
  double low=0,high=0;
  for(const auto c:corners){const auto d=p[0]*c.x+p[1]*c.y+p[2]*c.z+p[3];low=std::min(low,d);high=std::max(high,d);}
  if(low < -tolerance && high > tolerance)continue;
  if(high<=tolerance && low < -tolerance)for(double& v:p)v=-v;
  else if(low>=-tolerance && high<=tolerance)continue;
  // Include a two-metre receiver margin and all corner/light rounding error.
  double minimum=p[0]*light.x+p[1]*light.y+p[2]*light.z+p[3];
  for(const auto c:corners)minimum=std::min(minimum,p[0]*c.x+p[1]*c.y+p[2]*c.z+p[3]);
  p[3]+=2.+std::max(0.,-minimum)+tolerance;
  bool duplicate=false;
  for(unsigned n=0;n<volume.count;++n){const auto& q=volume.planes[n];if(p[0]*q[0]+p[1]*q[1]+p[2]*q[2]>1.-1e-9&&std::abs(p[3]-q[3])<1e-5){duplicate=true;break;}}
  if(!duplicate&&volume.count<volume.planes.size())volume.planes[volume.count++]=p;
 }
}
}
