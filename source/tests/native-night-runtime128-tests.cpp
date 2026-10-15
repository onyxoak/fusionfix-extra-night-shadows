#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <memory>
#include <vector>
#include <cassert>
#include <cstdio>
#include "NativeNightPrerequisite128.hpp"
#include "ShortlistHelperRuntime48.hpp"
#include "NativeProjectionRead66.hpp"
#include "FreshPageReadable.hpp"
#include "ImmediateRegion33.hpp"
#include "core/ShadowFloatingPointState.hpp"
uintptr_t game{},ff{},tls128{};bool nativeBackend=false;const int Layout511=511;const int* layout=&Layout511;
struct Original {std::vector<uint8_t> bytes;};std::unique_ptr<Original> original;
namespace ModelEffectObserver {struct LastErrorScope{DWORD saved=GetLastError();~LastErrorScope(){SetLastError(saved);}};}
namespace Liberty20 {unsigned reads{};}
namespace TrafficSignal {
 bool ReadableOriginal(uintptr_t at,size_t size) noexcept{return immediate_region33::Readable(at,size);}
 bool Readable(uintptr_t at,size_t size) noexcept {++Liberty20::reads;
  return traffic_signal::fresh_page::Readable(at,size,ReadableOriginal);}
}
namespace Liberty20 {
 #include "night128-copy.inc"
}
bool modulePresent128=true;
HMODULE Module128(LPCWSTR) noexcept{return modulePresent128?reinterpret_cast<HMODULE>(ff):nullptr;}
unsigned long Tls128(unsigned long offset) noexcept{assert(offset==0x2c);return static_cast<unsigned long>(tls128);}
#define GetModuleHandleW Module128
#define __readfsdword Tls128
#include "NativeNightPrerequisite128Runtime.inc"
#undef __readfsdword
#undef GetModuleHandleW
void Put128(uintptr_t at,uint32_t value){std::memcpy(reinterpret_cast<void*>(at),&value,4);}
void Jump128(uintptr_t at,uintptr_t to){auto* bytes=reinterpret_cast<uint8_t*>(at);bytes[0]=0xe9;bytes[5]=0x90;
 const auto delta=int32_t(to-at-5);std::memcpy(bytes+1,&delta,4);}
void Refresh128(native_night128::State expectedState){
 SetLastError(0x12800002);unsigned short before{},after{};__asm fnstcw before
 NativeNightPrerequisite128::Refresh();__asm fnstcw after
 assert(native_night128::currentState==expectedState&&GetLastError()==0x12800002);
 assert(before==after&&(_mm_getcsr()&0x6000u)==0x4000u);
}
int main(){
 namespace n=native_night128;namespace g=shortlist_helper48;
 constexpr size_t gameSize=0x1be6400,ffSize=0x400000;
 game=reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr,gameSize,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(game);
 ff=reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr,ffSize,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(ff);
 auto* tlsMemory=static_cast<uint8_t*>(VirtualAlloc(nullptr,8192,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(tlsMemory);tls128=reinterpret_cast<uintptr_t>(tlsMemory);
 const auto block=tls128+4096,quality=game+0x1000;
 original=std::make_unique<Original>();original->bytes.resize(n::NativeGetterRva+n::NativeGetterBytes);
 auto* expected=original->bytes.data()+n::NativeGetterRva;expected[0]=0x8b;expected[1]=0x0d;expected[6]=0x85;expected[39]=0xc3;
 std::memcpy(reinterpret_cast<void*>(game+n::NativeGetterRva),expected,n::NativeGetterBytes);
 Jump128(game+n::NativeGetterRva,game+0x2000);Jump128(game+0x2000,ff+g::FusionReadRva);
 std::memcpy(reinterpret_cast<void*>(ff+g::FusionReadRva),g::FusionReadBytes,sizeof(g::FusionReadBytes));
 for(const auto offset:g::FusionReadRelocs){auto* p=reinterpret_cast<uint8_t*>(ff+g::FusionReadRva+offset);uint32_t value{};
  std::memcpy(&value,p,4);value=uint32_t(value-0x56d00000+ff);std::memcpy(p,&value,4);}
 shortlist_helper48::runtime::binding={game,ff,gameSize,ffSize,true};
 Put128(ff+n::TlsIndexRva,0);Put128(tls128,uint32_t(block));Put128(block+0x10,uint32_t(INT32_MIN+8));
 Put128(ff+n::EpochRva,uint32_t(INT32_MIN+4));Put128(ff+n::QualityPointerRva,uint32_t(quality));Put128(quality,0);
 SetLastError(0x12800001);const auto mxcsr=_mm_getcsr();_mm_setcsr((mxcsr&~0x6000u)|0x4000u);
 NativeNightPrerequisite128::Prepare();assert(NativeNightPrerequisite128::route.authenticated);
 assert(GetLastError()==0x12800001&&(_mm_getcsr()&0x6000u)==0x4000u);
 Refresh128(n::State::Off);Put128(quality,3);Refresh128(n::State::Enabled);
 Put128(quality,99);Refresh128(n::State::Unknown);Put128(quality,1);
 Put128(ff+n::EpochRva,uint32_t(-1));Refresh128(n::State::Unknown);
 Put128(ff+n::EpochRva,uint32_t(INT32_MIN+4));Refresh128(n::State::Enabled);
 DWORD old{};assert(VirtualProtect(reinterpret_cast<void*>(quality),4096,PAGE_READWRITE|PAGE_GUARD,&old));
 Refresh128(n::State::Unknown);MEMORY_BASIC_INFORMATION info{};assert(VirtualQuery(reinterpret_cast<void*>(quality),&info,sizeof(info)));assert(info.Protect&PAGE_GUARD);
 assert(VirtualProtect(reinterpret_cast<void*>(quality),4096,PAGE_NOACCESS,&old));Refresh128(n::State::Unknown);
 assert(VirtualProtect(reinterpret_cast<void*>(quality),4096,PAGE_READWRITE,&old));Refresh128(n::State::Enabled);
 modulePresent128=false;const auto reads=Liberty20::reads;Refresh128(n::State::Unknown);assert(Liberty20::reads==reads);
 modulePresent128=true;Refresh128(n::State::Enabled);
 nativeBackend=true;NativeNightPrerequisite128::Prepare();assert(!NativeNightPrerequisite128::route.authenticated);Refresh128(n::State::Unknown);
 _mm_setcsr(mxcsr);
 std::puts("PASS128 actual Prepare/Refresh: exact getter route, live quality/TLS transitions, real guard/noaccess preservation, FP/LastError, missing module/unsupported backend unknown; no native calls or writes");
 assert(VirtualFree(tlsMemory,0,MEM_RELEASE));assert(VirtualFree(reinterpret_cast<void*>(ff),0,MEM_RELEASE));assert(VirtualFree(reinterpret_cast<void*>(game),0,MEM_RELEASE));
}
