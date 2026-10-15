#pragma once
#include <array>
#include <atomic>
#include <cstdint>
namespace frame_boundary154 {
enum Flag:uint32_t{First=1,ClockDiscontinuity=2,PauseGap=4,NativeFrameUnavailable=8,NativeFrameDiscontinuity=16};
struct Record{uint64_t ordinal{},start{},interval{};uint32_t nativeFrame{},nativeStep{},thread{},flags{};};
template<unsigned Capacity=4096>struct State{
 std::array<Record,Capacity>rows{};std::atomic<uint64_t>write{0},read{0},drops{0},threadRefusals{0},clockRefusals{0};
 std::atomic<uint32_t>owner{0};uint64_t previous{},ordinal{};uint32_t previousFrame=UINT32_MAX;bool paused{};
 void Pause(uint32_t thread)noexcept{if(!thread){++threadRefusals;return;}uint32_t empty=0;owner.compare_exchange_strong(empty,thread);if(owner.load()!=thread){++threadRefusals;return;}previous=0;previousFrame=UINT32_MAX;paused=true;}
 void Sample(uint32_t thread,uint32_t frame,uint64_t now)noexcept{
  if(!now){++clockRefusals;return;}if(!thread){++threadRefusals;return;}uint32_t empty=0;owner.compare_exchange_strong(empty,thread);if(owner.load()!=thread){++threadRefusals;return;}
  uint32_t flags=paused?PauseGap:0;paused=false;uint64_t interval=0;uint32_t step=0;
  if(!previous)flags|=First;else if(now<=previous)flags|=ClockDiscontinuity;else interval=now-previous;
  if(frame==UINT32_MAX)flags|=NativeFrameUnavailable;else if(previousFrame!=UINT32_MAX){step=frame-previousFrame;if(step>1024)flags|=NativeFrameDiscontinuity;}
  previous=now;previousFrame=frame;Record row{++ordinal,now,interval,frame,step,thread,flags};const auto w=write.load(std::memory_order_relaxed),r=read.load(std::memory_order_acquire);if(w-r>=Capacity){++drops;return;}rows[w%Capacity]=row;write.store(w+1,std::memory_order_release);
 }
 unsigned Drain(Record*out,unsigned max)noexcept{auto r=read.load();const auto w=write.load(std::memory_order_acquire);unsigned n=0;while(r<w&&n<max)out[n++]=rows[r++%Capacity];read.store(r,std::memory_order_release);return n;}
};
}
