#define NOMINMAX
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cstring>
#include <cstdio>
#include "FreshPageReadable.hpp"
#include "NativeClassDomains105.hpp"
#include "ArcadePlacement197.hpp"
#include <psapi.h>
BOOL WINAPI Query203(HANDLE,PVOID,DWORD);
SIZE_T WINAPI Region203(LPCVOID,PMEMORY_BASIC_INFORMATION,SIZE_T);
#undef QueryWorkingSetEx
#define QueryWorkingSetEx Query203
#define VirtualQuery Region203
#include "NativeProjectionRead66.hpp"
#undef VirtualQuery
#undef QueryWorkingSetEx
#define QueryWorkingSetEx K32QueryWorkingSetEx
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
arcade_placement197::Projections privateProjections197;
bool (*placementBank197)(rage::CLightSource&,Packet&,uint32_t) noexcept=nullptr;
thread_local native_projection66::AliasPass<> aliases;
thread_local native_projection66::Sidecar<> sidecar;
std::atomic<bool> active{true};std::atomic_flag lock=ATOMIC_FLAG_INIT;
std::atomic<uint32_t> accepted{},invalidQueued{},aliasPackets{},contended{},bankPreflights{},bankDenied{},packetFaults{},permissionQueries{},winnerPreflights{},winnerDenied{};
#include "projection66-read-extracted.inc"
#include "projection66-queue-extracted.inc"
}
using namespace NativeProjection66;
native_projection66::Packet Light(){Packet p{};p[0]=std::bit_cast<uint32_t>(1.f);p[5]=std::bit_cast<uint32_t>(1.f);p[12]=p[13]=p[14]=p[16]=std::bit_cast<uint32_t>(1.f);p[17]=0;p[18]=0x260;p[21]=std::bit_cast<uint32_t>(20.f);p[25]=UINT32_MAX;return p;}
namespace Reference203 {
using namespace NativeProjection66;
#include "packet203-reference-read.inc"
#include "packet203-reference-queue.inc"
}
unsigned qws203{},vm203{},pages203{},mode203{};uintptr_t expectedPages203[2]{};
void* revoke203{};bool dirtyFp203=false;
BOOL WINAPI Query203(HANDLE process,PVOID buffer,DWORD size){
 ++qws203;assert(size==sizeof(PSAPI_WORKING_SET_EX_INFORMATION)||size==2*sizeof(PSAPI_WORKING_SET_EX_INFORMATION));
 pages203=size/sizeof(PSAPI_WORKING_SET_EX_INFORMATION);auto* info=static_cast<PSAPI_WORKING_SET_EX_INFORMATION*>(buffer);
 for(unsigned i=0;i<pages203;++i)if(expectedPages203[i])assert(uintptr_t(info[i].VirtualAddress)==expectedPages203[i]);
 if(!mode203)return QueryWorkingSetEx(process,buffer,size);
 for(unsigned i=0;i<pages203;++i){info[i].VirtualAttributes.Flags=0;info[i].VirtualAttributes.Valid=1;info[i].VirtualAttributes.Win32Protection=PAGE_READWRITE;}
 if(mode203==2)info[0].VirtualAttributes.Win32Protection=PAGE_READWRITE|PAGE_GUARD;
 if(mode203==3)info[0].VirtualAttributes.Win32Protection=PAGE_NOACCESS;
 if(mode203==4)info[0].VirtualAttributes.Valid=0;
 if(mode203==6)info[0].VirtualAttributes.Locked=1;
 if(mode203==7)info[0].VirtualAttributes.LargePage=1;
 if(mode203==8)info[0].VirtualAttributes.Bad=1;
 if(mode203==9)info[0].VirtualAttributes.Win32Protection=PAGE_EXECUTE;
 if(mode203==10)info[0].VirtualAttributes.Win32Protection=PAGE_READWRITE|PAGE_NOCACHE;
 if(mode203==11){assert(pages203==2);info[1].VirtualAttributes.Win32Protection=PAGE_READWRITE|PAGE_GUARD;}
 if(mode203==12){assert(pages203==2);info[1].VirtualAttributes.Win32Protection=PAGE_NOACCESS;}
 if(mode203==13){DWORD old{};assert(revoke203&&VirtualProtect(revoke203,4096,PAGE_NOACCESS,&old));}
 if(mode203==14)info[0].VirtualAttributes.Flags|=ULONG_PTR(1u<<24); // unspecified reserved bit
 if(dirtyFp203){_mm_setcsr(0x3f80);__asm fninit}
 SetLastError(0x11223344);return mode203!=5;
}
SIZE_T WINAPI Region203(LPCVOID p,PMEMORY_BASIC_INFORMATION info,SIZE_T size){
 ++vm203;auto result=VirtualQuery(p,info,size);
 if(dirtyFp203){_mm_setcsr(0x3f80);__asm fninit}
 SetLastError(0x55667788);return result;
}
DWORD BadPage203()noexcept{_mm_setcsr(0x3f80);SetLastError(0xabcdef);return 0;}
void Reset203(unsigned mode){qws203=vm203=pages203=0;mode203=mode;expectedPages203[0]=expectedPages203[1]=0;revoke203=nullptr;}
int main(){
 assert(registry.Init({uint32_t(reinterpret_cast<uintptr_t>(classTokens105[0])),uint32_t(reinterpret_cast<uintptr_t>(tokens)),uint32_t(reinterpret_cast<uintptr_t>(classTokens105[1])),uint32_t(reinterpret_cast<uintptr_t>(classTokens105[2]))}));
 auto* bytes=static_cast<uint8_t*>(VirtualAlloc(nullptr,3*4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(bytes);
 memset(bytes,0,3*4096);auto* one=reinterpret_cast<rage::CLightSource*>(bytes+128);auto* cross=reinterpret_cast<rage::CLightSource*>(bytes+4096-60);
 auto p=Light();auto key=registry.Observe(p,7,[](uint32_t){return false;});assert(key&&issuedKeys.Publish(registry,key));p[24]=key;p[18]|=4;
 memcpy(one,p.data(),128);memcpy(cross,p.data(),128);
 for(auto* light:{one,cross}){
  const bool crossing=light==cross;Reset203(1);expectedPages203[0]=uintptr_t(bytes);if(crossing)expectedPages203[1]=uintptr_t(bytes+4096);
  Packet copied{};unsigned queries{};assert(CopyPacket(*light,copied,true,&queries)&&copied==p&&qws203==1&&vm203==0&&queries==1&&pages203==(crossing?2u:1u));
  assert(CopyPacket(*light,copied)&&qws203==2); // Each request fresh, including an enclosing legacy operation.
  {traffic_signal::fresh_page::Operation44 op;traffic_signal::fresh_page::Remember44(uintptr_t(light),4096,Query203);assert(CopyPacket(*light,copied)&&qws203==3);}
  Reset203(14);assert(CopyPacket(*light,copied)&&qws203==1&&vm203==0);
  for(unsigned mode=2;mode<=12;++mode){if(!crossing&&mode>=11)continue;Reset203(mode);dirtyFp203=true;
   const bool denied=mode==2||mode==3||mode==11||mode==12;
   SetLastError(0x203203);_mm_setcsr(0x5f80);unsigned short cw=0x0b7f,after{};__asm fldcw cw
   const auto allowed=CopyPacket(*light,copied,true,&queries);__asm fnstcw after
   assert(allowed!=denied&&GetLastError()==0x203203&&_mm_getcsr()==0x5f80&&after==cw);
   assert(qws203==1&&vm203==(denied?0u:1u)&&queries==qws203+vm203);dirtyFp203=false;
  }
  Reset203(1);SetLastError(0x203203);_mm_setcsr(0x5f80);
  auto result=native_projection66::PacketPreflight203(uintptr_t(light),Query203,Region203,BadPage203);
  assert(result.allowed&&qws203==0&&vm203==1&&GetLastError()==0x203203&&_mm_getcsr()==0x5f80);
  // Whole-bank path remains original region validation, then fresh winner witnesses.
  Reset203(1);CShadows::frame=7;ValidateQueued(light,1);assert(qws203==0&&vm203==1);
  const auto expected=Reference203::CollectDecision(0,light,1,7,0x2030);assert(expected==1&&vm203==2&&qws203==0);
  const auto current=CollectDecision(0,light,1,7,0x2030);assert(current==expected&&qws203==1&&vm203==2);
  for(auto field:native_projection66::WitnessFields){
   auto* words=reinterpret_cast<uint32_t*>(light);words[field]^=1;
   assert(CollectDecision(0,light,1,7,0x2030)==Reference203::CollectDecision(0,light,1,7,0x2030));words[field]^=1;
  }
  assert(CollectDecision(0,light,1,8,0x2030)==Reference203::CollectDecision(0,light,1,8,0x2030));
  assert(CollectDecision(0,light,1,7,0x2034)==Reference203::CollectDecision(0,light,1,7,0x2034));
 }
 Reset203(1);Packet copied{};assert(CopyPacket(*one,copied,false)&&qws203==0&&vm203==0); // bank preflight caller path unchanged
 // Permission revocation after successful QWS never bypasses the actual SEH read.
 Reset203(13);revoke203=bytes;unsigned faults=packetFaults.load();assert(!CopyPacket(*one,copied)&&packetFaults==faults+1&&qws203==1&&vm203==0);
 DWORD old{};assert(VirtualProtect(bytes,4096,PAGE_READWRITE,&old));
 Reset203(1);ValidateQueued(one,1);Reset203(13);revoke203=bytes;
 assert(CollectDecision(0,one,1,7,0x2030)==3&&qws203==1&&vm203==0);
 assert(Reference203::CollectDecision(0,one,1,7,0x2030)==3&&vm203==1);
 assert(VirtualProtect(bytes,4096,PAGE_READWRITE,&old));
 one->mCastShadows=0;Reset203(1);ValidateQueued(one,1);Reset203(13);revoke203=bytes;
 assert(CollectDecision(0,one,1,7,0x2030)==0&&Reference203::CollectDecision(0,one,1,7,0x2030)==0);
 assert(VirtualProtect(bytes,4096,PAGE_READWRITE,&old));memcpy(one,p.data(),128);
 // Real current guard/noaccess on either crossed page remains unconsumed.
 for(DWORD protection:{DWORD(PAGE_NOACCESS),DWORD(PAGE_READWRITE|PAGE_GUARD)}){
  assert(VirtualProtect(bytes+4096,4096,protection,&old));Reset203(0);auto beforeReads=Liberty20::packetReads;
  assert(!CopyPacket(*cross,copied)&&Liberty20::packetReads==beforeReads);
  MEMORY_BASIC_INFORMATION region{};assert(VirtualQuery(bytes+4096,&region,sizeof(region))&&region.Protect==protection);
  assert(VirtualProtect(bytes+4096,4096,PAGE_READWRITE,&old));
 }
 // Invalid address/overflow and disabled classification do not query or read.
 Reset203(1);assert(!native_projection66::PacketPreflight203(0).allowed&&!native_projection66::PacketPreflight203(UINTPTR_MAX-60).allowed&&qws203==0&&vm203==0);
 active=false;auto beforeReads=Liberty20::packetReads;assert(CollectDecision(0,one,1,7,0x2030)==3&&qws203==0&&vm203==0&&Liberty20::packetReads==beforeReads);active=true;
 assert(VirtualFree(bytes,0,MEM_RELEASE));
 puts("203 actual CopyPacket/CollectDecision: 196 parity, all112 witness bytes, one/two fresh pages, guard/noaccess, Invalid/failure/exotic original-region fallback, SEH revocation, no cache, FP/LastError and exact QWS/VM counts passed");
}
