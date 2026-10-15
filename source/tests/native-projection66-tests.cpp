#include "NativeProjectionRegistry66.hpp"
#include <cassert>
#include <cstring>
#include <limits>
#include <memory>
using namespace native_projection66;
Packet Light(){Packet p{};for(unsigned i=0;i<30;++i)p[i]=std::bit_cast<uint32_t>(float(i+1));p[17]=2;p[18]=0x260;p[21]=std::bit_cast<uint32_t>(25.f);p[22]=std::bit_cast<uint32_t>(0.95f);p[23]=std::bit_cast<uint32_t>(0.7f);p[24]=0;p[25]=UINT32_MAX;p[26]=UINT32_MAX;p[27]=3;return p;}
int main(){
 static_assert(sizeof(Projection)==76);static_assert(sizeof(PacketWitness)==112);
 Registry<64> registry;registry.Init(0x100000);
 IssuedKeys<64> ownership;
 assert(!ownership.Contains(0x100000,0x100000));
 assert(!ownership.Contains(0x100000,0x100040));
 assert(ownership.Publish(0x100000,0x100040));
 assert(ownership.Contains(0x100000,0x100040)&&!ownership.Contains(0x100000,0x100000));
 assert(!ownership.Publish(0x100000,0x100041)&&!ownership.Publish(0x100000,0x101000));
 const auto native=Light();assert(Eligible(native));
 auto key=registry.Observe(native,10,[](uint32_t){return false;});assert(key);
 assert(registry.Observe(native,11,[](uint32_t){assert(false);return true;})==key);
 auto accepted=native;accepted[24]=key;accepted[18]|=4;
 assert(registry.Validate(key,accepted)); // Native-current bank, no guessed frame lease.
 for(auto field:Fields){auto changed=native;changed[field]^=1;
  if(Eligible(changed)){const auto other=registry.Observe(changed,12,[](uint32_t){return false;});assert(other&&other!=key);}
  auto changedPacket=accepted;changedPacket[field]^=1;assert(!registry.Validate(key,changedPacket));
 }
 for(unsigned field:{24u,25u,30u,31u}){auto p=accepted;p[field]^=1;assert((field==24)?!registry.Validate(key,p):registry.Validate(key,p));}
 // Photometry remains conservative identity but separate native packets survive.
 AliasPass<> alias;alias.Begin();assert(alias.Add(0,key,accepted));assert(alias.Add(1,key,accepted));assert(alias.IsAlias(1));
 auto different=accepted;different[12]^=1;assert(alias.Add(2,key,different,10.f));assert(!alias.IsAlias(2));assert(alias.IsAlias(0)&&alias.IsAlias(1));
 for(auto field:{12u,13u,14u,16u}){auto p=native;p[field]^=1;assert(registry.Observe(p,13,[](uint32_t){return false;})==key);
  p[24]=key;p[18]|=4;assert(registry.Validate(key,p));assert(Witness(p)!=Witness(accepted));}
 Sidecar<> pass;pass.Begin(0x1234,2,10);pass.Own(0,accepted);pass.Own(1,accepted);
 assert(pass.Current(0,accepted,0x1234,2,10,0x5555)==Decision::Invalid);
 pass.Accept(0,false);pass.Accept(1,true);
 assert(pass.Current(0,accepted,0x1234,2,10,0x5555)==Decision::Valid);
 assert(pass.Current(1,accepted,0x1234,2,10,0x5555)==Decision::Alias);
 assert(pass.Current(0,accepted,0x1234,2,11,0x5555)==Decision::Invalid);
 assert(pass.Current(0,accepted,0x1234,2,10,0x5554)==Decision::Invalid);
 assert(pass.Current(0,different,0x1234,2,10,0x5555)==Decision::Invalid);
 pass.Begin(0x1234,2,10);assert(pass.Current(0,accepted,0x1234,2,10,0)==Decision::Invalid);
 Registry<2> small;small.Init(0x200000);assert(!small.Observe(native,1,[](uint32_t){return true;}));
 assert(!small.Observe(native,2,[](uint32_t){return false;})); // Burned collision never reused.
 auto second=native;second[8]^=1;assert(small.Observe(second,2,[](uint32_t){return false;})==0x200040);
 auto third=native;third[8]^=2;assert(!small.Observe(third,3,[](uint32_t){return false;}));
 for(auto field:{0u,4u,8u,12u,16u,21u,22u,23u}){auto bad=native;bad[field]=0x7fc00001;assert(!Eligible(bad));}
 // Ten seconds of512stationary sources with continuous authored photometry:
 // depth identity stays512, not307200; each packet still keeps fresh112witness.
 auto churn=std::make_unique<Registry<>>();churn->Init(0x300000);
 unsigned collisionChecks=0,issued=0,exhausted=0;
 for(unsigned frame=0;frame<600;++frame)for(unsigned source=0;source<512;++source){auto p=native;
  p[8]=std::bit_cast<uint32_t>(float(source));p[16]=std::bit_cast<uint32_t>(1.f+float(frame)*0.001f);
  if(churn->Observe(p,frame,[&](uint32_t){++collisionChecks;return false;}))++issued;else ++exhausted;
 }
 assert(churn->used==512&&issued==307200&&collisionChecks==512&&exhausted==0);
 // Real projector motion still produces distinct immutable geometry tokens.
 for(unsigned frame=1;frame<12;++frame)for(unsigned source=0;source<512;++source){auto p=native;
  p[8]=std::bit_cast<uint32_t>(float(source)+float(frame)*0.001f);
  if(!churn->Observe(p,frame,[&](uint32_t){++collisionChecks;return false;}))++exhausted;
 }
 assert(churn->used==4096&&collisionChecks==4096&&exhausted>0); // Exact68 fallback required; never recycle.
 const auto burned=churn->used;auto unobserved=native;unobserved[8]=std::bit_cast<uint32_t>(1000000.f);assert(!churn->Observe(unobserved,601,[](uint32_t){return false;}));assert(churn->used==burned);
 // Actual production transaction preserves every native field, including W,
 // independent RGB/intensity, room, texture, volume and the native EAX result.
 for(bool promote:{false,true}){Packet destination;destination.fill(0xdeadbeef);unsigned originals=0,assigns=0;
  auto result=ConstructOwned(destination.data(),[&](void* owned){++originals;assert(owned!=destination.data());std::memcpy(owned,native.data(),0x78);return 0xabcdef01u;},
   [&](Packet& owned){if(promote){owned[24]=key;owned[18]|=4;}},
   [&](void* output,const void* owned){++assigns;assert(output==destination.data());std::memcpy(output,owned,0x78);});
  assert(originals==1&&assigns==1&&result==0xabcdef01u);
  for(unsigned i=0;i<30;++i)assert(destination[i]==((promote&&i==24)?key:(promote&&i==18)?native[i]|4u:native[i]));
  assert(destination[30]==0xdeadbeef&&destination[31]==0xdeadbeef);
 }
}
