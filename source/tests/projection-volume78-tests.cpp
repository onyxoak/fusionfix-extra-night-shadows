#include "NativeProjectionRegistry66.hpp"
#include <cassert>
#include <memory>
using namespace native_projection66;
Packet Lamp78(unsigned source=0,uint32_t type=0){
 Packet p{};p[17]=type;p[21]=std::bit_cast<uint32_t>(18.f);
 // First group is the actual stationary family observed in the74 census.
 p[8]=std::bit_cast<uint32_t>(112.711777f+float(source));
 p[9]=std::bit_cast<uint32_t>(493.928345f);
 p[10]=std::bit_cast<uint32_t>(-1.597900f);
 p[16]=std::bit_cast<uint32_t>(1.f);p[18]=0x51u;return p;
}
int main(){
 static_assert(sizeof(Projection)==76&&sizeof(PacketWitness)==112);
 for(uint32_t type:{0u,2u})for(bool snow:{false,true}){
  Registry<16> r;r.Init(0x10000);r.snowMetadata70=snow;
  const auto original=Lamp78(0,type);const auto key=r.Observe(original,1,[](uint32_t){return false;});assert(key);
  for(uint32_t bits:{0u,0x3f800000u,0x3f000001u,0xffffffffu,0x7fc00001u,0x80000000u}){
   auto p=original;p[28]=bits;const auto before=p;
   assert(Values(p,snow)==Values(original,snow));
   assert(r.Observe(p,2,[](uint32_t){assert(false);return true;})==key&&p==before);
   p[18]|=4;p[24]=key;assert(r.Validate(key,p));
   Sidecar<8> s;s.Begin(0x20000,1,2);s.Own(0,p);s.Accept(0,false);
   assert(s.Current(0,p,0x20000,1,2,0)==Decision::Valid);
   p[28]^=1;assert(s.Current(0,p,0x20000,1,2,0)==Decision::Invalid);
   Packet copied{};const auto ret=ConstructOwned(copied.data(),[&](void* dst){std::memcpy(dst,before.data(),120);return 0xabcdu;},
    [](Packet&){},[](void* dst,const void* src){std::memcpy(dst,src,120);});
   assert(ret==0xabcdu&&copied[28]==bits&&std::memcmp(copied.data(),before.data(),120)==0);
  }
  assert(r.used==1);
  auto dim=original;dim[28]=std::bit_cast<uint32_t>(.1f);dim[24]=key;dim[18]|=4;
  auto bright=dim;bright[16]=std::bit_cast<uint32_t>(2.f);bright[28]=std::bit_cast<uint32_t>(.9f);
  const auto dimBefore=dim,brightBefore=bright;
  AliasPass<8> aliases;aliases.Begin();
  assert(aliases.Add(0,key,dim,1.f,snow)&&aliases.Add(1,key,bright,2.f,snow));
  assert(aliases.IsAlias(0)&&!aliases.IsAlias(1));
  assert(dim==dimBefore&&bright==brightBefore);
  for(unsigned field:Fields){auto changed=original;changed[field]^=1;
   if(Eligible(changed))assert(Values(changed,snow)!=Values(original,snow));
  }
 }
 // Animated emission does not consume historical keys for stationary lights.
 auto registry=std::make_unique<Registry<>>();registry->Init(0x300000);unsigned checks=0;
 for(unsigned frame=0;frame<600;++frame)for(unsigned source=0;source<512;++source){
  auto p=Lamp78(source,source%2?0u:2u);p[28]=std::bit_cast<uint32_t>(float(frame)/600.f);
  const auto before=p;const auto key=registry->Observe(p,frame,[&](uint32_t){++checks;return false;});
  assert(key&&p==before);p[24]=key;p[18]|=4;assert(registry->Validate(key,p));
 }
 assert(registry->used==512&&checks==512);
 // Real projection/participation changes retain separate immutable keys.
 auto changed=Lamp78();changed[8]=std::bit_cast<uint32_t>(9999.f);
 const auto first=registry->Observe(changed,601,[](uint32_t){return false;});
 changed[29]=std::bit_cast<uint32_t>(.5f);
 const auto next=registry->Observe(changed,602,[](uint32_t){return false;});
 assert(first&&next&&first!=next);
}
