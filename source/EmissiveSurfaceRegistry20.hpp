#pragma once
#include "TrafficSignalIdentity.hpp"
#include <array>
#include <cstdint>
namespace emissive_surface20 {
// Independent family: geometry regions are not fabricated LightAttrs ordinals.
// Owner free uses the existing shared pool dispatcher. Keys never recycle.
template<unsigned Capacity=32768>class Registry {
 static constexpr unsigned Cells=Capacity*2;
 std::array<uint32_t,Cells> cells{},ownerHeads{};
 std::array<uint32_t,Capacity> ownerNext{};
 static unsigned Hash(uint32_t pool,uint32_t owner,uint32_t source)noexcept{
  uint32_t h=pool^(owner*0x9e3779b9u)^(source*0x85ebca6bu);h^=h>>16;return h%Cells;
 }
public:
 struct Entry {traffic_signal::Identity id{};uint32_t source{},modelInfo{},material{},revision{},lastFrame{},key{};bool active{};};
 std::array<Entry,Capacity> entries{};uint32_t used{},base{};
 bool Configure(uint32_t value)noexcept{
  if(base||used||!value||(value&63)||uint64_t(value)+uint64_t(Capacity)*64>UINT32_MAX)return false;base=value;return true;
 }
 bool Contains(uint32_t key)const noexcept{return base&&key>=base&&uint64_t(key)-base<uint64_t(Capacity)*64;}
 Entry* Lookup(uint32_t key)noexcept{
  if(!Contains(key)||(key-base)%64)return nullptr;auto i=(key-base)/64;return i<used?&entries[i]:nullptr;
 }
 Entry* Observe(traffic_signal::Identity id,uint32_t source,uint32_t info,uint32_t material,uint32_t revision,uint32_t frame)noexcept{
  if(!base||!id.pool||!id.owner||!id.storage||!id.flags||!id.model||!info||!material)return nullptr;
  auto cell=Hash(id.pool,id.owner,source);
  for(unsigned n=0;n<Cells;++n,cell=(cell+1)%Cells){
   if(cells[cell]){auto& e=entries[cells[cell]-1];if(e.id.pool!=id.pool||e.id.owner!=id.owner||e.source!=source)continue;
    if(e.active&&e.id==id&&e.modelInfo==info&&e.material==material&&e.revision==revision&&uint32_t(frame-e.lastFrame)<=2){e.lastFrame=frame;return &e;}e.active=false;
   }
   if(used==Capacity)return nullptr;
   auto& e=entries[used];e={id,source,info,material,revision,frame,base+used*64,true};
   auto ownerCell=Hash(id.pool,id.owner,0);ownerNext[used]=ownerHeads[ownerCell];ownerHeads[ownerCell]=used+1;cells[cell]=++used;return &e;
  }return nullptr;
 }
 bool Validate(uint32_t key,traffic_signal::Identity id,uint32_t info,uint32_t material,uint32_t revision,uint32_t frame)noexcept{
  auto* e=Lookup(key);if(!e||!e->active)return false;
  if(e->id!=id||e->modelInfo!=info||e->material!=material||e->revision!=revision||e->lastFrame!=frame){e->active=false;return false;}return true;
 }
 void Free(uint32_t pool,uint32_t owner)noexcept{
  for(auto i=ownerHeads[Hash(pool,owner,0)];i;i=ownerNext[i-1]){auto& e=entries[i-1];if(e.id.pool==pool&&e.id.owner==owner)e.active=false;}
 }
};
}
