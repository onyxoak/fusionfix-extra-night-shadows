#include "OwnedImageReads224.hpp"
#include <windows.h>
#include "ImmediateRegion33.hpp"
#include <psapi.h>
#include <array>
#include <atomic>
#include <bit>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <optional>
BOOL WINAPI CountQuery(HANDLE,PVOID,DWORD);
inline constexpr auto RealQuery=QueryWorkingSetEx;
// Production helper default query functions are bound to the real counted OS
// wrapper. No positive permission response is synthesized in this fixture.
#undef QueryWorkingSetEx
#define QueryWorkingSetEx CountQuery
#include "FreshReadBatch30.hpp"
#undef QueryWorkingSetEx
#include "ModelLightSubmission20.hpp"
#include "ModelPrevalidation40.hpp"
#include "ModelOwnerTransformDiagnostic46.hpp"
#include "ModelEffectGetterDiagnostic62.hpp"
#include "ModelLightRegistry20.hpp"
#include "core/ShadowCpuProfile29.hpp"
namespace fp=traffic_signal::fresh_page;
unsigned queries=0;
void AfterQuery(PVOID bytes,DWORD size,BOOL success) noexcept;
BOOL WINAPI CountQuery(HANDLE process,PVOID bytes,DWORD size){++queries;const auto ok=RealQuery(process,bytes,size);AfterQuery(bytes,size,ok);return ok;}
bool Region(uintptr_t address,size_t size) noexcept {
 MEMORY_BASIC_INFORMATION m{};if(!VirtualQuery(reinterpret_cast<void*>(address),&m,sizeof(m)))return false;
 return m.State==MEM_COMMIT&&!(m.Protect&(PAGE_GUARD|PAGE_NOACCESS))&&address>=uintptr_t(m.BaseAddress)&&size<=m.RegionSize-(address-uintptr_t(m.BaseAddress));
}
struct SafetyHookContext{uint32_t eflags{},esp{},ebp{},edi{};};
namespace fusionfix::shadows{struct FloatingPointState{~FloatingPointState(){}};}
namespace CoronaLight20{bool requested=false;}
namespace PlayerShadowAllocation{bool Enabled() noexcept{return true;}}
std::atomic<bool> addonActive{true};uint32_t game=0x400000;
enum class Change{None,Descriptor,Generation,Revoke};
Change change=Change::None;unsigned ownerReads=0,changeAtOwnerRead=2;uint8_t* memory=nullptr;DWORD oldProtection=0;
bool revokeAfterQuery=false;unsigned racedRevocations=0;
void AfterQuery(PVOID bytes,DWORD size,BOOL success) noexcept{
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
namespace TrafficSignal{
 struct Match{traffic_signal::Identity identity{};uint32_t kind=1,modelInfo=4;std::array<uint8_t,68> ownerPose109{};};
 bool ReadableOriginal(uintptr_t p,size_t n) noexcept{return Region(p,n);}
 bool Readable(uintptr_t p,size_t n) noexcept{return fp::Readable(p,n,Region,CountQuery);}
 bool MatchOwner(uint32_t owner,Match& value) noexcept{
  ++ownerReads;
  if(ownerReads==changeAtOwnerRead){
   if(change==Change::Descriptor)memory[4096]=99;
   if(change==Change::Generation)memory[8192]=8;
   if(change==Change::Revoke)assert(VirtualProtect(memory+4096,4096,PAGE_NOACCESS,&oldProtection));
  }
  std::array<uint8_t,32> bytes{};
  traffic_signal::fresh_batch30::Span s{owner,bytes.data(),bytes.size()};
  if(!traffic_signal::fresh_batch30::Copy(&s,1,Region,CountQuery))return false;
  value.identity={1,2,3,owner,1,bytes[0],5};return true;
 }
}
namespace TrafficSignal{
 // This fixture models Point/final promotion boundaries; actual combined
 // production owner batching is independently extracted in batch48 tests.
 bool MatchOwnerWithDescriptor48(uintptr_t owner,Match& value,uintptr_t descriptor,
  void* output,size_t size,bool& copied) noexcept{
  const bool matched=MatchOwner(uint32_t(owner),value);
  const traffic_signal::fresh_batch30::Span span{descriptor,output,size};
  copied=traffic_signal::fresh_batch30::Copy(&span,1,Region,CountQuery);
  return matched;
 }
}
namespace Liberty20{
 #include "point-copy47-extracted.inc"
 bool Collision(uint32_t) noexcept{return false;}
}
namespace rage{struct CLightSource{uint32_t mCastShadows{},mFlags{};};}
namespace PlayerShadowAllocation{constexpr bool shortlistRequested48=false;bool DeferModel48(rage::CLightSource&,unsigned) noexcept{return false;}}
namespace CShadows{uint32_t frame=7;uint32_t* pFrameCounter=&frame;}
namespace ModelLightShadow20{
 bool requested=true;std::atomic<bool> active{true};std::atomic_flag lock=ATOMIC_FLAG_INIT;
 model_light20::Registry<16> registry;std::atomic<uint32_t> identitiesPublished32{0},refused{0},promoted{0},retired{0};
 #include "point-promotion47-extracted.inc"
 bool queuedWindow49=false;void ValidateQueuedWindow49(rage::CLightSource*,uint32_t) noexcept{assert(false);}
 #include "queued-permissions47-extracted.inc"
}
namespace ModelEffectObserver{
 using namespace model_effect_observer;
 struct LastErrorScope{DWORD error=GetLastError();~LastErrorScope(){SetLastError(error);}};
 struct Sample{std::array<uint8_t,68> ownerPose109{};bool diagnostic=false,ownerMatched=false,modelInfoMatched=false,descriptorCopied=false,matrixCopied=false,packetCopied=false;traffic_signal::Identity identity{};uint32_t ownerKind{},resolveMismatch{},site{},type{},flags{},key{},promotedKey{};std::array<uint8_t,108> descriptor{};std::array<float,16> matrix{};model_owner_transform46::Snapshot ownerTransform{};model_effect_getter62::Snapshot getter62{};std::array<float,3> position{},color{},direction{},up{};float intensity{},radius{},innerCone{},outerCone{};};
 namespace FixtureLight244 {void Correct(SafetyHookContext&,uint32_t,std::array<uint32_t,18>&,const Invocation&,Sample&) noexcept{}}
 struct Local{model_light_submission20::State state;std::array<Sample,8> samples{};} local;
 std::atomic<bool> enabled{true};std::atomic<uint32_t> readRejected{0},contextRejected{0},nativeInactiveSkipped40{0};bool hotCountersEnabled=true;
 uint32_t Frame() noexcept{return 7;}
 bool Copy(uintptr_t p,void* d,size_t n) noexcept{return Liberty20::Copy(p,d,n);}
 bool InnerEvidence(const SafetyHookContext&,ResolveEvidence&) noexcept{return false;} // Resolve is extracted but not invoked here.
 // Preserve this historical boundary fixture's legacy strategy; full fused
 // pipeline has its own production-extracted actual owner fixture.
 bool pointFusion50=false;enum class PointFusion50Result{Complete,Fallback};
 std::atomic<uint32_t> ownedFusedAttempts79{0},ownedFusedComplete79{0},ownedFusedLegacy79{0},ownedFusedKeys79{0};
 PointFusion50Result FusePoint50(SafetyHookContext&,const Invocation&,Sample&,uint32_t,const std::array<uint32_t,18>&,const PointEvidence&,std::array<uint32_t,32>* =nullptr) noexcept{assert(!pointFusion50);return PointFusion50Result::Fallback;}
 #include "point-permissions47-extracted.inc"
}
struct Result{unsigned queries{},owners{};uint32_t flags{},key{};bool invalid{};};
Result Run(bool scoped,Change requestedChange=Change::None,bool keyed=false,bool nested=false,uint32_t nativeFlags=4){
 using namespace ModelEffectObserver;
 assert(fp::operation44==nullptr);
 change=requestedChange;changeAtOwnerRead=2;ownerReads=0;queries=0;revokeAfterQuery=false;std::memset(memory,0,4096*4);memory[4096]=23;memory[4096+0x5b]=1;memory[8192]=7;
 auto* args=reinterpret_cast<uint32_t*>(memory);const auto vectors=uint32_t(reinterpret_cast<uintptr_t>(memory+12288));
 args[0]=2;args[1]=0;args[2]=keyed?nativeFlags:0u;args[3]=vectors;args[4]=vectors+16;args[5]=vectors+32;args[6]=vectors+48;args[7]=0x3f800000;args[10]=0x41200000;args[15]=keyed?77u:0u;
 auto* floats=reinterpret_cast<float*>(memory+12288);for(unsigned i=0;i<16;++i)floats[i]=1;
 const auto base=uint32_t(reinterpret_cast<uintptr_t>(memory));
 auto* context=reinterpret_cast<uint32_t*>(memory+128);context[0]=game+DispatchReturn;context[1]=2;context[2]=3;
 Invocation invocation{};invocation.frame=7;invocation.preStack=base+132;invocation.owner=base+8192;invocation.effect=2;invocation.matrix=3;invocation.modelInfo=4;invocation.sampled=true;invocation.ordinal=3;invocation.proposedKey=invocation.owner+invocation.ordinal;
 args[15]=keyed?invocation.proposedKey:0u;
 local.state={};local.samples={};assert(local.state.Enter(invocation));
 ResolveEvidence resolved{7,base+132,DispatchReturn,2,3,base+8192,2,base+4096,invocation.proposedKey};assert(local.state.Resolve(resolved));
 ModelLightShadow20::registry={};assert(ModelLightShadow20::registry.Configure(0x30000000));ModelLightShadow20::lock.clear();
 SafetyHookContext regs{};regs.esp=base;regs.ebp=base+124;regs.edi=base+4096;
 std::optional<fp::Operation44> outer;if(nested)outer.emplace();auto* prior=fp::operation44;
 SetLastError(73);const auto csr=_mm_getcsr();
 if(scoped)PointScoped47(regs,WrapperPoint);else PointUnscoped47(regs,WrapperPoint);
 assert(fp::operation44==prior&&GetLastError()==73&&_mm_getcsr()==csr);
 if(nested)assert(outer->count==0); // Point does not borrow or publish outer approvals.
 outer.reset();assert(fp::operation44==nullptr);
 Result result{queries,ownerReads,args[2],args[15],local.state.Current()->invalid};
 if(requestedChange==Change::Revoke)assert(VirtualProtect(memory+4096,4096,PAGE_READWRITE,&oldProtection));
 // Native caller resumes after the callback: fresh permission observation is
 // mandatory even for a page whose approval was reused inside Point.
 const auto before=queries;assert(TrafficSignal::Readable(base,4));assert(queries==before+1);
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
 Result result{queries,ownerReads,lights[0].mFlags,lights[0].mCastShadows,false};
 assert(lights[1].mFlags==0x100&&lights[1].mCastShadows==0);
 outer.reset();assert(fp::operation44==nullptr);
 if(requestedChange==Change::Revoke)assert(VirtualProtect(memory+4096,4096,PAGE_READWRITE,&oldProtection));
 const auto before=queries;assert(TrafficSignal::Readable(base,4));assert(queries==before+1);
 return result;
}
int main(){
 memory=static_cast<uint8_t*>(VirtualAlloc(nullptr,4096*4,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(memory);
 const auto unscoped=Run(false),scoped=Run(true);
 assert(unscoped.flags==scoped.flags&&unscoped.key==scoped.key&&scoped.flags==4&&scoped.key==0x30000000);
 assert(scoped.owners==2&&unscoped.owners==2&&scoped.queries<unscoped.queries);
 std::printf("accepted production Point/Promote: QWS calls %u -> %u; fresh owner reads %u -> %u\n",unscoped.queries,scoped.queries,unscoped.owners,scoped.owners);
 unsigned nativeCallbacks256=0;
 static unsigned* nativeCount256=nullptr;nativeCount256=&nativeCallbacks256;
 ModelEffectObserver::nativeFixture256=[](const model_effect_observer::Invocation& current,uint32_t key,uint32_t type,uint32_t flags) noexcept {
  assert(ModelEffectObserver::local.state.Current()==&current&&current.frame==7&&current.submitted&&!current.invalid);
  assert(key==current.proposedKey&&type==0&&(flags==4||flags==7));++*nativeCount256;
 };
 const auto rejectedBaseline=Run(false,Change::None,true),rejectedScoped=Run(true,Change::None,true);
 assert(nativeCallbacks256==2&&rejectedScoped.flags==4&&rejectedScoped.key!=0);
 const auto nativeSeven257=Run(true,Change::None,true,false,7);assert(nativeSeven257.flags==7&&nativeCallbacks256==3);
 Run(true);assert(nativeCallbacks256==3); // Unkeyed source takes its existing path.
 ModelEffectObserver::nativeFixture256=nullptr;
 assert(rejectedBaseline.flags==rejectedScoped.flags&&rejectedBaseline.key==rejectedScoped.key&&rejectedScoped.owners==0);
 for(const auto scenario:{Change::Descriptor,Change::Generation,Change::Revoke}){
  const auto a=Run(false,scenario),b=Run(true,scenario);
  assert(a.flags==0&&b.flags==0&&a.key==0&&b.key==0&&a.owners==2&&b.owners==2);
  std::printf("fresh final-check refusal scenario%u: QWS %u -> %u\n",unsigned(scenario),a.queries,b.queries);
 }
 const auto nested=Run(true,Change::None,false,true);assert(nested.key==scoped.key&&nested.queries==scoped.queries);
 const auto queuedA=Queued(false),queuedB=Queued(true);
 assert(queuedA.flags==queuedB.flags&&queuedB.flags==0x104&&queuedA.key==queuedB.key&&queuedA.owners==1&&queuedB.owners==1);
 std::printf("queued single-entry (separate owner/descriptor pages): QWS %u -> %u\n",queuedA.queries,queuedB.queries);
 for(const auto scenario:{Change::Descriptor,Change::Generation,Change::Revoke}){
  const auto a=Queued(false,scenario),b=Queued(true,scenario);
  assert(a.flags==0x100&&b.flags==0x100&&a.key==b.key&&a.owners==1&&b.owners==1);
  std::printf("queued freshness refusal scenario%u: QWS %u -> %u\n",unsigned(scenario),a.queries,b.queries);
 }
 assert(fp::operation44==nullptr);assert(VirtualFree(memory,0,MEM_RELEASE));
 std::puts("production-extracted Point/Promote/Copy passed; no live hook, whole-frame or FPS benefit claim");
}

