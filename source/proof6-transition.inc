// Bounded observation: eight preceding passes, first loss, three following passes.
// Producer owns storage until release; consumer reads only after completion.
struct ProofTransitionFrame {
 uint32_t frame{},tick{},key{},promoted{},renderCalls{},renderFrame{},count{},missingRun{};
 bool present{},selected{},committed{},continuity{},observed{},relevant{};
 unsigned claim=7; uint64_t generation{}; rage::CLightSource source{};
 struct Choice {uint32_t key{},flags{};unsigned claim=7;bool observed{},relevant{},own{};int cache=-1;} choices[7]{};
};
std::array<ProofTransitionFrame,16> proofTransition{};
std::atomic<unsigned> proofTransitionReady{0};
std::atomic<uint32_t> proofRenderCalls{0},proofRenderFrame{0};
unsigned proofTransitionCount=0;
void ObserveProofRender(uint32_t key,uint32_t slot,uint32_t kind,bool active) noexcept {
 if(key==ProofKey()&&active&&kind==4&&slot>=1&&slot<=7){++proofRenderCalls;proofRenderFrame=*CShadows::pFrameCounter;}
}
void CaptureProofTransition(bool selected) noexcept {
 using namespace PlayerShadowAllocation;
 if(proofTransitionReady.load(std::memory_order_acquire)||!proofArmed.load())return;
 static std::array<ProofTransitionFrame,8> history{};
 static unsigned written=0,post=0,missing=0;static bool previous=false,triggered=false;
 ProofTransitionFrame f{};f.frame=*CShadows::pFrameCounter;f.tick=GetTickCount();f.key=ProofKey();
 f.promoted=proofPromoted.load();f.renderCalls=proofRenderCalls.load();f.renderFrame=proofRenderFrame.load();
 f.selected=selected;f.committed=state.committed;f.continuity=state.continuityActive;f.count=CurrentCount();
 const auto* lights=CurrentLights();
 if(lights&&f.count<=4096){
  for(uint32_t i=0;i<f.count;++i)if(IsProofPoint(lights[i])){
   f.present=true;f.source=lights[i];f.generation=LampGeometry(lights[i]);
   const auto& c=state.nativeCandidates[i];f.observed=c.observed;f.claim=c.claim;f.relevant=labRelevant[i]!=0;break;
  }
  for(unsigned slot=0;slot<7;++slot){
   const auto record=game+fusionfix::shadows::ce::allocation::DynamicPassFirstPositionRva+slot*0x110;
   auto& out=f.choices[slot];if(!*reinterpret_cast<const uint8_t*>(record+0x25))continue;
   out.key=*reinterpret_cast<const uint32_t*>(record+0x30);
   for(uint32_t i=0;i<f.count;++i)if(static_cast<uint32_t>(lights[i].mCastShadows)==out.key){
    const auto& c=state.nativeCandidates[i];out.flags=lights[i].mFlags;out.claim=c.claim;out.observed=c.observed;
    out.relevant=labRelevant[i]!=0;out.own=c.ownBeam;out.cache=lights[i].mShadowCacheIndex;break;
   }
  }
 }
 missing=f.present?0:missing+1;f.missingRun=missing;
 if(!triggered&&previous&&!selected){
  triggered=true;const auto n=(std::min)(written,8u);
  for(unsigned i=0;i<n;++i)proofTransition[proofTransitionCount++]=history[(written-n+i)%8];
  proofTransition[proofTransitionCount++]=f;post=3;
 }else if(triggered){
  proofTransition[proofTransitionCount++]=f;
  if(!--post)proofTransitionReady.store(1,std::memory_order_release);
 }
 history[written++%8]=f;previous=selected;
}
void PublishProofTransition(){
 unsigned expected=1;if(!proofTransitionReady.compare_exchange_strong(expected,2,std::memory_order_acquire))return;
 Event(1,"PROOF_TRANSITION_BEGIN","first_selected_to_unselected bounded_12_passes");
 for(unsigned i=0;i<proofTransitionCount;++i){const auto& f=proofTransition[i];const auto& l=f.source;char text[900];
  sprintf_s(text,"n=%u frame=%u tick=%u key=%u generation=%llu present=%u missingRun=%u observed=%u relevant=%u claim=%u selected=%u committed=%u continuity=%u promoted=%u renderCalls=%u renderFrame=%u flags=%u cache=%d pos=%.4f,%.4f,%.4f rgb=%.3f,%.3f,%.3f intensity=%.6f radius=%.3f submitted=%u",i,f.frame,f.tick,f.key,f.generation,f.present,f.missingRun,f.observed,f.relevant,f.claim,f.selected,f.committed,f.continuity,f.promoted,f.renderCalls,f.renderFrame,l.mFlags,l.mShadowCacheIndex,l.mPosition.x,l.mPosition.y,l.mPosition.z,l.mColor.x,l.mColor.y,l.mColor.z,l.mIntensity,l.mRadius,f.count);Event(1,"PROOF_TRANSITION",text);
  for(unsigned j=0;j<7;++j){const auto& c=f.choices[j];sprintf_s(text,"frame=%u slot=%u key=%u flags=%u claim=%u observed=%u relevant=%u own=%u cache=%d",f.frame,j,c.key,c.flags,c.claim,c.observed,c.relevant,c.own,c.cache);Event(1,"PROOF_COMPETITOR",text);}
 }
 Event(1,"PROOF_TRANSITION_END","capture_complete no_more_transition_records");
}
