#define NOMINMAX
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cstring>
#include <cstdio>
#include "NativeClassDomains105.hpp"
#include "NativeProjectionRead66.hpp"
#include "ArcadePlacement197.hpp"
#include "FreshPageReadable.hpp"
#include "ImmediateRegion33.hpp"
#include "TrafficSignalNativeQueue.hpp"
#include "core/ShadowAllocationCE.hpp"
uint64_t keyReads{};unsigned regionQueries{};
struct Key {uint32_t value{};operator uint32_t() const noexcept {++keyReads;return value;}};
namespace rage {struct CLightSource {std::array<uint32_t,24> before;Key mCastShadows;std::array<uint32_t,7> after;};static_assert(sizeof(CLightSource)==128);}
uintptr_t game{};
namespace PlayerShadowAllocation {rage::CLightSource* completed{};rage::CLightSource* CurrentLights() noexcept{return completed;}}
namespace TrafficSignal {
bool ReadableOriginal(uintptr_t address,size_t size) noexcept {++regionQueries;return immediate_region33::Readable(address,size);}
bool Readable(uintptr_t address,size_t size) noexcept {return traffic_signal::fresh_page::Readable(address,size,ReadableOriginal);}
#include "projection66-collision-key-extracted.inc"
}
namespace Liberty20 {
#include "projection66-collision-extracted.inc"
}
namespace NativeProjection66 {
using native_projection66::Packet;
alignas(64) uint8_t tokens[4096][64]{};
alignas(64) uint8_t classTokens105[3][4096][64]{};
native_class_domains105::IssuedKeys<> issuedKeys;
native_class_domains105::Registry<> registry;std::atomic_flag lock=ATOMIC_FLAG_INIT;
std::atomic<uint32_t> contended{},capacityFallback{},tokensUsed74{};
std::array<std::atomic<uint32_t>,4> classTokensUsed105{};
arcade_placement197::Projections privateProjections197;
#include "projection66-intern-extracted.inc"
}
using namespace NativeProjection66;
Packet Light(unsigned i){Packet p{};p[0]=std::bit_cast<uint32_t>(1.f);p[5]=std::bit_cast<uint32_t>(1.f);
 p[8]=std::bit_cast<uint32_t>(float(i));p[12]=p[13]=p[14]=p[16]=std::bit_cast<uint32_t>(1.f);
 p[17]=0;p[18]=0x260;p[21]=std::bit_cast<uint32_t>(20.f);p[25]=UINT32_MAX;return p;}
int main(){
 constexpr unsigned count=512;constexpr size_t imageSize=0x1200000;
 game=reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr,imageSize,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(game);
 auto* bank=static_cast<rage::CLightSource*>(VirtualAlloc(nullptr,count*128,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(bank);
 auto* countAddress=reinterpret_cast<uint32_t*>(game+traffic_signal::SubmissionCountRva);
 *reinterpret_cast<rage::CLightSource**>(game+traffic_signal::SubmissionPointerRva)=bank;
 PlayerShadowAllocation::completed=bank+count;
 assert(registry.Init({uint32_t(reinterpret_cast<uintptr_t>(classTokens105[0])),uint32_t(reinterpret_cast<uintptr_t>(tokens)),uint32_t(reinterpret_cast<uintptr_t>(classTokens105[1])),uint32_t(reinterpret_cast<uintptr_t>(classTokens105[2]))}));memory_read46::Configure(true);
 unsigned originals=0,assigns=0;std::array<uint32_t,count> keys{};
 for(unsigned i=0;i<count;++i){const auto input=Light(i);
  const auto result=native_projection66::ConstructOwned(&bank[i],[&](void* owned){++originals;std::memcpy(owned,input.data(),0x78);return 0xabcddcbau;},
   [&](Packet& packet){keys[i]=Intern(packet,1);assert(keys[i]);},
   [&](void* output,const void* owned){++assigns;std::memcpy(output,owned,0x78);++*countAddress;});assert(result==0xabcddcbau);
 }
 auto readings=memory_read46::Snapshot();assert(originals==count&&assigns==count&&registry.Used()==count&&tokensUsed74.load()==count);
 assert(keyReads==uint64_t(count)*(count-1)/2); // Actual production producer scan.
 assert(readings[unsigned(memory_read46::Metric::ScalarHelpers)]==count-1);
 assert(readings[unsigned(memory_read46::Metric::ScalarOSQueries)]+regionQueries==count-1);
 std::printf("first512 actual Intern+LibertyCollision+TrafficKeyCollision: permission_queries=%llu key_reads=%llu native_ctor_calls=%u native_assignments=%u\n",
  readings[unsigned(memory_read46::Metric::ScalarOSQueries)]+regionQueries,keyReads,originals,assigns);
 const auto before=readings;const auto oldRegion=regionQueries;const auto oldKeys=keyReads;
 for(unsigned i=0;i<count;++i){auto packet=Light(i);assert(Intern(packet,2)==keys[i]);}
 readings=memory_read46::Snapshot();assert(readings==before&&regionQueries==oldRegion&&keyReads==oldKeys);
 std::puts("stable512 second generation: collision permission queries=0 key reads=0 (numeric registry lookup only)");
 assert(VirtualFree(bank,0,MEM_RELEASE));assert(VirtualFree(reinterpret_cast<void*>(game),0,MEM_RELEASE));
}
