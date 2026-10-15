// Read-only current fragment prerequisites; never changes traversal or draws.
#pragma once
#include "PoleCasterSnapshot134.hpp"
namespace pole_children135 {
struct Child {uint32_t address{},drawable90{},drawable94{},flags0c{};bool operator==(const Child&)const=default;};
struct Snapshot {uint32_t fragment{},root{},children{},alternate{};uint8_t count{};std::array<Child,16> child{};bool operator==(const Snapshot&)const=default;};
template<class Read> bool Once(Read&& read,uint32_t fragment,Snapshot& out) noexcept {
 out={};out.fragment=fragment;
 if(!pole_caster134::Address(fragment,0x1f4)||!read(uint64_t(fragment)+0xb4,&out.root,4)||
 !read(uint64_t(fragment)+0xc8,&out.alternate,4)||!read(uint64_t(fragment)+0xd4,&out.children,4)||
 !read(uint64_t(fragment)+0x1f3,&out.count,1)||out.count>16)return false;
 if(out.count&&!pole_caster134::Address(out.children,size_t(out.count)*4))return false;
 for(uint32_t i=0;i<out.count;++i){auto& c=out.child[i];
 if(!read(uint64_t(out.children)+i*4,&c.address,4)||!pole_caster134::Address(c.address,0x98)||
 !read(uint64_t(c.address)+0x90,&c.drawable90,4)||!read(uint64_t(c.address)+0x94,&c.drawable94,4)||
 !read(uint64_t(c.address)+0xc,&c.flags0c,4))return false;
 }return true;
}
template<class Read> bool Capture(Read&& read,uint32_t fragment,Snapshot& out) noexcept {
 Snapshot first{},second{};if(!Once(read,fragment,first)||!Once(read,fragment,second)||!(first==second)){out={};return false;}out=first;return true;
}
}
