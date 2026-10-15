// Included inside TrafficSignal. Every phase queries afresh; local staged
// values are hints until the final current pool/owner/slot consistency check.
struct OwnerRequest49 {uintptr_t owner{},descriptor{};};
struct OwnerResult49 {Match match{};std::array<uint8_t,0x6c> bytes{};rage::VehiclePool headers[2]{};uintptr_t pools[2]{},matrixPointer{};int16_t modelIndex{};bool matched{},serialFallback{},faulted{};};
#include "NativeOwnerBatch115Runtime.inc"
__declspec(noinline) bool NativeWord49(uintptr_t address,uintptr_t& out) noexcept {
 __try {out=*reinterpret_cast<const volatile uintptr_t*>(address);return true;}
 __except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION||GetExceptionCode()==EXCEPTION_IN_PAGE_ERROR||
 GetExceptionCode()==EXCEPTION_GUARD_PAGE?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){return false;}
}
void MatchOwnerWindow49(const OwnerRequest49* requests,OwnerResult49* results,unsigned count) noexcept {
 using namespace traffic_signal::fresh_window49;
 if(!requests||!results||count>Groups)return;
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::OwnerMatch,128);
 const immediate_region33::Scope operation;
 struct Row {
  uintptr_t pools[2]{},info{},matrix{};rage::VehiclePool headers[2]{},again[2]{};
  std::array<uint8_t,0x30> owner{},ownerAgain{};std::array<uint8_t,0x40> model{};
  std::array<float,16> transform{};uint8_t reference{},referenceAgain{};
  unsigned kind{},slot{};int16_t index{};bool ready{};
 } rows[Groups]{};
 Span spans[Groups][Spans]{};Group groups[Groups]{};
 auto headerValid=[](const rage::VehiclePool& h,unsigned k) noexcept {
  return h.m_nSize>0&&h.m_nSize<=65536&&h.m_nStorageSize==(k?0x320:0x70);
 };
 auto contains=[&](const rage::VehiclePool& h,unsigned k,uintptr_t owner) noexcept {
  const auto storage=reinterpret_cast<uintptr_t>(h.m_aStorage);const unsigned stride=k?0x320:0x70;
  return headerValid(h,k)&&owner>=storage&&uint64_t(owner-storage)<uint64_t(h.m_nSize)*stride&&(owner-storage)%stride==0;
 };
 for(unsigned i=0;i<count;++i){results[i]={};auto& r=rows[i];if(!requests[i].owner)continue;
  if(!NativeWord49(game+0xebd0e8,r.pools[0])||!NativeWord49(game+0x1232c60,r.pools[1])){results[i].faulted=true;continue;}
  spans[i][0]={r.pools[0],&r.headers[0],sizeof(r.headers[0])};spans[i][1]={r.pools[1],&r.headers[1],sizeof(r.headers[1])};groups[i]={spans[i],2};
 }
 auto phase=Copy(groups,count,ReadableOriginal,CE_OWNER_BATCH_QUERY35);
 for(unsigned i=0;i<count;++i){auto& r=rows[i];groups[i]={};
  if(!phase.copied[i]){results[i].faulted|=phase.faulted[i];results[i].serialFallback=!results[i].faulted;continue;}
  const bool b=contains(r.headers[0],0,requests[i].owner),o=contains(r.headers[1],1,requests[i].owner);
  if(b==o){results[i].serialFallback=!b&&(ownerClass109Active||ownerClass114Active);continue;}r.kind=o?1u:0u;const auto& h=r.headers[r.kind];
  r.slot=unsigned((requests[i].owner-reinterpret_cast<uintptr_t>(h.m_aStorage))/unsigned(h.m_nStorageSize));
  spans[i][0]={requests[i].owner,r.owner.data(),r.owner.size()};spans[i][1]={reinterpret_cast<uintptr_t>(h.m_aFlags)+r.slot,&r.reference,1};groups[i]={spans[i],2};
 }
 phase=Copy(groups,count,ReadableOriginal,CE_OWNER_BATCH_QUERY35);
 for(unsigned i=0;i<count;++i){auto& r=rows[i];groups[i]={};
  if(!phase.copied[i]){results[i].faulted|=phase.faulted[i];continue;}
  if(r.reference&0x80)continue;std::memcpy(&r.index,r.owner.data()+0x2e,2);std::memcpy(&r.matrix,r.owner.data()+0x20,4);
  if(r.index<0||r.index>=31000)continue;
  if(!NativeWord49(game+0xe95cd8+r.index*4,r.info)){results[i].faulted=true;continue;}
  spans[i][0]={r.info,r.model.data(),r.model.size()};spans[i][1]={r.matrix,r.transform.data(),sizeof(r.transform)};
  spans[i][2]={requests[i].descriptor,results[i].bytes.data(),results[i].bytes.size()};groups[i]={spans[i],3};
 }
 phase=Copy(groups,count,ReadableOriginal,CE_OWNER_BATCH_QUERY35);
 for(unsigned i=0;i<count;++i){auto& r=rows[i];groups[i]={};
  if(!phase.copied[i]){results[i].faulted|=phase.faulted[i];continue;}
  uint32_t model{};std::memcpy(&model,r.model.data()+0x3c,4);if(!model)continue;
  bool finite=true;for(unsigned j=0;j<3;++j)finite&=std::isfinite(r.transform[12+j]);if(!finite)continue;
  const auto& h=r.headers[r.kind];results[i].match.identity={uint32_t(r.pools[r.kind]),uint32_t(reinterpret_cast<uintptr_t>(h.m_aStorage)),
   uint32_t(reinterpret_cast<uintptr_t>(h.m_aFlags)),uint32_t(requests[i].owner),r.slot,r.reference,model};
  results[i].match.kind=r.kind;results[i].match.modelInfo=uint32_t(r.info);results[i].modelIndex=r.index;results[i].matrixPointer=r.matrix;
  for(unsigned k=0;k<2;++k){results[i].headers[k]=r.headers[k];results[i].pools[k]=r.pools[k];}
  for(unsigned j=0;j<3;++j)results[i].match.entity[j]=r.transform[12+j];
  spans[i][0]={r.pools[0],&r.again[0],sizeof(r.again[0])};spans[i][1]={r.pools[1],&r.again[1],sizeof(r.again[1])};
  spans[i][2]={requests[i].owner,r.ownerAgain.data(),r.ownerAgain.size()};spans[i][3]={reinterpret_cast<uintptr_t>(h.m_aFlags)+r.slot,&r.referenceAgain,1};groups[i]={spans[i],4};r.ready=true;
 }
 phase=Copy(groups,count,ReadableOriginal,CE_OWNER_BATCH_QUERY35);
 for(unsigned i=0;i<count;++i){auto& r=rows[i];if(!r.ready)continue;
  if(!phase.copied[i]){results[i].faulted|=phase.faulted[i];continue;}
  bool same=r.reference==r.referenceAgain&&!(r.referenceAgain&0x80);
  for(unsigned k=0;k<2;++k){uintptr_t current{};same&=NativeWord49(game+(k?0x1232c60:0xebd0e8),current)&&current==r.pools[k];
   const auto& a=r.headers[k];const auto& b=r.again[k];same&=a.m_aStorage==b.m_aStorage&&a.m_aFlags==b.m_aFlags&&a.m_nSize==b.m_nSize&&a.m_nStorageSize==b.m_nStorageSize;
  }
  int16_t index{};uintptr_t matrix{},info{};std::memcpy(&index,r.ownerAgain.data()+0x2e,2);std::memcpy(&matrix,r.ownerAgain.data()+0x20,4);
  same&=index==r.index&&matrix==r.matrix&&NativeWord49(game+0xe95cd8+r.index*4,info)&&info==r.info;
  results[i].matched=same;
 }
}

