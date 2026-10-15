// Clone-only expansion of the accepted Proof9b producer policy.
lab_siren::NearbyPolice nearbyPolice;
alignas(64) std::array<std::array<uint8_t,64>,lab_siren::NearbyPolice::Tokens> nearbyKeys{};
std::array<std::atomic<bool>,lab_siren::NearbyPolice::Tokens> nearbyKeyUsed{};
std::array<lab_siren::Vacancy,lab_siren::NearbyPolice::Candidates> nearbyVacancies{};
std::array<uint32_t,lab_siren::NearbyPolice::Candidates> nearbyVacancyGeneration{};
std::atomic_flag nearbyLock=ATOMIC_FLAG_INIT;
std::atomic<uint32_t> nearbyBound{0},nearbyPromoted{0},nearbySelected{0},nearbyRendered{0},nearbyMaxSelected{0},nearbyContention{0};
struct NearbyLock {
 bool acquired;NearbyLock() noexcept:acquired(!nearbyLock.test_and_set(std::memory_order_acquire)){if(!acquired)experiment_perf::Count(hotCountersEnabled,nearbyContention);}
 ~NearbyLock(){if(acquired)nearbyLock.clear(std::memory_order_release);}
};
uint32_t NearbyKey(unsigned token) noexcept {return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(nearbyKeys[token].data()));}
bool NearbyPoint(const rage::CLightSource& l) noexcept {
 if(l.mType!=0)return false;const auto key=static_cast<uint32_t>(l.mCastShadows),base=NearbyKey(0);
 const auto token=experiment_perf::TokenIndex(key,base,nearbyKeys.size());if(token==nearbyKeys.size())return false;
 return nearbyKeyUsed[token].load(std::memory_order_acquire);
}
// CE offsets/table are independently guarded against this executable. The
// hash field is read only; unfamiliar/custom model hashes stay unmodified.
bool ReadNearbyOwner(uintptr_t owner,lab_siren::Identity& id,uint32_t& model,bool& on) noexcept {
 __try {
  const auto pool=CVehicle::GetVehiclePool();if(!owner||!pool||!pool->m_aStorage||!pool->m_aFlags||pool->m_nSize<=0||pool->m_nSize>4096||pool->m_nStorageSize<0xF1F||pool->m_nStorageSize>0x10000)return false;
  const auto base=reinterpret_cast<uintptr_t>(pool->m_aStorage);if(owner<base||uint64_t(owner-base)>=uint64_t(pool->m_nSize)*pool->m_nStorageSize||(owner-base)%pool->m_nStorageSize)return false;
  const auto slot=static_cast<uint32_t>((owner-base)/pool->m_nStorageSize);if(pool->GetIsFree(slot))return false;
  const int index=*reinterpret_cast<const int16_t*>(owner+0x2E);if(index<0||index>=31000)return false;
  const auto info=*reinterpret_cast<const uintptr_t*>(game+0xE95CD8+index*4);if(info<0x10000||(info&3))return false;
  model=*reinterpret_cast<const uint32_t*>(info+0x3C);if(!lab_siren::PoliceModel(model))return false;
  id={owner,reinterpret_cast<uintptr_t>(pool),slot,pool->GetReference(slot)};
  on=lab_siren::NativeSirenOn(*reinterpret_cast<const uint8_t*>(owner+0xF19),*reinterpret_cast<const uint8_t*>(owner+0xF1E));return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool NearbyOrigin(fusionfix::shadows::Vec3& origin) noexcept {
 __try {const auto ped=CPlayer::getLocalPlayerPed?CPlayer::getLocalPlayerPed():0;if(!ped)return false;
  const auto matrix=*reinterpret_cast<const float* const*>(ped+0x20);if(!matrix)return false;origin={matrix[12],matrix[13],matrix[14]};
  return std::isfinite(origin.x)&&std::isfinite(origin.y)&&std::isfinite(origin.z);
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool NearbyValid(lab_siren::NearbyPolice::Owner& o) noexcept {
 lab_siren::Identity id{};uint32_t model{};bool on=false;
 if(!ReadNearbyOwner(o.identity.owner,id,model,on)||!(id==o.identity)||model!=o.model)return false;
 o.eligible=o.eligible&&on&&proofRequested.load();o.player=CPlayer::findPlayerCar&&CPlayer::findPlayerCar()==o.identity.owner;return true;
}
void RefreshNearby() noexcept {nearbyPolice.Begin(*CShadows::pFrameCounter,GetTickCount(),NearbyValid);}
bool NearbyNativeClear(unsigned token) noexcept {
 const uint32_t key=NearbyKey(token);const auto* lights=PlayerShadowAllocation::CurrentLights();const auto count=PlayerShadowAllocation::CurrentCount();
 if(count>4096||(count&&reinterpret_cast<uintptr_t>(lights)<0x10000))return false;
 for(uint32_t i=0;i<count;++i)if(static_cast<uint32_t>(lights[i].mCastShadows)==key)return false;
 for(unsigned i=0;i<8;++i)if(*reinterpret_cast<const uint32_t*>(game+fusionfix::shadows::ce::allocation::CacheAge0Rva+i*0x100+4)==key)return false;
 for(unsigned i=0;i<7;++i)if(*reinterpret_cast<const uint32_t*>(game+fusionfix::shadows::ce::allocation::DynamicPassFirstPositionRva+i*0x110+0x30)==key)return false;
 return true;
}
void UpdateNearby() noexcept {
 static uint32_t checked=0;const auto now=GetTickCount();if(now-checked>=500){checked=now;
  const auto ini=pe::ModulePath(self).replace_extension(L".ini");proofRequested=GetPrivateProfileIntW(L"LAB",L"PoliceProof",0,ini.c_str())==1;
  policePolicy=GetPrivateProfileIntW(L"LAB",L"PolicePolicy",0,ini.c_str())==1?1:0;vacancyRequested=GetPrivateProfileIntW(L"LAB",L"PoliceVacancy",0,ini.c_str())==1;
 }
 NearbyLock lock;if(!lock.acquired)return;RefreshNearby();unsigned bound=0;for(const auto& o:nearbyPolice.owners)if(o.live)++bound;nearbyBound=bound;
}
void PromoteNearby(SafetyHookContext& regs,uint32_t caller) noexcept {
 if(!addonActive.load(std::memory_order_acquire)||!proofRequested.load()||!lab_siren::PoliceSite(caller))return;
 auto* args=reinterpret_cast<uint32_t*>(regs.esp);if(args[2]!=0||args[3]!=0x110||args[16]!=0)return;
 const auto owner=lab_siren::PoliceOwnerAtSite(caller,regs.edi,regs.esi,regs.ecx);
 lab_siren::Identity id{};uint32_t model{};bool on=false;if(!ReadNearbyOwner(owner,id,model,on)||!on)return;
 float input{},radius{};memcpy(&input,args+8,4);memcpy(&radius,args+11,4);
 if(!std::isfinite(input)||input<0||!std::isfinite(radius)||radius<=0||radius>100)return;
 const auto* xyz=reinterpret_cast<const float*>(args[6]);if(reinterpret_cast<uintptr_t>(xyz)<0x10000)return;
 const fusionfix::shadows::Vec3 position{xyz[0],xyz[1],xyz[2]};fusionfix::shadows::Vec3 origin{};if(!NearbyOrigin(origin))return;
 const float x=position.x-origin.x,y=position.y-origin.y,z=position.z-origin.z,d=x*x+y*y+z*z;
 const bool player=CPlayer::findPlayerCar&&CPlayer::findPlayerCar()==owner;
 const bool eligible=std::isfinite(d)&&(player||d<=30.f*30.f)&&fusionfix::shadows::ShadowVolumeMayReachView(PlayerShadowAllocation::ReadGameplayView(),position,radius);
 NearbyLock lock;if(!lock.acquired)return;RefreshNearby();
 auto* o=nearbyPolice.Touch(id,model,caller,d,player,eligible,NearbyNativeClear);if(!o)return;
 nearbyKeyUsed[o->token].store(true,std::memory_order_release);
 if(!nearbyPolice.Promote(*o,args[8]))return;
 args[3]|=4u;args[16]=NearbyKey(o->token);experiment_perf::Count(hotCountersEnabled,nearbyPromoted);
}
lab_siren::NearbyPolice::Owner* NearbyOwnerForKey(uint32_t key) noexcept {const auto token=experiment_perf::TokenIndex(key,NearbyKey(0),nearbyKeys.size());if(token==nearbyKeys.size())return nullptr;for(auto& o:nearbyPolice.owners)if(o.live&&o.token==token)return &o;return nullptr;}
void BeginNearby(const rage::CLightSource*,uint32_t) noexcept {NearbyLock lock;if(lock.acquired)RefreshNearby();}
int CompareNearby(int,int unchanged,int challenger,int incumbent) noexcept {
 if(policePolicy.load()!=1||!proofRequested.load())return unchanged;using namespace PlayerShadowAllocation;
 const auto* lights=CurrentLights();if(!lights||challenger<0||incumbent<0||static_cast<uint32_t>(challenger)>=CurrentCount()||static_cast<uint32_t>(incumbent)>=CurrentCount())return unchanged;
 NearbyLock lock;if(!lock.acquired)return unchanged;
 auto* a=NearbyOwnerForKey(static_cast<uint32_t>(lights[challenger].mCastShadows));auto* b=NearbyOwnerForKey(static_cast<uint32_t>(lights[incumbent].mCastShadows));
 const bool ap=a&&a->rank<2&&a->eligible&&NearbyValid(*a),bp=b&&b->rank<2&&b->eligible&&NearbyValid(*b);
 const auto& ca=state.nativeCandidates[challenger];const auto& cb=state.nativeCandidates[incumbent];
 const auto important=[&](int i,bool privatePoint){const auto& l=lights[i];return !privatePoint&&!(l.mFlags&0x100u)&&labRelevant[i]&&fusionfix::shadows::ShadowVolumeMayReachView(state.view,{l.mPosition.x,l.mPosition.y,l.mPosition.z},l.mRadius);};
 return experiment_perf::CompareNearby(unchanged,ap,bp,ap?a->rank:2,bp?b->rank:2,ca.observed,cb.observed,labRelevant[challenger]!=0,labRelevant[incumbent]!=0,ca.ownBeam,cb.ownBeam,(ca.flags&0x400u)!=0,(cb.flags&0x400u)!=0,[&]{return important(challenger,NearbyPoint(lights[challenger]));},[&]{return important(incumbent,NearbyPoint(lights[incumbent]));});
}
void ApplyNearbyVacancy(SafetyHookContext& regs) noexcept {
 using namespace PlayerShadowAllocation;
 if(!Enabled()||state.depth!=1||!state.pass.Active()||!state.continuityActive||!state.stackAnchor||regs.esp!=state.stackAnchor||!CShadows::pFrameCounter||*CShadows::pFrameCounter!=state.frame)return;
 const auto* lights=CurrentLights();const auto count=CurrentCount();if(!lights||count>4096)return;
 NearbyLock lock;if(!lock.acquired)return;auto* indices=reinterpret_cast<int32_t*>(regs.esp+allocation::SelectedIndicesStackOffset);
 for(unsigned oi=0;oi<nearbyPolice.owners.size();++oi){auto& o=nearbyPolice.owners[oi];if(!o.live)continue;
  if(nearbyVacancyGeneration[oi]!=o.generation){nearbyVacancies[oi]=lab_siren::Vacancy{};nearbyVacancyGeneration[oi]=o.generation;}
  std::array<lab_siren::VacancyEntry,7> entries{};
  for(unsigned j=0;j<7;++j){const auto i=indices[j];if(i==-1)continue;if(i<0||static_cast<uint32_t>(i)>=count||!state.nativeCandidates[i].observed)return;
   const auto& l=lights[i];const auto* privateOwner=NearbyOwnerForKey(static_cast<uint32_t>(l.mCastShadows));
   entries[j]={static_cast<uint32_t>(l.mCastShadows),privateOwner?privateOwner->generation:((l.mFlags&0x100u)?0:LampGeometry(l)),state.nativeCandidates[i].ownBeam||(l.mFlags&0x400u)!=0||(o.rank==1&&!privateOwner&&!(l.mFlags&0x100u)&&labRelevant[i])};
  }
  float intensity{};memcpy(&intensity,&o.inputBits,4);const bool valid=NearbyValid(o);
  const int drop=nearbyVacancies[oi].Filter(state.frame,GetTickCount(),valid,o.rank<2&&o.eligible&&proofRequested.load()&&policePolicy.load()==1&&vacancyRequested.load(),valid&&o.eligible,o.inputFrame+1==state.frame,intensity,NearbyKey(o.token),entries);
  if(drop<0)continue;const auto removed=indices[drop];indices[drop]=-1;auto* pending=reinterpret_cast<int32_t*>(regs.esp+0x28);if(*pending==removed){*pending=-1;regs.esi=UINT32_MAX;}++vacancySuppressed;
 }
}
void ObserveNearbyResult() noexcept {
 unsigned selected=0;for(unsigned i=0;i<7;++i){const auto record=game+fusionfix::shadows::ce::allocation::DynamicPassFirstPositionRva+i*0x110;
  if(!*reinterpret_cast<const uint8_t*>(record+0x25))continue;rage::CLightSource l{};l.mType=rage::LT_POINT;l.mCastShadows=*reinterpret_cast<const uint32_t*>(record+0x30);if(NearbyPoint(l))++selected;
 }if(hotCountersEnabled)nearbySelected+=selected;auto old=nearbyMaxSelected.load();while(selected>old&&!nearbyMaxSelected.compare_exchange_weak(old,selected)){}
}
void ObserveNearbyRender(uint32_t key,uint32_t slot,uint32_t,bool active) noexcept {
 if(!active||slot<1||slot>7)return;rage::CLightSource l{};l.mType=rage::LT_POINT;l.mCastShadows=key;if(NearbyPoint(l))experiment_perf::Count(hotCountersEnabled,nearbyRendered);
}
