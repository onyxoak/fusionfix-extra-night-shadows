lab_siren::PoliceClaim policeClaim;
std::atomic<uint32_t> policeOverrides{0};
bool BoundPoliceOwnerValid() noexcept {
 if(proofRetired.load()||!proofBound||!CPlayer::findPlayerCar||CPlayer::findPlayerCar()!=proofOwner.load())return false;
 const auto pool=CVehicle::GetVehiclePool();
 return pool&&reinterpret_cast<uintptr_t>(pool)==proofPool&&pool->m_aStorage&&pool->m_aFlags&&pool->m_nSize>0&&
  pool->m_nStorageSize>=0xF1F&&proofSlot<static_cast<uint32_t>(pool->m_nSize)&&!pool->GetIsFree(proofSlot)&&pool->GetReference(proofSlot)==proofReference;
}
bool BoundSirenOn() noexcept {
 if(!BoundPoliceOwnerValid())return false;
 const auto owner=proofOwner.load();return lab_siren::NativeSirenOn(*reinterpret_cast<const uint8_t*>(owner+0xF19),*reinterpret_cast<const uint8_t*>(owner+0xF1E));
}
void BeginPoliceClaim() noexcept {
 policeClaim.Begin(BoundPoliceOwnerValid(),{proofOwner.load(),proofPool,proofSlot,proofReference},proofArmed.load()&&policePolicy.load()==1,BoundSirenOn(),*CShadows::pFrameCounter,GetTickCount());
}
int ComparePoliceClaim(int original,int unchanged,int challenger,int incumbent) noexcept {
 if(nearbyPoliceEnabled)return CompareNearby(original,unchanged,challenger,incumbent);
 if(policePolicy.load()!=1)return CompareProofGrace(original,unchanged,challenger,incumbent);
 using namespace PlayerShadowAllocation;
 const auto* lights=CurrentLights();if(!BoundPoliceOwnerValid()||!BoundSirenOn()||!proofArmed.load()||!lights)return unchanged;
 const bool ap=IsProofPoint(lights[challenger]),bp=IsProofPoint(lights[incumbent]);if(ap==bp)return unchanged;
 const auto index=ap?challenger:incumbent;
 const bool protect=policeClaim.Observe(true,state.nativeCandidates[index].observed&&labRelevant[index]);
 const auto result=lab_siren::PoliceClaim::Compare(unchanged,ap,bp,protect,state.nativeCandidates[challenger],state.nativeCandidates[incumbent]);
 if(result!=unchanged)++policeOverrides;
 return result;
}
struct PolicePacket {
 uint32_t frame{},tick{},mode{},siren{},present{},selected{},rank{},meta{},render{},count{},relevant{};float intensity{};
 struct Slot{uint32_t key{},flags{},own{},relevant{};int cache=-1;} slots[7]{},lost[7]{};
 bool event{};
};
#include "proof8-churn.inc"
struct PoliceBox {std::atomic<unsigned> state{0};PolicePacket value{};};
std::array<PoliceBox,8> policeBoxes;
std::atomic<uint32_t> policeDropped{0};
void CapturePolicePacket(bool present,int rank) noexcept {
 using namespace PlayerShadowAllocation;
 if(!proofBound)return;
 static PolicePacket previous{};static uint32_t lastSample=0;static unsigned events[2]{},samples[2]{};
 PolicePacket p{};p.frame=*CShadows::pFrameCounter;p.tick=GetTickCount();p.mode=policePolicy.load()==1?1:0;p.siren=BoundSirenOn();
 p.present=present;p.selected=rank>=0;p.rank=rank>=0?rank:7;p.render=proofRenderCalls.load();
 const auto* lights=CurrentLights();const auto count=CurrentCount();if(!lights||count>4096)return;
 for(uint32_t i=0;i<count;++i)if(IsProofPoint(lights[i])){p.intensity=lights[i].mIntensity;p.relevant=state.nativeCandidates[i].observed&&labRelevant[i];break;}
 policeClaim.Observe(present,p.relevant!=0);p.meta=policeClaim.Active();
 for(unsigned slot=0;slot<7;++slot){const auto record=game+fusionfix::shadows::ce::allocation::DynamicPassFirstPositionRva+slot*0x110;
  if(!*reinterpret_cast<const uint8_t*>(record+0x25))continue;
  ++p.count;auto& out=p.slots[slot];out.key=*reinterpret_cast<const uint32_t*>(record+0x30);
  for(uint32_t i=0;i<count;++i)if(static_cast<uint32_t>(lights[i].mCastShadows)==out.key){out.flags=lights[i].mFlags;out.own=state.nativeCandidates[i].ownBeam;out.relevant=labRelevant[i];out.cache=lights[i].mShadowCacheIndex;break;}
 }
 for(unsigned i=0;i<7;++i){const auto old=previous.slots[i];if(!old.key)continue;bool retained=false;for(const auto& s:p.slots)if(s.key==old.key)retained=true;if(!retained){p.lost[i]=old;
  for(uint32_t j=0;j<count;++j)if(static_cast<uint32_t>(lights[j].mCastShadows)==old.key){p.lost[i].relevant=labRelevant[j];break;}
 }}
 CountChurn(p,previous);
 p.event=p.mode!=previous.mode||p.siren!=previous.siren||p.selected!=previous.selected;
 const bool event=p.event&&events[p.mode]<48;
 const bool sample=p.tick-lastSample>=250&&samples[p.mode]<96;
 previous=p;if(!event&&!sample)return;
 for(auto& box:policeBoxes){unsigned free=0;if(box.state.compare_exchange_strong(free,1,std::memory_order_acquire)){
  box.value=p;box.state.store(2,std::memory_order_release);if(event)++events[p.mode];if(sample){++samples[p.mode];lastSample=p.tick;}return;
 }}++policeDropped;
}
void PublishPolicePackets(){
 for(auto& box:policeBoxes){unsigned ready=2;if(!box.state.compare_exchange_strong(ready,3,std::memory_order_acquire))continue;
  const auto p=box.value;box.state.store(0,std::memory_order_release);char s[600];
  sprintf_s(s,"frame=%u tick=%u mode=%u siren=%u present=%u relevant=%u selected=%u rank=%u metadata_active=%u intensity=%.6f render=%u active_slots=%u event=%u",p.frame,p.tick,p.mode,p.siren,p.present,p.relevant,p.selected,p.rank,p.meta,p.intensity,p.render,p.count,p.event);Event(1,"POLICE_SAMPLE",s);
  if(p.event){for(unsigned i=0;i<7;++i){const auto c=p.slots[i];sprintf_s(s,"frame=%u slot=%u key=%u flags=%u own=%u relevant=%u cache=%d",p.frame,i,c.key,c.flags,c.own,c.relevant,c.cache);Event(1,"POLICE_SLOT",s);
    const auto d=p.lost[i];if(d.key){sprintf_s(s,"frame=%u key=%u flags=%u own=%u relevant=%u cache=%d",p.frame,d.key,d.flags,d.own,d.relevant,d.cache);Event(1,"POLICE_DISPLACED",s);}
  }}
 }
}

