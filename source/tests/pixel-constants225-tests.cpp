#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "standalone-pe.hpp"
#include "PixelConstants225.hpp"
#include "GpuProfiling39.hpp"
#include "FreshPageReadable.hpp"
#include <cassert>
#include <iostream>
#include <d3d9.h>
using namespace onyxoak;
uintptr_t game=0x400000;
constexpr uint32_t facade225=0x100000,table225=0x200000;
std::array<uint8_t,187> live225;
uint32_t livePointer225=facade225,method225{};
bool current225=true,slots225=true;
unsigned slotChecks225{};
namespace Liberty20 {
bool Copy(uintptr_t at,void* out,size_t n)noexcept{
 uint32_t value{};
 if(at==game+0x13ed8d8&&n==4)value=livePointer225;
 else if(at==facade225&&n==4)value=table225;
 else if(at==table225+109*4&&n==4)value=method225;
 else if(at==game+0x225b0&&n==live225.size()){std::memcpy(out,live225.data(),n);return true;}
 else return false;
 std::memcpy(out,&value,4);return true;
}
}
namespace GpuProfiling39 {
bool Current(IDirect3DDevice9* d)noexcept{return current225&&uintptr_t(d)==facade225;}
bool DeviceSlots(IDirect3DDevice9*,IDirect3DDevice9*& p)noexcept{++slotChecks225;p=nullptr;return slots225;}
}
namespace VehicleBoxes216 {
template<class T>bool Copy(uintptr_t at,T& out)noexcept{return Liberty20::Copy(at,&out,sizeof(out));}
#include "fallback-device225.inc"
}
int main(int argc,char** argv){
 assert(argc==2);const auto raw=pe::Read(argv[1]);assert(raw.size()==187);unsigned mutations{};
 for(const auto base:{0x400000u,0x600000u,0x28000000u}){
  const std::span<const uint8_t> native(raw);
  assert(pixel_constants225::Cached(native));
  assert(!gpu39::Forwarder(native.first(22),109*4)); // Reproduces216/224 refusal on the saved live CE wrapper.
  game=base;method225=uint32_t(game+0x225b0);std::copy(native.begin(),native.end(),live225.begin());
  assert(uintptr_t(VehicleBoxes216::Device())==facade225);
  for(size_t i=0;i<live225.size();++i)for(unsigned bit=0;bit<8;++bit){
   live225[i]^=uint8_t(1u<<bit);assert(!pixel_constants225::Cached(live225));
   assert(!VehicleBoxes216::Device());live225[i]^=uint8_t(1u<<bit);++mutations;
  }
  current225=false;assert(!VehicleBoxes216::Device());current225=true;
  slots225=false;assert(!VehicleBoxes216::Device());slots225=true;
  ++method225;assert(!VehicleBoxes216::Device());--method225;
  livePointer225=0;assert(!VehicleBoxes216::Device());livePointer225=facade225;
  assert(uintptr_t(VehicleBoxes216::Device())==facade225);
 }
 assert(slotChecks225>0);
 std::cout<<"PASS saved live CE slot109 wrapper at3 simulated bases; original forwarder guard reproduced; "<<mutations<<" code mutations refused; real Device() current facade/proxy gates retained. Early disk code is Steam-protected: this fixture is live evidence, not an independent pristine disk-body proof. Not game pixels or FPS proof.\n";
}
