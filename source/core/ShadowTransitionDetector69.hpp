#pragma once
#include <array>
#include <atomic>
#include <cstdint>

// Detect allocator transitions, not pixel flicker. Never changes selection,
// light flags, shadow maps, caster geometry or source lifetime authority.
namespace shadow_transition69 {
struct Identity {uint64_t key{},generation{};bool operator==(const Identity&) const=default;};
enum class Kind:uint8_t { PresentSourceDropped=1,RapidReturn=2 };
struct Event {Kind kind{};uint32_t frame{},tick{},elapsed{};Identity source{},replacement{};unsigned slot{};};
class Detector {
 struct Loss {Identity source{};uint32_t tick{};bool present{};};
 std::array<Identity,7> previous_{};
 std::array<Loss,32> losses_{};
 uintptr_t session_{},subject_{};uint32_t frame_{},tick_{};unsigned cursor_{};bool begun_{};
public:
 void Reset() noexcept {previous_={};losses_={};session_=subject_=0;frame_=tick_=cursor_=0;begun_=false;}
 template<class Present,class Emit>
 void Observe(uintptr_t session,uintptr_t subject,uint32_t frame,uint32_t tick,
  const std::array<Identity,7>& current,Present present,Emit emit) noexcept {
  if(!session){Reset();return;}
  if(!begun_||session!=session_||subject!=subject_||frame<frame_||uint32_t(tick-tick_)>2000){
   Reset();begun_=true;session_=session;subject_=subject;frame_=frame;tick_=tick;previous_=current;return;
  }
  if(frame==frame_)return; // Auxiliary/duplicate native pass is not a change.
  auto contains=[](const auto& values,Identity id) noexcept {for(const auto& value:values)if(value.key&&value==id)return true;return false;};
  for(unsigned slot=0;slot<7;++slot){const auto old=previous_[slot];
   if(!old.key||contains(current,old))continue;
   const bool observed=present(old);
   losses_[cursor_++%losses_.size()]={old,tick,observed};
   if(observed)emit(Event{Kind::PresentSourceDropped,frame,tick,0,old,current[slot],slot});
  }
  for(unsigned slot=0;slot<7;++slot){const auto now=current[slot];
   if(!now.key||contains(previous_,now))continue;
   for(auto& loss:losses_)if(loss.source==now){
    const auto elapsed=uint32_t(tick-loss.tick);
    if(loss.present&&elapsed<=750)emit(Event{Kind::RapidReturn,frame,tick,elapsed,now,{},slot});
    loss={};break;
   }
  }
  previous_=current;frame_=frame;tick_=tick;
 }
};
// Bounded nonblocking telemetry. Producer/consumer never wait or write files.
template<unsigned Capacity=64> class Ring {
 std::array<Event,Capacity> values_{};unsigned head_{},size_{};
 std::atomic_flag lock_=ATOMIC_FLAG_INIT;
public:
 std::atomic<uint32_t> dropped{0};
 bool Push(const Event& event) noexcept {
  if(lock_.test_and_set(std::memory_order_acquire)){++dropped;return false;}
  const bool room=size_<Capacity;
  if(room){values_[(head_+size_)%Capacity]=event;++size_;}else ++dropped;
  lock_.clear(std::memory_order_release);return room;
 }
 bool Pop(Event& event) noexcept {
  if(lock_.test_and_set(std::memory_order_acquire))return false;
  const bool found=size_!=0;
  if(found){event=values_[head_];head_=(head_+1)%Capacity;--size_;}
  lock_.clear(std::memory_order_release);return found;
 }
};
}
