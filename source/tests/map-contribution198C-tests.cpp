#define NOMINMAX
#include <windows.h>
#include <array>
#include <vector>
#include <cassert>
#include <cstring>
#include <limits>
#include <iostream>
#include "core/standalone-game-abi.hpp"
#include "core/PlayerShadowBudget.hpp"
#include "core/NativeShadowContinuity42.hpp"
#include "core/ShadowSelectionLoss38.hpp"
#include "core/ShadowFloatingPointState.hpp"
#include "core/MapContribution198C.hpp"
namespace CaptureDiagnostic44 {std::atomic<bool> sampleSources=true;uint32_t lastStart{};}
namespace CShadows {uint32_t frame=40;uint32_t* pFrameCounter=&frame;}
namespace ShadowTrace34 {
 struct Event {uint32_t stage,frame,tick,key;int32_t a,b,c,d;float x,y,z,w;};
 std::atomic<bool> enabled=true;std::vector<Event> events;
 void Emit(Event e)noexcept{events.push_back(e);}
}
static unsigned copies{},priorCalls{},logs{};static bool copyAllowed=true;
namespace Liberty20 {bool CopyLocal(uintptr_t p,void* out,size_t bytes)noexcept{++copies;assert(bytes==120);if(!copyAllowed)return false;std::memcpy(out,reinterpret_cast<void*>(p),bytes);return true;}}
void Event(unsigned,const char*,const char*){++logs;}
namespace PlayerShadowAllocation {
 using Selection=fusionfix::shadows::budget::PlayerShadowBudget::Selection;
 struct Pass {Selection selection;const Selection& LastSelection()const{return selection;}};
 struct Multiplicity {std::array<bool,8> ambiguous{};bool Ambiguous(unsigned i)const{return ambiguous[i];}};
 struct State {
  bool committed=true;unsigned depth=1;uint32_t frame=40;Pass pass;Multiplicity keyMultiplicity;
  fusionfix::shadows::ShadowVolumeFrustum20 volumeFrustum;
  std::array<fusionfix::shadows::SelectionScoreDiagnostic38,8> scoreDiagnostics38{};
  std::array<fusionfix::shadows::NativeShadowContinuity42::Candidate,8> nativeCandidates{};
 } state;
 std::array<rage::CLightSource,8> lights{};uint32_t count=7;
 bool publicationEnabled=true,freshView198C=true;
 std::atomic<bool> unsupportedThread=false;std::atomic<DWORD> ownerThread=GetCurrentThreadId();
 void (*observeSelected198C)()noexcept=nullptr;
 auto CurrentLights(){return lights.data();}auto CurrentCount(){return count;}
}
namespace OwnHeadlightCaster {void (*labRenderObserver)(uint32_t,uint32_t,uint32_t,bool)noexcept=nullptr;}
#include "MapContribution198CRuntime.inc"
static void Prior(uint32_t,uint32_t,uint32_t,bool)noexcept{++priorCalls;}
static void Init(){
 using namespace PlayerShadowAllocation;
 state={};state.committed=true;state.depth=1;state.frame=CShadows::frame=40;count=7;
 state.pass.selection.validMask=127;state.pass.selection.count=7;state.pass.selection.safeToApply=true;
 fusionfix::shadows::ShadowView view;view.valid=true;for(unsigned i=0;i<4;++i)view.view[i][i]=view.projection[i][i]=1;
 state.volumeFrustum=fusionfix::shadows::PrepareShadowVolumeFrustum20(view);
 lights={};for(unsigned i=0;i<7;++i){auto& l=lights[i];l.mCastShadows=int32_t(100+i);l.mFlags=4;l.mRadius=.25f;l.mPosition={i==1?5.f:0.f,0,.5f};l.mIntensity=2;l.mColor={1,1,1,1};l.mShadowCacheIndex=-1;
  state.pass.selection.slots[i]={uint64_t(100+i),uint64_t(200+i),i,fusionfix::shadows::budget::Kind::Lamp};
  state.nativeCandidates[i].observed=true;state.nativeCandidates[i].focusDistance=3;
  state.scoreDiagnostics38[i].frame=40;state.scoreDiagnostics38[i].generation=200+i;state.scoreDiagnostics38[i].viewWeight=2;
 }
 publicationEnabled=freshView198C=true;unsupportedThread=false;ownerThread=GetCurrentThreadId();
 MapContribution198C::census={};MapContribution198C::renderScope=nullptr;MapContribution198C::requested=true;
 MapContribution198C::priorRender=Prior;addonActive=true;CaptureDiagnostic44::sampleSources=true;ShadowTrace34::enabled=true;
 CaptureDiagnostic44::lastStart=GetTickCount();copies=priorCalls=logs=0;copyAllowed=true;ShadowTrace34::events.clear();
}
static unsigned Count(unsigned stage){unsigned n=0;for(const auto& e:ShadowTrace34::events)n+=e.stage==stage;return n;}
int main(){
 using namespace MapContribution198C;using Class=map_contribution198::Class;
 Init();requested=false;Wire();assert(!PlayerShadowAllocation::observeSelected198C&&logs==0);Selected();assert(copies==0&&ShadowTrace34::events.empty());
 Init();OwnHeadlightCaster::labRenderObserver=Prior;Wire();assert(logs==1&&PlayerShadowAllocation::observeSelected198C==Selected&&OwnHeadlightCaster::labRenderObserver==RenderIdentity);
 Wire();assert(priorRender==Prior); // startup wiring cannot recurse if repeated
 const auto before=PlayerShadowAllocation::lights;Selected();assert(copies==7&&Count(240)==1&&Count(241)==7&&Count(242)==7&&Count(243)==7&&Count(248)==7&&census.valid);
 assert(census.rows[0].classification==Class::MayReachMain&&census.rows[1].classification==Class::OutsideMain);
 assert(!std::memcmp(before.data(),PlayerShadowAllocation::lights.data(),sizeof(before)));
 SetLastError(0x198c);const auto mxcsr=_mm_getcsr();{RenderScope scope;RenderIdentity(101,2,3,true);}assert(GetLastError()==0x198c&&_mm_getcsr()==mxcsr&&priorCalls==1&&Count(244)==1&&Count(245)==1);
 {RenderScope scope;RenderIdentity(101,2,4,true);}assert(Count(245)==2); // separate cached/static kind
 {RenderScope scope;RenderIdentity(101,2,4,true);}assert(Count(245)==2&&Count(246)==1); // duplicate is unknown, not double-counted
 {RenderScope scope;}assert(Count(246)==2); // deferred/unseen identity
 for(unsigned scenario=0;scenario<13;++scenario){Init();
  if(scenario==0)requested=false;if(scenario==1)addonActive=false;if(scenario==2)CaptureDiagnostic44::sampleSources=false;
  if(scenario==3)ShadowTrace34::enabled=false;if(scenario==4)CaptureDiagnostic44::lastStart-=10000;
  if(scenario==5)PlayerShadowAllocation::state.committed=false;if(scenario==6)PlayerShadowAllocation::publicationEnabled=false;
  if(scenario==7)PlayerShadowAllocation::state.depth=2;if(scenario==8)++CShadows::frame;if(scenario==9)PlayerShadowAllocation::ownerThread=0;
  if(scenario==10)PlayerShadowAllocation::unsupportedThread=true;if(scenario==11)PlayerShadowAllocation::count=641;
  if(scenario==12)PlayerShadowAllocation::state.pass.selection.safeToApply=false;
  Selected();assert(!census.valid&&copies==0&&ShadowTrace34::events.empty());
 }
 Init();PlayerShadowAllocation::freshView198C=false;Selected();assert(census.rows[1].classification==Class::UnknownView);
 Init();PlayerShadowAllocation::lights[1].mFlags|=0x100;PlayerShadowAllocation::state.keyMultiplicity.ambiguous[2]=true;
 PlayerShadowAllocation::lights[3].mRadius=NAN;Selected();assert(census.rows[1].classification==Class::Vehicle&&census.rows[2].classification==Class::Ambiguous&&!census.rows[2].valid&&census.rows[3].classification==Class::Invalid);
 Init();PlayerShadowAllocation::lights[1].mType=rage::LT_DIR;Selected();assert(census.rows[1].classification==Class::Invalid);
 Init();copyAllowed=false;Selected();for(const auto& r:census.rows)assert(!r.valid);assert(Count(241)==0);
 Init();PlayerShadowAllocation::lights[1].mCastShadows=100;PlayerShadowAllocation::state.pass.selection.slots[1].key=100;Selected();
 {RenderScope scope;RenderIdentity(100,1,3,true);}assert(Count(245)==0&&Count(246)==1);
 Init();Selected();{RenderScope outer;auto* current=renderScope;census.renderAttempts=28;{RenderScope refused;RenderIdentity(100,1,3,true);}assert(renderScope==current);}assert(Count(245)==0&&Count(246)==1);
 Init();Selected();{RenderScope outer;auto* current=renderScope;CaptureDiagnostic44::sampleSources=false;{RenderScope refused;CaptureDiagnostic44::sampleSources=true;RenderIdentity(100,1,3,true);}assert(renderScope==current);}assert(Count(244)==0&&Count(245)==0&&Count(246)==1);
 Init();Selected();{RenderScope outer;RenderIdentity(100,1,3,true);{RenderScope inner;RenderIdentity(101,2,3,true);}}assert(Count(245)==2&&!renderScope);
 bool nested=false;for(const auto& e:ShadowTrace34::events)if(e.stage==245&&e.key==101)nested=e.y==1;assert(nested);
 Init();Selected();{RenderScope scope;++CShadows::frame;RenderIdentity(100,1,3,true);}assert(Count(244)==0&&Count(245)==0);
 Init();Selected();{RenderScope scope;PlayerShadowAllocation::state.depth=2;Selected();RenderIdentity(100,1,3,true);}assert(Count(244)==0&&Count(245)==0&&!census.valid);
 Init();for(unsigned i=0;i<258;++i)Selected();assert(census.passes==256&&!census.valid&&Count(247)==1&&copies==256*7);
 Init();Selected();for(unsigned i=0;i<35;++i){RenderScope scope;}assert(census.renderAttempts==28&&Count(246)==28&&Count(249)==1);
 std::cout<<"ACTUAL Selected/RenderScope: selected7-only120-byte copies; fresh/unknown view; vehicle/invalid/ambiguous labels;13 refusals; bounded capture; native identity joins and unknown deferred; prior callback once; nested refusal isolation; no light writes; FP/LastError preserved. No GPU/visual proof.\n";
}
