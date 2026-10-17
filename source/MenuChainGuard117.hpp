#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <span>
namespace menu_chain117 {
enum class Owner:uint8_t {Refused,Native,FusionFix};
struct Result {Owner owner=Owner::Refused;uint32_t first{},destination{};unsigned hops{};};
inline uint32_t Jump(uint32_t from,std::span<const uint8_t> bytes) noexcept {
 if(bytes.size()<5||bytes[0]!=0xe9)return 0;
 uint32_t relative{};std::memcpy(&relative,bytes.data()+1,4);
 return from+5+relative;
}
// Authenticate one already-present chain. No trampoline-adjacent bytes are
// mistaken for the native function and arbitrary third-party chains are refused.
// Reader.Copy/Executable must freshly validate memory; FF authentication and
// relocated expected bytes are supplied by the exact module adapter.
template<class Reader> Result Authenticate(Reader& reader,uint32_t entry,
 std::span<const uint8_t> native,uint32_t ffEntry,std::span<const uint8_t> ffExpected,unsigned patchBytes=5) noexcept {
 Result out{};if(entry<0x10000||native.size()<16||native.size()>64)return out;
 std::array<uint8_t,64> current{};
 if(!reader.Executable(entry,native.size())||!reader.Copy(entry,current.data(),native.size()))return out;
 if(!std::memcmp(current.data(),native.data(),native.size())){out.owner=Owner::Native;out.destination=entry;return out;}
 if(!ffEntry||ffExpected.size()<16||ffExpected.size()>64||current[0]!=0xe9||patchBytes<5||patchBytes>=native.size())return out;
 for(unsigned n=5;n<patchBytes;++n)if(current[n]!=0x90)return out;
 if(std::memcmp(current.data()+patchBytes,native.data()+patchBytes,native.size()-patchBytes))return out;
 const auto first=Jump(entry,std::span(current).first(5));
 if(first<0x10000||first==entry)return out;
 out.first=first;out.hops=1;auto destination=first;
 if(first!=ffEntry){
  std::array<uint8_t,5> thunk{};
  if(!reader.Executable(first,5)||!reader.Copy(first,thunk.data(),5)||thunk[0]!=0xe9)return {};
  destination=Jump(first,thunk);out.hops=2;
 }
 if(destination!=ffEntry||!reader.Executable(ffEntry,ffExpected.size())||
    !reader.Copy(ffEntry,current.data(),ffExpected.size())||
    std::memcmp(current.data(),ffExpected.data(),ffExpected.size()))return {};
 out.owner=Owner::FusionFix;out.destination=destination;return out;
}
// Resolve the actual current owner instead of selecting a DLL by release hash
// or callback RVA. An unhooked native entry is valid with or without FusionFix.
// A hook must end in executable code of the loaded module and match that
// module's relocated image. The native patch and every jump are checked again
// by Authenticate; changed/foreign chains are never silently overwritten.
template<class Reader,class Executable> Result Discover233(Reader& reader,uint32_t entry,
 std::span<const uint8_t> native,uint32_t module,std::span<const uint8_t> image,
 Executable executable,unsigned patchBytes=5) noexcept {
 if(!module)return Authenticate(reader,entry,native,0,{},patchBytes);
 if(module<0x10000||image.size()<32||uint64_t(module)+image.size()>uint64_t(UINT32_MAX)+1)return {};
 std::array<uint8_t,64> current{};
 if(native.size()<16||native.size()>current.size()||!reader.Executable(entry,native.size())||
    !reader.Copy(entry,current.data(),native.size()))return {};
 if(!std::memcmp(current.data(),native.data(),native.size()))return Authenticate(reader,entry,native,0,{},patchBytes);
 if(current[0]!=0xe9)return {};
 const auto first=Jump(entry,std::span(current).first(5));auto destination=first;
 auto inside=[&](uint32_t at,size_t n){return at>=module&&uint64_t(at-module)+n<=image.size();};
 if(!inside(destination,32)){
  std::array<uint8_t,5> thunk{};
  if(first<0x10000||first==entry||!reader.Executable(first,thunk.size())||
     !reader.Copy(first,thunk.data(),thunk.size())||thunk[0]!=0xe9)return {};
  destination=Jump(first,thunk);
 }
 if(!inside(destination,32)||!executable(destination-module,32))return {};
 const auto result=Authenticate(reader,entry,native,destination,image.subspan(destination-module,32),patchBytes);
 if(result.owner!=Owner::FusionFix||result.first!=first||result.destination!=destination)return {};
 return result;
}
}
