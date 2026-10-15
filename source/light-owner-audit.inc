// Clone-only producer observation. No writes to arguments, lights or pool.
SafetyHookMid ownerProbe;
bool ownerAuditEnabled=false;
constexpr std::array<uint32_t,6> sirenSites{0x640d0a,0x64126e,0x64161d,0x6418f0,0x642053,0x6425ab};
struct OwnerObservation {uint32_t tick{},site{},owner{},type{},flags{},key{};float position[3]{},color[3]{},radius{},intensity{};};
struct OwnerMailbox {std::atomic<unsigned> state{0};OwnerObservation value{};};
std::array<OwnerMailbox,6> ownerMailboxes;
void ObserveOwner(SafetyHookContext& regs) noexcept {
 const fusionfix::shadows::FloatingPointState fp;
 if(!ownerAuditEnabled)return;
 const auto* stack=reinterpret_cast<const uint32_t*>(regs.esp);
 const uint32_t caller=stack[0]-static_cast<uint32_t>(game)-5;
 size_t index=0;while(index<sirenSites.size()&&sirenSites[index]!=caller)++index;
 if(index==sirenSites.size())return;
 auto& box=ownerMailboxes[index];unsigned empty=0;
 if(!box.state.compare_exchange_strong(empty,1,std::memory_order_acquire))return;
 const uint32_t now=GetTickCount();
 if(now-box.value.tick<250){box.state.store(0,std::memory_order_release);return;}
 auto& o=box.value;o.tick=now;o.site=caller;o.owner=regs.edi;
 o.type=stack[2];o.flags=stack[3];o.key=stack[16];
 // Exact guarded wrapper ABI: direction, tangent, position, RGB, intensity.
 memcpy(o.position,reinterpret_cast<const void*>(stack[6]),12);
 memcpy(o.color,reinterpret_cast<const void*>(stack[7]),12);
 memcpy(&o.intensity,&stack[8],4);memcpy(&o.radius,&stack[11],4);
 box.state.store(2,std::memory_order_release);
}
void PublishOwnerAudit(){
 if(!ownerAuditEnabled)return;
 const auto pool=CVehicle::GetVehiclePool();
 const bool valid=pool&&pool->m_aStorage&&pool->m_aFlags&&pool->m_nSize>0&&pool->m_nSize<=4096&&pool->m_nStorageSize>=0x24&&pool->m_nStorageSize<=0x10000;
 for(auto& box:ownerMailboxes){unsigned ready=2;if(!box.state.compare_exchange_strong(ready,3,std::memory_order_acquire))continue;
  const auto o=box.value;box.state.store(0,std::memory_order_release);
  int slot=-1,reference=-1;bool occupied=false;
  if(valid){const auto base=reinterpret_cast<uintptr_t>(pool->m_aStorage);const auto bytes=uint64_t(pool->m_nSize)*pool->m_nStorageSize;
   if(o.owner>=base&&uint64_t(o.owner-base)<bytes&&(o.owner-base)%pool->m_nStorageSize==0){slot=static_cast<int>((o.owner-base)/pool->m_nStorageSize);if(!pool->GetIsFree(slot)){reference=pool->GetReference(slot);occupied=CPlayer::findPlayerCar&&CPlayer::findPlayerCar()==o.owner;}}}
  char text[450];sprintf_s(text,"tick=%u site=%x pool_slot=%d reference=%d occupied=%d type=%u flags=%u input_key=%u pos=%.3f,%.3f,%.3f rgb=%.3f,%.3f,%.3f radius=%.3f intensity=%.5f",o.tick,o.site,slot,reference,occupied?1:0,o.type,o.flags,o.key,o.position[0],o.position[1],o.position[2],o.color[0],o.color[1],o.color[2],o.radius,o.intensity);
  Event(2,"SIREN_OWNER",text);
 }
}
