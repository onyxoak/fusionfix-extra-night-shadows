#pragma once
// Two authored CT_GW sources only. Geometry evidence: placement197-followup.
// Native point type and every non-position packet word remain untouched here.
#include "NativeProjectionRegistry66.hpp"
#include "TrafficSignalIdentity.hpp"
#include <cmath>
namespace arcade_placement197 {
using Packet=native_projection66::Packet;
using Matrix=std::array<float,16>;
constexpr uint32_t SourceModel=0x2e73d323,FixtureModel=0x90507341;
constexpr std::array<std::array<float,3>,2> Local{{{-15.212f,24.0128f,-1.43025f},{-6.94073f,30.2293f,-1.43025f}}};
constexpr std::array<std::array<float,3>,2> OriginalWorld{{{-34.497642517089844f,-326.367645263671875f,15.356832504272461f},{-26.226375579833984f,-320.151123046875f,15.356832504272461f}}};
// Closest VA_MH1 triangles65/204, offset30mm along actual outward normal.
constexpr std::array<std::array<float,3>,2> RepairedLocal{{{-15.074825134375509f,24.0128f,0.23388017289465424f},{-6.94073f,30.2293f,0.2287890625f}}};
constexpr std::array<float,3> Origin{-19.28564453125f,-350.38043212890625f,16.78708267211914f};
inline uint32_t Bits(float v) noexcept{return std::bit_cast<uint32_t>(v);}
inline bool KnownPose(const Matrix& m) noexcept {
 for(auto v:m)if(!std::isfinite(v))return false;
 for(unsigned row=0;row<3;++row)for(unsigned col=0;col<3;++col)
  if(m[row*4+col]!=(row==col?1.f:0.f))return false;
 for(unsigned j=0;j<3;++j)if(Bits(m[12+j])!=Bits(Origin[j]))return false;
 return true;
}
inline std::array<float,3> Transform(const Matrix& m,const std::array<float,3>& p) noexcept {
 std::array<float,3> out{};for(unsigned j=0;j<3;++j)out[j]=m[j]*p[0]+m[4+j]*p[1]+m[8+j]*p[2]+m[12+j];return out;
}
inline int NumericCandidate(const Packet& p) noexcept {
 if(p[17]!=0||p[18]!=0x41u||p[24]||p[12]!=Bits(1.f)||p[13]!=Bits(0.9647059440612793f)||
    p[14]!=Bits(0.7372549176216125f)||p[16]!=Bits(20.f)||p[21]!=Bits(10.f))return -1;
 for(unsigned n=0;n<2;++n){bool same=true;for(unsigned j=0;j<3;++j)same=same&&p[8+j]==Bits(OriginalWorld[n][j]);if(same)return int(n);}return -1;
}
inline bool Descriptor(const std::array<uint8_t,108>& d) noexcept {
 uint32_t radius{},intensity{};std::memcpy(&radius,d.data()+0x38,4);std::memcpy(&intensity,d.data()+0x3c,4);
 return d[0x5b]==1&&d[0x28]==255&&d[0x29]==246&&d[0x2a]==188&&radius==Bits(10.f)&&intensity==Bits(20.f);
}
inline bool Effect(unsigned ordinal,const std::array<uint32_t,4>& e) noexcept {
 if(ordinal>=2||e[0]<0x10000)return false;
 for(unsigned j=0;j<3;++j)if(e[j+1]!=Bits(Local[ordinal][j]))return false;return true;
}
inline bool SamePacket(const Packet& a,const Packet& b) noexcept {
 return a[24]==b[24]&&native_projection66::Witness(a)==native_projection66::Witness(b);
}
inline bool Move(Packet& p,unsigned ordinal,const Matrix& owner,const Matrix& fixture) noexcept {
 if(ordinal>=2||NumericCandidate(p)!=int(ordinal)||!KnownPose(owner)||!KnownPose(fixture))return false;
 const auto original=Transform(owner,Local[ordinal]);
 for(unsigned j=0;j<3;++j)if(Bits(original[j])!=p[8+j])return false;
 const auto out=Transform(fixture,RepairedLocal[ordinal]);
 for(unsigned j=0;j<3;++j)p[8+j]=Bits(out[j]);return true;
}
// Only bytes changed by this feature/Intern may be restored. Numeric native
// metadata outside the120-byte constructor witness is never replayed.
inline bool Restore(Packet& current,const Packet& moved,const Packet& original) noexcept {
 if(!SamePacket(current,moved))return false;
 for(unsigned j=0;j<3;++j)current[8+j]=original[8+j];
 current[18]=original[18];current[24]=original[24];return true;
}
// Actual constructor integration. Failed projection issuance or lease
// publication restores the complete owned numeric packet before fallback.
template<class Place,class Intern,class Finish>
inline bool Adapt(Packet& packet,bool placement,Place place,Intern intern,Finish finish) noexcept {
 if(!placement||NumericCandidate(packet)<0)return intern(packet,false);
 Packet original=packet;const bool relocated=place(packet,original);
 const bool success=intern(packet,relocated);
 if(relocated){const bool committed=finish(packet,success);
  if(!success||!committed){packet=original;return false;}}
 return success;
}
// Repair identities never merge with an unrelated native source at the same
// projection. Two immutable entries; exhaustion refuses relocation. Existing
// map selection and seven-map native allocator are unchanged.
struct Projections {
 alignas(64) std::array<std::array<uint8_t,64>,2> tokens{};
 native_projection66::Registry<2> registry;
 native_projection66::IssuedKeys<2> issued;
 void Init() noexcept{registry.Init(uint32_t(reinterpret_cast<uintptr_t>(tokens.data())));}
 bool Owns(uint32_t key)const noexcept{return registry.keyBase&&issued.Contains(registry.keyBase,key);}
 template<class Collision>uint32_t Intern(Packet& packet,uint32_t frame,Collision collision) noexcept {
  const auto key=registry.Observe(packet,frame,collision);if(!key||!issued.Publish(registry.keyBase,key))return 0;
  packet[24]=key;packet[18]|=4u;return key;
 }
 bool Validate(uint32_t key,const Packet& packet)const noexcept{return Owns(key)&&registry.Validate(key,packet);}
};
}
