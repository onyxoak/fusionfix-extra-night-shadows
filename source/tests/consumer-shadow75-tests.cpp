#define NOMINMAX
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cstring>
#include <vector>
#include "core/ConsumerShadow75.hpp"
#include "core/ShadowFloatingPointState.hpp"
#include "safetyhook/context.hpp"
using SafetyHookContext=safetyhook::Context32;
namespace ModelEffectObserver {struct LastErrorScope {DWORD value=GetLastError();~LastErrorScope(){SetLastError(value);}};}
namespace CaptureDiagnostic44 {std::atomic<bool> sampleSources{false};}
namespace CShadows {uint32_t frame=10;uint32_t* pFrameCounter=&frame;}
namespace ShadowTrace34 {
struct Event {uint32_t stage,frame,tick,key;int32_t a,b,c,d;float x,y,z,w;};
std::atomic<bool> enabled{false};uint32_t watched=123;std::vector<Event> events;
bool Tracked(uint32_t key) noexcept {return enabled.load()&&key==watched;}
void Emit(Event e) noexcept {events.push_back(e);}
}
std::atomic<bool> addonActive{true};uintptr_t game{};
namespace Liberty20 {
unsigned reads{};
__declspec(noinline) bool Contained(uintptr_t p,void* out,size_t n) noexcept {
 __try {std::memcpy(out,reinterpret_cast<const void*>(p),n);return true;}
 __except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION||GetExceptionCode()==EXCEPTION_GUARD_PAGE?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){return false;}
}
bool Copy(uintptr_t p,void* out,size_t n) noexcept {
 ++reads;SetLastError(999);
 if(p<0x10000 || n>UINTPTR_MAX-p)return false;
 for(auto at=p;at<p+n;){MEMORY_BASIC_INFORMATION m{};if(!VirtualQuery(reinterpret_cast<void*>(at),&m,sizeof(m))||m.State!=MEM_COMMIT||
   (m.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return false;
  auto end=reinterpret_cast<uintptr_t>(m.BaseAddress)+m.RegionSize;if(end<=at)return false;at=end;
 }return Contained(p,out,n);
}
}
namespace ConsumerShadow75 {
std::atomic<bool> active{false};thread_local consumer_shadow75::Context context;thread_local bool busy=false;
consumer_shadow75::FrameBudget budget,unkeyedBudget;std::atomic<uint32_t> completed{},refused{},limited{},sizeRejected{};
#include "consumer75-callbacks-extracted.inc"
}
static std::array<uint32_t,30> packet{};
static std::array<uint8_t,0x140> stack{};
static uint8_t* StackBase(){return stack.data()+0x20;}
static std::array<uint8_t,0x300> camera{};
static float scale=0.5f;
template<class T> void Put(void* base,size_t offset,const T& value){std::memcpy(static_cast<uint8_t*>(base)+offset,&value,sizeof(value));}
static SafetyHookContext LookupRegs(){SafetyHookContext r{};r.esp=reinterpret_cast<uintptr_t>(StackBase())-12;r.edi=reinterpret_cast<uintptr_t>(packet.data())+0x28;r.eax=4;return r;}
static SafetyHookContext GateRegs(){SafetyHookContext r{};r.esp=reinterpret_cast<uintptr_t>(StackBase());r.xmm0.f32[0]=0.5f;r.xmm1.f32[0]=0.25f;return r;}
static SafetyHookContext DispatchRegs(){SafetyHookContext r{};r.esp=reinterpret_cast<uintptr_t>(StackBase())-20;r.edi=0;r.esi=12;return r;}
static void Init(){
 packet.fill(0);packet[17]=0;packet[18]=0x55;packet[24]=123;float radius=18;
 Put(packet.data(),21*4,radius);std::array<float,3> pos{1,2,3};Put(packet.data(),8*4,pos);
 stack.fill(0);camera.fill(0);uint32_t source=uint32_t(reinterpret_cast<uintptr_t>(packet.data())+0x28),cam=uint32_t(reinterpret_cast<uintptr_t>(camera.data()));
 Put(StackBase(),0x1c,source);Put(StackBase(),0x28,cam);stack[0x20+0xd]=1;stack[0x20+0xe]=0;
 Put(StackBase(),0x14,radius);Put(StackBase(),0x40,pos);std::array<float,3> cameraPos{73,2,3};Put(camera.data(),0x70,cameraPos);float cameraScale=1;Put(camera.data(),0x2cc,cameraScale);
 game=reinterpret_cast<uintptr_t>(&scale)-0xc3efdc;ConsumerShadow75::context={};ShadowTrace34::events.clear();Liberty20::reads=0;
}
static void Run(){auto l=LookupRegs(),g=GateRegs(),d=DispatchRegs();ConsumerShadow75::Lookup(l);ConsumerShadow75::Gate(g);uint32_t mode=0;Put(reinterpret_cast<void*>(d.esp),0,mode);ConsumerShadow75::Dispatch(d);}
int main(){
 using namespace ConsumerShadow75;Init();auto l=LookupRegs();auto unchanged=l;
 SetLastError(0x12345678);auto oldMxcsr=_mm_getcsr();_mm_setcsr(0x3f80);
 alignas(16) uint8_t before[512]{},after[512]{};
 __asm fninit
 __asm fld1
 _fxsave(before);Lookup(l);_fxsave(after);
 assert(GetLastError()==0x12345678&&Liberty20::reads==0&&std::memcmp(&l,&unchanged,sizeof(l))==0);
 assert(std::memcmp(before,after,160)==0);_mm_setcsr(oldMxcsr);__asm {fstp st(0)}
 active=true;ShadowTrace34::enabled=true;Lookup(l);assert(Liberty20::reads==0); // lease OFF
 CaptureDiagnostic44::sampleSources=true;Init();auto bank=packet;auto cam=camera;auto st=stack;
 Run();assert(completed==1&&ShadowTrace34::events.size()==6&&sizeRejected==1);
 assert(packet==bank&&camera==cam&&stack==st&&ShadowTrace34::events[0].a==4&&ShadowTrace34::events[0].d==0);
 assert(ShadowTrace34::events[3].x==72&&ShadowTrace34::events[3].y==0.25f&&ShadowTrace34::events[3].z==0.5f);
 assert(ShadowTrace34::events[3].b==1); // actual SETBE matches reproduced finite predicate
 Init();l=LookupRegs();Lookup(l);auto g=GateRegs();g.esp+=4;Gate(g);assert(!context.pending&&ShadowTrace34::events.empty());
 Init();l=LookupRegs();Lookup(l);++CShadows::frame;g=GateRegs();Gate(g);assert(!context.pending&&ShadowTrace34::events.empty());
 Init();l=LookupRegs();Lookup(l);packet[15]^=1;g=GateRegs();Gate(g);assert(!context.pending); // full packet witness includes W
 Init();l=LookupRegs();Lookup(l);stack[0x20+0xd]=2;g=GateRegs();Gate(g);assert(!context.pending);
 Init();l=LookupRegs();auto beforeReads=Liberty20::reads;busy=true;Lookup(l);busy=false;assert(Liberty20::reads==beforeReads);
 Init();packet[24]=999;l=LookupRegs();Lookup(l);assert(!context.pending);
 Init();packet[17]=1;l=LookupRegs();Lookup(l);assert(!context.pending);
 Init();packet[24]=0;packet[18]=0x51;stack[0x20+0xd]=0;Run();assert(ShadowTrace34::events.size()==6 && ShadowTrace34::events[0].key==0 && ShadowTrace34::events[0].c==0x51 && ShadowTrace34::events[3].a==0);
 Init();packet[24]=UINT32_MAX;l=LookupRegs();Lookup(l);assert(!context.pending);
 Init();l=LookupRegs();l.esp=UINT32_MAX-12;Lookup(l);assert(!context.pending);
 Init();CShadows::pFrameCounter=nullptr;l=LookupRegs();Lookup(l);assert(Liberty20::reads==0);CShadows::pFrameCounter=&CShadows::frame;
 Init();l=LookupRegs();Lookup(l);auto* guarded=VirtualAlloc(nullptr,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);assert(guarded);
 DWORD prior{};assert(VirtualProtect(guarded,4096,PAGE_READWRITE|PAGE_GUARD,&prior));uint32_t ptr=uint32_t(reinterpret_cast<uintptr_t>(guarded));Put(StackBase(),0x28,ptr);g=GateRegs();Gate(g);assert(!context.pending);
 MEMORY_BASIC_INFORMATION m{};assert(VirtualQuery(guarded,&m,sizeof(m))&&m.Protect&PAGE_GUARD);assert(VirtualFree(guarded,0,MEM_RELEASE));
 consumer_shadow75::FrameBudget b;for(unsigned i=0;i<b.Maximum;++i)assert(b.Claim(10));assert(!b.Claim(10));assert(!b.Claim(9));assert(b.Claim(11));
 consumer_shadow75::Sample s{};s.available=1;s.camera={3,4,0};s.gateSource={0,0,0};s.radius=s.gateRadius=5;s.cameraScale=1;s.globalScale=1;s.threshold=1;s.ratio=1;s.sizeGate=1;consumer_shadow75::Calculate(s);assert(s.reproduced&&s.recomputedRatio==1&&s.distance==5);
 s.sizeGate=0;consumer_shadow75::Calculate(s);assert(!s.reproduced);s.ratio=NAN;consumer_shadow75::Calculate(s);assert(!s.finite);
 return 0;
}
