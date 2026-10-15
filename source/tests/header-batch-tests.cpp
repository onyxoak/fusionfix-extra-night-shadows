#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <intrin.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstring>
#include <functional>
#include <iostream>
#include <random>
#include <tuple>
#include <vector>
#include "TrafficSignalRegistry.hpp"
namespace rage {struct VehiclePool {uint8_t* m_aStorage;uint8_t* m_aFlags;int32_t m_nSize,m_nStorageSize,m_nFirstFreeIndex,m_numSlotsUsed;};}
static_assert(offsetof(rage::VehiclePool,m_nSize)==8&&offsetof(rage::VehiclePool,m_nStorageSize)==12);
std::vector<std::pair<uintptr_t,size_t>> queryTrace;
std::vector<std::pair<uintptr_t,unsigned>> headerTrace;
std::function<void(uintptr_t,size_t)> queryHook;
std::function<void(uintptr_t,unsigned)> headerHook;
SIZE_T CountQuery(const void* address,MEMORY_BASIC_INFORMATION* info,SIZE_T size){
 const auto result=VirtualQuery(address,info,size);queryTrace.push_back({reinterpret_cast<uintptr_t>(address),0});
 if(queryHook)queryHook(reinterpret_cast<uintptr_t>(address),result);return result;
}
uint32_t TestWord(uintptr_t address,unsigned offset) noexcept {
 headerTrace.push_back({address,offset});if(headerHook)headerHook(address,offset);
 return *reinterpret_cast<const volatile uint32_t*>(address+offset);
}
#define CE_POOL_HEADER_WORD(address,offset) TestWord(address,offset)
#include "PoolDescriptorSnapshots.hpp"
#undef CE_POOL_HEADER_WORD
#define VirtualQuery CountQuery
struct SafetyHookContext {uintptr_t esp{};};
namespace fusionfix::shadows {struct FloatingPointState {};}
namespace experiment_perf {void Count(bool enabled,std::atomic<uint32_t>& value){if(enabled)++value;}}
uint32_t frameValue=100;namespace CShadows {uint32_t* pFrameCounter=&frameValue;}
std::vector<unsigned char> gameBytes(0x1232c64);
uintptr_t game=reinterpret_cast<uintptr_t>(gameBytes.data());
struct Match {traffic_signal::Identity identity{};uint32_t kind{};float entity[3]{};};
struct Record {uint32_t event{},index=UINT32_MAX,tick{},frame{},previous{},caller{},thread{},kind=UINT32_MAX;float entity[3]{};
 friend bool operator==(const Record&,const Record&)=default;};
struct Snapshot {std::atomic<uint32_t> state{0};};
uint32_t MockTick(){return 12345;}uint32_t MockThread(){return 42;}
#define GetTickCount MockTick
#define GetCurrentThreadId MockThread
#define ENVIRONMENT \
 std::atomic<bool> enabled{true},fatal{false};std::atomic<uint32_t> observerThread{42}; \
 bool diagnosticsEnabled=true,hotCountersEnabled=true;traffic_signal::Registry<8> registry;std::array<Snapshot,8> snapshots;std::atomic<uint32_t> timerRebases{0};std::vector<Record> events; \
 void FatalThread(){fatal=true;}void Emit(Record r){events.push_back(r);} \
 void RecordEvent(uint32_t event,uint32_t index,uint32_t frame){Record r{};r.event=event;r.index=index;r.frame=frame;r.tick=MockTick();r.thread=MockThread();events.push_back(r);} \
 void Reset(){enabled=true;fatal=false;observerThread=42;registry={};assert(registry.Configure(0x100000,0));timerRebases=0;events.clear();for(auto&s:snapshots)s.state=0;}
