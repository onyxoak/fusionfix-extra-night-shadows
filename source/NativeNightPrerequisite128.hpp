#pragma once
// Read-only public setup status. No CSettings object layout or native calls.
#include "ShortlistHelperGuard48.hpp"
#include <atomic>
#include <span>

namespace native_night128 {
enum class State:uint8_t {Unknown,Off,Enabled};
enum class Reason:uint8_t {Unconfigured,Body,Chain,ReadFault,Tls,Initializing,Pointer,Value,Changed,Observed};
struct Observation {State state=State::Unknown;Reason reason=Reason::Unconfigured;int32_t quality=-1;};
inline std::atomic<State> currentState{State::Unknown};
struct Route {shortlist_helper48::Binding binding{};bool authenticated{};};
inline constexpr uint32_t NativeGetterRva=0x525e70,NativeGetterBytes=0x28;
inline constexpr uint32_t QualityPointerRva=0x3365fc,EpochRva=0x336604,TlsIndexRva=0x366150;

// Called once against the already authenticated CE/officialFF511 modules.
// Native getter's six-byte known FF overlay and its entire original tail are
// checked; FF's complete relocated getter includes its static initialization.
inline Route Authenticate(const shortlist_helper48::Binding& b,
 std::span<const uint8_t> original,shortlist_helper48::Read read) noexcept {
 Route result{};if(!b.exact511Authenticated||!read||original.size()!=NativeGetterBytes||
  !shortlist_helper48::InImage(b.game+NativeGetterRva,NativeGetterBytes,b.game,b.gameSize)||
  !shortlist_helper48::InImage(b.fusion+TlsIndexRva,4,b.fusion,b.fusionSize))return result;
 std::array<uint8_t,NativeGetterBytes> current{};
 if(!read(b.game+NativeGetterRva,current.data(),current.size())||current[0]!=0xe9||current[5]!=0x90||
  std::memcmp(current.data()+6,original.data()+6,NativeGetterBytes-6))return result;
 uintptr_t thunk{},destination{};
 if(!shortlist_helper48::Jump(read,b.game+NativeGetterRva,thunk))return result;
 if(thunk==b.fusion+shortlist_helper48::FusionReadRva)destination=thunk;
 else if(!shortlist_helper48::Jump(read,thunk,destination))return result;
 if(destination!=b.fusion+shortlist_helper48::FusionReadRva||
  !shortlist_helper48::Pattern(read,destination,shortlist_helper48::FusionReadBytes,
    shortlist_helper48::FusionReadRelocs,b.fusion,0x56d00000))return result;
 result.binding=b;result.authenticated=true;return result;
}

// Fresh small observational reads only. Never initializes FF's GetRef/CRT,
// derives an emission identity, changes effective engine policy or caches a
// borrowed pointer. Unknown cannot silently become Off or Enabled.
inline Observation Observe(const Route& route,shortlist_helper48::Read read,
 uintptr_t tlsArray) noexcept {
 Observation out{};if(!route.authenticated||!read)return out;
 const auto& b=route.binding;
 uint32_t index{},block{},pointer{},quality{},againPointer{},againBlock{};
 int32_t epoch{},threadEpoch{},againEpoch{};
 if(!read(b.fusion+TlsIndexRva,&index,4)){out.reason=Reason::ReadFault;return out;}
 if(index>=4096||tlsArray<0x10000||(tlsArray&3u)||tlsArray>UINT32_MAX-index*4){out.reason=Reason::Tls;return out;}
 if(!read(tlsArray+index*4,&block,4)||block<0x10000||(block&3u)||block>UINT32_MAX-0x14){out.reason=Reason::Tls;return out;}
 if(!read(b.fusion+EpochRva,&epoch,4)||!read(block+0x10,&threadEpoch,4)){out.reason=Reason::ReadFault;return out;}
 if(epoch==-1||epoch>threadEpoch){out.reason=Reason::Initializing;return out;}
 if(!read(b.fusion+QualityPointerRva,&pointer,4)){out.reason=Reason::ReadFault;return out;}
 if((pointer&3u)||(!shortlist_helper48::InImage(pointer,4,b.game,b.gameSize)&&
    !shortlist_helper48::InImage(pointer,4,b.fusion,b.fusionSize))){out.reason=Reason::Pointer;return out;}
 if(!read(pointer,&quality,4)||!read(b.fusion+QualityPointerRva,&againPointer,4)||
  !read(b.fusion+EpochRva,&againEpoch,4)||!read(tlsArray+index*4,&againBlock,4)){out.reason=Reason::ReadFault;return out;}
 if(pointer!=againPointer||epoch!=againEpoch||block!=againBlock){out.reason=Reason::Changed;return out;}
 if(quality>3){out.reason=Reason::Value;return out;}
 out={quality?State::Enabled:State::Off,Reason::Observed,int32_t(quality)};return out;
}
}
