#include "ModelLightRegistry20.hpp"
#include <cassert>
#include <array>
int main(){
 model_light20::Registry<65> registry;
 assert(!registry.Contains(0));assert(registry.Configure(0x30000000));
 traffic_signal::Identity owner{1,2,3,4,5,6,7};std::array<uint8_t,0x6c> descriptor{};
 for(unsigned i=0;i<65;++i){
  const auto key=0x30000000u+i*64;
  assert(!registry.Contains(key));assert(!registry.Publish(key));
  auto* entry=registry.Observe(owner,i,100+i,200,descriptor,7);assert(entry&&entry->key==key);
  assert(!registry.Contains(key));
  // A collision burns allocated storage but grants no authority over native keys.
  if(i==1){entry->active=false;assert(!registry.Publish(key));continue;}
  assert(registry.Publish(key));assert(registry.Contains(key));
  for(unsigned offset=1;offset<64;++offset)assert(!registry.Contains(key+offset));
 }
 assert(!registry.Contains(0x30000000u-64));assert(!registry.Contains(0x30000000u+65*64));
 assert(!registry.Publish(0x30000000u+65*64));
 // Native destruction retires validation authority, not key ownership.
 registry.Free(owner.pool,owner.owner);
 for(unsigned i=0;i<65;++i){
  const auto key=0x30000000u+i*64;
  assert(registry.Contains(key)==(i!=1));
  assert(!registry.Validate(key,owner,descriptor,7));assert(!registry.Publish(key));
 }
 // Copying an externally serialized registry snapshot retains exact issuance.
 auto snapshot=registry;
 for(unsigned i=0;i<65;++i)assert(snapshot.Contains(0x30000000u+i*64)==(i!=1));
}
