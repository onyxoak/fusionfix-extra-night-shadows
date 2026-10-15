#include "CoronaLightPolicy20.hpp"
#include "ModelLightRegistry20.hpp"
#include <cstdio>
#include <cstdlib>
#include <limits>
static unsigned checks;
#define CHECK(v) do{++checks;if(!(v)){std::fprintf(stderr,"FAIL %d %s\n",__LINE__,#v);std::abort();}}while(false)
static uint32_t Bits(float f){uint32_t bits;std::memcpy(&bits,&f,4);return bits;}
static void Author(corona_light20::Packet& p,unsigned offset,float value){std::memcpy(p.descriptor.data()+offset,&value,4);}
static corona_light20::Packet Native(){
 corona_light20::Packet p{};p.corona={0x9005,255,64,32,Bits(2),0x10000,Bits(300),0,Bits(1),0,Bits(5),0,0,0,1};
 p.dispatch[0]=0x8000;p.dispatch[1]=0x10000;p.dispatch[2]=Bits(.75f);p.dispatch[6]=0x9005;
 p.position=p.matrixPosition={2,4,6};Author(p,0x5c,2);return p;
}
int main(){
 auto p=Native();corona_light20::Settings s;corona_light20::Emission out;
 CHECK(corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,s,p,out));
 CHECK(out.radius==24&&out.intensity==.75f&&out.color.x==1&&out.approximateRadius&&out.approximateIntensity);
 // Far clip affects corona visibility; it never sets light attenuation.
 auto far=p;far.corona[6]=Bits(9000);Author(far,0x40,10000);
 CHECK(corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,s,far,out)&&out.radius==24);
 Author(p,0x38,42);Author(p,0x3c,4);
 CHECK(corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,s,p,out));
 CHECK(out.radius==42&&out.intensity==3&&!out.approximateRadius&&!out.approximateIntensity);
 s.allowApproximation=false;CHECK(corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,s,p,out));
 CHECK(!corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,s,Native(),out));s.allowApproximation=true;
 for(auto site:{0u,corona_light20::OffSite,0x642d19u})CHECK(!corona_light20::Reconstruct(site,0x9005,s,p,out));
 for(unsigned type:{1u,2u,3u,255u}){auto q=p;q.descriptor[0x5b]=uint8_t(type);CHECK(!corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,s,q,out));}
 for(unsigned field:{0u,13u,14u}){auto q=p;q.corona[field]=field==13?1:0;CHECK(!corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,s,q,out));}
 {auto q=p;q.dispatch[6]++;CHECK(!corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,s,q,out));}
 {auto q=p;q.corona[1]=q.corona[2]=q.corona[3]=0;CHECK(!corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,s,q,out));}
 for(unsigned field=1;field<=3;++field){auto q=p;q.corona[field]=256;CHECK(!corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,s,q,out));}
 for(float v:{-1.f,std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity()}){
  for(unsigned offset:{0x38u,0x3cu,0x5cu}){auto q=p;Author(q,offset,v);CHECK(!corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,s,q,out));}
  for(unsigned field:{2u,4u}){auto q=p;if(field==2)q.dispatch[field]=Bits(v);else q.corona[field]=Bits(v);CHECK(!corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,s,q,out));}
  auto q=p;q.position.x=v;CHECK(!corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,s,q,out));
 }
 {auto q=p;q.position.x++;CHECK(!corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,s,q,out));}
 for(unsigned field:{2u,4u}){auto q=p;if(field==2)q.dispatch[field]=0;else q.corona[field]=0;CHECK(!corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,s,q,out));}
 // Positive native authored attenuation/intensity retain their authored scale.
 Author(p,0x38,1000);Author(p,0x3c,1000);
 CHECK(corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,s,p,out)&&out.radius==1000&&out.intensity==750);
 for(auto member:{&corona_light20::Settings::radiusPerAuthoredSize,&corona_light20::Settings::fallbackIntensity,&corona_light20::Settings::intensityScale,
                 &corona_light20::Settings::minRadius,&corona_light20::Settings::maxRadius,&corona_light20::Settings::maxIntensity}){
  for(float v:{0.f,-1.f,std::numeric_limits<float>::quiet_NaN()}){auto cfg=s;cfg.*member=v;CHECK(!corona_light20::Reconstruct(corona_light20::ActiveSite,0x9005,cfg,p,out));}
 }
 // Same registry/free dispatch as authenticated native model emitters. A
 // descriptor/type change, stream-out, or wrapped pool slot cannot revive key.
 model_light20::Registry<4> registry;CHECK(registry.Configure(0x200000));
 traffic_signal::Identity id{0x10,0x9000,0xa000,0x9000,0,7,0xb000};auto bytes=p.descriptor;
 auto* entry=registry.Observe(id,5,0xc000,0xd000,bytes,10);CHECK(entry);const auto oldKey=entry->key;
 CHECK(registry.Validate(oldKey,id,bytes,11));registry.Free(id.pool,id.owner);CHECK(!registry.Validate(oldKey,id,bytes,11));
 auto* wrapped=registry.Observe(id,5,0xc000,0xd000,bytes,11);CHECK(wrapped&&wrapped->key!=oldKey);
 const auto newKey=wrapped->key;bytes[0x5b]=1;CHECK(!registry.Validate(newKey,id,bytes,11));
 std::printf("PASS authored corona reconstruction: %u checks; off paths, source type/identity, actual RGB/pose, far-clip distinction, approximation controls, and shared nonrecycled lifetime. No live point-face or coverage claim.\n",checks);
}
