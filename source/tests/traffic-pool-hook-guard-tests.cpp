#include "TrafficPoolHookGuard.hpp"
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace traffic_signal;
struct Fixture {
 uint32_t game=0x400000,ff=0x10000000,relay=0x300000,stub=0x301000,trampoline=0x302000;
 std::vector<uint8_t> original=std::vector<uint8_t>(0x66,0xcc);
 std::map<uint32_t,std::vector<uint8_t>> memory;
 static void Put(std::vector<uint8_t>& b,size_t at,uint32_t n){std::memcpy(b.data()+at,&n,4);}
 static std::vector<uint8_t> Jump(uint32_t at,uint32_t to){std::vector<uint8_t>b(5);b[0]=0xe9;Put(b,1,to-at-5);return b;}
 Fixture(uint32_t base=0x400000):game(base){
  constexpr uint8_t displaced[]{0x8b,0xc7,0x0f,0xaf,0xc2};std::copy(std::begin(displaced),std::end(displaced),original.begin()+10);
  auto live=original;auto j=Jump(game+PoolHookRva,relay);std::copy(j.begin(),j.end(),live.begin()+10);memory[game+PoolCtorRva]=live;
  memory[relay]=Jump(relay,stub);
  auto code=std::vector<uint8_t>(onyxoak::MidStubTemplate.begin(),onyxoak::MidStubTemplate.end());
  Put(code,2,stub+167);Put(code,89,stub+163);Put(code,163,ff+PoolCallbackRva);Put(code,167,trampoline);memory[stub]=code;
  memory[ff+PoolCallbackRva]=Jump(ff+PoolCallbackRva,ff+PoolCallbackBodyRva);
  auto continuation=std::vector<uint8_t>(10,0x90);j=Jump(trampoline+5,game+PoolHookRva+5);std::copy(j.begin(),j.end(),continuation.begin()+5);memory[trampoline]=continuation;
 }
 auto Verify(bool identity=true){return VerifyPoolHook(game,ff,identity,original,[&](uint32_t address,size_t n){
  auto it=memory.find(address);if(it==memory.end()||n>it->second.size())throw std::runtime_error("test_unreadable_destination");
  return std::vector<uint8_t>(it->second.begin(),it->second.begin()+n);
 });}
 bool Refuses(const char* reason){try{Verify();return false;}catch(const std::exception&e){return std::string(e.what())==reason;}}
};
int main(){
 Fixture valid;const auto proof=valid.Verify();assert(proof.relay==valid.relay&&proof.stub==valid.stub&&proof.trampoline==valid.trampoline);
 // Address relocation, backward branches and unsigned 32-bit branch wrap.
 for(auto base:{0x400000u,0x800000u,0xf0000000u}){Fixture f(base);assert(f.Verify().stub==f.stub);}
 try{valid.Verify(false);assert(false);}catch(const std::exception&e){assert(std::string(e.what())=="traffic_pool_FusionFix_identity_unverified");}
 for(size_t i=0;i<0x66;++i){if(i>=10&&i<15)continue;Fixture f;f.memory[f.game+PoolCtorRva][i]^=1;assert(f.Refuses("traffic_pool_ctor_remaining_bytes_changed"));}
 for(size_t i=0;i<163;++i){if((i>=2&&i<6)||(i>=89&&i<93))continue;Fixture f;f.memory[f.stub][i]^=1;assert(f.Refuses("traffic_pool_stub_code_changed"));}
 for(size_t i:{2,89}){Fixture f;f.memory[f.stub][i]^=1;assert(f.Refuses("traffic_pool_stub_slots_changed"));}
 {Fixture f;f.original[10]^=1;assert(f.Refuses("traffic_pool_original_instructions_changed"));}
 {Fixture f;f.memory[f.game+PoolCtorRva][10]=0x90;assert(f.Refuses("traffic_pool_hook_opcode_changed"));}
 {Fixture f;f.memory[f.relay][0]=0x90;assert(f.Refuses("traffic_pool_relay_changed"));}
 {Fixture f;f.memory[f.stub][163]^=1;assert(f.Refuses("traffic_pool_callback_owner_changed"));}
 {Fixture f;f.memory[f.ff+PoolCallbackRva][0]=0x90;assert(f.Refuses("traffic_pool_callback_thunk_changed"));}
 {Fixture f;f.memory[f.ff+PoolCallbackRva][1]^=1;assert(f.Refuses("traffic_pool_callback_destination_changed"));}
 for(size_t i=0;i<5;++i){Fixture f;f.memory[f.trampoline][i]=0xcc;assert(f.Refuses("traffic_pool_continuation_instructions_changed"));}
 {Fixture f;f.memory[f.trampoline][5]=0xc3;assert(f.Refuses("traffic_pool_continuation_opcode_changed"));}
 {Fixture f;f.memory[f.trampoline][6]^=1;assert(f.Refuses("traffic_pool_continuation_destination_changed"));}
 {Fixture f;f.memory.erase(f.stub);assert(f.Refuses("test_unreadable_destination"));}
 {Fixture f;f.memory[f.stub].resize(100);assert(f.Refuses("test_unreadable_destination"));}
 std::cout<<"PASS exact FF511 pool-hook authentication; 97 ctor-byte and 155 stub-byte mutations; ownership, slots, relocation, continuation and unreadable gates\n";
}
