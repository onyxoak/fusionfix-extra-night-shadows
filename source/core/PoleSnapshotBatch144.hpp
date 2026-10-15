// Batch permissions/copies only; retain all original used-field repeat guards.
#pragma once
#include "PoleCasterSnapshot134.hpp"
namespace pole_batch144 {
using namespace pole_caster134;
template<class Read> bool Body(Read&& read,uint32_t entity,std::array<uint8_t,0x3c>& prefix,uint32_t& flags,uint32_t& associated,uint32_t& instance) noexcept {
 std::array<uint8_t,0x284> bytes{};
 if(read(entity,bytes.data(),bytes.size())) {
  std::memcpy(prefix.data(),bytes.data(),prefix.size());
  std::memcpy(&flags,bytes.data()+0x210,4);std::memcpy(&associated,bytes.data()+0x1bc,4);std::memcpy(&instance,bytes.data()+0x280,4);return true;
 }
 // Wider spans may cross unreadable unused bytes. Original sparse admission
 // remains authoritative; do not reject formerly valid fields for this reason.
 return read(entity,prefix.data(),prefix.size())&&read(uint64_t(entity)+0x210,&flags,4)&&read(uint64_t(entity)+0x1bc,&associated,4)&&read(uint64_t(entity)+0x280,&instance,4);
}
template<class Read> bool Model(Read&& read,uint32_t info,uint32_t& hash,uint32_t& fragment) noexcept {
 std::array<uint8_t,0x40> bytes{};
 if(read(info,bytes.data(),bytes.size())){std::memcpy(&hash,bytes.data()+0x3c,4);std::memcpy(&fragment,bytes.data()+8,4);return true;}
 return read(uint64_t(info)+0x3c,&hash,4)&&read(uint64_t(info)+8,&fragment,4);
}
template<class Read,class Frame> bool Capture(Read&& read,Frame&& frame,uint32_t game,uint32_t entity,uint32_t expectedFrame,Snapshot& out) noexcept {
 out={};if(!Address(entity,0x284)||frame()!=expectedFrame)return false;
 uint32_t root{};Pool pool{};
 if(!read(uint64_t(game)+0x1232c60u,&root,4)||!Address(root,sizeof(pool))||!read(root,&pool,sizeof(pool))||
 pool.count<=0||pool.count>65536||pool.stride!=0x320||!Address(pool.storage,uint64_t(uint32_t(pool.count))*0x320)||!Address(pool.flags,uint32_t(pool.count))||entity<pool.storage)return false;
 const uint32_t delta=entity-pool.storage;if(delta%0x320||uint64_t(delta)>=uint64_t(uint32_t(pool.count))*0x320)return false;
 const uint32_t slot=delta/0x320;uint8_t ref{};std::array<uint8_t,0x3c> body{};uint32_t flags210{},associated{},instance{};
 if(!read(uint64_t(pool.flags)+slot,&ref,1)||(ref&0x80)||!Body(read,entity,body,flags210,associated,instance))return false;
 uint32_t type{},vtable{},flags24{},renderInstance{},physical38{};int16_t index{};
 std::memcpy(&type,body.data()+0x28,4);std::memcpy(&index,body.data()+0x2e,2);std::memcpy(&vtable,body.data(),4);
 std::memcpy(&flags24,body.data()+0x24,4);std::memcpy(&renderInstance,body.data()+0x34,4);std::memcpy(&physical38,body.data()+0x38,4);
 if(((type>>6)&15)!=4||index<0||index>=31000)return false;
 uint32_t info{},hash{},fragment{},associated28{};const uint64_t modelSlot=uint64_t(game)+0xe95cd8u+uint32_t(index)*4;
 if(!read(modelSlot,&info,4)||!Address(info,0x40)||!Model(read,info,hash,fragment)||!SupportedPole(hash))return false;
 if(associated&&(!Address(associated,0x2c)||!read(uint64_t(associated)+0x28,&associated28,4)))return false;
 uint32_t againRoot{},againInfo{},againHash{},againFragment{},again210{},againAssociated{},againInstance{},againAssociated28{};
 Pool againPool{};uint8_t againRef{};std::array<uint8_t,0x3c> againBody{};
 if(!read(uint64_t(game)+0x1232c60u,&againRoot,4)||againRoot!=root||!read(root,&againPool,sizeof(againPool))||!(againPool==pool)||
 !read(uint64_t(pool.flags)+slot,&againRef,1)||againRef!=ref||!Body(read,entity,againBody,again210,againAssociated,againInstance)||againBody!=body||
 !read(modelSlot,&againInfo,4)||againInfo!=info||!Model(read,info,againHash,againFragment)||againHash!=hash||againFragment!=fragment||
 again210!=flags210||againAssociated!=associated||againInstance!=instance||
 (associated&&(!read(uint64_t(associated)+0x28,&againAssociated28,4)||againAssociated28!=associated28))||frame()!=expectedFrame)return false;
 out={entity,root,slot,ref,uint32_t(index),info,hash,vtable,flags24,type,renderInstance,flags210,associated,associated28,instance,fragment,physical38};return true;
}
}
