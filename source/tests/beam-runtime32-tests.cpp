#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "OwnedImageReads224.hpp"
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
#include "core/ShadowAdapterCE.hpp"
#include "VehicleLampIdentity20.hpp"
#include "DualIssuedKeys127.hpp"
#include "ImmediateRegion33.hpp"
#include "FreshReadBatch30.hpp"
namespace rage {struct VehiclePool {uint8_t* m_aStorage;uint8_t* m_aFlags;int32_t m_nSize,m_nStorageSize,m_nFirstFreeIndex,m_numSlotsUsed;};}
std::vector<uint8_t> image(0xe95cd8+100);uintptr_t game=reinterpret_cast<uintptr_t>(image.data());
uint32_t frame=100;namespace CShadows {uint32_t* pFrameCounter=&frame;}
rage::VehiclePool* poolPointer{};namespace CVehicle {rage::VehiclePool** pool=&poolPointer;}
unsigned reads{};
unsigned batchQueries36{};
BOOL WINAPI CountQuery36(HANDLE process,PVOID entries,DWORD bytes){++batchQueries36;return QueryWorkingSetEx(process,entries,bytes);}
#define CE_VEHICLE_BATCH_QUERY36 CountQuery36
namespace TrafficSignal {bool Readable(uintptr_t address,size_t bytes) noexcept {
 ++reads;MEMORY_BASIC_INFORMATION info{};
 if(address<0x10000||!bytes||address>UINTPTR_MAX-bytes||!VirtualQuery(reinterpret_cast<void*>(address),&info,sizeof(info))||info.State!=MEM_COMMIT||(info.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return false;
 auto base=reinterpret_cast<uintptr_t>(info.BaseAddress);return address>=base&&address-base<=info.RegionSize&&bytes<=info.RegionSize-(address-base);
}bool ReadableOriginal(uintptr_t address,size_t bytes) noexcept {return Readable(address,bytes);}}
namespace Liberty20 {
std::atomic<bool> dualActive{true};std::atomic_flag registryLock=ATOMIC_FLAG_INIT;std::atomic<unsigned> contentions{};
alignas(64) uint8_t keyStorage[16384][64]{};liberty20::VehicleLampRegistry<> lampRegistry;
dual_keys127::PublishedKeys<> publishedKeys127;
#include "beam-runtime32-extracted.inc"
}
int main(){
 traffic_signal::fresh_page::readScopes233=false;
 SYSTEM_INFO sys{};GetSystemInfo(&sys);const auto page=sys.dwPageSize;
 auto owner=static_cast<uint8_t*>(VirtualAlloc(nullptr,page*2,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(owner);
 uint8_t flags[1]{7};rage::VehiclePool pool{owner,flags,1,0x1308,0,0};poolPointer=&pool;
 uint32_t model[16]{};model[15]=123;int16_t index=5;auto modelPointer=uint32_t(reinterpret_cast<uintptr_t>(model));
 std::memcpy(owner+0x2e,&index,2);std::memcpy(image.data()+0xe95cd8+index*4,&modelPointer,4);
 uint32_t type36=0;std::memcpy(owner+0x1304,&type36,4); // Resident fast-path fixture; cold pages retain fallback tests.
 liberty20::VehicleIdentity identity{};reads=0;batchQueries36=0;assert(Liberty20::Vehicle(uint32_t(reinterpret_cast<uintptr_t>(owner)),identity));
 assert(reads==3&&batchQueries36==2);std::cout<<"Vehicle36: 3 scalar + 2 batch queries; previous production path used 9 scalar queries.\n";
 const auto baselineIdentity233=identity;const auto baselineReads233=reads,baselineQueries233=batchQueries36;
 traffic_signal::fresh_page::readScopes233=true;reads=0;batchQueries36=0;
 assert(Liberty20::Vehicle(uint32_t(reinterpret_cast<uintptr_t>(owner)),identity)&&identity==baselineIdentity233);
 assert(reads<=baselineReads233&&batchQueries36<=baselineQueries233);
 std::cout<<"Callback read scope preserves the actual fresh vehicle identity; query calls "<<baselineQueries233<<" -> "<<batchQueries36<<".\n";
 assert(Liberty20::lampRegistry.Configure(uint32_t(reinterpret_cast<uintptr_t>(Liberty20::keyStorage))));auto pair=Liberty20::lampRegistry.Observe(identity,frame);assert(pair);auto key=pair->keys[0];
 // Allocated-but-unpublished is not our source identity or caster authority.
 reads=0;assert(!Liberty20::VehicleBeam(key,identity.owner));
 assert(!Liberty20::BeamOwner(key));assert(!Liberty20::BeamOrder(key));assert(reads==0);
 assert(Liberty20::publishedKeys127.PublishPair(uint32_t(reinterpret_cast<uintptr_t>(Liberty20::keyStorage)),pair->keys));
 using namespace fusionfix::shadows::ce;
 AdditionalVehicleBeam=Liberty20::VehicleBeam;AdditionalVehicleBeamOwner=Liberty20::BeamOwner;AdditionalVehicleBeamOrder=Liberty20::BeamOrder;
 reads=0;auto legacy=ResolveBeamSelection32(key,identity.owner);auto oldReads=reads;
 AdditionalVehicleBeamPriority32=Liberty20::BeamPriority32;reads=0;auto combined=ResolveBeamSelection32(key,identity.owner);auto newReads=reads;
 assert(legacy.own&&combined.own&&legacy.order==combined.order&&combined.privateOwner==identity.owner);
 assert(oldReads==newReads*2&&newReads>0);
 std::cout<<"Actual own-headlamp selection: guarded reads "<<oldReads<<" -> "<<newReads<<".\n";
 AdditionalVehicleBeamPriority32=nullptr;reads=0;legacy=ResolveBeamSelection32(key,identity.owner+0x1308);oldReads=reads;
 AdditionalVehicleBeamPriority32=Liberty20::BeamPriority32;reads=0;combined=ResolveBeamSelection32(key,identity.owner+0x1308);newReads=reads;
 // Old wrong-car test rejects before Vehicle(), so both traffic paths validate once.
 assert(!legacy.own&&!combined.own&&legacy.privateOwner==combined.privateOwner&&oldReads==newReads);
 flags[0]=8;auto reused=ResolveBeamSelection32(key,identity.owner);assert(!reused.own&&!reused.privateOwner&&!reused.order);flags[0]=7;
 Liberty20::lampRegistry.Free(identity.pool,identity.owner,identity.slot);auto freed=ResolveBeamSelection32(key,identity.owner);assert(!freed.own&&!freed.privateOwner);
 assert(Liberty20::publishedKeys127.Contains(uint32_t(reinterpret_cast<uintptr_t>(Liberty20::keyStorage)),key));
 reads=0;assert(!Liberty20::BeamOwner(key+1));assert(!Liberty20::VehicleBeam(key+1,identity.owner));assert(reads==0);
 assert(VirtualFree(owner,0,MEM_RELEASE));std::cout<<"Traffic equivalence, current reference changes, and freed keys rejected.\n";
}
