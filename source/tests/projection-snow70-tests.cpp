#include "NativeProjectionRegistry66.hpp"
#include <cassert>
#include <iostream>
using namespace native_projection66;
Packet Source(uint32_t type=2,float radius=12.f,uint32_t flags=0){
 Packet p{};p[17]=type;p[18]=flags;p[21]=std::bit_cast<uint32_t>(radius);
 p[28]=std::bit_cast<uint32_t>(.2f);p[29]=std::bit_cast<uint32_t>(.3f);return p;
}
Packet Snow(Packet p,uint32_t key){p[18]|=12;p[24]=key;p[28]=std::bit_cast<uint32_t>(.8f);p[29]=std::bit_cast<uint32_t>(.9f);return p;}
int main(){
 for(float radius:{8.f,12.f,20.f}){
  Registry<16> r;r.Init(0x10000);r.snowMetadata70=true;auto p=Source(2,radius);
  const auto key=r.Observe(p,1,[](uint32_t){return false;});assert(key);
  auto q=Snow(p,key);const auto before=q;assert(r.Validate(key,q)&&q==before);
  AliasPass<8> a;a.Begin();auto original=p;original[18]|=4;original[24]=key;
  assert(a.Add(0,key,original,1.f,true)&&a.Add(1,key,q,2.f,true)&&a.IsAlias(0));
  Sidecar<8> s;s.Begin(0x20000,1,5);s.Own(0,q);s.Accept(0,false);
  assert(s.Current(0,q,0x20000,1,5,0)==Decision::Valid);
  auto changed=q;changed[28]^=1;assert(s.Current(0,changed,0x20000,1,5,0)==Decision::Invalid);
  changed=q;changed[8]=std::bit_cast<uint32_t>(1.f);assert(!r.Validate(key,changed));
  changed=q;changed[0]=std::bit_cast<uint32_t>(1.f);assert(!r.Validate(key,changed));
  changed=q;changed[18]^=0x400;assert(!r.Validate(key,changed));
 }
 // Native and unverified callback domains retain exact metadata identity.
 for(const auto p:{Source(),Source(0),Source(2,7.f),Source(2,21.f),Source(2,12.f,0x100)}){
  for(bool verified:{false,true}){
   Registry<16> r;r.Init(0x10000);r.snowMetadata70=verified;
   const auto key=r.Observe(p,1,[](uint32_t){return false;});assert(key);
   const bool permitted=verified&&p[17]==2&&p[21]==std::bit_cast<uint32_t>(12.f)&&p[18]==0;
   assert(r.Validate(key,Snow(p,key))==permitted);
  }
 }
 std::cout<<"snow70 scoped metadata, geometry authority and current witness passed\n";
}
