// Reuse the existing model fixture and CMake's literal production extracts.
// Only the test fallback reader/OS result controls differ from fixture68.
#define MODEL_REGION196
#include "model-point-value68-tests.cpp"

unsigned virtualQueries196{},raceCount196{},fallbackBoundaryQueries196{};
bool raceRegion196=false,revokeBetween196=false;
immediate_region33::Scope* expectedOuter196=nullptr;
SIZE_T WINAPI CountVirtualQuery196(LPCVOID address,PMEMORY_BASIC_INFORMATION info,SIZE_T size) {
 ++virtualQueries196;return VirtualQuery(address,info,size);
}
void AfterRegion196(uintptr_t address,size_t,bool accepted) noexcept {
 if(raceRegion196&&accepted&&address==uintptr_t(memory+4096)){
  raceRegion196=false;++raceCount196;
  assert(VirtualProtect(memory+4096,4096,PAGE_NOACCESS,&oldProtection));
 }
}
void AfterPoint196() noexcept {
 assert(immediate_region33::current==expectedOuter196);
 if(expectedOuter196)assert(expectedOuter196->count==0);
 fallbackBoundaryQueries196=virtualQueries196;
 if(revokeBetween196){
  assert(!immediate_region33::current);
  assert(ModelEffectObserver::ModelValue68::pending[0].present);
  assert(VirtualProtect(memory+4096,4096,PAGE_NOACCESS,&oldProtection));
 }
}
struct Observation196 {Result result;ModelEffectObserver::Sample sample;unsigned virtualQueries,regions,afterPointQueries;};
Observation196 Observe196(bool scoped,unsigned mode,bool fused,bool deferred,Change mutation=Change::None) {
 disableRegions196=!scoped;forceLocked196=mode==0;failQuery196=mode==1;crossPage196=mode==2;
 virtualQueries196=regionCalls50=0;
 auto result=Run(true,mutation,false,false,fused,false,false,model_effect_observer::WrapperPoint,0,deferred,deferred?1u:0u);
 return {result,ModelEffectObserver::local.samples[0],virtualQueries196,regionCalls50,virtualQueries196-fallbackBoundaryQueries196};
}
void Equal196(const Observation196& a,const Observation196& b) {
 assert(a.result.flags==b.result.flags&&a.result.key==b.result.key&&a.result.invalid==b.result.invalid);
 assert(a.result.owners==b.result.owners&&a.result.bytes==b.result.bytes&&a.result.queries==b.result.queries);
 const auto& x=a.sample;const auto& y=b.sample;
 assert(x.identity==y.identity&&x.ownerKind==y.ownerKind&&x.ownerPose109==y.ownerPose109);
 assert(x.ownerMatched==y.ownerMatched&&x.modelInfoMatched==y.modelInfoMatched&&x.descriptorCopied==y.descriptorCopied&&x.packetCopied==y.packetCopied);
 assert(x.descriptor==y.descriptor&&x.matrix==y.matrix&&x.position==y.position&&x.color==y.color&&x.direction==y.direction&&x.up==y.up);
 assert(x.intensity==y.intensity&&x.radius==y.radius&&x.innerCone==y.innerCone&&x.outerCone==y.outerCone);
}
int main() {
 std::setvbuf(stdout,nullptr,_IONBF,0);
 memory=static_cast<uint8_t*>(VirtualAlloc(nullptr,4096*4,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(memory);
 auto allocate=[](){auto* p=static_cast<uint8_t*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(p);return p;};
 vectorPages50=static_cast<uint8_t*>(VirtualAlloc(nullptr,4096*4,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(vectorPages50);
 headers50=allocate();flags50=allocate();model50=allocate();dummyOwner50=allocate();game=uint32_t(uintptr_t(gameImage.data()));
 auto* building=reinterpret_cast<rage::VehiclePool*>(headers50);auto* object=building+1;
 *building={dummyOwner50,flags50,1,0x70,0,0};*object={memory+8192,flags50,1,0x320,0,0};
 auto root=[&](unsigned rva,uint32_t ptr){std::memcpy(gameImage.data()+rva,&ptr,4);};
 root(0xebd0e8,uint32_t(uintptr_t(building)));root(0x1232c60,uint32_t(uintptr_t(object)));root(0xe95cd8+5*4,uint32_t(uintptr_t(model50)));
 memory_read46::Configure(true);countBytes50=true;
 unsigned reductions=0;
 for(unsigned mode=0;mode<3;++mode)for(bool fused:{false,true})for(bool deferred:{false,true}){
  const auto prior=Observe196(false,mode,fused,deferred),now=Observe196(true,mode,fused,deferred);
  Equal196(prior,now);assert(now.result.key==0x30000000&&!now.result.invalid);
  assert(now.virtualQueries<prior.virtualQueries);++reductions;
  std::printf("model-region196 mode=%u fused=%u deferred=%u VirtualQuery=%u->%u fallback_calls=%u->%u copied_bytes=%u key=%08x\n",mode,unsigned(fused),unsigned(deferred),prior.virtualQueries,now.virtualQueries,prior.regions,now.regions,now.result.bytes,now.result.key);
 }
 assert(reductions==12);
 // Identical fresh source/owner mutation refusals with and without the region
 // cache: copied values and generation witnesses are never reused as data.
 for(auto mutation:{Change::Descriptor,Change::Generation,Change::Hash,Change::Matrix,Change::Index}){
  const auto prior=Observe196(false,0,true,false,mutation),now=Observe196(true,0,true,false,mutation);
  Equal196(prior,now);std::printf("mutation=%u key=%08x invalid=%u\n",unsigned(mutation),now.result.key,unsigned(now.result.invalid));assert(now.result.key==0);
 }
 // Permission proof from Point has ended before the native constructor's
 // fallback. Revoke the descriptor in that boundary; FinishPoint68 must query
 // current regions again and reject it instead of issuing a key.
 revokeBetween196=true;const auto revoked=Observe196(true,0,false,true);revokeBetween196=false;
 assert(revoked.result.key==0&&revoked.result.invalid&&revoked.afterPointQueries>1);
 assert(VirtualProtect(memory+4096,4096,PAGE_READWRITE,&oldProtection));
 // A race after accepted actual VirtualQuery remains contained by the real
 // SEH copies even when a positive region is already in this operation.
 raceRegion196=true;const auto raced=Observe196(true,0,false,false);
 assert(!raceRegion196&&raceCount196==1&&raced.result.key==0&&raced.result.invalid);
 assert(VirtualProtect(memory+4096,4096,PAGE_READWRITE,&oldProtection));
 // Nested Point owns an independent region scope and restores its caller.
 {immediate_region33::Scope outer;expectedOuter196=&outer;
  const auto nested=Observe196(true,0,true,false);assert(nested.result.key);
  assert(immediate_region33::current==&outer&&outer.count==1);expectedOuter196=nullptr;}
 assert(!immediate_region33::current&&!fp::operation44);
 for(auto p:{headers50,flags50,model50,dummyOwner50,vectorPages50,memory})assert(VirtualFree(p,0,MEM_RELEASE));
 std::puts("model-region196: actual Point/FinishPoint68 fallback parity; locked/query-failure/cross-page reduction; fresh witnesses, between-hook revocation, SEH race and nested scope restoration passed");
}
