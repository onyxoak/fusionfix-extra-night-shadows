#pragma once
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <span>
#include <stdexcept>
namespace native_copy70 {
inline constexpr uint32_t CopyRva=0x6bc8a0,CallbackRva=0x608b0,AllocationRva=0x3325f8;
inline constexpr size_t CopySize=0xdc,CallbackSize=0x113;
struct Evidence {bool fusionFixOverlay{};uint32_t relay{},trampoline{};};
inline uint32_t Word(const uint8_t* p){uint32_t n;std::memcpy(&n,p,4);return n;}
inline void Need(bool value,const char* reason){if(!value)throw std::runtime_error(reason);}
// The exact511CodeVerified premise means the module hash AND every relocated
// executable section were authenticated by VerifyOfficial. CallbackSize spans
// the normal function body through RET4; lazy-init/exception callees outside it
// remain covered by that full-module premise. No other overlay is authorized.
template<class Reader>
Evidence Verify(uint32_t game,uint32_t ff,bool exact511CodeVerified,
 std::span<const uint8_t> original,std::span<const uint8_t> expectedCallback,Reader read){
 Need(original.size()==CopySize,"copy70_original_size");
 const auto live=read(game+CopyRva,CopySize);
 Need(live.size()==CopySize,"copy70_native_unreadable");
 if(!std::memcmp(live.data(),original.data(),CopySize))return {};
 Need(exact511CodeVerified&&ff,"copy70_unverified_overlay");
 Need(expectedCallback.size()==CallbackSize,"copy70_callback_size");
 constexpr uint8_t displaced[]{0x8b,0x54,0x24,0x04,0xf3,0x0f,0x10,0x42,0x04};
 Need(!std::memcmp(original.data(),displaced,9),"copy70_displaced_instructions");
 Need(live[0]==0xe9,"copy70_native_branch");
 for(size_t i=5;i<9;++i)Need(live[i]==0x90,"copy70_native_padding");
 Need(!std::memcmp(live.data()+9,original.data()+9,CopySize-9),"copy70_native_suffix");
 const auto relay=uint32_t(game+CopyRva+5+Word(live.data()+1));
 const auto relayBytes=read(relay,5);
 Need(relayBytes.size()==5&&relayBytes[0]==0xe9,"copy70_relay_branch");
 Need(uint32_t(relay+5+Word(relayBytes.data()+1))==uint32_t(ff+CallbackRva),"copy70_callback_owner");
 const auto callback=read(ff+CallbackRva,CallbackSize);
 Need(callback.size()==CallbackSize&&!std::memcmp(callback.data(),expectedCallback.data(),CallbackSize),"copy70_callback_changed");
 // Authentic compiled CopyLight loads this Allocation before forwarding the
// original ECX, EDX, stack argument, then runs the official after-copy event.
 Need(callback[0x57]==0xa1&&Word(callback.data()+0x58)==uint32_t(ff+AllocationRva),"copy70_original_slot");
 constexpr uint8_t invoke[]{0xff,0x75,0x08,0x8b,0xd6,0x8b,0xcf,0xff,0xd0,0x8b,0xf8};
 Need(!std::memcmp(callback.data()+0x70,invoke,sizeof(invoke)),"copy70_original_call_contract");
 const auto allocation=read(ff+AllocationRva,8);
 Need(allocation.size()==8,"copy70_allocation_unreadable");
 const auto trampoline=Word(allocation.data());
 Need(trampoline>=0x10000&&Word(allocation.data()+4)==19&&relay==uint32_t(trampoline+14),"copy70_allocation_layout");
 const auto continuation=read(trampoline,14);
 Need(continuation.size()==14&&!std::memcmp(continuation.data(),displaced,9)&&continuation[9]==0xe9,"copy70_trampoline_instructions");
 Need(uint32_t(trampoline+14+Word(continuation.data()+10))==uint32_t(game+CopyRva+9),"copy70_trampoline_continuation");
 return {true,relay,trampoline};
}
}
