#pragma once
#include "ShadowVolumeVisibility43.hpp"
#include <array>
#include <cstdint>
#include <span>
namespace map_contribution198 {
enum class Class : int {UnknownView=0,MayReachMain=1,OutsideMain=2,Vehicle=3,Invalid=4,Ambiguous=5};
inline Class Classify(bool fresh,const fusionfix::shadows::ShadowVolumeFrustum20& frustum,
 fusionfix::shadows::Vec3 position,float radius,bool vehicle,bool ambiguous) noexcept {
 if(ambiguous)return Class::Ambiguous;
 if(vehicle)return Class::Vehicle;
 if(!std::isfinite(radius)||radius<=0||!std::isfinite(position.x)||!std::isfinite(position.y)||!std::isfinite(position.z))return Class::Invalid;
 if(!fresh||!frustum.valid)return Class::UnknownView;
 return fusionfix::shadows::ShadowVolumeMayReachView(frustum,position,radius)?Class::MayReachMain:Class::OutsideMain;
}
inline uint64_t Signature(std::span<const uint8_t> bytes) noexcept {
 uint64_t value=14695981039346656037ull;
 for(const auto byte:bytes){value^=byte;value*=1099511628211ull;}return value;
}
struct Row {uint32_t key{},index{},slot{};uint64_t generation{},signature{};Class classification=Class::Invalid;unsigned seenKinds{};bool valid{};};
// Numeric diagnostic correlation only. No owner, source or map authority.
inline int Match(const std::array<Row,7>& rows,uint32_t key,uint32_t slot) noexcept {
 int found=-1;
 for(unsigned i=0;i<rows.size();++i)if(rows[i].valid&&rows[i].key==key){if(found>=0)return -1;found=int(i);}
 return found>=0&&rows[size_t(found)].slot+1==slot?found:-1;
}
}
