#define NOMINMAX
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include "NativeClassDomains105.hpp"
#include "NativeProjectionRead66.hpp"
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
namespace reference81 {
using native_projection66::Projection;using native_projection66::Packet;using native_projection66::Values;
#include "alias81-reference.inc"
}
namespace Old91 {
using native_projection66::Packet;
alignas(64) uint8_t tokens[4096][64]{};
alignas(64) uint8_t classTokens105[3][4096][64]{};
native_class_domains105::Registry<> registry;
native_class_domains105::IssuedKeys<> issuedKeys;
thread_local reference81::AliasPass<> aliases;
thread_local native_projection66::Sidecar<> sidecar;
std::atomic<bool> active{true};std::atomic_flag lock=ATOMIC_FLAG_INIT;
std::atomic<uint32_t> accepted{},invalidQueued{},aliasPackets{},contended{},bankPreflights{},bankDenied{},packetFaults{},permissionQueries{},winnerPreflights{},winnerDenied{};
#include "projection66-read-extracted.inc"
#include "projection66-queue-extracted.inc"
}
namespace New91 {
using native_projection66::Packet;
alignas(64) uint8_t tokens[4096][64]{};
alignas(64) uint8_t classTokens105[3][4096][64]{};
native_class_domains105::Registry<> registry;
native_class_domains105::IssuedKeys<> issuedKeys;
thread_local native_projection66::AliasPass<> aliases;
thread_local native_projection66::Sidecar<> sidecar;
std::atomic<bool> active{true};std::atomic_flag lock=ATOMIC_FLAG_INIT;
std::atomic<uint32_t> accepted{},invalidQueued{},aliasPackets{},contended{},bankPreflights{},bankDenied{},packetFaults{},permissionQueries{},winnerPreflights{},winnerDenied{};
#include "projection66-read-extracted.inc"
#include "projection66-queue-extracted.inc"
}
native_projection66::Packet Light(){native_projection66::Packet p{};p[0]=std::bit_cast<uint32_t>(1.f);p[5]=std::bit_cast<uint32_t>(1.f);p[12]=p[13]=p[14]=p[16]=std::bit_cast<uint32_t>(1.f);p[17]=0;p[18]=0x260;p[21]=std::bit_cast<uint32_t>(20.f);p[25]=UINT32_MAX;return p;}

template<class Run> double Measure91(Run run,rage::CLightSource* bank,unsigned count){
 LARGE_INTEGER a{},b{},f{};QueryPerformanceFrequency(&f);QueryPerformanceCounter(&a);
 for(unsigned i=0;i<300;++i)run(bank,count);
 QueryPerformanceCounter(&b);return 1000.*double(b.QuadPart-a.QuadPart)/double(f.QuadPart)/300;
}
int main(){
 SetPriorityClass(GetCurrentProcess(),BELOW_NORMAL_PRIORITY_CLASS);
 assert(Old91::registry.Init({uint32_t(reinterpret_cast<uintptr_t>(Old91::classTokens105[0])),uint32_t(reinterpret_cast<uintptr_t>(Old91::tokens)),uint32_t(reinterpret_cast<uintptr_t>(Old91::classTokens105[1])),uint32_t(reinterpret_cast<uintptr_t>(Old91::classTokens105[2]))}));
 assert(New91::registry.Init({uint32_t(reinterpret_cast<uintptr_t>(New91::classTokens105[0])),uint32_t(reinterpret_cast<uintptr_t>(New91::tokens)),uint32_t(reinterpret_cast<uintptr_t>(New91::classTokens105[1])),uint32_t(reinterpret_cast<uintptr_t>(New91::classTokens105[2]))}));
 auto* oldBank=static_cast<rage::CLightSource*>(VirtualAlloc(nullptr,640*sizeof(rage::CLightSource),MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));
 auto* newBank=static_cast<rage::CLightSource*>(VirtualAlloc(nullptr,640*sizeof(rage::CLightSource),MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(oldBank&&newBank);
 for(unsigned i=0;i<640;++i){auto old=Light();old[8]=std::bit_cast<uint32_t>(float(i));auto now=old;
  const auto oldKey=Old91::registry.Observe(old,7,[](uint32_t){return false;});const auto newKey=New91::registry.Observe(now,7,[](uint32_t){return false;});
  assert(oldKey&&newKey&&Old91::issuedKeys.Publish(Old91::registry,oldKey)&&New91::issuedKeys.Publish(New91::registry,newKey));
  old[24]=oldKey;now[24]=newKey;old[18]|=4;now[18]|=4;
  std::memcpy(&oldBank[i],old.data(),sizeof(old));std::memcpy(&newBank[i],now.data(),sizeof(now));
 }
 for(unsigned count:{8u,32u,128u,500u,640u}){
  std::array<double,9> old{},now{};
  for(unsigned rep=0;rep<9;++rep){if(rep%2){old[rep]=Measure91(Old91::ValidateQueued,oldBank,count);now[rep]=Measure91(New91::ValidateQueued,newBank,count);}else{now[rep]=Measure91(New91::ValidateQueued,newBank,count);old[rep]=Measure91(Old91::ValidateQueued,oldBank,count);}}
  assert(Old91::sidecar.decisions==New91::sidecar.decisions);
  assert(Old91::aliases.alias==New91::aliases.alias&&Old91::invalidQueued==0&&New91::invalidQueued==0);
  std::sort(old.begin(),old.end());std::sort(now.begin(),now.end());
  std::printf("QUEUE sources%u baseline%.6fms candidate%.6fms\n",count,old[4],now[4]);
 }
 assert(VirtualFree(oldBank,0,MEM_RELEASE)&&VirtualFree(newBank,0,MEM_RELEASE));
 std::puts("Exact extracted production ValidateQueued/read/copy bodies, real bank preflight and packet reads included; offline callback timing, not game FPS.");
}
