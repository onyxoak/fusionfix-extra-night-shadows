// Private observation only: copied on the existing guarded selection thread.
// No source writes, new hooks, allocation decisions, or raw-pointer logging.
struct LightAuditSnapshot {
 uint32_t tick{},frame{},total{},count{};
 rage::Vector3 player{};
 std::array<rage::CLightSource,64> lights{};
};
LightAuditSnapshot auditSnapshot;
std::atomic<unsigned> auditState{0}; // 0 free, 1 producer owns, 2 ready, 3 consumer owns
bool auditEnabled=false;
void ObserveLights(const rage::CLightSource* lights,uint32_t count) noexcept {
 if(!auditEnabled||count>4096||!lights)return;
 static uint32_t last{};const auto now=GetTickCount();
 if(now-last<250)return;
 unsigned free=0;if(!auditState.compare_exchange_strong(free,1,std::memory_order_acquire))return;
 last=now;auditSnapshot.tick=now;auditSnapshot.frame=*CShadows::pFrameCounter;
 auditSnapshot.total=count;auditSnapshot.count=0;
 const auto p=PlayerShadowAllocation::state.player;auditSnapshot.player={p.x,p.y,p.z};
 for(uint32_t i=0;i<count&&auditSnapshot.count<auditSnapshot.lights.size();++i){
  const auto& l=lights[i];const float x=l.mPosition.x-p.x,y=l.mPosition.y-p.y,z=l.mPosition.z-p.z;
  if(std::isfinite(x)&&std::isfinite(y)&&std::isfinite(z)&&x*x+y*y+z*z<=40.f*40.f)
   auditSnapshot.lights[auditSnapshot.count++]=l;
 }
 auditState.store(2,std::memory_order_release);
}
void PublishLightAudit() {
 if(!auditEnabled)return;
 unsigned ready=2;if(!auditState.compare_exchange_strong(ready,3,std::memory_order_acquire))return;
 const auto snapshot=auditSnapshot;auditState.store(0,std::memory_order_release);
 char text[500];
 sprintf_s(text,"tick=%u frame=%u submitted=%u nearby=%u cap=64 radius=40",snapshot.tick,snapshot.frame,snapshot.total,snapshot.count);
 Event(2,"LIGHT_AUDIT_SAMPLE",text);
 for(uint32_t i=0;i<snapshot.count;++i){const auto& l=snapshot.lights[i];
  sprintf_s(text,"tick=%u key=%u type=%u flags=%u cache=%d pos=%.3f,%.3f,%.3f delta=%.3f,%.3f,%.3f rgb=%.3f,%.3f,%.3f intensity=%.3f radius=%.3f cone=%.3f corona=%d projection=%d",
   snapshot.tick,static_cast<uint32_t>(l.mCastShadows),static_cast<uint32_t>(l.mType),l.mFlags,l.mShadowCacheIndex,
   l.mPosition.x,l.mPosition.y,l.mPosition.z,l.mPosition.x-snapshot.player.x,l.mPosition.y-snapshot.player.y,l.mPosition.z-snapshot.player.z,
   l.mColor.x,l.mColor.y,l.mColor.z,l.mIntensity,l.mRadius,l.mOuterConeAngle,l.mCoronaTexHash,l.mProjTexHash);
  Event(2,"LIGHT_AUDIT",text);
 }
}
