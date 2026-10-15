#pragma once
#include "LabSirenLifetime.hpp"
namespace lab_siren {
// Private comparison metadata only. Owns no light, slot, map or cache entry.
class BlinkGrace {
 Identity owner_{}; bool bound_=false, enabled_=false, selected_=false, gap_=false;
 std::uint32_t frame_=0,tick_=0,key_=0,lastSelected_=0,temporary_=0;
 std::uint64_t temporaryGeneration_=0;
 unsigned ordinal_=7;
public:
 void Reset() noexcept {selected_=gap_=false;key_=temporary_=0;ordinal_=7;}
 void Begin(bool valid,Identity owner,std::uint32_t frame,std::uint32_t tick) noexcept {
  if(!valid||!bound_||!(owner_==owner)||frame-frame_!=1||tick-tick_>250)Reset();
  enabled_=valid;bound_=valid;owner_=owner;frame_=frame;tick_=tick;
 }
 bool Reclaim(std::uint32_t key,bool eligible,std::uint32_t other,std::uint64_t generation,unsigned claim) const noexcept {
  return enabled_&&eligible&&gap_&&key==key_&&frame_-lastSelected_==2&&
    temporary_&&other==temporary_&&generation==temporaryGeneration_&&claim==ordinal_;
 }
 unsigned Ordinal() const noexcept {return ordinal_;}
 void Finish(std::uint32_t key,bool present,int selectedSlot,std::uint32_t temporary,std::uint64_t generation,bool committed) noexcept {
  if(!enabled_||!committed){Reset();return;}
  if(present&&selectedSlot>=0&&selectedSlot<7){key_=key;ordinal_=selectedSlot;lastSelected_=frame_;selected_=true;gap_=false;temporary_=0;return;}
  if(!present&&selected_&&key==key_&&frame_-lastSelected_==1&&temporary){
   selected_=false;gap_=true;temporary_=temporary;temporaryGeneration_=generation;return;
  }
  Reset();
 }
};
}
