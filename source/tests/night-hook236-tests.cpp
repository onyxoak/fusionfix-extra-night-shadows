#include "NativeNightPrerequisite128.hpp"
#include "night-hook236-live-fixture.hpp"
#include <cassert>
#include <cstdio>
#include <map>
#include <vector>
namespace n=native_night128;
namespace g=shortlist_helper48;
namespace f=night_fixture236;
std::map<uintptr_t,std::vector<uint8_t>> memory236;
bool Read236(uintptr_t at,void* out,size_t size) noexcept {
 for(const auto& [base,bytes]:memory236)if(at>=base&&at-base<=bytes.size()&&size<=bytes.size()-(at-base)){
  std::memcpy(out,bytes.data()+at-base,size);return true;
 }
 return false;
}
void Word236(std::vector<uint8_t>& bytes,unsigned at,uint32_t value){std::memcpy(bytes.data()+at,&value,4);}
void Put236(uintptr_t at,uint32_t value){auto& bytes=memory236[at];bytes.resize(4);std::memcpy(bytes.data(),&value,4);}
uint32_t Word236(const uint8_t* bytes,unsigned at){uint32_t value{};std::memcpy(&value,bytes+at,4);return value;}
void Jump236(uint32_t at,uint32_t to){auto& bytes=memory236[at];bytes={0xe9,0,0,0,0};Word236(bytes,1,to-at-5);}
int main(){
 constexpr uint32_t game=0x400000,relay=0x320000,tls=0x330000,block=0x331000,value=game+0x380000;
 for(const auto base:{0x10000000u,0x65000000u})for(const auto rva:{0x12340u,0x1f400u}){
  memory236.clear();const uint32_t dest=base+rva;
  g::Binding binding{game,base,0x1be6400,0x400000,false};
  std::vector<uint8_t> original(std::begin(f::Native),std::end(f::Native));
  auto native=original;native[0]=0xe9;native[5]=0x90;Word236(native,1,relay-(game+n::NativeGetterRva)-5);
  memory236[game+n::NativeGetterRva]=native;Jump236(relay,dest);
  std::vector<uint8_t> body(std::begin(f::Body),std::end(f::Body));
  for(auto at:g::FusionReadRelocs)Word236(body,at,Word236(f::Body,at)-f::ModuleBase+base);
  Word236(body,82,dest+292);constexpr unsigned cases[]={191,86,121,156};
  for(unsigned i=0;i<4;++i)Word236(body,292+4*i,dest+cases[i]);
  for(auto at:{213u,269u,279u}){
   const uint32_t target=f::Getter+at+5+Word236(f::Body,at+1)-f::ModuleBase+base;
   Word236(body,at+1,target-dest-at-5);
  }
  memory236[dest]=body;
  auto route=n::Authenticate(binding,original,Read236);
  assert(route.authenticated&&route.getter==dest);
  assert(route.tlsIndex==Word236(body.data(),43)&&route.epoch==Word236(body.data(),51)&&route.qualityPointer==Word236(body.data(),68));
  // No legacy fixture slot is populated: observations must use recovered data.
  assert(route.qualityPointer!=base+n::QualityPointerRva);
  Put236(route.tlsIndex,7);Put236(tls+28,block);Put236(block+0x10,uint32_t(INT32_MIN+5));
  Put236(route.epoch,uint32_t(INT32_MIN+4));Put236(route.qualityPointer,value);
  for(uint32_t q=0;q<=3;++q){Put236(value,q);const auto result=n::Observe(route,Read236,tls);assert(result.quality==int32_t(q)&&result.state==(q?n::State::Enabled:n::State::Off));}
  Put236(route.epoch,uint32_t(-1));assert(n::Observe(route,Read236,tls).reason==n::Reason::Initializing);
  Put236(route.epoch,uint32_t(INT32_MIN+4));Put236(value,4);assert(n::Observe(route,Read236,tls).state==n::State::Unknown);
  // Contradictory bindings and real opcode changes are capability failures.
  Word236(memory236[dest],223,uint32_t(route.epoch+4));assert(!n::Authenticate(binding,original,Read236).authenticated);
  memory236[dest]=body;memory236[dest][72]^=1;assert(!n::Authenticate(binding,original,Read236).authenticated);
  memory236[dest]=body;Jump236(relay,relay);assert(!n::Authenticate(binding,original,Read236).authenticated);
 }
 std::puts("PASS236 actual live hook body at relocated addresses and new getter offsets, legacy version flag false, recovered TLS/epoch/value slots, live on/off/invalid/init transitions, cycle and contradictory-code rejection");
}
