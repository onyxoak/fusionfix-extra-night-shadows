#pragma once
#include "ShortlistHelperPatterns48.hpp"
#include <array>
#include <cstddef>
#include <cstring>
#include <limits>
namespace shortlist_helper48 {
enum class Reason:unsigned {Safe,Unconfigured,Bytes,Mode,NativeInit,Thunk,TlsIndex,TlsPointer,TlsEpoch,QualityPointer,ReadFault};
using Read=bool(*)(uintptr_t,void*,size_t) noexcept;
struct Binding {uintptr_t game{},fusion{};size_t gameSize{},fusionSize{};bool exact511Authenticated{};};
inline bool InImage(uintptr_t p,size_t n,uintptr_t base,size_t size) noexcept {
 return base&&p>=base&&n<=size&&p-base<=size-n&&base<=UINTPTR_MAX-size;
}
template<size_t N,size_t R> bool Pattern(Read read,uintptr_t address,
 const uint8_t(&expected)[N],const unsigned(&relocs)[R],uintptr_t base,uintptr_t recorded) noexcept {
 std::array<uint8_t,N> actual{},wanted{};
 if(!read(address,actual.data(),N))return false;
 std::memcpy(wanted.data(),expected,N);
 for(auto offset:relocs){uint32_t v{};std::memcpy(&v,wanted.data()+offset,4);v=uint32_t(v-recorded+base);std::memcpy(wanted.data()+offset,&v,4);}
 return actual==wanted;
}
inline bool Jump(Read read,uintptr_t from,uintptr_t& target) noexcept {
 uint8_t bytes[5]{};if(!read(from,bytes,5)||bytes[0]!=0xe9)return false;
 int32_t delta{};std::memcpy(&delta,bytes+1,4);
 const int64_t destination=int64_t(from)+5+delta;
 if(destination<0x10000||destination>UINT32_MAX)return false;target=uintptr_t(destination);return true;
}
// Read-only, operation-local proof. No native getter is called and no cache,
// constructor, Windows permission query or logging callback occurs here.
inline Reason Check(const Binding& b,Read read,uintptr_t tlsArray) noexcept {
 if(!read||!b.exact511Authenticated||!b.game||!b.fusion||b.game>UINT32_MAX||b.fusion>UINT32_MAX)return Reason::Unconfigured;
 if(!Pattern(read,b.game+NativeModeRva,NativeModeBytes,NativeModeRelocs,b.game,0x440000)||
    !Pattern(read,b.game+NativeSizeRva,NativeSizeBytes,NativeSizeRelocs,b.game,0x440000)||
    !Pattern(read,b.game+NativeInitRva,NativeInitBytes,NativeInitRelocs,b.game,0x440000)||
    !Pattern(read,b.game+NativeReadRva,NativeReadBytes,NativeReadRelocs,b.game,0x440000)||
    !Pattern(read,b.fusion+FusionReadRva,FusionReadBytes,FusionReadRelocs,b.fusion,0x56d00000))return Reason::Bytes;
 uint32_t mode{},init{},threshold{};uint64_t capacity{};
 if(!read(b.game+0xc45538,&mode,4)||!read(b.game+0x1899330,&init,4)||
    !read(b.game+0x13ed968,&capacity,8)||!read(b.game+0xd60ea8,&threshold,4))return Reason::ReadFault;
 if(mode!=0)return Reason::Mode;
 if(!(init&1))return Reason::NativeInit;
 uintptr_t thunk{},destination{};
 if(!Jump(read,b.game+0x525e70,thunk)||!Jump(read,thunk,destination)||destination!=b.fusion+FusionReadRva)return Reason::Thunk;
 uint32_t index{},tlsBlock{},qualityPointer{},quality{};int32_t globalEpoch{},threadEpoch{};uint8_t width{};uint32_t cookie{};
 if(!read(b.fusion+0x366150,&index,4))return Reason::ReadFault;
 if(index>=4096)return Reason::TlsIndex;
 if(tlsArray<0x10000||tlsArray>UINT32_MAX-index*4||!read(tlsArray+index*4,&tlsBlock,4)||tlsBlock<0x10000||tlsBlock>UINT32_MAX-0x14)return Reason::TlsPointer;
 if(!read(b.fusion+0x336604,&globalEpoch,4)||!read(tlsBlock+0x10,&threadEpoch,4))return Reason::ReadFault;
 // The authenticated initialized route branches to CRT only for signed greater.
 if(globalEpoch==-1||globalEpoch>threadEpoch)return Reason::TlsEpoch;
 if(!read(b.fusion+0x3365fc,&qualityPointer,4))return Reason::ReadFault;
 // Heap-backed settings have no proven lifetime contract: use full validation.
 if(!InImage(qualityPointer,4,b.game,b.gameSize)&&!InImage(qualityPointer,4,b.fusion,b.fusionSize))return Reason::QualityPointer;
 if(!read(qualityPointer,&quality,4)||!read(b.fusion+0x336518,&width,1)||!read(b.fusion+0x2e3980,&cookie,4))return Reason::ReadFault;
 return Reason::Safe;
}
}
