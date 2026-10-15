#pragma once
#include <windows.h>
#include <bcrypt.h>
#include <algorithm>
#include <array>
#include <cstring>
#include <optional>
#include <vector>
#include <string>
namespace compatibility117 {
struct Canonical {std::vector<uint8_t> bytes;size_t resourceOffset{},resourceSize{},timestampOffset{},checksumOffset{};};
inline std::optional<Canonical> CanonicalImage(const std::vector<uint8_t>& raw) {
 auto range=[&](size_t at,size_t n){return at<=raw.size()&&n<=raw.size()-at;};
 auto word=[&](size_t at){uint16_t v{};std::memcpy(&v,raw.data()+at,2);return v;};
 auto dword=[&](size_t at){uint32_t v{};std::memcpy(&v,raw.data()+at,4);return v;};
 if(!range(0,64)||raw[0]!='M'||raw[1]!='Z')return {};
 const size_t pe=dword(0x3c);if(!range(pe,24)||dword(pe)!=0x4550||word(pe+4)!=0x14c)return {};
 const size_t optional=pe+24,optionalBytes=word(pe+20),count=word(pe+6);
 if(!range(optional,optionalBytes)||optionalBytes<224||word(optional)!=0x10b||dword(optional+92)<16||!count||count>96)return {};
 const size_t sections=optional+optionalBytes;if(!range(sections,count*40))return {};
 const uint32_t resourceRva=dword(optional+112),resourceBytes=dword(optional+116);
 if(!resourceRva||!resourceBytes)return {};
 size_t offset=0,bytes=0;unsigned owners=0;
 for(size_t i=0;i<count;++i){const size_t s=sections+i*40;const uint32_t va=dword(s+12),virtualBytes=dword(s+8),diskBytes=dword(s+16),diskOffset=dword(s+20);
  if(uint64_t(resourceRva)>=va&&uint64_t(resourceRva)<uint64_t(va)+virtualBytes){
   if(!range(diskOffset,diskBytes)||!diskBytes||(dword(s+36)&0x20000000u)||uint64_t(resourceRva)+resourceBytes>uint64_t(va)+virtualBytes)return {};
   offset=diskOffset;bytes=diskBytes;++owners;
  }
 }
 if(owners!=1)return {};
 Canonical result{raw,offset,bytes,pe+8,optional+64};
 // No engine instructions, import/relocation/TLS directory, data or layout field
 // is masked. Existing full mapped-image and hook guards remain mandatory.
 std::fill(result.bytes.begin()+offset,result.bytes.begin()+offset+bytes,0);
 for(size_t at:{pe+8,optional+64,optional+116})std::fill_n(result.bytes.begin()+at,4,0);
 return result;
}
inline std::string Hash(const std::vector<uint8_t>& bytes){
 std::array<uint8_t,32> digest{};if(bytes.size()>UINT32_MAX||BCryptHash(BCRYPT_SHA256_ALG_HANDLE,nullptr,0,const_cast<PUCHAR>(bytes.data()),ULONG(bytes.size()),digest.data(),32)<0)return {};
 constexpr char hex[]="0123456789abcdef";std::string out;out.reserve(64);for(auto b:digest){out+=hex[b>>4];out+=hex[b&15];}return out;
}
inline constexpr const char* ExpectedCanonicalCE117="117fa32c231de3044e3527372479f93e8a01ab50c8f968e1b9674b1a71c90a4b4";
inline bool Accept(const std::vector<uint8_t>& raw,const char* expected) {
 if(Hash(raw)==expected)return true;
 if(std::strcmp(expected,"08759a5516f9837920ea504436236bbab89d0826a8e4d04ff106345177b5345d"))return false;
 const auto canonical=CanonicalImage(raw);return canonical&&Hash(canonical->bytes)==ExpectedCanonicalCE117;
}
}
