// Included inside ModelEffectObserver, before ValidateSource46 extraction.
// Diagnostics and authored corona-only metadata retain the existing path.
bool pointFusion50=true;
// Research comparison switch; no startup option or live toggle. The fixture
// compares both paths with identical owner, descriptor and emitted inputs.
bool eagerGroup53=true;
std::atomic<uint32_t> ownedFusedAttempts79{0},ownedFusedComplete79{0},ownedFusedLegacy79{0},ownedFusedKeys79{0};
enum class PointFusion50Result {Complete,Fallback};
PointFusion50Result FusePoint50(SafetyHookContext& regs,const Invocation& c,Sample& sample,
 uint32_t site,const std::array<uint32_t,18>& arguments,const PointEvidence& evidence,
 std::array<uint32_t,32>* owned79=nullptr) noexcept {
 TrafficSignal::OwnerResult49 staged{};bool fault=false;
 if(!TrafficSignal::NativeWord49(game+0xebd0e8,staged.pools[0])||
    !TrafficSignal::NativeWord49(game+0x1232c60,staged.pools[1])){local.state.Invalidate();++readRejected;return PointFusion50Result::Complete;}
 std::array<uint8_t,0x30> owner{};uint8_t reference{};
 std::array<uint8_t,0x40> model{};std::array<float,16> matrix{};
 const traffic_signal::fresh_batch30::Span first[]{
  {staged.pools[0],&staged.headers[0],sizeof(staged.headers[0])},{staged.pools[1],&staged.headers[1],sizeof(staged.headers[1])},
  {c.descriptor,sample.descriptor.data(),sample.descriptor.size()},
  {arguments[5],sample.position.data(),sizeof(sample.position)},{arguments[6],sample.color.data(),sizeof(sample.color)},
  {arguments[3],sample.direction.data(),sizeof(sample.direction)},{arguments[4],sample.up.data(),sizeof(sample.up)},
  {c.owner,owner.data(),owner.size()},{c.modelInfo,model.data(),model.size()}};
 // Vectors are consumed once; retain the same eight permission slots for
 // repeatedly read owner/model/descriptor pages, without retaining data values.
 // Speculative copied bytes are not authority. Pool membership, slot,
 // owner model index and current model table binding below must agree before
 // using them. Final complete evidence and tail remain unchanged.
 if(!traffic_signal::fresh_group50::Copy(first,eagerGroup53?9u:7u,TrafficSignal::ReadableOriginal,QueryWorkingSetEx,&fault,eagerGroup53?0x187u:0x7u)){
  if(!fault)return PointFusion50Result::Fallback;
  local.state.Invalidate();++readRejected;return PointFusion50Result::Complete;
 }
 if(CoronaLight20::requested&&sample.descriptor[0x5b]==0)return PointFusion50Result::Fallback;
 sample.descriptorCopied=true;sample.packetCopied=true;
 std::memcpy(&sample.intensity,&arguments[7],4);std::memcpy(&sample.radius,&arguments[10],4);
 std::memcpy(&sample.innerCone,&arguments[11],4);std::memcpy(&sample.outerCone,&arguments[12],4);
 auto contains=[&](unsigned kind) noexcept {
  const auto& h=staged.headers[kind];const int stride=kind?0x320:0x70;const auto storage=uintptr_t(h.m_aStorage);
  return h.m_nSize>0&&h.m_nSize<=65536&&h.m_nStorageSize==stride&&c.owner>=storage&&
   uint64_t(c.owner-storage)<uint64_t(h.m_nSize)*unsigned(stride)&&(c.owner-storage)%unsigned(stride)==0;
 };
 const bool b=contains(0),o=contains(1);
 if(b==o)return !b&&(TrafficSignal::ownerClass109Active||TrafficSignal::ownerClass114Active)?PointFusion50Result::Fallback:PointFusion50Result::Complete;
 const unsigned kind=o?1:0;const auto& h=staged.headers[kind];const auto slot=unsigned((c.owner-uintptr_t(h.m_aStorage))/unsigned(h.m_nStorageSize));
 const traffic_signal::fresh_batch30::Span body[]{{c.owner,owner.data(),owner.size()},{uintptr_t(h.m_aFlags)+slot,&reference,1}};
 if(!eagerGroup53&&(!traffic_signal::fresh_group50::Copy(body,2,TrafficSignal::ReadableOriginal,QueryWorkingSetEx,&fault)||(reference&0x80)))return PointFusion50Result::Complete;
 std::memcpy(&staged.modelIndex,owner.data()+0x2e,2);std::memcpy(&staged.matrixPointer,owner.data()+0x20,4);
 if(staged.modelIndex<0||staged.modelIndex>=31000)return PointFusion50Result::Complete;
 uintptr_t info{};if(!TrafficSignal::NativeWord49(game+0xe95cd8+staged.modelIndex*4,info))return PointFusion50Result::Complete;
 const traffic_signal::fresh_batch30::Span details[]{{info,model.data(),model.size()},{staged.matrixPointer,matrix.data(),sizeof(matrix)}};
 if(eagerGroup53){
  if(info!=c.modelInfo){local.state.Invalidate();++contextRejected;return PointFusion50Result::Complete;}
  const traffic_signal::fresh_batch30::Span dependencies[]{{uintptr_t(h.m_aFlags)+slot,&reference,1},{staged.matrixPointer,matrix.data(),sizeof(matrix)}};
  if(!traffic_signal::fresh_group50::Copy(dependencies,2,TrafficSignal::ReadableOriginal,QueryWorkingSetEx,&fault)||(reference&0x80))return PointFusion50Result::Complete;
 }else if(!traffic_signal::fresh_group50::Copy(details,2,TrafficSignal::ReadableOriginal,QueryWorkingSetEx,&fault))return PointFusion50Result::Complete;
 uint32_t hash{};std::memcpy(&hash,model.data()+0x3c,4);if(!hash)return PointFusion50Result::Complete;
 for(unsigned j=0;j<3;++j)if(!std::isfinite(matrix[12+j]))return PointFusion50Result::Complete;
 staged.match.identity={uint32_t(staged.pools[kind]),uint32_t(uintptr_t(h.m_aStorage)),uint32_t(uintptr_t(h.m_aFlags)),c.owner,slot,reference,hash};
 staged.match.kind=kind;staged.match.modelInfo=uint32_t(info);
 for(unsigned j=0;j<3;++j)staged.match.entity[j]=matrix[12+j];
 sample.ownerMatched=true;sample.identity=staged.match.identity;sample.ownerKind=kind;sample.modelInfoMatched=info==c.modelInfo;
 if(!sample.modelInfoMatched){local.state.Invalidate();++contextRejected;return PointFusion50Result::Complete;}
 bool finite=std::isfinite(sample.radius)&&sample.radius>0&&std::isfinite(sample.intensity)&&sample.intensity>=0;
 for(unsigned j=0;j<3;++j)finite=finite&&std::isfinite(sample.position[j])&&std::isfinite(sample.color[j])&&std::isfinite(sample.direction[j])&&std::isfinite(sample.up[j]);
 if(evidence.type==2)finite=finite&&std::isfinite(sample.innerCone)&&std::isfinite(sample.outerCone);
 (void)ModelLightShadow20::PublishFreshPoint50(regs,c,staged,sample.descriptor,site,arguments,finite,owned79);
 return PointFusion50Result::Complete;
}
