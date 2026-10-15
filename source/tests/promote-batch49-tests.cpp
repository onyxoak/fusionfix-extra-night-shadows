#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <bit>
#include "ModelLightSubmission20.hpp"
#include "ModelEffectObserverState.hpp"
#include "ModelLightRegistry20.hpp"
#include "ModelOwnerTransformDiagnostic46.hpp"
#include "ModelEffectGetterDiagnostic62.hpp"
#include <cstring>
#include <iostream>
#include <vector>
#include <optional>
#include "TrafficSignalIdentity.hpp"
#include "PoolDescriptorSnapshots.hpp"
#include "ImmediateRegion33.hpp"
#include "NativeOwnerClass109.hpp"
#include "NativeOwnerClass114.hpp"
#include "FreshReadGroup50.hpp"
#include "FreshReadBatch30.hpp"
namespace rage {struct VehiclePool {uint8_t* m_aStorage;uint8_t* m_aFlags;int32_t m_nSize,m_nStorageSize,m_nFirstFreeIndex,m_numSlotsUsed;};}
std::vector<uint8_t> image(0x1232c64);
uintptr_t game=reinterpret_cast<uintptr_t>(image.data());
struct Match {traffic_signal::Identity identity{};uint32_t kind{};float entity[3]{};uint32_t modelInfo{};std::array<uint8_t,68> ownerPose109{};};
void* revokeAddress{};size_t revokeSize{};bool inject{};DWORD injectedProtection=PAGE_NOACCESS;
unsigned scalarQueries35{},batchQueries35{};
bool Readable(uintptr_t address,size_t bytes) noexcept {
 ++scalarQueries35;
 MEMORY_BASIC_INFORMATION info{};
 bool ok=address>=0x10000&&bytes&&address<=UINTPTR_MAX-bytes&&VirtualQuery(reinterpret_cast<void*>(address),&info,sizeof(info))&&info.State==MEM_COMMIT&&!(info.Protect&(PAGE_NOACCESS|PAGE_GUARD));
 if(ok){auto base=reinterpret_cast<uintptr_t>(info.BaseAddress);ok=address>=base&&address-base<=info.RegionSize&&bytes<=info.RegionSize-(address-base);}
 if(ok&&inject&&address==reinterpret_cast<uintptr_t>(revokeAddress)){DWORD old{};assert(VirtualProtect(revokeAddress,revokeSize,injectedProtection,&old));inject=false;}
 return ok;
}
bool ReadableOriginal(uintptr_t address,size_t bytes) noexcept{return Readable(address,bytes);}
BOOL WINAPI InjectQuery35(HANDLE process,PVOID buffer,DWORD size){
 ++batchQueries35;
 auto ok=QueryWorkingSetEx(process,buffer,size);
 auto* entries=static_cast<PSAPI_WORKING_SET_EX_INFORMATION*>(buffer);
 if(ok&&inject)for(unsigned i=0;i<size/sizeof(*entries);++i)
  if(entries[i].VirtualAddress==revokeAddress){DWORD old{};assert(VirtualProtect(revokeAddress,revokeSize,injectedProtection,&old));inject=false;break;}
 return ok;
}
#define CE_OWNER_BATCH_QUERY35 InjectQuery35
#include "owner-runtime32-extracted.inc"
namespace TrafficSignal {using ::Match;using ::MatchOwner;using ::MatchOwnerWithDescriptor48;}
struct SafetyHookContext{uint32_t esp{};};
std::atomic<bool> addonActive{true};
namespace PlayerShadowAllocation{bool Enabled() noexcept{return true;}}
namespace Liberty20{
 bool Collision(uint32_t) noexcept{return false;}
 bool Copy(uintptr_t p,void* output,size_t size) noexcept{
  const traffic_signal::fresh_batch30::Span span{p,output,size};return OwnerCopy35(&span,1);
 }
}
namespace ModelLightShadow20 {
 bool requested=true;std::atomic<bool> active{true};std::atomic_flag lock=ATOMIC_FLAG_INIT;
 model_light20::Registry<16> registry;std::atomic<uint32_t> identitiesPublished32{0},refused{0},promoted{0};
 #include "promote-batch49-extracted.inc"
 #include "promote-baseline49-extracted.inc"
}
namespace CoronaLight20{bool requested=false;}
namespace ModelEffectObserver{
 using model_effect_observer::Invocation;
 struct Sample {std::array<uint8_t,68> ownerPose109{};bool diagnostic=false,ownerMatched{},modelInfoMatched{},descriptorCopied{},matrixCopied{};traffic_signal::Identity identity{};uint32_t ownerKind{};std::array<uint8_t,0x6c> descriptor{};std::array<float,16> matrix{};model_owner_transform46::Snapshot ownerTransform{};model_effect_getter62::Snapshot getter62{};};
 struct State {bool invalid{};void Invalidate(){invalid=true;}};
 struct Local {State state;} local;
 unsigned contextRejected{},readRejected{};
 bool Copy(uintptr_t address,void* bytes,size_t size) noexcept{return Liberty20::Copy(address,bytes,size);}
 #include "promote-source49-extracted.inc"
}
void Root(unsigned offset,uintptr_t address){std::memcpy(image.data()+offset,&address,4);}
int main(){
 SYSTEM_INFO sys{};GetSystemInfo(&sys);auto page=sys.dwPageSize;
 auto allocate=[&](){auto p=static_cast<uint8_t*>(VirtualAlloc(nullptr,page,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(p);return p;};
 auto headers=allocate(),owner=allocate(),buildingOwner=allocate(),flags=allocate(),model=allocate(),matrix=allocate();
 auto building=reinterpret_cast<rage::VehiclePool*>(headers),object=building+1;
 *building={buildingOwner,flags,1,0x70,0,0};*object={owner,flags,4,0x320,0,0};
 Root(0xebd0e8,reinterpret_cast<uintptr_t>(building));Root(0x1232c60,reinterpret_cast<uintptr_t>(object));
 const int16_t index=5;std::memcpy(owner+0x2e,&index,2);const auto matrixPointer=reinterpret_cast<uintptr_t>(matrix);std::memcpy(owner+0x20,&matrixPointer,4);
 Root(0xe95cd8+index*4,reinterpret_cast<uintptr_t>(model));uint32_t hash=123;std::memcpy(model+0x3c,&hash,4);flags[0]=7;
 for(unsigned i=0;i<3;++i)reinterpret_cast<float*>(matrix)[12+i]=float(i+1);
 Match value{};assert(MatchOwner(reinterpret_cast<uintptr_t>(owner),value));assert(value.kind==1&&value.identity.reference==7&&value.identity.model==123&&value.modelInfo==reinterpret_cast<uintptr_t>(model));
 assert(scalarQueries35==0&&batchQueries35==3);
 std::cout<<"Owner37: 3 fresh batch queries for either building or object; no scalar checks on resident path.\n";
 // Evidence is refreshed on the next call, never cached by owner address.
 auto model2=allocate();hash=456;std::memcpy(model2+0x3c,&hash,4);Root(0xe95cd8+index*4,reinterpret_cast<uintptr_t>(model2));
 assert(MatchOwner(reinterpret_cast<uintptr_t>(owner),value));assert(value.modelInfo==reinterpret_cast<uintptr_t>(model2)&&value.identity.model==456);
 flags[0]=0x87;assert(!MatchOwner(reinterpret_cast<uintptr_t>(owner),value));flags[0]=7;
 assert(!MatchOwner(reinterpret_cast<uintptr_t>(owner)+1,value));
 // Permission can change AFTER a successful query but BEFORE the raw read.
 revokeAddress=owner;revokeSize=page;inject=true;auto faults=ownerReadFaults32.load();
 assert(!MatchOwner(reinterpret_cast<uintptr_t>(owner),value));assert(!inject&&ownerReadFaults32.load()==faults);
 DWORD old{};assert(VirtualProtect(owner,page,PAGE_READWRITE,&old));assert(MatchOwner(reinterpret_cast<uintptr_t>(owner),value));
 // Both the batched object path and fallback building path must return model evidence.
 std::memcpy(buildingOwner+0x2e,&index,2);std::memcpy(buildingOwner+0x20,&matrixPointer,4);
 scalarQueries35=0;batchQueries35=0;
 assert(MatchOwner(reinterpret_cast<uintptr_t>(buildingOwner),value));assert(value.kind==0&&value.modelInfo==reinterpret_cast<uintptr_t>(model2));assert(scalarQueries35==0&&batchQueries35==3);
 revokeAddress=matrix;inject=true;faults=ownerReadFaults32.load();assert(!MatchOwner(reinterpret_cast<uintptr_t>(buildingOwner),value));assert(!inject&&ownerReadFaults32.load()==faults);
 assert(VirtualProtect(matrix,page,PAGE_READWRITE,&old));
 // Ambiguous overlapping native pools must never manufacture an identity.
 object->m_aStorage=buildingOwner;assert(!MatchOwner(reinterpret_cast<uintptr_t>(buildingOwner),value));object->m_aStorage=owner;
 // An unavailable peer descriptor must preserve the old valid-building path.
 Root(0x1232c60,0);assert(MatchOwner(reinterpret_cast<uintptr_t>(buildingOwner),value));assert(value.kind==0);Root(0x1232c60,reinterpret_cast<uintptr_t>(object));
 // Combined production model path saves an OS permission query while still
 // copying the descriptor and every owner detail fresh on each invocation.
 auto descriptor=allocate();for(unsigned k=0;k<0x6c;++k)descriptor[k]=uint8_t(k);
 std::array<uint8_t,0x6c> copiedBytes{};bool copied=false;
 scalarQueries35=0;batchQueries35=0;
 assert(MatchOwner(reinterpret_cast<uintptr_t>(owner),value));
 const traffic_signal::fresh_batch30::Span standalone{uintptr_t(descriptor),copiedBytes.data(),copiedBytes.size()};
 assert(OwnerCopy35(&standalone,1));assert(batchQueries35==4);
 scalarQueries35=0;batchQueries35=0;
 assert(MatchOwnerWithDescriptor48(uintptr_t(owner),value,uintptr_t(descriptor),copiedBytes.data(),copiedBytes.size(),copied));
 assert(copied&&batchQueries35==3&&scalarQueries35==0&&copiedBytes[10]==10);
 descriptor[10]=191;flags[0]=9;
 assert(MatchOwnerWithDescriptor48(uintptr_t(owner),value,uintptr_t(descriptor),copiedBytes.data(),copiedBytes.size(),copied));
 assert(copied&&copiedBytes[10]==191&&value.identity.reference==9);flags[0]=7;
 // Owner retirement remains rejected even if descriptor bytes are readable.
 flags[0]=0x87;
 assert(!MatchOwnerWithDescriptor48(uintptr_t(owner),value,uintptr_t(descriptor),copiedBytes.data(),copiedBytes.size(),copied));assert(copied);flags[0]=7;
 // Descriptor permission revocation after positive QWS does not publish output;
 // independent owner success is retained through the original fresh fallback.
 copiedBytes.fill(222);revokeAddress=descriptor;inject=true;
 assert(MatchOwnerWithDescriptor48(uintptr_t(owner),value,uintptr_t(descriptor),copiedBytes.data(),copiedBytes.size(),copied));
 assert(!inject&&!copied);for(auto b:copiedBytes)assert(b==222);
 assert(VirtualProtect(descriptor,page,PAGE_READWRITE,&old));
 assert(MatchOwnerWithDescriptor48(uintptr_t(owner),value,uintptr_t(descriptor),copiedBytes.data(),copiedBytes.size(),copied)&&copied);
 // Owner bytes revoked after authorization remain rejected; no stale identity.
 revokeAddress=owner;inject=true;
 assert(!MatchOwnerWithDescriptor48(uintptr_t(owner),value,uintptr_t(descriptor),copiedBytes.data(),copiedBytes.size(),copied));assert(!inject);
 assert(VirtualProtect(owner,page,PAGE_READWRITE,&old));
 // An early owner-root fault must not suppress independent readable metadata.
 const auto savedGame=game;game=1;copied=false;copiedBytes.fill(222);
 assert(!MatchOwnerWithDescriptor48(uintptr_t(owner),value,uintptr_t(descriptor),copiedBytes.data(),copiedBytes.size(),copied));
 assert(copied&&copiedBytes[10]==191);game=savedGame;
 // Existing unavailable-peer fallback also preserves fresh descriptor success.
 Root(0x1232c60,0);
 assert(MatchOwnerWithDescriptor48(uintptr_t(buildingOwner),value,uintptr_t(descriptor),copiedBytes.data(),copiedBytes.size(),copied)&&copied);
 Root(0x1232c60,reinterpret_cast<uintptr_t>(object));
 // Extracted production Promote and unchanged baseline both use the complete
 // production matcher, real permission queries and real ownership registry.
 assert(MatchOwner(uintptr_t(owner),value));
 model_effect_observer::Invocation invocation{};
 invocation.owner=uint32_t(uintptr_t(owner));invocation.descriptor=uint32_t(uintptr_t(descriptor));
 invocation.modelInfo=value.modelInfo;invocation.resolved=true;invocation.frame=7;
 std::array<uint8_t,0x6c> expected{};std::memcpy(expected.data(),descriptor,expected.size());
 auto packet=allocate();SafetyHookContext regs{uint32_t(uintptr_t(packet))};
 std::array<uint32_t,18> args{};args[10]=std::bit_cast<uint32_t>(10.f);
 auto run=[&](bool combined,bool warmed=false){
  std::optional<traffic_signal::fresh_page::Operation44> operation;
  if(warmed){operation.emplace();Match warm{};bool warmCopied=false;std::array<uint8_t,0x6c> warmBytes{};assert(MatchOwnerWithDescriptor48(invocation.owner,warm,invocation.descriptor,warmBytes.data(),warmBytes.size(),warmCopied)&&warmCopied);}
  ModelLightShadow20::registry={};assert(ModelLightShadow20::registry.Configure(0x30000000));
  ModelLightShadow20::lock.clear();std::memcpy(packet,args.data(),sizeof(args));
  scalarQueries35=0;batchQueries35=0;
  return combined?ModelLightShadow20::Promote(regs,invocation,value,expected,model_effect_observer::DirectPoint,args,true):
   ModelLightShadow20::PromoteBaseline49(regs,invocation,value,expected,model_effect_observer::DirectPoint,args,true);
 };
 assert(run(false)==0x30000000&&batchQueries35==4);
 assert(run(true)==0x30000000&&batchQueries35==3);
 assert(reinterpret_cast<uint32_t*>(packet)[2]==4&&reinterpret_cast<uint32_t*>(packet)[17]==0x30000000);
 assert(ModelLightShadow20::registry.used==1&&ModelLightShadow20::registry.entries[0].id==value.identity&&ModelLightShadow20::registry.entries[0].bytes==expected);
 assert(run(false,true)==0x30000000);const auto warmedBaseline=batchQueries35;
 assert(run(true,true)==0x30000000);const auto warmedCombined=batchQueries35;
 std::cout<<"Promote49 warmed operation real OS queries: baseline="<<warmedBaseline<<" combined="<<warmedCombined<<".\n";
 auto unchanged=[&](){assert(std::memcmp(packet,args.data(),sizeof(args))==0&&ModelLightShadow20::registry.used==0);};
 descriptor[10]^=1;assert(!run(false));unchanged();assert(!run(true));unchanged();descriptor[10]^=1;
 flags[0]=9;assert(!run(false));unchanged();assert(!run(true));unchanged();flags[0]=7;
 flags[0]=0x87;assert(!run(true));unchanged();flags[0]=7;
 auto savedModel=invocation.modelInfo;invocation.modelInfo=1;assert(!run(true));unchanged();invocation.modelInfo=savedModel;
 // PAGE_GUARD gets consumed by the first read fault. A retry must not
 // incorrectly turn that transient failure into shadow key publication.
 injectedProtection=PAGE_READWRITE|PAGE_GUARD;revokeAddress=descriptor;inject=true;
 assert(!run(false)&&!inject);unchanged();assert(VirtualProtect(descriptor,page,PAGE_READWRITE,&old));
 inject=true;assert(!run(true)&&!inject);unchanged();assert(VirtualProtect(descriptor,page,PAGE_READWRITE,&old));
 // Actual production ValidateSource46 must also refuse a guard consumed by
 // the combined initial Stage; retrying metadata would resurrect it.
 injectedProtection=PAGE_READWRITE|PAGE_GUARD;revokeAddress=descriptor;inject=true;
 ModelEffectObserver::Sample guardedSample{};ModelEffectObserver::local.state.invalid=false;
 ModelEffectObserver::ValidateSource46(invocation,guardedSample);
 assert(!inject&&!guardedSample.descriptorCopied&&ModelEffectObserver::local.state.invalid);
 assert(VirtualProtect(descriptor,page,PAGE_READWRITE,&old));
 injectedProtection=PAGE_NOACCESS;
 revokeAddress=descriptor;inject=true;assert(!run(true)&&!inject);unchanged();assert(VirtualProtect(descriptor,page,PAGE_READWRITE,&old));
 revokeAddress=owner;inject=true;assert(!run(true)&&!inject);unchanged();assert(VirtualProtect(owner,page,PAGE_READWRITE,&old));
 const auto savedRootGame=game;game=1;assert(!run(true));unchanged();game=savedRootGame;
 args[2]=4;assert(!run(true)&&batchQueries35==0);unchanged();args[2]=0;
 args[17]=123;assert(!run(true)&&batchQueries35==0);unchanged();args[17]=0;
 // Missing peer pool remains accepted by both routes for a building owner.
 Root(0x1232c60,0);invocation.owner=uint32_t(uintptr_t(buildingOwner));
 assert(MatchOwner(invocation.owner,value));invocation.modelInfo=value.modelInfo;
 assert(run(false)&&run(true));Root(0x1232c60,reinterpret_cast<uintptr_t>(object));
 assert(VirtualFree(packet,0,MEM_RELEASE));
 std::cout<<"Promote49 production final validation: 4-to-3 real OS queries; mutations, retirement, raced faults, no publication and missing-peer parity passed.\n";
 assert(VirtualFree(descriptor,0,MEM_RELEASE));
 std::cout<<"Model owner48: accepted 4-to-3 actual OS queries; fresh owner/descriptor, retirement, raced faults, no partial output and peer fallback passed.\n";
 for(auto p:{headers,owner,buildingOwner,flags,model,matrix,model2})assert(VirtualFree(p,0,MEM_RELEASE));
 std::cout<<"Fresh model evidence, pool retirement, alignment, and post-query fault containment passed.\n";
}
