#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>
#include <cassert>
#include <vector>
#include <limits>
#include <iostream>
#include "FixtureLight244.hpp"
namespace harness {
using namespace fixture_light244;
struct SafetyHookContext {uint32_t esp{};};
struct Invocation {uint32_t owner=1,descriptor=2,modelInfo=3,frame=10;bool invalid=false;};
struct Sample {bool ownerMatched=true,modelInfoMatched=true,descriptorCopied=true;uint32_t identity=7,ownerKind=0;std::array<uint8_t,0x6c> descriptor{};std::array<uint8_t,68> ownerPose109{};};
std::atomic<bool> addonActive{true};
namespace ModelLightShadow20 {bool requested=true;std::atomic<bool> active{true};}
namespace model_light_submission20 {struct Abi {unsigned words=18;explicit operator bool()const{return words!=0;}};Abi Layout(uint32_t site){return {site==1?18u:site==2?16u:0u};}}
std::array<uint32_t,18> stack{};std::array<float,3> direction{0,0,-1};
uint32_t hash=IndustrialFourHead,frame=10;bool rejectOwner=false,failRead=false,mutateStack=false,staleFrame=false;
uint32_t Frame(){return staleFrame?frame+1:frame;}
bool Copy(uintptr_t address,void* out,size_t n){
 if(failRead)return false;
 if(address==0x3f&&n==4){std::memcpy(out,&hash,4);return true;}
 if(address==0x2000&&n==12){std::memcpy(out,direction.data(),12);return true;}
 if(address==uint32_t(uintptr_t(stack.data()))&&n<=sizeof(stack)){std::memcpy(out,stack.data(),n);if(mutateStack)*reinterpret_cast<uint32_t*>(out)^=1;return true;}
 return false;
}
namespace TrafficSignal {
struct Match {uint32_t identity=7,kind=0,modelInfo=3;std::array<uint8_t,68> ownerPose109{};};
bool MatchOwnerWithDescriptor48(uint32_t,Match&,uint32_t,void* out,size_t n,bool& copied){copied=!rejectOwner;if(copied)std::memset(out,0,n);return copied;}
}
void ValidateSource46(const Invocation&,Sample& sample){if(rejectOwner)sample.ownerMatched=false;}
#include "FixtureLight244Runtime.inc"
}
int main(){
 using namespace harness;
 float range{};assert(Range(IndustrialFourHead,0,10,{0,0,-1},range)&&range==32);
 assert(!Range(IndustrialFourHead,2,10,{0,0,-1},range));
 assert(!Range(123,0,10,{0,0,-1},range));
 assert(!Range(IndustrialFourHead,0,50,{0,0,-1},range)&&range==50);
 assert(!Range(IndustrialFourHead,0,10,{1,0,0},range));
 assert(!Range(IndustrialFourHead,0,std::numeric_limits<float>::quiet_NaN(),{0,0,-1},range));
 assert(!Range(IndustrialFourHead,0,10,{0,0,std::numeric_limits<float>::infinity()},range));
 FixtureLight244::requested=true;
 for(unsigned abi:{1u,2u})for(unsigned failure=0;failure<8;++failure){
  stack.fill(0);stack[3]=0x2000;stack[10]=std::bit_cast<uint32_t>(10.f);
  auto arguments=stack;const auto before=stack;
  SafetyHookContext regs{uint32_t(uintptr_t(stack.data()))};Invocation invocation{};Sample sample{};
  hash=failure==1?123:IndustrialFourHead;rejectOwner=failure==2;failRead=failure==3;mutateStack=failure==4;staleFrame=failure==5;
  FixtureLight244::requested=failure!=6;addonActive=failure!=7;
  FixtureLight244::Correct(regs,abi,arguments,invocation,sample);
  if(failure==0){auto expected=before;expected[10]=std::bit_cast<uint32_t>(32.f);assert(stack==expected&&arguments==expected);}
  else assert(stack==before&&arguments==before);
 }
 std::cout<<"PASS244 production pre-store correction: both native ABIs, exact radius-only mutation, global model identity and stale/fault/owner refusal\n";
}
