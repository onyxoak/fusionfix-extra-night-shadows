#include "core/DeferredModelShortlist48.hpp"
#include <algorithm>
#include <bit>
#include <cassert>
#include <cstdio>
#include <random>
#include <vector>
using namespace fusionfix::shadows;
namespace s=shortlist48;
// Independent transcription of 5253F0 and 527D66..527DD0, including all eight
// native cells. Scratch is never overridden by the mod; only seven are returned.
s::Cells NativeReference(const std::vector<s::Record>& tape,const std::vector<bool>& valid){
 std::array<int,8> indices;indices.fill(-1);
 std::array<unsigned,8> flags{};
 std::array<float,8> distances;distances.fill(-1.f);
 auto find=[&](int index)->const s::Record*{for(const auto& r:tape)if(r.index==index)return &r;return nullptr;};
 for(unsigned n=0;n<tape.size();++n){
  if(tape[n].deferred&&!valid[n])continue;
  int stack24=tape[n].index;unsigned edi=tape[n].flags;float stack20=tape[n].nativeDistance;
  for(unsigned esi=0;esi<32;esi+=4){const auto row=esi/4;
   unsigned eax=flags[row],ecx=edi;int result;
   if(eax==0)result=0;
   else if(ecx&0x400)result=0;
   else if(ecx&0x100)result=(eax&0x400)?2:int((eax>>8)&1);
   else result=(eax&0x500)?2:1;
   const auto stack1c=eax; // two pushes: [ESP+24] aliases original [ESP+1C].
   if(esi<28&&stack24>=0&&indices[row]>=0){const auto* a=find(stack24);const auto* b=find(indices[row]);
    if(a&&b&&a->candidate.observed&&b->candidate.observed&&a->candidate.flags==edi&&b->candidate.flags==flags[row])
     result=NativeShadowContinuity42::Compare(result,a->candidate,b->candidate);
   }
   if(result==2||(result==1&&!(distances[row]>stack20)))continue;
   const auto xmm0=distances[row];const auto oldIndex=indices[row];
   flags[row]=edi;edi=stack1c;distances[row]=stack20;indices[row]=stack24;
   stack20=xmm0;stack24=oldIndex;
  }
 }
 s::Cells output{};for(unsigned i=0;i<7;++i)output[i]={indices[i],flags[i],distances[i]};return output;
}
void Equal(const s::Cells& a,const s::Cells& b){for(unsigned i=0;i<7;++i){assert(a[i].index==b[i].index);assert(a[i].flags==b[i].flags);assert(std::bit_cast<unsigned>(a[i].distance)==std::bit_cast<unsigned>(b[i].distance));}}
s::Record Lamp(unsigned i,float distance){s::Record r{};r.index=int(i);r.flags=4;r.nativeDistance=distance;r.deferred=true;r.candidate.flags=4;r.candidate.observed=true;r.candidate.identity=i+1000;r.candidate.focusDistance=distance;return r;}
void Compare(const std::vector<s::Record>& tape,const std::vector<bool>& valid){
 std::vector<unsigned> calls(tape.size());
 const auto plan=s::Build(tape,[&](const s::Record& r)noexcept{auto n=unsigned(&r-tape.data());++calls[n];return valid[n]?s::Validation::Valid:s::Validation::Invalid;});
 assert(plan.status==s::Status::Ready);Equal(plan.cells,NativeReference(tape,valid));
 for(unsigned n=0;n<tape.size();++n)assert(calls[n]<=1);
 for(const auto& cell:plan.cells)if(cell.index>=0){for(unsigned n=0;n<tape.size();++n)if(tape[n].index==cell.index)assert(!tape[n].deferred||plan.validations[n]==s::Validation::Valid);}
}
int main(){
 std::vector<s::Record> tape;std::vector<bool> valid;
 for(unsigned i=0;i<640;++i){tape.push_back(Lamp(i,float(i+1)));valid.push_back(i>=40);}
 auto plan=s::Build(tape,[&](const s::Record& r)noexcept{return valid[r.index]?s::Validation::Valid:s::Validation::Invalid;});
 Equal(plan.cells,NativeReference(tape,valid));assert(plan.status==s::Status::Ready&&plan.validationCalls==47);
 for(unsigned i=0;i<7;++i)assert(plan.cells[i].index==int(i+40));
 std::printf("complete 640-input tape: invalid first40 refill to40..46; fresh validations %u (no top8/top32 cap)\n",plan.validationCalls);
 std::fill(valid.begin(),valid.end(),true);
 plan=s::Build(tape,[](const s::Record&)noexcept{return s::Validation::Valid;});assert(plan.validationCalls==7);Compare(tape,valid);
 std::printf("ascending valid640 fixture: validations 640 -> %u\n",plan.validationCalls);
 for(unsigned i=0;i<640;++i){tape[i].nativeDistance=float(640-i);tape[i].candidate.focusDistance=tape[i].nativeDistance;}
 plan=s::Build(tape,[](const s::Record&)noexcept{return s::Validation::Valid;});assert(plan.validationCalls==640);Compare(tape,valid);
 std::printf("descending valid640 worst-case fixture: validations 640 -> %u\n",plan.validationCalls);
 std::fill(valid.begin(),valid.end(),false);Compare(tape,valid);
 plan=s::Build(tape,[](const s::Record&)noexcept{return s::Validation::Unavailable;});assert(plan.status==s::Status::ValidationUnavailable);
 auto malformed=tape;malformed[1].index=malformed[0].index;
 unsigned unexpected=0;plan=s::Build(malformed,[&](const s::Record&)noexcept{++unexpected;return s::Validation::Valid;});assert(plan.status==s::Status::MalformedTape&&unexpected==0);
 for(const auto invalidFlags:{2u,0x104u,6u}){malformed=tape;malformed[0].flags=invalidFlags;plan=s::Build(malformed,[&](const s::Record&)noexcept{++unexpected;return s::Validation::Valid;});assert(plan.status==s::Status::MalformedTape&&unexpected==0);}
 // Native equality and unordered distance behavior; scratch does not feed back.
 tape={Lamp(0,4),Lamp(1,4),Lamp(2,std::bit_cast<float>(0x7fc00001u))};valid={true,true,true};for(auto& r:tape)r.candidate.observed=false;Compare(tape,valid);
 std::mt19937 random(480047);constexpr unsigned Cases=30000;
 for(unsigned test=0;test<Cases;++test){
  tape.clear();valid.clear();const auto count=random()%129;
  for(unsigned i=0;i<count;++i){auto r=Lamp(i,float(random()%200));const auto family=random()%6;
   r.flags=family==0?0x404u:family==1?0x104u:family==2?0x24u:4u;
   r.deferred=r.flags==4&&(random()%4!=0);
   r.candidate.flags=r.flags;r.candidate.observed=random()%20!=0;r.candidate.ownBeam=(r.flags&0x100)!=0;
   r.candidate.claim=random()%8;r.candidate.focusDistance=random()%7==0?-1.f:float(random()%300);
   r.candidate.focusTier=random()%3;r.candidate.visualImportance=float(1+random()%5);
   r.candidate.stableOrder=r.candidate.ownBeam?random()%3:0;
   r.candidate.identity=1000+random()%100;
   tape.push_back(r);valid.push_back(random()%5!=0);
  }
  std::shuffle(tape.begin(),tape.end(),random);
  // Truth follows the now-current index; intentionally avoid index==tape row.
  std::vector<bool> shuffledTruth;for(const auto& r:tape)shuffledTruth.push_back(valid[r.index]);
  Compare(tape,shuffledTruth);
 }
 std::printf("%u randomized mixed-priority/continuity/equal-score tapes match independent eight-cell bulk baseline exactly\n",Cases);
 std::puts("offline planner only; no hook, key/cache authority, frame cost or FPS claim");
}
