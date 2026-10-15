#pragma once
#include "NativeShadowContinuity42.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <utility>

namespace fusionfix::shadows::shortlist48 {
// Offline planner only. The caller supplies the complete ordered Collect tape,
// never guessed candidates or the final top eight. No engine pointers/writes.
constexpr unsigned Capacity=640,Slots=7;
using Candidate=NativeShadowContinuity42::Candidate;
struct Record {
 std::int32_t index=-1;
 std::uint32_t flags{};
 float nativeDistance{};
 Candidate candidate{};
 bool deferred=false;
};
struct Cell {std::int32_t index=-1;std::uint32_t flags{};float distance=-1.f;};
using Cells=std::array<Cell,Slots>;
enum class Validation : std::uint8_t { Unknown,Valid,Invalid,Unavailable };
enum class Status : std::uint8_t { Ready,MalformedTape,ValidationUnavailable };
struct Plan {
 Status status=Status::MalformedTape;
 Cells cells{};
 std::array<Validation,Capacity> validations{};
 unsigned validationCalls{},rounds{},replayVisits{};
};
// 5253F0 instruction semantics. Zero incumbent flags mean empty; 0x400
// challenger inserts even against another 0x400. This is not a std::sort order.
constexpr int NativeFlags(std::uint32_t challenger,std::uint32_t incumbent) noexcept {
 if(!incumbent || (challenger&0x400u))return 0;
 if(challenger&0x100u){if(incumbent&0x400u)return 2;return int((incumbent>>8)&1u);}
 return (incumbent&0x500u)?2:1;
}
inline bool Insert(Cells& result,const Record& original,std::span<const Record> tape,
 const std::array<std::int16_t,Capacity>& byIndex) noexcept {
  bool wrote=false;Cell carry{original.index,original.flags,original.nativeDistance};
  for(unsigned row=0;row<Slots;++row){
   auto& slot=result[row];int choice=NativeFlags(carry.flags,slot.flags);
   // 527D6F runs after two PUSHes: its [ESP+24] is original [ESP+1C].
   // Therefore 527DB2 restores displaced flags along with index/distance.
   if(carry.index>=0 && slot.index>=0){
    const auto ai=byIndex[carry.index],bi=byIndex[slot.index];
    const Record* a=ai>=0?&tape[ai]:nullptr;const Record* b=bi>=0?&tape[bi]:nullptr;
    if(a&&b&&a->candidate.observed&&b->candidate.observed&&
       a->candidate.flags==carry.flags&&b->candidate.flags==slot.flags)
     choice=NativeShadowContinuity42::Compare(choice,a->candidate,b->candidate);
   }
   // COMISS/JBE: an equal/unordered distance does not replace. The eighth
   // scratch cell cannot feed back into the first seven and is not consumed.
   if(choice==0 || (choice==1 && slot.distance>carry.distance)){
    const auto displaced=slot;slot=carry;carry=displaced;wrote=true;
   }
  }
 return wrote;
}
template<class Validate>
Plan Build(std::span<const Record> tape,Validate&& validate) noexcept {
 Plan plan{};
 if(tape.size()>Capacity)return plan;
 std::array<std::int16_t,Capacity> byIndex{};byIndex.fill(-1);
 for(unsigned n=0;n<tape.size();++n){const auto& record=tape[n];
  if(record.index<0 || unsigned(record.index)>=Capacity || byIndex[record.index]>=0 ||
     (record.deferred&&(!(record.flags&4u)||(record.flags&0x102u))))return plan;
  byIndex[record.index]=static_cast<std::int16_t>(n);
  plan.validations[n]=record.deferred?Validation::Unknown:Validation::Valid;
 }
 // Inductive exact prefix: freshly validate each deferred candidate BEFORE
 // any cell write it would cause. A zero-write input cannot affect later
 // selection, even if the comparator is not a strict total order. Failed
 // insertions leave the complete prefix intact; the rest of the tape refills.
 plan.rounds=1;
 for(unsigned n=0;n<tape.size();++n){
  ++plan.replayVisits;auto proposed=plan.cells;
  const auto writes=Insert(proposed,tape[n],tape,byIndex);
  if(!writes)continue;
  if(plan.validations[n]==Validation::Unknown){
   ++plan.validationCalls;const auto verdict=validate(tape[n]);
   if(verdict!=Validation::Valid&&verdict!=Validation::Invalid){plan.status=Status::ValidationUnavailable;return plan;}
   plan.validations[n]=verdict;
  }
  if(plan.validations[n]==Validation::Valid)plan.cells=proposed;
 }
 plan.status=Status::Ready;return plan;
}
}
