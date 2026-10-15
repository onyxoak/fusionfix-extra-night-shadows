#define NOMINMAX
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <bit>
#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cfenv>
#include "ExperimentRuntimeOptions.hpp"
#include "NativeClassDomains105.hpp"
#include "NativeProjectionRead66.hpp"
#include "core/DeferredModelShortlist48.hpp"
#include "core/ShadowFloatingPointState.hpp"
#include "core/ShadowViewPriority.hpp"
#include "core/ShadowVolumeVisibility43.hpp"
#include "core/SceneShadowPriority21.hpp"
#include "core/ShadowCpuProfile29.hpp"
namespace rage {
struct Vector4 {float x{},y{},z{},w{};};
struct CLightSource {Vector4 mDirection{},mTangent{},mPosition{},mColor{};float mIntensity{};uint32_t mType{},mFlags{},texture1{},texture2{};float mRadius{},innerCone{},outerCone{};uint32_t mCastShadows{},cache{},interior{},room{};float volumeIntensity{},volumeScale{};uint32_t padding[2]{};};
static_assert(sizeof(CLightSource)==128);static_assert(offsetof(CLightSource,mCastShadows)==96);static_assert(offsetof(CLightSource,mFlags)==72);
}
uintptr_t game=0x400000;
std::array<rage::CLightSource,640> bank108{};
uint32_t count108=512;
bool measure108=true;
namespace CShadows {uint32_t frame=10;uint32_t* pFrameCounter=&frame;}
struct SafetyHookContext {uintptr_t esp{},eip{},edi{};uint32_t edx{},esi{},eax{};};
namespace ShadowTrace34 {struct Event{uint32_t a,b,c,d;int e,f,g,h;float i,j,k,l;};void Emit(const Event&) noexcept{}}
namespace PlayerShadowAllocation {
using FloatingPointState=fusionfix::shadows::FloatingPointState;
struct Pass {bool active=true;bool Active()const noexcept{return active;}};
struct State {Pass pass;unsigned depth=1;bool continuityActive=true,tracedComparison=false;uintptr_t stackAnchor{};uint32_t frame=10;rage::CLightSource* shortlistLights48=bank108.data();uint32_t shortlistCount48=512;fusionfix::shadows::ShadowView view{};std::array<fusionfix::shadows::NativeShadowContinuity42::Candidate,4096> nativeCandidates{};}state;
uintptr_t gameBase=game;
std::array<uint8_t,4096> labRelevant{};
std::array<std::atomic<uint32_t>,3> labComparisons{},labResults{};
std::atomic<uint32_t> labOwnLoss{},labClaimLoss{},continuityRejected{},continuityComparisons{},continuityOverrides{},projectionZeroWrites66{},projectionWouldWrites66{};
bool (*labPointSource)(const rage::CLightSource&) noexcept=nullptr;
int (*labCompareGrace)(int,int,int,int) noexcept=nullptr;
bool (*labGracePairNeutral108)(int (*)(int,int,int,int) noexcept,uint32_t,uint32_t) noexcept=nullptr;
unsigned (*projectionCollect66)(unsigned,rage::CLightSource*,uint32_t,uint32_t,uintptr_t) noexcept=nullptr;
bool enabled108=true;
bool Enabled() noexcept{return enabled108;}
rage::CLightSource* CurrentLights() noexcept{return bank108.data();}
uint32_t CurrentCount() noexcept{return count108;}
#include "preview108.inc"
#include "compare108.inc"
}
namespace Liberty20 {
#include "copy-local.inc"
bool Collision(uint32_t) noexcept{return false;} // Fixture-owned disjoint token arenas only.
}
namespace Owned108 {
using native_projection66::Packet;
thread_local native_projection66::AliasPass<> aliases;
thread_local native_projection66::Sidecar<> sidecar;
std::atomic<bool> active{true};std::atomic_flag lock=ATOMIC_FLAG_INIT;
std::atomic<uint32_t> accepted{},invalidQueued{},aliasPackets{},contended{},bankPreflights{},bankDenied{},packetFaults{},permissionQueries{},winnerPreflights{},winnerDenied{},capacityFallback{},tokensUsed74{};
alignas(64)uint8_t tokens[4096][64]{},classTokens105[3][4096][64]{};
native_class_domains105::Registry<> registry;
native_class_domains105::IssuedKeys<> issuedKeys;
std::array<std::atomic<uint32_t>,4> classTokensUsed105{};
#include "intern108.inc"
#include "read108.inc"
#include "queued108.inc"
}
lab_siren::NearbyPolice nearbyPolice;
alignas(64)std::array<std::array<uint8_t,64>,lab_siren::NearbyPolice::Tokens> nearbyKeys{};
std::atomic_flag nearbyLock=ATOMIC_FLAG_INIT;
unsigned locks108{},ownerChecks108{};
struct NearbyLock {bool acquired;NearbyLock() noexcept:acquired(!nearbyLock.test_and_set(std::memory_order_acquire)){++locks108;}~NearbyLock(){if(acquired)nearbyLock.clear(std::memory_order_release);}};
std::atomic<uint32_t> policePolicy{1};std::atomic<bool> proofRequested{true};bool nearbyPoliceEnabled=true,newPath108=true;
uint32_t NearbyKey(unsigned i) noexcept{return uint32_t(uintptr_t(nearbyKeys[i].data()));}
bool NearbyPoint(const rage::CLightSource& light) noexcept{return experiment_perf::TokenIndex(light.mCastShadows,NearbyKey(0),nearbyKeys.size())<nearbyKeys.size();}
lab_siren::NearbyPolice::Owner* NearbyOwnerForKey(uint32_t key) noexcept {const auto token=experiment_perf::TokenIndex(key,NearbyKey(0),nearbyKeys.size());if(token==nearbyKeys.size())return nullptr;for(auto& o:nearbyPolice.owners)if(o.live&&o.token==token)return &o;return nullptr;}
bool NearbyValid(lab_siren::NearbyPolice::Owner& owner) noexcept{++ownerChecks108;return owner.live;} // Owner validation cost is not fabricated.
int CompareNearby105(int,int,int,int) noexcept;
int CompareNearby(int,int,int,int) noexcept;
int ComparePoliceClaim(int original,int unchanged,int challenger,int incumbent) noexcept{return newPath108?CompareNearby(original,unchanged,challenger,incumbent):CompareNearby105(original,unchanged,challenger,incumbent);}
#include "nearby105.inc"
#include "nearby108.inc"
using namespace PlayerShadowAllocation;
std::array<uint8_t,256> stack108{};
void Gate105(SafetyHookContext& regs,unsigned index,unsigned count,unsigned projectionClass){auto* lights=bank108.data();
 const FloatingPointState fp;
#include "gate105.inc"
}
void Gate108(SafetyHookContext& regs,unsigned index,unsigned count,unsigned projectionClass){auto* lights=bank108.data();
 const FloatingPointState fp;
#include "gate108.inc"
}
void Store108(unsigned offset,uint32_t value){std::memcpy(stack108.data()+offset,&value,4);}
uint32_t Load108(unsigned offset){uint32_t value{};std::memcpy(&value,stack108.data()+offset,4);return value;}
struct Result108 {std::array<int32_t,7> indices{};unsigned checks{},queries{},locks{},owners{},skips{};double ms{};};
Result108 Run108(bool next){
 newPath108=next;labCompareGrace=ComparePoliceClaim;labGracePairNeutral108=next?NearbyGracePairNeutral108:nullptr;
 CShadows::frame=state.frame=10;state.depth=1;state.pass.active=true;state.continuityActive=true;state.stackAnchor=uintptr_t(stack108.data());state.tracedComparison=false;stack108={};
 for(unsigned row=0;row<7;++row){Store108(0x7C+row*4,UINT32_MAX);Store108(0x9C+row*4,0);Store108(0x5C+row*4,0);}
 locks108=ownerChecks108=0;Owned108::winnerPreflights=0;Owned108::permissionQueries=0;projectionZeroWrites66=0;
 LARGE_INTEGER a{},b{},f{};if(measure108){QueryPerformanceFrequency(&f);QueryPerformanceCounter(&a);}
 Owned108::ValidateQueued(bank108.data(),count108);
 for(unsigned i=0;i<count108;++i){
  Store108(0x20,std::bit_cast<uint32_t>(float(i+1)));Store108(0x14,i);Store108(0x18,i*128);Store108(0x1C,bank108[i].mFlags);
  SafetyHookContext regs{};regs.esp=uintptr_t(stack108.data());const auto cls=Owned108::Classify(i,bank108[i].mCastShadows,bank108.data(),count108,10);
  if(next)Gate108(regs,i,count108,cls);else Gate105(regs,i,count108,cls);
  if(regs.eip==game+0x527DDC)continue;
  int32_t carry=int32_t(i);uint32_t flags=bank108[i].mFlags;float distance=float(i+1);
  for(unsigned row=0;row<7;++row){
   const auto incumbent=int32_t(Load108(0x7C+row*4));const auto cellFlags=Load108(0x9C+row*4);const auto cellDistance=std::bit_cast<float>(Load108(0x5C+row*4));
   Store108(0x24,uint32_t(carry));regs.edi=flags;regs.esi=row*4;regs.eax=uint32_t(fusionfix::shadows::shortlist48::NativeFlags(flags,cellFlags));CompareNativeCandidates(regs);
   const auto choice=int(regs.eax);if(choice==0||(choice==1&&cellDistance>distance)){
    Store108(0x7C+row*4,uint32_t(carry));Store108(0x9C+row*4,flags);Store108(0x5C+row*4,std::bit_cast<uint32_t>(distance));carry=incumbent;flags=cellFlags;distance=cellDistance;
   }
  }
 }
 if(measure108)QueryPerformanceCounter(&b);Result108 result{};for(unsigned row=0;row<7;++row)result.indices[row]=int32_t(Load108(0x7C+row*4));
 result.checks=Owned108::winnerPreflights;result.queries=Owned108::permissionQueries;result.locks=locks108;result.owners=ownerChecks108;result.skips=projectionZeroWrites66;if(measure108)result.ms=1000.0*double(b.QuadPart-a.QuadPart)/double(f.QuadPart);return result;
}
int Unknown108(int,int unchanged,int,int) noexcept{return unchanged;}
int main(int argc,char** argv){
 if(argc!=1&&(argc!=2||std::strcmp(argv[1],"--checks")))return 2;
 measure108=argc==1;
 SetPriorityClass(GetCurrentProcess(),BELOW_NORMAL_PRIORITY_CLASS);
 projectionCollect66=Owned108::CollectDecision;
 assert(Owned108::registry.Init({uint32_t(uintptr_t(Owned108::classTokens105[0])),uint32_t(uintptr_t(Owned108::tokens)),uint32_t(uintptr_t(Owned108::classTokens105[1])),uint32_t(uintptr_t(Owned108::classTokens105[2]))}));
 for(unsigned i=0;i<count108;++i){native_projection66::Packet p{};p[2]=std::bit_cast<uint32_t>(-1.f);p[5]=std::bit_cast<uint32_t>(1.f);p[8]=std::bit_cast<uint32_t>(float(i));p[12]=p[13]=p[14]=p[16]=std::bit_cast<uint32_t>(1.f);p[17]=0;p[18]=0x41;p[21]=std::bit_cast<uint32_t>(10.f);p[25]=UINT32_MAX;p[26]=UINT32_MAX;p[27]=63;
  assert(Owned108::Intern(p,10));std::memcpy(&bank108[i],p.data(),128);auto& c=state.nativeCandidates[i];c={};c.observed=true;c.flags=bank108[i].mFlags;c.identity=bank108[i].mCastShadows;c.focusDistance=float(i+1);labRelevant[i]=1;
 }
 for(unsigned scenario:{0u,1u,2u}){
  const bool emergency=scenario!=0;const unsigned emergencyIndex=scenario==2?511u:0u;
  const auto originalKey=bank108[emergencyIndex].mCastShadows;if(emergency){bank108[emergencyIndex].mCastShadows=NearbyKey(0);state.nativeCandidates[emergencyIndex].identity=NearbyKey(0);auto& owner=nearbyPolice.owners[0];owner.live=true;owner.eligible=true;owner.rank=0;owner.token=0;}
  std::vector<double> before,after;Result108 lastOld{},lastNew{};
  for(unsigned rep=0;rep<(measure108?40u:1u);++rep){Result108 old{},now{};if(rep&1){now=Run108(true);old=Run108(false);}else{old=Run108(false);now=Run108(true);}assert(old.indices==now.indices);if(rep>=8){before.push_back(old.ms);after.push_back(now.ms);}lastOld=old;lastNew=now;}
  if(!emergency){assert(lastOld.checks==512&&lastNew.checks==7&&lastNew.skips==505);assert(lastNew.locks==0);}else if(scenario==1){assert(lastOld.checks==511&&lastNew.checks==511&&lastNew.skips==0);assert(lastOld.owners==lastNew.owners);}else{assert(lastOld.checks==511&&lastNew.checks==7&&lastNew.skips==504);assert(lastOld.owners==lastNew.owners);}
  if(measure108){std::sort(before.begin(),before.end());std::sort(after.begin(),after.end());std::printf("108 full-prefix scenario=%s n=32 old_checks=%u new_checks=%u old_queries=%u new_queries=%u old_locks=%u new_locks=%u old_owners=%u new_owners=%u skipped=%u old_p50_ms=%.6f new_p50_ms=%.6f old_p95_ms=%.6f new_p95_ms=%.6f\n",scenario==1?"police_incumbent":scenario==2?"police_challenger":"ordinary",lastOld.checks,lastNew.checks,lastOld.queries,lastNew.queries,lastOld.locks,lastNew.locks,lastOld.owners,lastNew.owners,lastNew.skips,before[16],after[16],before[30],after[30]);}
  bank108[emergencyIndex].mCastShadows=originalKey;state.nativeCandidates[emergencyIndex].identity=originalKey;nearbyPolice.owners={};
 }
 // Exact route/range guards and mutations do not borrow an earlier certificate.
 Run108(true);GracePrefix108 proof{};const auto address=uintptr_t(stack108.data());Store108(0x20,std::bit_cast<uint32_t>(1000.f));
 assert(CaptureGracePrefix108(address,511,bank108.data(),512,proof));assert(CurrentGracePrefix108(address,511,bank108.data(),512,proof));
 assert(!PreviewWrites48(address,511));
 const auto key=bank108[511].mCastShadows;bank108[511].mCastShadows=NearbyKey(0);assert(!CurrentGracePrefix108(address,511,bank108.data(),512,proof));assert(!CaptureGracePrefix108(address,511,bank108.data(),512,proof));bank108[511].mCastShadows=key;
 labCompareGrace=Unknown108;assert(!CaptureGracePrefix108(address,511,bank108.data(),512,proof));labCompareGrace=ComparePoliceClaim;
 assert(CaptureGracePrefix108(address,511,bank108.data(),512,proof));labCompareGrace=Unknown108;assert(!CurrentGracePrefix108(address,511,bank108.data(),512,proof));labCompareGrace=ComparePoliceClaim;
 state.depth=2;assert(!CaptureGracePrefix108(address,511,bank108.data(),512,proof));state.depth=1;
 Store108(0x7C,600);assert(!CaptureGracePrefix108(address,511,bank108.data(),512,proof));Store108(0x7C,0);
 assert(CaptureGracePrefix108(address,511,bank108.data(),512,proof));++CShadows::frame;assert(!CurrentGracePrefix108(address,511,bank108.data(),512,proof));--CShadows::frame;
 for(unsigned field=0;field<9;++field){
  auto& c=state.nativeCandidates[0];const auto saved=c;assert(CaptureGracePrefix108(address,511,bank108.data(),512,proof));
  switch(field){case 0:c.flags^=0x100;break;case 1:++c.claim;break;case 2:c.ownBeam=!c.ownBeam;break;case 3:c.observed=!c.observed;break;case 4:++c.stableOrder;break;case 5:c.focusDistance=std::bit_cast<float>(std::bit_cast<uint32_t>(c.focusDistance)^1u);break;case 6:++c.focusTier;break;case 7:++c.identity;break;case 8:c.visualImportance=std::bit_cast<float>(std::bit_cast<uint32_t>(c.visualImportance)^1u);break;}
  assert(!CurrentGracePrefix108(address,511,bank108.data(),512,proof));c=saved;
 }
 for(unsigned offset:{0x7Cu,0x9Cu,0x5Cu,0x20u}){assert(CaptureGracePrefix108(address,511,bank108.data(),512,proof));const auto saved=Load108(offset);Store108(offset,saved^1u);assert(!CurrentGracePrefix108(address,511,bank108.data(),512,proof));Store108(offset,saved);}
 const auto oldCount=count108;assert(CaptureGracePrefix108(address,511,bank108.data(),512,proof));count108=511;assert(!CurrentGracePrefix108(address,511,bank108.data(),512,proof));count108=oldCount;
 assert(CaptureGracePrefix108(address,511,bank108.data(),512,proof));nearbyPoliceEnabled=false;assert(!CurrentGracePrefix108(address,511,bank108.data(),512,proof));assert(!CaptureGracePrefix108(address,511,bank108.data(),512,proof));nearbyPoliceEnabled=true;
 assert(CaptureGracePrefix108(address,511,bank108.data(),512,proof));labGracePairNeutral108=nullptr;assert(!CurrentGracePrefix108(address,511,bank108.data(),512,proof));labGracePairNeutral108=NearbyGracePairNeutral108;
 assert(CaptureGracePrefix108(address,511,bank108.data(),512,proof));enabled108=false;assert(!CurrentGracePrefix108(address,511,bank108.data(),512,proof));assert(!CaptureGracePrefix108(address,511,bank108.data(),512,proof));enabled108=true;
 assert(NearbyKeysNeutral108(NearbyKey(0)+1,key));assert(!NearbyKeysNeutral108(NearbyKey(0),key));assert(!NearbyGracePairNeutral108(Unknown108,key,key));
 SetLastError(0x1234abcd);assert(CaptureGracePrefix108(address,511,bank108.data(),512,proof));assert(CurrentGracePrefix108(address,511,bank108.data(),512,proof));assert(GetLastError()==0x1234abcd);
 const auto oldRound=std::fegetround();std::fesetround(FE_DOWNWARD);std::feclearexcept(FE_ALL_EXCEPT);std::feraiseexcept(FE_INEXACT);const auto csr=_mm_getcsr();const auto except=std::fetestexcept(FE_ALL_EXCEPT);
 SafetyHookContext fpRegs{};fpRegs.esp=address;Owned108::ValidateQueued(bank108.data(),512);const auto cls=Owned108::Classify(511,bank108[511].mCastShadows,bank108.data(),512,10);const auto csrAfterPrepare=_mm_getcsr();const auto exceptAfterPrepare=std::fetestexcept(FE_ALL_EXCEPT);SetLastError(0x1234abcd);Gate108(fpRegs,511,512,cls);assert(std::fegetround()==FE_DOWNWARD&&_mm_getcsr()==csrAfterPrepare&&std::fetestexcept(FE_ALL_EXCEPT)==exceptAfterPrepare&&GetLastError()==0x1234abcd);(void)csr;(void)except;std::fesetround(oldRound);std::feclearexcept(FE_ALL_EXCEPT);
 const auto fullGate=[&](){Owned108::winnerPreflights=0;SafetyHookContext regs{};regs.esp=address;Gate108(regs,511,512,cls);assert(Owned108::winnerPreflights==1&&regs.eip==0);};
 labCompareGrace=Unknown108;fullGate();labCompareGrace=ComparePoliceClaim;
 nearbyPoliceEnabled=false;fullGate();nearbyPoliceEnabled=true;
 labGracePairNeutral108=nullptr;fullGate();labGracePairNeutral108=NearbyGracePairNeutral108;
 enabled108=false;fullGate();enabled108=true;
 std::puts("PASS actual prefix/gate/compare/nearby/owned-witness extraction, ordinary/police parity, unknown callback, key/index/frame/reentry guards; native owner proof cost not simulated, no FPS claim.");return 0;
}
