#pragma once
// Numeric snapshot of the complete native711140 bound source. Never call an
// object's virtual getter or change native visibility/room/physics state.
#include "ReceiverCasterCull226.hpp"
#include <cstring>
namespace receiver_bounds229 {
struct Snapshot {
 std::array<uint8_t,0x3c> body{};
 std::array<uint8_t,0x40> model{},bound{};
 std::array<float,16> pose{};
 std::array<uint8_t,16> getterCode{};
 uint32_t method{},getter{},modelInfo{},extra{},physical{},link{},boundAt{};
 float margin{};
 bool operator==(const Snapshot&)const noexcept=default;
};
template<class T>T At(const auto& a,unsigned offset) noexcept{T out{};memcpy(&out,a.data()+offset,sizeof(T));return out;}
template<class Read>bool Capture(Read&& read,uintptr_t game,uintptr_t entity,Snapshot& out) noexcept {
 Snapshot s{};
 if(!read(entity,s.body.data(),s.body.size()))return false;
 const auto type=(At<uint32_t>(s.body,0x28)>>6)&15u;
 const auto model=At<int16_t>(s.body,0x2e);const auto vt=At<uint32_t>(s.body,0);
 if(type<1||type>4||model<0||model>=31000||
  !read(uintptr_t(vt)+0x5c,&s.method,4)||s.method!=game+0x711140||
  !read(uintptr_t(vt)+0xa0,&s.getter,4))return false;
 const uint32_t expected=uint32_t(game)+(type==2?0x649190u:type==3?0x1d4fc0u:0x60d320u);
 if(s.getter!=expected||!read(s.getter,s.getterCode.data(),s.getterCode.size()))return false;
 if(type==2||type==3){
  const auto offset=type==2?0xdc4u:0x7b4u;
  const std::array<uint8_t,7> bytes{0x8b,0x81,uint8_t(offset),uint8_t(offset>>8),0,0,0xc3};
  if(memcmp(s.getterCode.data(),bytes.data(),bytes.size())||!read(entity+offset,&s.extra,4))return false;
 }else{
  constexpr std::array<uint8_t,16> bytes{0xf7,0x41,0x24,0,0,0,4,0x74,4,0x8b,0x41,0x38,0xc3,0x33,0xc0,0xc3};
  if(s.getterCode!=bytes)return false;
 }
 //711140 takes A0's result first, then entity+38. Both are snapshot data;
 // neither is executed or retained after this synchronous callback.
 s.physical=s.extra?s.extra:At<uint32_t>(s.body,0x38);
 if(s.physical&&(!read(uintptr_t(s.physical)+4,&s.link,4)||!s.link||
  !read(uintptr_t(s.link)+0xc,&s.boundAt,4)||!s.boundAt||!read(s.boundAt,s.bound.data(),s.bound.size())))return false;
 if(!read(game+0xe95cd8+uint32_t(model)*4,&s.modelInfo,4)||!s.modelInfo||
  !read(s.modelInfo,s.model.data(),s.model.size())||!read(entity+0xfc,&s.margin,4))return false;
 const auto pose=At<uint32_t>(s.body,0x20);
 if(pose&&!read(pose,s.pose.data(),sizeof(s.pose)))return false;
 out=s;return true;
}
struct Sphere {receiver_cull226::V centre{};double radius{};bool valid{};};
inline Sphere Enclose(const Snapshot& s)noexcept {
 using receiver_cull226::V;using receiver_cull226::Finite;
 auto vector=[&](const auto& bytes,unsigned at){return V{At<float>(bytes,at),At<float>(bytes,at+4),At<float>(bytes,at+8)};};
 const V lo=vector(s.model,0x20),hi=vector(s.model,0x30),modelCentre=vector(s.model,0x10);
 const double modelRadius=At<float>(s.model,0x1c);
 if(!Finite(lo)||!Finite(hi)||!Finite(modelCentre)||lo.x>hi.x||lo.y>hi.y||lo.z>hi.z||
  !std::isfinite(modelRadius)||modelRadius<0||!std::isfinite(s.margin))return {};
 auto length=[](V p){return std::sqrt(p.x*p.x+p.y*p.y+p.z*p.z);};
 const V extent{std::max(std::abs(lo.x),std::abs(hi.x)),std::max(std::abs(lo.y),std::abs(hi.y)),std::max(std::abs(lo.z),std::abs(hi.z))};
 // Enclose the model AABB AND model sphere, including the native fragment
 // bound when present. Root-centred enclosure also admits no-matrix heading.
 double radius=std::max(length(extent),length(modelCentre)+modelRadius);
 if(s.physical){const auto c=vector(s.bound,0x30);const double r=At<float>(s.bound,8);
  if(!Finite(c)||!std::isfinite(r)||r<0)return {};radius=std::max(radius,length(c)+r);}
 Sphere out{};double scale=1;
 if(At<uint32_t>(s.body,0x20)){
  for(unsigned i=0;i<4;++i)for(unsigned j=0;j<3;++j)if(!std::isfinite(s.pose[i*4+j]))return {};
  double largest=0;
  // Infinity-norm of A^T A bounds its largest eigenvalue, so its square root
  // bounds every scale/shear. Clamping to1 also encloses the native unscaled
  // radius. Padding words are not affine coefficients and are ignored.
  for(unsigned i=0;i<3;++i){double row=0;for(unsigned j=0;j<3;++j){double dot=0;for(unsigned k=0;k<3;++k)dot+=double(s.pose[i*4+k])*s.pose[j*4+k];row+=std::abs(dot);}largest=std::max(largest,row);}
  if(!std::isfinite(largest)||largest<=0)return {};scale=std::max(1.,std::sqrt(largest));
  out.centre={s.pose[12],s.pose[13],s.pose[14]};
 }else out.centre=vector(s.body,0x10);
 // Native CPhysical margin plus conservative2m articulation allowance. This
 // retains more geometry; it never shrinks a supplied native/model bound.
 out.radius=radius*scale+std::max(0.,double(s.margin))+2.;out.valid=Finite(out.centre)&&std::isfinite(out.radius)&&out.radius>=0;return out;
}
inline bool Outside(const receiver_cull226::Volume& volume,const Sphere& s)noexcept {
 if(!s.valid||!volume.count||volume.count>volume.planes.size())return false;
 for(unsigned i=0;i<volume.count;++i){const auto& p=volume.planes[i];const double d=p[0]*s.centre.x+p[1]*s.centre.y+p[2]*s.centre.z+p[3];
  if(std::isfinite(d)&&d+s.radius<-(1e-5*(std::abs(d)+s.radius)+.001))return true;}
 return false;
}
}
