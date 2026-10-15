#pragma once
// Separate immutable metadata ledgers for the retained native flags & 0x300.
// Identical canonical projections cannot cross these classes. No source,
// illumination, geometry or native shadow-map capacity is changed.
#include "NativeProjectionRegistry66.hpp"
namespace native_class_domains105 {
inline constexpr uint32_t NativeClassMask=0x300u;
template<unsigned Capacity=4096> struct Registry {
 using Ledger=native_projection66::Registry<Capacity>;
 using Entry=typename Ledger::Entry;
 std::array<Ledger,4> domains{};
 bool configured{},snowMetadata70{},volumetricMetadata199{};
 static constexpr uint64_t Span=uint64_t(Capacity)*64;
 static bool Range(uint32_t base) noexcept {
  return base&&!(base&63u)&&uint64_t(base)+Span<=(uint64_t(1)<<32);
 }
 bool Init(const std::array<uint32_t,4>& bases) noexcept {
  if(configured){
   for(unsigned i=0;i<4;++i)if(bases[i]!=domains[i].keyBase)return false;
   return true;
  }
  for(unsigned i=0;i<4;++i){
   if(!Range(bases[i]))return false;
   for(unsigned j=0;j<i;++j)
    if(!(uint64_t(bases[i])+Span<=bases[j]||uint64_t(bases[j])+Span<=bases[i]))return false;
  }
  for(unsigned i=0;i<4;++i){domains[i].Init(bases[i]);domains[i].snowMetadata70=snowMetadata70;domains[i].volumetricMetadata199=volumetricMetadata199;}
  configured=true;return true;
 }
 static unsigned DomainIndex(const native_projection66::Packet& packet) noexcept {
  return (packet[18]&NativeClassMask)>>8;
 }
 Ledger& Domain(const native_projection66::Packet& packet) noexcept {return domains[DomainIndex(packet)];}
 const Ledger& Domain(const native_projection66::Packet& packet) const noexcept {return domains[DomainIndex(packet)];}
 uint32_t Used() const noexcept {
  uint32_t used=0;for(const auto& domain:domains)used+=domain.used;return used;
 }
 bool Full(const native_projection66::Packet& packet) const noexcept {return Domain(packet).used==Capacity;}
 int KeyDomain(uint32_t key) const noexcept {
  if(!configured)return -1;
  for(unsigned i=0;i<4;++i){const auto base=domains[i].keyBase;
   if(key>=base&&uint64_t(key)-base<Span&&(key-base)%64==0)return int(i);
  }
  return -1;
 }
 const Entry* Lookup(uint32_t key) const noexcept {
  const auto domain=KeyDomain(key);return domain<0?nullptr:domains[unsigned(domain)].Lookup(key);
 }
 template<class Collision> uint32_t Observe(const native_projection66::Packet& packet,uint32_t frame,Collision collision) noexcept {
  if(!configured)return 0;
  auto& domain=Domain(packet);domain.snowMetadata70=snowMetadata70;domain.volumetricMetadata199=volumetricMetadata199;
  return domain.Observe(packet,frame,collision);
 }
 bool Validate(uint32_t key,const native_projection66::Packet& packet) const noexcept {
  const auto domain=KeyDomain(key);
  if(domain<0||unsigned(domain)!=DomainIndex(packet))return false;
  const auto* entry=Lookup(key);
  if(!entry||packet[24]!=key||(packet[18]&6u)!=4u)return false;
  auto normalized=packet;normalized[18]&=~4u;normalized[24]=0;
  return native_projection66::Eligible(normalized)&&entry->projection==native_projection66::Values(normalized,snowMetadata70,volumetricMetadata199);
 }
};
template<unsigned Capacity=4096> struct IssuedKeys {
 std::array<native_projection66::IssuedKeys<Capacity>,4> domains{};
 bool Publish(const Registry<Capacity>& registry105,uint32_t key) noexcept {
  const auto domain=registry105.KeyDomain(key);
  return domain>=0&&domains[unsigned(domain)].Publish(registry105.domains[unsigned(domain)].keyBase,key);
 }
 bool Contains(const Registry<Capacity>& registry105,uint32_t key) const noexcept {
  const auto domain=registry105.KeyDomain(key);
  return domain>=0&&domains[unsigned(domain)].Contains(registry105.domains[unsigned(domain)].keyBase,key);
 }
};
}
