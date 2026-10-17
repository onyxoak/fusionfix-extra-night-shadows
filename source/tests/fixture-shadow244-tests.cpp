#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <map>
#include <limits>
#include <vector>
#include "TrafficSignalIdentity.hpp"
#include "NativeProjectionRegistry66.hpp"
namespace test244 {
uint32_t game=0x400000,frame=10;std::atomic<bool> addonActive{true};bool fault=false,match=false,fallback=false;
std::map<uintptr_t,std::vector<uint8_t>> memory;
template<class T> void Put(uintptr_t at,const T& v){auto& bytes=memory[at];bytes.resize(sizeof(v));std::memcpy(bytes.data(),&v,sizeof(v));}
bool Read(uintptr_t at,void* p,size_t n){if(fault)return false;for(auto& [base,bytes]:memory)if(at>=base&&uint64_t(at)+n<=uint64_t(base)+bytes.size()){std::memcpy(p,bytes.data()+at-base,n);return true;}return false;}
struct Invocation {uint32_t owner=0x10000,modelInfo=0x40000,descriptor=0x60000,frame=10;};
Invocation invocation;
namespace ModelEffectObserver {
using ::test244::Invocation;
void (*nativeFixture256)(const Invocation&,uint32_t,uint32_t,uint32_t) noexcept=nullptr;
struct LastErrorScope{DWORD value=GetLastError();~LastErrorScope(){SetLastError(value);}};
uint32_t Frame(){return frame;}
bool Copy(uintptr_t at,void* p,size_t n){return Read(at,p,n);}
struct Sample {bool diagnostic{},packetCopied{};std::array<float,3> direction{},up{},position{},color{};float intensity{},radius{},innerCone{},outerCone{};uint32_t promotedKey{};};
struct State {unsigned Depth(){return 1;}const Invocation* Current(){return &invocation;}};struct Local {State state;std::array<Sample,8> samples{};}local;
namespace ModelValue68 {bool Matches(int){return match;}}
}
namespace TrafficSignal {
struct Match {traffic_signal::Identity identity{};uint32_t modelInfo{},kind{};std::array<uint8_t,68> ownerPose109{};};
std::map<uint32_t,Match> owners;
bool MatchOwner(uint32_t owner,Match& out){if(fault||!owners.contains(owner))return false;out=owners[owner];return true;}
bool MatchOwnerWithDescriptor48(uint32_t owner,Match& out,uint32_t descriptor,void* bytes,size_t n,bool& copied){copied=MatchOwner(owner,out)&&Read(descriptor,bytes,n);return copied;}
}
namespace fusionfix::shadows {struct FloatingPointState{};}
namespace traffic_signal {using ::traffic_signal::Identity;namespace fresh_page {struct Operation44 {explicit Operation44(unsigned){}};}}
namespace immediate_region33 {struct Scope{};}
namespace Liberty20 {bool CopyLocal(uintptr_t at,void* p,size_t n){return Read(at,p,n);}}
namespace NativeProjection66 {
using Constructed244=void(*)(const native_projection66::Packet&,uint32_t,const native_projection66::ConstructorContext&)noexcept;
std::atomic<Constructed244> constructed244{nullptr};
bool OwnKey(uint32_t key){return key>=64;}
}
int ModelBinding69(const native_projection66::ConstructorContext&){return 0;}
namespace OwnHeadlightCaster {void (*labRenderObserver)(uint32_t,uint32_t,uint32_t,bool)noexcept=nullptr;bool(*boxExclude216)(uint32_t,uintptr_t,uint32_t)noexcept=nullptr;}
namespace VehicleBoxes216 {bool Exclude(uint32_t,uintptr_t,uint32_t){return fallback;}}
void Event(int,const char*,const char*){}
#include "FixtureShadow244Runtime.inc"
void Reset(){
 memory.clear();TrafficSignal::owners.clear();frame=10;fault=false;match=true;fallback=false;addonActive=true;invocation={};
 FixtureShadow244::records={};FixtureShadow244::selected={};FixtureShadow244::requested=true;
 ModelEffectObserver::local.samples={};
 const uint32_t hash=1117016355,info=0x40000;Put(info+0x3c,hash);
 Put(game+0xe95cd8+7*4,info);Put(0x60000,std::array<uint8_t,108>{});
 std::array<float,16> matrix{};matrix[0]=matrix[5]=matrix[10]=matrix[15]=1;
 matrix[12]=100;matrix[13]=200;matrix[14]=5;
 for(uint32_t entity:{0x10000u,0x20000u,0x30000u}){
  const auto pointer=entity+0x100;Put(entity+0x20,pointer);Put(entity+0x2e,uint16_t(7));
  auto pose=matrix;if(entity==0x30000)pose[12]+=10;Put(pointer,pose);
  TrafficSignal::Match owner{};owner.modelInfo=info;owner.identity={1,2,3,entity,4,5,hash};TrafficSignal::owners[entity]=owner;
 }
}
void Publish(uint32_t key=64){native_projection66::Packet packet{};packet[24]=key;FixtureShadow244::Constructed(packet,frame,{});}
}
int main(){
 using namespace test244;using namespace FixtureShadow244;
 Reset();Publish();Observe(64,1,4,true);
 assert(Exclude(64,0x10000,4)); // Original emitting support.
 assert(Exclude(64,0x20000,5)); // Same fixture's native-pool representation.
 assert(!Exclude(64,0x30000,4)); // Another instance of the same model.
 assert(!Exclude(128,0x10000,4)); // Another light still shadows this pole.
 assert(!Exclude(64,0x10000,2));
 assert(!Exclude(64,0x10000,3));
 // Captured physical/native aliases agree in all12 affine components while
 // padding lanes contain CDCDCDCD or a heading. These are still the same pose.
 for(unsigned lane:{3u,7u,11u,15u}){
  Reset();std::array<float,16> alias{};assert(Read(0x20100,alias.data(),64));
  alias[lane]=-431602080.f;Put(0x20100,alias);Publish();Observe(64,1,4,true);assert(Exclude(64,0x20000,5));
  alias[lane]=std::numeric_limits<float>::quiet_NaN();Put(0x20100,alias);assert(Exclude(64,0x20000,5));
 }
 Reset();std::array<float,16> changedPose{};assert(Read(0x20100,changedPose.data(),64));
 for(unsigned lane:{0u,1u,2u,4u,5u,6u,8u,9u,10u,12u,13u,14u}){
  auto changed=changedPose;changed[lane]+=.1f;Put(0x20100,changed);Publish();Observe(64,1,4,true);assert(!Exclude(64,0x20000,5));
 }
 Reset();Publish();Observe(64,1,4,true);

 Put(0x10000+0x2e,uint16_t(8));assert(!Exclude(64,0x10000,4));Put(0x10000+0x2e,uint16_t(7));
 ++TrafficSignal::owners[0x10000].identity.reference;assert(!Exclude(64,0x20000,5));
 Reset();Publish();Observe(64,1,4,true);++frame;assert(!Exclude(64,0x10000,4));
 Reset();Publish();Observe(64,1,4,true);fault=true;assert(!Exclude(64,0x10000,4));
 Reset();Publish();Observe(64,1,4,true);requested=false;assert(!Exclude(64,0x10000,4));
 Reset();Publish();Observe(64,1,4,true);addonActive=false;assert(!Exclude(64,0x10000,4));
 Reset();Publish();Observe(64,1,4,true);Observe(128,2,4,true);assert(!Exclude(64,0x10000,4));
 Reset();Publish();invocation.owner=0x20000;Publish();invocation.owner=0x10000;Publish();Observe(64,1,4,true);assert(!Exclude(64,0x10000,4));
 Reset();match=false;Publish();Observe(64,1,4,true);assert(!Exclude(64,0x10000,4));
 Reset();Publish();Observe(64,1,4,true);fallback=true;assert(Combined(64,0x90000,2));
 Reset();
 for(uint32_t owner:{0x10000u,0x20000u}){Put(owner+0x20,uint32_t(0));Put(owner+0x10,std::array<float,4>{100,200,5,.25f});}
 Publish();Observe(64,1,4,true);assert(Exclude(64,0x20000,1));
 Put(0x20000+0x10,std::array<float,4>{100,200,5,.5f});assert(!Exclude(64,0x20000,1));
 assert(Proxy(0x46c33c49));
 // Street and park poles use the same owner-scoped exclusion, never other lights.
 for(uint32_t hash:{0x46c33c49u,0x1485e82fu,0x2e9b0c05u,0x97dfda59u}){
  Reset();Put(invocation.modelInfo+0x3c,hash);Publish();Observe(64,1,4,true);
  assert(Exclude(64,0x10000,4)&&Exclude(64,0x20000,5));
  assert(!Exclude(128,0x10000,4)&&!Exclude(64,0x30000,4));
  assert(!Exclude(64,0x10000,2)&&!Exclude(64,0x10000,3));
 }
 // Actual native producer callback records the original key without allocating
 // a projection or changing any native light/owner data.
 for(uint32_t hash:{0x46c33c49u,0x1485e82fu,0x97dfda59u}){
  Reset();Put(invocation.modelInfo+0x3c,hash);const auto before=memory;
  const uint32_t nativeKey=0x10004;Native256(invocation,nativeKey,0,4);Observe(nativeKey,1,4,true);
  assert(Exclude(nativeKey,0x10000,4)&&Exclude(nativeKey,0x20000,5));
  assert(!Exclude(nativeKey+64,0x10000,4)&&!Exclude(nativeKey,0x30000,4));
  assert(memory==before);
  for(uint32_t nativeFlags:{4u,5u,6u,7u}){
   Reset();Put(invocation.modelInfo+0x3c,hash);Native256(invocation,nativeKey,0,nativeFlags);Observe(nativeKey,1,4,true);
   assert(Exclude(nativeKey,0x10000,4)&&!Exclude(nativeKey+64,0x10000,4)&&!Exclude(nativeKey,0x30000,4));
  }

  Reset();Put(invocation.modelInfo+0x3c,hash);Native256(invocation,nativeKey,0,0);Observe(nativeKey,1,4,true);assert(!Exclude(nativeKey,0x10000,4));
  Native256(invocation,nativeKey,0,2);Observe(nativeKey,1,4,true);assert(!Exclude(nativeKey,0x10000,4));
  auto foreign=invocation;Native256(foreign,nativeKey,0,4);Observe(nativeKey,1,4,true);assert(!Exclude(nativeKey,0x10000,4));
  fault=true;Native256(invocation,nativeKey,0,4);fault=false;Observe(nativeKey,1,4,true);assert(!Exclude(nativeKey,0x10000,4));
 }

 // Captured native keys607288644/608664900 alias the old direct slot.
 // Both emitters must keep independent own-fixture exclusions in one frame.
 Reset();const uint32_t first259=607288644u,second259=608664900u;
 assert(((first259>>6)&1023u)==((second259>>6)&1023u));
 Native256(invocation,first259,0,7);invocation.owner=0x30000;
 Native256(invocation,second259,0,7);invocation.owner=0x10000;
 Observe(first259,1,4,true);assert(Exclude(first259,0x10000,4)&&Exclude(first259,0x20000,5));
 assert(!Exclude(first259,0x30000,4));
 Observe(second259,2,4,true);assert(Exclude(second259,0x30000,4));
 assert(!Exclude(second259,0x10000,4));
 Observe(first259,1,4,true);assert(Exclude(first259,0x10000,4));
 // Exhaustion refuses the new record without evicting any current emitter.
 Reset();std::array<uint32_t,ProbeCount259+1> collision259{};unsigned n259=0;
 for(uint32_t key=1;n259<collision259.size();++key)if(Start259(key)==0)collision259[n259++]=key;
 Record prototype259{};prototype259.frame=frame;prototype259.owner=invocation.owner;
 for(unsigned i=0;i<ProbeCount259;++i){prototype259.key=collision259[i];assert(Store259(prototype259));}
 prototype259.key=collision259.back();assert(!Store259(prototype259));
 for(unsigned i=0;i<ProbeCount259;++i)assert(Find259(collision259[i],frame));
 assert(!Find259(collision259.back(),frame));
 ++frame;prototype259.frame=frame;assert(Store259(prototype259));
 assert(Find259(collision259.back(),frame));
 for(unsigned i=0;i<ProbeCount259;++i)assert(!Find259(collision259[i],frame));
 assert(Proxy(3310280868)); // Arrival-port fixture is included globally.
 Reset();requested=false;ModelEffectObserver::local.samples[0].diagnostic=true;
 native_projection66::Packet observed{};observed[24]=64;observed[8]=std::bit_cast<uint32_t>(123.f);observed[21]=std::bit_cast<uint32_t>(50.f);
 Constructed(observed,10,{});
 assert(ModelEffectObserver::local.samples[0].packetCopied&&ModelEffectObserver::local.samples[0].promotedKey==64&&ModelEffectObserver::local.samples[0].position[0]==123&&ModelEffectObserver::local.samples[0].radius==50);
 std::cout<<"PASS244 production self-occlusion scope: own support/alias only; other instances, other lights, floor models, stale/free/fault/ambiguous/disabled scopes and vehicle fallback preserved\n";
}
