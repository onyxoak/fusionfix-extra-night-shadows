#include "FreshPageReadable.hpp"
#include "ImmediateRegion33.hpp"
#include "ModelLightSubmission20.hpp"
#include "ModelPrevalidation40.hpp"
#include "ModelOwnerTransformDiagnostic46.hpp"
#include "ModelEffectGetterDiagnostic62.hpp"
#include <optional>
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstring>
struct SafetyHookContext{uint32_t eflags{},esp=1000,ebp=992,edi=9;};
namespace fusionfix::shadows{struct FloatingPointState{~FloatingPointState(){}};}
namespace CoronaLight20{bool requested=false;}
namespace ModelEffectObserver {bool Copy(uintptr_t,void*,size_t) noexcept;}
namespace TrafficSignal{
 struct Match{uint32_t identity=8,kind=1,modelInfo=4;std::array<uint8_t,68> ownerPose109{};};unsigned ownerReads=0;bool modelOK=true,ownerOK=true;
 bool MatchOwner(uint32_t,Match& m){++ownerReads;m={};if(!modelOK)m.modelInfo=5;return ownerOK;}
 bool ReadableOriginal(uintptr_t,size_t) noexcept{return true;}
}
uint32_t game=0x400000;
std::array<uint32_t,18> emitted{};uint32_t originalMatrix=3;
namespace traffic_signal::fresh_batch30 {
 struct Span{uintptr_t address;void* output;size_t size;};
 bool Copy(const Span* spans,unsigned count,fresh_page::Check) noexcept{
  for(unsigned i=0;i<count;++i){if(spans[i].address==1000)std::memcpy(spans[i].output,emitted.data(),spans[i].size);
   else if(spans[i].address==996){const uint32_t original[]{game+model_effect_observer::DispatchReturn,2,originalMatrix};std::memcpy(spans[i].output,original,spans[i].size);}
   else {auto* f=static_cast<float*>(spans[i].output);for(unsigned j=0;j<spans[i].size/4;++j)f[j]=1.0f;}}
  return true;
 }
}
namespace ModelLightShadow20 {
 bool requested=false;std::atomic<bool> active{false};
 unsigned promotions=0;uint8_t lastDescriptor=0;
 uint32_t Promote(SafetyHookContext&,const model_effect_observer::Invocation&,const TrafficSignal::Match&,const std::array<uint8_t,108>& descriptor,uint32_t,const std::array<uint32_t,18>&,bool finite){assert(finite);++promotions;lastDescriptor=descriptor[0];return 77;}
 uint32_t RegisterKey68(const model_effect_observer::Invocation&,const TrafficSignal::Match&,const std::array<uint8_t,108>&,uint32_t,const std::array<uint32_t,18>&,bool){assert(false&&"Historical legacy fixture must not enter the disabled constructor fallback");return 0;}
}
// Mock the combined production adapter using the same fresh owner/descriptor
// providers; actual QWS batching and raced faults are covered by owner48.
namespace TrafficSignal {
 bool MatchOwnerWithDescriptor48(uintptr_t owner,Match& match,uintptr_t descriptor,void* output,size_t size,bool& copied) noexcept {
  const bool matched=MatchOwner(uint32_t(owner),match);
  copied=ModelEffectObserver::Copy(descriptor,output,size);return matched;
 }
}
namespace ModelEffectObserver {
 using namespace model_effect_observer;
 constexpr size_t MaxDepth=8;
 struct LastErrorScope{~LastErrorScope(){}};
 struct Sample{std::array<uint8_t,68> ownerPose109{};bool diagnostic=false,ownerMatched=false,modelInfoMatched=false,descriptorCopied=false,matrixCopied=false,packetCopied=false;uint32_t identity{},ownerKind{},resolveMismatch{},site{},type{},flags{},key{},promotedKey{};std::array<uint8_t,108> descriptor{};std::array<float,16> matrix{};model_owner_transform46::Snapshot ownerTransform{};model_effect_getter62::Snapshot getter62{};std::array<float,3> position{},color{},direction{},up{};float intensity{},radius{},innerCone{},outerCone{};};
 // Historical observer fixture leaves the separate correction disabled;
 // fixture-light244 compiles and exercises its actual production callback.
 namespace FixtureLight244 {void Correct(SafetyHookContext&,uint32_t,std::array<uint32_t,18>&,const Invocation&,Sample&) noexcept{}}
 struct Local{model_light_submission20::State state;std::array<Sample,8> samples{};} local;
 std::atomic<bool> enabled{true};std::atomic<uint32_t> readRejected{0},contextRejected{0},nativeInactiveSkipped40{0};bool hotCountersEnabled=true;
 unsigned innerReads=0,descriptorReads=0,typeReads=0,matrixReads=0;uint8_t descriptorValue=1,fixtureType=1;bool copyOK=true,getterFault62=false;
 uint32_t Frame() noexcept{return 7;}
 bool InnerEvidence(const SafetyHookContext&,ResolveEvidence& e) noexcept{++innerReads;const auto& c=*local.state.Current();e={c.frame,c.preStack,DispatchReturn,c.effect,c.matrix,c.owner,c.effect,9,c.proposedKey};return true;}
 bool Copy(uintptr_t a,void* p,size_t n) noexcept{
  if(getterFault62&&a==2&&n==16)return false;
  if(!copyOK)return false;
  if(n==1){++typeReads;*static_cast<uint8_t*>(p)=fixtureType;return true;}
  std::memset(p,0,n);if(a==9){++descriptorReads;static_cast<uint8_t*>(p)[0]=descriptorValue;static_cast<uint8_t*>(p)[0x5b]=fixtureType;}else ++matrixReads;return true;
 }
 // Preserve this historical boundary fixture's legacy strategy; full fused
 // pipeline has its own production-extracted actual owner fixture.
 bool pointFusion50=false;enum class PointFusion50Result{Complete,Fallback};
 std::atomic<uint32_t> ownedFusedAttempts79{0},ownedFusedComplete79{0},ownedFusedLegacy79{0},ownedFusedKeys79{0};
 PointFusion50Result FusePoint50(SafetyHookContext&,const Invocation&,Sample&,uint32_t,const std::array<uint32_t,18>&,const PointEvidence&,std::array<uint32_t,32>* =nullptr) noexcept{assert(!pointFusion50);return PointFusion50Result::Fallback;}
 #include "model-deferred46-extracted.inc"
}
int main(){
 using namespace ModelEffectObserver;
 auto begin=[&](bool diagnostic=false){local.state={};local.samples={};Invocation c{};c.frame=7;c.preStack=1000;c.owner=1;c.effect=2;c.matrix=3;c.modelInfo=4;c.proposedKey=1;c.sampled=true;assert(local.state.Enter(c));local.samples[0].diagnostic=diagnostic;emitted={};emitted[0]=2;emitted[1]=0;emitted[2]=0;emitted[3]=2000;emitted[4]=2004;emitted[5]=2008;emitted[6]=2012;emitted[7]=0x3f800000;emitted[10]=0x41200000;originalMatrix=3;TrafficSignal::modelOK=TrafficSignal::ownerOK=true;copyOK=true;};
 SafetyHookContext regs{};
 // Native already-shadowed packet retains native execution and performs no
 // owner/full-descriptor validation in Resolve or rejected Point.
 begin();unsigned beforeOwner=TrafficSignal::ownerReads,beforeDescriptor=descriptorReads,beforePromotion=ModelLightShadow20::promotions;Resolve(regs);
 assert(local.state.Current()->resolved&&TrafficSignal::ownerReads==beforeOwner&&descriptorReads==beforeDescriptor);
 emitted[2]=4;emitted[15]=1;Point(regs,WrapperPoint);assert(TrafficSignal::ownerReads==beforeOwner&&descriptorReads==beforeDescriptor&&ModelLightShadow20::promotions==beforePromotion);
 // Accepted native point/spot metadata is read fresh AFTER eligibility.
 begin();Resolve(regs);descriptorValue=23;Point(regs,WrapperPoint);assert(TrafficSignal::ownerReads==beforeOwner+1&&descriptorReads==beforeDescriptor+1&&ModelLightShadow20::lastDescriptor==23);
 begin();Resolve(regs);emitted[1]=2;Point(regs,model_light_submission20::WrapperSpot);assert(ModelLightShadow20::promotions==beforePromotion+2);
 // Changed original matrix or descriptor cannot be promoted.
 begin();Resolve(regs);originalMatrix=4;Point(regs,WrapperPoint);assert(local.state.Current()->invalid&&ModelLightShadow20::promotions==beforePromotion+2);
 begin();Resolve(regs);regs.edi=10;Point(regs,WrapperPoint);regs.edi=9;assert(local.state.Current()->invalid&&ModelLightShadow20::promotions==beforePromotion+2);
 begin();Resolve(regs);TrafficSignal::modelOK=false;Point(regs,WrapperPoint);assert(local.state.Current()->invalid&&ModelLightShadow20::promotions==beforePromotion+2);
 begin();Resolve(regs);copyOK=false;Point(regs,WrapperPoint);assert(local.state.Current()->invalid&&ModelLightShadow20::promotions==beforePromotion+2);
 // Full diagnostic and corona-only pre-submission evidence remains at Resolve.
 begin(true);auto owned=TrafficSignal::ownerReads;Resolve(regs);assert(TrafficSignal::ownerReads==owned+1&&local.samples[0].matrixCopied);
 assert(local.samples[0].ownerTransform.status==model_owner_transform46::Status::NullPointer&&!local.state.Current()->invalid); // Optional owner pose failure cannot suppress native emission.
 // Actual production Resolve invokes the optional getter observer. Its fault
 // must affect only the diagnostic status, not current context or promotion.
 begin(true);getterFault62=false;Resolve(regs);Point(regs,WrapperPoint);
 const auto accepted62=*local.state.Current();const auto args62=emitted;const auto regs62=regs;
 const auto promotions62=ModelLightShadow20::promotions;
 begin(true);getterFault62=true;Resolve(regs);
 assert(local.samples[0].getter62.status==model_effect_getter62::Status::EffectUnreadable);
 Point(regs,WrapperPoint);getterFault62=false;
 assert(ModelLightShadow20::promotions==promotions62+1&&emitted==args62);
 assert(std::memcmp(&regs,&regs62,sizeof(regs))==0);
 const auto& actual62=*local.state.Current();
 assert(actual62.invalid==accepted62.invalid&&actual62.resolved==accepted62.resolved&&actual62.submitted==accepted62.submitted&&actual62.descriptor==accepted62.descriptor&&actual62.effect==accepted62.effect);
 CoronaLight20::requested=true;fixtureType=0;begin();owned=TrafficSignal::ownerReads;Resolve(regs);assert(typeReads==1&&TrafficSignal::ownerReads==owned+1&&local.samples[0].matrixCopied&&local.samples[0].descriptorCopied);
 fixtureType=1;begin();owned=TrafficSignal::ownerReads;Resolve(regs);assert(TrafficSignal::ownerReads==owned&&!local.samples[0].descriptorCopied);Point(regs,WrapperPoint);assert(TrafficSignal::ownerReads==owned+1);
}
