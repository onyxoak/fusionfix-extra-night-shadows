#include <cassert>
#include <cstring>
#include <limits>
#include <bit>
#include "core/ConsumerReach136.hpp"
using namespace consumer_shadow75;
int main(){
 Sample s{};s.available=1;s.sizeGate=0;s.finite=true;s.reproduced=true;s.flags=69;s.type=0;
 s.gateRadius=10;s.distance=31;s.threshold=.339770854f;s.ratio=10.f/31;
 assert(consumer_reach136::Shoulder(s));
 s.ratio=s.threshold/consumer_reach136::StaticShoulder162;assert(consumer_reach136::Shoulder(s));
 s.ratio=std::nextafter(s.ratio,0.f);assert(!consumer_reach136::Shoulder(s));
 const float band162=s.threshold/consumer_reach136::StaticShoulder162;assert(consumer_reach136::NumericPotential142(s.threshold,band162));assert(!consumer_reach136::NumericPotential142(s.threshold,std::nextafter(band162,0.f)));
 s.ratio=10.f/31;
 for(auto flags:{0x104u,0x504u,0x404u}){s.flags=flags;assert(!consumer_reach136::Shoulder(s));}s.flags=69;
 for(auto type:{1u,3u,4u}){s.type=type;assert(!consumer_reach136::Shoulder(s));}s.type=0;
 s.sizeGate=1;assert(!consumer_reach136::Shoulder(s));s.sizeGate=0;
 s.available=0;assert(!consumer_reach136::Shoulder(s));s.available=1;
 s.reproduced=false;assert(!consumer_reach136::Shoulder(s));s.reproduced=true;
 for(float invalid:{0.f,-1.f,std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()}){
  auto bad=s;bad.threshold=invalid;assert(!consumer_reach136::Shoulder(bad));bad=s;bad.gateRadius=invalid;assert(!consumer_reach136::Shoulder(bad));
 }
 Context c{};Registers r{};int reads=0,writes=0,authorizes=0;
 auto read=[&](uintptr_t,void*,size_t)noexcept{++reads;return false;};
 auto authorize=[&](const Packet&,uint32_t)noexcept{++authorizes;return true;};
 auto write=[&](uintptr_t,uint8_t)noexcept{++writes;return true;};
 assert(!consumer_reach136::Gate(false,c,r,1,1,0,read,authorize,write));assert(reads==0&&writes==0&&authorizes==0);
 c.pending=true;c.frame=2;assert(!consumer_reach136::Gate(true,c,r,1,1,0,read,authorize,write));assert(!c.pending&&writes==0&&authorizes==0);
 // Exercise actual production Gate decoder and final byte publication.
 static std::array<uint8_t,0x100> nativeStack{};
 static std::array<uint8_t,0x300> nativeCamera{};
 static Packet nativePacket{};static float global=1.f;
 const uintptr_t stack=reinterpret_cast<uintptr_t>(nativeStack.data());
 const uintptr_t camera=reinterpret_cast<uintptr_t>(nativeCamera.data());
 const uintptr_t source=reinterpret_cast<uintptr_t>(&nativePacket);
 nativePacket.words[17]=0;nativePacket.words[18]=69;nativePacket.words[21]=std::bit_cast<uint32_t>(10.f);nativePacket.words[24]=123;
 auto initialize=[&]{
  nativeStack.fill(0);nativeCamera.fill(0);nativeStack[0xd]=1;
  const uint32_t cameraPointer=static_cast<uint32_t>(camera),sourcePointer=static_cast<uint32_t>(source+0x28);
  const float radius=10.f,position=31.f,scale=.339770854f;
  std::memcpy(nativeStack.data()+0x28,&cameraPointer,4);std::memcpy(nativeStack.data()+0x1c,&sourcePointer,4);
  std::memcpy(nativeStack.data()+0x14,&radius,4);std::memcpy(nativeCamera.data()+0x70,&position,4);std::memcpy(nativeCamera.data()+0x2cc,&scale,4);
  c={};c.pending=true;c.anchor=stack;c.source=source;c.frame=10;c.packet=nativePacket;
  r={stack,0,0,0,scale,10.f/31};reads=writes=authorizes=0;
 };
 auto realRead=[&](uintptr_t address,void* out,size_t n)noexcept{
  ++reads;auto in=[&](uintptr_t base,size_t size){return address>=base&&address-base<=size&&n<=size-(address-base);};
  if(!in(stack,nativeStack.size())&&!in(camera,nativeCamera.size())&&!in(source,sizeof(nativePacket))&&!in(reinterpret_cast<uintptr_t>(&global),4))return false;
  std::memcpy(out,reinterpret_cast<const void*>(address),n);return true;
 };
 auto realWrite=[&](uintptr_t address,uint8_t value)noexcept{++writes;assert(address==stack+0xe&&value==1);nativeStack[0xe]=value;return true;};
 initialize();assert(consumer_reach136::Gate(true,c,r,10,1,reinterpret_cast<uintptr_t>(&global),realRead,authorize,realWrite));assert(writes==1&&authorizes==1&&nativeStack[0xe]==1);
 //162 A certified existing static map now reaches this37m receiver; the old1.15 shoulder refused it.
 initialize();const float farther162=37;std::memcpy(nativeCamera.data()+0x70,&farther162,4);r.ratio=10.f/farther162;assert(r.ratio<r.threshold/1.15f&&r.ratio>=r.threshold/consumer_reach136::StaticShoulder162);assert(consumer_reach136::Gate(true,c,r,10,1,reinterpret_cast<uintptr_t>(&global),realRead,authorize,realWrite)&&writes==1&&authorizes==1);
 for(auto beam162:{0x104u,0x504u,0x404u}){initialize();nativePacket.words[18]=beam162;c.packet=nativePacket;std::memcpy(nativeCamera.data()+0x70,&farther162,4);r.ratio=10.f/farther162;assert(!consumer_reach136::Gate(true,c,r,10,1,reinterpret_cast<uintptr_t>(&global),realRead,authorize,realWrite)&&writes==0&&authorizes==0);}nativePacket.words[18]=69;

 initialize();auto denied=[&](const Packet&,uint32_t)noexcept{++authorizes;return false;};assert(!consumer_reach136::Gate(true,c,r,10,1,reinterpret_cast<uintptr_t>(&global),realRead,denied,realWrite));assert(writes==0);
 initialize();auto mutate=[&](const Packet&,uint32_t)noexcept{++authorizes;nativePacket.words[18]=0x104;return true;};assert(!consumer_reach136::Gate(true,c,r,10,1,reinterpret_cast<uintptr_t>(&global),realRead,mutate,realWrite));assert(writes==0);nativePacket.words[18]=69;
 initialize();r.esp+=4;assert(!consumer_reach136::Gate(true,c,r,10,1,reinterpret_cast<uintptr_t>(&global),realRead,authorize,realWrite));assert(writes==0&&authorizes==0);
 initialize();nativeStack[0xe]=1;assert(!consumer_reach136::Gate(true,c,r,10,1,reinterpret_cast<uintptr_t>(&global),realRead,authorize,realWrite));assert(writes==0&&authorizes==0);
 initialize();assert(!consumer_reach136::Gate(false,c,r,10,1,reinterpret_cast<uintptr_t>(&global),realRead,authorize,realWrite));assert(reads==0&&writes==0&&authorizes==0);
}


