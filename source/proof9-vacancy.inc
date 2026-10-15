lab_siren::Vacancy nineVacancy;
SafetyHookMid vacancyHook;
std::atomic<bool> vacancyRequested{false};
std::atomic<uint32_t> vacancyInputFrame{0},vacancyInputBits{0},vacancySuppressed{0};
void RememberVacancyInput(const uint32_t* args) noexcept {vacancyInputBits=args[8];vacancyInputFrame.store(*CShadows::pFrameCounter,std::memory_order_release);}
void ApplyVacancy(SafetyHookContext& regs) noexcept {
 const fusionfix::shadows::FloatingPointState fp;using namespace PlayerShadowAllocation;
 if(nearbyPoliceEnabled){ApplyNearbyVacancy(regs);return;}
 if(!Enabled()||state.depth!=1||!state.pass.Active()||!state.continuityActive||!state.stackAnchor||regs.esp!=state.stackAnchor||!CShadows::pFrameCounter||*CShadows::pFrameCounter!=state.frame)return;
 const auto* lights=CurrentLights();const auto count=CurrentCount();if(!lights||count>4096)return;
 auto* indices=reinterpret_cast<int32_t*>(regs.esp+allocation::SelectedIndicesStackOffset);
 std::array<lab_siren::VacancyEntry,7> identities{};
 for(unsigned j=0;j<7;++j){const auto i=indices[j];if(i==-1)continue;if(i<0||static_cast<uint32_t>(i)>=count||!state.nativeCandidates[i].observed)return;
  const auto& l=lights[i];identities[j]={static_cast<uint32_t>(l.mCastShadows),IsProofPoint(l)?LampGeometry(l):((l.mFlags&0x100u)?0:LampGeometry(l)),state.nativeCandidates[i].ownBeam||(l.mFlags&0x400u)!=0};
 }
 const auto inputFrame=vacancyInputFrame.load(std::memory_order_acquire);const auto bits=vacancyInputBits.load();float intensity;memcpy(&intensity,&bits,4);
 const int drop=nineVacancy.Filter(state.frame,GetTickCount(),BoundPoliceOwnerValid(),proofArmed.load()&&policePolicy.load()==1&&vacancyRequested.load(),BoundSirenOn(),inputFrame+1==state.frame,intensity,ProofKey(),identities);
 if(drop<0)return;
 const int removed=indices[drop];indices[drop]=-1;
 // The cache candidate is not necessarily in the selected set. Only cancel it if it IS the withdrawn borrower.
 auto* pending=reinterpret_cast<int32_t*>(regs.esp+0x28);if(*pending==removed){*pending=-1;regs.esi=UINT32_MAX;}
 ++vacancySuppressed;
}

