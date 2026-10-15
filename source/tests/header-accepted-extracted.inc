bool Readable(uintptr_t address,size_t size) noexcept {
 if(address<0x10000||!size||address>UINTPTR_MAX-size)return false;
 MEMORY_BASIC_INFORMATION info{};
 if(!VirtualQuery(reinterpret_cast<void*>(address),&info,sizeof(info))||info.State!=MEM_COMMIT||
    (info.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;
 const auto base=reinterpret_cast<uintptr_t>(info.BaseAddress);
 return address>=base&&address+size<=base+info.RegionSize;
}
bool PoolMatch(uintptr_t owner,uint32_t pointerRva,uint32_t stride,uint32_t kind,Match& result) noexcept {
 const auto pool=*reinterpret_cast<rage::VehiclePool**>(game+pointerRva);
 if(!Readable(reinterpret_cast<uintptr_t>(pool),0x1c))return false;
 if(pool->m_nSize<=0||pool->m_nSize>65536||pool->m_nStorageSize!=static_cast<int32_t>(stride))return false;
 const auto storage=reinterpret_cast<uintptr_t>(pool->m_aStorage),flags=reinterpret_cast<uintptr_t>(pool->m_aFlags);
 const uint64_t bytes=uint64_t(pool->m_nSize)*stride;
 if(owner<storage||uint64_t(owner-storage)>=bytes||(owner-storage)%stride||!Readable(owner,0x30))return false;
 const auto slot=uint32_t((owner-storage)/stride);
 if(!Readable(flags+slot,1))return false;const auto reference=pool->m_aFlags[slot];if(reference&0x80)return false;
 const auto index=*reinterpret_cast<const int16_t*>(owner+0x2e);if(index<0||index>=31000)return false;
 const auto info=*reinterpret_cast<const uintptr_t*>(game+0xe95cd8+index*4);
 if(!Readable(info,0x40))return false;const auto model=*reinterpret_cast<const uint32_t*>(info+0x3c);if(!model)return false;
 const auto matrix=*reinterpret_cast<const uintptr_t*>(owner+0x20);if(!Readable(matrix,64))return false;
 for(unsigned i=0;i<3;++i){result.entity[i]=reinterpret_cast<const float*>(matrix)[12+i];if(!std::isfinite(result.entity[i]))return false;}
 if(pool->m_aFlags[slot]!=reference)return false;
 result.identity={uint32_t(reinterpret_cast<uintptr_t>(pool)),uint32_t(storage),uint32_t(flags),uint32_t(owner),slot,reference,model};result.kind=kind;return true;
}
bool MatchOwner(uintptr_t owner,Match& value) noexcept {
 Match b{},o{};const bool building=PoolMatch(owner,0xebd0e8,0x70,0,b),object=PoolMatch(owner,0x1232c60,0x320,1,o);
 if(building==object)return false;value=building?b:o;return true;
}
void Retire(uint32_t index,traffic_signal::End reason,uint32_t event,uint32_t frame) noexcept {
 if(index>=registry.Used())return;
 if(registry.Retire(index,reason)){snapshots[index].state.store(2,std::memory_order_release);RecordEvent(event,index,frame);}
}
void NativeTimerRestore(SafetyHookContext& regs) noexcept {
 const fusionfix::shadows::FloatingPointState fp;
 if(!enabled.load()||fatal.load())return;
 if(GetCurrentThreadId()!=observerThread.load()){FatalThread();return;}
 const auto frame=*CShadows::pFrameCounter,saved=*reinterpret_cast<const uint32_t*>(game+0xd73684);
 if(*reinterpret_cast<const uint8_t*>(game+0xd73610)!=1||frame!=saved)return;
 uint32_t caller=0;if(Readable(regs.esp,4))caller=*reinterpret_cast<const uint32_t*>(regs.esp);
 for(uint32_t i=0;i<registry.Used();++i){auto& entry=registry.entries[i];if(entry.retired||!entry.leased)continue;
  Match current{};if(!MatchOwner(entry.id.owner,current)||!(current.identity==entry.id)){Retire(i,traffic_signal::End::Identity,6,frame);continue;}
  const auto previous=entry.lastFrame;
  if(registry.Rebase(i,current.identity,frame,saved,true)){snapshots[i].state=0;experiment_perf::Count(hotCountersEnabled,timerRebases);
   Record r{};r.event=13;r.index=i;r.tick=GetTickCount();r.frame=frame;r.previous=previous;r.caller=caller;r.thread=GetCurrentThreadId();Emit(r);}
 }
}