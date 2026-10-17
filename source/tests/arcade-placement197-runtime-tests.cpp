#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#include <array>
#include <atomic>
#include <bit>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <optional>
#include <vector>
#include <algorithm>
#include "standalone-pe.hpp"
#include "core/ShadowFloatingPointState.hpp"
#include "FreshReadBatch30.hpp"
#include "FreshReadGroup50.hpp"
#include "ImmediateRegion33.hpp"
#include "TrafficSignalIdentity.hpp"
#include "PoolDescriptorSnapshots.hpp"
#include "NativeOwnerClass109.hpp"
#include "NativeOwnerClass114.hpp"
#include "ModelLightSubmission20.hpp"
#include "ArcadePlacement197.hpp"
#include "NativeClassDomains105.hpp"
#include "NativeProjectionRead66.hpp"
#include "core/SceneShadowPriority21.hpp"
namespace pe=onyxoak::pe;
uint32_t game{};
namespace rage {
struct VehiclePool{uint8_t* m_aStorage;uint8_t* m_aFlags;int32_t m_nSize,m_nStorageSize,m_nFirstFreeIndex,m_numSlotsUsed;};
struct CLightSource{std::array<uint32_t,32> words;};
}
void Event(int,const char*,const std::string&){}
unsigned copies{},ownerCalls{},queryCalls{};
SIZE_T WINAPI CountQuery(LPCVOID p,PMEMORY_BASIC_INFORMATION m,SIZE_T n){++queryCalls;return VirtualQuery(p,m,n);}
bool ReadableOriginal(uintptr_t p,size_t n) noexcept{return immediate_region33::Readable(p,n,CountQuery);}
bool Readable(uintptr_t p,size_t n) noexcept{return traffic_signal::fresh_page::Readable(p,n,ReadableOriginal);}
struct Match{traffic_signal::Identity identity{};uint32_t kind{};float entity[3]{};uint32_t modelInfo{};std::array<uint8_t,68> ownerPose109{};};
// Exact production pool/header/generation/model checks, as used by68/196.
#include "owner-runtime32-extracted.inc"
namespace TrafficSignal {
using ::Match;
bool MatchOwner(uintptr_t p,Match& m) noexcept{++ownerCalls;return ::MatchOwner(p,m);}
}
bool RawCopy(uintptr_t at,void* out,size_t size) noexcept{
 __try{std::memcpy(out,reinterpret_cast<void*>(at),size);return true;}
 __except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION||GetExceptionCode()==EXCEPTION_GUARD_PAGE||
          GetExceptionCode()==EXCEPTION_IN_PAGE_ERROR?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){return false;}
}
void (*afterCopy)(uintptr_t,size_t){};
namespace ModelLightShadow20{bool requested=true;std::atomic<bool> active{true};}
struct SafetyHookContext{uint32_t esp{},ebp{},edi{};};
namespace ModelEffectObserver {
using namespace model_effect_observer;
struct LastErrorScope{DWORD error=GetLastError();~LastErrorScope(){SetLastError(error);}};
struct Sample{bool diagnostic{};};
struct Local{model_light_submission20::State state;std::array<Sample,MaxDepth> samples;} local;
uint32_t currentFrame=7;
uint32_t Frame()noexcept{return currentFrame;}
bool Copy(uintptr_t at,void* out,size_t n)noexcept{
 ++copies;if(!Readable(at,n)||!RawCopy(at,out,n))return false;if(afterCopy)afterCopy(at,n);return true;
}
uint32_t FinishPoint68(SafetyHookContext&,uint32_t,const std::array<uint32_t,18>&,const PointEvidence&,
 const Invocation*,Sample&,std::array<uint32_t,32>*)noexcept{assert(false);return 0;}
#include "ModelPointValue68Runtime.inc"
}
static ModelEffectObserver::ModelValue68::Binding ModelBinding69(const native_projection66::ConstructorContext& c)noexcept{
 return {c.caller,c.returnSlot,c.destination,c.position,c.color,c.type,c.flags,c.key};
}
namespace CShadows {uint32_t* pFrameCounter=&ModelEffectObserver::currentFrame;}
namespace Liberty20 {bool CopyLocal(uintptr_t p,void* out,size_t n)noexcept{return RawCopy(p,out,n);}}
namespace NativeProjection66 {
using native_projection66::Packet;
arcade_placement197::Projections privateProjections197;
native_class_domains105::Registry<> registry;
native_class_domains105::IssuedKeys<> issuedKeys;
thread_local native_projection66::AliasPass<> aliases;
thread_local native_projection66::Sidecar<> sidecar;
std::atomic<bool> active{true};std::atomic_flag lock=ATOMIC_FLAG_INIT;
std::atomic<uint32_t> accepted{},invalidQueued{},aliasPackets{},contended{},bankPreflights{},bankDenied{},packetFaults{},permissionQueries{},winnerPreflights{},winnerDenied{};
bool (*placementBank197)(rage::CLightSource&,Packet&,uint32_t)noexcept=nullptr;
#include "projection66-read-extracted.inc"
#include "projection66-queue-extracted.inc"
}
#include "ArcadePlacement197Runtime.inc"
using arcade_placement197::Packet;
using arcade_placement197::Matrix;
uint8_t *memory{},*gameImage{};
rage::VehiclePool* pools{};
uint8_t* source197{};uint8_t* fixture{};uint8_t* descriptor{};uint32_t* effect{};
Matrix* matrix{};Matrix* fixtureMatrix{};float* position{};
native_projection66::ConstructorContext binding{};
Packet Original(unsigned ordinal){
 Packet p{};p[0]=std::bit_cast<uint32_t>(1.f);p[5]=std::bit_cast<uint32_t>(1.f);
 for(unsigned j=0;j<3;++j)p[8+j]=arcade_placement197::Bits(arcade_placement197::OriginalWorld[ordinal][j]);
 p[12]=arcade_placement197::Bits(1.f);p[13]=arcade_placement197::Bits(.9647059440612793f);p[14]=arcade_placement197::Bits(.7372549176216125f);
 p[16]=arcade_placement197::Bits(20.f);p[17]=0;p[18]=0x41;p[21]=arcade_placement197::Bits(10.f);p[25]=UINT32_MAX;return p;
}
bool Gate(uint32_t,uint32_t,uint32_t)noexcept{return true;}
template<class T>void Put(void* base,size_t at,T value){std::memcpy(static_cast<uint8_t*>(base)+at,&value,sizeof(value));}
void Reset(unsigned ordinal=0){
 using namespace ArcadePlacement197;using namespace ModelEffectObserver;
 DWORD old{};assert(VirtualProtect(memory,65536,PAGE_READWRITE,&old));std::memset(memory,0,65536);
 requested=assetsAuthenticated=true;leases={};pending={};leaseLock.clear();
 NativeProjection66::placementBank197=ArcadePlacement197::Bank;
 NativeProjection66::privateProjections197.registry={};for(auto& w:NativeProjection66::privateProjections197.issued.words)w=0;NativeProjection66::privateProjections197.Init();
 fixtureHint=0;scanPool=0;scanSlots={};scanFrame=UINT32_MAX;currentFrame=7;afterCopy=nullptr;
 pools=reinterpret_cast<rage::VehiclePool*>(memory);source197=memory+4096;fixture=source197+0x70;
 descriptor=memory+8192;effect=reinterpret_cast<uint32_t*>(memory+12288);
 matrix=reinterpret_cast<Matrix*>(memory+16384);fixtureMatrix=matrix+1;position=reinterpret_cast<float*>(memory+20480);
 *matrix={};(*matrix)[0]=(*matrix)[5]=(*matrix)[10]=(*matrix)[15]=1.f;
 for(unsigned j=0;j<3;++j)(*matrix)[12+j]=arcade_placement197::Origin[j];*fixtureMatrix=*matrix;
 for(unsigned k=0;k<4;++k){const uint32_t stride=k==0?0x70:k==1?0x320:k==2?0x80:0x110;
  pools[k]={k?memory+24576+k*1024:source197,memory+28672+k*128,k?1:2,int32_t(stride),0,0};pools[k].m_aFlags[0]=7;
 }
 pools[0].m_aFlags[1]=9;
 constexpr uint32_t roots[]{0xebd0e8,0x1232c60,0x126d9ec,0x12f7d60};
 for(unsigned k=0;k<4;++k)Put(gameImage,roots[k],uint32_t(uintptr_t(pools+k)));
 auto* sourceInfo=memory+32768;auto* fixtureInfo=sourceInfo+128;
 Put(sourceInfo,0x3c,arcade_placement197::SourceModel);Put(fixtureInfo,0x3c,arcade_placement197::FixtureModel);
 Put(gameImage,0xe95cd8+5*4,uint32_t(uintptr_t(sourceInfo)));Put(gameImage,0xe95cd8+6*4,uint32_t(uintptr_t(fixtureInfo)));
 Put(source197,0x2e,int16_t(5));Put(fixture,0x2e,int16_t(6));Put(source197,0x20,uint32_t(uintptr_t(matrix)));Put(fixture,0x20,uint32_t(uintptr_t(fixtureMatrix)));
 descriptor[0x5b]=1;descriptor[0x28]=255;descriptor[0x29]=246;descriptor[0x2a]=188;
 Put(descriptor,0x38,10.f);Put(descriptor,0x3c,20.f);effect[0]=game+0x100;
 for(unsigned j=0;j<3;++j){effect[j+1]=arcade_placement197::Bits(arcade_placement197::Local[ordinal][j]);position[j]=arcade_placement197::OriginalWorld[ordinal][j];}
 local.state={};local.samples={};ModelValue68::pending={};ModelValue68::canReplace=Gate;
 Invocation c{};c.frame=7;c.nonce=11;c.owner=uint32_t(uintptr_t(source197));c.effect=uint32_t(uintptr_t(effect));
 c.matrix=uint32_t(uintptr_t(matrix));c.modelInfo=uint32_t(uintptr_t(sourceInfo));c.ordinal=ordinal;c.preStack=uint32_t(uintptr_t(memory+40000));c.proposedKey=c.owner+ordinal;c.sampled=true;
 assert(local.state.Enter(c));
 ResolveEvidence resolve{7,c.preStack,DispatchReturn,c.effect,c.matrix,c.owner,c.effect,uint32_t(uintptr_t(descriptor)),c.proposedKey};assert(local.state.Resolve(resolve));
 PointEvidence point{7,c.preStack,DispatchReturn,c.effect,uint32_t(uintptr_t(descriptor)),WrapperPoint,0,1,0};assert(local.state.Submission(point));
 SafetyHookContext regs{};regs.esp=uint32_t(uintptr_t(memory+45000));
 std::array<uint32_t,18> arguments{};arguments[1]=0;arguments[2]=1;arguments[5]=uint32_t(uintptr_t(position));arguments[6]=uint32_t(uintptr_t(memory+21000));
 assert(ModelValue68::TryDefer(regs,WrapperPoint,arguments,*local.state.Current(),local.samples[0]));
 const auto ordinaryDeferred=ModelValue68::pending[0];
 ModelValue68::ClearDepth(1);local.samples[0].diagnostic=true;
 assert(ModelValue68::TryDefer(regs,WrapperPoint,arguments,*local.state.Current(),local.samples[0]));
 assert(ModelValue68::pending[0].arguments==ordinaryDeferred.arguments&&ModelValue68::pending[0].site==ordinaryDeferred.site);
 local.samples[0].diagnostic=false;
 const auto slot=ModelValue68::ReturnSlot(regs.esp,WrapperPoint);
 binding={game+0x6bd194,slot,slot+0xa8,0,0,arguments[5],arguments[6],0,0x41,0,0,0};
 assert(ModelValue68::Matches(ModelBinding69(binding)));copies=ownerCalls=queryCalls=0;
}
bool Apply(Packet& p){const ModelEffectObserver::LastErrorScope e;const fusionfix::shadows::FloatingPointState fp;Packet original;return ArcadePlacement197::Apply(p,original,7,binding);}
Packet Promote(unsigned ordinal=0){Reset(ordinal);auto p=Original(ordinal);assert(Apply(p));assert(NativeProjection66::privateProjections197.Intern(p,7,[](uint32_t){return false;}));
 assert(ArcadePlacement197::Finish(p,7,binding,true));return p;}
