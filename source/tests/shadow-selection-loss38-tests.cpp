#include "../core/ShadowSelectionLoss38.hpp"
#include "../core/NativeShadowContinuity42.hpp"
#include "../core/PlayerShadowBudget.hpp"
#include <cassert>
#include <cstdio>
#include <atomic>
#include <vector>
using namespace fusionfix::shadows;
namespace budget=fusionfix::shadows::budget;
namespace ShadowTrace34 {
struct Event {uint32_t stage,frame,tick,key;int32_t a,b,c,d;float x,y,z,w;};
static_assert(sizeof(Event)==48);
std::atomic<bool> enabled{false};
std::vector<Event> events;
void Emit(Event event){events.push_back(event);}
}
namespace rage {
struct V {float x{},y{},z{};};
struct CLightSource {uint32_t mCastShadows{},mFlags{};int mShadowCacheIndex{},mType{};
 V mPosition,mDirection,mTangent;float mRadius{},mOuterConeAngle{},mInnerConeAngle{};};
}
struct State {std::array<NativeShadowContinuity42::Candidate,4096> nativeCandidates;
 std::array<SelectionScoreDiagnostic38,4096> scoreDiagnostics38;
 struct Multiplicity {bool ambiguous=false;bool Ambiguous(unsigned)const{return ambiguous;}} keyMultiplicity;
 uint32_t frame=10,previousSelectionFrame=9;uintptr_t occupiedCar=0;} state;
std::array<uint8_t,4096> labRelevant{};
std::array<rage::CLightSource,10> lights{};uint32_t count=0;
uint32_t CurrentCount(){return count;}const rage::CLightSource* CurrentLights(){return lights.data();}
#include "selection-loss38-extracted.inc"
const ShadowTrace34::Event& event(unsigned stage){for(const auto& e:ShadowTrace34::events)if(e.stage==stage)return e;assert(false);return ShadowTrace34::events[0];}
bool hasEvent(unsigned stage){for(const auto& e:ShadowTrace34::events)if(e.stage==stage)return true;return false;}
int main(){
 budget::PlayerShadowBudget::Slot old{};old.key=64;old.generation=0x123456789abcdef0ull;
 budget::PlayerShadowBudget::Selection selected{};
 TraceSelectionLoss38(selected,old,0,100);assert(ShadowTrace34::events.empty());
 ShadowTrace34::enabled=true;TraceSelectionLoss38(selected,old,0,100);
 assert(event(30).c==int(SelectionLossReason38::MissingSource));assert(!hasEvent(33));
 assert(ShadowTrace34::events.size()<=10);assert(event(34).c==9);
 assert(uint32_t(event(34).a)==0x9abcdef0 && uint32_t(event(34).b)==0x12345678);
 count=1;lights[0].mCastShadows=64;lights[0].mFlags=0;
 ShadowTrace34::events.clear();TraceSelectionLoss38(selected,old,0,100);
 assert(event(30).c==int(SelectionLossReason38::ShadowFlagAbsent));
 lights[0].mFlags=6;lights[0].mShadowCacheIndex=-1;
 ShadowTrace34::events.clear();TraceSelectionLoss38(selected,old,0,100);
 assert(event(30).c==int(SelectionLossReason38::CacheUnavailable));
 lights[0].mFlags=4;lights[0].mShadowCacheIndex=0;
 ShadowTrace34::events.clear();TraceSelectionLoss38(selected,old,0,100);
 assert(event(30).c==int(SelectionLossReason38::NotCollected));
 state.nativeCandidates[0].observed=true;state.nativeCandidates[0].focusDistance=12;
 state.scoreDiagnostics38[0].frame=10;state.scoreDiagnostics38[0].generation=old.generation+1;
 ShadowTrace34::events.clear();TraceSelectionLoss38(selected,old,0,100);
 assert(event(30).c==int(SelectionLossReason38::ChangedGeneration));
 state.scoreDiagnostics38[0].generation=old.generation;
 ShadowTrace34::events.clear();TraceSelectionLoss38(selected,old,0,100);
 assert(event(30).c==int(SelectionLossReason38::Irrelevant));
 labRelevant[0]=1;state.scoreDiagnostics38[0].rawFocus=123;
 state.scoreDiagnostics38[0].viewWeight=3;state.scoreDiagnostics38[0].subjectCoverage=.75f;
 count=2;lights[1].mCastShadows=128;lights[1].mFlags=4;
 selected.validMask=1;selected.slots[0].key=128;selected.slots[0].generation=17;selected.slots[0].index=1;
 state.nativeCandidates[1].observed=true;state.nativeCandidates[1].focusDistance=3;
 state.scoreDiagnostics38[1].frame=10;state.scoreDiagnostics38[1].rawFocus=9;
 ShadowTrace34::events.clear();TraceSelectionLoss38(selected,old,0,100);
 assert(event(30).c==int(SelectionLossReason38::EligibleNotSelected));
 assert(event(33).a==128 && event(33).b==1 && event(33).c==0);
 assert(event(31).x==123 && event(32).x==.75f && event(33).y==9);
 assert(ShadowTrace34::events.size()==10);
 state.keyMultiplicity.ambiguous=true;
 ShadowTrace34::events.clear();TraceSelectionLoss38(selected,old,0,100);
 assert(event(30).c==int(SelectionLossReason38::AmbiguousIdentity));
 state.keyMultiplicity.ambiguous=false;state.scoreDiagnostics38[0].frame=9;
 ShadowTrace34::events.clear();TraceSelectionLoss38(selected,old,0,100);
 assert(event(38).d==0 && std::isnan(event(31).x));
 // A same-key DIFFERENT generation is a selected reference, never retained.
 selected.slots[0]=old;selected.slots[0].generation=old.generation+1;
 assert(SelectionLossReference38(selected,old,0)==0);
 selected.slots[0]=old;assert(SelectionLossReference38(selected,old,0)==-1);
 puts("PASS: production-extracted loss emitter gating, reasons, generation/frame identity, selected reference and ABI.");
}
