#include "ModelLightRegistry20.hpp"
#include <cassert>
int main(){
 using model_light20::Registry;
 Registry<8> r;assert(!r.Configure(65));assert(r.Configure(0x10000));assert(!r.Configure(0x20000));
 traffic_signal::Identity id{1,2,3,4,5,6,7};std::array<uint8_t,0x6c> bytes{};
 auto* e=r.Observe(id,0,10,11,bytes,100);assert(e);const auto first=e->key;
 assert(!r.Contains(first));assert(r.Publish(first));assert(r.Contains(first));
 for(uint32_t offset=1;offset<64;++offset)assert(!r.Contains(first+offset));
 assert(!r.Contains(first+64));assert(!r.Publish(first+64));
 assert(r.Observe(id,0,10,11,bytes,101)->key==first);
 auto* other=r.Observe(id,1,12,11,bytes,101);assert(other&&other->key!=first);
 assert(r.Validate(first,id,bytes,102));assert(!r.Lookup(first+1));assert(!r.Lookup(0x20000));
 r.Free(9,id.owner);assert(e->active);r.Free(id.pool,id.owner);assert(!e->active&&!other->active);
 assert(r.Contains(first));assert(!r.Contains(other->key));assert(!r.Publish(other->key));
 assert(!r.Validate(first,id,bytes,102));e=r.Observe(id,0,10,11,bytes,102);assert(e&&e->key!=first);
 const auto second=e->key;bytes[8]=1;e=r.Observe(id,0,10,11,bytes,102);assert(e&&e->key!=second);
 const auto third=e->key;assert(!r.Validate(third,id,bytes,105));
 e=r.Observe(id,0,10,11,bytes,105);assert(e&&e->key==third);
 const auto fourth=e->key;id.reference=7;e=r.Observe(id,0,10,11,bytes,105);assert(e&&e->key!=fourth);
 assert(!r.Validate(fourth,id,bytes,105));
 auto changed=id;changed.model=8;assert(!r.Validate(e->key,changed,bytes,105));
 assert(r.Observe(id,0,10,11,bytes,106));assert(r.Observe(id,1,12,11,bytes,106));
 assert(r.used==7);assert(r.Observe(id,2,14,11,bytes,106));assert(r.used==8);
 assert(!r.Observe(id,3,16,11,bytes,106));assert(r.used==8);
 Registry<2> wrap;assert(wrap.Configure(0x30000));auto* wrapped=wrap.Observe(id,0,10,11,bytes,UINT32_MAX);
 assert(wrapped&&wrap.Observe(id,0,10,11,bytes,0)->key==wrapped->key);assert(!wrap.Validate(wrapped->key,id,bytes,UINT32_MAX-1));
 // Thousands of offscreen/streaming visibility gaps cannot exhaust identities
 // for the SAME freshly authenticated live emitter. Stale packets stay refused.
 Registry<2> gaps;assert(gaps.Configure(0x40000));
 auto* stable=gaps.Observe(id,0,10,11,bytes,1);assert(stable);const auto stableKey=stable->key;
 for(uint32_t frame=101;frame<1000000;frame+=100){
  assert(!gaps.Validate(stableKey,id,bytes,frame));
  auto* fresh=gaps.Observe(id,0,10,11,bytes,frame);
  assert(fresh&&fresh->key==stableKey&&gaps.used==1);
  assert(gaps.Validate(stableKey,id,bytes,frame));
 }
 gaps.Free(id.pool,id.owner);assert(!gaps.Validate(stableKey,id,bytes,1000000));
 stable=gaps.Observe(id,0,10,11,bytes,1000000);assert(stable&&stable->key!=stableKey);
 assert(gaps.used==2); // Destruction still consumes a genuinely new lifetime.
}
