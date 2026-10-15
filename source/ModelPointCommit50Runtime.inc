// Final scalar dependency rereads match the legacy matcher tail, after the
// detailed/model/matrix/descriptor copies. Earlier page checks are callback-
// local; this raw-read leaf still contains revocation/guard races with SEH.
__declspec(noinline) bool TailPointIdentity50(const TrafficSignal::OwnerResult49& staged,uint8_t reference) noexcept {
 __try {
  for(unsigned k=0;k<2;++k){
   if(*reinterpret_cast<const volatile uintptr_t*>(game+(k?0x1232c60:0xebd0e8))!=staged.pools[k])return false;
   const auto* h=reinterpret_cast<const rage::VehiclePool*>(staged.pools[k]);const auto& a=staged.headers[k];
   if(h->m_aStorage!=a.m_aStorage||h->m_aFlags!=a.m_aFlags||h->m_nSize!=a.m_nSize||h->m_nStorageSize!=a.m_nStorageSize)return false;
  }
  const auto& id=staged.match.identity;
  if(*reinterpret_cast<const volatile uint8_t*>(uintptr_t(id.flags)+id.slot)!=reference)return false;
  if(*reinterpret_cast<const volatile int16_t*>(uintptr_t(id.owner)+0x2e)!=staged.modelIndex||
     *reinterpret_cast<const volatile uintptr_t*>(uintptr_t(id.owner)+0x20)!=staged.matrixPointer)return false;
  return *reinterpret_cast<const volatile uintptr_t*>(game+0xe95cd8+staged.modelIndex*4)==staged.match.modelInfo;
 } __except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION||GetExceptionCode()==EXCEPTION_IN_PAGE_ERROR||
 GetExceptionCode()==EXCEPTION_GUARD_PAGE?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){return false;}
}
// Included inside ModelLightShadow20. Initial evidence and final complete
// fresh evidence must agree before any native stack or registry publication.
uint32_t PublishFreshPointLocked50(SafetyHookContext& regs,const model_effect_observer::Invocation& c,
 const TrafficSignal::OwnerResult49& staged,const std::array<uint8_t,0x6c>& descriptor,
 uint32_t site,const std::array<uint32_t,18>& arguments,bool packetValid,
 std::array<uint32_t,32>* owned79=nullptr) noexcept {
 if(!requested||!active.load(std::memory_order_acquire)||!addonActive.load(std::memory_order_acquire)||!PlayerShadowAllocation::Enabled())return 0;
 const auto abi=model_light_submission20::Layout(site);
 if(!abi||(arguments[2]&6u)||arguments[abi.keyOffset/4]||!packetValid||c.invalid||!c.resolved||
    c.frame!=*CShadows::pFrameCounter||!std::isfinite(std::bit_cast<float>(arguments[10]))||std::bit_cast<float>(arguments[10])<=0){++refused;return 0;}
 rage::VehiclePool headers[2]{};std::array<uint8_t,0x30> owner{};uint8_t reference{};
 uint32_t model{};std::array<float,16> matrix{};std::array<uint8_t,0x6c> bytes{};
 const auto& id=staged.match.identity;
 const traffic_signal::fresh_batch30::Span spans[]{
  {staged.pools[0],&headers[0],sizeof(headers[0])},{staged.pools[1],&headers[1],sizeof(headers[1])},
  {c.owner,owner.data(),owner.size()},{uintptr_t(id.flags)+id.slot,&reference,1},
  {uintptr_t(staged.match.modelInfo)+0x3c,&model,4},{staged.matrixPointer,matrix.data(),sizeof(matrix)},
  {c.descriptor,bytes.data(),bytes.size()}};
 bool fault=false;
 if(!traffic_signal::fresh_group50::Copy(spans,7,TrafficSignal::ReadableOriginal,QueryWorkingSetEx,&fault)||
    (reference&0x80)||reference!=id.reference||model!=id.model||bytes!=descriptor){++refused;return 0;}
 for(unsigned k=0;k<2;++k){uintptr_t pool{};
  if(!TrafficSignal::NativeWord49(game+(k?0x1232c60:0xebd0e8),pool)){++refused;return 0;}
  const auto& a=staged.headers[k];const auto& b=headers[k];
  if(pool!=staged.pools[k]||a.m_aStorage!=b.m_aStorage||a.m_aFlags!=b.m_aFlags||a.m_nSize!=b.m_nSize||a.m_nStorageSize!=b.m_nStorageSize){++refused;return 0;}
 }
 int16_t index{};uintptr_t pointer{},info{};std::memcpy(&index,owner.data()+0x2e,2);std::memcpy(&pointer,owner.data()+0x20,4);
 if(index!=staged.modelIndex||pointer!=staged.matrixPointer||!TrafficSignal::NativeWord49(game+0xe95cd8+index*4,info)||
    info!=staged.match.modelInfo||info!=c.modelInfo||c.frame!=*CShadows::pFrameCounter){++refused;return 0;}
 for(unsigned j=0;j<3;++j)if(!std::isfinite(matrix[12+j])){++refused;return 0;}
 if(!TailPointIdentity50(staged,reference)||!active.load(std::memory_order_acquire)||
    c.frame!=*CShadows::pFrameCounter){++refused;return 0;}
 const auto oldUsed=registry.used;
 auto* entry=registry.Observe(id,c.ordinal,c.descriptor,c.modelInfo,descriptor,c.frame);
 identitiesPublished32.store(registry.used,std::memory_order_relaxed);bool ok=entry!=nullptr;
 if(ok&&registry.used!=oldUsed&&Liberty20::Collision(entry->key)){entry->active=false;ok=false;}
 uint32_t key{};
 if(ok&&active.load(std::memory_order_acquire)&&c.frame==*CShadows::pFrameCounter&&registry.Publish(entry->key)){
  // Constructor fallback owns a normalized temporary, not the pre-call stack.
  // Preserve native classification/FF metadata and every illumination field.
  // Legacy callers retain their exact original two stack stores.
  if(owned79){(*owned79)[24]=entry->key;(*owned79)[18]|=4u;}
  else {auto* words=reinterpret_cast<uint32_t*>(regs.esp);words[abi.keyOffset/4]=entry->key;words[2]=arguments[2]|4u;}
  ++promoted;key=entry->key;
 }else ++refused;
 return key;
}

uint32_t PublishFreshPoint50(SafetyHookContext& regs,const model_effect_observer::Invocation& c,
 const TrafficSignal::OwnerResult49& staged,const std::array<uint8_t,0x6c>& descriptor,
 uint32_t site,const std::array<uint32_t,18>& arguments,bool packetValid,
 std::array<uint32_t,32>* owned79=nullptr) noexcept {
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::ModelPromote,128);
 if(!requested||!active.load(std::memory_order_acquire)||!addonActive.load(std::memory_order_acquire)||!PlayerShadowAllocation::Enabled())return 0;
 if(lock.test_and_set(std::memory_order_acquire)){++refused;return 0;}
 // Serialize final current evidence with registry publication. A native free
 // racing this scope follows the existing fail-closed OnFree contention path
 // and clears active; the final gate refuses native key/flag publication.
 const auto key=PublishFreshPointLocked50(regs,c,staged,descriptor,site,arguments,packetValid,owned79);
 lock.clear(std::memory_order_release);return key;
}