// A fresh full per-entry commit observation, not a permission certificate.
// Only addresses/sizes are exported, never staged source values or authority.
void OwnerCommitRanges54(const OwnerResult49& staged,uintptr_t descriptor,
 traffic_signal::fresh_window49::Span* spans,void* output) noexcept {
 const auto& id=staged.match.identity;
 spans[0]={staged.pools[0],output,sizeof(rage::VehiclePool)};spans[1]={staged.pools[1],output,sizeof(rage::VehiclePool)};
 spans[2]={id.owner,output,0x30};spans[3]={uintptr_t(id.flags)+id.slot,output,1};
 spans[4]={uintptr_t(staged.match.modelInfo)+0x3c,output,4};spans[5]={staged.matrixPointer,output,64};spans[6]={descriptor,output,0x6c};
}
// Addresses learned in earlier phases are reread and all dependencies rebound
// to fresh owner/header/table words before any registry authorization.
bool CommitOwnerWindow49(const OwnerResult49& staged,uintptr_t descriptor,
 Match& current,std::array<uint8_t,0x6c>& bytes,bool& faulted,
 traffic_signal::fresh_certificate54::Certificate* certificate=nullptr,unsigned certificateGroup=0) noexcept {
 using namespace traffic_signal::fresh_window49;
 rage::VehiclePool headers[2]{};std::array<uint8_t,0x30> owner{};
 std::array<float,16> matrix{};uint8_t reference{};uint32_t model{};
 const auto& id=staged.match.identity;
 const Span spans[]{
  {staged.pools[0],&headers[0],sizeof(headers[0])},{staged.pools[1],&headers[1],sizeof(headers[1])},
  {id.owner,owner.data(),owner.size()},{uintptr_t(id.flags)+id.slot,&reference,1},
  {uintptr_t(staged.match.modelInfo)+0x3c,&model,sizeof(model)},
  {staged.matrixPointer,matrix.data(),sizeof(matrix)},{descriptor,bytes.data(),bytes.size()}};
 bool copied=false;faulted=false;
 const bool handled=certificate&&traffic_signal::fresh_certificate54::TryCopy(*certificate,certificateGroup,spans,7,copied,faulted);
 if(!handled){const Group group{spans,7};const auto result=Copy(&group,1,ReadableOriginal,CE_OWNER_BATCH_QUERY35);faulted=result.faulted[0];copied=result.copied[0];}
 if(!copied||(reference&0x80)||!model)return false;
 for(unsigned k=0;k<2;++k){uintptr_t pool{};
  if(!NativeWord49(game+(k?0x1232c60:0xebd0e8),pool)){faulted=true;return false;}
  const auto& a=staged.headers[k];const auto& b=headers[k];
  if(pool!=staged.pools[k]||a.m_aStorage!=b.m_aStorage||a.m_aFlags!=b.m_aFlags||a.m_nSize!=b.m_nSize||a.m_nStorageSize!=b.m_nStorageSize)return false;
 }
 int16_t index{};uintptr_t pointer{},info{};std::memcpy(&index,owner.data()+0x2e,2);std::memcpy(&pointer,owner.data()+0x20,4);
 if(index!=staged.modelIndex||pointer!=staged.matrixPointer)return false;
 if(!NativeWord49(game+0xe95cd8+index*4,info)){faulted=true;return false;}
 if(info!=staged.match.modelInfo)return false;
 current=staged.match;current.identity.reference=reference;current.identity.model=model;
 for(unsigned j=0;j<3;++j){if(!std::isfinite(matrix[12+j]))return false;current.entity[j]=matrix[12+j];}
 return true;
}

#include "ModelOwnerScratch59Runtime.inc"