namespace accepted {ENVIRONMENT
#include "header-accepted-extracted.inc"
}
namespace candidate {ENVIRONMENT
#include "header-candidate-extracted.inc"
}
#undef ENVIRONMENT
#undef VirtualQuery
#undef GetTickCount
#undef GetCurrentThreadId
std::vector<void*> allocations;
size_t page{};
void* Allocate(size_t bytes){auto* p=VirtualAlloc(nullptr,bytes,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);assert(p);allocations.push_back(p);return p;}
void Protect(void* p,size_t bytes,DWORD flags){DWORD previous{};assert(VirtualProtect(p,bytes,flags,&previous));}
uintptr_t Address(void* p){return reinterpret_cast<uintptr_t>(p);}
struct Fixture {
 unsigned char *headers,*owner,*buildingOwner,*flags,*model,*matrix,*newFlags;
 rage::VehiclePool *building,*object;uintptr_t globalObject()const{return game+0x1232c60;}
 Fixture(){headers=(unsigned char*)Allocate(page*3);owner=(unsigned char*)Allocate(page*2);buildingOwner=(unsigned char*)Allocate(page);
  flags=(unsigned char*)Allocate(page);model=(unsigned char*)Allocate(page);matrix=(unsigned char*)Allocate(page);newFlags=(unsigned char*)Allocate(page);Reset();}
 void Root(unsigned rva,uintptr_t p){std::memcpy(gameBytes.data()+rva,&p,4);}
 void Reset(unsigned gap=32,bool reversed=false){
  queryHook={};headerHook={};queryTrace.clear();headerTrace.clear();
  for(void* p:allocations)Protect(p,p==headers?page*3:p==owner?page*2:page,PAGE_READWRITE);
  std::memset(headers,0,page*3);std::memset(owner,0,page*2);std::memset(buildingOwner,0,page);
  std::memset(flags,1,page);std::memset(newFlags,2,page);std::memset(model,0,page);std::memset(matrix,0,page);
  building=reinterpret_cast<rage::VehiclePool*>(headers+(reversed?gap:0));
  object=reinterpret_cast<rage::VehiclePool*>(headers+(reversed?0:gap));
  *building={buildingOwner,flags,1,0x70,0,0};*object={owner,flags,4,0x320,0,0};
  Root(0xebd0e8,Address(building));Root(0x1232c60,Address(object));
  const uintptr_t matrixAddress=Address(matrix);std::memcpy(owner+0x20,&matrixAddress,4);const int16_t index=5;std::memcpy(owner+0x2e,&index,2);
  const auto info=Address(model);std::memcpy(gameBytes.data()+0xe95cd8+5*4,&info,4);const uint32_t hash=123;std::memcpy(model+0x3c,&hash,4);
  for(unsigned i=0;i<3;++i)reinterpret_cast<float*>(matrix)[12+i]=float(i+1);
  frameValue=100;std::memcpy(gameBytes.data()+0xd73684,&frameValue,4);gameBytes[0xd73610]=1;
 }
};
struct Result {bool success{},fault{};Match match{};std::vector<std::pair<uintptr_t,size_t>> queries;std::vector<std::pair<uintptr_t,unsigned>> words;};
bool Contained(bool(*fn)(uintptr_t,Match&) noexcept,uintptr_t owner,Match* out,bool* success){
 __try {*success=fn(owner,*out);return false;}
 __except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION||GetExceptionCode()==EXCEPTION_GUARD_PAGE?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){return true;}
}
Result Run(bool optimized,uintptr_t owner){
 queryTrace.clear();headerTrace.clear();Result r;r.match.kind=77;r.match.entity[0]=88;
 r.fault=Contained(optimized?candidate::MatchOwner:accepted::MatchOwner,owner,&r.match,&r.success);r.queries=queryTrace;r.words=headerTrace;return r;
}
bool Equal(const Result&a,const Result&b){return a.success==b.success&&a.fault==b.fault&&a.match.identity==b.match.identity&&a.match.kind==b.match.kind&&
 !std::memcmp(a.match.entity,b.match.entity,sizeof(a.match.entity));}
