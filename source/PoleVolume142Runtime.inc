// Current selected native light only; no additional shadow map or cached owner.
#include "core/ShadowReceiver.hpp"
#ifndef LIBERTY_POLE_TEST_PREFLIGHT142
#include "NativeProjectionRead66.hpp"
#endif
namespace PoleVolume142 {
struct Volume {
 uint32_t bank{},count{},index{};std::array<uint8_t,128> packet{};bool valid{};
};
inline float Float(const std::array<uint8_t,128>& p,unsigned offset) noexcept {float v{};std::memcpy(&v,p.data()+offset,4);return v;}
inline uint32_t Word(const std::array<uint8_t,128>& p,unsigned offset) noexcept {uint32_t v{};std::memcpy(&v,p.data()+offset,4);return v;}
template<class Read> bool Capture(Read&& read,uint32_t game,uint32_t key,Volume& result) noexcept {
 result={};uint32_t bank{},count{};
 if(!read(uint64_t(game)+0xc3eed8,&bank,4)||!read(uint64_t(game)+0x110e240,&count,4)||!count||count>640||!pole_caster134::Address(bank,size_t(count)*128))return false;
 std::array<uint8_t,81920> bytes{};
 // One bounded bank copy per map, never per visited geometry entity.
 if(!native_projection66::Preflight(bank,size_t(count)*128).allowed||!Liberty20::CopyLocal(bank,bytes.data(),size_t(count)*128))return false;
 unsigned matches=0,index=0;for(unsigned i=0;i<count;++i){uint32_t current{};std::memcpy(&current,bytes.data()+i*128+0x60,4);if(current==key){++matches;index=i;}}
 if(matches!=1)return false;
 std::array<uint8_t,128> packet{};uint32_t againBank{},againCount{};
 if(!read(uint64_t(bank)+index*128,packet.data(),128)||std::memcmp(packet.data(),bytes.data()+index*128,128)||
  !read(uint64_t(game)+0xc3eed8,&againBank,4)||againBank!=bank||!read(uint64_t(game)+0x110e240,&againCount,4)||againCount!=count)return false;
 const auto type=Word(packet,0x44);const float radius=Float(packet,0x54),intensity=Float(packet,0x40);
 if(!std::isfinite(intensity)||intensity<=0)return false;
 bool emitting=false;for(unsigned at:{0x30u,0x34u,0x38u}){const float color=Float(packet,at);if(!std::isfinite(color)||color<0)return false;emitting|=color>0;}if(!emitting)return false;
 if((type!=0&&type!=2)||!std::isfinite(radius)||radius<=0||radius>10000)return false;
 for(unsigned at:{0x20u,0x24u,0x28u})if(!std::isfinite(Float(packet,at)))return false;
 if(type==2){const float cosine=Float(packet,0x5c);double norm=0;for(unsigned at:{0u,4u,8u}){const float v=Float(packet,at);if(!std::isfinite(v))return false;norm+=double(v)*v;}if(norm<0.0001||!std::isfinite(cosine)||cosine<=0||cosine>1)return false;}
 result={bank,count,index,packet,true};return true;
}
template<class Read> bool Current(Read&& read,uint32_t game,const Volume& v) noexcept {
 uint32_t bank{},count{};std::array<uint8_t,128> packet{};
 return v.valid&&read(uint64_t(game)+0xc3eed8,&bank,4)&&bank==v.bank&&read(uint64_t(game)+0x110e240,&count,4)&&count==v.count&&
  read(uint64_t(v.bank)+v.index*128,packet.data(),128)&&packet==v.packet;
}
// Rejection proxy, NOT an authenticated native model/frustum bound: a 30m
// sphere centered at the current root. No upright-pole assumption is made.
// Native per-light frustum, room, damage and fragment traversal still decide
// the actual draw. Future measured model envelopes may tighten this proxy.
template<class Read> bool Touches(Read&& read,uint32_t entity,const Volume& v,uint32_t& pose,std::array<float,16>& matrix) noexcept {
 if(!v.valid||!read(uint64_t(entity)+0x20,&pose,4)||!pole_caster134::Address(pose,64)||!read(pose,matrix.data(),64))return false;
 for(float value:matrix)if(!std::isfinite(value))return false;
 using fusionfix::shadows::Vec3;
 // Unknown/scaled/sheared poses retain native eligibility rather than risk a
 // false geometry rejection based on this stock-size proxy.
 for(unsigned i=0;i<3;++i){double norm=0;for(unsigned j=0;j<3;++j)norm+=double(matrix[i*4+j])*matrix[i*4+j];if(norm<0.95||norm>1.05)return true;
  for(unsigned k=i+1;k<3;++k){double dot=0;for(unsigned j=0;j<3;++j)dot+=double(matrix[i*4+j])*matrix[k*4+j];if(std::abs(dot)>0.05)return true;}}
 const Vec3 center{matrix[12],matrix[13],matrix[14]};const Vec3 source{Float(v.packet,0x20),Float(v.packet,0x24),Float(v.packet,0x28)};
 constexpr float extent=30;const float radius=Float(v.packet,0x54);
 if(Word(v.packet,0x44)==2)return fusionfix::shadows::BeamTouchesReceiver(center,extent,source,{Float(v.packet,0),Float(v.packet,4),Float(v.packet,8)},Float(v.packet,0x5c),radius);
 const double x=double(center.x)-source.x,y=double(center.y)-source.y,z=double(center.z)-source.z;
 return x*x+y*y+z*z<=double(radius+extent)*(radius+extent);
}
}
