#pragma once
// Independent immutable value ledgers. This partitions metadata, never maps,
// illumination packets, emitter lifetimes or geometry. No issued key is reused.
#include "NativeProjectionRegistry66.hpp"
namespace projection_domains101 {
inline constexpr uint32_t NativeVehicleStyle=0x100u;
template<unsigned Capacity=4096> struct Registry {
 using Ledger=native_projection66::Registry<Capacity>;
 using Entry=typename Ledger::Entry;
 Ledger vehicle{},world{};
 bool configured{},snowMetadata70{};
 static constexpr uint64_t Span=uint64_t(Capacity)*64;
 static bool Range(uint32_t base) noexcept {
  return base&&!(base&63u)&&uint64_t(base)+Span<=(uint64_t(1)<<32);
 }
 bool Init(uint32_t vehicleBase,uint32_t worldBase) noexcept {
  // Startup-only initialization cannot rebind an issued key or erase its
  // ownership classification after a later refused configuration request.
  if(configured)return vehicleBase==vehicle.keyBase&&worldBase==world.keyBase;
  if(!Range(vehicleBase)||!Range(worldBase)||
     !(uint64_t(vehicleBase)+Span<=worldBase||uint64_t(worldBase)+Span<=vehicleBase))return false;
  vehicle.Init(vehicleBase);world.Init(worldBase);
  vehicle.snowMetadata70=world.snowMetadata70=snowMetadata70;
  configured=true;return true;
 }
 static bool Vehicle(const native_projection66::Packet& packet) noexcept {
  return (packet[18]&NativeVehicleStyle)!=0;
 }
 Ledger& Domain(const native_projection66::Packet& packet) noexcept {return Vehicle(packet)?vehicle:world;}
 const Ledger& Domain(const native_projection66::Packet& packet) const noexcept {return Vehicle(packet)?vehicle:world;}
 uint32_t Used() const noexcept {return vehicle.used+world.used;}
 bool Full(const native_projection66::Packet& packet) const noexcept {return Domain(packet).used==Capacity;}
 int KeyDomain(uint32_t key) const noexcept {
  if(!configured)return -1;
  const auto inside=[key](uint32_t base){return key>=base&&uint64_t(key)-base<Span&&(key-base)%64==0;};
  return inside(vehicle.keyBase)?0:inside(world.keyBase)?1:-1;
 }
 const Entry* Lookup(uint32_t key) const noexcept {
  const auto domain=KeyDomain(key);return domain==0?vehicle.Lookup(key):domain==1?world.Lookup(key):nullptr;
 }
 template<class Collision> uint32_t Observe(const native_projection66::Packet& packet,uint32_t frame,Collision collision) noexcept {
  if(!configured)return 0;
  auto& domain=Domain(packet);domain.snowMetadata70=snowMetadata70;
  return domain.Observe(packet,frame,collision);
 }
 bool Validate(uint32_t key,const native_projection66::Packet& packet) const noexcept {
  const auto domain=KeyDomain(key);
  if(domain<0||bool(domain==0)!=Vehicle(packet))return false;
  const auto* entry=Lookup(key);
  if(!entry||packet[24]!=key||(packet[18]&6u)!=4u)return false;
  auto normalized=packet;normalized[18]&=~4u;normalized[24]=0;
  return native_projection66::Eligible(normalized)&&entry->projection==native_projection66::Values(normalized,snowMetadata70);
 }
};
template<unsigned Capacity=4096> struct IssuedKeys {
 native_projection66::IssuedKeys<Capacity> vehicle{},world{};
 bool Publish(const Registry<Capacity>& registry101,uint32_t key) noexcept {
  const auto domain=registry101.KeyDomain(key);
  return domain==0?vehicle.Publish(registry101.vehicle.keyBase,key):domain==1?world.Publish(registry101.world.keyBase,key):false;
 }
 bool Contains(const Registry<Capacity>& registry101,uint32_t key) const noexcept {
  const auto domain=registry101.KeyDomain(key);
  return domain==0?vehicle.Contains(registry101.vehicle.keyBase,key):domain==1?world.Contains(registry101.world.keyBase,key):false;
 }
};
}
