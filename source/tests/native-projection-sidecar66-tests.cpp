#define NOMINMAX
#include "OwnedImageReads224.hpp"
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cstring>
#include "NativeClassDomains105.hpp"
#include "NativeProjectionRead66.hpp"
#include "ArcadePlacement197.hpp"
#include "core/ShadowCpuProfile29.hpp"
#include "core/SceneShadowPriority21.hpp"
namespace rage {struct CLightSource {std::array<uint32_t,24> before;uint32_t mCastShadows;std::array<uint32_t,7> after;};static_assert(sizeof(CLightSource)==128);}
namespace CShadows {uint32_t frame=7;uint32_t* pFrameCounter=&frame;}
namespace Liberty20 {
#define CopyLocal CopyLocalActual
#include "projection66-copy-extracted.inc"
#undef CopyLocal
 unsigned packetReads{};
 bool CopyLocal(uintptr_t address,void* output,size_t size) noexcept {
  ++packetReads;return CopyLocalActual(address,output,size);
 }
}
namespace NativeProjection66 {
using native_projection66::Packet;
alignas(64) uint8_t tokens[4096][64]{};
alignas(64) uint8_t classTokens105[3][4096][64]{};
native_class_domains105::Registry<> registry;
native_class_domains105::IssuedKeys<> issuedKeys;
thread_local native_projection66::AliasPass<> aliases;
thread_local native_projection66::Sidecar<> sidecar;
std::atomic<bool> active{true};std::atomic_flag lock=ATOMIC_FLAG_INIT;
std::atomic<uint32_t> accepted{},invalidQueued{},aliasPackets{},contended{},bankPreflights{},bankDenied{},packetFaults{},permissionQueries{},winnerPreflights{},winnerDenied{};
arcade_placement197::Projections privateProjections197;
#include "projection66-read-extracted.inc"
bool (*placementBank197)(rage::CLightSource&,Packet&,uint32_t) noexcept=nullptr;
#include "projection66-queue-extracted.inc"
}
using namespace NativeProjection66;
native_projection66::Packet Light(){Packet p{};p[0]=std::bit_cast<uint32_t>(1.f);p[5]=std::bit_cast<uint32_t>(1.f);p[12]=p[13]=p[14]=p[16]=std::bit_cast<uint32_t>(1.f);p[17]=0;p[18]=0x260;p[21]=std::bit_cast<uint32_t>(20.f);p[25]=UINT32_MAX;return p;}
int main(){
 assert(registry.Init({uint32_t(reinterpret_cast<uintptr_t>(classTokens105[0])),uint32_t(reinterpret_cast<uintptr_t>(tokens)),uint32_t(reinterpret_cast<uintptr_t>(classTokens105[1])),uint32_t(reinterpret_cast<uintptr_t>(classTokens105[2]))}));
 constexpr unsigned count=512;const auto size=count*sizeof(rage::CLightSource);
 auto* bank=static_cast<rage::CLightSource*>(VirtualAlloc(nullptr,size,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(bank);
 for(unsigned i=0;i<count;++i){auto p=Light();p[8]=std::bit_cast<uint32_t>(float(i));auto key=registry.Observe(p,7,[](uint32_t){return false;});assert(key);assert(issuedKeys.Publish(registry,key));p[24]=key;p[18]|=4;std::memcpy(&bank[i],p.data(),sizeof(p));}
 std::array<rage::CLightSource,count> before{};std::memcpy(before.data(),bank,size);
 SetLastError(0x12345678);ValidateQueued(bank,count);assert(GetLastError()==0x12345678);assert(accepted==count);
 assert(permissionQueries==1&&bankPreflights==1);
 for(unsigned i=0;i<count;++i)assert(Classify(i,bank[i].mCastShadows,bank,count,7)==1);
 assert(permissionQueries==1); // All512 numeric classifications: zero OS queries.
 for(unsigned i=0;i<7;++i)assert(CollectDecision(i,bank,count,7,0x1000)==1);
 assert(permissionQueries==8); // Fresh permissions ONLY for actual would-writers.
 assert(std::memcmp(before.data(),bank,size)==0);
 // Native-current accepted bank is presence authority across producer epochs.
 CShadows::frame=8;ValidateQueued(bank,count);assert(CollectDecision(0,bank,count,8,0x2000)==1);
 // Reentry/stack/bank/count/frame mismatch never retains authorization.
 assert(CollectDecision(0,bank,count,8,0x2004)==3);
 assert(CollectDecision(0,bank,count-1,8,0x2000)==3);
 assert(CollectDecision(0,bank,count,9,0x2000)==3);
 lock.test_and_set();ValidateQueued(bank,count);lock.clear();assert(CollectDecision(0,bank,count,8,0x2000)==3);
 assert(std::memcmp(before.data(),bank,size)==0);
 DWORD old{};assert(VirtualProtect(bank,4096,PAGE_NOACCESS,&old));ValidateQueued(bank,count);
 assert(VirtualProtect(bank,4096,old,&old));
 assert(CollectDecision(0,bank,count,8,0x2000)==3);assert(std::memcmp(before.data(),bank,size)==0);
 // Production helper walks legal cross-region banks, preserving every packet.
 assert(VirtualProtect(bank,4096,PAGE_READONLY,&old));ValidateQueued(bank,count);
 assert(CollectDecision(0,bank,count,8,0x2000)==1);assert(VirtualProtect(bank,4096,old,&old));
 // Identical projection contributes independently; only duplicate map insertion is bypassed.
 bank[1]=bank[0];ValidateQueued(bank,count);assert(CollectDecision(0,bank,count,8,0x3000)==1);
 assert(CollectDecision(1,bank,count,8,0x3000)==2);assert(Alias(1,bank[1].mCastShadows,bank,count,8));
 auto* words=reinterpret_cast<uint32_t*>(&bank[0]);words[8]^=1;
 assert(CollectDecision(0,bank,count,8,0x3000)==3);words[8]^=1;
 assert(VirtualProtect(bank,4096,PAGE_NOACCESS,&old));
 assert(CollectDecision(0,bank,count,8,0x3000)==3); // Actual production SEH copy fault.
 assert(VirtualProtect(bank,4096,old,&old));
 // Old whole-bank permission evidence never bypasses a fresh guard-page check.
 assert(VirtualProtect(bank,4096,PAGE_READWRITE|PAGE_GUARD,&old));
 SetLastError(0x12345678);assert(CollectDecision(0,bank,count,8,0x3000)==3);assert(GetLastError()==0x12345678);
 MEMORY_BASIC_INFORMATION guarded{};assert(VirtualQuery(bank,&guarded,sizeof(guarded)));
 assert(guarded.Protect&PAGE_GUARD);assert(VirtualProtect(bank,4096,old,&old));
 active=false;assert(CollectDecision(1,bank,count,8,0x3000)==3);
 const auto disabledQueries=permissionQueries.load();const auto disabledReads=Liberty20::packetReads;
 std::memcpy(before.data(),bank,size);ValidateQueued(bank,count);
 assert(permissionQueries==disabledQueries&&Liberty20::packetReads==disabledReads);
 assert(Classify(0,bank[0].mCastShadows,bank,count,8)==3);
 assert(CollectDecision(0,bank,count,8,0x3000)==3);
 assert(permissionQueries==disabledQueries&&Liberty20::packetReads==disabledReads);
 assert(std::memcmp(before.data(),bank,size)==0);
 active=true;
 // Full512 stationary photometry changes reuse projection keys while retaining
 // fresh112 per-packet witnesses. The native bank remains actual illumination.
 const auto warmKeys=registry.Used();std::array<uint32_t,count> stable{};
 for(unsigned i=0;i<count;++i)stable[i]=bank[i].mCastShadows;
 for(unsigned step=0;step<60;++step){
  for(unsigned i=0;i<count;++i){Packet p{};std::memcpy(p.data(),&bank[i],0x78);p[18]&=~4u;p[24]=0;
   p[16]=std::bit_cast<uint32_t>(1.f+float(step)*.01f);p[12]=std::bit_cast<uint32_t>(1.f+float(step)*.003f);
   const auto key=registry.Observe(p,100+step,[](uint32_t){assert(false);return true;});assert(key==stable[i]);
   p[24]=key;p[18]|=4u;std::memcpy(&bank[i],p.data(),0x78);
  }
  CShadows::frame=100+step;ValidateQueued(bank,count);assert(registry.Used()==warmKeys);
  auto* current=reinterpret_cast<uint32_t*>(&bank[0]);current[16]^=1;
  assert(CollectDecision(0,bank,count,CShadows::frame,0x4000)==3);current[16]^=1;
 }
 // Native foreign keys inside allocated-but-unissued storage remain ordinary.
 auto foreign=Light();foreign[8]=std::bit_cast<uint32_t>(1000000.f);
 const auto burnedKey=registry.domains[2].keyBase+registry.domains[2].used*64;
 assert(!registry.Observe(foreign,200,[](uint32_t){return true;}));
 assert(!OwnKey(burnedKey));
 for(const auto foreignKey:{burnedKey,registry.domains[2].keyBase+4095*64}){
  bank[0].mCastShadows=foreignKey;std::memcpy(before.data(),bank,size);
  CShadows::frame=200;ValidateQueued(bank,count);
  assert(Classify(0,foreignKey,bank,count,200)==0);
  assert(!Alias(0,foreignKey,bank,count,200));
  assert(CollectDecision(0,bank,count,200,0x5000)==0);
  assert(std::memcmp(before.data(),bank,size)==0);
 }
 // Actual queue/consumer integration accepts separately issued arcade keys,
 // and a retiring callback cannot leave a stale sidecar insertion lease.
 privateProjections197.Init();auto repaired=Light();repaired[18]=0x41;repaired[8]=std::bit_cast<uint32_t>(-34.36047f);
 const auto repairKey=privateProjections197.Intern(repaired,201,[](uint32_t){return false;});assert(repairKey&&OwnKey(repairKey));
 std::memcpy(&bank[0],repaired.data(),sizeof(repaired));CShadows::frame=201;
 ValidateQueued(bank,1);assert(CollectDecision(0,bank,1,201,0x6000)==1);
 placementBank197=[](rage::CLightSource& light,Packet& p,uint32_t)noexcept{
  p[8]=std::bit_cast<uint32_t>(-34.49764f);p[18]&=~4u;p[24]=0;std::memcpy(&light,p.data(),0x78);return false;
 };
 assert(CollectDecision(0,bank,1,201,0x6000)==3);assert(bank[0].mCastShadows==0);
 std::memcpy(&bank[0],repaired.data(),sizeof(repaired));ValidateQueued(bank,1);
 assert(bank[0].mCastShadows==0&&CollectDecision(0,bank,1,201,0x6000)==0);
 placementBank197=nullptr;
 assert(VirtualFree(bank,0,MEM_RELEASE));
}

