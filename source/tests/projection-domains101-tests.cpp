#include "ProjectionDomains101.hpp"
#include <cassert>
#include <cstdio>
#include <limits>
using native_projection66::Packet;
#include "RecordedWorldPackets101.hpp"
static projection_domains101::Registry<> domains;
static projection_domains101::IssuedKeys<> issued;
static native_projection66::Registry<> parent;
static Packet Light(bool vehicle,unsigned index=0) {
 Packet p{};p[2]=std::bit_cast<uint32_t>(-1.f);p[5]=std::bit_cast<uint32_t>(1.f);
 p[8]=std::bit_cast<uint32_t>(float(index));p[9]=std::bit_cast<uint32_t>(2.f);p[10]=std::bit_cast<uint32_t>(3.f);
 p[12]=p[13]=p[14]=std::bit_cast<uint32_t>(1.f);p[16]=std::bit_cast<uint32_t>(8.f);
 p[17]=vehicle?2u:0u;p[18]=vehicle?0x160u:0x51u;p[21]=std::bit_cast<uint32_t>(14.f);
 p[25]=UINT32_MAX;p[26]=UINT32_MAX;p[27]=0;p[29]=std::bit_cast<uint32_t>(.4f);return p;
}
static uint32_t Publish(Packet& p,unsigned frame) {
 const auto before=p;const auto key=domains.Observe(p,frame,[](uint32_t){return false;});
 if(key){assert(issued.Publish(domains,key));p[24]=key;p[18]|=4u;}
 for(unsigned i=0;i<p.size();++i)if(i!=18&&i!=24)assert(p[i]==before[i]);return key;
}
int main(){
 assert(domains.Init(0x10000000,0x20000000));parent.Init(0x30000000);
 for(unsigned i=0;i<4096;++i){auto p=Light(true,i),old=p;assert(Publish(p,1));assert(parent.Observe(old,1,[](uint32_t){return false;}));assert(domains.Validate(p[24],p));}
 assert(domains.vehicle.used==4096&&domains.world.used==0&&domains.Used()==4096);
 auto beyond=Light(true,4096);assert(!Publish(beyond,2)&&!beyond[24]);
 auto repeated=Light(true,55);assert(Publish(repeated,2)==0x10000000+55*64);
 native_projection66::Sidecar<> sidecar;native_projection66::AliasPass<> aliases;
 sidecar.Begin(0x50000000,6,7);aliases.Begin();
 std::array<Packet,6> packets{};
 for(unsigned i=0;i<5;++i){auto p=RecordedWorldPackets101[i];
  auto old=p;assert(!parent.Observe(old,7,[](uint32_t){return false;}));assert(Publish(p,7));
  assert(domains.Validate(p[24],p));sidecar.Own(i,p);assert(aliases.Add(i,p[24],p));sidecar.Accept(i,false);packets[i]=p;
  assert(sidecar.Current(i,p,0x50000000,6,7,0x60000000)==native_projection66::Decision::Valid);
 }
 // RGB/intensity aliases remain independent, sharing the exact same depth key.
 packets[5]=packets[0];packets[5][24]=0;packets[5][18]&=~4u;packets[5][12]=std::bit_cast<uint32_t>(.75f);packets[5][16]=std::bit_cast<uint32_t>(12.f);
 assert(Publish(packets[5],7)==packets[0][24]);sidecar.Own(5,packets[5]);assert(aliases.Add(5,packets[5][24],packets[5]));assert(aliases.IsAlias(5));sidecar.Accept(5,true);
 assert(sidecar.Current(5,packets[5],0x50000000,6,7,0x60000000)==native_projection66::Decision::Alias);
 assert(domains.world.used==5&&domains.vehicle.used==4096);
 // Fill only remaining world slots; both old capacities remain unchanged.
 for(unsigned i=5;i<4096;++i){auto p=Light(false,i+10000);assert(Publish(p,8));}
 assert(domains.Used()==8192&&domains.world.used==4096);
 auto exhausted=Light(false,90000);assert(!Publish(exhausted,9));auto current=packets[0];current[24]=0;current[18]&=~4u;assert(Publish(current,9)==packets[0][24]);
 // Ownership is successful issuance, never an allocated numeric range.
 projection_domains101::Registry<16> small;projection_domains101::IssuedKeys<16> smallIssued;
 assert(small.Init(0x40000000,0x50000000));
 for(bool vehicle:{false,true}){auto p=Light(vehicle);const auto base=vehicle?0x40000000u:0x50000000u;
  assert(!small.Observe(p,1,[](uint32_t){return true;}));assert(!smallIssued.Contains(small,base));assert(!small.Lookup(base));
  assert(!small.Observe(p,2,[](uint32_t){return false;}));p[8]=std::bit_cast<uint32_t>(1.f);
  const auto key=small.Observe(p,2,[](uint32_t){return false;});assert(key==base+64&&smallIssued.Publish(small,key));
  assert(!smallIssued.Contains(small,base+128)&&smallIssued.Contains(small,key));p[24]=key;p[18]|=4;assert(small.Validate(key,p));
  p[18]^=0x100u;assert(!small.Validate(key,p));
 }
 assert(!small.Init(0x60000000,0x70000000)&&small.configured); // no rebinding/lost issued authority
 for(unsigned bad:{0u,0x10000001u,0xfffffff0u}){projection_domains101::Registry<16> invalid;assert(!invalid.Init(bad,0x20000000));assert(!invalid.configured);}
 projection_domains101::Registry<16> overlap;assert(!overlap.Init(0x10000000,0x10000040));
 projection_domains101::Registry<16> edge;assert(edge.Init(0xfffffc00,0x10000000));
 // Every malformed value and shadow/native-key gate matches unchanged66.
 for(unsigned changed:{0u,1u,2u,4u,5u,6u,8u,9u,10u,12u,13u,14u,16u,21u,22u,23u}){
  auto p=Light(false);p[changed]=0x7fc00000;projection_domains101::Registry<16> a;native_projection66::Registry<16> b;
  assert(a.Init(0x10000000,0x20000000));b.Init(0x30000000);assert(!a.Observe(p,1,[](uint32_t){return false;}));assert(!b.Observe(p,1,[](uint32_t){return false;}));
 }
 for(unsigned flag:{2u,4u,6u}){auto p=Light(false);p[18]|=flag;assert(!domains.Observe(p,1,[](uint32_t){return false;}));}
 // Signed zero, other flags, cones, rooms and directions remain exact identity.
 projection_domains101::Registry<16> exact;assert(exact.Init(0x60000000,0x70000000));auto p=Light(false);
 const auto first=exact.Observe(p,1,[](uint32_t){return false;});assert(first);p[0]=0x80000000;assert(exact.Observe(p,1,[](uint32_t){return false;})!=first);
 // AuthenticatedFF snow metadata affects ONLY world-scoped type2 map identity;
 // postcallback packet is still witnessed in full and mutation is refused.
 projection_domains101::Registry<16> snow;snow.snowMetadata70=true;assert(snow.Init(0x60000000,0x70000000));
 p=Light(false);p[17]=2;p[18]=0x41;const auto snowKey=snow.Observe(p,1,[](uint32_t){return false;});assert(snowKey);
 p[24]=snowKey;p[18]|=12;p[28]=std::bit_cast<uint32_t>(.75f);p[29]=std::bit_cast<uint32_t>(.25f);assert(snow.Validate(snowKey,p));
 native_projection66::Sidecar<> witness;witness.Begin(0x1000,1,1);witness.Own(0,p);witness.Accept(0,false);
 assert(witness.Current(0,p,0x1000,1,1,0x2000)==native_projection66::Decision::Valid);p[28]^=1;
 assert(witness.Current(0,p,0x1000,1,1,0x2000)==native_projection66::Decision::Invalid);
 std::puts("projection101 PASS: moving4096 unchanged; five capturedworld packets admitted in offline fixture; world4096 unchanged; native7maps unchanged; aliases, snow, fullwitness, collision/issued/overflow/NaN gates preserved");return 0;
}
