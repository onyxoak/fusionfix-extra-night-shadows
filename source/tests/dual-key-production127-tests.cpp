#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "OwnedImageReads224.hpp"
#include <windows.h>
#include <psapi.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstring>
#include <mutex>
#include <vector>
#include <limits>
#include <cstdio>
#include "core/standalone-game-abi.hpp"
#include "core/ShadowAllocationCE.hpp"
#include "VehicleLampIdentity20.hpp"
#include "DualIssuedKeys127.hpp"
#include "ImmediateRegion33.hpp"
#include "FreshReadBatch30.hpp"
uintptr_t game{};
unsigned vehicleReads127{},collisionQueries127{};
uint32_t foreignKey127{};
namespace TrafficSignal {
 bool Readable(uintptr_t address,size_t bytes) noexcept {
  ++vehicleReads127;return immediate_region33::Readable(address,bytes);
 }
 bool ReadableOriginal(uintptr_t address,size_t bytes) noexcept {return Readable(address,bytes);}
 uint32_t KeyCollision(uint32_t key) noexcept {++collisionQueries127;return key==foreignKey127?1u:0u;}
}
namespace CShadows {
 uint32_t frame=100;uint32_t* pFrameCounter=&frame;
 struct Stable {std::mutex stateMutex;bool PrepareFrame(){return true;}
  struct Submitted {void Record(uint32_t,uint32_t){}} submitted;} gStableHeadlightShadow;
 using Native=void(*)(int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int);
 struct {Native fun{};} hbStoreStaticShadow;
}
namespace Liberty20 {
 std::atomic<bool> dualActive{true},worldActive{false};
 alignas(64) uint8_t keyStorage[16384][64]{};
 liberty20::VehicleLampRegistry<> lampRegistry;
 dual_keys127::PublishedKeys<> publishedKeys127;
 std::atomic_flag registryLock=ATOMIC_FLAG_INIT;
 std::atomic<uint32_t> splitPairs{},fallbacks{},contentions{},collisions{},worldEligible{},
  playerPairs{},staleQueued{},worldKeyed{},worldCached{},worldCacheMatched{},sourcePasses{};
 std::array<std::atomic<uint32_t>,4> worldFlags{};
 std::array<liberty20::Vec3,2> lastPositions{};uint32_t lastSlot{},lastReference{};
 struct Invocation {bool valid{};uint32_t owner{},frame{},stack{};
  std::array<liberty20::Vec3,2> positions{};liberty20::VehicleIdentity identity{};};
 thread_local Invocation invocation;
 #include "dual127-vehicle.inc"
 #include "dual127-publication.inc"
}
using namespace Liberty20;
std::array<int,16> input127{};
struct Submitted127 {std::array<int,16> words{};liberty20::Vec3 position{};};
std::vector<Submitted127> submissions127;
bool reenter127=false,entered127=false;
uint32_t KeyBase127(){return uint32_t(reinterpret_cast<uintptr_t>(keyStorage));}
void Native127(int a,int b,int c,int d,int e,int f,int g,int h,
 int i,int j,int k,int l,int m,int n,int o,int p){
 assert(publishedKeys127.ContainsPair(KeyBase127(),lampRegistry.slots[0].keys));
 assert(!registryLock.test_and_set(std::memory_order_acquire));registryLock.clear(std::memory_order_release);
 Submitted127 record{{a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p},{}};
 std::memcpy(&record.position,reinterpret_cast<const void*>(uint32_t(f)),sizeof(record.position));
 submissions127.push_back(record);
 if(reenter127&&!entered127){entered127=true;assert(Split(input127));}
}
rage::CLightSource Light127(uint32_t key){
 rage::CLightSource light{};light.mType=rage::LT_SPOT;light.mFlags=0x504;
 light.mCastShadows=int32_t(key);light.mShadowCacheIndex=-1;light.mRadius=20;
 light.mColor={.25f,.5f,.75f,1};light.mIntensity=8;light.mPosition={1,2,3};return light;
}
void Unchanged127(uint32_t key){auto light=Light127(key),before=light;
 vehicleReads127=0;PromoteNativeWorld(&light,1);assert(!memcmp(&light,&before,sizeof(light)));
 assert(vehicleReads127==0);
}
void Stripped127(uint32_t key){auto light=Light127(key),expected=light;
 expected.mFlags&=~4u;PromoteNativeWorld(&light,1);assert(!memcmp(&light,&expected,sizeof(light)));
}
int main(){
 SYSTEM_INFO system{};GetSystemInfo(&system);const auto page=size_t(system.dwPageSize);
 game=reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr,0xf00000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(game);
 auto* owner=static_cast<uint8_t*>(VirtualAlloc(nullptr,page*2,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(owner);
 auto* references=static_cast<uint8_t*>(VirtualAlloc(nullptr,page,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(references);
 references[0]=7;rage::VehiclePool pool{owner,references,1,0x1308,0,0};rage::VehiclePool* pointer=&pool;CVehicle::pool=&pointer;
 uint32_t model[16]{};model[15]=123;int16_t index=5;
 const auto modelAddress=uint32_t(reinterpret_cast<uintptr_t>(model));
 std::memcpy(owner+0x2e,&index,2);std::memcpy(reinterpret_cast<void*>(game+0xe95cd8+20),&modelAddress,4);
 uint32_t automobile=0;std::memcpy(owner+0x1304,&automobile,4);
 liberty20::VehicleIdentity identity{};assert(Vehicle(uint32_t(reinterpret_cast<uintptr_t>(owner)),identity));
 invocation={true,identity.owner,CShadows::frame,0,{{{1,2,3},{4,5,6}}},identity};
 assert(lampRegistry.Configure(KeyBase127()));
 bool headlights=true;bHeadlightShadows.value=&headlights;
 CShadows::hbStoreStaticShadow.fun=Native127;
 for(unsigned i=0;i<16;++i)input127[i]=int(100+i);
 input127[1]=2;input127[2]=0x504;input127[15]=int(identity.owner);
 float intensity=8;std::memcpy(&input127[7],&intensity,4);
 // Never-issued, unaligned, before/after arena addresses remain foreign.
 Unchanged127(KeyBase127());Unchanged127(KeyBase127()+1);
 Unchanged127(KeyBase127()-1);Unchanged127(KeyBase127()+sizeof(keyStorage));
 foreignKey127=KeyBase127();assert(!Split(input127));assert(submissions127.empty());
 assert(lampRegistry.used==2&&!publishedKeys127.Contains(KeyBase127(),foreignKey127));Unchanged127(foreignKey127);
 foreignKey127=KeyBase127()+3*64;assert(!Split(input127));assert(lampRegistry.used==4);Unchanged127(foreignKey127);
 // Real successful Split, including synchronous native reentry, owns BOTH
 // keys before either call and retains all original non-split input words.
 foreignKey127=0;reenter127=true;assert(Split(input127));reenter127=false;
 assert(submissions127.size()==4&&lampRegistry.used==6);
 const auto keys=lampRegistry.slots[0].keys;assert(publishedKeys127.ContainsPair(KeyBase127(),keys));
 for(const auto& record:submissions127){
  for(unsigned i=0;i<16;++i)if(i!=5&&i!=7&&i!=15)assert(record.words[i]==input127[i]);
  float actualIntensity{};std::memcpy(&actualIntensity,&record.words[7],4);assert(actualIntensity==4);
  const auto side=uint32_t(record.words[15])==keys[0]?0u:1u;
  assert(uint32_t(record.words[15])==keys[side]);assert(!memcmp(&record.position,&invocation.positions[side],sizeof(record.position)));
 }
 // Brightness164 feeds one already-dimmed native packet to this unchanged
 // production split; neither side applies another dim or changes payload.
 const auto dimBegin164=submissions127.size();intensity=8*.85f;std::memcpy(&input127[7],&intensity,4);
 assert(Split(input127));assert(submissions127.size()==dimBegin164+2);
 float total164=0;for(size_t j=dimBegin164;j<submissions127.size();++j){float side164{};std::memcpy(&side164,&submissions127[j].words[7],4);total164+=side164;assert(side164==intensity*.5f);}
 assert(total164==8*.85f);intensity=8;std::memcpy(&input127[7],&intensity,4);
 auto active=Light127(keys[0]),before=active;PromoteNativeWorld(&active,1);assert(!memcmp(&active,&before,sizeof(active)));
 const auto queriesBefore=collisionQueries127;assert(Split(input127));assert(collisionQueries127==queriesBefore);
 lampRegistry.Free(identity.pool,identity.owner,identity.slot);assert(publishedKeys127.Contains(KeyBase127(),keys[0]));Stripped127(keys[0]);
 // Refused finite/NaN inputs never publish; first later successful attempt
 // MUST repeat collision validation for the previously allocated pair.
 intensity=std::numeric_limits<float>::quiet_NaN();std::memcpy(&input127[7],&intensity,4);
 const auto callsBefore=submissions127.size();assert(!Split(input127));const auto unpublished=lampRegistry.slots[0].keys;
 assert(!publishedKeys127.Contains(KeyBase127(),unpublished[0]));Unchanged127(unpublished[0]);
 foreignKey127=unpublished[1];intensity=8;std::memcpy(&input127[7],&intensity,4);
 assert(!Split(input127));assert(submissions127.size()==callsBefore);Unchanged127(unpublished[1]);
 foreignKey127=0;assert(Split(input127));const auto fresh=lampRegistry.slots[0].keys;
 assert(fresh!=unpublished&&publishedKeys127.ContainsPair(KeyBase127(),fresh));
 references[0]=8;Stripped127(fresh[0]);references[0]=7;
 // Memory guard denial remains fresh, contained, and observable guard bit is
 // preserved. Ordinary foreign keys don't acquire new owner reads at all.
 DWORD old{};assert(VirtualProtect(references,page,PAGE_READWRITE|PAGE_GUARD,&old));
 Unchanged127(KeyBase127()+1);Stripped127(fresh[0]);
 MEMORY_BASIC_INFORMATION region{};assert(VirtualQuery(references,&region,sizeof(region)));assert(region.Protect&PAGE_GUARD);
 assert(VirtualProtect(references,page,PAGE_NOACCESS,&old));Stripped127(fresh[1]);
 assert(VirtualProtect(references,page,PAGE_READWRITE,&old));
 assert(!registryLock.test_and_set());Stripped127(fresh[0]);registryLock.clear();
 dualActive=false;Unchanged127(unpublished[0]);Stripped127(fresh[0]);assert(!Split(input127));
 PromoteNativeWorld(nullptr,1);PromoteNativeWorld(&active,4097);
 std::puts("PASS127 actual Split/Collision/Vehicle/Promote: foreign/burned keys preserved, atomic/reentrant publication, retired-issued/fresh generation/guard refusal, payload preserved");
 assert(VirtualFree(references,0,MEM_RELEASE));assert(VirtualFree(owner,0,MEM_RELEASE));assert(VirtualFree(reinterpret_cast<void*>(game),0,MEM_RELEASE));
}
