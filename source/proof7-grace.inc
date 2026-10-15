lab_siren::BlinkGrace proofBlinkGrace;
std::atomic<uint32_t> proofGraceComparisons{0};
bool ProofLifetimeValid() noexcept {
 if(!proofArmed.load()||proofRetired.load()||!proofBound||!CPlayer::findPlayerCar||CPlayer::findPlayerCar()!=proofOwner.load())return false;
 const auto pool=CVehicle::GetVehiclePool();
 return pool&&reinterpret_cast<uintptr_t>(pool)==proofPool&&pool->m_aFlags&&pool->m_aStorage&&
   proofSlot<static_cast<uint32_t>(pool->m_nSize)&&!pool->GetIsFree(proofSlot)&&pool->GetReference(proofSlot)==proofReference;
}
void BeginProofGrace() noexcept {
 proofBlinkGrace.Begin(ProofLifetimeValid(),{proofOwner.load(),proofPool,proofSlot,proofReference},*CShadows::pFrameCounter,GetTickCount());
}
int CompareProofGrace(int original,int unchanged,int challenger,int incumbent) noexcept {
 using namespace PlayerShadowAllocation;
 const auto* lights=CurrentLights();
 if(!ProofLifetimeValid()||!lights)return unchanged;
 const bool ap=IsProofPoint(lights[challenger]),bp=IsProofPoint(lights[incumbent]);
 if(ap==bp)return unchanged;
 const auto pi=ap?challenger:incumbent,oi=ap?incumbent:challenger;
 const auto& other=lights[oi];
 // Only a known temporary ordinary lamp may yield its comparison claim.
 if(other.mFlags&0x100u)return unchanged;
 const auto& pc=state.nativeCandidates[pi];const auto& oc=state.nativeCandidates[oi];
 if(!proofBlinkGrace.Reclaim(ProofKey(),pc.observed&&labRelevant[pi],static_cast<uint32_t>(other.mCastShadows),LampGeometry(other),oc.claim))return unchanged;
 auto a=state.nativeCandidates[challenger],b=state.nativeCandidates[incumbent];
 (ap?a:b).claim=proofBlinkGrace.Ordinal();
 const auto result=fusionfix::shadows::NativeShadowContinuity42::Compare(original,a,b);
 if(result!=unchanged)++proofGraceComparisons;
 return result;
}
void FinishProofGrace(bool present,int selectedSlot) noexcept {
 using namespace PlayerShadowAllocation;
 uint32_t temporary=0;uint64_t generation=0;
 const auto ordinal=proofBlinkGrace.Ordinal();const auto count=CurrentCount();const auto* lights=CurrentLights();
 if(ordinal<7&&lights&&count<=4096){
  const auto record=game+fusionfix::shadows::ce::allocation::DynamicPassFirstPositionRva+ordinal*0x110;
  if(*reinterpret_cast<const uint8_t*>(record+0x25)){
   const auto key=*reinterpret_cast<const uint32_t*>(record+0x30);
   for(uint32_t i=0;i<count;++i)if(static_cast<uint32_t>(lights[i].mCastShadows)==key&&!(lights[i].mFlags&0x100u)){
    temporary=key;generation=LampGeometry(lights[i]);break;
   }
  }
 }
 if(!ProofLifetimeValid())proofBlinkGrace.Reset();
 else proofBlinkGrace.Finish(ProofKey(),present,selectedSlot,temporary,generation,state.committed);
}
