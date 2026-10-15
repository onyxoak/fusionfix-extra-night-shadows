// Callback-owned numeric/address hints. Every authorizing byte is reread under
// the individual model registry lock; no native pointer becomes cached authority.
namespace NativeOwnerBatch115 {
bool requested=false;
bool (*class114Capability)() noexcept=[]() noexcept {return ownerClass114Active.load(std::memory_order_acquire);};
constexpr unsigned Groups=32;
constexpr uintptr_t Roots[]{0xebd0e8,0x1232c60,native_owner109::PoolRva,0x12f7d60};
constexpr unsigned Strides[]{0x70,0x320,native_owner109::Stride,0x110};
constexpr uintptr_t Vtables[]{0,0,native_owner109::VtableRva,0xad46a4};
bool Active(unsigned roots) noexcept {
 return requested&&(roots==3?ownerClass109Active.load(std::memory_order_acquire):
  roots==4&&class114Capability&&class114Capability());
}
struct Hint {
 uintptr_t pools[4]{},info{},matrix{},owner{},descriptor{};
 rage::VehiclePool headers[4]{};
 std::array<uint8_t,0x30> body{};
 std::array<uint8_t,0x40> model{};
 std::array<float,16> pose{};
 uint8_t reference{};int16_t index{};unsigned roots{},slot{};
 bool ready{},faulted{};
};
struct Scratch {
 Hint rows[Groups]{};
 traffic_signal::fresh_native_window115::Span spans[Groups][16]{};
 traffic_signal::fresh_native_window115::Group groups[Groups]{};
 traffic_signal::fresh_native_certificate115::Certificate ownerCertificate{},descriptorCertificate{};
};
#ifndef CE_ALLOCATE_NATIVE_SCRATCH115
#define CE_ALLOCATE_NATIVE_SCRATCH115(size) HeapAlloc(GetProcessHeap(),0,size)
#endif
#ifndef CE_RELEASE_NATIVE_SCRATCH115
#define CE_RELEASE_NATIVE_SCRATCH115(pointer) HeapFree(GetProcessHeap(),0,pointer)
#endif
struct OwnedScratch {
 Scratch* value{};bool tried{};
 Scratch* Ensure() noexcept {
  if(value||tried)return value;tried=true;
  void* nativeMemory115{};{const traffic_signal::fresh_page::Preserve preserve;nativeMemory115=CE_ALLOCATE_NATIVE_SCRATCH115(sizeof(Scratch));}
  if(nativeMemory115)value=new(nativeMemory115) Scratch{};
  return value;
 }
 ~OwnedScratch() noexcept {
  if(!value)return;
  value->ownerCertificate.active=false;value->descriptorCertificate.active=false;value->~Scratch();
  const traffic_signal::fresh_page::Preserve preserve;(void)CE_RELEASE_NATIVE_SCRATCH115(value);
 }
 OwnedScratch()=default;OwnedScratch(const OwnedScratch&)=delete;OwnedScratch& operator=(const OwnedScratch&)=delete;
};
bool HeaderValid(const rage::VehiclePool& h,unsigned kind) noexcept {
 const auto storage=reinterpret_cast<uintptr_t>(h.m_aStorage);
 return h.m_nStorageSize==int32_t(Strides[kind])&&h.m_nSize>0&&h.m_nSize<=(kind==3?500:65536)&&storage>=0x10000&&
  uint64_t(storage)+uint64_t(uint32_t(h.m_nSize))*Strides[kind]<=uint64_t(UINT32_MAX)+1;
}
bool Outside(const rage::VehiclePool& h,uintptr_t owner) noexcept {
 const auto storage=reinterpret_cast<uintptr_t>(h.m_aStorage);
 return owner<storage||uint64_t(owner-storage)>=uint64_t(uint32_t(h.m_nSize))*uint32_t(h.m_nStorageSize);
}
bool Membership(const Hint& h) noexcept {
 if(!Active(h.roots))return false;
 const unsigned kind=h.roots-1;
 for(unsigned k=0;k<h.roots;++k)if(!HeaderValid(h.headers[k],k))return false;
 for(unsigned k=0;k<kind;++k)if(!Outside(h.headers[k],h.owner))return false;
 const auto& pool=h.headers[kind];const auto storage=reinterpret_cast<uintptr_t>(pool.m_aStorage);
 const auto flags=reinterpret_cast<uintptr_t>(pool.m_aFlags);
 return native_owner109::Range(h.owner,storage,pool.m_nSize,pool.m_nStorageSize)&&flags>=0x10000&&
  uint64_t(flags)+uint32_t(pool.m_nSize)<=uint64_t(UINT32_MAX)+1;
}
bool Class(const Hint& h) noexcept {
 uintptr_t table{};uint32_t kind{};std::memcpy(&table,h.body.data(),4);std::memcpy(&kind,h.body.data()+0x28,4);
 return table==game+Vtables[h.roots-1]&&((kind>>6)&15u)==(h.roots==3?5u:1u);
}
bool SameHeader(const rage::VehiclePool& a,const rage::VehiclePool& b) noexcept {
 return a.m_aStorage==b.m_aStorage&&a.m_aFlags==b.m_aFlags&&a.m_nSize==b.m_nSize&&a.m_nStorageSize==b.m_nStorageSize;
}
// Staging discovers addresses, never certifies a source. Old-pool results and
// faults are untouched. Unknown/changed classes retain the scalar matcher.
void Stage(const OwnerRequest49* requests,OwnerResult49* results,unsigned count,Scratch& scratch) noexcept {
 // Permission-only region cache ends at this read phase, before any caller restart.
 const immediate_region33::Scope regions202;
 using namespace traffic_signal::fresh_native_window115;
 if(!requested||count>Groups){for(auto& row:scratch.rows)row.ready=false;return;}
 for(unsigned i=0;i<Groups;++i){scratch.rows[i]={};scratch.groups[i]={};}
 bool any=false;
 for(unsigned i=0;i<count;++i)if(results[i].serialFallback&&!results[i].faulted&&requests[i].owner){
  auto& h=scratch.rows[i];h.owner=requests[i].owner;h.descriptor=requests[i].descriptor;h.roots=3;any=true;
  for(unsigned k=0;k<3;++k)scratch.spans[i][k]={game+Roots[k],&h.pools[k],4};scratch.groups[i]={scratch.spans[i],3};
 }
 if(!any)return;
 auto phase=Copy(scratch.groups,count,ReadableOriginal,CE_OWNER_BATCH_QUERY35);
 for(unsigned i=0;i<count;++i){auto& h=scratch.rows[i];scratch.groups[i]={};
  if(!phase.copied[i]){h.faulted=phase.faulted[i];continue;}
  for(unsigned k=0;k<3;++k)scratch.spans[i][k]={h.pools[k],&h.headers[k],sizeof(h.headers[k])};scratch.groups[i]={scratch.spans[i],3};
 }
 phase=Copy(scratch.groups,count,ReadableOriginal,CE_OWNER_BATCH_QUERY35);
 for(unsigned i=0;i<count;++i){auto& h=scratch.rows[i];scratch.groups[i]={};
  if(!phase.copied[i]){h.faulted|=phase.faulted[i];continue;}
  if(Membership(h))continue;
  if(Active(4)){h.roots=4;scratch.spans[i][0]={game+Roots[3],&h.pools[3],4};scratch.groups[i]={scratch.spans[i],1};}
 }
 phase=Copy(scratch.groups,count,ReadableOriginal,CE_OWNER_BATCH_QUERY35);
 for(unsigned i=0;i<count;++i){auto& h=scratch.rows[i];scratch.groups[i]={};
  if(h.roots!=4)continue;
  if(!phase.copied[i]){h.faulted|=phase.faulted[i];continue;}
  scratch.spans[i][0]={h.pools[3],&h.headers[3],sizeof(h.headers[3])};scratch.groups[i]={scratch.spans[i],1};
 }
 phase=Copy(scratch.groups,count,ReadableOriginal,CE_OWNER_BATCH_QUERY35);
 for(unsigned i=0;i<count;++i){auto& h=scratch.rows[i];scratch.groups[i]={};
  if(h.roots==4&&!phase.copied[i]){h.faulted|=phase.faulted[i];continue;}
  if(!h.owner||h.faulted||!Membership(h))continue;
  const auto& pool=h.headers[h.roots-1];h.slot=unsigned((h.owner-reinterpret_cast<uintptr_t>(pool.m_aStorage))/Strides[h.roots-1]);
  scratch.spans[i][0]={h.owner,h.body.data(),h.body.size()};scratch.spans[i][1]={reinterpret_cast<uintptr_t>(pool.m_aFlags)+h.slot,&h.reference,1};scratch.groups[i]={scratch.spans[i],2};
 }
 phase=Copy(scratch.groups,count,ReadableOriginal,CE_OWNER_BATCH_QUERY35);
 for(unsigned i=0;i<count;++i){auto& h=scratch.rows[i];scratch.groups[i]={};
  if(!phase.copied[i]){h.faulted|=phase.faulted[i];continue;}
  if((h.reference&0x80)||!Class(h))continue;
  std::memcpy(&h.index,h.body.data()+0x2e,2);std::memcpy(&h.matrix,h.body.data()+0x20,4);
  if(h.index<0||h.index>=31000)continue;
  scratch.spans[i][0]={game+0xe95cd8+h.index*4,&h.info,4};scratch.groups[i]={scratch.spans[i],1};
 }
 phase=Copy(scratch.groups,count,ReadableOriginal,CE_OWNER_BATCH_QUERY35);
 for(unsigned i=0;i<count;++i){auto& h=scratch.rows[i];scratch.groups[i]={};
  if(!phase.copied[i]){h.faulted|=phase.faulted[i];continue;}
  if(h.info<0x10000||h.info>UINT32_MAX-0x40)continue;
  scratch.spans[i][0]={h.info,h.model.data(),h.model.size()};
  scratch.spans[i][1]={h.matrix?h.matrix:h.owner+0x10,h.pose.data(),h.matrix?64u:16u};scratch.groups[i]={scratch.spans[i],2};
 }
 phase=Copy(scratch.groups,count,ReadableOriginal,CE_OWNER_BATCH_QUERY35);
 for(unsigned i=0;i<count;++i){auto& h=scratch.rows[i];
  if(!phase.copied[i]){h.faulted|=phase.faulted[i];}
  else {
   uint32_t hash{};std::memcpy(&hash,h.model.data()+0x3c,4);const unsigned offset=h.matrix?12u:0u;
   bool finite=true;for(unsigned j=0;j<3;++j)finite=finite&&std::isfinite(h.pose[offset+j]);
   if(hash&&finite&&Membership(h)&&Class(h)){
    h.ready=true;results[i].match.modelInfo=uint32_t(h.info);results[i].match.kind=h.roots-1;
    results[i].matched=true;results[i].serialFallback=false;
   }
  }
  if(h.faulted){results[i].faulted=true;results[i].serialFallback=false;h.ready=false;}
 }
}
unsigned OwnerSpans(const Hint& addresses,Hint& output,traffic_signal::fresh_native_window115::Span* spans) noexcept {
 unsigned n=0;for(unsigned k=0;k<addresses.roots;++k)spans[n++]={game+Roots[k],&output.pools[k],4};
 for(unsigned k=0;k<addresses.roots;++k)spans[n++]={addresses.pools[k],&output.headers[k],sizeof(output.headers[k])};
 spans[n++]={addresses.owner,output.body.data(),output.body.size()};
 spans[n++]={reinterpret_cast<uintptr_t>(addresses.headers[addresses.roots-1].m_aFlags)+addresses.slot,&output.reference,1};
 spans[n++]={game+0xe95cd8+addresses.index*4,&output.info,4};
 // Native scalar detail preparation reads the complete model header under
 // the source lock; outside-lock staging cannot replace that current read.
 spans[n++]={addresses.info,output.model.data(),output.model.size()};
 spans[n++]={addresses.matrix?addresses.matrix:addresses.owner+0x10,output.pose.data(),addresses.matrix?64u:16u};
 return n;
}
void Preflight(Scratch& scratch,unsigned count) noexcept {
 // Permission-only region cache ends at this read phase, before any caller restart.
 const immediate_region33::Scope regions202;
 using namespace traffic_signal::fresh_native_window115;uint8_t dummy{};
 bool any=false;for(unsigned i=0;i<count;++i)any=any||scratch.rows[i].ready;
 if(!requested||!any)return;
 for(unsigned i=0;i<count;++i){auto& h=scratch.rows[i];scratch.groups[i]={};
  if(h.ready){Hint unused{};const auto n=OwnerSpans(h,unused,scratch.spans[i]);
   for(unsigned j=0;j<n;++j)scratch.spans[i][j].output=&dummy;scratch.groups[i]={scratch.spans[i],n};}
 }
 traffic_signal::fresh_native_certificate115::Preflight(scratch.ownerCertificate,scratch.groups,count,ReadableOriginal,CE_OWNER_BATCH_QUERY35);
 for(unsigned i=0;i<count;++i){auto& h=scratch.rows[i];scratch.groups[i]={};
  if(h.ready){scratch.spans[i][0]={h.descriptor,&dummy,0x6c};scratch.groups[i]={scratch.spans[i],1};}
 }
 traffic_signal::fresh_native_certificate115::Preflight(scratch.descriptorCertificate,scratch.groups,count,ReadableOriginal,CE_OWNER_BATCH_QUERY35);
}
// Called while the source registry lock is held. Staging values are only address
// hints; bind fresh roots/body/flag/table/hash/pose twice, then copy descriptor.
bool Commit(const Hint& addresses,Match& published,std::array<uint8_t,0x6c>& bytes,bool& faulted,bool& restart,
 Scratch& scratch,unsigned group) noexcept {
 // Permission-only region cache ends at this read phase, before any caller restart.
 const immediate_region33::Scope regions202;
 using namespace traffic_signal::fresh_native_window115;
 faulted=false;restart=false;if(!addresses.ready||!Active(addresses.roots))return false;
 Hint current=addresses,again=addresses;Span spans[16]{};
 auto copy=[&](Hint& output) noexcept {
  const auto n=OwnerSpans(addresses,output,spans);bool copied=false,fault=false;
  const bool handled=traffic_signal::fresh_native_certificate115::TryCopy(scratch.ownerCertificate,group,spans,n,copied,fault);
  if(!handled){const Group g{spans,n};const auto r=Copy(&g,1,ReadableOriginal,CE_OWNER_BATCH_QUERY35);copied=r.copied[0];fault=r.faulted[0];}
  faulted=faulted||fault;return copied;
 };
 auto subset=[&](const Span* wanted,unsigned n) noexcept {
  bool copied=false,fault=false;
  const bool handled=traffic_signal::fresh_native_certificate115::TryCopySubset(scratch.ownerCertificate,group,wanted,n,copied,fault);
  // Without the exact reusable permission domain, run the ORIGINAL complete
  // scalar sequence after unlock instead of repeating six individual phases.
  // No actual source read or read fault occurred in this declined subset.
  if(!handled){restart=true;return false;}
  faulted=faulted||fault;if(!copied)restart=!faulted;return copied;
 };
 // Reproduce the native dependency order. Pointer-bearing current values are
 // read and checked BEFORE any dependent address from the outside-lock hints.
 for(unsigned k=0;k<addresses.roots;++k)spans[k]={game+Roots[k],&current.pools[k],4};
 if(!subset(spans,addresses.roots))return false;
 for(unsigned k=0;k<addresses.roots;++k)if(current.pools[k]!=addresses.pools[k]){restart=true;return false;}
 for(unsigned k=0;k<addresses.roots;++k)spans[k]={current.pools[k],&current.headers[k],sizeof(current.headers[k])};
 if(!subset(spans,addresses.roots))return false;
 for(unsigned k=0;k<addresses.roots;++k)if(!SameHeader(current.headers[k],addresses.headers[k])){restart=true;return false;}
 spans[0]={addresses.owner,current.body.data(),current.body.size()};
 spans[1]={reinterpret_cast<uintptr_t>(current.headers[addresses.roots-1].m_aFlags)+addresses.slot,&current.reference,1};
 if(!subset(spans,2))return false;
 int16_t freshIndex{};uintptr_t freshMatrix{};std::memcpy(&freshIndex,current.body.data()+0x2e,2);std::memcpy(&freshMatrix,current.body.data()+0x20,4);
 if(freshIndex!=addresses.index||freshMatrix!=addresses.matrix){restart=true;return false;}
 if(!Membership(current)||!Class(current)||(current.reference&0x80))return false;
 spans[0]={game+0xe95cd8+freshIndex*4,&current.info,4};if(!subset(spans,1))return false;
 if(current.info!=addresses.info){restart=true;return false;}
 spans[0]={current.info,current.model.data(),current.model.size()};
 spans[1]={freshMatrix?freshMatrix:addresses.owner+0x10,current.pose.data(),freshMatrix?64u:16u};
 if(!subset(spans,2))return false;
 uint32_t detailHash{};std::memcpy(&detailHash,current.model.data()+0x3c,4);if(!detailHash)return false;
 const unsigned detailOffset=freshMatrix?12u:0u;
 for(unsigned j=0;j<3;++j)if(!std::isfinite(current.pose[detailOffset+j]))return false;
 if(!copy(again)){
  // A refused OLD address hint is not current native ownership authority.
  // Rebind through the original scalar validator after unlock; never retry a
  // source that actually faulted and could have consumed a one-shot guard.
  restart=!faulted;return false;
 }
 if(!Active(addresses.roots))return false;
 // Native TYPE1's scalar matcher rereads all roots after its final body/pose
 // copies. Preserve that ordering, with a completely fresh permission query.
 // TYPE5 retains its original joint snapshot semantics.
 if(addresses.roots==4){uintptr_t rootsAfter[4]{};Span rootSpans[4]{};
  for(unsigned k=0;k<4;++k)rootSpans[k]={game+Roots[k],&rootsAfter[k],4};
  const Group rootGroup{rootSpans,4};const auto rootsCopy=Copy(&rootGroup,1,ReadableOriginal,CE_OWNER_BATCH_QUERY35);
  faulted=faulted||rootsCopy.faulted[0];if(!rootsCopy.copied[0]||!Active(addresses.roots))return false;
  for(unsigned k=0;k<4;++k)if(rootsAfter[k]!=again.pools[k])return false;
 }
 int16_t index{};uintptr_t matrix{};std::memcpy(&index,current.body.data()+0x2e,2);std::memcpy(&matrix,current.body.data()+0x20,4);
 for(unsigned k=0;k<addresses.roots;++k)if(current.pools[k]!=addresses.pools[k]||!SameHeader(current.headers[k],addresses.headers[k]))restart=true;
 if(index!=addresses.index||matrix!=addresses.matrix||current.info!=addresses.info)restart=true;
 if(restart)return false;
 if(!Membership(current)||!Class(current)||!Class(again)||(current.reference&0x80)||current.reference!=again.reference||current.info!=again.info)return false;
 for(unsigned k=0;k<addresses.roots;++k)if(current.pools[k]!=again.pools[k]||!SameHeader(current.headers[k],again.headers[k]))return false;
 if(std::memcmp(current.body.data()+0x20,again.body.data()+0x20,4)||std::memcmp(current.body.data()+0x2e,again.body.data()+0x2e,2)||
  std::memcmp(current.model.data()+0x3c,again.model.data()+0x3c,4)||std::memcmp(current.pose.data(),again.pose.data(),addresses.matrix?64u:16u))return false;
 if(!addresses.matrix&&std::memcmp(again.body.data()+0x10,again.pose.data(),16))return false;
 uint32_t hash{};std::memcpy(&hash,current.model.data()+0x3c,4);if(!hash)return false;
 const unsigned offset=addresses.matrix?12u:0u;for(unsigned j=0;j<3;++j)if(!std::isfinite(current.pose[offset+j]))return false;
 const Span descriptor{addresses.descriptor,bytes.data(),bytes.size()};bool copied=false,fault=false;
 const bool handled=traffic_signal::fresh_native_certificate115::TryCopy(scratch.descriptorCertificate,group,&descriptor,1,copied,fault);
 if(!handled){const Group g{&descriptor,1};const auto r=Copy(&g,1,ReadableOriginal,CE_OWNER_BATCH_QUERY35);copied=r.copied[0];fault=r.faulted[0];}
 faulted=faulted||fault;if(!copied||!Active(addresses.roots))return false;
 Match value{};const auto& h=current.headers[addresses.roots-1];
 value.identity={uint32_t(current.pools[addresses.roots-1]),uint32_t(reinterpret_cast<uintptr_t>(h.m_aStorage)),uint32_t(reinterpret_cast<uintptr_t>(h.m_aFlags)),uint32_t(addresses.owner),addresses.slot,current.reference,hash};
 value.kind=addresses.roots-1;value.modelInfo=uint32_t(current.info);std::memcpy(value.ownerPose109.data(),&matrix,4);
 std::memcpy(value.ownerPose109.data()+4,current.pose.data(),addresses.matrix?64u:16u);
 for(unsigned j=0;j<3;++j)value.entity[j]=current.pose[offset+j];published=value;return true;
}
}
