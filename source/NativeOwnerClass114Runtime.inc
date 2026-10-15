// Included inside TrafficSignal after OwnerCopy35. No old pool hot-path change.
bool ownerClass114Authenticated=false;std::atomic<bool> ownerClass114Active{false};
std::atomic<uint32_t> ownerClass114Attempts{0},ownerClass114Matched{0};
bool MatchOwnerClass114(uintptr_t owner,Match& result) noexcept {
 if(!ownerClass114Active)return false;
#ifdef CE_OWNER_CLASS114_ENTER
 CE_OWNER_CLASS114_ENTER();
#endif
 ++ownerClass114Attempts;
 constexpr uintptr_t roots[]{0xebd0e8,0x1232c60,0x126d9ec,native_owner114::PoolRva};
 uintptr_t pools[4]{};rage::VehiclePool headers[4]{},again[4]{};
 const traffic_signal::fresh_batch30::Span rootSpans[]{
  {game+roots[0],&pools[0],4},{game+roots[1],&pools[1],4},{game+roots[2],&pools[2],4},{game+roots[3],&pools[3],4}};
 if(!OwnerCopy35(rootSpans,4))return false;
 const traffic_signal::fresh_batch30::Span headerSpans[]{
  {pools[0],&headers[0],sizeof(headers[0])},{pools[1],&headers[1],sizeof(headers[1])},{pools[2],&headers[2],sizeof(headers[2])},{pools[3],&headers[3],sizeof(headers[3])}};
 if(!OwnerCopy35(headerSpans,4))return false;
 const auto& h=headers[3];const auto storage=reinterpret_cast<uintptr_t>(h.m_aStorage);
 // Outside all three existing pools is required; overlapping ranges are ambiguous.
 for(unsigned k=0;k<3;++k){
  const auto& peer=headers[k];const auto peerStorage=reinterpret_cast<uintptr_t>(peer.m_aStorage);
  const auto stride=int32_t(k==0?0x70:k==1?0x320:0x80);
  if(peer.m_nStorageSize!=stride||peer.m_nSize<=0||peer.m_nSize>65536||peerStorage<0x10000||
     uint64_t(peerStorage)+uint64_t(uint32_t(peer.m_nSize))*uint32_t(stride)>uint64_t(UINT32_MAX)+1)return false;
  if(owner>=peerStorage&&uint64_t(owner-peerStorage)<uint64_t(uint32_t(peer.m_nSize))*uint32_t(stride))return false;
 }
 if(h.m_nSize>500||h.m_nStorageSize!=int32_t(native_owner114::Stride)||!native_owner109::Range(owner,storage,h.m_nSize,h.m_nStorageSize))return false;
 const auto slot=uint32_t((owner-storage)/native_owner114::Stride);
 const auto flags=reinterpret_cast<uintptr_t>(h.m_aFlags);
 if(flags<0x10000||uint64_t(flags)+uint32_t(h.m_nSize)>uint64_t(UINT32_MAX)+1)return false;
 const auto flagAddress=flags+slot;
 std::array<uint8_t,0x30> body{},tail{};uint8_t reference{},referenceAgain{};
 const traffic_signal::fresh_batch30::Span ownerSpans[]{{owner,body.data(),body.size()},{flagAddress,&reference,1}};
 if(!OwnerCopy35(ownerSpans,2)||(reference&0x80)||!native_owner114::Class(body,game))return false;
 int16_t index{};uintptr_t matrix{},info{};
 std::memcpy(&index,body.data()+0x2e,2);std::memcpy(&matrix,body.data()+0x20,4);
 if(index<0||index>=31000)return false;
 const traffic_signal::fresh_batch30::Span tableSpan{game+0xe95cd8+index*4,&info,4};
 if(!OwnerCopy35(&tableSpan,1)||info<0x10000||info>UINT32_MAX-0x40)return false;
 std::array<uint8_t,0x40> model{};std::array<float,16> pose{},poseAgain{};
 // The native entity position getter uses +20 matrix translation, else +10
 // inline translation/heading. This is OWNER pose, never the emitter matrix.
 const uintptr_t poseAddress=matrix?matrix:owner+0x10;
 const size_t poseSize=matrix?64:16;
 const traffic_signal::fresh_batch30::Span detailSpans[]{{info,model.data(),model.size()},{poseAddress,pose.data(),poseSize}};
 if(!OwnerCopy35(detailSpans,2))return false;
 uint32_t hash{};std::memcpy(&hash,model.data()+0x3c,4);if(!hash)return false;
 const unsigned offset=matrix?12:0;
 for(unsigned j=0;j<3;++j)if(!std::isfinite(pose[offset+j]))return false;
 uintptr_t rootsAgain[4]{},infoAgain{};uint32_t hashAgain{};
 const traffic_signal::fresh_batch30::Span finalSpans[]{
  {game+roots[0],&rootsAgain[0],4},{game+roots[1],&rootsAgain[1],4},{game+roots[2],&rootsAgain[2],4},{game+roots[3],&rootsAgain[3],4},
  {pools[0],&again[0],sizeof(again[0])},{pools[1],&again[1],sizeof(again[1])},{pools[2],&again[2],sizeof(again[2])},{pools[3],&again[3],sizeof(again[3])},
  {owner,tail.data(),tail.size()},{flagAddress,&referenceAgain,1},
  {game+0xe95cd8+index*4,&infoAgain,4},{info+0x3c,&hashAgain,4},{poseAddress,poseAgain.data(),poseSize}};
 if(!OwnerCopy35(finalSpans,4)||!traffic_signal::fresh_group50::Copy(finalSpans+4,9,ReadableOriginal,CE_OWNER_BATCH_QUERY35,nullptr,UINT32_MAX)||
    !OwnerCopy35(finalSpans,4)||!ownerClass114Active)return false;
 if(referenceAgain!=reference||(referenceAgain&0x80)||infoAgain!=info||hashAgain!=hash||
    !native_owner114::Class(tail,game)||std::memcmp(body.data()+0x20,tail.data()+0x20,4)||
    std::memcmp(body.data()+0x2e,tail.data()+0x2e,2)||std::memcmp(pose.data(),poseAgain.data(),poseSize))return false;
 if(!matrix&&std::memcmp(tail.data()+0x10,poseAgain.data(),16))return false;
 for(unsigned k=0;k<4;++k){const auto& a=headers[k];const auto& b=again[k];
  if(rootsAgain[k]!=pools[k]||a.m_aStorage!=b.m_aStorage||a.m_aFlags!=b.m_aFlags||a.m_nSize!=b.m_nSize||a.m_nStorageSize!=b.m_nStorageSize)return false;
 }
 Match published{};published.identity={uint32_t(pools[3]),uint32_t(storage),uint32_t(reinterpret_cast<uintptr_t>(h.m_aFlags)),uint32_t(owner),slot,reference,hash};
 published.kind=3;published.modelInfo=uint32_t(info);
 std::memcpy(published.ownerPose109.data(),&matrix,4);
 std::memcpy(published.ownerPose109.data()+4,pose.data(),poseSize);
 for(unsigned j=0;j<3;++j)published.entity[j]=pose[offset+j];
 result=published;++ownerClass114Matched;return true;
}
