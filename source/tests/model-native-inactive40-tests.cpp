#include "FreshPageReadable.hpp"
#include "ImmediateRegion33.hpp"

#include <array>
#include <optional>
#include <atomic>
#include <cassert>
#include <cstdio>
#include "ModelPrevalidation40.hpp"
#include "ModelLightSubmission20.hpp"
#include "ModelOwnerTransformDiagnostic46.hpp"
#include "ModelEffectGetterDiagnostic62.hpp"

struct SafetyHookContext {uint32_t eflags;};
namespace fusionfix::shadows {struct FloatingPointState{};}
// FreshPageReadable includes the actual CPU profile Scope (disabled by default).
namespace CoronaLight20 {bool requested=false;}
namespace ModelEffectObserver {bool Copy(uint32_t,void*,size_t);}
namespace TrafficSignal {struct Match{uint32_t identity=8,kind=1,modelInfo=4;std::array<uint8_t,68> ownerPose109{};};unsigned ownerReads=0;bool MatchOwner(uint32_t,Match& m){++ownerReads;m={};return true;}}
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
struct LastErrorScope{};
struct Sample{std::array<uint8_t,68> ownerPose109{};bool diagnostic=false,ownerMatched=false,modelInfoMatched=false,descriptorCopied=false,matrixCopied=false;uint32_t identity{},ownerKind{},resolveMismatch{};std::array<uint8_t,108> descriptor{};std::array<float,16> matrix{};model_owner_transform46::Snapshot ownerTransform{};model_effect_getter62::Snapshot getter62{};};
struct Local{model_light_submission20::State state;std::array<Sample,8> samples{};} local;
std::atomic<bool> enabled{true};std::atomic<uint32_t> readRejected{0},contextRejected{0},nativeInactiveSkipped40{0};
bool hotCountersEnabled=true;unsigned innerReads=0,copyReads=0;
bool InnerEvidence(const SafetyHookContext&,ResolveEvidence& evidence){++innerReads;const auto& c=*local.state.Current();evidence={c.frame,c.preStack,DispatchReturn,c.effect,c.matrix,c.owner,c.effect,9,c.proposedKey};return true;}
bool Copy(uint32_t,void* p,size_t n){++copyReads;for(size_t i=0;i<n;++i)((uint8_t*)p)[i]=0;return true;}
#include "native-inactive40-extracted.inc"
}
int main(){
 using namespace ModelEffectObserver;
 Invocation invocation{};invocation.frame=7;invocation.preStack=100;invocation.owner=1;invocation.effect=2;invocation.matrix=3;invocation.modelInfo=4;invocation.proposedKey=1;invocation.sampled=true;
 auto enter=[&](bool diagnostic){assert(local.state.Enter(invocation));local.samples[local.state.Depth()-1]={};local.samples[local.state.Depth()-1].diagnostic=diagnostic;};
 auto leave=[&](){Invocation finished{};assert(local.state.Leave(100,7,finished)==LeaveResult::Valid);};
 enter(false);SafetyHookContext regs{0x40};Resolve(regs);assert(innerReads==0&&TrafficSignal::ownerReads==0&&copyReads==0);assert(local.state.Depth()==1&&!local.state.Current()->resolved);assert(nativeInactiveSkipped40==1);leave();
 enter(false);hotCountersEnabled=false;Resolve(regs);assert(nativeInactiveSkipped40==1&&innerReads==0);leave();hotCountersEnabled=true;
 enter(true);Resolve(regs);assert(innerReads==1&&TrafficSignal::ownerReads==1&&copyReads==4&&local.state.Current()->resolved);assert(nativeInactiveSkipped40==1);
 assert(local.samples[0].ownerTransform.status==model_owner_transform46::Status::NullPointer&&!local.state.Current()->invalid);leave();
 enter(false);regs.eflags=0;Resolve(regs);assert(innerReads==2&&TrafficSignal::ownerReads==1&&copyReads==4&&local.state.Current()->resolved);leave();
 enter(false);enter(false);regs.eflags=0x40;Resolve(regs);assert(local.state.Depth()==2);leave();regs.eflags=0;Resolve(regs);assert(local.state.Depth()==1&&innerReads==3&&TrafficSignal::ownerReads==1);leave();
 Resolve(regs);assert(local.state.Depth()==0&&innerReads==3);
 assert(!model_prevalidation40::NativeInactive(0x40,true));assert(!model_prevalidation40::NativeInactive(0,false));assert(model_prevalidation40::NativeInactive(0x40,false));
 std::puts("6 extracted-production Resolve scenarios +3 flag-policy assertions passed; no live hook validation claimed");
}
