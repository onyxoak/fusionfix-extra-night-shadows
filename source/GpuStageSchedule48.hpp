#pragma once
#include <cstdint>
namespace gpu48 {
// Caller serializes with the existing profiler mutex. Reset/rebind invalidates
// all stack tokens; never decrement a new scene's depth using an old token.
class StageSchedule {
public:
 struct Token { uint64_t epoch=0,interval=0; uint32_t generation=0,ordinal=0; bool registered=false,sample=false; };
 uint64_t observed=0,nested=0,budgetSkipped=0;
private:
 uint64_t epoch=0,interval=0;uint32_t generation=0,depth=0,ordinal=0;
 bool scene=false;
public:
 void Reset() noexcept {++epoch;scene=false;depth=ordinal=0;}
 void BeginScene(uint64_t i,uint32_t g) noexcept {Reset();interval=i;generation=g;scene=true;}
 bool Open() const noexcept {return depth!=0;}
 Token Begin() noexcept {
  if(!scene)return {};
  Token token{epoch,interval,generation,0,true,false};++observed;
  if(depth++){++nested;return token;}
  token.ordinal=++ordinal;
  if(interval%16==0){if(ordinal<=7)token.sample=true;else ++budgetSkipped;}
  return token;
 }
 bool End(Token token) noexcept {
  if(!token.registered||!scene||token.epoch!=epoch||token.interval!=interval||token.generation!=generation||!depth)return false;
  --depth;return token.sample;
 }
};
}
