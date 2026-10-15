#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <span>
#include <string>
#include <string_view>
#include "StartupErrorCodes129.hpp"
namespace startup_conflict129 {
struct Mismatch {
 bool valid{};
 std::uint32_t rva{};
 unsigned count{};
 std::array<std::uint8_t,8> expected{},actual{};
};
inline Mismatch FirstMismatch(std::span<const std::uint8_t> expected,
 std::span<const std::uint8_t> actual,std::uint32_t rva) noexcept {
 Mismatch result{};
 if(expected.size()!=actual.size()||std::uint64_t(rva)+expected.size()>std::uint64_t(UINT32_MAX)+1)return result;
 for(size_t i=0;i<expected.size();++i)if(expected[i]!=actual[i]){
  result.valid=true;result.rva=rva+std::uint32_t(i);
  result.count=unsigned((std::min<size_t>)(8,expected.size()-i));
  std::memcpy(result.expected.data(),expected.data()+i,result.count);
  std::memcpy(result.actual.data(),actual.data()+i,result.count);break;
 }
 return result;
}
inline std::string Token(std::string_view text,std::string_view fallback) {
 if(text.empty()||text.size()>96)return std::string(fallback);
 for(char c:text)if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='-'||c=='.'))return std::string(fallback);
 return std::string(text);
}
inline std::string Fields(std::string_view build,std::string_view error,const Mismatch& mismatch) {
 // Only compile-time fixed errors from the actual startup guards. Foreign
 // exception messages (even plain ASCII names/tokens) are never copied.
 const bool known=std::find(KnownStartupErrors129.begin(),KnownStartupErrors129.end(),error)!=KnownStartupErrors129.end();
 std::string result="candidate="+Token(build,"unknown_candidate")+" error="+(known?std::string(error):"startup_exception");
 if(!mismatch.valid)return result+" normalized_game_rva=unavailable";
 char field[64]{};sprintf_s(field," normalized_game_rva=%08X expected=",mismatch.rva);result+=field;
 const auto count=(std::min)(mismatch.count,8u);
 for(unsigned i=0;i<count;++i){sprintf_s(field,"%02X",unsigned(mismatch.expected[i]));result+=field;}
 result+=" actual=";
 for(unsigned i=0;i<count;++i){sprintf_s(field,"%02X",unsigned(mismatch.actual[i]));result+=field;}
 return result+" normalized_guard_evidence=1 attribution=unknown";
}
}
