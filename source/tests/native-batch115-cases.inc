unsigned nativeAllocations115{},nativeReleases115{};bool failNativeAllocation115=false;
std::vector<void*> nativeLive115;
void* NativeAllocate115(size_t n) noexcept {
 ++nativeAllocations115;SetLastError(11500);ClobberAllocatorFp59();
 if(failNativeAllocation115){failNativeAllocation115=false;return nullptr;}
 auto* p=HeapAlloc(GetProcessHeap(),0,n);assert(p);for(auto* existing:nativeLive115)assert(existing!=p);nativeLive115.push_back(p);return p;
}
bool NativeRelease115(void* p) noexcept {
 ++nativeReleases115;assert(!nativeLive115.empty()&&nativeLive115.back()==p);nativeLive115.pop_back();
 SetLastError(11501);ClobberAllocatorFp59();return HeapFree(GetProcessHeap(),0,p)!=FALSE;
}
struct NativeCorpus115 {
 std::vector<uint8_t*> allocations;
 std::vector<rage::CLightSource> packets;
 model_light20::Registry<1024> registry;
 uint8_t *owners5{},*owners1{},*flags5{},*flags1{},*poses{},*descriptors{},*models{};
 unsigned count{};bool distinct{};
 uint8_t* Allocate(size_t n){auto* p=static_cast<uint8_t*>(VirtualAlloc(nullptr,n,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(p);allocations.push_back(p);return p;}
 NativeCorpus115(unsigned number,bool separate,unsigned extraOld=0):count(number),distinct(separate){
  Initialize49();Configure49();TrafficSignal::ownerClass109Active=true;TrafficSignal::ownerClass114Active=true;
  const auto arenaBytes115=size_t(number)*Page49;
  owners5=Allocate(size_t(number)*0x80);owners1=Allocate(size_t(number)*0x110);
  flags5=Allocate(Page49);flags1=Allocate(Page49);poses=Allocate(separate?arenaBytes115:size_t(number)*64);
  descriptors=Allocate(separate?arenaBytes115:size_t(number)*128);models=Allocate(separate?arenaBytes115:Page49);
  auto* header5=reinterpret_cast<rage::VehiclePool*>(Allocate(Page49));
  auto* header1=reinterpret_cast<rage::VehiclePool*>(Allocate(Page49));
  *header5={owners5,flags5,int32_t(number),0x80,0,int32_t(number)};
  *header1={owners1,flags1,int32_t(number),0x110,0,int32_t(number)};
  Root49(native_owner109::PoolRva,uintptr_t(header5));Root49(native_owner114::PoolRva,uintptr_t(header1));
  // Both old0/1 classes participate in every corpus.
  ModelLightShadow20::registry={};assert(ModelLightShadow20::registry.Configure(0x30000000));
  const int16_t oldIndex=5;const auto oldMatrix=uintptr_t(matrices49);
  std::memcpy(building49+0x2e,&oldIndex,2);std::memcpy(building49+0x20,&oldMatrix,4);
  TrafficSignal::Match oldObject{};assert(TrafficSignal::MatchOwner(uintptr_t(owners49),oldObject));
  std::array<uint8_t,108> objectBytes{};std::memcpy(objectBytes.data(),descriptors49,108);
  auto* object=ModelLightShadow20::registry.Observe(oldObject.identity,0,uint32_t(uintptr_t(descriptors49)),oldObject.modelInfo,objectBytes,7);
  assert(object&&ModelLightShadow20::registry.Publish(object->key));packets.push_back({object->key,0x44});
  TrafficSignal::Match oldBuilding{};assert(TrafficSignal::MatchOwner(uintptr_t(building49),oldBuilding));
  std::array<uint8_t,108> oldBytes{};std::memcpy(oldBytes.data(),descriptors49,108);
  auto* old=ModelLightShadow20::registry.Observe(oldBuilding.identity,0,uint32_t(uintptr_t(descriptors49)),oldBuilding.modelInfo,oldBytes,7);
  assert(old&&ModelLightShadow20::registry.Publish(old->key));packets.push_back({old->key,0x44});
  for(unsigned i=0;i<number;++i){
   const bool type1=(i&2)!=0;auto* owner=(type1?owners1+i*0x110:owners5+i*0x80);
   const uint32_t vtable=uint32_t(game+(type1?native_owner114::VtableRva:native_owner109::VtableRva));
   const uint32_t type=type1?0x40u:0x140u;const int16_t index=int16_t(separate?100+i:6);
   std::memcpy(owner,&vtable,4);std::memcpy(owner+0x28,&type,4);std::memcpy(owner+0x2e,&index,2);
   (type1?flags1:flags5)[i]=7;
   if(i&1)for(unsigned j=0;j<4;++j)reinterpret_cast<float*>(owner+0x10)[j]=float(1+i+j);
   else {const uintptr_t matrix=uintptr_t(poses+(separate?i*Page49:i*64));std::memcpy(owner+0x20,&matrix,4);
    for(unsigned j=0;j<3;++j)reinterpret_cast<float*>(matrix)[12+j]=float(i+j+1);}
   auto* model=models+(separate?i*Page49:0);const uint32_t hash=1000+i*(separate?1u:0u);
   std::memcpy(model+0x3c,&hash,4);Root49(0xe95cd8+index*4,uintptr_t(model));
   auto* descriptor=descriptors+(separate?i*Page49:i*128);
   for(unsigned j=0;j<108;++j)descriptor[j]=uint8_t(i+j);
   TrafficSignal::Match match{};assert(TrafficSignal::MatchOwner(uintptr_t(owner),match));assert(match.kind==(type1?3u:2u));
   std::array<uint8_t,108> bytes{};std::memcpy(bytes.data(),descriptor,108);
   auto* e=ModelLightShadow20::registry.Observe(match.identity,0,uint32_t(uintptr_t(descriptor)),match.modelInfo,bytes,7);
   assert(e&&ModelLightShadow20::registry.Publish(e->key));packets.push_back({e->key,0x44});
  }
  for(unsigned i=1;i<=extraOld;++i){TrafficSignal::Match m{};assert(TrafficSignal::MatchOwner(uintptr_t(owners49+i*0x320),m));
   std::array<uint8_t,108> bytes{};std::memcpy(bytes.data(),descriptors49+i*128,108);
   auto* e=ModelLightShadow20::registry.Observe(m.identity,0,uint32_t(uintptr_t(descriptors49+i*128)),m.modelInfo,bytes,7);
   assert(e&&ModelLightShadow20::registry.Publish(e->key));packets.push_back({e->key,0x44});}
  registry=ModelLightShadow20::registry;
 }
 void Restore(){ModelLightShadow20::registry=registry;Baseline111::registry=registry;ModelLightShadow20::active=true;
  ModelLightShadow20::lock.clear();CShadows::frame=7;mutate49=nullptr;mutateAt49=0;queryOrder59.clear();
  ModelLightShadow20::queuedScratch32Requested59=true;ModelLightShadow20::queuedCommitCertificate54=true;
  Baseline111::queuedScratch32Requested59=true;Baseline111::queuedCommitCertificate54=true;
 }
 ~NativeCorpus115(){TrafficSignal::NativeOwnerBatch115::requested=false;
  TrafficSignal::ownerClass109Active=false;TrafficSignal::ownerClass114Active=false;
  for(auto* p:allocations)assert(VirtualFree(p,0,MEM_RELEASE));
  for(auto* p:{headers49,owners49,building49,flags49,model49,matrices49,descriptors49})assert(VirtualFree(p,0,MEM_RELEASE));
 }
};
struct NativeRun115 {std::vector<rage::CLightSource> packets;unsigned queries{},pages{},virtualQueries{};double ms{};};
NativeRun115 RunNative115(NativeCorpus115& data,bool candidate,bool certificate=true){
 data.Restore();auto packets=data.packets;TrafficSignal::NativeOwnerBatch115::requested=candidate;
 ModelLightShadow20::queuedCommitCertificate54=certificate;Baseline111::queuedCommitCertificate54=certificate;
 queries49=pages49=virtualQueries115=0;SetLastError(0x115);const auto csr=_mm_getcsr();LARGE_INTEGER before{},after{},frequency{};
 assert(QueryPerformanceFrequency(&frequency));assert(QueryPerformanceCounter(&before));
 if(candidate)ModelLightShadow20::ValidateQueued(packets.data(),unsigned(packets.size()));
 else Baseline111::ValidateQueued(packets.data(),unsigned(packets.size()));
 assert(QueryPerformanceCounter(&after));assert(GetLastError()==0x115&&_mm_getcsr()==csr);
 assert(liveScratch59.empty()&&nativeLive115.empty());return {packets,queries49,pages49,virtualQueries115,double(after.QuadPart-before.QuadPart)*1000/double(frequency.QuadPart)};
}
void CompareNative115(const NativeRun115& a,const NativeRun115& b){assert(a.packets.size()==b.packets.size());
 for(unsigned i=0;i<a.packets.size();++i)assert(a.packets[i].mFlags==b.packets[i].mFlags&&a.packets[i].mCastShadows==b.packets[i].mCastShadows);
}
NativeCorpus115* mutateNative115{};unsigned nativeHookMode115{},nativeHookHits115{};
void MutationNative115(unsigned group){
 if(group!=2||++nativeHookHits115!=1)return;
 nativeStageHook115=nullptr;auto& d=*mutateNative115;
 if(nativeHookMode115==0)d.flags5[0]^=1;
 if(nativeHookMode115==1)d.models[0x3c]^=1;
 if(nativeHookMode115==2)d.descriptors[0]^=1;
 if(nativeHookMode115==3)reinterpret_cast<float*>(d.poses)[12]=NAN;
 if(nativeHookMode115==4){const int16_t index=-1;std::memcpy(d.owners5+0x2e,&index,2);}
 if(nativeHookMode115==5){const uintptr_t matrix=uintptr_t(d.poses+64);std::memcpy(d.owners5+0x20,&matrix,4);}
 if(nativeHookMode115==6){DWORD prior{};assert(VirtualProtect(d.owners5,Page49,PAGE_READWRITE|PAGE_GUARD,&prior));}
 if(nativeHookMode115==7){DWORD prior{};assert(VirtualProtect(d.descriptors,Page49,PAGE_NOACCESS,&prior));}
 if(nativeHookMode115==8)++CShadows::frame;
 if(nativeHookMode115==9)TrafficSignal::ownerClass109Active=false;
}
void MetadataNative115(){auto& d=*mutateNative115;auto* e=ModelLightShadow20::registry.Lookup(d.packets[2].mCastShadows);assert(e);e->descriptor=uint32_t(uintptr_t(d.descriptors+128));}
bool nestedNativeSeen115{};
unsigned invalidDetailHits115{};bool invalidDetailSeen115{};
void InvalidDetail115(unsigned group){if(group!=2||++invalidDetailHits115!=5)return;nativeStageHook115=nullptr;
 const uint32_t zero{};std::memcpy(mutateNative115->models+0x3c,&zero,4);
 DWORD prior{};assert(VirtualProtect(mutateNative115->owners5,Page49,PAGE_READWRITE|PAGE_GUARD,&prior));invalidDetailSeen115=true;
}
uint8_t* reboundOldPose115{};uint8_t* reboundNewPose115{};bool reboundPoseSeen115{};
void RebindDeniedPose115(PSAPI_WORKING_SET_EX_INFORMATION* q,DWORD count){
 bool old=false,body=false;for(unsigned i=0;i<count;++i){old=old||uintptr_t(q[i].VirtualAddress)==uintptr_t(reboundOldPose115);body=body||uintptr_t(q[i].VirtualAddress)==uintptr_t(mutateNative115->owners5);}
 if(!old||!body)return;nativeBeforeQueryHook115=nullptr;
 const uintptr_t matrix=uintptr_t(reboundNewPose115);std::memcpy(mutateNative115->owners5+0x20,&matrix,4);
 DWORD prior{};assert(VirtualProtect(reboundOldPose115,Page49,PAGE_NOACCESS,&prior));reboundPoseSeen115=true;
}
void RebindPositivePose115(unsigned group){if(group!=2)return;nativeStageHook115=nullptr;
 const uintptr_t matrix=uintptr_t(reboundNewPose115);std::memcpy(mutateNative115->owners5+0x20,&matrix,4);
 DWORD prior{};assert(VirtualProtect(reboundOldPose115,Page49,PAGE_NOACCESS,&prior));reboundPoseSeen115=true;
}
unsigned rootAfterHits115{};bool rootAfterMutated115{};uintptr_t originalRoot115{};
void ChangeRootAfter115(PSAPI_WORKING_SET_EX_INFORMATION* q,DWORD count){
 if(count!=4||uintptr_t(q[3].VirtualAddress)!=game+native_owner114::PoolRva)return;
 Root49(native_owner114::PoolRva,0);rootAfterMutated115=true;nativeQueryHook115=nullptr;
}
void ArmRootAfter115(unsigned group){
 if(group!=4||++rootAfterHits115!=2)return;
 nativeStageHook115=nullptr;nativeQueryHook115=ChangeRootAfter115;
}
uintptr_t alternateModel115{},originalModel115{};unsigned restoredModelState115{};
uint8_t* modelFirstRegion115{};bool modelRegionChanged115{};
void ProtectModelHeader115(PSAPI_WORKING_SET_EX_INFORMATION* q,DWORD count){
 bool body=false,table=false;for(unsigned i=0;i<count;++i){body=body||uintptr_t(q[i].VirtualAddress)==uintptr_t(mutateNative115->owners5);table=table||uintptr_t(q[i].VirtualAddress)==game+0xe95cd8+6*4;}
 if(!body||!table)return;nativeBeforeQueryHook115=nullptr;
 DWORD prior{};assert(VirtualProtect(modelFirstRegion115,Page49,PAGE_READWRITE|PAGE_GUARD,&prior));modelRegionChanged115=true;}
void RestoreModelHint115(PSAPI_WORKING_SET_EX_INFORMATION* q,DWORD count){
 const uintptr_t table=game+0xe95cd8+6*4;
 for(unsigned i=0;i<count;++i){const auto address=uintptr_t(q[i].VirtualAddress);
  if(restoredModelState115==0&&address==table){Root49(0xe95cd8+6*4,alternateModel115);restoredModelState115=1;return;}
  if(restoredModelState115==1&&address==alternateModel115){Root49(0xe95cd8+6*4,originalModel115);restoredModelState115=2;nativeQueryHook115=nullptr;return;}
 }
}
void NestedNative115(){auto& d=*mutateNative115;auto packets=d.packets;
 assert(liveScratch59.size()==1&&nativeLive115.size()==1);ModelLightShadow20::ValidateQueued(packets.data(),unsigned(packets.size()));
 assert(liveScratch59.size()==1&&nativeLive115.size()==1);for(const auto& p:packets)assert(p.mFlags==0x44);nestedNativeSeen115=true;}
void AdversarialNative115(){
 {NativeCorpus115 d(100,false);d.packets.resize(3);mutateNative115=&d;invalidDetailHits115=0;invalidDetailSeen115=false;
  nativeStageHook115=InvalidDetail115;auto actual=RunNative115(d,true);nativeStageHook115=nullptr;assert(invalidDetailSeen115&&actual.packets[2].mFlags==0x40);
  MEMORY_BASIC_INFORMATION info{};assert(VirtualQuery(d.owners5,&info,sizeof(info))&&(info.Protect&PAGE_GUARD));
  DWORD prior{};assert(VirtualProtect(d.owners5,Page49,PAGE_READWRITE,&prior));
 }
 {NativeCorpus115 d(100,false);auto* alternate=d.Allocate(Page49);std::memcpy(alternate,d.models,0x40);
  originalModel115=uintptr_t(d.models);alternateModel115=uintptr_t(alternate);restoredModelState115=0;nativeQueryHook115=RestoreModelHint115;
  auto actual=RunNative115(d,true);nativeQueryHook115=nullptr;assert(restoredModelState115==2);
  auto reference=RunNative115(d,false);CompareNative115(reference,actual);for(const auto& packet:actual.packets)assert(packet.mFlags==0x44);
 }
 // Valid current pose rebind with an unreadable OLD pose must use the original
 // fresh matcher rather than turn a staging address into a rejection authority.
 {NativeCorpus115 d(100,true);mutateNative115=&d;reboundOldPose115=d.poses;reboundNewPose115=d.Allocate(Page49);
  std::memcpy(reboundNewPose115,reboundOldPose115,64);reboundPoseSeen115=false;nativeBeforeQueryHook115=RebindDeniedPose115;
  auto actual=RunNative115(d,true);nativeBeforeQueryHook115=nullptr;assert(reboundPoseSeen115);
  auto reference=RunNative115(d,false);CompareNative115(reference,actual);assert(actual.packets[2].mFlags==0x44);
  DWORD prior{};assert(VirtualProtect(reboundOldPose115,Page49,PAGE_READWRITE,&prior));
 }
 {NativeCorpus115 d(100,true);mutateNative115=&d;reboundOldPose115=d.poses;reboundNewPose115=d.Allocate(Page49);
  std::memcpy(reboundNewPose115,reboundOldPose115,64);reboundPoseSeen115=false;nativeStageHook115=RebindPositivePose115;
  auto actual=RunNative115(d,true);nativeStageHook115=nullptr;assert(reboundPoseSeen115);
  auto reference=RunNative115(d,false);CompareNative115(reference,actual);assert(actual.packets[2].mFlags==0x44);
  DWORD prior{};assert(VirtualProtect(reboundOldPose115,Page49,PAGE_READWRITE,&prior));
 }
 // At native TYPE1's second owner copy, arm a root change for the following
 // post-body root query. The current root MUST refuse the affected TYPE1.
 {NativeCorpus115 d(100,false);std::memcpy(&originalRoot115,image49.data()+native_owner114::PoolRva,4);
  rootAfterHits115=0;rootAfterMutated115=false;nativeStageHook115=ArmRootAfter115;
  auto actual=RunNative115(d,true);nativeStageHook115=nullptr;nativeQueryHook115=nullptr;
  assert(rootAfterMutated115&&actual.packets[4].mFlags==0x40&&actual.packets[2].mFlags==0x44);
  Root49(native_owner114::PoolRva,originalRoot115);
 }
 for(unsigned mode=0;mode<10;++mode){NativeCorpus115 d(100,false);d.Restore();mutateNative115=&d;nativeHookMode115=mode;nativeHookHits115=0;
  nativeStageHook115=MutationNative115;auto result=RunNative115(d,true);nativeStageHook115=nullptr;
  const unsigned expected=mode==5?0x44u:0x40u; // A valid replacement pose is freshly rebound by original scalar fallback.
  if(!nativeHookHits115||result.packets[2].mFlags!=expected)std::fprintf(stderr,"native115 mode=%u hits=%u flags=%x\n",mode,nativeHookHits115,result.packets[2].mFlags);
  assert(nativeHookHits115&&result.packets[2].mFlags==expected);
  if(mode==6||mode==7){DWORD prior{};assert(VirtualProtect(mode==6?d.owners5:d.descriptors,Page49,PAGE_READWRITE,&prior));}
  if(mode==9)TrafficSignal::ownerClass109Active=true;
 }
 {NativeCorpus115 d(100,false);d.Restore();mutateNative115=&d;TrafficSignal::NativeOwnerBatch115::requested=true;
  auto packets=d.packets;queries49=pages49=0;mutateAt49=1;mutate49=MetadataNative115;
  ModelLightShadow20::ValidateQueued(packets.data(),unsigned(packets.size()));assert(packets[2].mFlags==0x40&&mutate49==nullptr);}
 // Reentry during typed staging owns a distinct native workspace and leaves
 // every outer address/value hint and permission domain untouched.
 {NativeCorpus115 d(100,false);d.Restore();mutateNative115=&d;TrafficSignal::NativeOwnerBatch115::requested=true;
  auto packets=d.packets;queries49=pages49=0;mutateAt49=6;mutate49=NestedNative115;nestedNativeSeen115=false;
  ModelLightShadow20::ValidateQueued(packets.data(),unsigned(packets.size()));assert(nestedNativeSeen115);
  for(const auto& p:packets)assert(p.mFlags==0x44);assert(liveScratch59.empty()&&nativeLive115.empty());}
 // Permission certificate OFF and a failed working-set query both retain
 // complete fresh current copies through the original VirtualQuery fallback.
 {NativeCorpus115 d(100,false);for(bool certificate:{false,true}){
   auto reference=RunNative115(d,false,certificate),actual=RunNative115(d,true,certificate);CompareNative115(reference,actual);
   failWs115=true;reference=RunNative115(d,false,certificate);actual=RunNative115(d,true,certificate);CompareNative115(reference,actual);failWs115=false;
  }}
 // Cross-page model, allocated pose, and descriptor spans have two fresh
 // protection regions and cannot reuse a one-page positive certificate.
 {NativeCorpus115 d(100,false);auto* model=d.Allocate(2*Page49)+Page49-32;
  auto* pose=d.Allocate(2*Page49)+Page49-32;auto* descriptor=d.Allocate(2*Page49)+Page49-54;
  auto* e=d.registry.Lookup(d.packets[2].mCastShadows);assert(e);const uint32_t originalHash=1000;std::memcpy(model+0x3c,&originalHash,4);
  reinterpret_cast<float*>(pose)[12]=1;reinterpret_cast<float*>(pose)[13]=2;reinterpret_cast<float*>(pose)[14]=3;
  const uintptr_t matrix=uintptr_t(pose);std::memcpy(d.owners5+0x20,&matrix,4);Root49(0xe95cd8+6*4,uintptr_t(model));
  e->modelInfo=uint32_t(uintptr_t(model));e->descriptor=uint32_t(uintptr_t(descriptor));std::memcpy(descriptor,e->bytes.data(),108);
  auto reference=RunNative115(d,false),actual=RunNative115(d,true);CompareNative115(reference,actual);assert(actual.packets[2].mFlags==0x44);
  // The first model region changes AFTER initial full-model staging, at
  // final preflight. Its hash4 remains readable in the second region.
  modelFirstRegion115=model-Page49+32;modelRegionChanged115=false;mutateNative115=&d;nativeBeforeQueryHook115=ProtectModelHeader115;
  actual=RunNative115(d,true);nativeBeforeQueryHook115=nullptr;assert(modelRegionChanged115&&actual.packets[2].mFlags==0x40);
  DWORD restoreProtection{};assert(VirtualProtect(modelFirstRegion115,Page49,PAGE_READWRITE,&restoreProtection));
  for(auto* second:{model+32,pose+32,descriptor+54})for(DWORD protection:{DWORD(PAGE_GUARD|PAGE_READWRITE),DWORD(PAGE_NOACCESS)}){
   DWORD prior{};assert(VirtualProtect(second,Page49,protection,&prior));reference=RunNative115(d,false);
   assert(VirtualProtect(second,Page49,PAGE_READWRITE,&prior));assert(VirtualProtect(second,Page49,protection,&prior));actual=RunNative115(d,true);CompareNative115(reference,actual);
   MEMORY_BASIC_INFORMATION info{};assert(VirtualQuery(second,&info,sizeof(info)));assert(protection!=DWORD(PAGE_GUARD|PAGE_READWRITE)||(info.Protect&PAGE_GUARD));
   assert(VirtualProtect(second,Page49,PAGE_READWRITE,&prior));
  }}
 // Failed lazy allocation retains the complete original native scalar path,
 // including every query and packet result, and attempts allocation only once.
 {NativeCorpus115 d(100,false);auto reference=RunNative115(d,false);const auto allocations=nativeAllocations115;
  failNativeAllocation115=true;auto actual=RunNative115(d,true);CompareNative115(reference,actual);
  assert(nativeAllocations115==allocations+1&&actual.queries==reference.queries&&!failNativeAllocation115);}
 // Default OFF must not allocate or touch any extra permission domain.
 {NativeCorpus115 d(100,false);const auto allocations=nativeAllocations115;auto reference=RunNative115(d,false),off=RunNative115(d,false);CompareNative115(reference,off);
  assert(nativeAllocations115==allocations&&reference.queries==off.queries);}
 // Active prototype with exclusively old classes still has no typed workspace.
 {NativeCorpus115 d(100,false);d.packets.resize(2);const auto allocations=nativeAllocations115;
  auto reference=RunNative115(d,false),actual=RunNative115(d,true);CompareNative115(reference,actual);
  assert(nativeAllocations115==allocations&&reference.queries==actual.queries);}
 // A missing fourth root must not change either old class or TYPE5 authority.
 // A missing third root is required by both native matchers, so both refuse.
 {NativeCorpus115 d(100,false);for(unsigned root:{native_owner114::PoolRva,native_owner109::PoolRva}){
  uintptr_t prior{};std::memcpy(&prior,image49.data()+root,4);Root49(root,0);
  auto reference=RunNative115(d,false),actual=RunNative115(d,true);CompareNative115(reference,actual);
  assert(actual.packets[0].mFlags==0x44&&actual.packets[1].mFlags==0x44);
  if(root==native_owner114::PoolRva)assert(actual.packets[2].mFlags==0x44);
  Root49(root,prior);
 }}
 std::puts("PASS native115 fresh commit mutation: reference/hash/descriptor/pose/index/matrix/Guard/NoAccess/frame/capability/metadata; native allocation-failure exact query/output fallback; Off and old-only no typed allocation");
}
void NativeBatch115Cases(bool measure){
 {using namespace traffic_signal::fresh_native_window115;using namespace traffic_signal::fresh_native_certificate115;
  auto* region=static_cast<uint8_t*>(VirtualAlloc(nullptr,Page49,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(region);std::memset(region,0x17,Page49);
  std::array<uint8_t,8> a{},b{},other{};Span exact[2]{{uintptr_t(region),a.data(),8},{uintptr_t(region+16),b.data(),8}};
  Span foreign{uintptr_t(region+32),other.data(),8};Group groups[2]{{exact,2},{&foreign,1}};Certificate permission{};bool copied{},faulted{};
  Preflight(permission,groups,2,TrafficSignal::ReadableOriginal,CountQuery49);assert(permission.active&&permission.allowed[0]&&permission.reusable[0]);
  region[0]=0x29;assert(TryCopySubset(permission,0,exact,1,copied,faulted)&&copied&&!faulted&&a[0]==0x29);
  other.fill(0xee);assert(!TryCopySubset(permission,0,&foreign,1,copied,faulted)&&!permission.active&&!copied&&!faulted&&other[0]==0xee);
  Preflight(permission,groups,2,TrafficSignal::ReadableOriginal,CountQuery49);Span widened{uintptr_t(region),a.data(),7};a.fill(0xee);
  assert(!TryCopySubset(permission,0,&widened,1,copied,faulted)&&!permission.active&&a[0]==0xee);
  Preflight(permission,groups,2,TrafficSignal::ReadableOriginal,CountQuery49);DWORD prior{};assert(VirtualProtect(region,Page49,PAGE_READWRITE|PAGE_GUARD,&prior));
  assert(TryCopySubset(permission,0,exact,1,copied,faulted)&&!copied&&faulted&&!permission.active&&a[0]==0xee);
  assert(VirtualProtect(region,Page49,PAGE_READWRITE,&prior));assert(VirtualFree(region,0,MEM_RELEASE));
  std::puts("PASS exact same-group subset permissions: fresh changed bytes, wrong group/size domain termination, terminal guard fault with no partial output");
 }
 wchar_t module115[MAX_PATH]{};assert(GetModuleFileNameW(nullptr,module115,MAX_PATH)>0);
 const std::wstring ini115=std::wstring(module115)+L".nativeOwner115-test.ini";
 assert(WritePrivateProfileStringW(L"SUPPORT",L"NativeOwnerBatch115",nullptr,ini115.c_str()));
 ModelLightShadow20::ConfigureQueuedScratch59(ini115.c_str());assert(TrafficSignal::NativeOwnerBatch115::requested);
 for(const wchar_t* value:{L"1",L"0",L"2"}){assert(WritePrivateProfileStringW(L"SUPPORT",L"NativeOwnerBatch115",value,ini115.c_str()));
  ModelLightShadow20::ConfigureQueuedScratch59(ini115.c_str());assert(TrafficSignal::NativeOwnerBatch115::requested==(value[0]==L'1'));}
 assert(DeleteFileW(ini115.c_str()));TrafficSignal::NativeOwnerBatch115::requested=false;
 AdversarialNative115();
 for(unsigned count:{100u,500u,250u})for(bool distinct:{false,true}){
  NativeCorpus115 data(count,distinct,count==250?248u:0u);auto old=RunNative115(data,false),now=RunNative115(data,true);CompareNative115(old,now);
  for(const auto& p:now.packets)assert(p.mFlags==0x44);
  assert(now.queries<old.queries);
  // Fresh flag, descriptor, model, inline/allocated pose and class refusal.
  for(unsigned mode=0;mode<5;++mode){uint8_t* address{};uint8_t saved{};
   if(mode==0)address=data.flags5;
   if(mode==1)address=data.descriptors;
   if(mode==2)address=data.models+0x3c;
   if(mode==3)address=data.owners5;
   if(mode==4)address=data.owners1+2*0x110;
   saved=*address;*address=uint8_t(saved^0x80);auto reference=RunNative115(data,false),result=RunNative115(data,true);CompareNative115(reference,result);*address=saved;
  }
  // Actual guard/noaccess pages must retain refusal and guard state; no mock permission backend.
  for(auto* page:{data.owners5,data.owners1,data.poses,data.models,data.descriptors})for(DWORD protection:{DWORD(PAGE_NOACCESS),DWORD(PAGE_READWRITE|PAGE_GUARD)}){
   DWORD prior{};assert(VirtualProtect(page,Page49,protection,&prior));auto reference=RunNative115(data,false);
   assert(VirtualProtect(page,Page49,PAGE_READWRITE,&prior));assert(VirtualProtect(page,Page49,protection,&prior));auto result=RunNative115(data,true);CompareNative115(reference,result);
   if(protection&PAGE_GUARD){MEMORY_BASIC_INFORMATION info{};assert(VirtualQuery(page,&info,sizeof(info))&&(info.Protect&PAGE_GUARD));}
   assert(VirtualProtect(page,Page49,PAGE_READWRITE,&prior));
  }
  std::vector<double> oldTimes,newTimes;
  traceQueries115=false;
  if(measure)for(unsigned i=0;i<32;++i){auto first=RunNative115(data,(i&1)!=0),second=RunNative115(data,(i&1)==0);CompareNative115(first,second);
   const auto& baseline=(i&1)?second:first;const auto& candidate=(i&1)?first:second;
   std::printf("RAW115 native=%u packets=%u layout=%s run=%u warmup=%u baseline_ms=%.6f candidate_ms=%.6f baseline_os=%u candidate_os=%u\n",count,unsigned(data.packets.size()),distinct?"distinct":"shared",i,i<4,baseline.ms,candidate.ms,baseline.queries+baseline.virtualQueries,candidate.queries+candidate.virtualQueries);
   if(i>=4){oldTimes.push_back((i&1)?second.ms:first.ms);newTimes.push_back((i&1)?first.ms:second.ms);}}
  std::sort(oldTimes.begin(),oldTimes.end());std::sort(newTimes.begin(),newTimes.end());
  traceQueries115=true;
  std::printf("115 fullqueued native=%u packets=%u layout=%s baseline_queries=%u candidate_queries=%u baseline_vq=%u candidate_vq=%u baseline_pages=%u candidate_pages=%u baseline_median_ms=%.6f candidate_median_ms=%.6f\n",count,unsigned(data.packets.size()),distinct?"distinct":"shared",old.queries,now.queries,old.virtualQueries,now.virtualQueries,old.pages,now.pages,
   oldTimes.empty()?0:oldTimes[oldTimes.size()/2],newTimes.empty()?0:newTimes[newTimes.size()/2]);
 }
 std::puts("PASS actual111/114 scalar owner versus115 fullqueued mixed0/1/2/3 inline/matrix, flags/descriptors/model/class and realGuard/NoAccess parity; no FPS claim");
}
