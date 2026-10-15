#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <xmmintrin.h>
const auto RealQuery49=QueryWorkingSetEx;
BOOL WINAPI CountQuery49(HANDLE,PVOID,DWORD);
BOOL WINAPI FailedQuery54(HANDLE,PVOID,DWORD){return FALSE;}
#undef QueryWorkingSetEx
#define QueryWorkingSetEx CountQuery49
#include "TrafficSignalIdentity.hpp"
#include "PoolDescriptorSnapshots.hpp"
#include "ImmediateRegion33.hpp"
#include "NativeOwnerClass109.hpp"
#include "NativeOwnerClass114.hpp"
#include "FreshReadGroup50.hpp"
#include "FreshReadBatch30.hpp"
void CertificateStage54(unsigned) noexcept;
#define CE_CERTIFICATE_BEFORE_STAGE54 CertificateStage54
#include "FreshReadWindow49.hpp"
#include "ModelLightRegistry20.hpp"
namespace rage {struct VehiclePool {uint8_t* m_aStorage;uint8_t* m_aFlags;int32_t m_nSize,m_nStorageSize,m_nFirstFreeIndex,m_numSlotsUsed;};struct CLightSource{uint32_t mCastShadows{},mFlags{};};}
std::vector<uint8_t> image49(0x1232c64);uintptr_t game=uintptr_t(image49.data());
std::vector<std::vector<uintptr_t>> queryOrder59;
unsigned queries49{},pages49{},mutateAt49{};void(*mutate49)()=nullptr;
BOOL WINAPI CountQuery49(HANDLE p,PVOID b,DWORD n){++queries49;pages49+=n/sizeof(PSAPI_WORKING_SET_EX_INFORMATION);
const auto ok=RealQuery49(p,b,n);if(mutate49&&queries49==mutateAt49){auto fn=mutate49;mutate49=nullptr;fn();}return ok;}
namespace TrafficSignal {
struct Match {traffic_signal::Identity identity{};uint32_t kind{};float entity[3]{};uint32_t modelInfo{};std::array<uint8_t,68> ownerPose109{};};
bool ReadableOriginal(uintptr_t address,size_t bytes) noexcept{return immediate_region33::Readable(address,bytes);}
bool Readable(uintptr_t address,size_t bytes) noexcept{return traffic_signal::fresh_page::Readable(address,bytes,ReadableOriginal);}
#define CE_OWNER_BATCH_QUERY35 CountQuery49
#include "owner-runtime32-extracted.inc"
#include "ModelOwnerWindow49Runtime.inc"
}
namespace Liberty20 {
#include "point-copy47-extracted.inc"
}
namespace CShadows{uint32_t frame=7;uint32_t* pFrameCounter=&frame;}
namespace PlayerShadowAllocation {bool shortlistRequested48=false;bool DeferModel48(rage::CLightSource&,unsigned) noexcept{return false;}}
void* AllocateScratchTest59(size_t) noexcept;bool ReleaseScratchTest59(void*) noexcept;


bool benchAllocationFail59=false;
void* BenchAllocation59(size_t n) noexcept {if(benchAllocationFail59){benchAllocationFail59=false;return nullptr;}return HeapAlloc(GetProcessHeap(),0,n);}
#define CE_ALLOCATE_SCRATCH59(size) BenchAllocation59(size)
namespace ModelLightShadow20 {
model_light20::Registry<1024> registry;std::atomic<bool> active{true};std::atomic_flag lock=ATOMIC_FLAG_INIT;std::atomic<uint32_t> retired{0},faults{0};
#include "lifecycle54-extracted.inc"
#include "ModelQueuedWindow49Runtime.inc"
#include "queued-window49-extracted.inc"
}
// Frozen53 production references: exact owner and queued implementations.
namespace ParentTraffic53 {
 using Match=TrafficSignal::Match;
 bool ReadableOriginal(uintptr_t a,size_t n) noexcept{return TrafficSignal::ReadableOriginal(a,n);}
 #include "parent-owner53-reference.inc"
}
namespace ParentQueued53 {
 model_light20::Registry<1024> registry;auto& active=ModelLightShadow20::active;auto& lock=ModelLightShadow20::lock;auto& retired=ModelLightShadow20::retired;
 bool ValidateOne48(rage::CLightSource& l) noexcept{return ModelLightShadow20::ValidateOne48(l);}
 #define TrafficSignal ParentTraffic53
 #include "parent-queued53-reference.inc"
 #undef TrafficSignal
}

