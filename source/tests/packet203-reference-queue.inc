void ValidateQueued(rage::CLightSource* lights,uint32_t count) noexcept {
 const auto frame=*CShadows::pFrameCounter;
 aliases.Begin();sidecar.Begin(reinterpret_cast<uintptr_t>(lights),count,frame);
 // Disabled or failed hooks must add no native-bank permission/read work.
 // Reset first so a formerly active pass cannot retain authorization.
 if(!active.load(std::memory_order_acquire))return;
 if(!lights||count>640)return;
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::ProjectionQueued69,32);
 if(count){++bankPreflights;
  // Current native list is numeric game-owned packet storage, not a borrowed
  // model/owner. One fresh whole-bank committed-region preflight; no cached
  // permissions. Current values are still freshly SEH-copied at each consumer.
  const auto permission=native_projection66::Preflight(reinterpret_cast<uintptr_t>(lights),size_t(count)*sizeof(rage::CLightSource));
  permissionQueries.fetch_add(permission.queries,std::memory_order_relaxed);
  if(!permission.allowed){++bankDenied;return;}
 }
 // Snapshot every own-domain entry as invalid before attempting registry lock.
 // A failed bounded read cannot authorize any insertion in this pass.
 for(unsigned i=0;i<count;++i){Packet packet{};
  if(CopyPacket(lights[i],packet,false)&&OwnKey(packet[24]))sidecar.Own(i,packet);
 }
 if(lock.test_and_set(std::memory_order_acquire)){++contended;return;}
 for(unsigned i=0;i<count;++i){
  if(sidecar.decisions[i]!=native_projection66::Decision::Invalid)continue;
  const auto& packet=sidecar.packets[i];const auto key=packet[24];
  const float weight=fusionfix::shadows::SceneBrightness44(std::bit_cast<float>(packet[16]),
   {std::bit_cast<float>(packet[12]),std::bit_cast<float>(packet[13]),std::bit_cast<float>(packet[14])});
  if(!registry.Validate(key,packet)||!aliases.Add(i,key,packet,weight,registry.snowMetadata70)){++invalidQueued;continue;}
  sidecar.Accept(i,false);++accepted;
 }
 for(unsigned i=0;i<count;++i)if(sidecar.decisions[i]==native_projection66::Decision::Valid){
  sidecar.Accept(i,aliases.IsAlias(i));if(aliases.IsAlias(i))++aliasPackets;
 }
 lock.clear(std::memory_order_release);
}
// 0 ordinary,1 valid owned projection,2 same-projection shadow alias,3 invalid.
// The consumer checks exact native index/list/stride/stack flags before using
// this numeric sidecar. Invalid/alias bypass ONLY shadow insertion via EIP;
// separate RGB/intensity packets remain in the original native lighting bank.
unsigned CollectDecision(unsigned index,rage::CLightSource* lights,uint32_t count,uint32_t frame,uintptr_t stack) noexcept {
 if(!active.load(std::memory_order_acquire))return 3;
 if(!lights||index>=count||count>640)return 3;
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::ProjectionWinner69,128);
 Packet packet{};
 if(!CopyPacket(lights[index],packet)) {
  return sidecar.Envelope(reinterpret_cast<uintptr_t>(lights),count,frame)&&
   sidecar.decisions[index]!=native_projection66::Decision::Ordinary?3u:0u;
 }
 if(!OwnKey(packet[24]))return 0;
 if(!active.load(std::memory_order_acquire))return 3;
 const auto decision=sidecar.Current(index,packet,reinterpret_cast<uintptr_t>(lights),count,frame,stack);
 return decision==native_projection66::Decision::Valid?1u:decision==native_projection66::Decision::Alias?2u:3u;
}
unsigned Classify(unsigned index,uint32_t key,rage::CLightSource* lights,uint32_t count,uint32_t frame) noexcept {
 if(!OwnKey(key))return 0;
 if(!active.load(std::memory_order_acquire)||!sidecar.Envelope(reinterpret_cast<uintptr_t>(lights),count,frame)||index>=count)return 3;
 const auto d=sidecar.decisions[index];
 return sidecar.packets[index][24]==key&&(d==native_projection66::Decision::Valid||d==native_projection66::Decision::Alias)?1u:3u;
}
bool Alias(unsigned index,uint32_t key,rage::CLightSource* lights,uint32_t count,uint32_t frame) noexcept {
 // Multiplicity hint only: never authorizes native insertion or map reuse.
 return Classify(index,key,lights,count,frame)==1&&sidecar.decisions[index]==native_projection66::Decision::Alias;
}
