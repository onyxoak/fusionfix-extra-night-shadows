#pragma once
// View/light hull culling. Original camera planes whose positive half-space
// contains the light contain every segment from that light to a receiver.
// Silhouette-edge planes are deliberately omitted: this is a larger volume.
// See NVIDIA GPU Gems chapter15, Managing Visibility for Per-Pixel Lighting.
#include "ShadowVolumeVisibility43.hpp"
#include <array>
#include <cmath>

namespace receiver_cull226 {
struct V { double x{},y{},z{}; };
struct Volume {std::array<std::array<double,4>,18> planes{};unsigned count{};};
inline bool Finite(V v) noexcept {return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z);}
inline Volume Prepare(const fusionfix::shadows::ShadowView& view,V light) noexcept {
 Volume out{};if(!view.valid||!Finite(light))return out;
 const auto f=fusionfix::shadows::PrepareShadowVolumeFrustum20(view);if(!f.valid)return out;
 for(const auto& p:f.planes){
  if(!(p[4]>1e-10)||!std::isfinite(p[4]))return {};
  std::array<double,4> plane{};for(unsigned i=0;i<4;++i)plane[i]=p[i]/p[4];
  // An expanded view adds room for temporal jitter, clipping and camera edges.
  plane[3]+=2.0;
  const auto d=plane[0]*light.x+plane[1]*light.y+plane[2]*light.z+plane[3];
  if(!std::isfinite(d))return {};
  if(d>=0)out.planes[out.count++]=plane;
 }
 return out;
}
inline bool OutsideBox(const Volume& volume,V lo,V hi,const std::array<float,16>& m) noexcept {
 if(!volume.count||volume.count>volume.planes.size()||!Finite(lo)||!Finite(hi)||lo.x>hi.x||lo.y>hi.y||lo.z>hi.z)return false;
 for(float v:m)if(!std::isfinite(v))return false;
 // Arbitrary affine scales, reflections and shears are supported. A non-affine
 // or degenerate native transform retains the original draw.
 if(std::abs(m[3])>1e-6||std::abs(m[7])>1e-6||std::abs(m[11])>1e-6||std::abs(m[15]-1.f)>1e-6)return false;
 const double a[3]{lo.x,lo.y,lo.z},b[3]{hi.x,hi.y,hi.z};
 for(unsigned j=0;j<volume.count;++j){
  const auto& p=volume.planes[j];double support=p[3]+p[0]*m[12]+p[1]*m[13]+p[2]*m[14];
  double error=std::abs(p[3])+std::abs(p[0]*m[12])+std::abs(p[1]*m[13])+std::abs(p[2]*m[14]);
  for(unsigned i=0;i<3;++i){
   const double dot=p[0]*m[i*4]+p[1]*m[i*4+1]+p[2]*m[i*4+2];
   const auto high=std::max(dot*a[i],dot*b[i]);support+=high;error+=std::abs(high);
  }
  // Extra relative slack covers native float arithmetic at large coordinates.
  if(std::isfinite(support)&&support<-(1e-5*error+.001))return true;
 }
 return false;
}
inline bool OutsideRotatedOrigin(const Volume& volume,V lo,V hi,V origin) noexcept {
 if(!volume.count||volume.count>volume.planes.size()||!Finite(lo)||!Finite(hi)||!Finite(origin)||lo.x>hi.x||lo.y>hi.y||lo.z>hi.z)return false;
 // A no-matrix native placement has position+heading. This enclosing sphere
 // admits every possible rotation of the complete model AABB around its root.
 const double x=std::max(std::abs(lo.x),std::abs(hi.x)),y=std::max(std::abs(lo.y),std::abs(hi.y)),z=std::max(std::abs(lo.z),std::abs(hi.z));
 const double radius=std::sqrt(x*x+y*y+z*z);if(!std::isfinite(radius))return false;
 for(unsigned i=0;i<volume.count;++i){const auto& p=volume.planes[i];
  const auto d=p[0]*origin.x+p[1]*origin.y+p[2]*origin.z+p[3];
  if(std::isfinite(d)&&d+radius<-(1e-5*(std::abs(d)+radius)+.001))return true;
 }
 return false;
}
}

#include "ReceiverHull241.hpp"
