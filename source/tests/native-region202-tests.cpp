// Reuse the generated actual queued-validation fixture and exact frozen196
// NativeOwnerBatch body. Only phase-local region scopes differ in this test;
// both bodies use the current certificate helper tested separately.
#include <memory>
#define main ExistingNativeFixtureMain202
#include "native-owner-batch115-generated.cpp"
#undef main
namespace TrafficSignal {
#define NativeOwnerBatch115 NativeOwnerBatch196
#include "native-owner196-reference202.inc"
#undef NativeOwnerBatch115
}
int main(){
 using Old=TrafficSignal::NativeOwnerBatch196::Scratch;using New=TrafficSignal::NativeOwnerBatch115::Scratch;
 NativeCorpus115 data(100,false);data.Restore();
 TrafficSignal::NativeOwnerBatch115::requested=true;TrafficSignal::NativeOwnerBatch196::requested=true;
 constexpr unsigned Count=20;TrafficSignal::OwnerRequest49 requests[Count]{};TrafficSignal::OwnerResult49 oldResults[Count]{},newResults[Count]{};
 for(unsigned i=0;i<Count;++i){const auto* e=data.registry.Lookup(data.packets[i+2].mCastShadows);assert(e);requests[i]={e->id.owner,e->descriptor};oldResults[i].serialFallback=newResults[i].serialFallback=true;}
 auto old=std::make_unique<Old>();auto now=std::make_unique<New>();
 failWs115=true;virtualQueries115=0;TrafficSignal::NativeOwnerBatch196::Stage(requests,oldResults,Count,*old);const auto stageOld=virtualQueries115;
 virtualQueries115=0;TrafficSignal::NativeOwnerBatch115::Stage(requests,newResults,Count,*now);const auto stageNew=virtualQueries115;
 assert(!immediate_region33::current&&stageNew<stageOld);
 for(unsigned i=0;i<Count;++i){assert(old->rows[i].ready&&now->rows[i].ready);assert(oldResults[i].match.identity==newResults[i].match.identity);assert(oldResults[i].match.modelInfo==newResults[i].match.modelInfo);}
 virtualQueries115=0;TrafficSignal::NativeOwnerBatch196::Preflight(*old,Count);const auto preflightOld=virtualQueries115;
 virtualQueries115=0;TrafficSignal::NativeOwnerBatch115::Preflight(*now,Count);const auto preflightNew=virtualQueries115;
 assert(!immediate_region33::current&&preflightNew<preflightOld);
 // Rebuild current positive owner certificates, then force fresh region
 // queries for TYPE1's final root reread and the descriptor copy.
 failWs115=false;TrafficSignal::NativeOwnerBatch196::Preflight(*old,Count);TrafficSignal::NativeOwnerBatch115::Preflight(*now,Count);
 old->descriptorCertificate.active=now->descriptorCertificate.active=false;failWs115=true;
 TrafficSignal::Match oldMatch{},newMatch{};std::array<uint8_t,108> oldBytes{},newBytes{};bool oldFault{},newFault{},oldRestart{},newRestart{};
 virtualQueries115=0;const bool oldAccepted=TrafficSignal::NativeOwnerBatch196::Commit(old->rows[2],oldMatch,oldBytes,oldFault,oldRestart,*old,2);const auto commitOld=virtualQueries115;
 virtualQueries115=0;const bool newAccepted=TrafficSignal::NativeOwnerBatch115::Commit(now->rows[2],newMatch,newBytes,newFault,newRestart,*now,2);const auto commitNew=virtualQueries115;
 assert(oldAccepted&&newAccepted&&!oldFault&&!newFault&&!oldRestart&&!newRestart);
 assert(oldMatch.identity==newMatch.identity&&oldMatch.ownerPose109==newMatch.ownerPose109&&oldBytes==newBytes);
 assert(!immediate_region33::current&&commitNew<commitOld);
 // Stage's permission scope cannot survive into the next phase. Revoke a
 // model page before Preflight; the new region query must see the denial.
 DWORD prior{};assert(VirtualProtect(data.models,Page49,PAGE_NOACCESS,&prior));virtualQueries115=0;
 TrafficSignal::NativeOwnerBatch115::Preflight(*now,Count);assert(virtualQueries115>0&&!now->ownerCertificate.allowed[0]);
 assert(!TrafficSignal::NativeOwnerBatch115::Commit(now->rows[0],newMatch,newBytes,newFault,newRestart,*now,0)&&newRestart&&!newFault);
 assert(VirtualProtect(data.models,Page49,PAGE_READWRITE,&prior));
 // A nested invocation owns its own bounded cache and restores the caller.
 {immediate_region33::Scope outer;
  for(auto& result:newResults){result={};result.serialFallback=true;}
  TrafficSignal::NativeOwnerBatch115::Stage(requests,newResults,Count,*now);
  assert(immediate_region33::current==&outer&&outer.count==0);
 }
 failWs115=false;assert(!immediate_region33::current);
 std::printf("actual NativeOwnerBatch115 versus frozen196 body (same certificate helper): VirtualQuery Stage %u->%u, Preflight %u->%u, Commit %u->%u; current packet/identity parity, between-phase revocation, independent nested scope passed. No FPS proof.\n",stageOld,stageNew,preflightOld,preflightNew,commitOld,commitNew);
}
