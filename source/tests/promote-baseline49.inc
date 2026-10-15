uint32_t PromoteBaseline49(SafetyHookContext& regs,const model_effect_observer::Invocation& c,
 const TrafficSignal::Match& match,const std::array<uint8_t,0x6c>& descriptor,
 uint32_t site,const std::array<uint32_t,18>& arguments,bool packetValid) noexcept {
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::ModelPromote,128);
 if(!requested||!active.load(std::memory_order_acquire)||!addonActive.load(std::memory_order_acquire)||!PlayerShadowAllocation::Enabled())return 0;
 const auto abi=model_light_submission20::Layout(site);
 // Retain native cached/static shadow identity; only previously unshadowed
 // native point/spot packets acquire our private dynamic map identity.
 if(!abi||(arguments[2]&6u)||arguments[abi.keyOffset/4]||!packetValid||c.invalid||!c.resolved||
    !std::isfinite(std::bit_cast<float>(arguments[10]))||std::bit_cast<float>(arguments[10])<=0){++refused;return 0;}
 TrafficSignal::Match fresh{};std::array<uint8_t,0x6c> again{};
 if(!TrafficSignal::MatchOwner(c.owner,fresh)||fresh.identity!=match.identity||
    fresh.modelInfo!=c.modelInfo||
    !Liberty20::Copy(c.descriptor,again.data(),again.size())||again!=descriptor){++refused;return 0;}
 if(lock.test_and_set(std::memory_order_acquire)){++refused;return 0;}
 const auto oldUsed=registry.used;
 auto* entry=registry.Observe(fresh.identity,c.ordinal,c.descriptor,c.modelInfo,descriptor,c.frame);
 identitiesPublished32.store(registry.used,std::memory_order_relaxed);
 bool ok=entry!=nullptr;
 if(ok&&registry.used!=oldUsed&&Liberty20::Collision(entry->key)){entry->active=false;ok=false;}
 uint32_t resultKey{};
 if(ok&&active.load(std::memory_order_acquire)){
  auto* words=reinterpret_cast<uint32_t*>(regs.esp);
  words[abi.keyOffset/4]=entry->key;words[2]=arguments[2]|4u;++promoted;
  // Report only the key actually written, without rereading native arguments
  // or exposing a registry entry beyond its existing ownership lock.
  resultKey=entry->key;
 }else ++refused;
 lock.clear(std::memory_order_release);
 return resultKey;
}