unsigned differential=0,protection=0,mutation=0;
void StableCases(Fixture& f){
 std::mt19937 random(0x145918);
 for(unsigned n=0;n<2048;++n){f.Reset();const auto type=n%19;uintptr_t owner=Address(f.owner);
  if(type==1)f.building->m_aStorage=f.owner;
  if(type==2){f.building->m_aStorage=f.owner;f.object->m_nStorageSize=0;}
  if(type==3)f.object->m_nSize=0;
  if(type==4)f.object->m_nSize=-1;
  if(type==5)f.object->m_nSize=65537;
  if(type==6)f.object->m_nStorageSize=0x321;
  if(type==7)f.building->m_nSize=0;
  if(type==8)f.building->m_nStorageSize=0;
  if(type==9)f.flags[0]=0x80;
  if(type==10)*reinterpret_cast<int16_t*>(f.owner+0x2e)=-1;
  if(type==11)*reinterpret_cast<int16_t*>(f.owner+0x2e)=31000;
  if(type==12)*reinterpret_cast<uint32_t*>(f.model+0x3c)=0;
  if(type==13)reinterpret_cast<float*>(f.matrix)[12]=NAN;
  if(type==14)owner++;
  if(type==15)f.object->m_aStorage=f.owner+0x320;
  if(type==16)f.object->m_aFlags=nullptr;
  if(type==17)*reinterpret_cast<uint32_t*>(f.owner+0x20)=0;
  if(type==18)f.flags[0]=static_cast<unsigned char>(random()%128);
  auto a=Run(false,owner),b=Run(true,owner);assert(Equal(a,b));assert(!a.fault);
  if(type==1||type==2){assert(b.queries.size()==a.queries.size()+1);for(auto [p,off]:b.words)assert(p!=Address(f.object));}
  else assert(b.queries.size()+1==a.queries.size());
  if(type==3||type==4||type==5){for(auto [p,off]:b.words)if(p==Address(f.object))assert(off==8);}
  if(type==6){for(auto [p,off]:b.words)if(p==Address(f.object))assert(off==8||off==12);}
  if(type==12)for(auto [p,off]:b.queries)assert(p!=Address(f.matrix));
  ++differential;
 }
 for(auto [gap,reverse]:std::array<std::pair<unsigned,bool>,5>{{{32,true},{228,false},{232,false},{0,false},{page,false}}}){
  f.Reset(gap,reverse);if(!gap){f.object->m_nStorageSize=0x320;}
  auto a=Run(false,Address(f.owner)),b=Run(true,Address(f.owner));assert(Equal(a,b));
  if(gap>228)assert(a.queries.size()==b.queries.size());else assert(b.queries.size()+1==a.queries.size());++differential;
 }
 // The full original 0x1c extents must fit. At a protection boundary, union
 // cannot be authorized and the original two fresh descriptor queries run.
 f.Reset();f.building=reinterpret_cast<rage::VehiclePool*>(f.headers+page-32);f.object=reinterpret_cast<rage::VehiclePool*>(f.headers+page);
 *f.building={f.buildingOwner,f.flags,1,0x70,0,0};*f.object={f.owner,f.flags,4,0x320,0,0};
 f.Root(0xebd0e8,Address(f.building));f.Root(0x1232c60,Address(f.object));Protect(f.headers+page,page,PAGE_READONLY);
 auto a=Run(false,Address(f.owner)),b=Run(true,Address(f.owner));assert(Equal(a,b)&&a.success);assert(b.queries.size()==a.queries.size()+1);++differential;
}
void ProtectionCases(Fixture&f){
 for(bool objectProtected:{false,true})for(DWORD p:{DWORD(PAGE_NOACCESS),DWORD(PAGE_READWRITE|PAGE_GUARD)}){
  f.Reset();f.building=reinterpret_cast<rage::VehiclePool*>(f.headers+page-32);f.object=reinterpret_cast<rage::VehiclePool*>(f.headers+page);
  *f.building={f.buildingOwner,f.flags,1,0x70,0,0};*f.object={f.owner,f.flags,4,0x320,0,0};
  f.Root(0xebd0e8,Address(f.building));f.Root(0x1232c60,Address(f.object));
  auto* protectedPage=f.headers+(objectProtected?page:0);Protect(protectedPage,page,p);
  auto a=Run(false,Address(f.owner)),b=Run(true,Address(f.owner));assert(Equal(a,b));assert(!a.fault);
  MEMORY_BASIC_INFORMATION info{};assert(VirtualQuery(protectedPage,&info,sizeof(info)));assert(info.Protect==p);++protection;
 }
 f.Reset();Protect(f.matrix,page,PAGE_NOACCESS);*reinterpret_cast<uint32_t*>(f.model+0x3c)=0;
 auto a=Run(false,Address(f.owner)),b=Run(true,Address(f.owner));assert(Equal(a,b)&&!a.success);
 for(auto [p,s]:b.queries)assert(p!=Address(f.matrix));++protection;
 // Deterministic post-query header fault: accepted faults; candidate returns
 // validation failure with unchanged output and no object snapshot published.
 for(DWORD p:{DWORD(PAGE_NOACCESS),DWORD(PAGE_READWRITE|PAGE_GUARD)}){
  f.Reset();bool changed=false;queryHook=[&](uintptr_t address,size_t result){if(!changed&&address==Address(f.building)&&result){changed=true;Protect(f.headers,page,p);}};
  a=Run(false,Address(f.owner));assert(a.fault);f.Reset();changed=false;
  queryHook=[&](uintptr_t address,size_t result){if(!changed&&address==Address(f.building)&&result){changed=true;Protect(f.headers,page,p);}};
  b=Run(true,Address(f.owner));assert(!b.fault&&!b.success&&b.match.kind==77&&b.match.entity[0]==88);++protection;
 }
 // Fault during second header read discards the partially completed first stage.
 f.Reset();bool changed=false;headerHook=[&](uintptr_t address,unsigned offset){if(!changed&&address==Address(f.object)&&offset==8){changed=true;Protect(f.headers,page,PAGE_NOACCESS);}};
 b=Run(true,Address(f.owner));assert(!b.fault&&!b.success&&b.match.kind==77);++protection;f.Reset();
}
void MutationCases(Fixture&f){
 for(unsigned change=0;change<3;++change){
  std::array<Result,2> result;
  for(unsigned variant=0;variant<2;++variant){f.Reset();bool changed=false;
   queryHook=[&](uintptr_t address,size_t){if(!changed&&address==Address(f.matrix)){changed=true;
    if(change==0)++f.flags[0];else if(change==1)f.object->m_aFlags=f.newFlags;else *reinterpret_cast<uint32_t*>(f.model+0x3c)=321;}};
   result[variant]=Run(variant==1,Address(f.owner));
  }
  if(change==0)assert(Equal(result[0],result[1])&&!result[1].success);
  if(change==1){assert(!result[0].success&&result[1].success);assert(result[1].match.identity.flags==Address(f.newFlags));assert(result[1].queries.size()==11);} // explicit restart to fresh original state
  if(change==2){assert(Equal(result[0],result[1]));}++mutation;
 }
 // Object root changes during the short building read: restart original with
 // fresh pointers before any object snapshot/native body read.
 f.Reset();auto* replacement=reinterpret_cast<rage::VehiclePool*>(f.headers+128);*replacement=*f.object;bool changed=false;
 headerHook=[&](uintptr_t address,unsigned offset){if(!changed&&address==Address(f.building)&&offset==8){changed=true;f.Root(0x1232c60,Address(replacement));}};
 auto b=Run(true,Address(f.owner));assert(b.success&&b.match.identity.pool==Address(replacement));assert(b.queries.size()==7);
 for(auto[p,off]:b.words)assert(p!=Address(f.object));++mutation;f.Reset();
 // Fresh subsequent invocations observe reference/model/root changes, including
 // replacement at the same address; keys/retirement use the real Registry.
 for(unsigned change=0;change<4;++change){
  f.Reset();auto first=Run(false,Address(f.owner));assert(first.success);accepted::Reset();candidate::Reset();
  for(auto* registry:{&accepted::registry,&candidate::registry}){auto r=registry->Observe(first.match.identity,100,0,true);assert(r.index==0);}
  accepted::snapshots[0].state=1;candidate::snapshots[0].state=1;
  if(change==0)++f.flags[0];if(change==1)++*reinterpret_cast<uint32_t*>(f.model+0x3c);
  if(change==2){auto* next=reinterpret_cast<rage::VehiclePool*>(f.headers+128);*next=*f.object;f.Root(0x1232c60,Address(next));}
  if(change==3){f.object->m_aFlags=f.newFlags;}
  uint32_t caller=0x921029;SafetyHookContext context{Address(&caller)};
  accepted::NativeTimerRestore(context);candidate::NativeTimerRestore(context);
  assert(accepted::registry.entries[0].retired&&candidate::registry.entries[0].retired);
  assert(accepted::registry.entries[0].reason==traffic_signal::End::Identity&&candidate::registry.entries[0].reason==traffic_signal::End::Identity);
  assert(accepted::snapshots[0].state==2&&candidate::snapshots[0].state==2&&accepted::timerRebases==0&&candidate::timerRebases==0);
  assert(accepted::events==candidate::events);++mutation;
 }
 // Native free/reuse: old token remains retired; fresh lifetime receives a new
 // token, including allocation-reference ABA, per the unchanged registry.
 f.Reset();auto first=Run(true,Address(f.owner));traffic_signal::Registry<8> registry;assert(registry.Configure(0x100000,0));
 auto old=registry.Observe(first.match.identity,100,0,true);assert(registry.Free(first.match.identity.pool,first.match.identity.owner)==old.index);
 f.flags[0]=2;auto fresh=Run(true,Address(f.owner));auto next=registry.Observe(fresh.match.identity,100,0,true);assert(next.index!=old.index&&registry.entries[old.index].retired);++mutation;
}
unsigned checkCalls=0;
bool NeverCheck(uintptr_t,size_t) noexcept{++checkCalls;return false;}
void Bounds(){
 using namespace traffic_signal::pool_descriptors;Header h{};uintptr_t object=0x20020;
 for(auto[b,o]:std::array<std::pair<uintptr_t,uintptr_t>,5>{{{0,object},{0xffff,object},{UINTPTR_MAX-16,object},{0x20000,UINTPTR_MAX-16},{0x20000,0x21000}}}){
  checkCalls=0;assert(FreshRejectedBuilding(b,o,0,Address(&object),NeverCheck,h)==Attempt::Fallback&&checkCalls==0);
 }
 checkCalls=0;assert(FreshRejectedBuilding(0x20000,0x200e4,0,Address(&object),NeverCheck,h)==Attempt::Fallback&&checkCalls==1);
 checkCalls=0;assert(FreshRejectedBuilding(0x20000,0x200e8,0,Address(&object),NeverCheck,h)==Attempt::Fallback&&checkCalls==0);
}
int main(){static_assert(sizeof(void*)==4);SYSTEM_INFO s{};GetSystemInfo(&s);page=s.dwPageSize;Fixture f;StableCases(f);ProtectionCases(f);MutationCases(f);Bounds();
 queryHook={};headerHook={};for(void* p:allocations)assert(VirtualFree(p,0,MEM_RELEASE));
 std::cout<<"PASS "<<differential<<" stable differential scenarios; "<<protection<<" protection/fault cases; "<<mutation<<" lifecycle/restart cases; overflow/union bounds. Fast ordinary object:6->5 fresh queries; building/boundary fallback:+1; unsuitable layout:+0; flags restart5+6=11. Original NativeTimerRestore no-rebase retirement exercised. No GTA access.\n";}
