#pragma once
#include <bit>
#include <cmath>
#include <cstdint>
namespace traffic_signal {
// Called ONLY by the existing authenticated R/G/Y native producer, after its
// fresh owner/lifetime/lease checks. Registry admission is one-per-frame and
// requires original flags200/key0, so neither repeated callbacks nor queued
// private packets can compound this spill-only gain.
template<class Registry,class Identity>
bool ApplySignalSpill164(Registry& registry,uint32_t index,Identity current,
 uint32_t frame,uint32_t caller,uint32_t phase,bool collision,uint32_t* args,
 const float* rgb) noexcept {
 if(!registry.Apply(index,current,frame,caller,args[2],phase,collision,args[3],args[16]))return false;
 const float intensity=std::bit_cast<float>(args[8]);
 if(!rgb||!std::isfinite(intensity)||intensity<=0)return true;
 bool emission=false;
 for(unsigned i=0;i<3;++i){if(!std::isfinite(rgb[i])||rgb[i]<0)return true;emission=emission||rgb[i]>0;}
 const float adjusted=intensity*1.05f;
 if(emission&&std::isfinite(adjusted)&&adjusted>0)args[8]=std::bit_cast<uint32_t>(adjusted);
 return true;
}
}
