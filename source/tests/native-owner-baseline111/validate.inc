bool ValidateOne48(rage::CLightSource& l) noexcept {
 const auto key=uint32_t(l.mCastShadows);
 if(!registry.Contains(key)||!(l.mFlags&4u))return true;
 const traffic_signal::fresh_page::Operation44 queuedReadOperation47;
 bool valid=false;
 if(active.load(std::memory_order_acquire)&&!lock.test_and_set(std::memory_order_acquire)){
  auto* entry=registry.Lookup(key);TrafficSignal::Match fresh{};std::array<uint8_t,0x6c> bytes{};
  valid=entry&&entry->active&&TrafficSignal::MatchOwner(entry->id.owner,fresh)&&
   fresh.modelInfo==entry->modelInfo&&Liberty20::Copy(entry->descriptor,bytes.data(),bytes.size())&&
   registry.Validate(key,fresh.identity,bytes,*CShadows::pFrameCounter);
  lock.clear(std::memory_order_release);
 }
 if(!valid){l.mFlags&=~4u;++retired;}
 return valid;
}
void ValidateQueued(rage::CLightSource* lights,uint32_t count) noexcept {
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::ModelQueued,32);
 if(!lights||count>4096)return;
 if(queuedWindow49&&!PlayerShadowAllocation::shortlistRequested48){ValidateQueuedWindow49(lights,count);return;}
 for(unsigned i=0;i<count;++i){auto& l=lights[i];const auto key=uint32_t(l.mCastShadows);
  if(!registry.Contains(key)||!(l.mFlags&4u))continue;
  if(PlayerShadowAllocation::DeferModel48(l,i))continue;
  ValidateOne48(l);
 }
}
