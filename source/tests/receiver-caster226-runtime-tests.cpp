#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>
#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "core/ShadowFloatingPointState.hpp"
#include "core/ReceiverCasterCull226.hpp"
namespace rage {struct grcViewport{};}
namespace standalone {
uintptr_t game{};std::atomic<bool> addonActive{true};
struct Image{std::vector<uint8_t> bytes;};std::unique_ptr<Image> original;
std::vector<uint8_t> ReadMemory(uintptr_t p,size_t n){return {reinterpret_cast<uint8_t*>(p),reinterpret_cast<uint8_t*>(p+n)};}
void Require(bool x,const char* m){if(!x)throw std::runtime_error(m);}
void Event(int,const char*,const std::string&){}
namespace CShadows {uint32_t frame=123;uint32_t* pFrameCounter=&frame;}
namespace WorldGeometry20 {std::atomic<bool> active{true};}
namespace PlayerShadowAllocation {
std::atomic<uint32_t> ownerThread{};fusionfix::shadows::ShadowView view;
uintptr_t expectedView{};
fusionfix::shadows::ShadowView ReadView(const rage::grcViewport* p){assert(uintptr_t(p)==expectedView);return view;}
}
namespace Liberty20 {
unsigned reads{};uintptr_t mutateAddress{};unsigned mutateOn{},mutateReads{};
bool CopyLocal(uintptr_t at,void* out,size_t n)noexcept{
 ++reads;
 __try{
  if(at==mutateAddress&&++mutateReads==mutateOn)*reinterpret_cast<float*>(at)=float(200);
  memcpy(out,reinterpret_cast<void*>(at),n);return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
}
bool(*receiverCull226)(uintptr_t,uint32_t)noexcept=nullptr;
#include "ReceiverCull226Runtime.inc"
}
template<class T>void Put(uintptr_t p,T v){memcpy(reinterpret_cast<void*>(p),&v,sizeof(v));}
int main(){
 using namespace standalone;using namespace ReceiverCull226;
 game=uintptr_t(VirtualAlloc(nullptr,0x1400000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(game);
 auto alloc=[](){auto p=uintptr_t(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(p);return p;};
 const auto object=alloc(),entity=alloc(),matrix=alloc(),info=alloc();
 PlayerShadowAllocation::expectedView=object+0x4a0;
 PlayerShadowAllocation::ownerThread=GetCurrentThreadId();auto& view=PlayerShadowAllocation::view;view.valid=true;
 for(unsigned i=0;i<4;++i)view.view[i][i]=1;
 view.projection[0][0]=view.projection[1][1]=1;view.projection[2][2]=100.f/99.f;view.projection[2][3]=1;view.projection[3][2]=-100.f/99.f;
 Put(object+0x940,uint32_t(1));const auto map=0x110u;
 Put(game+0xd9f1ed+map,uint8_t(1));Put(game+0xd9f1fc+map,uint32_t(4));Put(game+0xd9f1f8+map,uint32_t(55));
 Put(game+0xd9f1c8+map,float(10));Put(entity,uint32_t(game+0xaaf554));Put(entity+0x20,uint32_t(matrix));Put(entity+0x28,uint32_t(1u<<6));Put(entity+0x2e,int16_t(5));
 Put(game+0xaaf554+0x5c,uint32_t(game+0x6314e0));Put(game+0xe95cd8+5*4,uint32_t(info));
 const std::array<float,8> bounds{-1,-1,-1,0,1,1,1,0};memcpy(reinterpret_cast<void*>(info+0x20),bounds.data(),sizeof(bounds));
 std::array<float,16> pose{};for(unsigned i=0;i<4;++i)pose[5*i]=1;pose[12]=100;pose[14]=10;
 memcpy(reinterpret_cast<void*>(matrix),pose.data(),sizeof(pose));
 requested242=true;prepared=false;EnableConfigured242();assert(!mode.load()&&!tightHull241.load());
 prepared=true;EnableConfigured242();assert(mode.load()&&tightHull241.load());
 requested242=false;tightHull241=false;mode=false;EnableConfigured242();assert(!mode.load()&&!tightHull241.load());
 prepared=experimental=true;assert(LibertySetReceiverCull226(FALSE));
 {Scope s(reinterpret_cast<void*>(object));assert(!binding.valid&&!Visit(1,1));}
 assert(LibertySetReceiverCull226(TRUE));
 const auto fp=_mm_getcsr();SetLastError(226);
 {
  Scope s(reinterpret_cast<void*>(object));assert(binding.valid);
  assert(Visit(entity,1));
  // The observed regular CBuilding table differs from the first fixture. Its
  // exact native getter, not one vtable address, supplies the bounds contract.
  Put(entity,uint32_t(game+0xa9a56c));Put(game+0xa9a56c+0x5c,uint32_t(game+0x6314e0));assert(Visit(entity,1));
  Put(game+0xa9a56c+0x5c,uint32_t(game+0x711140));assert(!Visit(entity,1));
  Put(entity,uint32_t(game+0xaaf554));
  Put(matrix+3*4,float(41));Put(matrix+7*4,float(42));Put(matrix+11*4,float(43));Put(matrix+15*4,float(44));assert(Visit(entity,1));
  assert(!Visit(1,2));assert(!Visit(1,4));assert(!Visit(1,1));
  Put(matrix+12*4,float(0));assert(!Visit(entity,1));Put(matrix+12*4,float(100));
  CShadows::frame++;assert(!Visit(entity,1));CShadows::frame--;
  Put(entity+0x20,uint32_t(0));Put(entity+0x10,float(100));Put(entity+0x18,float(10));assert(Visit(entity,1));Put(entity+0x20,uint32_t(matrix));
  Put(entity,uint32_t(0x10000));assert(!Visit(entity,1));Put(entity,uint32_t(game+0xaaf554));
  Put(info+0x20,NAN);assert(!Visit(entity,1));Put(info+0x20,float(-1));
  Liberty20::mutateAddress=matrix;Liberty20::mutateOn=2;Liberty20::mutateReads=0;assert(!Visit(entity,1));Liberty20::mutateAddress=0;
 }
 assert(!binding.valid&&GetLastError()==226&&_mm_getcsr()==fp);
 Put(object+0x940,uint32_t(0));{Scope s(reinterpret_cast<void*>(object));assert(!binding.valid);}Put(object+0x940,uint32_t(1));
 Put(game+0xd9f1fc+map,uint32_t(3));{Scope s(reinterpret_cast<void*>(object));assert(!binding.valid);}Put(game+0xd9f1fc+map,uint32_t(4));
 Put(game+0x1193312,uint8_t(1));{Scope s(reinterpret_cast<void*>(object));assert(!binding.valid);}Put(game+0x1193312,uint8_t(0));
 PlayerShadowAllocation::ownerThread=0;assert(!LibertySetReceiverCull226(FALSE));{Scope s(reinterpret_cast<void*>(object));assert(binding.valid);}
 PlayerShadowAllocation::ownerThread=GetCurrentThreadId();assert(LibertySetReceiverCull226(FALSE));assert(!mode.load());
 //229 physical branch: actual production reader, native A0 source priority,
 // all physical entity classes and final recapture before a rejected draw.
 const auto physical=alloc(),link=alloc(),bound=alloc();
 Put(entity,uint32_t(game+0xaaf554));Put(game+0xaaf554+0x5c,uint32_t(game+0x711140));
 Put(entity+0x38,uint32_t(physical));Put(physical+4,uint32_t(link));Put(link+0xc,uint32_t(bound));
 Put(bound+8,float(2));Put(entity+0xfc,float(0));
 pose={};for(unsigned i=0;i<4;++i)pose[5*i]=1;pose[12]=200;pose[14]=10;memcpy(reinterpret_cast<void*>(matrix),pose.data(),64);
 const std::array<uint8_t,16> physicalCode{0xf7,0x41,0x24,0,0,0,4,0x74,4,0x8b,0x41,0x38,0xc3,0x33,0xc0,0xc3};
 memcpy(reinterpret_cast<void*>(game+0x60d320),physicalCode.data(),physicalCode.size());
 nativeBounds229=true;assert(LibertySetReceiverCull226(TRUE));
 {Scope s(reinterpret_cast<void*>(object));assert(binding.valid);
  for(unsigned type:{1u,2u,3u,4u}){
   Put(entity+0x28,uint32_t(type<<6));
   const auto method=type==2?0x649190u:type==3?0x1d4fc0u:0x60d320u;
   Put(game+0xaaf554+0xa0,uint32_t(game+method));
   if(type==2||type==3){const auto offset=type==2?0xdc4u:0x7b4u;const std::array<uint8_t,7> code{0x8b,0x81,uint8_t(offset),uint8_t(offset>>8),0,0,0xc3};memcpy(reinterpret_cast<void*>(game+method),code.data(),code.size());Put(entity+offset,uint32_t(physical));}
   assert(Visit(entity,1));
   Put(matrix+12*4,float(0));assert(!Visit(entity,1));Put(matrix+12*4,float(200));
   Put(game+0xaaf554+0xa0,uint32_t(game+0x1234));assert(!Visit(entity,1));Put(game+0xaaf554+0xa0,uint32_t(game+method));
  }
  Put(bound+8,NAN);assert(!Visit(entity,1));Put(bound+8,float(2));
  Liberty20::mutateAddress=bound;Liberty20::mutateOn=2;Liberty20::mutateReads=0;assert(!Visit(entity,1));Liberty20::mutateAddress=0;
 }
 nativeBounds229=false;assert(LibertySetReceiverCull226(FALSE));
 for(auto p:{physical,link,bound})assert(VirtualFree(reinterpret_cast<void*>(p),0,MEM_RELEASE));
 for(auto p:{object,entity,matrix,info,game})assert(VirtualFree(reinterpret_cast<void*>(p),0,MEM_RELEASE));
 std::cout<<"PASS226/229 actual production scope/reader/callback: native map/type/frame/thread guards, mode OFF, all physical classes, source priority, bad pointers/bounds/getters, changing poses/bounds, zero-matrix enclosure, scoped state, FP and last-error preservation; not a game performance proof\n";
}
