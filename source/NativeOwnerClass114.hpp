#pragma once
#include "NativeOwnerClass109.hpp"
namespace native_owner114 {
inline constexpr uint32_t PoolRva=0x12f7d60,Stride=0x110,VtableRva=0xad46a4;
inline constexpr std::array<native_owner109::CodeRange,4> Code{{{0x879760,0x51},{0x879870,0x35},{0x631e40,0x50},{0x60d330,0x34}}};
template<class Read>void Authenticate(std::span<const uint8_t> original,Read read){
 for(const auto range:Code){
  if(uint64_t(range.rva)+range.size>original.size())throw std::runtime_error("native_instance114_original_range_invalid");
  const auto actual=read(range.rva,range.size);
  if(actual.size()!=range.size||std::memcmp(actual.data(),original.data()+range.rva,range.size))
   throw std::runtime_error("native_instance114_body_changed");
 }
}
inline bool Class(const std::array<uint8_t,0x30>& bytes,uintptr_t imageBase114) noexcept {
 uintptr_t vtable{};uint32_t kind{};
 std::memcpy(&vtable,bytes.data(),4);std::memcpy(&kind,bytes.data()+0x28,4);
 return vtable==imageBase114+VtableRva&&((kind>>6)&15u)==1;
}
}
