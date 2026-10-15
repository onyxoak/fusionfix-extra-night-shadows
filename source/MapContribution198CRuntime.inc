// Default-off diagnostic only. Reuse existing selection/render callbacks and
// the existing trace writer. No game writes, extra hooks or per-caster work.
#include "core/MapContribution198C.hpp"
namespace MapContribution198C {
inline bool requested=false;
inline void (*priorRender)(uint32_t,uint32_t,uint32_t,bool) noexcept=nullptr;
struct Preserve {DWORD error=GetLastError();fusionfix::shadows::FloatingPointState fp;~Preserve()noexcept{SetLastError(error);}};
struct Census {
 std::array<map_contribution198::Row,7> rows{};
 uint32_t clip{},frame{},thread{},serial{},passes{},renderAttempts{};
 bool valid{},limited{},renderLimited{};LARGE_INTEGER frequency{};
};
inline thread_local Census census;
inline bool Gates() noexcept {return requested&&addonActive.load(std::memory_order_acquire)&&
 CaptureDiagnostic44::sampleSources.load(std::memory_order_relaxed)&&ShadowTrace34::enabled.load(std::memory_order_relaxed);}
inline bool Window(uint32_t now,uint32_t clip) noexcept {return Gates()&&clip==CaptureDiagnostic44::lastStart&&uint32_t(now-clip)<10000u;}
inline uint32_t Frame() noexcept {return CShadows::pFrameCounter?*CShadows::pFrameCounter:UINT32_MAX;}
void Selected() noexcept {
 census.valid=false;
 if(!Gates())return;
 const Preserve preserve;const uint32_t now=GetTickCount(),clip=CaptureDiagnostic44::lastStart;
 if(!Window(now,clip))return;
 if(census.clip!=clip){census={};census.clip=clip;(void)QueryPerformanceFrequency(&census.frequency);}
 if(census.passes>=256){if(!census.limited){census.limited=true;ShadowTrace34::Emit({247,Frame(),now,0,int32_t(clip),int32_t(census.passes),256,0,0,0,0,0});}return;}
 const auto& state=PlayerShadowAllocation::state;const auto thread=GetCurrentThreadId();
 if(!state.committed||!PlayerShadowAllocation::publicationEnabled||state.depth!=1||
    PlayerShadowAllocation::unsupportedThread.load(std::memory_order_relaxed)||
    thread!=PlayerShadowAllocation::ownerThread.load(std::memory_order_relaxed)||Frame()!=state.frame)return;
 const auto& selection=state.pass.LastSelection();const auto* lights=PlayerShadowAllocation::CurrentLights();const auto count=PlayerShadowAllocation::CurrentCount();
 if(!selection.safeToApply||!lights||count>640)return;
 census.rows={};census.frame=state.frame;census.thread=thread;census.renderAttempts=0;census.renderLimited=false;++census.passes;++census.serial;
 ShadowTrace34::Emit({240,state.frame,now,thread,int32_t(census.serial),int32_t(clip),int32_t(selection.validMask),PlayerShadowAllocation::freshView198C?1:0,0,0,0,0});
 for(unsigned slot=0;slot<7;++slot){
  if(!(selection.validMask&(1u<<slot)))continue;
  const auto& selected=selection.slots[slot];if(selected.index>=count||!selected.key||selected.key>UINT32_MAX)continue;
  rage::CLightSource light{};
  // Fixed native numeric light bank; SEH copy only, no permission/owner query.
  if(!Liberty20::CopyLocal(uintptr_t(&lights[selected.index]),&light,0x78)||uint32_t(light.mCastShadows)!=selected.key)continue;
  const bool ambiguous=state.keyMultiplicity.Ambiguous(selected.index);
  const bool vehicle=selected.kind!=fusionfix::shadows::budget::Kind::Lamp||(light.mFlags&0x500u)!=0;
  const auto classification=(light.mType==rage::LT_POINT||light.mType==rage::LT_SPOT)?
   map_contribution198::Classify(PlayerShadowAllocation::freshView198C,state.volumeFrustum,
    {light.mPosition.x,light.mPosition.y,light.mPosition.z},light.mRadius,vehicle,ambiguous):map_contribution198::Class::Invalid;
  const auto signature=map_contribution198::Signature({reinterpret_cast<const uint8_t*>(&light),0x78});
  census.rows[slot]={uint32_t(selected.key),selected.index,slot,selected.generation,signature,classification,0,!ambiguous};
  ShadowTrace34::Emit({241,state.frame,now,uint32_t(selected.key),int32_t(slot),int32_t(selected.index),int32_t(classification),int32_t(census.serial),light.mPosition.x,light.mPosition.y,light.mPosition.z,light.mRadius});
  const auto& score=state.scoreDiagnostics38[selected.index];const auto& candidate=state.nativeCandidates[selected.index];
  const bool current=score.frame==state.frame&&score.generation==selected.generation;
  const float unknown=std::numeric_limits<float>::quiet_NaN();
  ShadowTrace34::Emit({242,state.frame,now,uint32_t(selected.key),int32_t(uint32_t(selected.generation)),int32_t(uint32_t(selected.generation>>32)),int32_t(light.mFlags),int32_t(census.serial),
   current?score.viewWeight:unknown,candidate.observed?candidate.focusDistance:unknown,current?score.subjectCoverage:unknown,current?score.roadCoverage:unknown});
  ShadowTrace34::Emit({243,state.frame,now,uint32_t(selected.key),int32_t(uint32_t(signature)),int32_t(uint32_t(signature>>32)),int32_t(census.serial),int32_t(thread),light.mIntensity,light.mColor.x,light.mColor.y,light.mColor.z});
  ShadowTrace34::Emit({248,state.frame,now,uint32_t(selected.key),int32_t(light.mType),light.mShadowCacheIndex,int32_t(census.serial),int32_t(thread),0,0,0,0});
 }
 census.valid=Frame()==census.frame&&Window(GetTickCount(),clip);
}
class RenderScope;
inline thread_local RenderScope* renderScope=nullptr;
class RenderScope {
 RenderScope* previous{};LARGE_INTEGER begin{};uint32_t frame{},clip{},serial{},key{},slot{},kind{};int row=-1;bool timed{};
public:
 RenderScope() noexcept:previous(renderScope){
  if(!requested)return;
  renderScope=this; // A refused nested scope must not bind its parent's pass.
  if(!Gates())return;
  const Preserve preserve;const auto now=GetTickCount();
  if(!census.valid||!Window(now,census.clip)||Frame()!=census.frame||GetCurrentThreadId()!=census.thread)return;
  if(census.renderAttempts>=28){if(!census.renderLimited){census.renderLimited=true;ShadowTrace34::Emit({249,census.frame,now,0,int32_t(census.serial),28,int32_t(census.clip),0,0,0,0,0});}return;}
  ++census.renderAttempts;frame=census.frame;clip=census.clip;serial=census.serial;
  timed=QueryPerformanceCounter(&begin)!=0&&census.frequency.QuadPart>0;
 }
 void Bind(uint32_t source,uint32_t mapSlot,uint32_t mapKind,bool active) noexcept {
  if(!timed||row>=0||!active||!census.valid||mapKind<3||mapKind>5||Frame()!=frame||!Window(GetTickCount(),clip)||census.serial!=serial)return;
  const auto match=map_contribution198::Match(census.rows,source,mapSlot);if(match<0)return;
  auto& entry=census.rows[size_t(match)];const auto bit=1u<<(mapKind-3);if(entry.seenKinds&bit)return;
  entry.seenKinds|=bit;row=match;key=source;slot=mapSlot;kind=mapKind;
  ShadowTrace34::Emit({244,frame,GetTickCount(),key,int32_t(slot),int32_t(kind),int32_t(entry.classification),int32_t(serial),0,0,0,0});
 }
 ~RenderScope() noexcept {
  if(renderScope==this)renderScope=previous;
  if(!timed)return;
  const Preserve preserve;LARGE_INTEGER end{};const auto now=GetTickCount();
  if(!census.valid||!QueryPerformanceCounter(&end)||end.QuadPart<begin.QuadPart||!Window(now,clip)||census.serial!=serial||Frame()!=frame)return;
  const float us=float(double(end.QuadPart-begin.QuadPart)*1000000.0/double(census.frequency.QuadPart));
  if(row>=0)ShadowTrace34::Emit({245,frame,now,key,int32_t(slot),int32_t(kind),int32_t(census.rows[size_t(row)].classification),int32_t(serial),us,previous&&previous->timed?1.f:0.f,0,0});
  else ShadowTrace34::Emit({246,frame,now,0,int32_t(serial),int32_t(census.renderAttempts),int32_t(clip),0,us,0,0,0});
 }
 RenderScope(const RenderScope&)=delete;RenderScope& operator=(const RenderScope&)=delete;
};
void RenderIdentity(uint32_t key,uint32_t slot,uint32_t kind,bool active) noexcept {
 if(priorRender)priorRender(key,slot,kind,active);
 if(!Gates()||!renderScope)return;
 const Preserve preserve;renderScope->Bind(key,slot,kind,active);
}
void Wire() noexcept {
 if(!requested)return;
 PlayerShadowAllocation::observeSelected198C=Selected;
 if(OwnHeadlightCaster::labRenderObserver!=RenderIdentity){priorRender=OwnHeadlightCaster::labRenderObserver;OwnHeadlightCaster::labRenderObserver=RenderIdentity;}
 Event(1,"MAP_CONTRIBUTION198C_READY","default_off=1 capture_lease=1 first10seconds=1 max_selection_passes=256 selected_slots=7 max_render_attempts_per_pass=28 per_caster_queries=0 selection_changed=0 main_view_bound_not_culling_authority=1 deferred_unjoined=unknown cpu_submission_not_gpu=1");
}
}
