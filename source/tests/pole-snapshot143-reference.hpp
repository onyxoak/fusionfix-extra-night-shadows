// Liberty's Shadows targeted read-only caster diagnostic. GPL-3.0-or-later.
#pragma once
#include <array>
#include <cstdint>
#include <cstring>

namespace pole_caster134 {
inline constexpr uint32_t PoleHash=0x46c33c49u;
inline bool SupportedPole(uint32_t hash) noexcept {switch(hash){case 0x67105bd6u:case 0x46c33c49u:case 0xd7b45b49u:case 0x12272a5fu:case 0xba49a074u:case 0xbaa79cdau:case 0x3a8020ffu:case 0x70ab8d51u:case 0x9eed69d4u:case 0x97dfda59u:case 0x4af44083u:case 0x5b38e534u:case 0x6cb90840u:case 0x0b9145f2u:case 0x1cd7687eu:case 0x307b9012u:case 0x2d7e0a07u:case 0x2e9b0c05u:case 0x70a9d8e5u:case 0xbef47579u:case 0x1485e82fu:case 0xd4e8589du:case 0x9a978a05u:case 0x3d4e4f74u:case 0x0f1c59b2u:case 0x517fd33au:return true;default:return false;}}
struct Pool {uint32_t storage{},flags{};int32_t count{},stride{},unused{},unused2{};bool operator==(const Pool&) const=default;};
struct Snapshot {
 uint32_t entity{},pool{},slot{},reference{},modelIndex{},modelInfo{},modelHash{},vtable{};
 uint32_t flags24{},type28{},renderInstance34{},flags210{},associated1bc{},associated28{},instance280{},fragment8{},physical38{};
};
inline bool Address(uint64_t at,size_t size) noexcept {return at>=0x10000u&&size&&at+size<=0x100000000ull;}
template<class Read,class Frame> bool Capture(Read&& read,Frame&& frame,uint32_t game,uint32_t entity,uint32_t expectedFrame,Snapshot& out) noexcept {
 // Only the already-visited TYPE4 object pool, no pool scan or virtual call.
 out={};if(!Address(entity,0x284)||frame()!=expectedFrame)return false;
 uint32_t root{};Pool pool{};
 if(!read(uint64_t(game)+0x1232c60u,&root,4)||!Address(root,sizeof(pool))||!read(root,&pool,sizeof(pool))||
    pool.count<=0||pool.count>65536||pool.stride!=0x320||!Address(pool.storage,uint64_t(uint32_t(pool.count))*0x320)||
    !Address(pool.flags,uint32_t(pool.count))||entity<pool.storage)return false;
 const uint32_t delta=entity-pool.storage;if(delta%0x320||uint64_t(delta)>=uint64_t(uint32_t(pool.count))*0x320)return false;
 const uint32_t slot=delta/0x320;uint8_t ref{};std::array<uint8_t,0x3c> body{};
 if(!read(uint64_t(pool.flags)+slot,&ref,1)||(ref&0x80)||!read(entity,body.data(),body.size()))return false;
 uint32_t type{},vtable{},flags24{},renderInstance{},physical38{};int16_t index{};
 std::memcpy(&type,body.data()+0x28,4);std::memcpy(&index,body.data()+0x2e,2);
 std::memcpy(&vtable,body.data(),4);std::memcpy(&flags24,body.data()+0x24,4);std::memcpy(&renderInstance,body.data()+0x34,4);std::memcpy(&physical38,body.data()+0x38,4);
 if(((type>>6)&15)!=4||index<0||index>=31000)return false;
 uint32_t info{},hash{},fragment{},flags210{},associated{},associated28{},instance{};
 const uint64_t modelSlot=uint64_t(game)+0xe95cd8u+uint32_t(index)*4;
 if(!read(modelSlot,&info,4)||!Address(info,0x40)||!read(uint64_t(info)+0x3c,&hash,4)||!SupportedPole(hash)||
    !read(uint64_t(info)+8,&fragment,4)||!read(uint64_t(entity)+0x210,&flags210,4)||
    !read(uint64_t(entity)+0x1bc,&associated,4)||!read(uint64_t(entity)+0x280,&instance,4))return false;
 if(associated&&(!Address(associated,0x2c)||!read(uint64_t(associated)+0x28,&associated28,4)))return false;
 // A new frame, free/reused pool slot, changed native model or object relation
 // refuses publication. Sequential equality is not an atomic snapshot claim.
 uint32_t againRoot{},againInfo{},againHash{},againFragment{},again210{},againAssociated{},againInstance{},againAssociated28{};
 Pool againPool{};uint8_t againRef{};std::array<uint8_t,0x3c> againBody{};
 if(!read(uint64_t(game)+0x1232c60u,&againRoot,4)||againRoot!=root||!read(root,&againPool,sizeof(againPool))||!(againPool==pool)||
    !read(uint64_t(pool.flags)+slot,&againRef,1)||againRef!=ref||!read(entity,againBody.data(),againBody.size())||againBody!=body||
    !read(modelSlot,&againInfo,4)||againInfo!=info||!read(uint64_t(info)+0x3c,&againHash,4)||againHash!=hash||
    !read(uint64_t(info)+8,&againFragment,4)||againFragment!=fragment||!read(uint64_t(entity)+0x210,&again210,4)||again210!=flags210||
    !read(uint64_t(entity)+0x1bc,&againAssociated,4)||againAssociated!=associated||!read(uint64_t(entity)+0x280,&againInstance,4)||againInstance!=instance||
    (associated&&(!read(uint64_t(associated)+0x28,&againAssociated28,4)||againAssociated28!=associated28))||frame()!=expectedFrame)return false;
 out={entity,root,slot,ref,uint32_t(index),info,hash,vtable,flags24,type,renderInstance,flags210,associated,associated28,instance,fragment,physical38};return true;
}
}
