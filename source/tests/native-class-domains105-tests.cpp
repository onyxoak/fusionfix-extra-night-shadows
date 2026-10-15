#include "NativeClassDomains105.hpp"
#include "ProjectionDomains101.hpp"
#include "CapturedWorldValues105.hpp"
#include "CapturedTargetPackets105.hpp"
#include <cassert>
#include <cstdio>
using native_projection66::Packet;
static native_class_domains105::Registry<> candidate;
static native_class_domains105::IssuedKeys<> issued;
static projection_domains101::Registry<> parent;
static constexpr std::array<uint32_t,4> Bases{0x10000000,0x20000000,0x30000000,0x40000000};
static Packet Light(unsigned domain,unsigned index) {
 Packet p{};p[2]=std::bit_cast<uint32_t>(-1.f);p[5]=std::bit_cast<uint32_t>(1.f);
 p[8]=std::bit_cast<uint32_t>(float(index));p[9]=std::bit_cast<uint32_t>(2.f);p[10]=std::bit_cast<uint32_t>(3.f);
 p[12]=p[13]=p[14]=p[16]=std::bit_cast<uint32_t>(1.f);
 p[17]=0;p[18]=0x51u|(domain<<8);p[21]=std::bit_cast<uint32_t>(14.f);
 p[25]=UINT32_MAX;p[26]=UINT32_MAX;p[29]=std::bit_cast<uint32_t>(.4f);return p;
}
static uint32_t Publish(Packet& p,unsigned frame) {
 const auto before=p;const auto key=candidate.Observe(p,frame,[](uint32_t){return false;});
 if(key){assert(issued.Publish(candidate,key));p[24]=key;p[18]|=4u;assert(candidate.Validate(key,p));}
 for(unsigned i=0;i<p.size();++i)if(i!=18&&i!=24)assert(p[i]==before[i]);return key;
}
int main() {
 candidate.snowMetadata70=parent.snowMetadata70=true;
 assert(candidate.Init(Bases));assert(parent.Init(0x50000000,0x60000000));
 // Exact immutable identities from the authenticated full101 world ledger.
 // Excluded photometry is reconstructed only for this registry fixture.
 for(const auto& values:CapturedWorldValues105){auto p=Light(0,0);
  for(unsigned j=0;j<native_projection66::Fields.size();++j)p[native_projection66::Fields[j]]=values[j];
  assert(native_projection66::Values(p,true)==values);assert(native_projection66::Eligible(p));
  auto old=p;assert(parent.Observe(old,1,[](uint32_t){return false;}));assert(Publish(p,1));
 }
 assert(parent.world.used==4096&&candidate.domains[0].used==2214&&candidate.domains[2].used==1882);
 assert(candidate.domains[1].used==0&&candidate.domains[3].used==0);
 // Fill only the documented competing class200; ordinary class0 stays open.
 for(unsigned j=0;candidate.domains[2].used<4096;++j){auto p=Light(2,1000000+j);assert(Publish(p,2));}
 auto rejected=Light(2,2000000);assert(!Publish(rejected,3)&&!rejected[24]);
 native_projection66::Sidecar<> sidecar;native_projection66::AliasPass<> aliases;
 constexpr auto Count=unsigned(CapturedTargetPackets105.size()+1);
 sidecar.Begin(0x50000000,Count,7);aliases.Begin();std::array<Packet,Count> current{};
 for(unsigned j=0;j<CapturedTargetPackets105.size();++j){auto p=CapturedTargetPackets105[j];assert(!p[24]);
  auto old=p;assert(!parent.Observe(old,7,[](uint32_t){return false;}));assert(Publish(p,7));
  sidecar.Own(j,p);assert(aliases.Add(j,p[24],p,1.f,true));sidecar.Accept(j,false);current[j]=p;
  assert(sidecar.Current(j,p,0x50000000,Count,7,0x60000000)==native_projection66::Decision::Valid);
 }
 // Photometry remains an independent packet and an exact shared projector.
 auto alias=CapturedTargetPackets105[0];alias[12]=std::bit_cast<uint32_t>(.75f);alias[16]=std::bit_cast<uint32_t>(12.f);
 assert(Publish(alias,7)==current[0][24]);const auto aliasIndex=Count-1;
 sidecar.Own(aliasIndex,alias);assert(aliases.Add(aliasIndex,alias[24],alias,1.f,true));sidecar.Accept(aliasIndex,true);
 assert(aliases.IsAlias(aliasIndex));
 assert(sidecar.Current(aliasIndex,alias,0x50000000,Count,7,0x60000000)==native_projection66::Decision::Alias);
 alias[28]^=1;assert(sidecar.Current(aliasIndex,alias,0x50000000,Count,7,0x60000000)==native_projection66::Decision::Invalid);
 // Radius and retained native flags remain separate exact projections.
 auto variant=CapturedTargetPackets105[0];const auto first=current[0][24];variant[21]=std::bit_cast<uint32_t>(27.f);
 assert(Publish(variant,7)!=first);variant=CapturedTargetPackets105[0];variant[18]^=0x10u;assert(Publish(variant,7)!=first);
 // No class sacrifices the previous4096 capacity; all four full fail safely.
 for(unsigned d=0;d<4;++d){
  for(unsigned j=0;candidate.domains[d].used<4096;++j){auto p=Light(d,3000000+j);assert(Publish(p,8));}
  auto p=Light(d,4000000);assert(!Publish(p,9));
 }
 assert(candidate.Used()==16384);auto stable=CapturedTargetPackets105[0];assert(Publish(stable,10)==first);
 for(unsigned d=1;d<4;++d){auto mutated=current[0];mutated[18]^=d<<8;assert(!candidate.Validate(first,mutated));}
 // Collision-burned and unused ranges are never owned in ANY class.
 native_class_domains105::Registry<16> small;native_class_domains105::IssuedKeys<16> smallIssued;
 assert(small.Init(Bases));
 for(unsigned d=0;d<4;++d){auto p=Light(d,0);const auto base=Bases[d];
  assert(!small.Observe(p,1,[](uint32_t){return true;}));assert(!small.Lookup(base)&&!smallIssued.Contains(small,base));
  assert(!small.Observe(p,2,[](uint32_t){return false;}));p[8]=std::bit_cast<uint32_t>(1.f);
  const auto key=small.Observe(p,2,[](uint32_t){return false;});assert(key==base+64&&smallIssued.Publish(small,key));
  assert(smallIssued.Contains(small,key)&&!smallIssued.Contains(small,base+128));p[24]=key;p[18]|=4u;
  assert(small.Validate(key,p));assert(!small.Lookup(base+1));
 }
 assert(!small.Init({0x70000000,0x20000000,0x30000000,0x40000000}));assert(small.Init(Bases));
 for(unsigned d=0;d<4;++d)for(unsigned bad:{0u,0x10000001u,0xffffffc0u}){
  auto bases=Bases;bases[d]=bad;native_class_domains105::Registry<16> invalid;
  assert(!invalid.Init(bases)&&!invalid.configured&&invalid.Used()==0);
 }
 for(unsigned a=0;a<4;++a)for(unsigned b=a+1;b<4;++b){auto bases=Bases;bases[b]=bases[a]+64;
  native_class_domains105::Registry<16> invalid;assert(!invalid.Init(bases)&&!invalid.configured);
 }
 native_class_domains105::Registry<16> edge;assert(edge.Init({0xfffffc00,0x20000000,0x30000000,0x40000000}));
 assert(edge.KeyDomain(0xffffffc0)==0&&edge.KeyDomain(0xffffffff)==-1&&edge.KeyDomain(0)==-1);
 // Snow overlay remains scoped; fullcurrent witness rejects every mutation.
 native_class_domains105::Registry<16> snow;snow.snowMetadata70=true;assert(snow.Init(Bases));auto p=Light(0,0);p[17]=2;p[18]=0x41;
 const auto snowKey=snow.Observe(p,1,[](uint32_t){return false;});assert(snowKey);p[24]=snowKey;p[18]|=12;
 p[28]=std::bit_cast<uint32_t>(.75f);p[29]=std::bit_cast<uint32_t>(.25f);assert(snow.Validate(snowKey,p));
 native_projection66::Sidecar<> witness;witness.Begin(0x1000,1,1);witness.Own(0,p);witness.Accept(0,false);
 assert(witness.Current(0,p,0x1000,1,1,0x2000)==native_projection66::Decision::Valid);p[29]^=1;
 assert(witness.Current(0,p,0x1000,1,1,0x2000)==native_projection66::Decision::Invalid);
 for(unsigned d=0;d<4;++d){auto malformed=Light(d,0);malformed[8]=0x7fc00000;
  assert(!small.Observe(malformed,1,[](uint32_t){return false;}));
 }
 std::puts("projection105 PASS: exact captured4096 world identities remapped2214/1882; ten actual key0 target packets admitted offline with class200 full; all4 capacities4096/full16384; canonical aliases, current112 witness, snow and collision/issued/range gates preserved; native7maps unchanged");
}
