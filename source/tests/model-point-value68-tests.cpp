#include "OwnedImageReads224.hpp"
#include <windows.h>
#include <psapi.h>
#include <array>
#include <atomic>
#include <bit>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <optional>
#include <vector>
#include <algorithm>
#ifdef MODEL_REGION196
#include "ImmediateRegion33.hpp"
bool forceLocked196=false,failQuery196=false,crossPage196=false,disableRegions196=false;
SIZE_T WINAPI CountVirtualQuery196(LPCVOID,PMEMORY_BASIC_INFORMATION,SIZE_T);
void AfterRegion196(uintptr_t,size_t,bool) noexcept;
void AfterPoint196() noexcept;
#endif
BOOL WINAPI CountQuery(HANDLE,PVOID,DWORD);
inline constexpr auto RealQuery=QueryWorkingSetEx;
// Production helper default query functions are bound to the real counted OS
// wrapper. No positive permission response is synthesized in this fixture.
#undef QueryWorkingSetEx
#define QueryWorkingSetEx CountQuery
#define Copy ProductionBatchCopy50
#include "FreshReadBatch30.hpp"
#undef Copy
unsigned copiedBytes50{};bool countBytes50=false;
namespace traffic_signal::fresh_batch30 {
 bool Copy(const Span* spans,unsigned count,fresh_page::Check fallback,fresh_page::Query query=QueryWorkingSetEx,bool* fault=nullptr) noexcept{
  const auto ok=ProductionBatchCopy50(spans,count,fallback,query,fault);
  if(ok&&countBytes50)for(unsigned i=0;i<count;++i)copiedBytes50+=unsigned(spans[i].size);return ok;
 }
}
#undef QueryWorkingSetEx
#define QueryWorkingSetEx CountQuery
#include "ModelLightSubmission20.hpp"
#include "ModelPrevalidation40.hpp"
#include "ModelOwnerTransformDiagnostic46.hpp"
#include "ModelEffectGetterDiagnostic62.hpp"
#include "ModelLightRegistry20.hpp"
#include "core/ShadowCpuProfile29.hpp"
namespace fp=traffic_signal::fresh_page;
unsigned queries=0,queriedPages50=0,regionCalls50=0;
void AfterQuery(PVOID bytes,DWORD size,BOOL success) noexcept;
BOOL WINAPI CountQuery(HANDLE process,PVOID bytes,DWORD size){++queries;queriedPages50+=size/sizeof(PSAPI_WORKING_SET_EX_INFORMATION);const auto ok=RealQuery(process,bytes,size);AfterQuery(bytes,size,ok);
#ifdef MODEL_REGION196
 if(forceLocked196&&ok){auto* entries=static_cast<PSAPI_WORKING_SET_EX_INFORMATION*>(bytes);for(unsigned i=0;i<size/sizeof(*entries);++i)if(entries[i].VirtualAttributes.Valid)entries[i].VirtualAttributes.Locked=1;}
 if(failQuery196)return FALSE;
#endif
 return ok;}
