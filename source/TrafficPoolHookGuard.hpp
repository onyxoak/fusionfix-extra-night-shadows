#pragma once
#include "standalone-midstub.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <span>
#include <stdexcept>
namespace traffic_signal {
// Exact FF 5.1.1 atPool adapter. The module identity and executable sections
// must have been verified before this check. Heap code is authenticated through
// the complete SafetyHook stub, its callback, and its native continuation.
inline constexpr uint32_t PoolCtorRva=0x86c5f0,PoolHookRva=0x86c5fa;
inline constexpr uint32_t PoolCallbackRva=0x98570,PoolCallbackBodyRva=0x9cae0;
struct PoolHookEvidence {uint32_t relay{},stub{},trampoline{};};
inline uint32_t Word(const uint8_t* p){uint32_t n;std::memcpy(&n,p,4);return n;}
inline void Need(bool value,const char* reason){if(!value)throw std::runtime_error(reason);}
// A native constructor needs no overlay or observer: its original size/flags
// semantics are the same ones consumed by the fresh pool identity checks.
// Authenticate the entire mapped function, including displaced arithmetic.
template<class Reader>
void VerifyNativePoolCtor(uint32_t game,std::span<const uint8_t> original,Reader read){
 Need(original.size()==0x66,"traffic_native_pool_original_range_invalid");
 constexpr uint8_t arithmetic[]{0x8b,0xc7,0x0f,0xaf,0xc2};
 Need(!std::memcmp(original.data()+10,arithmetic,sizeof(arithmetic)),
      "traffic_native_pool_original_instructions_changed");
 const auto live=read(game+PoolCtorRva,original.size());
 Need(live.size()==original.size(),"traffic_native_pool_ctor_unreadable");
 Need(!std::memcmp(live.data(),original.data(),original.size()),
      "traffic_native_pool_ctor_changed");
}
template<class Reader>
PoolHookEvidence VerifyPoolHook(uint32_t game,uint32_t ff,bool exact511,
 std::span<const uint8_t> original,Reader read){
 Need(exact511,"traffic_pool_FusionFix_identity_unverified");
 Need(original.size()==0x66,"traffic_pool_original_range_invalid");
 constexpr uint8_t replaced[]{0x8b,0xc7,0x0f,0xaf,0xc2};
 Need(!std::memcmp(original.data()+10,replaced,5),"traffic_pool_original_instructions_changed");
 const auto live=read(game+PoolCtorRva,original.size());
 Need(live.size()==original.size(),"traffic_pool_ctor_unreadable");
 for(size_t i=0;i<original.size();++i)if(i<10||i>=15)
  Need(live[i]==original[i],"traffic_pool_ctor_remaining_bytes_changed");
 auto branch=[&](uint32_t address,const char* reason){
  const auto b=read(address,5);Need(b.size()==5&&b[0]==0xe9,reason);
  return uint32_t(address+5+Word(b.data()+1));
 };
 Need(live[10]==0xe9,"traffic_pool_hook_opcode_changed");
 const uint32_t relay=game+PoolHookRva+5+Word(live.data()+11);
 const auto stubAddress=branch(relay,"traffic_pool_relay_changed");
 const auto stub=read(stubAddress,onyxoak::MidStubTemplate.size());
 Need(stub.size()==onyxoak::MidStubTemplate.size(),"traffic_pool_stub_unreadable");
 for(size_t i=0;i<163;++i){
  if((i>=2&&i<6)||(i>=89&&i<93))continue;
  Need(stub[i]==onyxoak::MidStubTemplate[i],"traffic_pool_stub_code_changed");
 }
 Need(Word(stub.data()+2)==uint32_t(stubAddress+167)&&
      Word(stub.data()+89)==uint32_t(stubAddress+163),"traffic_pool_stub_slots_changed");
 Need(Word(stub.data()+163)==uint32_t(ff+PoolCallbackRva),"traffic_pool_callback_owner_changed");
 Need(branch(ff+PoolCallbackRva,"traffic_pool_callback_thunk_changed")==uint32_t(ff+PoolCallbackBodyRva),
      "traffic_pool_callback_destination_changed");
 const auto trampoline=Word(stub.data()+167);
 const auto continuation=read(trampoline,10);
 Need(continuation.size()==10,"traffic_pool_continuation_unreadable");
 for(size_t i=0;i<5;++i)Need(continuation[i]==0x90,"traffic_pool_continuation_instructions_changed");
 Need(continuation[5]==0xe9,"traffic_pool_continuation_opcode_changed");
 Need(uint32_t(trampoline+10+Word(continuation.data()+6))==uint32_t(game+PoolHookRva+5),
      "traffic_pool_continuation_destination_changed");
 return {relay,stubAddress,trampoline};
}
}
