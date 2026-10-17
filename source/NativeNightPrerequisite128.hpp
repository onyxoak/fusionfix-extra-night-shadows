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
struct Route {
 shortlist_helper48::Binding binding{};bool authenticated{};
 uintptr_t getter{},qualityPointer{},epoch{},tlsIndex{};
};
inline constexpr uint32_t NativeGetterRva=0x525e70,NativeGetterBytes=0x28;
inline constexpr uint32_t QualityPointerRva=0x3365fc,EpochRva=0x336604,TlsIndexRva=0x366150;

// These RVAs describe the native game getter and historical test fixtures.
// The hooked getter and every DLL data address are recovered from live code;
// the legacy exact511Authenticated flag is deliberately not consulted.
inline Route Authenticate(const shortlist_helper48::Binding& b,
 std::span<const uint8_t> original,shortlist_helper48::Read read) noexcept {
 Route result{};if(!read||original.size()!=NativeGetterBytes||
  !shortlist_helper48::InImage(b.game+NativeGetterRva,NativeGetterBytes,b.game,b.gameSize)||
  !b.fusion||!b.fusionSize)return result;
 std::array<uint8_t,NativeGetterBytes> current{};
 if(!read(b.game+NativeGetterRva,current.data(),current.size())||current[0]!=0xe9||current[5]!=0x90||
  std::memcmp(current.data()+6,original.data()+6,NativeGetterBytes-6))return result;
 uintptr_t destination=b.game+NativeGetterRva;
 std::array<uintptr_t,8> visited{};
 for(size_t hop=0;hop<visited.size();++hop){
  for(size_t i=0;i<hop;++i)if(visited[i]==destination)return result;
  visited[hop]=destination;
  uint8_t jump[5]{};if(!read(destination,jump,sizeof(jump)))return result;
  if(jump[0]!=0xe9)break;
  if(!shortlist_helper48::Jump(read,destination,destination))return result;
 }
 constexpr size_t size=sizeof(shortlist_helper48::FusionReadBytes);
 if(!shortlist_helper48::InImage(destination,size,b.fusion,b.fusionSize))return result;
 std::array<uint8_t,size> body{};if(!read(destination,body.data(),body.size()))return result;
 // This is an instruction/argument contract, with relocated globals and call
 // targets recovered below. Neither a recorded module base nor a release ID
 // participates. A genuinely different contract leaves status unavailable.
 constexpr unsigned calls[]={213,269,279};
 for(size_t i=0;i<size;++i){
  bool operand=false;
  for(auto offset:shortlist_helper48::FusionReadRelocs)operand|=i>=offset&&i<offset+4;
  for(auto offset:calls)operand|=i>offset&&i<offset+5;
  if(!operand&&body[i]!=shortlist_helper48::FusionReadBytes[i])return result;
 }
 const auto word=[&](unsigned offset){uint32_t value{};std::memcpy(&value,body.data()+offset,4);return uintptr_t(value);};
 const auto dllData=[&](uintptr_t at){return shortlist_helper48::InImage(at,4,b.fusion,b.fusionSize);};
 for(auto offset:shortlist_helper48::FusionReadRelocs)if(!dllData(word(offset)))return result;
 for(auto offset:calls){
  int32_t relative{};std::memcpy(&relative,body.data()+offset+1,4);
  const int64_t target=int64_t(destination)+offset+5+relative;
  if(target<0||!shortlist_helper48::InImage(uintptr_t(target),1,b.fusion,b.fusionSize))return result;
 }
 const auto tlsIndex=word(43),epoch=word(51),quality=word(68),width=word(88);
 if(word(209)!=epoch||word(223)!=epoch||word(275)!=epoch||word(252)!=quality||
    word(123)!=width||word(158)!=width||word(82)!=destination+292)return result;
 constexpr unsigned cases[]={191,86,121,156};
 for(size_t i=0;i<std::size(cases);++i)if(word(292+unsigned(i)*4)!=destination+cases[i])return result;
 result.binding=b;result.authenticated=true;result.getter=destination;
 result.qualityPointer=quality;result.epoch=epoch;result.tlsIndex=tlsIndex;
 return result;
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
 if(!read(route.tlsIndex,&index,4)){out.reason=Reason::ReadFault;return out;}
 if(index>=4096||tlsArray<0x10000||(tlsArray&3u)||tlsArray>UINT32_MAX-index*4){out.reason=Reason::Tls;return out;}
 if(!read(tlsArray+index*4,&block,4)||block<0x10000||(block&3u)||block>UINT32_MAX-0x14){out.reason=Reason::Tls;return out;}
 if(!read(route.epoch,&epoch,4)||!read(block+0x10,&threadEpoch,4)){out.reason=Reason::ReadFault;return out;}
 if(epoch==-1||epoch>threadEpoch){out.reason=Reason::Initializing;return out;}
 if(!read(route.qualityPointer,&pointer,4)){out.reason=Reason::ReadFault;return out;}
 if((pointer&3u)||(!shortlist_helper48::InImage(pointer,4,b.game,b.gameSize)&&
    !shortlist_helper48::InImage(pointer,4,b.fusion,b.fusionSize))){out.reason=Reason::Pointer;return out;}
 if(!read(pointer,&quality,4)||!read(route.qualityPointer,&againPointer,4)||
  !read(route.epoch,&againEpoch,4)||!read(tlsArray+index*4,&againBlock,4)){out.reason=Reason::ReadFault;return out;}
 if(pointer!=againPointer||epoch!=againEpoch||block!=againBlock){out.reason=Reason::Changed;return out;}
 if(quality>3){out.reason=Reason::Value;return out;}
 out={quality?State::Enabled:State::Off,Reason::Observed,int32_t(quality)};return out;
}
}
