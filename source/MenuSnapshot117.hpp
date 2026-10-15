#pragma once
#include "NativeGraphicsMenu117.hpp"
namespace native_graphics117 {
struct Snapshot {
 std::array<Option,RowLimit> rows{};uint16_t count{};bool active{};
 void Capture(std::span<const Option> current) noexcept {
  active=false;if(current.empty()||current.size()>RowLimit||!MakePlan(current,true))return;
  std::copy(current.begin(),current.end(),rows.begin());count=uint16_t(current.size());active=true;
 }
 // Native/FF appends may move storage. Adoption is based only on copied row
 // VALUES: never dereference the old freed allocation. Preserve all old rows
 // in order and the exact owned block before renewing a receipt.
 bool CanRenew(std::span<const Option> current) const noexcept {
  if(!active||!count||count>RowLimit||current.size()<count||current.size()>RowLimit)return false;
  const auto plan=MakePlan(current,true);if(!plan||!plan->already)return false;
  if(std::memcmp(&rows[count-1],&current.back(),sizeof(Option)))return false;
  // Renew only append-before-END changes. Every earlier physical ordinal,
  // including the replacement slot, must stay exact for native sliders.
  for(size_t old=0;old+1<count;++old)
   if(std::memcmp(&rows[old],&current[old],sizeof(Option)))return false;
  return true;
 }
};
}
