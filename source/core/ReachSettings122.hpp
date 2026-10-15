#pragma once
#include <atomic>
#include <cstdint>
#include <string_view>
// Add-on-owned numeric settings. Legacy startup writes remain supported;
// only the authenticated selection thread publishes live complete pairs.
struct AddonSettings {
 std::atomic<int> headlightReach{10},lampReach{40};
private:
 std::atomic<uint64_t> livePair122_{0};
public:
 bool SetReachPair122(unsigned lamp,unsigned headlight) noexcept {
  if(lamp>100||headlight>100)return false;
  livePair122_.store((uint64_t(1)<<63)|(uint64_t(headlight)<<32)|lamp,std::memory_order_release);return true;
 }
 int Get(std::string_view name) const noexcept {
  if(name!="PREF_HEADLIGHT_REACH"&&name!="PREF_LAMP_REACH")return 0;
  const auto pair=livePair122_.load(std::memory_order_acquire);
  if(pair>>63)return name=="PREF_LAMP_REACH"?int(uint32_t(pair)):int(uint32_t(pair>>32)&0x7fffffffu);
  return name=="PREF_LAMP_REACH"?lampReach.load(std::memory_order_relaxed):headlightReach.load(std::memory_order_relaxed);
 }
};
