#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <float.h>
#include <array>
#include <map>
#include <vector>
#include <atomic>
#include <cassert>
#include <cstring>
#include <iostream>
#include "core/PresentFrameTime151.hpp"
#include "ShadowFloatingPointState.hpp"
#include "FreshPageReadable.hpp"
#include "ImmediateRegion33.hpp"
uint32_t game=0x400000;
namespace CShadows {uint32_t frame=17;uint32_t* pFrameCounter=&frame;}
namespace fixture151 {
unsigned reads{},nativeCalls{};uint64_t fail{};
std::map<uint64_t,std::vector<uint8_t>> memory;
template<class T>void Put(uint64_t at,T value){memory[at].resize(sizeof(T));std::memcpy(memory[at].data(),&value,sizeof(T));}
void Reset(){reads=0;fail=0;memory.clear();Put(game+0x13ed8d8,uint32_t(0x20000000));Put(0x20000000,uint32_t(0x30000000));Put(0x30000000+17*4,uint32_t(game+0x212f0));Put(0x200011ac,uint32_t(0x40000000));}
bool Copy(uintptr_t at,void* out,size_t n){++reads;SetLastError(999);_mm_setcsr(0x7f80);if(at==fail)return false;
 if(at==uintptr_t(CShadows::pFrameCounter)&&n==4){std::memcpy(out,CShadows::pFrameCounter,4);return true;}
 auto i=memory.find(at);if(i==memory.end()||i->second.size()!=n)return false;std::memcpy(out,i->second.data(),n);return true;}
DWORD inError=73,outError=314;unsigned inCsr=0x3f80,outCsr=0x5f80;
const RECT* wantedA{};const RECT* wantedB{};HWND wantedWindow{};const RGNDATA* wantedDirty{};
HRESULT STDMETHODCALLTYPE Native(IDirect3DDevice9* d,const RECT* a,const RECT* b,HWND w,const RGNDATA* dirty){
 ++nativeCalls;assert(d==reinterpret_cast<IDirect3DDevice9*>(0x20000000));assert(a==wantedA&&b==wantedB&&w==wantedWindow&&dirty==wantedDirty);
 assert(GetLastError()==inError&&_mm_getcsr()==inCsr);unsigned control{};_controlfp_s(&control,0,0);assert((control&_MCW_RC)==_RC_DOWN);
 _controlfp_s(&control,_RC_UP,_MCW_RC);_mm_setcsr(outCsr);SetLastError(outError);return 37;
}
void Input(){unsigned control{};_controlfp_s(&control,_RC_DOWN,_MCW_RC);_mm_setcsr(inCsr);SetLastError(inError);}
void Output(){assert(GetLastError()==outError&&_mm_getcsr()==outCsr);unsigned control{};_controlfp_s(&control,0,0);assert((control&_MCW_RC)==_RC_UP);}
}
namespace Liberty20 {bool Copy(uintptr_t at,void* out,size_t n)noexcept{return fixture151::Copy(at,out,n);}}
namespace FrameTime151 {
using NativePresent=HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,const RECT*,const RECT*,HWND,const RGNDATA*);
present_time151::State<> state;std::atomic<bool> enabled{false};
#include "present151-extracted.inc"
}
int main(){
 assert(!present_time151::PublicationCapReached153(0)&&!present_time151::PublicationCapReached153(65535));
 assert(present_time151::PublicationCapReached153(65536)&&present_time151::PublicationCapReached153(69631)&&present_time151::PublicationCapReached153(UINT64_MAX));
 using namespace fixture151;using namespace present_time151;
 RECT a{},b{};wantedA=&a;wantedB=&b;wantedWindow=reinterpret_cast<HWND>(123);wantedDirty=reinterpret_cast<const RGNDATA*>(456);
 auto d=reinterpret_cast<IDirect3DDevice9*>(0x20000000);LARGE_INTEGER f{};assert(QueryPerformanceFrequency(&f));FrameTime151::state.Configure(uint64_t(f.QuadPart));
 Reset();Input();assert(FrameTime151::Call(Native,d,&a,&b,wantedWindow,wantedDirty)==37);Output();assert(reads==0&&nativeCalls==1);
 FrameTime151::enabled=true;Input();assert(FrameTime151::Call(Native,d,&a,&b,wantedWindow,wantedDirty)==37);Output();assert(reads==5&&nativeCalls==2);
 std::array<Record,8> rows{};assert(FrameTime151::state.Drain(rows.data(),8)==1&&rows[0].token.valid&&rows[0].token.frame==17&&rows[0].result==37&&rows[0].nativeEnd>=rows[0].token.nativeStart);
 Reset();fail=game+0x13ed8d8;Input();assert(FrameTime151::Call(Native,d,&a,&b,wantedWindow,wantedDirty)==37);Output();assert(nativeCalls==3&&FrameTime151::state.Drain(rows.data(),8)==0);
 // Numeric state tests: discontinuities/refusals/overrun/generation do not
 // turn these CPU/API timestamps into GPU or display-completion measurements.
 State<2> s;s.Configure(1000);auto t=s.Begin(7,1,2,8,100);assert(t.valid&&(t.flags&First));t.nativeStart=102;s.End(t,110,0);
 t=s.Begin(7,1,2,9,120);assert(t.interval==20);t.nativeStart=121;assert(!s.Begin(7,1,2,9,125).valid&&s.nestedRefusals==1);assert(!s.Begin(8,1,2,9,125).valid&&s.threadRefusals==1);s.End(t,125,0);
 t=s.Begin(7,1,2,10,119);assert(t.flags&ClockDiscontinuity);t.nativeStart=120;s.End(t,121,0);assert(s.drops==1);assert(s.Drain(rows.data(),8)==2);
 t=s.Begin(7,1,3,11,130);assert((t.flags&EpochChanged)&&t.epoch==2&&!t.interval);t.nativeStart=132;s.End(t,131,0);assert(s.Drain(rows.data(),8)==1&&(rows[0].token.flags&NativeClockInvalid));
 t=s.Begin(7,1,3,12,140);t.nativeStart=140;s.End(t,141,int32_t(D3DERR_DEVICELOST));assert(s.Drain(rows.data(),8)==1&&(rows[0].token.flags&LostIndication));
 t=s.Begin(7,1,3,13,150);t.nativeStart=150;s.End(t,151,0);assert(s.Drain(rows.data(),8)==1&&(rows[0].token.flags&RecoveryIndication));
 s.ObserveReset(8);t=s.Begin(7,1,3,14,160);assert(!(t.flags&ExplicitReset));t.nativeStart=160;s.End(t,161,0);s.Drain(rows.data(),8);
 s.ObserveReset(7);t=s.Begin(7,1,3,15,170);assert(t.flags&ExplicitReset);t.nativeStart=170;s.End(t,171,0);s.Drain(rows.data(),8);
 assert(!s.Begin(7,0,3,16,180).valid&&s.identityRefusals==1);assert(!s.Begin(7,1,3,16,0).valid&&s.clockRefusals==1);
 t=s.Begin(7,1,3,17,11000);assert(t.flags&LongGap);t.nativeStart=11000;s.End(t,11001,0);
 std::cout<<"PASS151 actual production Ops/Call: disabled0reads; enabled5identity/frame reads; nativeargs/result and caller/calleeLastError/MXCSR/x87 preserved; epoch/thread/nesting/discontinuity/overrun/lost/recovery/reset policies\n";
}

