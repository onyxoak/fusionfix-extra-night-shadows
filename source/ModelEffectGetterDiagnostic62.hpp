#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cmath>

// Diagnostic observations only. No captured pointer/code is execution or
// lifetime authority; no getter is called and failures do not reject a light.
namespace model_effect_getter62 {
enum class Status : std::uint32_t { Empty, InvalidAddress, EffectUnreadable,
 SlotsUnreadable, CodeUnreadable, EffectChanged, SlotsChanged, Observed };
struct Snapshot {
 Status status{};
 std::uint32_t effect{},vtable{};
 std::array<float,3> localOrigin{};
 bool localOriginCopied{},localOriginFinite{};
 std::array<std::uint32_t,4> slots{};
 std::array<std::array<std::uint8_t,16>,3> code{};
 std::uint32_t codeMask{};
};
constexpr bool Range(std::uint32_t p,std::uint32_t n) noexcept {
 return p && n && p<=UINT32_MAX-(n-1);
}
template<class Read>
Snapshot Observe(std::uint32_t effect,Read&& read) noexcept {
 Snapshot s{};s.effect=effect;
 if(!Range(effect,16)){s.status=Status::InvalidAddress;return s;}
 // Native81DC58/6F/7D read effect+4/+8/+C before virtual dispatch.
 // This is the outer effect's authored origin, not a guessed descriptor
 // offset or a claim that the getter returns the same object.
 std::array<std::uint8_t,16> header{};
 if(!read(effect,header.data(),header.size())){s.status=Status::EffectUnreadable;return s;}
 std::memcpy(&s.vtable,header.data(),4);
 std::memcpy(s.localOrigin.data(),header.data()+4,12);
 s.localOriginCopied=true;
 s.localOriginFinite=std::isfinite(s.localOrigin[0])&&std::isfinite(s.localOrigin[1])&&std::isfinite(s.localOrigin[2]);
 if(!Range(s.vtable,16)){s.status=Status::InvalidAddress;return s;}
 if(!read(s.vtable,s.slots.data(),16)){s.status=Status::SlotsUnreadable;return s;}
 for(unsigned i=0;i<3;++i){
  const auto target=s.slots[i+1];
  if(Range(target,16)&&read(target,s.code[i].data(),16))s.codeMask|=1u<<i;
 }
 std::array<std::uint8_t,16> fresh{};std::array<std::uint32_t,4> slots{};
 if(!read(effect,fresh.data(),fresh.size())||fresh!=header){s.status=Status::EffectChanged;return s;}
 if(!read(s.vtable,slots.data(),16)||slots!=s.slots){s.status=Status::SlotsChanged;return s;}
 s.status=s.codeMask==7?Status::Observed:Status::CodeUnreadable;
 return s;
}
inline void Hex(const std::array<std::uint8_t,16>& bytes,char(&out)[33]) noexcept {
 constexpr char h[]="0123456789abcdef";
 for(unsigned i=0;i<16;++i){out[i*2]=h[bytes[i]>>4];out[i*2+1]=h[bytes[i]&15];}
 out[32]=0;
}
template<std::size_t N>
bool Format(const Snapshot& s,std::uint32_t id,std::uint32_t frame,
 std::uint32_t thread,std::uint32_t descriptor,char(&out)[N]) noexcept {
 char kind[33],common[33],light[33];
 Hex(s.code[0],kind);Hex(s.code[1],common);Hex(s.code[2],light);
 const int n=std::snprintf(out,N,
  "id=%u frame=%u thread=%u effect=%x descriptor=%x status=%u vtable=%x kind_getter=%x common_getter=%x light_getter=%x code_mask=%u kind_bytes=%s common_bytes=%s light_bytes=%s observed_not_called=1 no_authority=1",
  id,frame,thread,s.effect,descriptor,unsigned(s.status),s.vtable,s.slots[1],s.slots[2],s.slots[3],s.codeMask,kind,common,light);
 return n>=0&&std::size_t(n)<N;
}
template<std::size_t N>
bool FormatOrigin(const Snapshot& s,std::uint32_t id,std::uint32_t frame,
 std::uint32_t thread,char(&out)[N]) noexcept {
 const int n=std::snprintf(out,N,
  "id=%u frame=%u thread=%u effect=%x status=%u origin_copied=%u local_origin=%.9g,%.9g,%.9g finite=%u origin_offsets=4,8,12 source=native_outer81dc58 observation_only=1 no_authority=1",
  id,frame,thread,s.effect,unsigned(s.status),unsigned(s.localOriginCopied),double(s.localOrigin[0]),double(s.localOrigin[1]),double(s.localOrigin[2]),unsigned(s.localOriginFinite));
 return n>=0&&std::size_t(n)<N;
}
}
