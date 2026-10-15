#pragma once
#include "TrafficSignalIdentity.hpp"
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
// Private model emitter identities. Never recycle a key within a process.
namespace model_light20 {
template<unsigned Capacity=32768> class Registry {
 static_assert(std::atomic_ref<uint32_t>::required_alignment<=alignof(uint32_t));
 static constexpr unsigned Cells=Capacity*2;
 std::array<uint32_t,Cells> cells{};
 // Index owner destruction separately from emitter lookup. Keys and entries
 // never recycle; chains retain retired entries but only visit this owner's
 // hash bucket, rather than every emitter ever seen on every pool free.
 std::array<uint32_t,Cells> ownerHeads{};
 std::array<uint32_t,Capacity> ownerNext{};
 // Storage allocation is not key ownership. Publish only after a successful
 // collision check; bits remain set after retirement so stale owned packets
 // still receive the full fresh validator. Queries occur outside our lock.
 // Keep the registry's existing externally serialized snapshot semantics.
 // Atomic references make unlocked consumer queries and publication race-free.
 mutable std::array<uint32_t,(Capacity+31)/32> issued{};
 unsigned KeyIndex(uint32_t key) const noexcept {
  if(!base||key<base||uint64_t(key)-base>=uint64_t(Capacity)*64||(key-base)%64)return Capacity;
  return (key-base)/64;
 }
 static unsigned Hash(uint32_t pool,uint32_t owner,uint32_t ordinal) noexcept {
  auto h=pool^(owner*0x9e3779b9u)^(ordinal*0x85ebca6bu);h^=h>>16;return h%Cells;
 }
public:
 struct Entry {
  traffic_signal::Identity id{};
  uint32_t ordinal{},descriptor{},modelInfo{},lastFrame{},key{};
  std::array<uint8_t,0x6c> bytes{};
  bool active{};
 };
 std::array<Entry,Capacity> entries{};
 uint32_t used{},base{};
 bool Configure(uint32_t value) noexcept {
  if(base||used||!value||(value&63)||uint64_t(value)+uint64_t(Capacity)*64>UINT32_MAX)return false;
  base=value;return true;
 }
 bool Contains(uint32_t key) const noexcept {
  const auto index=KeyIndex(key);if(index==Capacity)return false;
  return (std::atomic_ref<uint32_t>(issued[index/32]).load(std::memory_order_acquire)&(1u<<(index%32)))!=0;
 }
 Entry* Lookup(uint32_t key) noexcept {
  const auto index=KeyIndex(key);return index<used?&entries[index]:nullptr;
 }
 bool Publish(uint32_t key) noexcept {
  const auto index=KeyIndex(key);
  if(index>=used||!entries[index].active||entries[index].key!=key)return false;
  std::atomic_ref<uint32_t>(issued[index/32]).fetch_or(1u<<(index%32),std::memory_order_release);return true;
 }
 Entry* Observe(traffic_signal::Identity id,uint32_t ordinal,uint32_t descriptor,uint32_t modelInfo,
                const std::array<uint8_t,0x6c>& bytes,uint32_t frame) noexcept {
  if(!base||!id.pool||!id.owner||!id.storage||!id.flags||!id.model||!descriptor||!modelInfo||ordinal>65535)return nullptr;
  auto cell=Hash(id.pool,id.owner,ordinal);
  for(unsigned n=0;n<Cells;++n,cell=(cell+1)%Cells){
   if(cells[cell]){
    auto& e=entries[cells[cell]-1];
    if(e.id.pool!=id.pool||e.id.owner!=id.owner||e.ordinal!=ordinal)continue;
    // Visibility/submission gaps are not native owner destruction. The caller
    // freshly authenticates the complete pool identity and descriptor before
    // Observe. Keep a live emitter's non-recycled key across forward gaps;
    // Free, identity/descriptor changes and backwards time still retire it.
    if(e.active&&e.id==id&&e.descriptor==descriptor&&e.modelInfo==modelInfo&&e.bytes==bytes&&uint32_t(frame-e.lastFrame)<=0x7fffffffu){e.lastFrame=frame;return &e;}
    e.active=false;
   }
   if(used==Capacity)return nullptr;
   auto& e=entries[used];e={id,ordinal,descriptor,modelInfo,frame,base+used*64,bytes,true};
   const auto ownerCell=Hash(id.pool,id.owner,0);
   ownerNext[used]=ownerHeads[ownerCell];ownerHeads[ownerCell]=used+1;
   cells[cell]=++used;return &e;
  }
  return nullptr;
 }
 bool Validate(uint32_t key,traffic_signal::Identity id,const std::array<uint8_t,0x6c>& bytes,uint32_t frame) noexcept {
  auto* e=Lookup(key);if(!e||!e->active)return false;
  if(e->id!=id||e->bytes!=bytes){e->active=false;return false;}
  // A stale queued packet cannot authorize a shadow, but must not destroy the
  // emitter's ownership lifetime merely because it was not submitted recently.
  if(uint32_t(frame-e->lastFrame)>2)return false;
  return true;
 }
 void Free(uint32_t pool,uint32_t owner) noexcept {
  for(auto link=ownerHeads[Hash(pool,owner,0)];link;link=ownerNext[link-1]){
   auto& e=entries[link-1];
   if(e.id.pool==pool&&e.id.owner==owner)e.active=false;
  }
 }
};
}
