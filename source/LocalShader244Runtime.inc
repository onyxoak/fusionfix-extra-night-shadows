#include "LocalShaderBinding244.hpp"
namespace LocalShader244 {
bool Discover(uint32_t& epoch,uint32_t& group) noexcept {
 try {
  if(!ff||!FusionBackend130::prepared||reinterpret_cast<uintptr_t>(FusionBackend130::pinnedModule)!=ff||
     reinterpret_cast<uintptr_t>(pe::ModuleAt(ff))!=ff)return false;
  const pe::Image image(pe::Read(pe::ModulePath(FusionBackend130::pinnedModule)),ff);
  std::vector<local_shader244::Region> regions;
  for(const auto& section:image.sections)regions.push_back({section.VirtualAddress,section.Misc.VirtualSize,section.Characteristics});
  const auto binding=local_shader244::Discover(image.bytes,uint32_t(ff),regions);
  if(!binding||ReadMemory(ff+binding.registration,21)!=std::vector<uint8_t>(image.bytes.begin()+binding.registration,image.bytes.begin()+binding.registration+21)||
     ReadMemory(ff+binding.name,9)!=std::vector<uint8_t>(image.bytes.begin()+binding.name,image.bytes.begin()+binding.name+9))return false;
  epoch=binding.epoch;group=binding.group;
  Event(1,"LOCAL_SHADER244_BOUND","registered_wd_local=1 version_gate=0 epoch_rva="+std::to_string(epoch)+" group_rva="+std::to_string(group));
  return true;
 }catch(...){return false;}
}
}
