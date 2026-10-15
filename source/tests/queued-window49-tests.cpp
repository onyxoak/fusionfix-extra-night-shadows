#include <optional>
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
const auto RealQuery49=QueryWorkingSetEx;
BOOL WINAPI CountQuery49(HANDLE,PVOID,DWORD);
#undef QueryWorkingSetEx
#define QueryWorkingSetEx CountQuery49
#include "TrafficSignalIdentity.hpp"
#include "PoolDescriptorSnapshots.hpp"
#include "ImmediateRegion33.hpp"
#include "NativeOwnerClass109.hpp"
#include "NativeOwnerClass114.hpp"
#include "FreshReadGroup50.hpp"
#include "FreshReadBatch30.hpp"
#include "FreshReadWindow49.hpp"
#include "ModelLightRegistry20.hpp"
namespace rage {struct VehiclePool {uint8_t* m_aStorage;uint8_t* m_aFlags;int32_t m_nSize,m_nStorageSize,m_nFirstFreeIndex,m_numSlotsUsed;};struct CLightSource{uint32_t mCastShadows{},mFlags{};};}
std::vector<uint8_t> image49(0x1232c64);uintptr_t game=uintptr_t(image49.data());
unsigned queries49{},pages49{},mutateAt49{};void(*mutate49)()=nullptr;
BOOL WINAPI CountQuery49(HANDLE p,PVOID b,DWORD n){++queries49;pages49+=n/sizeof(PSAPI_WORKING_SET_EX_INFORMATION);const auto ok=RealQuery49(p,b,n);if(mutate49&&queries49==mutateAt49){auto fn=mutate49;mutate49=nullptr;fn();}return ok;}
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
namespace ModelLightShadow20 {
model_light20::Registry<1024> registry;std::atomic<bool> active{true};std::atomic_flag lock=ATOMIC_FLAG_INIT;std::atomic<uint32_t> retired{0};
#include "ModelQueuedWindow49Runtime.inc"
#include "queued-window49-extracted.inc"
}
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
void Configure49(){using namespace ModelLightShadow20;registry={};assert(registry.Configure(0x30000000));initial49.clear();active=true;retired=0;lock.clear();
 for(unsigned i=0;i<Number49;++i){TrafficSignal::Match m{};assert(TrafficSignal::MatchOwner(uintptr_t(owners49+i*0x320),m));std::array<uint8_t,108> bytes{};std::memcpy(bytes.data(),descriptors49+i*128,108);
  auto* e=registry.Observe(m.identity,0,uint32_t(uintptr_t(descriptors49+i*128)),m.modelInfo,bytes,7);assert(e);assert(registry.Publish(e->key));initial49.push_back({e->key,0x44});}
 CShadows::frame=7;queries49=pages49=0;mutate49=nullptr;
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
std::vector<rage::CLightSource> Run49(bool window,unsigned count=Number49){auto v=initial49;ModelLightShadow20::queuedWindow49=window;queries49=pages49=0;ModelLightShadow20::ValidateQueued(v.data(),count);return v;}
void AssertSame49(const std::vector<rage::CLightSource>& a,const std::vector<rage::CLightSource>& b){assert(a.size()==b.size());for(unsigned i=0;i<a.size();++i)assert(a[i].mCastShadows==b[i].mCastShadows&&a[i].mFlags==b[i].mFlags);}
int main(){
 // This inherited fixture deliberately tests the exact original16 pipeline.
 ModelLightShadow20::queuedScratch32Requested59=false;Initialize49();Configure49();auto baseline=Run49(false);const auto serialQueries=queries49,serialPages=pages49;auto candidate=Run49(true);const auto wideQueries=queries49,widePages=pages49;AssertSame49(baseline,candidate);for(auto l:candidate)assert(l.mFlags==0x44);
 assert(wideQueries<=4*((Number49+15)/16)+Number49&&serialQueries>=Number49*3&&wideQueries<serialQueries);
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
 // Optional unavailable object peer preserves valid original building fallback.
 Configure49();const int16_t buildingIndex=5;const auto buildingMatrix=uintptr_t(matrices49);std::memcpy(building49+0x2e,&buildingIndex,2);std::memcpy(building49+0x20,&buildingMatrix,4);
 TrafficSignal::Match buildingMatch{};assert(TrafficSignal::MatchOwner(uintptr_t(building49),buildingMatch));std::array<uint8_t,108> buildingDescriptor{};std::memcpy(buildingDescriptor.data(),descriptors49,108);
 auto* buildingEntry=ModelLightShadow20::registry.Observe(buildingMatch.identity,0,uint32_t(uintptr_t(descriptors49)),buildingMatch.modelInfo,buildingDescriptor,7);assert(buildingEntry);assert(ModelLightShadow20::registry.Publish(buildingEntry->key));initial49[0].mCastShadows=buildingEntry->key;
 Root49(0x1232c60,0);candidate=Run49(true,16);baseline=Run49(false,16);AssertSame49(baseline,candidate);assert(candidate[0].mFlags==0x44);Root49(0x1232c60,uintptr_t(op49));
 Configure49();initial49[0]={0x123456,0x44};initial49[1].mFlags=0x40;candidate=Run49(true,16);assert(candidate[0].mFlags==0x44&&candidate[1].mFlags==0x40);
 Configure49();baseline=Run49(false,1);const auto oneSerial=queries49;candidate=Run49(true,1);AssertSame49(baseline,candidate);assert(queries49==oneSerial);
 Configure49();baseline=Run49(false,2);candidate=Run49(true,2);AssertSame49(baseline,candidate);
 Configure49();ModelLightShadow20::lock.test_and_set();candidate=Run49(true,16);for(unsigned i=0;i<16;++i)assert(candidate[i].mFlags==0x40);ModelLightShadow20::lock.clear();
 Configure49();std::array<double,9> serialMs{},wideMs{};LARGE_INTEGER frequency{};QueryPerformanceFrequency(&frequency);
 for(unsigned rep=0;rep<9;++rep)for(bool wide:{false,true}){LARGE_INTEGER a{},b{};QueryPerformanceCounter(&a);auto v=Run49(wide);QueryPerformanceCounter(&b);for(auto l:v)assert(l.mFlags==0x44);(wide?wideMs:serialMs)[rep]=1000.0*double(b.QuadPart-a.QuadPart)/double(frequency.QuadPart);}
 std::sort(serialMs.begin(),serialMs.end());std::sort(wideMs.begin(),wideMs.end());std::printf("Offline warm fixture median over nine runs: serial %.3f ms; window %.3f ms. Not game frame time or FPS.\n",serialMs[4],wideMs[4]);
 for(auto p:{headers49,owners49,building49,flags49,model49,matrices49,descriptors49})assert(VirtualFree(p,0,MEM_RELEASE));
 std::puts("Actual staged queued owner/descriptor validation integration; per-source final relookup, mutation and fault containment passed.");
}
