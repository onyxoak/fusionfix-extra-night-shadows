// One occupied-vehicle lifetime only. Never rebind or recycle a shadow key.
alignas(64) uint8_t proofIdentity[64]{};
std::atomic<bool> proofRequested{false},proofArmed{false},proofRetired{false};
std::atomic<uint32_t> proofOwner{0},proofFrame{0},proofPromoted{0};
std::atomic<uint32_t> proofSelected{0},proofFrames{0},proofMaxSlots{0};
uint32_t proofSlot=0,proofReference=0,proofPool=0;
bool proofBound=false;
uint32_t ProofKey() noexcept {return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(proofIdentity));}
bool IsProofPoint(const rage::CLightSource& l) noexcept {return l.mType==0&&static_cast<uint32_t>(l.mCastShadows)==ProofKey();}
void RetireProof() noexcept {proofArmed=false;proofRetired=true;}
void UpdateProof(){
 static uint32_t checked=0;const auto now=GetTickCount();
 if(now-checked>=500){checked=now;const auto ini=pe::ModulePath(self).replace_extension(L".ini");proofRequested=GetPrivateProfileIntW(L"LAB",L"PoliceProof",0,ini.c_str())==1;}
 if(proofRetired.load())return;
 const auto owner=CPlayer::findPlayerCar?CPlayer::findPlayerCar():0;
 const auto pool=CVehicle::GetVehiclePool();
 if(!owner||!pool||!pool->m_aStorage||!pool->m_aFlags||pool->m_nSize<=0||pool->m_nSize>4096||pool->m_nStorageSize<0x24||pool->m_nStorageSize>0x10000){if(proofBound)RetireProof();return;}
 const auto base=reinterpret_cast<uintptr_t>(pool->m_aStorage);
 if(owner<base||uint64_t(owner-base)>=uint64_t(pool->m_nSize)*pool->m_nStorageSize||(owner-base)%pool->m_nStorageSize){if(proofBound)RetireProof();return;}
 const auto slot=static_cast<uint32_t>((owner-base)/pool->m_nStorageSize);const auto reference=pool->GetReference(slot);
 if(pool->GetIsFree(slot)){if(proofBound)RetireProof();return;}
 if(proofBound&&(proofOwner.load()!=owner||proofSlot!=slot||proofReference!=reference||proofPool!=reinterpret_cast<uintptr_t>(pool))){RetireProof();Event(1,"PROOF_RETIRED","vehicle_lifetime_changed restart_required");return;}
 if(!proofBound){if(!proofRequested.load())return;proofSlot=slot;proofReference=reference;proofPool=static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pool));proofOwner=static_cast<uint32_t>(owner);proofBound=true;Event(1,"PROOF_BOUND","one_vehicle_lifetime no_key_reuse");}
 proofFrame=*CShadows::pFrameCounter;proofArmed=proofRequested.load();
}
void PromoteProof(SafetyHookContext& regs,uint32_t caller) noexcept {
 if(caller!=0x640d0a||!proofArmed.load()||proofRetired.load()||regs.edi!=proofOwner.load())return;
 auto* args=reinterpret_cast<uint32_t*>(regs.esp);
 if(args[2]!=0||args[3]!=0x110||args[16]!=0)return;
 const uint32_t frame=*CShadows::pFrameCounter;
 if(frame-proofFrame.load()>2)return;
 static thread_local uint32_t lastFrame=UINT32_MAX;
 if(lastFrame==frame)return;
 const auto pool=CVehicle::GetVehiclePool();
 if(reinterpret_cast<uintptr_t>(pool)!=proofPool||pool->GetIsFree(proofSlot)||pool->GetReference(proofSlot)!=proofReference){RetireProof();return;}
 // The opaque key has reserved storage for the whole DLL lifetime; never a bool.
 // Only this verified producer's call arguments are changed. Native 7-slot budget remains intact.
 lastFrame=frame;args[3]|=4u;args[16]=ProofKey();++proofPromoted;
}
void ObserveProofResult() noexcept {
 if(!proofOwner.load())return;
 unsigned active=0;bool selected=false;
 for(unsigned i=0;i<7;++i){const auto record=game+fusionfix::shadows::ce::allocation::DynamicPassFirstPositionRva+i*0x110;
  if(*reinterpret_cast<const uint8_t*>(record+0x25)){++active;if(*reinterpret_cast<const uint32_t*>(record+0x30)==ProofKey())selected=true;}}
 ++proofFrames;if(selected)++proofSelected;auto old=proofMaxSlots.load();while(active>old&&!proofMaxSlots.compare_exchange_weak(old,active)){}
}
