#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <span>
#include <stdexcept>
namespace native_owner109 {
inline constexpr uint32_t PoolRva=0x126d9ec,Stride=0x80,VtableRva=0xaaf554;
struct CodeRange {uint32_t rva,size;};
// Native initializer, constructor + deleting destructor, and pool iterator.
// Generic 70BA00 destruction is guarded by the existing Liberty20 free hook.
inline constexpr std::array<CodeRange,5> Code{{{0x7584e0,0x70},{0x7588b0,0x35},{0x7589a0,0x50},{0x631ee0,0x50},{0x60d330,0x34}}};
template<class Read>
void Authenticate(std::span<const uint8_t> original,Read read) {
 for(const auto range:Code){
  if(uint64_t(range.rva)+range.size>original.size())throw std::runtime_error("native_class5_original_range_invalid");
  const auto actual=read(range.rva,range.size);
  if(actual.size()!=range.size||std::memcmp(actual.data(),original.data()+range.rva,range.size))
   throw std::runtime_error("native_class5_body_changed");
 }
}
inline bool Range(uintptr_t owner,uintptr_t storage,int32_t size,int32_t stride) noexcept {
 return size>0&&size<=65536&&stride>0&&storage>=0x10000&&
  uint64_t(storage)+uint64_t(uint32_t(size))*uint32_t(stride)<=uint64_t(UINT32_MAX)+1&&
  owner>=storage&&uint64_t(owner-storage)<uint64_t(uint32_t(size))*uint32_t(stride)&&
  (owner-storage)%uint32_t(stride)==0;
}
inline bool Class(const std::array<uint8_t,0x30>& bytes,uintptr_t imageBase109) noexcept {
 uintptr_t vtable{};uint32_t kind{};
 std::memcpy(&vtable,bytes.data(),4);std::memcpy(&kind,bytes.data()+0x28,4);
 return vtable==imageBase109+VtableRva&&((kind>>6)&15u)==5;
}
}
