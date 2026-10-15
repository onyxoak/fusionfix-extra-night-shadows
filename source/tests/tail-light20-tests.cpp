#include "TailLightPolicy20.hpp"
#include <cstdio>
#include <cstdlib>
#include <limits>
static unsigned checks;
#define CHECK(v) do{++checks;if(!(v)){std::fprintf(stderr,"FAIL %d %s\n",__LINE__,#v);std::abort();}}while(false)
static uint32_t Bits(float f){uint32_t bits;std::memcpy(&bits,&f,4);return bits;}
static tail_light20::Packet Native(){
 tail_light20::Packet p{};p.words={0x1000,0x2000,0x3000,0x4000,Bits(.7f),Bits(12.f),Bits(10.f),Bits(30.f),5,0,0x8001,0,0};
 p.matrix[0]=p.matrix[5]=p.matrix[10]=p.matrix[15]=1.f;p.position={2,4,6};p.direction={0,-1,0};p.color={1,.04f,.02f};return p;
}
int main(){
 const auto p=Native();
 for(auto site:{tail_light20::CombinedSite,tail_light20::SingleSite}){
  CHECK(tail_light20::Authorized(site,0x8000,0x4000,p));
  auto left=p;left.words[10]=0x8000;CHECK(tail_light20::Authorized(site,0x8000,0x4000,left));
  CHECK(tail_light20::NativeChannel(0x8000,left)==0);CHECK(tail_light20::NativeChannel(0x8000,p)==1);
  for(unsigned field:{0u,1u,2u,3u,10u,11u,12u}){auto q=p;q.words[field]=field==11||field==12?1:0;CHECK(!tail_light20::Authorized(site,0x8000,0x4000,q));}
  for(unsigned field=4;field<=7;++field)for(float value:{-1.f,std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()}){
   auto q=p;q.words[field]=Bits(value);CHECK(!tail_light20::Authorized(site,0x8000,0x4000,q));
  }
  for(unsigned field:{4u,5u}){auto q=p;q.words[field]=0;CHECK(!tail_light20::Authorized(site,0x8000,0x4000,q));}
  for(unsigned i=0;i<16;++i){auto q=p;q.matrix[i]=std::numeric_limits<float>::quiet_NaN();CHECK(!tail_light20::Authorized(site,0x8000,0x4000,q));}
  auto q=p;q.direction.y=1;CHECK(!tail_light20::Authorized(site,0x8000,0x4000,q));
  q=p;q.color.x=-1;CHECK(!tail_light20::Authorized(site,0x8000,0x4000,q));
  q=p;q.position.z=std::numeric_limits<float>::infinity();CHECK(!tail_light20::Authorized(site,0x8000,0x4000,q));
 }
 CHECK(!tail_light20::Authorized(0x63fe11,0x8000,0x4000,p));
 CHECK(!tail_light20::Authorized(tail_light20::CombinedSite,0,0x4000,p));
 // Lifetime evidence, not owner-pointer equivalence, authorizes queued maps.
 liberty20::VehicleLampRegistry<2,8> registry;CHECK(registry.Configure(0x100000));
 liberty20::VehicleIdentity a{0x100,0x8000,0x7000,0x8000,0,7,0x9000,0xa000},out{};
 const auto* first=registry.Observe(a,100);CHECK(first);const auto keys=first->keys;
 CHECK(registry.Resolve(keys[0],102,out)&&out==a);CHECK(!registry.Resolve(keys[0],103,out));
 registry.Free(a.pool,a.owner,a.slot);CHECK(!registry.Resolve(keys[0],100,out));CHECK(!registry.Resolve(keys[1],100,out));
 // Even a wrapped native reference cannot revive a key after observed free.
 const auto* wrapped=registry.Observe(a,101);CHECK(wrapped&&wrapped->keys!=keys);
 auto b=a;b.model++;const auto wrappedKeys=wrapped->keys;const auto* changed=registry.Observe(b,102);
 CHECK(changed&&changed->keys!=wrappedKeys);CHECK(!registry.Resolve(wrappedKeys[0],102,out));
 const auto changeKeys=changed->keys;const auto* rewind=registry.Observe(b,1);CHECK(rewind&&rewind->keys!=changeKeys);
 CHECK(!registry.Resolve(changeKeys[0],1,out));registry.Free(b.pool,b.owner,b.slot);
 CHECK(!registry.Observe(a,2));CHECK(!registry.Resolve(keys[0],2,out));
 std::printf("PASS native tail authorization and lifetime: %u checks. No live compatibility, allocation or receiver-coverage claim.\n",checks);
}
