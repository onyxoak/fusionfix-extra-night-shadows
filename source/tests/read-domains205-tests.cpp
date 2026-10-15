#include <optional>
#include <memory>
#include "VehicleLampIdentity20.hpp"
#define main ExistingNativeFixtureMain205
#include "native-owner-batch115-generated.cpp"
#undef main
namespace TrafficSignal {
#include "owner204-wrappers205.inc"
}
rage::VehiclePool** vehicleRoot205{};
namespace CVehicle {auto& pool=vehicleRoot205;}
#define CE_VEHICLE_BATCH_QUERY36 CountQuery49
namespace Liberty20 {
#include "vehicle205-extracted.inc"
#include "vehicle204-wrapper205.inc"
}
namespace fp205=traffic_signal::fresh_page;
struct VehicleFixture205 {
 uint8_t *root{},*header{},*owner{},*flags{},*model{};
 VehicleFixture205(){
  auto alloc=[](unsigned bytes){auto* p=static_cast<uint8_t*>(VirtualAlloc(nullptr,bytes,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(p);std::memset(p,0,bytes);return p;};
  root=alloc(Page49);header=alloc(Page49);owner=alloc(Page49*2);flags=alloc(Page49);model=alloc(Page49);
  vehicleRoot205=reinterpret_cast<rage::VehiclePool**>(root);*vehicleRoot205=reinterpret_cast<rage::VehiclePool*>(header);
  **vehicleRoot205={owner,flags,1,0x1308,0,0};flags[0]=7;const int16_t index=7;
  std::memcpy(owner+0x2e,&index,2);const uint32_t hash=777;std::memcpy(model+0x3c,&hash,4);Root49(0xe95cd8+7*4,uintptr_t(model));
 }
 ~VehicleFixture205(){for(auto p:{root,header,owner,flags,model})assert(VirtualFree(p,0,MEM_RELEASE));}
};
struct Observation205 {bool accepted{},copied{};TrafficSignal::Match match{};liberty20::VehicleIdentity vehicle{};std::array<uint8_t,108> bytes{};unsigned queries{},pages{},vm{};};
unsigned invocation205{};bool clobber205{},nested205{},nestedSeen205{};uintptr_t trigger205{},revoke205{};DWORD protection205{};void(*mutation205)(){};bool hookSeen205{};
void Hook205(PSAPI_WORKING_SET_EX_INFORMATION* entries,DWORD count){
 if(clobber205){SetLastError(0xdead);ClobberAllocatorFp59();}
 if(nested205){nested205=false;const auto* prior=fp205::operation44;TrafficSignal::Match match{};
  assert(TrafficSignal::MatchOwner(uintptr_t(owners49),match));assert(fp205::operation44==prior);nestedSeen205=true;}
 if(!trigger205)return;
 bool found=false;for(unsigned i=0;i<count;++i)found=found||uintptr_t(entries[i].VirtualAddress)/Page49==trigger205/Page49;
 if(!found)return;trigger205=0;hookSeen205=true;
 if(revoke205){DWORD prior{};assert(VirtualProtect(reinterpret_cast<void*>(revoke205),Page49,protection205,&prior));}
 if(mutation205)mutation205();
}
Observation205 Call205(bool candidate,unsigned kind,uintptr_t owner,uintptr_t descriptor=0){
 Observation205 out;out.bytes.fill(0xee);queries49=pages49=virtualQueries115=0;
 auto* previous=fp205::operation44;auto* previousRegion=immediate_region33::current;
 if(kind==0)out.accepted=candidate?TrafficSignal::MatchOwner(owner,out.match):TrafficSignal::MatchOwner204(owner,out.match);
 if(kind==1)out.accepted=candidate?TrafficSignal::MatchOwnerWithDescriptor48(owner,out.match,descriptor,out.bytes.data(),out.bytes.size(),out.copied):TrafficSignal::MatchOwnerWithDescriptor48204(owner,out.match,descriptor,out.bytes.data(),out.bytes.size(),out.copied);
 if(kind==2)out.accepted=candidate?Liberty20::Vehicle(uint32_t(owner),out.vehicle):Liberty20::Vehicle204(uint32_t(owner),out.vehicle);
 assert(fp205::operation44==previous&&immediate_region33::current==previousRegion);
 out.queries=queries49;out.pages=pages49;out.vm=virtualQueries115;return out;
}
void Same205(const Observation205& old,const Observation205& now){
 assert(old.accepted==now.accepted&&old.copied==now.copied&&old.bytes==now.bytes);
 if(old.accepted){assert(old.match.identity==now.match.identity&&old.match.kind==now.match.kind&&old.match.modelInfo==now.match.modelInfo&&old.match.ownerPose109==now.match.ownerPose109);
  assert(std::memcmp(old.match.entity,now.match.entity,sizeof(old.match.entity))==0);assert(old.vehicle==now.vehicle);}
}
NativeCorpus115* native205{};VehicleFixture205* car205{};unsigned change205{};
void Change205(){
 auto& n=*native205;
 if(change205==1)n.flags5[0]^=1;
 if(change205==2)n.models[0x3c]^=1;
 if(change205==3){auto* p=*reinterpret_cast<rage::VehiclePool**>(game+native_owner109::PoolRva);p->m_aStorage+=0x80;}
 if(change205==4)car205->flags[0]^=1;
 if(change205==5)std::memset(car205->model+0x3c,0,4);
 if(change205==6)(*vehicleRoot205)->m_nSize=2;
}
void Checks205(){
 NativeCorpus115 corpus(4,false);native205=&corpus;VehicleFixture205 car;car205=&car;
 traceQueries115=false;corpus.Restore();shadow_cpu29::enabled=false;memory_read46::enabled=true;
 const uintptr_t owners[]{uintptr_t(owners49),uintptr_t(building49),uintptr_t(corpus.owners5),uintptr_t(corpus.owners1+2*0x110)};
 for(unsigned kind=0;kind<3;++kind)for(unsigned slot=0;slot<(kind==2?1u:4u);++slot){
  const auto owner=kind==2?uintptr_t(car.owner):owners[slot];
  auto old=Call205(false,kind,owner,uintptr_t(corpus.descriptors));auto now=Call205(true,kind,owner,uintptr_t(corpus.descriptors));Same205(old,now);assert(now.accepted&&now.queries<=old.queries&&now.pages<=old.pages);
  std::printf("205 kind%u class%u QWS %u->%u pages %u->%u VM %u->%u\n",kind,slot,old.queries,now.queries,old.pages,now.pages,old.vm,now.vm);
  // Existing parent domains remain the exact same object; wrapper must neither
  // install a nested domain nor erase its already authenticated entries.
  for(unsigned capacity:{8u,32u})for(bool candidate:{false,true}){fp205::Operation44 outer(capacity);const auto warm=Call205(candidate,kind,owner,uintptr_t(corpus.descriptors));assert(warm.accepted&&outer.count);
   const auto count=outer.count;const auto repeat=Call205(candidate,kind,owner,uintptr_t(corpus.descriptors));Same205(warm,repeat);assert(outer.count==count&&outer.capacity==capacity);if(capacity==32)assert(repeat.queries==0);}
 }
 // Outside a wrapper is the return-to-native boundary: no permission object
 // survives, so the following invocation queries anew and observes new data.
 for(bool candidate:{false,true}){
  auto first=Call205(candidate,1,owners[2],uintptr_t(corpus.descriptors));assert(first.accepted&&first.copied&&!fp205::operation44);
  corpus.flags5[0]=9;corpus.models[0x3c]^=1;corpus.descriptors[3]^=1;
  auto changed=Call205(candidate,1,owners[2],uintptr_t(corpus.descriptors));assert(changed.accepted&&changed.queries>0&&changed.match.identity.reference==9&&changed.match.identity.model!=first.match.identity.model&&changed.bytes!=first.bytes);
  corpus.flags5[0]=7;corpus.models[0x3c]^=1;corpus.descriptors[3]^=1;
  auto carFirst=Call205(candidate,2,uintptr_t(car.owner));car.flags[0]=9;
  auto carChanged=Call205(candidate,2,uintptr_t(car.owner));assert(carFirst.accepted&&carChanged.accepted&&carChanged.vehicle.reference==9&&carChanged.queries>0);car.flags[0]=7;
 }
 // Mutations after positive detail/model permission are caught by actual
 // current generation/model/header rereads, even when their pages are known.
 for(unsigned change=1;change<=6;++change){Observation205 pair[2];
  for(unsigned candidate=0;candidate<2;++candidate){
   const auto savedHeader=**reinterpret_cast<rage::VehiclePool**>(game+native_owner109::PoolRva);const auto savedCar=**vehicleRoot205;uint32_t savedHash{};std::memcpy(&savedHash,car.model+0x3c,4);
   change205=change;mutation205=Change205;trigger205=change<=3?uintptr_t(corpus.models):uintptr_t(car.model);hookSeen205=false;nativeQueryHook115=Hook205;
   pair[candidate]=Call205(candidate!=0,change<=3?0:2,change<=3?owners[2]:uintptr_t(car.owner));
   // The model mutation occurs before its first byte copy, so both versions
   // legitimately accept the same new hash; earlier lifetime reads must reject.
   assert(hookSeen205&&pair[candidate].accepted==(change==2));
   nativeQueryHook115=nullptr;mutation205=nullptr;corpus.flags5[0]=7;if(change==2)corpus.models[0x3c]^=1;
   **reinterpret_cast<rage::VehiclePool**>(game+native_owner109::PoolRva)=savedHeader;**vehicleRoot205=savedCar;car.flags[0]=7;std::memcpy(car.model+0x3c,&savedHash,4);
  }Same205(pair[0],pair[1]);
 }
 // Known-page revocation remains contained by the actual Stage/SEH leaves.
 for(unsigned kind:{0u,1u,2u})for(DWORD protection:{DWORD(PAGE_NOACCESS),DWORD(PAGE_READWRITE|PAGE_GUARD)}){
  Observation205 pair[2];for(unsigned candidate=0;candidate<2;++candidate){
   revoke205=kind==2?uintptr_t(car.flags):uintptr_t(corpus.owners5);trigger205=kind==2?uintptr_t(car.model):uintptr_t(corpus.models);
   protection205=protection;hookSeen205=false;nativeQueryHook115=Hook205;
   pair[candidate]=Call205(candidate!=0,kind,kind==2?uintptr_t(car.owner):owners[2],uintptr_t(corpus.descriptors));assert(hookSeen205&&!pair[candidate].accepted);
   nativeQueryHook115=nullptr;DWORD prior{};assert(VirtualProtect(reinterpret_cast<void*>(revoke205),Page49,PAGE_READWRITE,&prior));revoke205=0;
  }Same205(pair[0],pair[1]);
 }
 // A descriptor revoked immediately after its positive initial query cannot
 // be reauthorized by the independent descriptor fallback or publish bytes.
 for(DWORD protection:{DWORD(PAGE_NOACCESS),DWORD(PAGE_READWRITE|PAGE_GUARD)})for(bool candidate:{false,true}){
  trigger205=revoke205=uintptr_t(corpus.descriptors);protection205=protection;nativeQueryHook115=Hook205;hookSeen205=false;
  const auto out=Call205(candidate,1,owners[0],uintptr_t(corpus.descriptors));assert(hookSeen205&&out.accepted&&!out.copied);for(auto byte:out.bytes)assert(byte==0xee);
  nativeQueryHook115=nullptr;DWORD prior{};assert(VirtualProtect(corpus.descriptors,Page49,PAGE_READWRITE,&prior));revoke205=0;
 }
 // Initially guarded pages are refused before access, leaving guard intact.
 for(bool candidate:{false,true}){DWORD prior{};assert(VirtualProtect(car.flags,Page49,PAGE_READWRITE|PAGE_GUARD,&prior));
  assert(!Call205(candidate,2,uintptr_t(car.owner)).accepted);MEMORY_BASIC_INFORMATION info{};assert(VirtualQuery(car.flags,&info,sizeof(info))&&info.Protect&PAGE_GUARD);
  assert(VirtualProtect(car.flags,Page49,PAGE_READWRITE,&prior));}
 // Query failure uses the original region path. No failed observation enters
 // Operation44; both wrapper decisions and bytes remain identical.
 failWs115=true;for(unsigned kind=0;kind<3;++kind){auto old=Call205(false,kind,kind==2?uintptr_t(car.owner):owners[2],uintptr_t(corpus.descriptors));auto now=Call205(true,kind,kind==2?uintptr_t(car.owner):owners[2],uintptr_t(corpus.descriptors));Same205(old,now);assert(now.vm>0);}failWs115=false;
 // Nested validator invocations borrow an existing domain and restore TLS.
 nested205=true;nestedSeen205=false;nativeQueryHook115=Hook205;assert(Call205(true,0,owners[2]).accepted&&nestedSeen205&&!fp205::operation44);nativeQueryHook115=nullptr;
 // Query-side FP/LastError damage must remain inside existing preservation.
 for(unsigned kind=0;kind<3;++kind)for(bool candidate:{false,true}){
  clobber205=true;nativeQueryHook115=Hook205;SetLastError(0x205);alignas(16) unsigned char before[512]{},after[512]{};_fxsave(before);
  const auto out=Call205(candidate,kind,kind==2?uintptr_t(car.owner):owners[2],uintptr_t(corpus.descriptors));_fxsave(after);
  assert(out.accepted&&GetLastError()==0x205&&std::memcmp(before,after,5)==0&&std::memcmp(before+24,after+24,8)==0&&std::memcmp(before+32,after+32,128)==0);
  clobber205=false;nativeQueryHook115=nullptr;
 }
 memory_read46::enabled=false;
 std::puts("205 actual wrappers: sealed204 decision/identity/descriptor parity, outer-domain reuse, fresh between-call bytes, generation/header/model changes, guard/revocation, descriptor no-retry, nested TLS, FP controls/x87 stack and LastError passed. No game/FPS proof.");
}
#ifndef READ_DOMAINS205_LIBRARY
int main(){std::setvbuf(stdout,nullptr,_IONBF,0);Checks205();return 0;}
#endif