#include "reference-window32.hpp"
namespace ReferenceOwner32 {
 using Match=TrafficSignal::Match;
 bool ReadableOriginal(uintptr_t a,size_t n) noexcept{return TrafficSignal::ReadableOriginal(a,n);}
 #include "reference-owner32.inc"
}
namespace ReferenceQueued32 {
 model_light20::Registry<1024> registry;auto& active=ModelLightShadow20::active;auto& lock=ModelLightShadow20::lock;auto& retired=ModelLightShadow20::retired;
 bool ValidateOne48(rage::CLightSource& l) noexcept{return ModelLightShadow20::ValidateOne48(l);}
 #define TrafficSignal ReferenceOwner32
 #include "reference-queued32.inc"
 #undef TrafficSignal
}
__declspec(noinline) void ClobberAllocatorFp59() noexcept { __asm {
 fninit
 fld1
 }
 _mm_setcsr(_mm_getcsr()^0x6000u); }
bool failAllocation59=false;unsigned allocations59{},releases59{};std::vector<void*> liveScratch59;
void* AllocateScratchTest59(size_t n) noexcept {
 ++allocations59;SetLastError(123);ClobberAllocatorFp59();
 if(failAllocation59){failAllocation59=false;return nullptr;}
 auto* p=HeapAlloc(GetProcessHeap(),0,n);assert(p);for(auto* existing:liveScratch59)assert(existing!=p);liveScratch59.push_back(p);return p;
}
bool ReleaseScratchTest59(void* p) noexcept {++releases59;assert(!liveScratch59.empty()&&liveScratch59.back()==p);liveScratch59.pop_back();SetLastError(456);ClobberAllocatorFp59();return HeapFree(GetProcessHeap(),0,p)!=FALSE;}

namespace ExactParent54 {
model_light20::Registry<1024> registry;auto& active=ModelLightShadow20::active;auto& lock=ModelLightShadow20::lock;auto& retired=ModelLightShadow20::retired;
bool ValidateOne48(rage::CLightSource& l) noexcept{return ModelLightShadow20::ValidateOne48(l);}
#include "benchmark-parent54.inc"
}

