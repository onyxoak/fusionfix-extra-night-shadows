#include "core/DeferredModelShortlist48.hpp"
#include "core/ShadowAllocationPass.hpp"
#include "NativeProjectionRegistry66.hpp"
#include <array>
#include <cassert>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <random>
namespace s=fusionfix::shadows::shortlist48;
struct Light{uint32_t mFlags;};
struct State{std::array<Light,640> lights{};Light* shortlistLights48=lights.data();uint32_t shortlistCount48=640;std::array<s::Candidate,4096> nativeCandidates{};}state;
#include "preview48-extracted.inc"
int main(){
 std::mt19937 rng(480048);std::array<uint8_t,256> stack{};std::array<s::Record,640> tape{};std::array<int16_t,640> byIndex{};
 for(unsigned i=0;i<640;++i){tape[i].index=int(i);byIndex[i]=int16_t(i);}
 for(unsigned trial=0;trial<50000;++trial){
  s::Cells cells{};
  for(unsigned i=0;i<8;++i){auto& r=tape[i];r.flags=i%4==0?0x404u:i%4==1?0x104u:4u;
   r.nativeDistance=float(rng()%200);r.candidate={};r.candidate.flags=r.flags;r.candidate.observed=rng()%8!=0;
   r.candidate.claim=rng()%5;r.candidate.identity=rng()%6;r.candidate.focusDistance=float(rng()%150);r.candidate.focusTier=rng()%3;
   state.nativeCandidates[i]=r.candidate;state.lights[i].mFlags=r.flags;
   if(i<7){cells[i]={rng()%7==0?-1:int(i),r.flags,r.nativeDistance};if(cells[i].index<0)cells[i].flags=0;
    std::memcpy(stack.data()+0x7c+i*4,&cells[i].index,4);std::memcpy(stack.data()+0x9c+i*4,&cells[i].flags,4);std::memcpy(stack.data()+0x5c+i*4,&cells[i].distance,4);}
  }
  std::memcpy(stack.data()+0x20,&tape[7].nativeDistance,4);
  const auto before=stack;auto proposed=cells;
  const bool reference=s::Insert(proposed,tape[7],tape,byIndex);
  assert(PreviewWrites48(reinterpret_cast<uintptr_t>(stack.data()),7)==reference);
  assert(stack==before); // Preview never writes native cells or scratch.
 }
 // OWN66 winner-only witness gate uses this actual extracted production preview.
 // A populated seven-entry prefix rejects505 weak challengers with no witness.
 for(bool improving:{false,true}){
  constexpr unsigned sources=512;std::array<s::Record,640> records{};std::array<int16_t,640> indices{};
  for(unsigned i=0;i<640;++i){records[i].index=int(i);records[i].flags=4;records[i].candidate={};indices[i]=int16_t(i);
   state.lights[i].mFlags=4;state.nativeCandidates[i]={};}
  s::Cells prefix{};for(unsigned i=0;i<7;++i)prefix[i]={-1,0,0};
  unsigned witnesses=0,skips=0;
  for(unsigned i=0;i<sources;++i){auto& r=records[i];r.nativeDistance=improving?float(sources-i):float(i+1);
   r.candidate.flags=4;r.candidate.observed=true;r.candidate.focusDistance=r.nativeDistance;
   state.nativeCandidates[i]=r.candidate;
   for(unsigned row=0;row<7;++row){std::memcpy(stack.data()+0x7c+row*4,&prefix[row].index,4);
    std::memcpy(stack.data()+0x9c+row*4,&prefix[row].flags,4);std::memcpy(stack.data()+0x5c+row*4,&prefix[row].distance,4);}
   std::memcpy(stack.data()+0x20,&r.nativeDistance,4);const auto beforePreview=stack;
   const auto writes=PreviewWritesBank66(reinterpret_cast<uintptr_t>(stack.data()),i,state.lights.data(),sources);
   auto reference=prefix;const bool expected=s::Insert(reference,r,records,indices);assert(writes==expected&&stack==beforePreview);
   const auto decision=native_projection66::GateInsertion(true,true,writes,[&](){++witnesses;return 1u;});
   if(decision==native_projection66::InsertionDecision::Skipped)++skips;
   else {assert(decision==native_projection66::InsertionDecision::Accepted);prefix=reference;}
  }
  assert(witnesses==(improving?512u:7u));assert(skips==(improving?0u:505u));
  for(unsigned row=0;row<7;++row)assert(prefix[row].index==int(improving?511-row:row));
 }
 // An unchecked zero-write input can be observed but never published by
 // nativeSelectionOnly. Give it best possible policy score to test the boundary.
 using namespace fusionfix::shadows::budget;
 std::array<std::array<uint8_t,128>,8> packets{};ShadowAllocationPass pass;
 pass.Begin({1,1,1,false},packets.data(),8,true);
 for(unsigned i=0;i<8;++i)pass.Observe({100+i,i,Kind::Lamp,i==7?0.f:100.f,true,true,1,3.f,3600.f,-1.f},packets[i].data());
 std::array<int32_t,7> selected{0,1,2,3,4,5,6};const auto native=selected;
 assert(pass.Commit(packets.data(),8,selected.data(),true,true));assert(selected==native);
 for(const auto& slot:pass.LastSelection().slots)assert(slot.index!=7);
 std::puts("production nativeSelectionOnly preserves seven prefix indices despite best-scoring observed unchecked eighth input");
 std::puts("50,000 production-extracted native-cell previews equal exact prefix planner; input cells unchanged");
}
