#include "ModelLightSubmission20.hpp"
#include <cstdio>
#include <cstdlib>
using namespace model_effect_observer;
using model_light_submission20::Layout;
static unsigned checks;
#define CHECK(v) do{++checks;if(!(v)){std::fprintf(stderr,"FAIL %d %s\n",__LINE__,#v);std::abort();}}while(false)
static Invocation Context(){Invocation c{};c.frame=50;c.preStack=0x8000;c.owner=0x10000;c.effect=0x20000;c.matrix=0x30000;c.ordinal=5;c.proposedKey=c.owner+c.ordinal;c.sampled=true;return c;}
static ResolveEvidence Resolve(const Invocation& c){return{c.frame,c.preStack,DispatchReturn,c.effect,c.matrix,c.owner,c.effect,0x50000,c.proposedKey};}
static PointEvidence Packet(const Invocation& c,unsigned site,unsigned flags){return {c.frame,c.preStack,DispatchReturn,c.effect,0x50000,site,Layout(site).type,flags,(flags&6)?c.proposedKey:0};}
int main(){
 constexpr unsigned sites[]={WrapperPoint,DirectPoint,model_light_submission20::WrapperSpot,model_light_submission20::DirectSpot};
 static_assert(Layout(0x81ebab).words==16&&Layout(0x81ebab).keyOffset==0x3c&&Layout(0x81ebab).type==2);
 static_assert(Layout(0x81ecfc).words==18&&Layout(0x81ecfc).keyOffset==0x44&&Layout(0x81ecfc).type==2);
 CHECK(!Layout(0));CHECK(!Layout(0x81ebaa));
 const auto c=Context();Invocation out{};
 for(auto site:sites)for(unsigned flags:{1u,3u,5u,7u,0x11u}){
  model_light_submission20::State s;CHECK(s.Enter(c));CHECK(s.Resolve(Resolve(c)));CHECK(s.Submission(Packet(c,site,flags)));
  CHECK(s.Leave(c.preStack,c.frame,out)==LeaveResult::Valid);CHECK(out.submitted&&out.resolved&&!out.invalid);
 }
 // A spotlight cannot impersonate a point call or vice versa, even though
 // each ABI has the same packet offsets. No retry may revive invalid context.
 for(auto site:sites){model_light_submission20::State s;CHECK(s.Enter(c));CHECK(s.Resolve(Resolve(c)));
  auto p=Packet(c,site,7);p.type=p.type==0?2:0;CHECK(!s.Submission(p));CHECK(!s.Submission(Packet(c,site,7)));
  CHECK(s.Leave(c.preStack,c.frame,out)==LeaveResult::Invalid);
 }
 // Authored glow-only type and unknown producers are never normalized.
 for(unsigned type:{1u,3u,4u,UINT32_MAX}){model_light_submission20::State s;CHECK(s.Enter(c));CHECK(s.Resolve(Resolve(c)));auto p=Packet(c,model_light_submission20::DirectSpot,7);p.type=type;CHECK(!s.Submission(p));}
 for(auto member:{&PointEvidence::frame,&PointEvidence::preStack,&PointEvidence::effect,&PointEvidence::descriptor,&PointEvidence::key,&PointEvidence::returnRva}){
  model_light_submission20::State s;CHECK(s.Enter(c));CHECK(s.Resolve(Resolve(c)));auto p=Packet(c,model_light_submission20::DirectSpot,7);++(p.*member);CHECK(!s.Submission(p));CHECK(s.Leave(c.preStack,c.frame,out)==LeaveResult::Invalid);
 }
 {model_light_submission20::State s;CHECK(s.Enter(c));auto p=Packet(c,WrapperPoint,7);p.site=0x81eaf0;CHECK(!s.Submission(p));CHECK(s.Current()->invalid);}
 // Nested spot/point invocations must retain independent associations.
 {model_light_submission20::State s;auto nested=c;nested.owner+=0x100;nested.proposedKey=nested.owner+nested.ordinal;nested.preStack-=0x100;nested.effect+=0x100;nested.matrix+=0x100;
  CHECK(s.Enter(c));CHECK(s.Resolve(Resolve(c)));CHECK(s.Enter(nested));CHECK(s.Resolve(Resolve(nested)));
  CHECK(s.Submission(Packet(nested,model_light_submission20::WrapperSpot,3)));CHECK(s.Leave(nested.preStack,nested.frame,out)==LeaveResult::Valid);
  CHECK(s.Submission(Packet(c,DirectPoint,7)));CHECK(s.Leave(c.preStack,c.frame,out)==LeaveResult::Valid);
 }
 std::printf("PASS native model point/spot association: %u checks; both store ABIs, forged sites/types, stale context, native key rules and mixed nesting. No live geometry/coverage claim.\n",checks);
}
