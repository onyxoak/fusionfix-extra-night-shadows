#include "core/VisibleReceiverRanking133.hpp"
#include "core/NativeLampDistance38.hpp"
#include "core/ShadowReach.hpp"
#include "core/ShadowVolumeVisibility43.hpp"
#include "core/NativeLampContinuity41.hpp"
#include "core/ShadowFloatingPointState.hpp"
#include "ModelLightRegistry20.hpp"
#include "DualIssuedKeys127.hpp"
#include <atomic>
#include <array>
#include <cassert>
#include <cstring>
#include <limits>
#include "core/standalone-game-abi.hpp"
#include "NativeProjectionRegistry66.hpp"
#include "NativeProjectionRead66.hpp"
namespace TailLight20 {bool Contains(uint32_t key) noexcept{return key>=0x500000&&key<0x510000;}}
namespace Liberty20 {alignas(64) uint8_t keyStorage[128]{};dual_keys127::PublishedKeys<2> publishedKeys127;}
namespace ModelLightShadow20 {std::atomic<bool> active{true};model_light20::Registry<8> registry;}
namespace ModelEffectObserver {struct LastErrorScope {DWORD saved=GetLastError();~LastErrorScope(){SetLastError(saved);}};}
namespace TrafficSignal {unsigned calls{};uint32_t key=0x600000;
uint64_t LampGeneration(const rage::CLightSource& light) noexcept{++calls;return uint32_t(light.mCastShadows)==key?1:0;}}
namespace Liberty20 {
bool CopyLocal(uintptr_t address,void* out,size_t size) noexcept {
 __try{std::memcpy(out,reinterpret_cast<const void*>(address),size);return true;}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
}
namespace NativeProjection66 {
using native_projection66::Packet;
std::atomic<bool> active{true};native_projection66::Sidecar<> sidecar;
uint32_t projectedKey=0x700000;
std::atomic<uint32_t> winnerPreflights{},permissionQueries{},winnerDenied{},packetFaults{};
unsigned Classify(unsigned index,uint32_t key,rage::CLightSource* bank,uint32_t count,uint32_t frame) noexcept {
 if(key!=projectedKey)return 0;
 if(!active||!sidecar.Envelope(reinterpret_cast<uintptr_t>(bank),count,frame)||index>=count)return 3;
 const auto d=sidecar.decisions[index];return sidecar.packets[index][24]==key&&
  (d==native_projection66::Decision::Valid||d==native_projection66::Decision::Alias)?1u:3u;
}
#include "receiver133-copy.inc"
}
std::atomic<uint32_t> projectionRankingChecks133{},projectionRankingQueries133{},projectionRankingAccepted133{},projectionRankingRefused133{};
#include "receiver133-source.inc"
using namespace fusionfix::shadows;
struct SafetyHookContext {uintptr_t edi{},esi{},esp{};struct {float f32[4]{};} xmm0;};
unsigned sourceCalls133{};
bool Predicate133(const rage::CLightSource& light,unsigned index,rage::CLightSource* bank,uint32_t count,uint32_t frame) noexcept{++sourceCalls133;return VisibleReceiverSource133(light,index,bank,count,frame);}
namespace Fixture133 {
bool nativeLampPriority=true,publicationEnabled=true,visibleReceiverExperiment133=false;
float scenePriorityStrength=1.5f;
bool (*visibleReceiverSource133)(const rage::CLightSource&,unsigned,rage::CLightSource*,uint32_t,uint32_t) noexcept=Predicate133;
std::atomic<unsigned> lampDistanceAdjusted{};
rage::CLightSource current{};
struct {int Get(const char*)const noexcept{return 40;}} FusionFixSettings;
bool Enabled()noexcept{return true;}
rage::CLightSource* CurrentLights()noexcept{return &current;}
unsigned CurrentCount()noexcept{return 1;}
uint64_t LampGeometry(const rage::CLightSource&)noexcept{return 1;}
struct {
 unsigned depth=1;uint32_t frame=1;Vec3 player{};bool occupiedCar=false;ShadowView view{};ShadowVolumeFrustum20 volumeFrustum{};
 struct {bool Active()const noexcept{return true;}} pass;
 struct {bool Ambiguous(unsigned)const noexcept{return false;}} keyMultiplicity;
 NativeLampContinuity41 lampContinuity;
} state;
#include "receiver133-adjust.inc"
namespace budget {enum class Kind {Lamp,PlayerBeam,OtherBeam};}
struct {float distanceSquared{};} geometry;
bool Relevance133(bool support,budget::Kind kind,float distance,float radius,float reach,float viewWeight,bool volumeVisible){
 current.mRadius=radius;current.mType=rage::LT_POINT;current.mOuterConeAngle=.8f;geometry.distanceSquared=distance*distance;
 const auto& light=current;const unsigned index=0;auto* lights=&current;const uint32_t count=1;
 visibleReceiverSource133=support?Predicate133:nullptr;
#include "receiver133-relevance.inc"
 return relevant;
}
}
int main(){
 assert(ModelLightShadow20::registry.Configure(0x400000));
 traffic_signal::Identity identity{1,2,3,4,0,5,6};std::array<uint8_t,0x6c> descriptor{};
 auto* entry=ModelLightShadow20::registry.Observe(identity,0,100,200,descriptor,1);assert(entry);
 rage::CLightSource light{};light.mFlags=4;light.mCastShadows=int32_t(entry->key);
 assert(!VisibleReceiverSource133(light,0,&light,1,1)); // Allocated but never published.
 assert(ModelLightShadow20::registry.Publish(entry->key));assert(VisibleReceiverSource133(light,0,&light,1,1));
 for(auto flags:{0x104u,0x504u,0x400u}){light.mFlags=flags;const auto calls=TrafficSignal::calls;assert(!VisibleReceiverSource133(light,0,&light,1,1)&&calls==TrafficSignal::calls);}
 light.mFlags=4;light.mCastShadows=0x500001;assert(!VisibleReceiverSource133(light,0,&light,1,1)); // Tail domain, even without100.
 const auto dualBase=uint32_t(reinterpret_cast<uintptr_t>(Liberty20::keyStorage));
 assert(Liberty20::publishedKeys127.PublishPair(dualBase,{dualBase,dualBase+64}));
 light.mCastShadows=int32_t(dualBase);assert(!VisibleReceiverSource133(light,0,&light,1,1)); // Forward issued key without classification bit.
 light.mCastShadows=0x700000;assert(!VisibleReceiverSource133(light,0,&light,1,1)); // Anonymous but no current accepted witness.
 light.mCastShadows=int32_t(TrafficSignal::key);assert(VisibleReceiverSource133(light,0,&light,1,1));
 light.mCastShadows=int32_t(entry->key);light.mFlags=0x100;assert(!VisibleReceiverSource133(light,0,&light,1,1));
 light.mFlags=4;ModelLightShadow20::active=false;assert(!VisibleReceiverSource133(light,0,&light,1,1));ModelLightShadow20::active=true;
 // Classification cannot revive a retired owner: the unchanged validator still refuses it.
 ModelLightShadow20::registry.Free(identity.pool,identity.owner);assert(VisibleReceiverSource133(light,0,&light,1,1));
 assert(!ModelLightShadow20::registry.Validate(entry->key,identity,descriptor,1));
 using namespace Fixture133;
 current=light;current.mPosition={80,0,0};current.mDirection={0,0,-1};current.mRadius=30;current.mIntensity=8;current.mColor={1,1,1,0};current.mOuterConeAngle=.8f;
 state.view.valid=true;state.volumeFrustum.valid=true;const auto original=current;
 std::array<uint32_t,12> stack{};stack[5]=0;stack[7]=current.mFlags;
 SafetyHookContext regs{};regs.edi=reinterpret_cast<uintptr_t>(&current);regs.esp=reinterpret_cast<uintptr_t>(stack.data());regs.xmm0.f32[0]=6400;
 sourceCalls133=0;AdjustLampDistance(regs);assert(regs.xmm0.f32[0]==6400&&sourceCalls133==0);
 assert(std::memcmp(&current,&original,sizeof(current))==0);
 // Use the positively certified traffic fixture for the current callback.
 current.mCastShadows=int32_t(TrafficSignal::key);visibleReceiverExperiment133=true;AdjustLampDistance(regs);
 assert(sourceCalls133==1&&regs.xmm0.f32[0]<6400&&regs.xmm0.f32[0]>=6400/1.15f);
 assert(current.mPosition.x==80&&current.mRadius==30&&current.mIntensity==8&&current.mFlags==4);
 assert(Relevance133(true,budget::Kind::Lamp,80,30,60.96f,3,true));
 assert(!Relevance133(false,budget::Kind::Lamp,80,30,60.96f,3,true));
 assert(!Relevance133(true,budget::Kind::OtherBeam,80,30,60.96f,3,true));
 assert(!Relevance133(true,budget::Kind::Lamp,80,30,60.96f,3,false));
 visibleReceiverExperiment133=false;assert(!Relevance133(true,budget::Kind::Lamp,80,30,60.96f,3,true));
 assert(Relevance133(true,budget::Kind::PlayerBeam,10,30,60.96f,3,false)); // Parent priority predicate unchanged.
 // Dark/nonfinite packets have zero qualifier calls and zero extra page queries.
 visibleReceiverExperiment133=true;visibleReceiverSource133=Predicate133;
 const auto darkCalls=sourceCalls133,darkQueries=projectionRankingQueries133.load();
 for(unsigned dark=0;dark<4;++dark){
  current.mIntensity=dark==0?0.f:8.f;current.mColor={1,1,1,0};
  if(dark==1)current.mColor={0,0,0,0};
  if(dark==2)current.mIntensity=std::numeric_limits<float>::quiet_NaN();
  if(dark==3)current.mColor.x=std::numeric_limits<float>::quiet_NaN();
  regs.xmm0.f32[0]=6400;AdjustLampDistance(regs);
  assert(regs.xmm0.f32[0]==6400&&!Relevance133(true,budget::Kind::Lamp,80,30,60.96f,3,true));
 }
 assert(sourceCalls133==darkCalls&&projectionRankingQueries133==darkQueries);
 current.mIntensity=8;current.mColor={1,1,1,0};current.mCastShadows=int32_t(NativeProjection66::projectedKey);
 NativeProjection66::Packet packet{};std::memcpy(packet.data(),&current,0x78);
 const auto bank=reinterpret_cast<uintptr_t>(&current);
 NativeProjection66::sidecar.Begin(bank,1,state.frame);NativeProjection66::sidecar.Own(0,packet);NativeProjection66::sidecar.Accept(0,false);
 const auto beforeChecks=projectionRankingChecks133.load();SetLastError(13313);
 assert(VisibleReceiverSource133(current,0,&current,1,state.frame)&&GetLastError()==13313);
 assert(projectionRankingChecks133==beforeChecks+1&&projectionRankingQueries133>darkQueries);
 assert(NativeProjection66::sidecar.anchor==0);
 assert(NativeProjection66::sidecar.Current(0,packet,bank,1,state.frame,0x1234)==native_projection66::Decision::Valid);
 assert(VisibleReceiverSource133(current,0,&current,1,state.frame)&&NativeProjection66::sidecar.anchor==0x1234);
 // Full112 mutation refusal including W, photometry and non-projector volume.
 for(unsigned field:native_projection66::WitnessFields){
  auto mutation=packet;mutation[field]^=1;std::memcpy(&current,mutation.data(),0x78);
  assert(!VisibleReceiverSource133(current,0,&current,1,state.frame));
 }
 std::memcpy(&current,packet.data(),0x78);
 auto reads=projectionRankingChecks133.load();NativeProjection66::sidecar.Accept(0,true);
 assert(!VisibleReceiverSource133(current,0,&current,1,state.frame)&&projectionRankingChecks133==reads);
 NativeProjection66::sidecar.Accept(0,false);
 assert(!VisibleReceiverSource133(current,0,&current,1,state.frame+1)&&projectionRankingChecks133==reads);
 NativeProjection66::active=false;assert(!VisibleReceiverSource133(current,0,&current,1,state.frame)&&projectionRankingChecks133==reads);NativeProjection66::active=true;
 // Real current permission refusal: key/flags live on the first page, while
 // full packet tails touch a guarded second page. Guard remains untouched.
 auto* pages=static_cast<uint8_t*>(VirtualAlloc(nullptr,8192,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(pages);
 auto* crossing=reinterpret_cast<rage::CLightSource*>(pages+4096-0x68);std::memcpy(crossing,&current,sizeof(current));
 NativeProjection66::sidecar.Begin(reinterpret_cast<uintptr_t>(crossing),1,1);NativeProjection66::sidecar.Own(0,packet);NativeProjection66::sidecar.Accept(0,false);
 for(DWORD protection:{DWORD(PAGE_READWRITE|PAGE_GUARD),DWORD(PAGE_NOACCESS)}){
  DWORD old{};assert(VirtualProtect(pages+4096,4096,protection,&old));SetLastError(13314);
  assert(!VisibleReceiverSource133(*crossing,0,crossing,1,1)&&GetLastError()==13314);
  MEMORY_BASIC_INFORMATION info{};assert(VirtualQuery(pages+4096,&info,sizeof(info))&&info.Protect==protection);
  assert(VirtualProtect(pages+4096,4096,PAGE_READWRITE,&old));
 }
 assert(VirtualFree(pages,0,MEM_RELEASE));
 // Mismatched native index/flags and nested callbacks cannot use the experiment.
 visibleReceiverExperiment133=true;visibleReceiverSource133=Predicate133;regs.xmm0.f32[0]=6400;stack[7]=0;
 const auto calls=sourceCalls133;AdjustLampDistance(regs);assert(regs.xmm0.f32[0]==6400&&sourceCalls133==calls);
 stack[7]=current.mFlags;state.depth=2;AdjustLampDistance(regs);assert(regs.xmm0.f32[0]==6400&&sourceCalls133==calls);
 // Actual pre-cache callback: same authenticated traffic family, walking and
 // driving each prefer central visible influence; source packet stays exact.
 state.depth=1;state.view={};state.view.valid=true;
 for(unsigned i=0;i<4;++i)state.view.view[i][i]=state.view.projection[i][i]=1.f;
 state.view.view[3][0]=-16.f;current={};current.mType=rage::LT_POINT;current.mFlags=4;current.mCastShadows=int32_t(TrafficSignal::key);
 current.mRadius=.1f;current.mIntensity=8;current.mColor={1,1,1,0};stack[7]=4;
 for(bool driving163:{false,true}){
  state.occupiedCar=driving163;current.mPosition={16,0,.5f};auto unchanged163=current;
  regs.xmm0.f32[0]=6400;AdjustLampDistance(regs);const float centerRank163=regs.xmm0.f32[0];assert(std::memcmp(&current,&unchanged163,sizeof(current))==0);
  current.mPosition={17,0,.5f};unchanged163=current;regs.xmm0.f32[0]=6400;AdjustLampDistance(regs);assert(centerRank163<regs.xmm0.f32[0]&&std::memcmp(&current,&unchanged163,sizeof(current))==0);
 }
 // Exact native vehicle-family exclusion performs no owner qualification or
 // center sampling and leaves its ranking input unchanged.
 for(uint32_t flags163:{0x104u,0x504u,0x564u}){current.mFlags=stack[7]=flags163;regs.xmm0.f32[0]=6400;const auto before163=sourceCalls133;AdjustLampDistance(regs);assert(regs.xmm0.f32[0]==6400&&sourceCalls133==before163);}
 return 0;
}
