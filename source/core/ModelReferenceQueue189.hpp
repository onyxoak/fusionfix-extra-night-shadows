#pragma once
#include <array>
#include <cstdint>
#include <span>
namespace model_reference189 {
// CE542000 increments model+44, queues its index, and CE5424E0 releases
// every queued reference. One held reference per model per ring slot suffices
// until that slot drains. No draw, entity, light or shadow is omitted.
struct Seen {
 std::array<uint32_t,2048> bits{};
 uintptr_t pointer{};
 uint32_t count{};
 bool known{};
 bool Contains(uint16_t model) const noexcept {return (bits[model>>5]&(1u<<(model&31)))!=0;}
 void Mark(uint16_t model) noexcept {bits[model>>5]|=1u<<(model&31);}
 void Seed(uintptr_t p,std::span<const uint16_t> entries) noexcept {
  bits.fill(0);for(auto id:entries)Mark(id);pointer=p;count=uint32_t(entries.size());known=true;
 }
};
enum class Result { Added, AlreadyHeld, Invalid };
// Native append remains responsible for AddRef and the matching queue entry.
// A bounded spill retains an extra model without writing beyond the native
// allocation. Its matching release runs after the same native ring drains.
template<class Spill,class Append>
Result Retain(Seen& seen,uint16_t model,uint16_t*& entries,uint16_t& count,
              uint16_t& capacity,Spill&& spill,Append&& append) {
 if(!entries||!capacity||count>capacity)return Result::Invalid;
 if(!seen.known||seen.pointer!=uintptr_t(entries)||seen.count!=count)
  seen.Seed(uintptr_t(entries),{entries,count});
 if(seen.Contains(model))return Result::AlreadyHeld;
 if(count==capacity){if(!spill(model))return Result::Invalid;}
 else append();
 seen.Mark(model);seen.count=count;return Result::Added;
}
}
