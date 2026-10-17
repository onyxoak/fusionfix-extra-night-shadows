#pragma once
#include <windows.h>
#include <bcrypt.h>
#include <array>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <algorithm>
namespace onyxoak::pe {std::filesystem::path ModulePath(HMODULE module) {
 std::array<wchar_t,32768> path{};
 if(!GetModuleFileNameW(module,path.data(),static_cast<DWORD>(path.size())))throw std::runtime_error("module path unavailable");
 return path.data();
}
inline HMODULE ModuleAt(uintptr_t address) noexcept {
 HMODULE module{};
 if(!address||!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
    reinterpret_cast<LPCWSTR>(address),&module))return nullptr;
 return module;
}
std::vector<uint8_t> Read(const std::filesystem::path& path) {
 std::ifstream file(path,std::ios::binary);if(!file)throw std::runtime_error("file unavailable");
 return {(std::istreambuf_iterator<char>(file)),{}};
}
std::string Hash(const std::vector<uint8_t>& data) {
 std::array<uint8_t,32> digest{};
 if(BCryptHash(BCRYPT_SHA256_ALG_HANDLE,nullptr,0,const_cast<PUCHAR>(data.data()),static_cast<ULONG>(data.size()),digest.data(),32)<0)
  throw std::runtime_error("SHA256 unavailable");
 std::ostringstream out;for(auto b:digest)out<<std::hex<<std::setfill('0')<<std::setw(2)<<unsigned(b);return out.str();
}
uint32_t U32(const uint8_t* p){uint32_t v;std::memcpy(&v,p,4);return v;}
struct Image {
 std::vector<uint8_t> bytes;
 uint32_t preferred{};
 uintptr_t mappedBase{};
 std::vector<IMAGE_SECTION_HEADER> sections;
 explicit Image(const std::vector<uint8_t>& raw,uintptr_t loaded=0) {
  if(raw.size()<4096 || raw[0]!='M'||raw[1]!='Z')throw std::runtime_error("invalid PE");
  const auto off=U32(raw.data()+0x3c);if(off>raw.size()-sizeof(IMAGE_NT_HEADERS32))throw std::runtime_error("invalid PE offset");
  const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(raw.data()+off);
  if(nt->Signature!=IMAGE_NT_SIGNATURE||nt->FileHeader.Machine!=IMAGE_FILE_MACHINE_I386||nt->OptionalHeader.Magic!=0x10b)
   throw std::runtime_error("not x86 PE");
  if(nt->OptionalHeader.SizeOfImage>128*1024*1024)throw std::runtime_error("image too large");
  preferred=nt->OptionalHeader.ImageBase;mappedBase=loaded?loaded:preferred;bytes.resize(nt->OptionalHeader.SizeOfImage);
  if(nt->OptionalHeader.SizeOfHeaders>raw.size()||nt->OptionalHeader.SizeOfHeaders>bytes.size())throw std::runtime_error("bad headers");
  std::memcpy(bytes.data(),raw.data(),nt->OptionalHeader.SizeOfHeaders);
  const auto* sec=IMAGE_FIRST_SECTION(nt);
  if(reinterpret_cast<const uint8_t*>(sec+nt->FileHeader.NumberOfSections)>raw.data()+raw.size())throw std::runtime_error("bad sections");
  for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){
   const auto s=sec[i];if(uint64_t(s.PointerToRawData)+s.SizeOfRawData>raw.size()||s.VirtualAddress>=bytes.size())throw std::runtime_error("bad section");
   const auto count=(std::min<size_t>)(s.SizeOfRawData,bytes.size()-s.VirtualAddress);
   std::memcpy(bytes.data()+s.VirtualAddress,raw.data()+s.PointerToRawData,count);sections.push_back(s);
  }
  if(loaded&&loaded!=preferred){
   const auto dir=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
   if(uint64_t(dir.VirtualAddress)+dir.Size>bytes.size())throw std::runtime_error("bad relocations");
   for(size_t n=0;n<dir.Size;){
    auto* block=reinterpret_cast<const IMAGE_BASE_RELOCATION*>(bytes.data()+dir.VirtualAddress+n);
    if(block->SizeOfBlock<8||block->SizeOfBlock>dir.Size-n)throw std::runtime_error("bad relocation block");
    const auto* fix=reinterpret_cast<const uint16_t*>(block+1);
    for(unsigned i=0;i<(block->SizeOfBlock-8)/2;++i){
     if((fix[i]>>12)==IMAGE_REL_BASED_ABSOLUTE)continue;
     if((fix[i]>>12)!=IMAGE_REL_BASED_HIGHLOW)throw std::runtime_error("unsupported relocation");
     const size_t at=block->VirtualAddress+(fix[i]&0xfff);if(at+4>bytes.size())throw std::runtime_error("relocation out of image");
     auto value=U32(bytes.data()+at)+static_cast<uint32_t>(loaded-preferred);std::memcpy(bytes.data()+at,&value,4);
    }n+=block->SizeOfBlock;
   }
  }
 }
 size_t Find(const char* pattern) const {
  std::istringstream in(pattern);std::vector<int> tokens;std::string token;
  while(in>>token)tokens.push_back(token=="?"?-1:std::stoi(token,nullptr,16));
  size_t found=0,count=0;
  for(const auto& s:sections){if(!(s.Characteristics&IMAGE_SCN_MEM_EXECUTE))continue;
   const auto end=(std::min<size_t>)(bytes.size(),size_t(s.VirtualAddress)+s.Misc.VirtualSize);
   for(size_t at=s.VirtualAddress;at+tokens.size()<=end;++at){bool match=true;
    for(size_t n=0;n<tokens.size();++n)if(tokens[n]>=0&&bytes[at+n]!=tokens[n]){match=false;break;}
    if(match){found=at;++count;}
   }
  }
  if(count!=1)throw std::runtime_error("pattern count="+std::to_string(count)+": "+pattern);return found;
 }
 size_t Pause() const {
  size_t found=0,count=0;uint32_t first=0,second=0;
  for(const auto& s:sections){if(!(s.Characteristics&IMAGE_SCN_MEM_EXECUTE))continue;
   const auto end=(std::min<size_t>)(bytes.size(),size_t(s.VirtualAddress)+s.Misc.VirtualSize);
   for(size_t at=s.VirtualAddress;at+12<=end;++at){const auto* p=bytes.data()+at;
    if(p[0]!=0x0a||p[1]!=0x05||p[6]!=0x0a||p[7]!=0x05)continue;
    const auto a=U32(p+2),b=U32(p+8);
    if(a<mappedBase||b<mappedBase||a>=mappedBase+bytes.size()||b>=mappedBase+bytes.size())continue;
    if(count&&!((first==a&&second==b)||(first==b&&second==a)))throw std::runtime_error("ambiguous pause operands "+std::to_string(first)+"/"+std::to_string(second)+" vs "+std::to_string(a)+"/"+std::to_string(b));
    first=a;second=b;found=at;++count;
   }
  }
  if(!count)throw std::runtime_error("pause operands absent");return found;
 }
};
}
