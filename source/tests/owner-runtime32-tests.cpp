#include <optional>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>
#include "TrafficSignalIdentity.hpp"
#include "PoolDescriptorSnapshots.hpp"
#include "ImmediateRegion33.hpp"
#include "NativeOwnerClass109.hpp"
#include "NativeOwnerClass114.hpp"
#define Stage ProductionStage205
#include "FreshReadBatch30.hpp"
#undef Stage
void BeforeFinalGroup205() noexcept;
namespace traffic_signal::fresh_batch30 {
 inline bool Stage(const Span* spans,unsigned count,unsigned char (*bytes)[MaximumBytes],bool preserveGuard231=false) noexcept {
  BeforeFinalGroup205();return ProductionStage205(spans,count,bytes,preserveGuard231);
 }
}
#include "FreshReadGroup50.hpp"
namespace rage {struct VehiclePool {uint8_t* m_aStorage;uint8_t* m_aFlags;int32_t m_nSize,m_nStorageSize,m_nFirstFreeIndex,m_numSlotsUsed;};}
std::vector<uint8_t> image(0x12f7d64);
uintptr_t game=reinterpret_cast<uintptr_t>(image.data());
struct Match {traffic_signal::Identity identity{};uint32_t kind{};float entity[3]{};uint32_t modelInfo{};std::array<uint8_t,68> ownerPose109{};};
void* revokeAddress{};size_t revokeSize{};bool inject{};
unsigned scalarQueries35{},batchQueries35{};
unsigned mutationAt109{};uint8_t* mutationByte109{};uint8_t mutationValue109{};
// Final group permission queries can disappear in205; inject at the actual
// fresh final copy boundary, after initial model/pose/header observations.
void BeforeFinalGroup205() noexcept {
 if(mutationAt109&&mutationByte109){*mutationByte109=mutationValue109;mutationAt109=0;}
}
bool Readable(uintptr_t address,size_t bytes) noexcept {
 ++scalarQueries35;
 MEMORY_BASIC_INFORMATION info{};
 bool ok=address>=0x10000&&bytes&&address<=UINTPTR_MAX-bytes&&VirtualQuery(reinterpret_cast<void*>(address),&info,sizeof(info))&&info.State==MEM_COMMIT&&!(info.Protect&(PAGE_NOACCESS|PAGE_GUARD));
 if(ok){auto base=reinterpret_cast<uintptr_t>(info.BaseAddress);ok=address>=base&&address-base<=info.RegionSize&&bytes<=info.RegionSize-(address-base);}
 if(ok&&inject&&address==reinterpret_cast<uintptr_t>(revokeAddress)){DWORD old{};assert(VirtualProtect(revokeAddress,revokeSize,PAGE_NOACCESS,&old));inject=false;}
 return ok;
}
bool ReadableOriginal(uintptr_t address,size_t bytes) noexcept{return Readable(address,bytes);}
BOOL WINAPI InjectQuery35(HANDLE process,PVOID buffer,DWORD size){
 ++batchQueries35;
 auto ok=QueryWorkingSetEx(process,buffer,size);
 auto* entries=static_cast<PSAPI_WORKING_SET_EX_INFORMATION*>(buffer);
 if(ok&&inject)for(unsigned i=0;i<size/sizeof(*entries);++i)
  if(entries[i].VirtualAddress==revokeAddress){DWORD old{};assert(VirtualProtect(revokeAddress,revokeSize,PAGE_NOACCESS,&old));inject=false;break;}
 return ok;
}
#define CE_OWNER_BATCH_QUERY35 InjectQuery35
#include "owner-runtime32-extracted.inc"
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
 // Actual production matcher: new type5 native pool is separate from old
 // ranges. Guard OFF performs exactly the old header-only rejection.
 auto extraOwner109=allocate(),extraFlags109=allocate();auto extra109=object+1;
 *extra109={extraOwner109,extraFlags109,1,0x80,0,1};
 Root(native_owner109::PoolRva,reinterpret_cast<uintptr_t>(extra109));
 const uint32_t vtable109=uint32_t(game+native_owner109::VtableRva),type109=0x140;
 std::memcpy(extraOwner109,&vtable109,4);std::memcpy(extraOwner109+0x28,&type109,4);
 std::memcpy(extraOwner109+0x2e,&index,2);extraFlags109[0]=13;
 auto* inline109=reinterpret_cast<float*>(extraOwner109+0x10);
 for(unsigned j=0;j<4;++j)inline109[j]=float(j+8);
 ownerClass109Active=false;scalarQueries35=batchQueries35=0;
 assert(!MatchOwner(uintptr_t(extraOwner109),value)&&scalarQueries35==0&&batchQueries35==1);
 ownerClass109Active=true;scalarQueries35=batchQueries35=0;
 assert(MatchOwner(uintptr_t(extraOwner109),value)&&value.kind==2&&value.entity[2]==10&&value.identity.reference==13);
 assert(value.identity.pool==uintptr_t(extra109)&&value.modelInfo==uintptr_t(model2));
 assert(value.ownerPose109[0]==0&&batchQueries35==5);
 // Current allocated matrix and inline owner placement remain distinct from
 // a native emitter matrix. Only current finite owner XYZ are used here.
 std::memcpy(extraOwner109+0x20,&matrixPointer,4);
 assert(MatchOwner(uintptr_t(extraOwner109),value)&&value.entity[0]==1);
 assert(std::memcmp(value.ownerPose109.data()+4,matrix,64)==0);
 std::memset(extraOwner109+0x20,0,4);
 extraFlags109[0]|=0x80;assert(!MatchOwner(uintptr_t(extraOwner109),value));extraFlags109[0]=13;
 assert(!MatchOwner(uintptr_t(extraOwner109)+1,value));
 extraOwner109[0]^=1;assert(!MatchOwner(uintptr_t(extraOwner109),value));extraOwner109[0]^=1;
 extraOwner109[0x29]=0;assert(!MatchOwner(uintptr_t(extraOwner109),value));extraOwner109[0x29]=1;
 // Fresh final copy refuses generation, native inline pose, type/model and
 // same-count pool geometry changes, with no partial Match publication.
 for(auto* field109:{extraFlags109,extraOwner109+0x10,extraOwner109+0x29,extraOwner109+0x2e,
                    reinterpret_cast<uint8_t*>(&extra109->m_aStorage),model2+0x3c}){
  const auto prior109=*field109;Match untouched109{};untouched109.identity.model=0xabcdef01;
  scalarQueries35=batchQueries35=0;mutationAt109=7;mutationByte109=field109;mutationValue109=uint8_t(prior109^1);
  assert(!MatchOwner(uintptr_t(extraOwner109),untouched109));assert(mutationAt109==0&&untouched109.identity.model==0xabcdef01);
  *field109=prior109;mutationByte109=nullptr;assert(MatchOwner(uintptr_t(extraOwner109),value));
 }
 std::memcpy(extraOwner109+0x20,&matrixPointer,4);
 const auto priorMatrix109=matrix[0];scalarQueries35=batchQueries35=0;
 mutationAt109=7;mutationByte109=matrix;mutationValue109=uint8_t(priorMatrix109^1);
 assert(!MatchOwner(uintptr_t(extraOwner109),value));matrix[0]=priorMatrix109;mutationByte109=nullptr;
 std::memset(extraOwner109+0x20,0,4);
 extra109->m_nStorageSize=0x70;assert(!MatchOwner(uintptr_t(extraOwner109),value));extra109->m_nStorageSize=0x80;
 extra109->m_nSize=-1;assert(!MatchOwner(uintptr_t(extraOwner109),value));extra109->m_nSize=1;
 inline109[2]=NAN;assert(!MatchOwner(uintptr_t(extraOwner109),value));inline109[2]=10;
 auto extraOwner114=allocate(),extraFlags114=allocate();auto extra114=object+2;
 *extra114={extraOwner114,extraFlags114,1,0x110,0,1};
 Root(native_owner114::PoolRva,reinterpret_cast<uintptr_t>(extra114));
 const uint32_t vtable114=uint32_t(game+native_owner114::VtableRva),type114=0x40;
 std::memcpy(extraOwner114,&vtable114,4);std::memcpy(extraOwner114+0x28,&type114,4);
 std::memcpy(extraOwner114+0x2e,&index,2);extraFlags114[0]=13;
 auto* inline114=reinterpret_cast<float*>(extraOwner114+0x10);
 for(unsigned j=0;j<4;++j)inline114[j]=float(j+8);
 ownerClass109Active=false;ownerClass114Active=false;scalarQueries35=batchQueries35=0;
 assert(!MatchOwner(uintptr_t(extraOwner114),value)&&scalarQueries35==0&&batchQueries35==1);
 ownerClass114Active=true;scalarQueries35=batchQueries35=0;
 assert(MatchOwner(uintptr_t(extraOwner114),value)&&value.kind==3&&value.entity[2]==10&&value.identity.reference==13);
 assert(value.identity.pool==uintptr_t(extra114)&&value.modelInfo==uintptr_t(model2));
 assert(value.ownerPose109[0]==0&&batchQueries35==5);
 // Current allocated matrix and inline owner placement remain distinct from
 // a native emitter matrix. Only current finite owner XYZ are used here.
 std::memcpy(extraOwner114+0x20,&matrixPointer,4);
 assert(MatchOwner(uintptr_t(extraOwner114),value)&&value.entity[0]==1);
 assert(std::memcmp(value.ownerPose109.data()+4,matrix,64)==0);
 std::memset(extraOwner114+0x20,0,4);
 extraFlags114[0]|=0x80;assert(!MatchOwner(uintptr_t(extraOwner114),value));extraFlags114[0]=13;
 assert(!MatchOwner(uintptr_t(extraOwner114)+1,value));
 extraOwner114[0]^=1;assert(!MatchOwner(uintptr_t(extraOwner114),value));extraOwner114[0]^=1;
 extraOwner114[0x28]=0;assert(!MatchOwner(uintptr_t(extraOwner114),value));extraOwner114[0x28]=0x40;
 // Fresh final copy refuses generation, native inline pose, type/model and
 // same-count pool geometry changes, with no partial Match publication.
 for(auto* field114:{extraFlags114,extraOwner114+0x10,extraOwner114+0x29,extraOwner114+0x2e,
                    reinterpret_cast<uint8_t*>(&extra114->m_aStorage),model2+0x3c}){
  const auto prior114=*field114;Match untouched114{};untouched114.identity.model=0xabcdef01;
  scalarQueries35=batchQueries35=0;mutationAt109=7;mutationByte109=field114;mutationValue109=uint8_t(prior114^1);
  assert(!MatchOwner(uintptr_t(extraOwner114),untouched114));assert(mutationAt109==0&&untouched114.identity.model==0xabcdef01);
  *field114=prior114;mutationByte109=nullptr;assert(MatchOwner(uintptr_t(extraOwner114),value));
 }
 std::memcpy(extraOwner114+0x20,&matrixPointer,4);
 const auto priorMatrix114=matrix[0];scalarQueries35=batchQueries35=0;
 mutationAt109=7;mutationByte109=matrix;mutationValue109=uint8_t(priorMatrix114^1);
 assert(!MatchOwner(uintptr_t(extraOwner114),value));matrix[0]=priorMatrix114;mutationByte109=nullptr;
 std::memset(extraOwner114+0x20,0,4);
 extra114->m_nStorageSize=0x70;assert(!MatchOwner(uintptr_t(extraOwner114),value));extra114->m_nStorageSize=0x110;
 extra114->m_nSize=-1;assert(!MatchOwner(uintptr_t(extraOwner114),value));extra114->m_nSize=1;
 inline114[2]=NAN;assert(!MatchOwner(uintptr_t(extraOwner114),value));inline114[2]=10;

 ownerClass114Active=false;
 assert(VirtualFree(extraOwner114,0,MEM_RELEASE)&&VirtualFree(extraFlags114,0,MEM_RELEASE));
 // Third class gets no queued-window certificate: native callbacks invalidate
 // the existing certificate before entering the fully fresh serial matcher.
 ownerClass109Active=false;
 native_owner114::Authenticate(image,[&](uint32_t rva,uint32_t bytes){return std::vector<uint8_t>(image.begin()+rva,image.begin()+rva+bytes);});
 // The deleting destructor RET4 at8797AE includes its final operand byte8797B0.
 assert(native_owner114::Code[0].rva+native_owner114::Code[0].size==0x8797b1);
 for(const auto range114:native_owner114::Code){bool refused114=false;
  try{native_owner114::Authenticate(image,[&](uint32_t rva,uint32_t bytes){auto bytes114=std::vector<uint8_t>(image.begin()+rva,image.begin()+rva+bytes);if(rva==range114.rva)bytes114.back()^=1;return bytes114;});}catch(const std::runtime_error&){refused114=true;}assert(refused114);
 }
 native_owner109::Authenticate(image,[&](uint32_t rva,uint32_t bytes){return std::vector<uint8_t>(image.begin()+rva,image.begin()+rva+bytes);});
 for(const auto range109:native_owner109::Code){bool refused109=false;
  try{native_owner109::Authenticate(image,[&](uint32_t rva,uint32_t bytes){auto result109=std::vector<uint8_t>(image.begin()+rva,image.begin()+rva+bytes);if(rva==range109.rva)result109.back()^=1;return result109;});}
  catch(const std::runtime_error&){refused109=true;}assert(refused109);
 }
 assert(!native_owner109::Range(UINT32_MAX-10,UINT32_MAX-16,1,0x80));
 assert(!native_owner109::Range(0x10000,0x10000,-1,0x80));
 assert(native_owner109::Range(0x10080,0x10000,2,0x80));
 assert(VirtualFree(extraOwner109,0,MEM_RELEASE)&&VirtualFree(extraFlags109,0,MEM_RELEASE));
 for(auto p:{headers,owner,buildingOwner,flags,model,matrix,model2})assert(VirtualFree(p,0,MEM_RELEASE));
 std::cout<<"Fresh model evidence, pool retirement, alignment, and post-query fault containment passed.\n";
}
