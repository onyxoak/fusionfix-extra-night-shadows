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
BOOL WINAPI CountQuery(HANDLE process,PVOID bytes,DWORD size){++queries;queriedPages50+=size/sizeof(PSAPI_WORKING_SET_EX_INFORMATION);const auto ok=RealQuery(process,bytes,size);AfterQuery(bytes,size,ok);return ok;}
bool Region(uintptr_t address,size_t size) noexcept {
 ++regionCalls50; MEMORY_BASIC_INFORMATION m{};if(!VirtualQuery(reinterpret_cast<void*>(address),&m,sizeof(m)))return false;
 return m.State==MEM_COMMIT&&!(m.Protect&(PAGE_GUARD|PAGE_NOACCESS))&&address>=uintptr_t(m.BaseAddress)&&size<=m.RegionSize-(address-uintptr_t(m.BaseAddress));
}
struct SafetyHookContext{uint32_t eflags{},esp{},ebp{},edi{};};
#include "core/ShadowFloatingPointState.hpp"
namespace CoronaLight20{bool requested=false;}
namespace PlayerShadowAllocation{bool Enabled() noexcept{return true;}}
std::atomic<bool> addonActive{true};uint32_t game=0x400000;
namespace CShadows{uint32_t frame=7;uint32_t* pFrameCounter=&frame;}
enum class Change{None,Descriptor,Generation,Revoke,Guard,Index,Hash,Matrix,Range,Free};
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
bool instanceCase114=false,class5Case109=false,inlineCase109=false,mutatePose109=false;
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
  if((class5Case109||instanceCase114)&&mutatePose109&&ownerReads==2){
   auto* pose109=reinterpret_cast<float*>(inlineCase109?memory+8192+0x10:memory+12288);pose109[0]+=0.125f;
  }
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
  }
  const auto ok=ProductionCopy50(spans,count,fallback,query,fault,rememberMask);
  if(ok&&countBytes50)for(unsigned i=0;i<count;++i)copiedBytes50+=unsigned(spans[i].size);return ok;
 }
}
namespace Liberty20{
 #include "point-copy47-extracted.inc"
 bool Collision(uint32_t) noexcept{return false;}
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
 struct Sample{std::array<uint8_t,68> ownerPose109{};bool diagnostic=false,ownerMatched=false,modelInfoMatched=false,descriptorCopied=false,matrixCopied=false,packetCopied=false;traffic_signal::Identity identity{};uint32_t ownerKind{},resolveMismatch{},site{},type{},flags{},key{},promotedKey{};std::array<uint8_t,108> descriptor{};std::array<float,16> matrix{};model_owner_transform46::Snapshot ownerTransform{};model_effect_getter62::Snapshot getter62{};std::array<float,3> position{},color{},direction{},up{};float intensity{},radius{},innerCone{},outerCone{};};
 struct Local{model_light_submission20::State state;std::array<Sample,8> samples{};} local;
 std::atomic<bool> enabled{true};std::atomic<uint32_t> readRejected{0},contextRejected{0},nativeInactiveSkipped40{0};bool hotCountersEnabled=true;
 uint32_t Frame() noexcept{return 7;}
 bool Copy(uintptr_t p,void* d,size_t n) noexcept{return Liberty20::Copy(p,d,n);}
 bool InnerEvidence(const SafetyHookContext&,ResolveEvidence&) noexcept{return false;} // Resolve is extracted but not invoked here.
 #include "ModelPointFusion50Runtime.inc"
 #include "point-permissions47-extracted.inc"
}
struct Result{unsigned queries{},owners{};uint64_t elapsed{};unsigned batches{},pages{},bytes{};uint32_t flags{},key{};bool invalid{};};
Result Run(bool scoped,Change requestedChange=Change::None,bool keyed=false,bool nested=false,bool fused=false,bool diagnostic50=false,bool corona50=false,uint32_t site50=model_effect_observer::WrapperPoint,unsigned abnormal50=0){
 CoronaLight20::requested=corona50;
 ModelEffectObserver::pointFusion50=fused;groupCalls50=0;
 using namespace ModelEffectObserver;
 assert(fp::operation44==nullptr);
 ModelLightShadow20::active=true;
 uint32_t resetHash50=123;std::memcpy(model50+0x3c,&resetHash50,4);reinterpret_cast<rage::VehiclePool*>(headers50)[1].m_nSize=1;
 change=requestedChange;changeAtOwnerRead=2;ownerReads=0;queries=0;queriedPages50=0;copiedBytes50=0;revokeAfterQuery=false;std::memset(memory,0,4096*4);memory[4096]=23;memory[4096+0x5b]=corona50?0:1;flags50[0]=7;
 const int16_t index50=5;std::memcpy(memory+8192+0x2e,&index50,2);auto matrix50=uint32_t(uintptr_t(memory+12288));std::memcpy(memory+8192+0x20,&matrix50,4);
 if(instanceCase114){
  auto* pools109=reinterpret_cast<rage::VehiclePool*>(headers50);
  pools109[1].m_aStorage=dummyOwner50;
  pools109[2]={dummyOwner50+128,flags50,1,0x80,0,1};
  const auto peer114=uint32_t(uintptr_t(pools109+2));std::memcpy(gameImage.data()+native_owner109::PoolRva,&peer114,4);
  pools109[3]={memory+8192,flags50,1,0x110,0,1};
  const auto root109=uint32_t(uintptr_t(pools109+3));std::memcpy(gameImage.data()+native_owner114::PoolRva,&root109,4);
  const uint32_t vtable109=game+native_owner114::VtableRva,type109=0x40;
  std::memcpy(memory+8192,&vtable109,4);std::memcpy(memory+8192+0x28,&type109,4);
  if(inlineCase109){std::memset(memory+8192+0x20,0,4);for(unsigned j109=0;j109<4;++j109)reinterpret_cast<float*>(memory+8192+0x10)[j109]=float(j109+8);}
 }else if(class5Case109){
  auto* pools109=reinterpret_cast<rage::VehiclePool*>(headers50);
  pools109[1].m_aStorage=dummyOwner50;
  pools109[2]={memory+8192,flags50,1,0x80,0,1};
  const auto root109=uint32_t(uintptr_t(pools109+2));std::memcpy(gameImage.data()+native_owner109::PoolRva,&root109,4);
  const uint32_t vtable109=game+native_owner109::VtableRva,type109=0x140;
  std::memcpy(memory+8192,&vtable109,4);std::memcpy(memory+8192+0x28,&type109,4);
  if(inlineCase109){std::memset(memory+8192+0x20,0,4);for(unsigned j109=0;j109<4;++j109)reinterpret_cast<float*>(memory+8192+0x10)[j109]=float(j109+8);}
 }else reinterpret_cast<rage::VehiclePool*>(headers50)[1].m_aStorage=memory+8192;
 const auto abi50=model_light_submission20::Layout(site50);const auto keyIndex50=abi50?unsigned(abi50.keyOffset/4):15u;
 auto* args=reinterpret_cast<uint32_t*>(memory);const auto vectors=uint32_t(reinterpret_cast<uintptr_t>(memory+12288));
 if(scattered50){for(unsigned i=0;i<4;++i)for(unsigned j=0;j<3;++j)reinterpret_cast<float*>(vectorPages50+i*4096)[j]=1;}
 args[0]=2;args[1]=abi50.type;args[2]=keyed?4u:0u;args[3]=vectors;args[4]=vectors+16;args[5]=vectors+32;args[6]=vectors+48;args[7]=0x3f800000;args[10]=0x41200000;args[15]=keyed?77u:0u;
 if(scattered50){args[3]=uint32_t(uintptr_t(vectorPages50));args[4]=uint32_t(uintptr_t(vectorPages50+4096));args[5]=uint32_t(uintptr_t(vectorPages50+8192));args[6]=uint32_t(uintptr_t(vectorPages50+12288));}
 auto* floats=reinterpret_cast<float*>(memory+12288);for(unsigned i=0;i<16;++i)floats[i]=1;
 const auto base=uint32_t(reinterpret_cast<uintptr_t>(memory));
 auto* context=reinterpret_cast<uint32_t*>(memory+128);context[0]=game+DispatchReturn;context[1]=2;context[2]=base+12288;
 Invocation invocation{};invocation.frame=7;invocation.preStack=base+132;invocation.owner=base+8192;invocation.effect=2;invocation.matrix=base+12288;invocation.modelInfo=uint32_t(uintptr_t(model50));invocation.sampled=true;invocation.ordinal=3;invocation.proposedKey=invocation.owner+invocation.ordinal;
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
 QueryPerformanceCounter(&tickEnd);
 for(unsigned i=0;i<18;++i)if(i!=2&&i!=keyIndex50)assert(args[i]==nativeBefore50[i]);
 assert(fp::operation44==prior&&GetLastError()==73&&_mm_getcsr()==csr);
 if(nested)assert(outer->count==0); // Point does not borrow or publish outer approvals.
 outer.reset();assert(fp::operation44==nullptr);
 Result result{queries,ownerReads,uint64_t(tickEnd.QuadPart-tickBegin.QuadPart),unsigned(memory_read46::local.values[unsigned(memory_read46::Metric::BatchHelpers)]-batchesBefore50),queriedPages50,copiedBytes50,args[2],args[keyIndex50],local.state.Current()->invalid};
 if(abnormal50==12){assert(!guardOwnerAfterQuery53&&groupCalls50==1&&ownerReads==0&&result.invalid);assert(VirtualProtect(memory+8192,4096,PAGE_READWRITE,&oldProtection));}
 if(requestedChange==Change::Revoke||requestedChange==Change::Guard)assert(VirtualProtect(memory+4096,4096,PAGE_READWRITE,&oldProtection));
 // Native caller resumes after the callback: fresh permission observation is
 // mandatory even for a page whose approval was reused inside Point.
 const auto before=queries,regionBefore50=regionCalls50;assert(TrafficSignal::Readable(base,4));assert(queries==before+(fp::preferRegion?0u:1u));assert(regionCalls50==regionBefore50+(fp::preferRegion?1u:0u));
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
 memory_read46::Configure(true);
 const auto unscoped=Run(false),scoped=Run(true);
 const auto fusedResult50=Run(true,Change::None,false,false,true);
 assert(fusedResult50.flags==scoped.flags&&fusedResult50.key==scoped.key&&groupCalls50==3);
 std::printf("Fused50 actual queries=%u batches=%u groups=%u versus49 queries=%u batches=%u\n",fusedResult50.queries,fusedResult50.batches,groupCalls50,scoped.queries,scoped.batches);
 std::printf("Point49 actual batch helpers: unscoped=%u scoped=%u\n",unscoped.batches,scoped.batches);
 assert(unscoped.flags==scoped.flags&&unscoped.key==scoped.key&&scoped.flags==4&&scoped.key==0x30000000);
 assert(scoped.owners==2&&unscoped.owners==2&&scoped.queries<unscoped.queries);
 std::printf("accepted production Point/Promote: QWS calls %u -> %u; fresh owner reads %u -> %u\n",unscoped.queries,scoped.queries,unscoped.owners,scoped.owners);
 const auto rejectedBaseline=Run(false,Change::None,true),rejectedScoped=Run(true,Change::None,true);
 assert(rejectedBaseline.flags==rejectedScoped.flags&&rejectedBaseline.key==rejectedScoped.key&&rejectedScoped.owners==0);
 for(const auto scenario:{Change::Descriptor,Change::Generation,Change::Revoke}){
  const auto a=Run(false,scenario),b=Run(true,scenario);
  const auto fusion50=Run(true,scenario,false,false,true);assert(fusion50.flags==0&&fusion50.key==0);
  assert(a.flags==0&&b.flags==0&&a.key==0&&b.key==0&&a.owners==2&&b.owners==2);
  std::printf("fresh final-check refusal scenario%u: QWS %u -> %u\n",unsigned(scenario),a.queries,b.queries);
 }
 for(const auto scenario50:{Change::Guard,Change::Index,Change::Hash,Change::Matrix,Change::Range,Change::Free}){
  const auto r=Run(true,scenario50,false,false,true);std::fprintf(stderr,"scenario=%u flags=%u key=%u used=%u active=%u\n",unsigned(scenario50),r.flags,r.key,ModelLightShadow20::registry.used,unsigned(ModelLightShadow20::active.load()));assert(r.flags==0&&r.key==0&&ModelLightShadow20::registry.used==0);
 }
 for(auto site50:{model_effect_observer::WrapperPoint,model_effect_observer::DirectPoint,model_light_submission20::WrapperSpot,model_light_submission20::DirectSpot}){
  const auto result50=Run(true,Change::None,false,false,true,false,false,site50);assert(result50.flags==4&&result50.key);
  const auto keyed50=Run(true,Change::None,true,false,true,false,false,site50);assert(keyed50.flags==4&&keyed50.key&&groupCalls50==0);
  for(unsigned bad50=1;bad50<=10;++bad50){if(bad50==7&&model_light_submission20::Layout(site50).type!=2)continue;
   const auto refused50=Run(true,Change::None,false,false,true,false,false,site50,bad50);assert(refused50.flags==0&&refused50.key==0);}
 }
 const auto invalidSite50=Run(true,Change::None,false,false,true,false,false,1);assert(invalidSite50.flags==0&&!invalidSite50.key);
 for(const auto early53:{Change::Descriptor,Change::Hash,Change::Index,Change::Range,Change::Matrix}){
  mutatePhase53=2;earlyChange53=early53;
  const auto result53=Run(true,Change::None,false,false,true);assert(!result53.flags&&!result53.key&&ModelLightShadow20::registry.used==0);
 }
 mutatePhase53=0;earlyChange53=Change::None;
 scattered50=true;
 const auto nonfiniteOwner53=Run(true,Change::None,false,false,true,false,false,model_effect_observer::WrapperPoint,11);
 assert(!nonfiniteOwner53.flags&&!nonfiniteOwner53.key);
 scattered50=false;
 const auto guardOwner53=Run(true,Change::None,false,false,true,false,false,model_effect_observer::WrapperPoint,12);
 assert(!guardOwner53.flags&&!guardOwner53.key&&ModelLightShadow20::registry.used==0);
 // Twelve independent resident pages exercise the expanded group capacity.
 // All output is transactional; a stage fault at any span publishes nothing.
 {
  std::array<std::array<uint8_t,4>,12> output53{};
  uint8_t* pages53[]{memory,memory+4096,memory+8192,memory+12288,headers50,flags50,model50,dummyOwner50,
    vectorPages50,vectorPages50+4096,vectorPages50+8192,vectorPages50+12288};
  traffic_signal::fresh_group50::Span spans53[12]{};
  for(unsigned i53=0;i53<12;++i53)spans53[i53]={uintptr_t(pages53[i53]),output53[i53].data(),4};
  bool fault53=false;change=Change::None;groupCalls50=0;
  assert(traffic_signal::fresh_group50::Copy(spans53,12,Region,CountQuery,&fault53)&&!fault53);
  assert(!traffic_signal::fresh_group50::Copy(spans53,13,Region,CountQuery,&fault53)&&!fault53);
  for(auto& out53:output53)out53.fill(0x5a);
  assert(VirtualProtect(vectorPages50+12288,4096,PAGE_READWRITE|PAGE_GUARD,&oldProtection));
  // Remembered approvals are local only, and never defeat contained reads.
  fp::Operation44 operation53;fp::Remember44(uintptr_t(vectorPages50+12288),fp::PageSize(),CountQuery);
  assert(!traffic_signal::fresh_group50::Copy(spans53,12,Region,CountQuery,&fault53)&&fault53);
  for(const auto& out53:output53)for(auto value53:out53)assert(value53==0x5a);
  assert(VirtualProtect(vectorPages50+12288,4096,PAGE_READWRITE,&oldProtection));
 }
 const auto diagnosticResult50=Run(true,Change::None,false,false,true,true);
 assert(diagnosticResult50.flags==4&&diagnosticResult50.key&&groupCalls50==0&&ownerReads==3); // diagnostic owner transform adds its own fresh identity check
 const auto coronaResult50=Run(true,Change::None,false,false,true,false,true);
 assert(coronaResult50.flags==4&&coronaResult50.key&&groupCalls50==1&&ownerReads==2);
 const auto nested=Run(true,Change::None,false,true);assert(nested.key==scoped.key&&nested.queries==scoped.queries);
 const auto queuedA=Queued(false),queuedB=Queued(true);
 assert(queuedA.flags==queuedB.flags&&queuedB.flags==0x104&&queuedA.key==queuedB.key&&queuedA.owners==1&&queuedB.owners==1);
 std::printf("queued single-entry (separate owner/descriptor pages): QWS %u -> %u\n",queuedA.queries,queuedB.queries);
 for(const auto scenario:{Change::Descriptor,Change::Generation,Change::Revoke}){
  const auto a=Queued(false,scenario),b=Queued(true,scenario);
  assert(a.flags==0x100&&b.flags==0x100&&a.key==b.key&&a.owners==1&&b.owners==1);
  std::printf("queued freshness refusal scenario%u: QWS %u -> %u\n",unsigned(scenario),a.queries,b.queries);
 }
 // Region strategy keeps the exact established Point route. The new group
 // primitive independently honors current-region requests without any QWS.
 fp::Configure(true);change=Change::None;groupCalls50=0;queries=0;
 std::array<uint8_t,8> regionOutput50{};const traffic_signal::fresh_group50::Span regionSpan50{uintptr_t(memory+4096),regionOutput50.data(),regionOutput50.size()};
 bool regionFault50=false;assert(traffic_signal::fresh_group50::Copy(&regionSpan50,1,Region,CountQuery,&regionFault50));assert(!queries&&!regionFault50);
 const auto regionLegacy50=Run(true),regionRequestedFusion50=Run(true,Change::None,false,false,true);
 assert(!groupCalls50&&regionLegacy50.flags==regionRequestedFusion50.flags&&regionLegacy50.key==regionRequestedFusion50.key&&regionLegacy50.queries==regionRequestedFusion50.queries&&regionLegacy50.batches==regionRequestedFusion50.batches);
 fp::Configure(false);
 memory_read46::Configure(false);
 LARGE_INTEGER freq50{};QueryPerformanceFrequency(&freq50);
 for(bool layout50:{false,true}){
  scattered50=layout50;memory_read46::Configure(true);
  ModelEffectObserver::eagerGroup53=false;countBytes50=true;const auto a50=Run(true,Change::None,false,false,true);
  ModelEffectObserver::eagerGroup53=true;const auto b50=Run(true,Change::None,false,false,true);countBytes50=false;
  assert(a50.flags==b50.flags&&a50.key==b50.key&&b50.batches+1==a50.batches);
  std::printf("Actual layout=%u baseline51 QWS=%u/helpers=%u/pages=%u/bytes=%u eager53 QWS=%u/helpers=%u/pages=%u/bytes=%u\n",unsigned(layout50),a50.queries,a50.batches,a50.pages,a50.bytes,b50.queries,b50.batches,b50.pages,b50.bytes);
  memory_read46::Configure(false);
 }
 for(bool layout50:{false,true})for(bool coldCache50:{false,true})for(bool profile50:{false,true}){
  scattered50=layout50;cold50=coldCache50;shadow_cpu29::Configure(profile50);
  std::array<std::vector<uint64_t>,2> ticks50;
  for(unsigned i=0;i<3000;++i)for(unsigned order50=0;order50<2;++order50){
   const unsigned fused50=(i+order50)&1u;ModelEffectObserver::eagerGroup53=fused50!=0;const auto r=Run(true,Change::None,false,false,true);assert(r.flags==4&&r.key);
   if(i>=100)ticks50[fused50].push_back(r.elapsed);
  }
  for(unsigned fused50=0;fused50<2;++fused50){auto& values50=ticks50[fused50];std::sort(values50.begin(),values50.end());
   std::printf("Point real matcher scattered=%u cold=%u fused=%u profile=%u cpu_us median=%.3f p95=%.3f calls=%zu interleaved=1\n",unsigned(layout50),unsigned(coldCache50),fused50,unsigned(profile50),double(values50[values50.size()/2])*1e6/double(freq50.QuadPart),double(values50[values50.size()*95/100])*1e6/double(freq50.QuadPart),values50.size());
  }
 }
 // Actual production Point -> fused rejection -> complete legacy owner and
 // descriptor validation -> guarded key-only publication for native class5.
 class5Case109=true;ownerClass109Active=false;
 const auto refusedClass109=Run(true,Change::None,false,false,true);assert(!refusedClass109.flags&&!refusedClass109.key);
 ownerClass109Active=true;
 for(bool inline109:{false,true})for(auto site109:{model_effect_observer::WrapperPoint,model_effect_observer::DirectPoint,model_light_submission20::WrapperSpot,model_light_submission20::DirectSpot}){
  inlineCase109=inline109;
  const auto adopted109=Run(true,Change::None,false,false,true,false,false,site109);
  assert(adopted109.flags==4&&adopted109.key==0x30000000&&ModelEffectObserver::local.samples[0].ownerKind==2);
  auto* entry109=ModelLightShadow20::registry.Lookup(adopted109.key);assert(entry109&&entry109->active);
  assert(entry109->id.pool==uint32_t(uintptr_t(reinterpret_cast<rage::VehiclePool*>(headers50)+2)));
  const auto queued109=Queued(true);assert(queued109.flags==0x104&&queued109.key==adopted109.key);
  ModelLightShadow20::OnFree(entry109->id.pool,entry109->id.owner);
  rage::CLightSource retired109{adopted109.key,0x104};assert(!ModelLightShadow20::ValidateOneScoped48(retired109)&&retired109.mFlags==0x100);
  for(auto refusal109:{Change::Descriptor,Change::Generation}){
   const auto changed109=Run(true,refusal109,false,false,true,false,false,site109);
   assert(!changed109.flags&&!changed109.key);
  }
  mutatePose109=true;const auto changedPose109=Run(true,Change::None,false,false,true,false,false,site109);mutatePose109=false;
  assert(!changedPose109.flags&&!changedPose109.key);
 }
 ownerClass109Active=false;class5Case109=false;inlineCase109=false;
 {instanceCase114=true;ownerClass114Active=false;
 const auto refusedClass114=Run(true,Change::None,false,false,true);assert(!refusedClass114.flags&&!refusedClass114.key);
 ownerClass114Active=true;
 for(bool inline114:{false,true})for(auto site114:{model_effect_observer::WrapperPoint,model_effect_observer::DirectPoint,model_light_submission20::WrapperSpot,model_light_submission20::DirectSpot}){
  inlineCase109=inline114;
  const auto adopted114=Run(true,Change::None,false,false,true,false,false,site114);
  assert(adopted114.flags==4&&adopted114.key==0x30000000&&ModelEffectObserver::local.samples[0].ownerKind==3);
  auto* entry114=ModelLightShadow20::registry.Lookup(adopted114.key);assert(entry114&&entry114->active);
  assert(entry114->id.pool==uint32_t(uintptr_t(reinterpret_cast<rage::VehiclePool*>(headers50)+3)));
  const auto queued114=Queued(true);assert(queued114.flags==0x104&&queued114.key==adopted114.key);
  ModelLightShadow20::OnFree(entry114->id.pool,entry114->id.owner);
  rage::CLightSource retired114{adopted114.key,0x104};assert(!ModelLightShadow20::ValidateOneScoped48(retired114)&&retired114.mFlags==0x100);
  for(auto refusal114:{Change::Descriptor,Change::Generation}){
   const auto changed114=Run(true,refusal114,false,false,true,false,false,site114);
   assert(!changed114.flags&&!changed114.key);
  }
  mutatePose109=true;const auto changedPose114=Run(true,Change::None,false,false,true,false,false,site114);mutatePose109=false;
  assert(!changedPose114.flags&&!changedPose114.key);
 }
 ownerClass114Active=false;instanceCase114=false;inlineCase109=false;}
 assert(fp::operation44==nullptr);for(auto p:{headers50,flags50,model50,dummyOwner50,vectorPages50})assert(VirtualFree(p,0,MEM_RELEASE));assert(VirtualFree(memory,0,MEM_RELEASE));
 std::puts("production-extracted Point/Promote/Copy passed; no live hook, whole-frame or FPS benefit claim");
}

