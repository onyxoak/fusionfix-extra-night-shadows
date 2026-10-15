#include "core/DeferredModelShortlist48.hpp"
#include "core/ShadowAllocationPass.hpp"
#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
namespace s=fusionfix::shadows::shortlist48;
namespace budget=fusionfix::shadows::budget;
namespace rage{struct CLightSource{uint32_t mFlags{},mCastShadows{};int32_t mShadowCacheIndex=-1;std::array<uint8_t,116> padding{};};}
static_assert(sizeof(rage::CLightSource)==128);
struct State{
 std::array<rage::CLightSource,640> lights{};rage::CLightSource* shortlistLights48=lights.data();uint32_t shortlistCount48=8;
 std::array<s::Candidate,4096> nativeCandidates{};std::array<uint8_t,640> pending48{};std::array<uint32_t,16> resident48{};
 budget::ShadowAllocationPass pass;bool deferModels48=true,continuityActive=true;
}state;
std::array<uint8_t,4096> labRelevant{};
std::atomic<uint32_t> shortlistResident48{0},shortlistDeferred48{0},shortlistValidated48{0},shortlistInvalid48{0},fallbackPasses{0};
std::array<std::atomic<uint32_t>,8> rejectedPassReasons{};
uintptr_t gameBase=0x440000;
unsigned validations{},flushes{},labObserved{};bool verdict=true;
bool Validate(rage::CLightSource& l) noexcept{++validations;if(!verdict)l.mFlags&=~4u;return verdict;}
auto validateModelSource48=&Validate;
bool (*labPointSource)(const rage::CLightSource&) noexcept=nullptr;
void FlushPending48() noexcept{++flushes;state.deferModels48=false;for(unsigned i=0;i<8;++i)if(state.pending48[i]){state.pending48[i]=0;Validate(state.lights[i]);}}
#include "preview48-extracted.inc"
#include "defer48-extracted.inc"
struct Registers{uintptr_t esp{},eip{};};
void Stage(Registers& regs,uint32_t index) noexcept{
 auto* lights=state.shortlistLights48;const uint32_t count=state.shortlistCount48;
 const bool deferredCurrent48=state.deferModels48&&state.pending48[index];
 const auto& light=lights[index];const auto key=light.mCastShadows;
 const auto kind=budget::Kind::Lamp;struct Geometry{float distanceSquared=100.f;}geometry;
 const bool affectsPlayer=true;const uint64_t generation=1;const float viewWeight=1,reach=60,priorityDistance=-1;
 #include "collect-stage48-extracted.inc"
}
void Reset(std::array<uint8_t,256>& stack,bool strong){
 state.deferModels48=state.continuityActive=true;state.pending48.fill(0);state.resident48.fill(0);validations=flushes=0;verdict=true;
 for(unsigned i=0;i<8;++i){state.lights[i].mFlags=4;state.lights[i].mCastShadows=100+i;state.lights[i].mShadowCacheIndex=-1;
  state.nativeCandidates[i]={};state.nativeCandidates[i].observed=true;state.nativeCandidates[i].flags=4;state.nativeCandidates[i].focusDistance=float(i+1);}
 state.nativeCandidates[7].focusDistance=strong?0.f:100.f;
 for(unsigned i=0;i<7;++i){int32_t n=int(i);float d=float(i+1);uint32_t f=4;
  std::memcpy(stack.data()+0x7c+i*4,&n,4);std::memcpy(stack.data()+0x9c+i*4,&f,4);std::memcpy(stack.data()+0x5c+i*4,&d,4);}
 float current=strong?0.f:100.f;std::memcpy(stack.data()+0x20,&current,4);
 state.pass.Begin({1,1,1,false},state.lights.data(),8,true);
 for(unsigned i=0;i<7;++i)state.pass.Observe({100+i,i,budget::Kind::Lamp,float(i+1),true,true,1,1,3600,-1},&state.lights[i]);
}
int main(){
 std::array<uint8_t,256> stack{};Reset(stack,false);
 assert(DeferModel48(state.lights[7],7));assert(state.pending48[7]);
 const auto before=stack;Registers regs{reinterpret_cast<uintptr_t>(stack.data()),0};Stage(regs,7);
 assert(validations==0&&state.pending48[7]&&regs.eip==0&&stack==before&&state.pass.Active());
 std::array<int32_t,7> chosen{0,1,2,3,4,5,6};assert(state.pass.Commit(state.lights.data(),8,chosen.data(),true,true));
 for(auto i:chosen)assert(i!=7);
 Reset(stack,true);assert(DeferModel48(state.lights[7],7));verdict=false;regs.eip=0;Stage(regs,7);
 assert(validations==1&&!state.pending48[7]&&!(state.lights[7].mFlags&4)&&regs.eip==gameBase+0x527ddc&&state.pass.Active());
 // Invalid source never enters the transaction snapshot; unchanged prefix commits.
 assert(state.pass.Commit(state.lights.data(),8,chosen.data(),true,true));
 Reset(stack,true);assert(DeferModel48(state.lights[7],7));regs.eip=0;Stage(regs,7);
 assert(validations==1&&!state.pending48[7]&&(state.lights[7].mFlags&4)&&regs.eip==0);
 // Resident, cache-backed and native-special sources retain early validation.
 for(uint32_t flags:{2u,6u,0x104u}){state.pending48.fill(0);state.lights[7].mFlags=flags;assert(!DeferModel48(state.lights[7],7));}
 state.lights[7].mFlags=4;state.resident48[15]=107;assert(!DeferModel48(state.lights[7],7));
 state.resident48.fill(0);state.lights[7].mShadowCacheIndex=0;assert(!DeferModel48(state.lights[7],7));
 std::puts("production-extracted Collect stage: zero-write no fresh read, failed fresh winner skips 527DDC before snapshot, accepted winner fresh; residents/native specials stay early");
}