bool Region(uintptr_t address,size_t size) noexcept {
#ifdef MODEL_REGION196
 ++regionCalls50;auto* saved=immediate_region33::current;if(disableRegions196)immediate_region33::current=nullptr;
 const bool ok=immediate_region33::Readable(address,size,CountVirtualQuery196);
 immediate_region33::current=saved;AfterRegion196(address,size,ok);return ok;
#else
 ++regionCalls50; MEMORY_BASIC_INFORMATION m{};if(!VirtualQuery(reinterpret_cast<void*>(address),&m,sizeof(m)))return false;
 return m.State==MEM_COMMIT&&!(m.Protect&(PAGE_GUARD|PAGE_NOACCESS))&&address>=uintptr_t(m.BaseAddress)&&size<=m.RegionSize-(address-uintptr_t(m.BaseAddress));
#endif
}
struct SafetyHookContext{uint32_t eflags{},esp{},ebp{},edi{};};
#include "core/ShadowFloatingPointState.hpp"
namespace CoronaLight20{bool requested=false;}
namespace PlayerShadowAllocation{bool Enabled() noexcept{return true;}}
std::atomic<bool> addonActive{true};uint32_t game=0x400000;
namespace CShadows{uint32_t frame=7;uint32_t* pFrameCounter=&frame;}
enum class Change{None,Descriptor,Generation,Revoke,Guard,Index,Hash,Matrix,Range,Free,Frame};
Change change=Change::None;unsigned ownerReads=0,changeAtOwnerRead=2;uint8_t* memory=nullptr;DWORD oldProtection=0;
bool revokeAfterQuery=false;unsigned racedRevocations=0;
bool guardOwnerAfterQuery53=false;
void AfterQuery(PVOID bytes,DWORD size,BOOL success) noexcept{
 if(success&&guardOwnerAfterQuery53){
  const auto* spans53=static_cast<const PSAPI_WORKING_SET_EX_INFORMATION*>(bytes);
  for(unsigned i53=0;i53<size/sizeof(*spans53);++i53){
   if(uintptr_t(spans53[i53].VirtualAddress)/4096==uintptr_t(memory+8192)/4096&&spans53[i53].VirtualAttributes.Valid){
    assert(VirtualProtect(memory+8192,4096,PAGE_READWRITE|PAGE_GUARD,&oldProtection));
    guardOwnerAfterQuery53=false;break;
   }
  }
 }
 if(!success||!revokeAfterQuery)return;
 const auto* entries=static_cast<const PSAPI_WORKING_SET_EX_INFORMATION*>(bytes);
 for(unsigned i=0;i<size/sizeof(*entries);++i){
  if(entries[i].VirtualAddress==memory+4096&&entries[i].VirtualAttributes.Valid){
   // Real positive permission snapshot precedes the mapping race. Production
   // CopyLocal must contain this fault even if Operation44 remembers approval.
   assert(VirtualProtect(memory+4096,4096,PAGE_NOACCESS,&oldProtection));
   revokeAfterQuery=false;++racedRevocations;return;
  }
 }
}
namespace rage {struct VehiclePool {uint8_t* m_aStorage;uint8_t* m_aFlags;int32_t m_nSize,m_nStorageSize,m_nFirstFreeIndex,m_numSlotsUsed;};}
std::vector<uint8_t> gameImage(0x12f7d64);
bool instanceCase114=false,class5Case109=false,inlineCase109=false;
uint8_t *headers50{},*flags50{},*model50{},*dummyOwner50{};
#include "TrafficSignalIdentity.hpp"
#include "PoolDescriptorSnapshots.hpp"
#include "ImmediateRegion33.hpp"
#include "NativeOwnerClass109.hpp"
#include "NativeOwnerClass114.hpp"
namespace traffic_signal::fresh_group50 {bool Copy(const fresh_batch30::Span*,unsigned,fresh_page::Check,fresh_page::Query,bool*,uint32_t) noexcept;}
struct Match {traffic_signal::Identity identity{};uint32_t kind{};float entity[3]{};uint32_t modelInfo{};std::array<uint8_t,68> ownerPose109{};};
bool ReadableOriginal(uintptr_t p,size_t n) noexcept{return Region(p,n);}
bool Readable(uintptr_t p,size_t n) noexcept{return fp::Readable(p,n,Region,CountQuery);}
#define CE_OWNER_BATCH_QUERY35 CountQuery
#include "owner-runtime32-extracted.inc"
#include "FreshReadWindow49.hpp"
namespace TrafficSignal{
 using ::Match; using ::ownerClass109Active; using ::ownerClass114Active;
 #include "ModelOwnerWindow49Runtime.inc"
 bool ReadableOriginal(uintptr_t p,size_t n) noexcept{return ::ReadableOriginal(p,n);}
 bool Readable(uintptr_t p,size_t n) noexcept{return ::Readable(p,n);}
 void Mutate50(){
  ++ownerReads;
  if(ownerReads==changeAtOwnerRead){
   if(change==Change::Descriptor)memory[4096]=99;
   if(change==Change::Generation)flags50[0]=8;
   if(change==Change::Revoke)assert(VirtualProtect(memory+4096,4096,PAGE_NOACCESS,&oldProtection));
  }
 }
 bool MatchOwner(uintptr_t owner,Match& value) noexcept{Mutate50();return ::MatchOwner(owner,value);}
 bool MatchOwnerWithDescriptor48(uintptr_t owner,Match& value,uintptr_t descriptor,void* output,size_t size,bool& copied) noexcept{
  Mutate50();return ::MatchOwnerWithDescriptor48(owner,value,descriptor,output,size,copied);
 }
}
unsigned groupCalls50{};bool cold50=false,scattered50=false;uint8_t* vectorPages50{};
unsigned mutatePhase53=0;Change earlyChange53=Change::None;
#define Copy ProductionCopy50
#include "FreshReadGroup50.hpp"
#undef Copy
namespace ModelLightShadow20 {void OnFree(uint32_t,uint32_t) noexcept;}
namespace traffic_signal::fresh_group50 {
 bool Copy(const Span* spans,unsigned count,fresh_page::Check fallback,fresh_page::Query query=QueryWorkingSetEx,bool* fault=nullptr,uint32_t rememberMask=UINT32_MAX) noexcept{
  ++groupCalls50;
  if(mutatePhase53&&groupCalls50==mutatePhase53){
   if(earlyChange53==Change::Descriptor)memory[4096]=99;
   if(earlyChange53==Change::Hash){uint32_t h53=456;std::memcpy(model50+0x3c,&h53,4);}
   if(earlyChange53==Change::Index){int16_t n53=6;std::memcpy(memory+8192+0x2e,&n53,2);}
   if(earlyChange53==Change::Range)reinterpret_cast<rage::VehiclePool*>(headers50)[1].m_nSize=0;
   if(earlyChange53==Change::Matrix){uint32_t p53=0;std::memcpy(memory+8192+0x20,&p53,4);}
  }
  if(count==7&&groupCalls50>1){
   if(change==Change::Descriptor)memory[4096]=99;
   if(change==Change::Generation)flags50[0]=8;
   if(change==Change::Revoke)assert(VirtualProtect(memory+4096,4096,PAGE_NOACCESS,&oldProtection));
   if(change==Change::Guard)assert(VirtualProtect(memory+4096,4096,PAGE_READWRITE|PAGE_GUARD,&oldProtection));
   if(change==Change::Index){int16_t index=6;std::memcpy(memory+8192+0x2e,&index,2);}
   if(change==Change::Hash){uint32_t hash=456;std::memcpy(model50+0x3c,&hash,4);}
   if(change==Change::Matrix){uint32_t matrix=0;std::memcpy(memory+8192+0x20,&matrix,4);}
   if(change==Change::Range)reinterpret_cast<rage::VehiclePool*>(headers50)[1].m_nSize=0;
   if(change==Change::Free)ModelLightShadow20::OnFree(uint32_t(uintptr_t(headers50)+sizeof(rage::VehiclePool)),uint32_t(uintptr_t(memory+8192)));
   if(change==Change::Frame)CShadows::frame=8;
  }
  const auto ok=ProductionCopy50(spans,count,fallback,query,fault,rememberMask);
  if(ok&&countBytes50)for(unsigned i=0;i<count;++i)copiedBytes50+=unsigned(spans[i].size);return ok;
 }
}
bool collision79=false;
namespace Liberty20{
 #include "point-copy47-extracted.inc"
 bool Collision(uint32_t) noexcept{return collision79;}
}
namespace rage{struct CLightSource{uint32_t mCastShadows{},mFlags{};};}
namespace PlayerShadowAllocation{constexpr bool shortlistRequested48=false;bool DeferModel48(rage::CLightSource&,unsigned) noexcept{return false;}}
namespace ModelLightShadow20{
 bool requested=true;std::atomic<bool> active{true};std::atomic_flag lock=ATOMIC_FLAG_INIT;
 model_light20::Registry<16> registry;std::atomic<uint32_t> identitiesPublished32{0},refused{0},promoted{0},retired{0},faults{0};
 #include "point-onfree50-extracted.inc"
 #include "ModelPointCommit50Runtime.inc"
 #include "point-promotion47-extracted.inc"
 bool queuedWindow49=false;void ValidateQueuedWindow49(rage::CLightSource*,uint32_t) noexcept{assert(false);}
 #include "queued-permissions47-extracted.inc"
}
namespace ModelEffectObserver{
 using namespace model_effect_observer;
 struct LastErrorScope{DWORD error=GetLastError();~LastErrorScope(){SetLastError(error);}};
 struct Sample{std::array<uint8_t,68> ownerPose109{};Invocation invocation{};uint32_t status{};bool diagnostic=false,ownerMatched=false,modelInfoMatched=false,descriptorCopied=false,matrixCopied=false,packetCopied=false;traffic_signal::Identity identity{};uint32_t ownerKind{},resolveMismatch{},site{},type{},flags{},key{},promotedKey{};std::array<uint8_t,108> descriptor{};std::array<float,16> matrix{};model_owner_transform46::Snapshot ownerTransform{};model_effect_getter62::Snapshot getter62{};std::array<float,3> position{},color{},direction{},up{};float intensity{},radius{},innerCone{},outerCone{};};
 struct Local{model_light_submission20::State state;std::array<Sample,8> samples{};} local;
 std::atomic<bool> enabled{true};std::atomic<uint32_t> readRejected{0},contextRejected{0},nativeInactiveSkipped40{0};bool hotCountersEnabled=true;
 uint32_t fixtureFrame68=7;
 uint32_t Frame() noexcept{return fixtureFrame68;}
 bool Copy(uintptr_t p,void* d,size_t n) noexcept{return Liberty20::Copy(p,d,n);}
 bool InnerEvidence(const SafetyHookContext&,ResolveEvidence&) noexcept{return false;} // Resolve is extracted but not invoked here.
 #include "ModelPointFusion50Runtime.inc"
 #include "point-permissions47-extracted.inc"
 void Queue(const Sample&) noexcept{}
 #include "model-value-post68-extracted.inc"
}
struct Result{unsigned queries{},owners{};uint64_t elapsed{};unsigned batches{},pages{},bytes{};uint32_t flags{},key{};bool invalid{};};
bool gate68=true;
bool Gate68(uint32_t type,uint32_t flags,uint32_t key) noexcept{return gate68&&(type==0||type==2)&&!(flags&6u)&&!key;}
Result Run(bool scoped,Change requestedChange=Change::None,bool keyed=false,bool nested=false,bool fused=false,bool diagnostic50=false,bool corona50=false,uint32_t site50=model_effect_observer::WrapperPoint,unsigned abnormal50=0,bool fast68=false,unsigned outcome68=0,uint32_t inputFlags79=0,uint32_t classification79=0x40){
 CoronaLight20::requested=corona50;
 ModelEffectObserver::pointFusion50=fused;groupCalls50=0;
 using namespace ModelEffectObserver;
 ModelValue68::canReplace=fast68?Gate68:nullptr;
 for(auto& pending68:ModelValue68::pending)pending68={};
 fixtureFrame68=7;
 CShadows::frame=7;
 assert(fp::operation44==nullptr);
 ModelLightShadow20::active=true;
 uint32_t resetHash50=123;std::memcpy(model50+0x3c,&resetHash50,4);reinterpret_cast<rage::VehiclePool*>(headers50)[1].m_nSize=1;
 change=requestedChange;changeAtOwnerRead=2;ownerReads=0;queries=0;queriedPages50=0;copiedBytes50=0;revokeAfterQuery=false;std::memset(memory,0,4096*4);memory[4096]=23;memory[4096+0x5b]=corona50?0:1;flags50[0]=7;
 const int16_t index50=5;std::memcpy(memory+8192+0x2e,&index50,2);auto matrix50=uint32_t(uintptr_t(memory+12288));std::memcpy(memory+8192+0x20,&matrix50,4);
 if(instanceCase114){
  auto* pools109=reinterpret_cast<rage::VehiclePool*>(headers50);pools109[1].m_aStorage=dummyOwner50;
  pools109[2]={dummyOwner50+128,flags50,1,0x80,0,1};
  const auto peer114=uint32_t(uintptr_t(pools109+2));std::memcpy(gameImage.data()+native_owner109::PoolRva,&peer114,4);
  pools109[3]={memory+8192,flags50,1,0x110,0,1};
  const auto root109=uint32_t(uintptr_t(pools109+3));std::memcpy(gameImage.data()+native_owner114::PoolRva,&root109,4);
  const uint32_t vtable109=game+native_owner114::VtableRva,type109=0x40;
  std::memcpy(memory+8192,&vtable109,4);std::memcpy(memory+8192+0x28,&type109,4);
  if(inlineCase109){std::memset(memory+8192+0x20,0,4);for(unsigned j109=0;j109<4;++j109)reinterpret_cast<float*>(memory+8192+0x10)[j109]=float(j109+8);}
 }else if(class5Case109){
  auto* pools109=reinterpret_cast<rage::VehiclePool*>(headers50);pools109[1].m_aStorage=dummyOwner50;
  pools109[2]={memory+8192,flags50,1,0x80,0,1};
  const auto root109=uint32_t(uintptr_t(pools109+2));std::memcpy(gameImage.data()+native_owner109::PoolRva,&root109,4);
  const uint32_t vtable109=game+native_owner109::VtableRva,type109=0x140;
  std::memcpy(memory+8192,&vtable109,4);std::memcpy(memory+8192+0x28,&type109,4);
  if(inlineCase109){std::memset(memory+8192+0x20,0,4);for(unsigned j109=0;j109<4;++j109)reinterpret_cast<float*>(memory+8192+0x10)[j109]=float(j109+8);}
 }else reinterpret_cast<rage::VehiclePool*>(headers50)[1].m_aStorage=memory+8192;
 const auto abi50=model_light_submission20::Layout(site50);const auto keyIndex50=abi50?unsigned(abi50.keyOffset/4):15u;
 auto* args=reinterpret_cast<uint32_t*>(memory);const auto vectors=uint32_t(reinterpret_cast<uintptr_t>(memory+12288));
 if(scattered50){for(unsigned i=0;i<4;++i)for(unsigned j=0;j<3;++j)reinterpret_cast<float*>(vectorPages50+i*4096)[j]=1;}
 args[0]=2;args[1]=abi50.type;args[2]=keyed?4u:inputFlags79;args[3]=vectors;args[4]=vectors+16;args[5]=vectors+32;args[6]=vectors+48;args[7]=0x3f800000;args[10]=0x41200000;args[11]=std::bit_cast<uint32_t>(0.4f);args[12]=std::bit_cast<uint32_t>(0.8f);args[15]=keyed?77u:0u;
 if(scattered50){args[3]=uint32_t(uintptr_t(vectorPages50));args[4]=uint32_t(uintptr_t(vectorPages50+4096));args[5]=uint32_t(uintptr_t(vectorPages50+8192));args[6]=uint32_t(uintptr_t(vectorPages50+12288));}
 auto* floats=reinterpret_cast<float*>(memory+12288);for(unsigned i=0;i<16;++i)floats[i]=1;
#ifdef MODEL_REGION196
 if(crossPage196){for(unsigned i=3;i<=6;++i)args[i]=uint32_t(uintptr_t(vectorPages50+4092));const float crossing[3]{1,1,1};std::memcpy(reinterpret_cast<void*>(args[5]),crossing,sizeof(crossing));}
#endif
 const auto base=uint32_t(reinterpret_cast<uintptr_t>(memory));
 auto* context=reinterpret_cast<uint32_t*>(memory+128);context[0]=game+DispatchReturn;context[1]=2;context[2]=base+12288;
 Invocation invocation{};invocation.frame=7;invocation.nonce=9;invocation.preStack=base+132;invocation.owner=base+8192;invocation.effect=2;invocation.matrix=base+12288;invocation.modelInfo=uint32_t(uintptr_t(model50));invocation.sampled=true;invocation.ordinal=3;invocation.proposedKey=invocation.owner+invocation.ordinal;
 args[keyIndex50]=keyed?invocation.proposedKey:0u;
 local.state={};local.samples={};assert(local.state.Enter(invocation));
 ResolveEvidence resolved{7,base+132,DispatchReturn,2,base+12288,base+8192,2,base+4096,invocation.proposedKey};assert(local.state.Resolve(resolved));local.samples[0].diagnostic=diagnostic50;
 if(diagnostic50)ValidateSource46(*local.state.Current(),local.samples[0]);
 ModelLightShadow20::registry={};assert(ModelLightShadow20::registry.Configure(0x30000000));ModelLightShadow20::lock.clear();
 SafetyHookContext regs{};regs.esp=base;regs.ebp=base+124;regs.edi=base+4096;
 std::optional<fp::Operation44> outer;if(nested)outer.emplace();auto* prior=fp::operation44;
 SetLastError(73);const auto csr=_mm_getcsr();
 const auto batchesBefore50=memory_read46::local.values[unsigned(memory_read46::Metric::BatchHelpers)];
 if(abnormal50==1)args[10]=0x7fc00000;
 if(abnormal50==2)args[7]=std::bit_cast<uint32_t>(-1.f);
 if(abnormal50>=3&&abnormal50<=6)*reinterpret_cast<uint32_t*>(uintptr_t(args[abnormal50]))=0x7fc00000;
 if(abnormal50==7)args[11]=0x7fc00000;
 if(abnormal50==8)args[1]=1;
 if(abnormal50==9)context[0]=0;
 if(abnormal50==10)args[5]=1;
 if(abnormal50==11)floats[12]=std::bit_cast<float>(0x7fc00000u);
 if(abnormal50==12)guardOwnerAfterQuery53=true;
 std::array<uint32_t,18> nativeBefore50{};std::memcpy(nativeBefore50.data(),args,sizeof(nativeBefore50));
 if(cold50){
  for(auto* p:{memory,memory+128,memory+4096,memory+8192,flags50,headers50,model50,memory+12288}){_mm_clflush(p);_mm_clflush(p+64);}
  _mm_clflush(gameImage.data()+0xebd0e8);_mm_clflush(gameImage.data()+0x1232c60);_mm_clflush(gameImage.data()+0xe95cd8+20);
  if(scattered50)for(unsigned i=0;i<4;++i)_mm_clflush(vectorPages50+i*4096);
  _mm_mfence();
 }
 LARGE_INTEGER tickBegin{},tickEnd{};QueryPerformanceCounter(&tickBegin);
 if(scoped)PointScoped47(regs,site50);else PointUnscoped47(regs,site50);
#ifdef MODEL_REGION196
 AfterPoint196();
#endif
 uint32_t finalFlags68=args[2],finalKey68=args[keyIndex50];
 if(fast68&&ModelValue68::pending[0].present&&outcome68!=3){
  const auto slot68=ModelValue68::ReturnSlot(regs.esp,site50);
  ModelValue68::Binding binding68{game+0x6bd194,slot68,slot68+0xa8,args[5],args[6],args[1],abi50.words==16?(args[2]&~8u):args[2],args[keyIndex50]};
  assert(ModelValue68::Matches(binding68));
  // Actual6BCF49/6BCF98/6BCFA0 changes classification after Point. Original
  // scalar remains unchanged; every legal native outcome still binds this
  // exact source and supports capacity fallback. Other flag bits stay exact.
  for(const auto nativeClass77:{0x20u,0x40u,0x60u}){
   auto classified77=binding68;classified77.flags=(classified77.flags&~0x60u)|nativeClass77;
   assert(ModelValue68::Matches(classified77));
  }
  binding68.flags=(binding68.flags&~0x60u)|classification79;
  // Every unrelated source/caller/stack/type/flag/key fails without consuming
  // the pending model source. A correct constructor can still follow it.
  for(unsigned changed68=0;changed68<9;++changed68){auto bad68=binding68;
   switch(changed68){case 0:++bad68.caller;break;case 1:bad68.returnSlot+=16;break;case 2:++bad68.destination;break;
    case 3:++bad68.position;break;case 4:++bad68.color;break;case 5:bad68.type=1;break;
    case 6:bad68.flags^=8;break;case 7:bad68.key=7;break;case 8:fixtureFrame68=8;break;}
   assert(!ModelValue68::Matches(bad68));fixtureFrame68=7;
  }
  if(outcome68==2){
   Invocation child68=invocation;child68.nonce=10;child68.preStack-=32;
   assert(local.state.Enter(child68));assert(!ModelValue68::Matches(binding68));
   Invocation left68{};assert(local.state.Leave(child68.preStack,7,left68)==LeaveResult::Valid);
   assert(ModelValue68::Matches(binding68));
  }
  std::array<uint32_t,32> owned68{};for(unsigned k68=0;k68<owned68.size();++k68)owned68[k68]=0x1000+k68;
  owned68[17]=args[1];owned68[18]=binding68.flags;owned68[24]=0;
  const auto beforeOwned68=owned68;
  if(outcome68==1){const auto key68=ModelValue68::Fallback(owned68,7,binding68);assert(key68==owned68[24]);
   const auto reads68=ownerReads;assert(!ModelValue68::Fallback(owned68,7,binding68));assert(ownerReads==reads68);
  }else {owned68[24]=0x50000000;owned68[18]|=4;}
  for(unsigned k68=0;k68<owned68.size();++k68)if(k68!=18&&k68!=24)assert(owned68[k68]==beforeOwned68[k68]);
  finalFlags68=owned68[18];finalKey68=owned68[24];
  for(unsigned k68=0;k68<18;++k68)assert(args[k68]==nativeBefore50[k68]);
  ModelValue68::Completed(binding68);assert(!ModelValue68::Matches(binding68)&&!ModelValue68::pending[0].present);
 }

 QueryPerformanceCounter(&tickEnd);
 for(unsigned i=0;i<18;++i)if(i!=2&&i!=keyIndex50)assert(args[i]==nativeBefore50[i]);
 assert(fp::operation44==prior&&GetLastError()==73&&_mm_getcsr()==csr);
 if(nested)assert(outer->count==0); // Point does not borrow or publish outer approvals.
 outer.reset();assert(fp::operation44==nullptr);
 Result result{queries,ownerReads,uint64_t(tickEnd.QuadPart-tickBegin.QuadPart),unsigned(memory_read46::local.values[unsigned(memory_read46::Metric::BatchHelpers)]-batchesBefore50),queriedPages50,copiedBytes50,finalFlags68,finalKey68,local.state.Current()->invalid};
 if(abnormal50==12){assert(!guardOwnerAfterQuery53&&groupCalls50==1&&ownerReads==0&&result.invalid);assert(VirtualProtect(memory+8192,4096,PAGE_READWRITE,&oldProtection));}
 if(requestedChange==Change::Revoke||requestedChange==Change::Guard)assert(VirtualProtect(memory+4096,4096,PAGE_READWRITE,&oldProtection));
 // Native caller resumes after the callback: fresh permission observation is
 // mandatory even for a page whose approval was reused inside Point.
 const auto before=queries,regionBefore50=regionCalls50;assert(TrafficSignal::Readable(base,4));assert(queries==before+(fp::preferRegion?0u:1u));
#ifdef MODEL_REGION196
 assert(regionCalls50==regionBefore50+((fp::preferRegion||forceLocked196||failQuery196)?1u:0u));
#else
 assert(regionCalls50==regionBefore50+(fp::preferRegion?1u:0u));
#endif
 return result;
}
Result Queued(bool scoped,Change requestedChange=Change::None){
 using namespace ModelLightShadow20;
 // Populate real registry through accepted production Point before auditing
 // queued validation. Registry and descriptor are reused as native code would.
 const auto accepted=Run(true);change=requestedChange;changeAtOwnerRead=1;ownerReads=0;queries=0;
 if(requestedChange==Change::Revoke){change=Change::None;revokeAfterQuery=true;racedRevocations=0;}
 const auto base=uint32_t(reinterpret_cast<uintptr_t>(memory));
 // Outer positive evidence is deliberately not borrowed by the per-entry
 // scope. Fresh queued reads must not publish approvals into its caller.
 std::optional<fp::Operation44> outer;outer.emplace();
 rage::CLightSource lights[]{ {accepted.key,0x104},{0,0x100} };
 if(scoped)ValidateQueuedScoped47(lights,2);else ValidateQueuedUnscoped47(lights,2);
 assert(fp::operation44==&*outer);
 if(scoped)assert(outer->count==0);
 if(requestedChange==Change::Revoke)assert(racedRevocations==1&&!revokeAfterQuery);
 Result result{queries,ownerReads,0,0,0,0,lights[0].mFlags,lights[0].mCastShadows,false};
 assert(lights[1].mFlags==0x100&&lights[1].mCastShadows==0);
 outer.reset();assert(fp::operation44==nullptr);
 if(requestedChange==Change::Revoke)assert(VirtualProtect(memory+4096,4096,PAGE_READWRITE,&oldProtection));
 const auto before=queries,regionBefore50=regionCalls50;assert(TrafficSignal::Readable(base,4));assert(queries==before+(fp::preferRegion?0u:1u));assert(regionCalls50==regionBefore50+(fp::preferRegion?1u:0u));
 return result;
}
#ifndef MODEL_REGION196
#ifndef CONTAINED_REPLAY236
int main(){
 memory=static_cast<uint8_t*>(VirtualAlloc(nullptr,4096*4,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(memory);
 auto allocate50=[](){auto* p=static_cast<uint8_t*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(p);return p;};
 vectorPages50=static_cast<uint8_t*>(VirtualAlloc(nullptr,4096*4,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(vectorPages50);
 headers50=allocate50();flags50=allocate50();model50=allocate50();dummyOwner50=allocate50();game=uint32_t(uintptr_t(gameImage.data()));
 auto* building50=reinterpret_cast<rage::VehiclePool*>(headers50);auto* object50=building50+1;
 *building50={dummyOwner50,flags50,1,0x70,0,0};*object50={memory+8192,flags50,1,0x320,0,0};
 auto root50=[&](unsigned rva,uint32_t ptr){std::memcpy(gameImage.data()+rva,&ptr,4);};
 root50(0xebd0e8,uint32_t(uintptr_t(building50)));root50(0x1232c60,uint32_t(uintptr_t(object50)));root50(0xe95cd8+5*4,uint32_t(uintptr_t(model50)));
 uint32_t hash50=123;std::memcpy(model50+0x3c,&hash50,4);

 memory_read46::Configure(true);countBytes50=true;
 unsigned oldOwners68=0,fastOwners68=0,oldQueries68=0,fastQueries68=0,fusedGroups68=0;
 for(unsigned source68=0;source68<500;++source68){
  const auto old68=Run(true,Change::None,false,false,false);assert(old68.flags==4&&old68.key);oldOwners68+=old68.owners;oldQueries68+=old68.queries;
  const auto fused68=Run(true,Change::None,false,false,true);assert(fused68.flags==4&&fused68.key);fusedGroups68+=groupCalls50;
  const auto fast68=Run(true,Change::None,false,false,true,false,false,model_effect_observer::WrapperPoint,0,true,source68==499?1:0);
  assert(fast68.flags==0x44&&fast68.key);fastOwners68+=fast68.owners;fastQueries68+=fast68.queries;
 }
 std::fprintf(stderr,"owners old=%u fast=%u current_fused_heavy_groups=%u\n",oldOwners68,fastOwners68,fusedGroups68);
 assert(oldOwners68==1000&&fastOwners68==0&&fusedGroups68==1500);
 std::printf("79 production Point + fused key-only fallback: 500 sources, 499 handled normalized, old_owner_reads=%u candidate_scalar_owner_reads=%u old_QWS=%u candidate_QWS=%u\n",oldOwners68,fastOwners68,oldQueries68,fastQueries68);
 for(auto site68:{model_effect_observer::WrapperPoint,model_effect_observer::DirectPoint,model_light_submission20::WrapperSpot,model_light_submission20::DirectSpot}){
  const auto fast68=Run(true,Change::None,false,false,true,false,false,site68,0,true);assert(!fast68.owners&&fast68.flags==0x44);
  const auto oldFall79=Run(true,Change::None,false,false,false,false,false,site68,0,true,1);assert(oldFall79.owners==2&&oldFall79.flags==0x44&&oldFall79.key==0x30000000);
  const auto fall68=Run(true,Change::None,false,false,true,false,false,site68,0,true,1);assert(fall68.owners==0&&groupCalls50==3&&fall68.flags==oldFall79.flags&&fall68.key==oldFall79.key);
  // Native classification bits and FF511 metadata remain native output. Both
  // variants operate on the full local packet, and Run checks every other
  // word (including p28/p29 photometry) and the entire original ABI stack.
  for(const auto flags79:{0u,8u,0x100u,0x108u})for(const auto class79:{0x20u,0x40u,0x60u}){
   const auto old79=Run(true,Change::None,false,false,false,false,false,site68,0,true,1,flags79,class79);
   const auto fused79=Run(true,Change::None,false,false,true,false,false,site68,0,true,1,flags79,class79);
   assert(old79.key==0x30000000&&fused79.key==old79.key&&fused79.flags==old79.flags);
   assert(old79.owners==2&&fused79.owners==0&&groupCalls50==3);
  }
  const auto nested68=Run(true,Change::None,false,false,true,false,false,site68,0,true,2);assert(!nested68.owners&&nested68.flags==0x44);
  gate68=false;const auto inactive68=Run(true,Change::None,false,false,true,false,false,site68,0,true);gate68=true;assert(groupCalls50==3&&inactive68.key==0x30000000);
  const auto diag68=Run(true,Change::None,false,false,true,true,false,site68,0,true);assert(diag68.owners&&diag68.key==0x30000000);
  for(auto fault68:{Change::Descriptor,Change::Generation,Change::Revoke,Change::Guard,Change::Index,Change::Hash,Change::Matrix,Change::Range,Change::Free,Change::Frame}){
   const auto faultFallback68=Run(true,fault68,false,false,true,false,false,site68,0,true,1);assert(faultFallback68.flags==0x40&&!faultFallback68.key&&faultFallback68.owners==0);
  }
  for(unsigned invalid79=1;invalid79<=12;++invalid79){
   const auto invalidFallback79=Run(true,Change::None,false,false,true,false,false,site68,invalid79,true,1);
   if(invalid79==7&&model_light_submission20::Layout(site68).type==0){
    // Point projection does not consume spot cones; keep the established
    // successful point behavior instead of inventing a new light refusal.
    assert(invalidFallback79.key==0x30000000&&(invalidFallback79.flags&4u));
   }else assert(!invalidFallback79.key&&!(invalidFallback79.flags&4u));
  }
  const auto keyed68=Run(true,Change::None,true,false,true,false,false,site68,0,true);assert(!keyed68.owners&&keyed68.flags==4);
  // Category disabled retains the actual old native illumination/eligibility path.
  ModelLightShadow20::requested=false;const auto category68=Run(true,Change::None,false,false,true,false,false,site68,0,true);ModelLightShadow20::requested=true;
  assert(!category68.flags&&!category68.key);
  collision79=true;
  const auto collisionOld79=Run(true,Change::None,false,false,false,false,false,site68,0,true,1);
  const auto collisionFused79=Run(true,Change::None,false,false,true,false,false,site68,0,true,1);
  collision79=false;
  assert(!collisionOld79.key&&!collisionFused79.key&&collisionOld79.flags==collisionFused79.flags);
 }
 // Whole synchronous Point + constructor fallback, actual extracted legacy
 // RegisterKey68 against actual fused owner/descriptor/commit code. Both use
 // real QueryWorkingSetEx and SEH; only the native ctor association boundary
 // is supplied by the fixture. Time excludes allocation/reset and includes
 // every counted callback permission query, association and final tail.
 LARGE_INTEGER frequency79{};assert(QueryPerformanceFrequency(&frequency79));
 for(const auto scenario79:{0u,1u,2u}){
  cold50=scenario79!=0;scattered50=scenario79==2;
  std::vector<uint64_t> oldTimes79,fusedTimes79;
  unsigned oldQueries79=0,fusedQueries79=0,oldPages79=0,fusedPages79=0;
  unsigned oldBytes79=0,fusedBytes79=0,oldOwners79=0,fusedOwners79=0;
  for(unsigned iteration79=0;iteration79<288;++iteration79){
   Result old79{},fused79{};
   // Alternate ordering to limit systematic warmed-page/timer bias.
   if(iteration79&1u){fused79=Run(true,Change::None,false,false,true,false,false,model_effect_observer::WrapperPoint,0,true,1);old79=Run(true,Change::None,false,false,false,false,false,model_effect_observer::WrapperPoint,0,true,1);}
   else {old79=Run(true,Change::None,false,false,false,false,false,model_effect_observer::WrapperPoint,0,true,1);fused79=Run(true,Change::None,false,false,true,false,false,model_effect_observer::WrapperPoint,0,true,1);}
   assert(fused79.key==old79.key&&fused79.flags==old79.flags&&old79.owners==2&&fused79.owners==0);
   if(iteration79<32)continue;
   oldTimes79.push_back(old79.elapsed);fusedTimes79.push_back(fused79.elapsed);
   oldQueries79+=old79.queries;fusedQueries79+=fused79.queries;
   oldPages79+=old79.pages;fusedPages79+=fused79.pages;
   oldBytes79+=old79.bytes;fusedBytes79+=fused79.bytes;
   oldOwners79+=old79.owners;fusedOwners79+=fused79.owners;
  }
  std::sort(oldTimes79.begin(),oldTimes79.end());std::sort(fusedTimes79.begin(),fusedTimes79.end());
  const auto us79=[&](uint64_t ticks79){return double(ticks79)*1000000.0/double(frequency79.QuadPart);};
  std::printf("79 whole-fallback scenario=%s n=256 old_QWS=%u fused_QWS=%u old_pages=%u fused_pages=%u old_bytes=%u fused_bytes=%u old_scalar_owner=%u fused_scalar_owner=%u old_p50_us=%.3f fused_p50_us=%.3f old_p95_us=%.3f fused_p95_us=%.3f\n",
   scenario79==0?"warm_dense":scenario79==1?"cold_dense":"cold_scattered",oldQueries79,fusedQueries79,oldPages79,fusedPages79,oldBytes79,fusedBytes79,oldOwners79,fusedOwners79,
   us79(oldTimes79[128]),us79(fusedTimes79[128]),us79(oldTimes79[243]),us79(fusedTimes79[243]));
  assert(oldOwners79==512&&fusedOwners79==0);
 }
 cold50=false;scattered50=false;
 // A native corona descriptor deliberately retains the old reconstruction
 // path. This optimization cannot suppress authored special categories.
 {
  const auto old79=Run(true,Change::None,false,false,false,false,true,model_effect_observer::WrapperPoint,0,true,1);
  const auto fused79=Run(true,Change::None,false,false,true,false,true,model_effect_observer::WrapperPoint,0,true,1);
  assert(old79.key==fused79.key&&old79.flags==fused79.flags&&fused79.owners==old79.owners);
 }
 // The actual native dispatcher Post clears a source whose store did not
 // reach the constructor, including invalid native-return correlation.
 for(bool invalidPost68:{false,true}){
  (void)Run(true,Change::None,false,false,true,false,false,model_effect_observer::WrapperPoint,0,true,3);
  assert(ModelEffectObserver::ModelValue68::pending[0].present);
  SafetyHookContext post68{};post68.esp=ModelEffectObserver::local.state.Current()->preStack+model_effect_observer::PostStackAdjustment+(invalidPost68?1u:0u);
  ModelEffectObserver::Post(post68);
  assert(!ModelEffectObserver::ModelValue68::pending[0].present);
 }
 // A changed invocation nonce cannot reuse even an identical native stack.
 (void)Run(true,Change::None,false,false,true,false,false,model_effect_observer::WrapperPoint,0,true,3);
 {using namespace ModelEffectObserver;auto& p68=ModelValue68::pending[0];const auto slot68=ModelValue68::ReturnSlot(p68.regs.esp,p68.site);
  ModelValue68::Binding b68{game+0x6bd194,slot68,slot68+0xa8,p68.arguments[5],p68.arguments[6],p68.arguments[1],p68.arguments[2]&~8u,0};
  ++local.state.Current()->nonce;assert(!ModelValue68::Matches(b68));ModelValue68::ClearDepth(1);
 }
 class5Case109=true;ownerClass109Active=true;
 for(bool inline109:{false,true})for(auto site109:{model_effect_observer::WrapperPoint,model_effect_observer::DirectPoint,model_light_submission20::WrapperSpot,model_light_submission20::DirectSpot}){
  inlineCase109=inline109;
  const auto fallback109=Run(true,Change::None,false,false,true,false,false,site109,0,true,1);
  assert(fallback109.key==0x30000000&&(fallback109.flags&4u)&&ModelEffectObserver::local.samples[0].ownerKind==2);
  const auto successfulValue109=Run(true,Change::None,false,false,true,false,false,site109,0,true,0);
  assert(successfulValue109.key==0x50000000&&!successfulValue109.owners); // value-owned success still avoids owner preparation
 }
 ownerClass109Active=false;class5Case109=false;inlineCase109=false;
 instanceCase114=true;ownerClass114Active=true;
 for(bool inline114:{false,true})for(auto site114:{model_effect_observer::WrapperPoint,model_effect_observer::DirectPoint,model_light_submission20::WrapperSpot,model_light_submission20::DirectSpot}){
  inlineCase109=inline114;
  const auto fallback114=Run(true,Change::None,false,false,true,false,false,site114,0,true,1);
  assert(fallback114.key==0x30000000&&(fallback114.flags&4u)&&ModelEffectObserver::local.samples[0].ownerKind==3);
  const auto successfulValue114=Run(true,Change::None,false,false,true,false,false,site114,0,true,0);
  assert(successfulValue114.key==0x50000000&&!successfulValue114.owners); // value-owned success still avoids owner preparation
 }
 ownerClass114Active=false;instanceCase114=false;inlineCase109=false;
 ModelEffectObserver::ModelValue68::canReplace=nullptr;
 assert(fp::operation44==nullptr);for(auto q68:{headers50,flags50,model50,dummyOwner50,vectorPages50})assert(VirtualFree(q68,0,MEM_RELEASE));assert(VirtualFree(memory,0,MEM_RELEASE));
 std::puts("68 extracted production OFF/ON/nonactive/category/fallback/reentrant/frame/ABI/owner-fault/native-args checks passed; simulated66 boundary, no live FPS claim");
}
#endif
#endif // CONTAINED_REPLAY236
