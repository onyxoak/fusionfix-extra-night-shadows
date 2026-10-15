// Bounded observation only: no writes to native lights, slots, caches or arguments.
struct NineEvent {uint32_t stage{},frame{},tick{},thread{},key{};uint64_t generation{};int slot=-1,cache=-1;uint32_t cacheOwner{},flags{},present{},relevant{};float intensity{},red{},green{},blue{};};
struct NineBox {std::atomic<unsigned> ready{0};NineEvent e;};
std::array<NineBox,65536> nineEvents;
std::atomic<uint32_t> nineCount{0},nineStart{0};
std::atomic<bool> nineClosed{false};
void NineWrite(NineEvent e) noexcept {
 if(!diagnosticsEnabled||nineClosed.load()||!proofArmed.load()||proofRetired.load())return;
 e.frame=*CShadows::pFrameCounter;e.tick=GetTickCount();e.thread=GetCurrentThreadId();
 uint32_t zero=0;nineStart.compare_exchange_strong(zero,e.frame);
 if(e.frame-nineStart.load()>=1024){nineClosed=true;return;}
 const auto i=nineCount.fetch_add(1);if(i>=nineEvents.size()){nineClosed=true;return;}
 nineEvents[i].e=e;nineEvents[i].ready.store(1,std::memory_order_release);
}
void NineInput(const uint32_t* args) noexcept {
 if(!diagnosticsEnabled)return;
 NineEvent e{};e.stage=0;e.key=ProofKey();e.flags=args[3];e.present=1;
 memcpy(&e.intensity,args+8,4);const auto* color=reinterpret_cast<const float*>(args[7]);e.red=color[0];e.green=color[1];e.blue=color[2];NineWrite(e);
}
void NineSelection() noexcept {
 using namespace PlayerShadowAllocation;
 if(!diagnosticsEnabled||!proofArmed.load()||nineClosed.load())return;
 const auto* lights=CurrentLights();const auto count=CurrentCount();if(!lights||count>4096)return;
 for(uint32_t i=0;i<count;++i){const auto& l=lights[i];const auto key=static_cast<uint32_t>(l.mCastShadows);if(!key)continue;
  int slot=-1;for(unsigned j=0;j<7;++j){const auto p=game+fusionfix::shadows::ce::allocation::DynamicPassFirstPositionRva+j*0x110;if(*reinterpret_cast<const uint8_t*>(p+0x25)&&*reinterpret_cast<const uint32_t*>(p+0x30)==key)slot=j+1;}
  if(slot<0&&!IsProofPoint(l)&&!((l.mFlags&0x100u)&&state.nativeCandidates[i].observed&&labRelevant[i]))continue;
  NineEvent e{};e.stage=1;e.key=key;e.generation=IsProofPoint(l)?LampGeometry(l):((l.mFlags&0x100u)?0:LampGeometry(l));e.slot=slot;e.cache=l.mShadowCacheIndex;
  if(e.cache>=0&&e.cache<8)e.cacheOwner=*reinterpret_cast<const uint32_t*>(game+fusionfix::shadows::ce::allocation::CacheAge0Rva+e.cache*0x100+4);
  e.flags=l.mFlags;e.present=1;e.relevant=state.nativeCandidates[i].observed&&labRelevant[i];e.intensity=l.mIntensity;e.red=l.mColor.x;e.green=l.mColor.y;e.blue=l.mColor.z;NineWrite(e);
 }
 NineEvent done{};done.stage=3;done.key=ProofKey();done.flags=BoundSirenOn();NineWrite(done);
}
void NineRender(uint32_t key,uint32_t slot,uint32_t kind,bool active) noexcept {
 if(nearbyPoliceEnabled){ObserveNearbyRender(key,slot,kind,active);return;}
 ObserveProofRender(key,slot,kind,active);
 if(!diagnosticsEnabled||!active||slot<1||slot>7)return;
 NineEvent e{};e.stage=2;e.key=key;e.slot=slot;e.flags=kind;e.present=active;
 const auto p=game+fusionfix::shadows::ce::allocation::DynamicPassFirstPositionRva+(slot-1)*0x110;
 e.cache=*reinterpret_cast<const int*>(p+0x28);
 if(e.cache>=0&&e.cache<8)e.cacheOwner=*reinterpret_cast<const uint32_t*>(game+fusionfix::shadows::ce::allocation::CacheAge0Rva+e.cache*0x100+4);
 NineWrite(e);
}
void PublishNine(){
 if(!diagnosticsEnabled)return;
 static bool saved=false;if(saved||!nineClosed.load())return;
 const auto count=(std::min)(nineCount.load(),static_cast<uint32_t>(nineEvents.size()));
 for(uint32_t i=0;i<count;++i)if(!nineEvents[i].ready.load(std::memory_order_acquire))return;
 saved=true;const auto path=pe::ModulePath(self).replace_extension(L".proof9.csv");
 HANDLE file=CreateFileW(path.c_str(),GENERIC_WRITE,FILE_SHARE_READ,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);
 if(file==INVALID_HANDLE_VALUE){Event(1,"NINE_WRITE_FAILED","existing_file_or_access");return;}
 std::string out="stage,frame,tick,thread,key,generation,slot,cache,cacheOwner,flags,present,relevant,intensity,red,green,blue\n";
 char line[350];for(uint32_t i=0;i<count;++i){const auto& e=nineEvents[i].e;sprintf_s(line,"%u,%u,%u,%u,%u,%llu,%d,%d,%u,%u,%u,%u,%.9g,%.9g,%.9g,%.9g\n",e.stage,e.frame,e.tick,e.thread,e.key,e.generation,e.slot,e.cache,e.cacheOwner,e.flags,e.present,e.relevant,e.intensity,e.red,e.green,e.blue);out+=line;}
 DWORD written=0;const bool ok=WriteFile(file,out.data(),static_cast<DWORD>(out.size()),&written,nullptr)&&written==out.size();CloseHandle(file);
 Event(1,ok?"NINE_TRACE_SAVED":"NINE_WRITE_FAILED",std::string("events=")+std::to_string(count)+" frames=1024 bounded_no_native_writes");
}