int main(){
 memory=static_cast<uint8_t*>(VirtualAlloc(nullptr,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(memory);
 gameImage=static_cast<uint8_t*>(VirtualAlloc(nullptr,0x1300000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(gameImage);game=uint32_t(uintptr_t(gameImage));
 for(unsigned ordinal=0;ordinal<2;++ordinal){Reset(ordinal);auto p=Original(ordinal),before=p;
  const auto originalMatrix=*matrix;SetLastError(0x12345678);const auto csr=_mm_getcsr();assert(Apply(p));assert(GetLastError()==0x12345678&&_mm_getcsr()==csr);
  for(unsigned word=0;word<32;++word)if(word<8||word>10)assert(p[word]==before[word]);
  const auto expected=arcade_placement197::Transform(*fixtureMatrix,arcade_placement197::RepairedLocal[ordinal]);
  for(unsigned j=0;j<3;++j)assert(p[8+j]==arcade_placement197::Bits(expected[j]));assert(*matrix==originalMatrix);
  assert(std::bit_cast<float>(p[10])-std::bit_cast<float>(before[10])>1.65f);
 }
 // Unrelated numeric inputs, disabled feature and unauthenticated assets never
 // issue one extra owner/permission/descriptor query or alter any word.
 for(unsigned n=0;n<10;++n){Reset();auto p=Original(0);if(n<8){constexpr unsigned fields[]{8,9,10,12,13,14,16,21};p[fields[n]]^=1;}else if(n==8)ArcadePlacement197::requested=false;else ArcadePlacement197::assetsAuthenticated=false;
  const auto before=p;assert(!Apply(p)&&p==before&&copies==0&&ownerCalls==0&&queryCalls==0);
 }
 for(unsigned n=0;n<11;++n){Reset();auto p=Original(0),before=p;
  switch(n){case 0:Put(memory+32768,0x3c,uint32_t(123));break;case 1:pools[0].m_aFlags[0]|=0x80;break;
   case 2:descriptor[0x28]^=1;break;case 3:effect[1]^=1;break;case 4:(*fixtureMatrix)[14]+=.1f;break;
   case 5:pools[0].m_aFlags[1]|=0x80;break;case 6:position[0]+=.1f;break;case 7:ModelEffectObserver::local.state.Current()->ordinal=1;break;
   case 8:++binding.returnSlot;break;case 9:(*matrix)[12]+=.1f;break;case 10:ModelEffectObserver::local.state.Current()->matrix+=64;break;}
  assert(!Apply(p)&&p==before);
 }
 // Actual descriptor snapshot changes between reads must fail, even if the
 // altered byte has no interpreted semantic meaning in this policy.
 Reset();afterCopy=[](uintptr_t at,size_t){if(at==uintptr_t(descriptor)){descriptor[0x10]^=1;afterCopy=nullptr;}};
 {auto p=Original(0),before=p;assert(!Apply(p)&&p==before);}
 Reset();afterCopy=[](uintptr_t at,size_t){if(at==uintptr_t(descriptor)){ModelEffectObserver::currentFrame=8;afterCopy=nullptr;}};
 {auto p=Original(0),before=p;assert(!Apply(p)&&p==before);}
 // Fresh current-bank checks and exact restore after each distinct revocation.
 for(unsigned n=0;n<8;++n){auto p=Promote();rage::CLightSource light{p};assert(ArcadePlacement197::Bank(light,p,7));
  DWORD old{};switch(n){case 0:++pools[0].m_aFlags[0];break;case 1:++pools[0].m_aFlags[1];break;case 2:descriptor[0x10]^=1;break;
   case 3:effect[2]^=1;break;case 4:(*fixtureMatrix)[12]+=.1f;break;case 5:ArcadePlacement197::requested=false;break;
   case 6:assert(VirtualProtect(descriptor,4096,PAGE_NOACCESS,&old));break;case 7:(*matrix)[13]+=.1f;break;}
  assert(!ArcadePlacement197::Bank(light,p,7));assert(p==Original(0)&&light.words==Original(0));
  if(n==6)assert(VirtualProtect(descriptor,4096,old,&old));
 }
 {auto p=Promote();rage::CLightSource light{p};assert(!ArcadePlacement197::Bank(light,p,8));assert(p==Original(0));}
 {auto p=Promote();rage::CLightSource light{p};afterCopy=[](uintptr_t at,size_t){if(at==uintptr_t(descriptor)){ModelEffectObserver::currentFrame=8;afterCopy=nullptr;}};
  assert(!ArcadePlacement197::Bank(light,p,7)&&p==Original(0));}
 {Reset();auto p=Original(0);assert(Apply(p));assert(NativeProjection66::privateProjections197.Intern(p,7,[](uint32_t){return false;}));ModelEffectObserver::currentFrame=8;
  assert(!ArcadePlacement197::Finish(p,7,binding,true)&&!ArcadePlacement197::leases[0].present);}
 // A changed current-bank packet is rejected and never overwritten with saved
 // state. Full metadata mutation is independently witnessed by projection66.
 {auto p=Promote();rage::CLightSource light{p};p[12]^=1;light.words=p;const auto before=p;
  assert(!ArcadePlacement197::Bank(light,p,7)&&p==before&&light.words==before);}
 // Failed publication cannot leave a lease, and retired numeric keys do not
 // authorize a later unrelated point. Nested read scopes restore their caller.
 Reset();{traffic_signal::fresh_page::Operation44 outer;immediate_region33::Scope region;
  auto p=Original(0);assert(Apply(p));assert(traffic_signal::fresh_page::operation44==&outer&&immediate_region33::current==&region);
  assert(!ArcadePlacement197::Finish(p,7,binding,false));assert(!ArcadePlacement197::leases[0].present);}
 // The exact helper used by production Constructor restores the native-owned
 // packet before legacy fallback for either issuance or publication failure.
 for(unsigned outcome=0;outcome<4;++outcome){Reset();auto p=Original(0);const auto before=p;unsigned calls=0;
  const bool success=arcade_placement197::Adapt(p,outcome!=3,
   [&](Packet& owned,Packet& original)noexcept{return ArcadePlacement197::Apply(owned,original,7,binding);},
   [&](Packet& owned,bool relocated)noexcept{++calls;if(outcome==1)return false;
    if(relocated)return NativeProjection66::privateProjections197.Intern(owned,7,[](uint32_t){return false;})!=0;
    owned[24]=0x50000000;owned[18]|=4;return true;},
   [&](Packet& owned,bool issued)noexcept{if(outcome==2)ArcadePlacement197::leaseLock.test_and_set();
    const auto ok=ArcadePlacement197::Finish(owned,7,binding,issued);ArcadePlacement197::leaseLock.clear();return ok;});
  assert(calls==1&&success==(outcome==0||outcome==3));
  if(outcome==1||outcome==2)assert(p==before&&!ArcadePlacement197::leases[0].present);
  if(outcome==3){for(unsigned j=0;j<3;++j)assert(p[8+j]==before[8+j]);assert(copies==0&&ownerCalls==0);}
 }
 // A second projection for one ordinal retires the previous key but retains
 // its exact original packet, so old current-bank storage can be restored.
 {auto old=Promote();auto next=Original(0);next[26]=1;assert(Apply(next));
  assert(NativeProjection66::privateProjections197.Intern(next,7,[](uint32_t){return false;}));assert(next[24]!=old[24]);
  assert(ArcadePlacement197::Finish(next,7,binding,true));rage::CLightSource oldBank{old},nextBank{next};
  assert(!ArcadePlacement197::Bank(oldBank,old,7)&&old==Original(0));assert(ArcadePlacement197::Bank(nextBank,next,7));
  ArcadePlacement197::leases={};assert(!ArcadePlacement197::Bank(nextBank,next,7));
 }
 // Full production queued/CollectDecision path, with the real lifetime Bank
 // callback. A previously accepted key becomes invalid after a second-key
 // publication; missing leases also fail closed, including after queue setup.
 {auto old=Promote();rage::CLightSource lights[2]{{old},{}};
  NativeProjection66::ValidateQueued(lights,1);assert(NativeProjection66::CollectDecision(0,lights,1,7,0x1000)==1);
  auto next=Original(0);next[26]=1;assert(Apply(next));assert(NativeProjection66::privateProjections197.Intern(next,7,[](uint32_t){return false;}));
  assert(ArcadePlacement197::Finish(next,7,binding,true));
  assert(NativeProjection66::CollectDecision(0,lights,1,7,0x1000)==3&&lights[0].words==Original(0));
  lights[0].words=old;lights[1].words=next;NativeProjection66::ValidateQueued(lights,2);
  assert(NativeProjection66::CollectDecision(0,lights,2,7,0x2000)==0);
  assert(NativeProjection66::CollectDecision(1,lights,2,7,0x2000)==1);
  ArcadePlacement197::leases={};assert(NativeProjection66::CollectDecision(1,lights,2,7,0x2000)==3);
  NativeProjection66::ValidateQueued(lights,2);assert(NativeProjection66::CollectDecision(1,lights,2,7,0x3000)==3);
 }
 // Current-bank permission revocation after approval is contained by SEH.
 {auto p=Promote();auto* bank=reinterpret_cast<rage::CLightSource*>(memory+57344);bank->words=p;
  assert(Readable(uintptr_t(bank),120));DWORD old{};assert(VirtualProtect(bank,4096,PAGE_NOACCESS,&old));
  assert(!ArcadePlacement197::RestoreBank(*bank,p,Original(0)));assert(VirtualProtect(bank,4096,old,&old));assert(bank->words==p);}
 // Identical unrelated projections retain separate identities and never enter
 // the arcade's retirement route. Capacity is hard-bounded and never recycled.
 {arcade_placement197::Projections privateKeys;privateKeys.Init();native_projection66::Registry<4> ordinary;ordinary.Init(0x60000000);
  for(unsigned ordinal=0;ordinal<2;++ordinal){Reset(ordinal);auto p=Original(ordinal);assert(Apply(p));auto ordinaryPacket=p;
   const auto normal=ordinary.Observe(ordinaryPacket,7,[](uint32_t){return false;});
   const auto special=privateKeys.Intern(p,7,[](uint32_t){return false;});assert(normal&&special&&normal!=special);
   assert(privateKeys.Owns(special)&&!privateKeys.Owns(normal)&&privateKeys.Validate(special,p));
  }
  auto third=Original(0);third[8]^=1;assert(!privateKeys.Intern(third,8,[](uint32_t){return false;}));assert(privateKeys.registry.used==2);
 }
 std::puts("arcade197: exact production owner/association/placement/restore; both ordinals; numeric no-read gate; mutations/lifetimes/nested scopes/SEH passed");
 VirtualFree(memory,0,MEM_RELEASE);VirtualFree(gameImage,0,MEM_RELEASE);
 return 0;
}

