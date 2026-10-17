#pragma once
#include <cstdint>
#include <cstring>
#include <span>

namespace local_shader244 {
struct Region {uint32_t begin{},size{},flags{};};
struct Binding {uint32_t registration{},name{},epoch{},group{};explicit operator bool()const noexcept{return registration!=0;}};
inline uint32_t Word(const uint8_t* p) noexcept {uint32_t n{};std::memcpy(&n,p,4);return n;}
inline bool In(std::span<const Region> regions,uint32_t at,uint32_t n,uint32_t flags) noexcept {
 for(const auto& r:regions)if((r.flags&flags)==flags&&at>=r.begin&&uint64_t(at)+n<=uint64_t(r.begin)+r.size)return true;
 return false;
}
// Follow the registration of the actual local-depth technique. Neither a mod
// filename, version nor historical data address supplies draw-pass authority.
inline Binding Discover(std::span<const uint8_t> image,uint32_t base,std::span<const Region> regions) noexcept {
 Binding result{};
 for(const auto& region:regions){
  if(!(region.flags&0x20000000u))continue;
  const uint64_t end=uint64_t(region.begin)+region.size;
  if(end>image.size())return {};
  for(uint32_t at=region.begin;uint64_t(at)+21<=end;++at){
   const auto* p=image.data()+at;
   if(p[0]!=0x68||p[5]!=0xc6||p[6]!=0x45||p[7]!=0xfc||p[8]!=8||p[9]!=0xff||p[10]!=0xd7||p[11]!=0x68||p[16]!=0xa3)continue;
   const auto name=Word(p+1),epoch=Word(p+12),group=Word(p+17);
   if(name<base||epoch<base||group<base)continue;
   const uint32_t nr=name-base,er=epoch-base,gr=group-base;
   if(uint64_t(nr)+9>image.size()||uint64_t(er)+4>image.size()||uint64_t(gr)+4>image.size()||
      (er&3u)||(gr&3u)||er==gr||
      !In(regions,nr,9,0x40000000u)||!In(regions,er,4,0x80000000u)||!In(regions,gr,4,0x80000000u)||
      std::memcmp(image.data()+nr,"wd_local",9))continue;
   if(result)return {}; // Ambiguous registration never authorizes a write.
   result={at,nr,er,gr};
  }
 }
 return result;
}
}
