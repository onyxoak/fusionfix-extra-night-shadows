#include "NativeProjectionRegistry66.hpp"
#include <cassert>
#include <memory>
using namespace native_projection66;
Packet Light74(uint32_t type=0){
 Packet p{};p[17]=type;p[21]=std::bit_cast<uint32_t>(12.f);
 p[8]=std::bit_cast<uint32_t>(104.29008483886719f);
 p[9]=std::bit_cast<uint32_t>(850.5333862304688f);
 p[10]=std::bit_cast<uint32_t>(16.774398803710938f);
 p[16]=std::bit_cast<uint32_t>(1.f);return p;
}
int main(){
 static_assert(sizeof(Projection)==76&&sizeof(PacketWitness)==112);
 // Position-W samples observed at the stationary apartment source in clip70.
 constexpr uint32_t recordedW[]{1832768u,3957998429u,330770320u,297217728u,3957998397u};
 for(uint32_t type:{0u,2u}){
  Registry<16> r;r.Init(0x10000);auto p=Light74(type);
  const auto key=r.Observe(p,1,[](uint32_t){return false;});assert(key);
  for(uint32_t w:recordedW){auto q=p;q[11]=w;
   const auto original=q;assert(r.Observe(q,2,[](uint32_t){assert(false);return true;})==key);
   assert(q==original);q[24]=key;q[18]|=4;assert(r.Validate(key,q));
  }
  for(unsigned field:{3u,7u,11u,15u})for(uint32_t bits:{0u,0xffffffffu,0x7fc00001u,0x80000000u}){
   auto q=p;q[field]=bits;const auto original=q;
   assert(Values(q)==Values(p));assert(r.Observe(q,3,[](uint32_t){assert(false);return true;})==key&&q==original);
   q[24]=key;q[18]|=4;assert(r.Validate(key,q));
   Sidecar<8> s;s.Begin(0x20000,1,3);s.Own(0,q);s.Accept(0,false);
   assert(s.Current(0,q,0x20000,1,3,0)==Decision::Valid);
   q[field]^=1;assert(s.Current(0,q,0x20000,1,3,0)==Decision::Invalid);
  }
  assert(r.used==1);
  for(unsigned field:Fields){auto q=p;q[field]^=1;
   if(Eligible(q))assert(Values(q)!=Values(p));
   q[24]=key;q[18]|=4;assert(!r.Validate(key,q));
  }
 }
 // Ten seconds of512 stationary projectors: all four native W lanes vary
 // every frame, while exact projection and every independent packet survive.
 auto r=std::make_unique<Registry<>>();r->Init(0x300000);unsigned collisionChecks=0;
 for(unsigned frame=0;frame<600;++frame)for(unsigned source=0;source<512;++source){
  auto p=Light74(source&1?0u:2u);p[8]=std::bit_cast<uint32_t>(float(source));
  p[3]=frame*1664525u+source;p[7]=frame^source;p[11]=frame*2654435761u+source;p[15]=~frame+source;
  const auto original=p;const auto key=r->Observe(p,frame,[&](uint32_t){++collisionChecks;return false;});
  assert(key&&p==original);p[24]=key;p[18]|=4;assert(r->Validate(key,p));
 }
 assert(r->used==512&&collisionChecks==512);
 // True geometry changes still demand a new immutable key, never key reuse.
 auto moved=Light74();moved[8]=std::bit_cast<uint32_t>(999.f);
 const auto k1=r->Observe(moved,601,[](uint32_t){return false;});moved[0]=std::bit_cast<uint32_t>(1.f);
 const auto k2=r->Observe(moved,602,[](uint32_t){return false;});assert(k1&&k2&&k1!=k2);
}
