#pragma once
#include <atomic>
#include <cstdint>
#include <limits>
#include <optional>
namespace fusion_backend130 {
// FF511's four effective tuples are injective. No CSettings layout is used.
inline std::optional<uint8_t> Decode(uint8_t extra,uint8_t head,uint8_t vehicle) noexcept {
 if(extra>1||head>1||vehicle>1)return {};
 if(!extra&&!head&&vehicle)return uint8_t(0);
 if(extra&&!head&&vehicle)return uint8_t(1);
 if(extra&&head&&!vehicle)return uint8_t(2);
 if(extra&&head&&vehicle)return uint8_t(3);
 return {};
}
struct Lease {
 uint32_t owner{};
 std::atomic<bool> armed{false},wanted{false},owned{false};
 // Linearized genuine requests: sequence in high62bits, mode in low2bits.
 // Only numeric values cross threads; never retain callback pointers/objects.
 std::atomic<uint64_t> requested{0};
 uint64_t processed=UINT64_MAX;bool processedWanted=false,dirty=false; // owner thread only
 bool Arm(uint32_t thread,uint8_t mode) noexcept {
  if(!thread||mode>3||armed.load(std::memory_order_acquire))return false;
  owner=thread;requested.store(mode,std::memory_order_relaxed);processed=mode;processedWanted=false;dirty=false;
  wanted.store(false,std::memory_order_relaxed);armed.store(true,std::memory_order_release);return true;
 }
 bool Owns() const noexcept {return armed.load(std::memory_order_acquire)&&wanted.load(std::memory_order_acquire)&&owned.load(std::memory_order_acquire);}
 void Close() noexcept {owned.store(false,std::memory_order_release);wanted.store(false,std::memory_order_release);}
 bool Request(uint8_t mode) noexcept {
  if(!armed.load(std::memory_order_acquire)||mode>3){Close();return false;}
  auto before=requested.load(std::memory_order_acquire);
  for(;;){
   if((before>>2)==((std::numeric_limits<uint64_t>::max)()>>2)){Close();return false;}
   const auto after=((before>>2)+1)*4+mode;
   if(requested.compare_exchange_weak(before,after,std::memory_order_acq_rel,std::memory_order_acquire))return true;
  }
 }
 bool Pending(uint32_t thread) const noexcept {
  return armed.load(std::memory_order_acquire)&&thread==owner&&
   (requested.load(std::memory_order_acquire)!=processed||wanted.load(std::memory_order_acquire)!=processedWanted||(!wanted.load(std::memory_order_acquire)&&dirty));
 }
 template<class Writable,class Original>
 bool Apply(uint32_t thread,bool force,Writable writable,Original original) noexcept {
  if(!armed.load(std::memory_order_acquire)||thread!=owner)return false;
  const auto value=requested.load(std::memory_order_acquire);
  const bool enabled=wanted.load(std::memory_order_acquire);
  if(!force&&!Pending(thread))return true;
  // An exceptional request while tuple3 is already owned has no additional
  // effects. Coalesce it numerically; original calls are intentionally deferred.
  if(force||!enabled||!owned.load(std::memory_order_acquire)){
   if(!writable()){Close();return false;}
   dirty=true; // Includes a fault partway through the official three stores.
   if(!original(enabled?uint8_t(3):uint8_t(value&3))){Close();return false;}
   if(!enabled)dirty=false;
  }
  processed=value;processedWanted=enabled;
  if(enabled)owned.store(true,std::memory_order_release);
  // Close may race this final store, but Owns also requires wanted=true.
  return true;
 }
};
}