bool disableAtCommit54=false,frameAtCommit54=false;
void CertificateStage54(unsigned group) noexcept {if(frameAtCommit54&&group==0){frameAtCommit54=false;++CShadows::frame;}if(disableAtCommit54&&group==0){disableAtCommit54=false;const auto id=ModelLightShadow20::registry.entries[0].id;ModelLightShadow20::OnFree(id.pool,id.owner);assert(!ModelLightShadow20::active&&ModelLightShadow20::faults>0);}}
constexpr unsigned Number49=639,Page49=4096,Storage49=((Number49*0x320+Page49-1)/Page49)*Page49;
uint8_t *headers49{},*owners49{},*building49{},*flags49{},*model49{},*matrices49{},*descriptors49{};DWORD old49{};
rage::VehiclePool *bp49{},*op49{};std::vector<rage::CLightSource> initial49;
void Root49(unsigned offset,uintptr_t value){std::memcpy(image49.data()+offset,&value,4);}
void Initialize49(){
 auto allocate=[](unsigned bytes){auto p=static_cast<uint8_t*>(VirtualAlloc(nullptr,bytes,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(p);std::memset(p,1,bytes);return p;};
 headers49=allocate(Page49);owners49=allocate(Storage49);building49=allocate(Page49);flags49=allocate(Page49);model49=allocate(Page49);
 matrices49=allocate(Number49*64);descriptors49=allocate(Number49*128);bp49=reinterpret_cast<rage::VehiclePool*>(headers49);op49=bp49+1;
 *bp49={building49,flags49,1,0x70,0,0};*op49={owners49,flags49,Number49,0x320,0,0};Root49(0xebd0e8,uintptr_t(bp49));Root49(0x1232c60,uintptr_t(op49));
 uint32_t hash=123;std::memcpy(model49+0x3c,&hash,4);Root49(0xe95cd8+5*4,uintptr_t(model49));
 for(unsigned i=0;i<Number49;++i){const int16_t index=5;const auto matrix=uintptr_t(matrices49+i*64);std::memcpy(owners49+i*0x320+0x2e,&index,2);std::memcpy(owners49+i*0x320+0x20,&matrix,4);flags49[i]=7;
  for(unsigned j=0;j<3;++j)reinterpret_cast<float*>(matrices49+i*64)[12+j]=float(j+1);
 }
}
void Configure49(){using namespace ModelLightShadow20;registry={};assert(registry.Configure(0x30000000));initial49.clear();active=true;retired=0;faults=0;lock.clear();
 for(unsigned i=0;i<Number49;++i){TrafficSignal::Match m{};assert(TrafficSignal::MatchOwner(uintptr_t(owners49+i*0x320),m));std::array<uint8_t,108> bytes{};std::memcpy(bytes.data(),descriptors49+i*128,108);
  auto* e=registry.Observe(m.identity,0,uint32_t(uintptr_t(descriptors49+i*128)),m.modelInfo,bytes,7);assert(e);assert(registry.Publish(e->key));initial49.push_back({e->key,0x44});}
 CShadows::frame=7;ParentQueued53::registry=ModelLightShadow20::registry;queries49=pages49=0;mutate49=nullptr;
}
void Descriptor49(){descriptors49[0]^=1;}
void Guard49();
void Index49(){const int16_t value=6;std::memcpy(owners49+0x2e,&value,2);}
void MatrixPointer49(){const auto value=uintptr_t(matrices49+64);std::memcpy(owners49+0x20,&value,4);}
void MatrixNan49(){reinterpret_cast<float*>(matrices49)[12]=NAN;}
void GuardGeneration49(){Guard49();flags49[1]=8;}
void Hash49(){model49[0x3c]^=1;}
void Generation49(){flags49[0]=8;}
void Retire49(){ModelLightShadow20::registry.Free(uint32_t(uintptr_t(op49)),uint32_t(uintptr_t(owners49)));}
void Pool49(){op49->m_aStorage=owners49+0x320;}
void Guard49(){assert(VirtualProtect(owners49,Page49,PAGE_READWRITE|PAGE_GUARD,&old49));}
void Revoke49(){assert(VirtualProtect(owners49,Page49,PAGE_NOACCESS,&old49));}
void Metadata49(){ModelLightShadow20::registry.entries[0].descriptor=uint32_t(uintptr_t(descriptors49+128));}
std::vector<rage::CLightSource> Run49(bool window,unsigned count=Number49){auto v=initial49;ModelLightShadow20::queuedWindow49=true;ModelLightShadow20::queuedCommitCertificate54=window;queries49=pages49=0;ModelLightShadow20::ValidateQueued(v.data(),count);return v;}
void AssertSame49(const std::vector<rage::CLightSource>& a,const std::vector<rage::CLightSource>& b){assert(a.size()==b.size());for(unsigned i=0;i<a.size();++i)assert(a[i].mCastShadows==b[i].mCastShadows&&a[i].mFlags==b[i].mFlags);}
int RunSuite59(bool scratch){ModelLightShadow20::queuedScratch32Requested59=scratch;Initialize49();Configure49();auto baseline=Run49(false);const auto serialQueries=queries49,serialPages=pages49;auto candidate=Run49(true);const auto wideQueries=queries49,widePages=pages49;AssertSame49(baseline,candidate);for(auto l:candidate)assert(l.mFlags==0x44);
 assert(serialQueries==(scratch?719u:799u)&&wideQueries==(scratch?145u:245u)&&wideQueries<serialQueries);
 std::printf("Production ValidateQueued, %u actual pool owners: queries %u -> %u, pages %u -> %u; identical %u accepted packet flags/keys.\n",Number49,serialQueries,wideQueries,serialPages,widePages,Number49);
 // Stable bad bytes/generation/frame/identity reject exactly as serial does.
 for(unsigned kind=0;kind<5;++kind){Configure49();if(kind==0)Descriptor49();if(kind==1)Generation49();if(kind==2)CShadows::frame=10;if(kind==3)Retire49();if(kind==4){const int16_t bad=-1;std::memcpy(owners49+0x2e,&bad,2);}
  baseline=Run49(false);if(kind==0)Descriptor49();if(kind==1)flags49[0]=7;if(kind==4){const int16_t good=5;std::memcpy(owners49+0x2e,&good,2);}
  Configure49();if(kind==0)Descriptor49();if(kind==1)Generation49();if(kind==2)CShadows::frame=10;if(kind==3)Retire49();if(kind==4){const int16_t bad=-1;std::memcpy(owners49+0x2e,&bad,2);}
  candidate=Run49(true);AssertSame49(baseline,candidate);assert(candidate[0].mFlags==0x40);
  if(kind==0)Descriptor49();if(kind==1)flags49[0]=7;if(kind==4){const int16_t good=5;std::memcpy(owners49+0x2e,&good,2);}
 }
 // Last content changes after phase-3 copies must be freshly refused at commit.
 Configure49();mutateAt49=4;mutate49=Descriptor49;candidate=Run49(true,16);assert(candidate[0].mFlags==0x40&&!ModelLightShadow20::registry.entries[0].active);Descriptor49();
 Configure49();mutateAt49=4;mutate49=Hash49;candidate=Run49(true,16);for(unsigned i=0;i<16;++i)assert(candidate[i].mFlags==0x40);Hash49();
 // Mutations after phase 4 are caught by the complete per-entry commit read.
 Configure49();mutateAt49=5;mutate49=Generation49;candidate=Run49(true,16);assert(candidate[0].mFlags==0x40&&!ModelLightShadow20::registry.entries[0].active);flags49[0]=7;
 Configure49();mutateAt49=5;mutate49=Pool49;candidate=Run49(true,16);for(unsigned i=0;i<16;++i)assert(candidate[i].mFlags==0x40);op49->m_aStorage=owners49;
 Configure49();mutateAt49=5;mutate49=Index49;candidate=Run49(true,16);assert(candidate[0].mFlags==0x40);const int16_t restoreIndex=5;std::memcpy(owners49+0x2e,&restoreIndex,2);
 Configure49();mutateAt49=5;mutate49=MatrixPointer49;candidate=Run49(true,16);assert(candidate[0].mFlags==0x40);const auto restoreMatrix=uintptr_t(matrices49);std::memcpy(owners49+0x20,&restoreMatrix,4);
 Configure49();mutateAt49=5;mutate49=MatrixNan49;candidate=Run49(true,16);assert(candidate[0].mFlags==0x40);reinterpret_cast<float*>(matrices49)[12]=1.0f;
 // Changes after address snapshots must not authorize stale owner/descriptor.
 Configure49();mutateAt49=3;mutate49=Descriptor49;candidate=Run49(true,16);assert(candidate[0].mFlags==0x40&&!ModelLightShadow20::registry.entries[0].active);Descriptor49();
 Configure49();mutateAt49=3;mutate49=Generation49;candidate=Run49(true,16);assert(candidate[0].mFlags==0x40);for(unsigned i=1;i<16;++i)assert(candidate[i].mFlags==0x44);flags49[0]=7;
 Configure49();mutateAt49=4;mutate49=Retire49;candidate=Run49(true,16);assert(candidate[0].mFlags==0x40);for(unsigned i=1;i<16;++i)assert(candidate[i].mFlags==0x44);
 Configure49();mutateAt49=2;mutate49=Pool49;candidate=Run49(true,16);for(unsigned i=0;i<16;++i)assert(candidate[i].mFlags==0x40);op49->m_aStorage=owners49;
 // Final registry relookup notices descriptor metadata replacement, then uses
 // the original fresh validator rather than authorizing copied hint metadata.
 Configure49();mutateAt49=4;mutate49=Metadata49;candidate=Run49(true,16);assert(candidate[0].mFlags==0x44);assert(ModelLightShadow20::queuedFallback49.load()>0);
 // Already-guarded page rejected without consuming PAGE_GUARD.
 Configure49();Guard49();candidate=Run49(true,16);assert(candidate[0].mFlags==0x40);MEMORY_BASIC_INFORMATION mbi{};assert(VirtualQuery(owners49,&mbi,sizeof(mbi)));assert(mbi.Protect&PAGE_GUARD);assert(VirtualProtect(owners49,Page49,PAGE_READWRITE,&old49));
 // Guard installed after QWS is one-shot. Faulting source is not retried;
 // subsequent groups on that page can read after guard consumption. A fresh
 // changed generation still rejects them. Never claim global guard retention.
 Configure49();volatile uint8_t warm=owners49[0];(void)warm;mutateAt49=2;mutate49=Guard49;candidate=Run49(true,16);assert(candidate[0].mFlags==0x40);assert(ModelLightShadow20::queuedWindowFaults49.load()>0);assert(VirtualProtect(owners49,Page49,PAGE_READWRITE,&old49));
 Configure49();volatile uint8_t sharedWarm=owners49[0];(void)sharedWarm;mutateAt49=2;mutate49=GuardGeneration49;candidate=Run49(true,16);assert(candidate[0].mFlags==0x40&&candidate[1].mFlags==0x40);for(unsigned i=2;i<16;++i)assert(candidate[i].mFlags==0x44);flags49[1]=7;assert(VirtualProtect(owners49,Page49,PAGE_READWRITE,&old49));
 Configure49();mutateAt49=2;mutate49=Revoke49;candidate=Run49(true,16);assert(candidate[0].mFlags==0x40);for(unsigned i=6;i<16;++i)assert(candidate[i].mFlags==0x44);assert(VirtualProtect(owners49,Page49,PAGE_READWRITE,&old49));
 // Final permission query has no source data authority: actual guard after
 // positive preflight faults once, ends the domain, and later commits query fresh.
 Configure49();mutateAt49=5;mutate49=Guard49;candidate=Run49(true,16);assert(candidate[0].mFlags==0x40&&queries49>5);for(unsigned i=1;i<16;++i)assert(candidate[i].mFlags==0x44);assert(VirtualProtect(owners49,Page49,PAGE_READWRITE,&old49));
 Configure49();mutateAt49=5;mutate49=GuardGeneration49;candidate=Run49(true,16);assert(candidate[0].mFlags==0x40&&candidate[1].mFlags==0x40&&queries49>5);flags49[1]=7;assert(VirtualProtect(owners49,Page49,PAGE_READWRITE,&old49));
 Configure49();mutateAt49=5;mutate49=Revoke49;candidate=Run49(true,16);assert(candidate[0].mFlags==0x40&&queries49>5);for(unsigned i=6;i<16;++i)assert(candidate[i].mFlags==0x44);assert(VirtualProtect(owners49,Page49,PAGE_READWRITE,&old49));
 Configure49();frameAtCommit54=true;candidate=Run49(true,16);for(unsigned i=0;i<16;++i)assert(candidate[i].mFlags==0x40);assert(CShadows::frame==8);
 Configure49();disableAtCommit54=true;candidate=Run49(true,16);for(unsigned i=0;i<16;++i)assert(candidate[i].mFlags==0x40);assert(!ModelLightShadow20::active);
 // Region preference retains current per-entry commit instead of certificate.
 Configure49();traffic_signal::fresh_page::preferRegion=true;baseline=Run49(false,16);const auto regionQueries54=queries49;candidate=Run49(true,16);AssertSame49(baseline,candidate);assert(queries49==regionQueries54);traffic_signal::fresh_page::preferRegion=false;
 // Optional unavailable object peer preserves valid original building fallback.
 Configure49();const int16_t buildingIndex=5;const auto buildingMatrix=uintptr_t(matrices49);std::memcpy(building49+0x2e,&buildingIndex,2);std::memcpy(building49+0x20,&buildingMatrix,4);
 TrafficSignal::Match buildingMatch{};assert(TrafficSignal::MatchOwner(uintptr_t(building49),buildingMatch));std::array<uint8_t,108> buildingDescriptor{};std::memcpy(buildingDescriptor.data(),descriptors49,108);
 auto* buildingEntry=ModelLightShadow20::registry.Observe(buildingMatch.identity,0,uint32_t(uintptr_t(descriptors49)),buildingMatch.modelInfo,buildingDescriptor,7);assert(buildingEntry);assert(ModelLightShadow20::registry.Publish(buildingEntry->key));initial49[0].mCastShadows=buildingEntry->key;
 Root49(0x1232c60,0);candidate=Run49(true,16);baseline=Run49(false,16);AssertSame49(baseline,candidate);assert(candidate[0].mFlags==0x44);Root49(0x1232c60,uintptr_t(op49));
 Configure49();initial49[0]={0x123456,0x44};initial49[1].mFlags=0x40;candidate=Run49(true,16);assert(candidate[0].mFlags==0x44&&candidate[1].mFlags==0x40);
 Configure49();baseline=Run49(false,1);const auto oneSerial=queries49;candidate=Run49(true,1);AssertSame49(baseline,candidate);assert(queries49==oneSerial);
 Configure49();baseline=Run49(false,2);candidate=Run49(true,2);AssertSame49(baseline,candidate);
 Configure49();ModelLightShadow20::lock.test_and_set();candidate=Run49(true,16);for(unsigned i=0;i<16;++i)assert(candidate[i].mFlags==0x40);ModelLightShadow20::lock.clear();
 
 // Exact-span mismatch refuses permission reuse and requires a fresh path;
 // no copied values or stale positive permission are exported.
 {using namespace traffic_signal::fresh_certificate54;
  std::array<uint8_t,16> output{};Span a{uintptr_t(descriptors49),output.data(),16};Group g{&a,1};
  auto cert=Preflight(&g,1,TrafficSignal::ReadableOriginal,CountQuery49);assert(cert.active&&cert.allowed[0]);
  bool copied=false,fault=false;Span changed{uintptr_t(descriptors49+128),output.data(),16};assert(!TryCopy(cert,0,&changed,1,copied,fault)&&!cert.active&&!copied&&!fault);
  output.fill(0xee);auto unavailable=Preflight(&g,1,TrafficSignal::ReadableOriginal,CountQuery49);assert(unavailable.allowed[0]);
  assert(VirtualProtect(descriptors49,Page49,PAGE_NOACCESS,&old49));assert(TryCopy(unavailable,0,&a,1,copied,fault)&&fault&&!copied&&!unavailable.active);for(auto b:output)assert(b==0xee);assert(VirtualProtect(descriptors49,Page49,PAGE_READWRITE,&old49));
  // Negative approvals are not cached: restoration before the actual
  // commit requires the original fresh query rather than a reused denial.
  assert(VirtualProtect(descriptors49,Page49,PAGE_NOACCESS,&old49));auto denied=Preflight(&g,1,TrafficSignal::ReadableOriginal,CountQuery49);assert(!denied.allowed[0]);assert(VirtualProtect(descriptors49,Page49,PAGE_READWRITE,&old49));
  assert(!TryCopy(denied,0,&a,1,copied,fault)&&!denied.active&&!copied&&!fault);const auto restored=traffic_signal::fresh_window49::Copy(&g,1,TrafficSignal::ReadableOriginal,CountQuery49);assert(restored.copied[0]);
  // Cross-page and failed-QWS positive VirtualQuery approvals are permission
  // hints only. Current bytes are copied later, under an active domain.
  Span cross{uintptr_t(descriptors49+Page49-8),output.data(),16};Group crossGroup{&cross,1};auto crossCert=Preflight(&crossGroup,1,TrafficSignal::ReadableOriginal,CountQuery49);assert(crossCert.allowed[0]&&crossCert.fallbacks==1);
  descriptors49[Page49-8]^=1;assert(!crossCert.reusable[0]&&!TryCopy(crossCert,0,&cross,1,copied,fault)&&!crossCert.active&&!copied&&!fault);const auto crossFresh=traffic_signal::fresh_window49::Copy(&crossGroup,1,TrafficSignal::ReadableOriginal,CountQuery49);assert(crossFresh.copied[0]&&output[0]==descriptors49[Page49-8]);descriptors49[Page49-8]^=1;
  auto failed=Preflight(&g,1,TrafficSignal::ReadableOriginal,FailedQuery54);assert(failed.allowed[0]&&failed.fallbacks==1);assert(!failed.reusable[0]&&!TryCopy(failed,0,&a,1,copied,fault)&&!failed.active&&!copied&&!fault);const auto failedFresh=traffic_signal::fresh_window49::Copy(&g,1,TrafficSignal::ReadableOriginal,CountQuery49);assert(failedFresh.copied[0]);
  auto* cold=static_cast<uint8_t*>(VirtualAlloc(nullptr,Page49,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(cold);
  Span coldSpan{uintptr_t(cold),output.data(),16};Group coldGroup{&coldSpan,1};auto coldCert=Preflight(&coldGroup,1,TrafficSignal::ReadableOriginal,CountQuery49);assert(coldCert.allowed[0]&&coldCert.fallbacks==1);
  cold[0]=0x3c;assert(!coldCert.reusable[0]&&!TryCopy(coldCert,0,&coldSpan,1,copied,fault)&&!coldCert.active&&!copied&&!fault);const auto coldFresh=traffic_signal::fresh_window49::Copy(&coldGroup,1,TrafficSignal::ReadableOriginal,CountQuery49);assert(coldFresh.copied[0]&&output[0]==0x3c);assert(VirtualFree(cold,0,MEM_RELEASE));
 }
 if(!GetEnvironmentVariableA("CE_SKIP_TIMING54",nullptr,0)){
 Configure49();std::array<double,9> serialMs{},wideMs{},parentMs{};LARGE_INTEGER frequency{};QueryPerformanceFrequency(&frequency);
 for(unsigned rep=0;rep<9;++rep){LARGE_INTEGER parentStart54{},parentEnd54{};queries49=pages49=0;QueryPerformanceCounter(&parentStart54);auto parentPackets=initial49;ParentQueued53::ValidateQueuedWindow49(parentPackets.data(),Number49);QueryPerformanceCounter(&parentEnd54);parentMs[rep]=1000.0*double(parentEnd54.QuadPart-parentStart54.QuadPart)/double(frequency.QuadPart);for(auto l:parentPackets)assert(l.mFlags==0x44);assert(queries49==799);
 for(bool wide:{false,true}){LARGE_INTEGER a{},b{};QueryPerformanceCounter(&a);auto v=Run49(wide);QueryPerformanceCounter(&b);for(auto l:v)assert(l.mFlags==0x44);(wide?wideMs:serialMs)[rep]=1000.0*double(b.QuadPart-a.QuadPart)/double(frequency.QuadPart);}}
 std::sort(parentMs.begin(),parentMs.end());std::sort(serialMs.begin(),serialMs.end());std::sort(wideMs.begin(),wideMs.end());std::printf("Offline warm fixture median over nine runs: exact_parent53 %.3f ms; certificate_OFF54 %.3f ms; certificate_ON54 %.3f ms. Not game frame time or FPS.\n",parentMs[4],serialMs[4],wideMs[4]);
 }else std::puts("Timing skipped during author recording; semantic/fault checks only.");
 for(auto p:{headers49,owners49,building49,flags49,model49,matrices49,descriptors49})assert(VirtualFree(p,0,MEM_RELEASE));
 std::puts("Actual staged queued owner/descriptor validation integration; per-source final relookup, mutation and fault containment passed.");return 0;
}

void ExtraTests59(){Initialize49();Configure49();ModelLightShadow20::queuedScratch32Requested59=true;
 for(unsigned count:{1u,16u,17u,31u,32u,33u,63u,64u,65u,639u}){
  Configure49();ReferenceQueued32::registry=ModelLightShadow20::registry;queryOrder59.clear();queries49=pages49=0;
  auto expected=initial49;ReferenceQueued32::ValidateQueuedWindow49(expected.data(),count);auto expectedOrder=queryOrder59;
  queryOrder59.clear();auto actual=Run49(true,count);AssertSame49(expected,actual);assert(queryOrder59==expectedOrder);
 }
 // Allocation failure chooses the exact original16 before any source reads.
 Configure49();ModelLightShadow20::queuedScratch32Requested59=false;queryOrder59.clear();auto expected=Run49(true);auto expectedOrder=queryOrder59;
 ModelLightShadow20::queuedScratch32Requested59=true;failAllocation59=true;queryOrder59.clear();auto actual=Run49(true);AssertSame49(expected,actual);assert(queryOrder59==expectedOrder&&liveScratch59.empty());
 // Independently nested callbacks own separate allocations and restore the outer buffers.
 Configure49();mutateAt49=1;mutate49=[](){assert(liveScratch59.size()==1);auto nested=initial49;ModelLightShadow20::ValidateQueued(nested.data(),32);for(auto l:nested)assert(l.mFlags==0x44);assert(liveScratch59.size()==1);};
 actual=Run49(true);for(auto l:actual)assert(l.mFlags==0x44);assert(liveScratch59.empty());
 Configure49();auto packets=initial49;alignas(16) unsigned char savedFp59[512]{},beforeFp59[512]{},afterFp59[512]{};_fxsave(savedFp59);__asm {
 fninit
 fld1
 fldz
 }
_fxsave(beforeFp59);SetLastError(0xABC);const auto mxcsr59=_mm_getcsr();ModelLightShadow20::ValidateQueued(packets.data(),32);assert(GetLastError()==0xABC&&_mm_getcsr()==mxcsr59);_fxsave(afterFp59);assert(!std::memcmp(beforeFp59,afterFp59,24));// Only the two nonempty x87 stack registers are architectural values;
// empty-tag register payload bytes are unspecified after FXRSTOR.
for(unsigned reg59=0;reg59<2;++reg59)assert(!std::memcmp(beforeFp59+32+reg59*16,afterFp59+32+reg59*16,10));_fxrstor(savedFp59);
 std::printf("Callback-owned scratch bytes: %zu; allocations %u, releases %u (one deliberate allocation failure).\n",sizeof(ModelLightShadow20::QueuedScratch59),allocations59,releases59);
 assert(allocations59==releases59+1); // one deliberate failed allocation
 for(auto p:{headers49,owners49,building49,flags49,model49,matrices49,descriptors49})assert(VirtualFree(p,0,MEM_RELEASE));
 std::puts("Exact frozen32 query order/boundaries, original16 allocation-failure fallback, nested independent scratch and LastError passed.");
}

bool SafeBench59(unsigned trial){DWORD pid{};GetWindowThreadProcessId(GetForegroundWindow(),&pid);
 FILE* f{};if(fopen_s(&f,R"(D:\Codex-Work\Libertys-Shadows-2.0\evidence\latest-scene-capture.json)","rb")||!f)return false;
 char status[8192]{};const auto n=std::fread(status,1,sizeof(status)-1,f);std::fclose(f);
 const bool safe=pid!=40504&&n>0&&std::strstr(status,R"("state": "armed_press_f8")")&&std::strstr(status,R"("images": 1330)");
 std::printf("STATE trial %u tick %llu foreground %lu armed1330 %u\n",trial,GetTickCount64(),pid,unsigned(safe));return safe;
}
int main(){SetPriorityClass(GetCurrentProcess(),BELOW_NORMAL_PRIORITY_CLASS);Initialize49();
 LARGE_INTEGER frequency{};QueryPerformanceFrequency(&frequency);std::array<std::array<double,9>,5> times{};
 const auto started=GetTickCount64();
 for(unsigned rep=0;rep<9;++rep)for(unsigned offset=0;offset<5;++offset){const unsigned mode=(rep+offset)%5;
  if(GetTickCount64()-started>60000||!SafeBench59(rep*5+offset))return 2;
  Configure49();ExactParent54::registry=ModelLightShadow20::registry;ExactParent54::queuedCommitCertificate54=mode!=0;
  ModelLightShadow20::queuedScratch32Requested59=true;ModelLightShadow20::queuedCommitCertificate54=mode!=2;
  benchAllocationFail59=mode==4;auto packets=initial49;queries49=pages49=0;LARGE_INTEGER a{},b{};
  QueryPerformanceCounter(&a);
  if(mode<2)ExactParent54::ValidateQueuedWindow49(packets.data(),Number49);else ModelLightShadow20::ValidateQueued(packets.data(),Number49);
  QueryPerformanceCounter(&b);for(auto l:packets)assert(l.mFlags==0x44);
  assert(queries49==(mode==0?799u:mode==1||mode==4?245u:mode==2?719u:145u));
  times[mode][rep]=1000.*double(b.QuadPart-a.QuadPart)/double(frequency.QuadPart);
  std::printf("RUN rep%u mode%u %.6fms queries%u pages%u\n",rep,mode,times[mode][rep],queries49,pages49);
  if(!SafeBench59(rep*5+offset))return 2;
 }
 for(unsigned mode=0;mode<5;++mode){std::sort(times[mode].begin(),times[mode].end());std::printf("MEDIAN mode%u %.6fms\n",mode,times[mode][4]);}
 std::puts("Offline allocation-inclusive fixture; no game frame time/FPS proof.");
}
