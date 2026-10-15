#pragma once
// OnyxOak / Liberty's Shadows private 2.0 candidate. GPL-3.0.
#include <array>
#include <cstdint>
#include <cmath>
namespace liberty20 {
struct VehicleIdentity {
    uint32_t pool{},storage{},flags{},owner{},slot{},reference{},modelInfo{},model{};
    bool operator==(const VehicleIdentity&) const = default;
};
struct Vec3 { float x{},y{},z{}; };
inline bool Finite(Vec3 p) noexcept {return std::isfinite(p.x)&&std::isfinite(p.y)&&std::isfinite(p.z);}
inline bool LampPositions(const std::array<float,16>& m,Vec3 a,Vec3 b,std::array<Vec3,2>& out) noexcept {
    for(float x:m)if(!std::isfinite(x))return false;
    if(!Finite(a)||!Finite(b))return false;
    auto transform=[&](Vec3 p){return Vec3{p.x*m[0]+p.y*m[4]+p.z*m[8]+m[12],
        p.x*m[1]+p.y*m[5]+p.z*m[9]+m[13],p.x*m[2]+p.y*m[6]+p.z*m[10]+m[14]};};
    out={transform(a),transform(b)};
    const auto dx=out[0].x-out[1].x,dy=out[0].y-out[1].y,dz=out[0].z-out[1].z;
    const auto separation=dx*dx+dy*dy+dz*dz;
    // Reject coincident/malformed attachments instead of inventing offsets.
    return Finite(out[0])&&Finite(out[1])&&separation>0.0001f&&separation<400.f;
}
template<size_t Slots=4096,size_t Keys=16384> class VehicleLampRegistry {
public:
    struct Pair {VehicleIdentity identity{};std::array<uint32_t,2> keys{};uint32_t frame{};bool alive{};};
    std::array<Pair,Slots> slots{};
    std::array<VehicleIdentity,Keys> issued{};
    std::array<bool,Keys> retired{};
    uint32_t base{},used{};
    bool Configure(uint32_t address) noexcept {
        if(!address||(address&63)||uint64_t(address)+uint64_t(Keys)*64>UINT32_MAX)return false;
        base=address;return true;
    }
    void Retire(uint32_t slot) noexcept {
        if(slot>=Slots||!slots[slot].alive)return;
        for(auto key:slots[slot].keys)retired[(key-base)/64]=true;
        slots[slot].alive=false;
    }
    const Pair* Observe(VehicleIdentity id,uint32_t frame) noexcept {
        if(!base||!id.owner||id.slot>=Slots||!id.modelInfo||!id.model)return nullptr;
        auto& pair=slots[id.slot];
        if(pair.alive&&!(pair.identity==id))Retire(id.slot);
        if(pair.alive&&uint32_t(frame-pair.frame)>=0x80000000u)Retire(id.slot);
        if(!pair.alive){
            if(used+2>Keys)return nullptr;
            pair={id,{base+used*64,base+(used+1)*64},frame,true};
            issued[used++]=id;issued[used++]=id;
        }
        pair.frame=frame;return &pair;
    }
    uint32_t Index(uint32_t key) const noexcept {
        if(key<base||(key-base)%64)return UINT32_MAX;
        const auto i=(key-base)/64;return i<used?i:UINT32_MAX;
    }
    bool Resolve(uint32_t key,uint32_t frame,VehicleIdentity& id) const noexcept {
        const auto index=Index(key);if(index==UINT32_MAX||retired[index])return false;
        id=issued[index];const auto& pair=slots[id.slot];
        return pair.alive&&pair.identity==id&&uint32_t(frame-pair.frame)<=2;
    }
    void Free(uint32_t pool,uint32_t owner,uint32_t slot) noexcept {
        if(slot<Slots&&slots[slot].alive&&slots[slot].identity.pool==pool&&slots[slot].identity.owner==owner)Retire(slot);
    }
};
}
