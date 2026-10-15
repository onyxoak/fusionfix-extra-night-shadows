#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
#include "core/ShadowFloatingPointState.hpp"
#include "FreshPageReadable.hpp"
#include "core/ShadowReceiver.hpp"
#include "core/PoleCasterSnapshot134.hpp"
unsigned regionQueries160=0,workingQueries160=0,fullReads160=0,events160=0,mutationReads160=0;uintptr_t mutation160=0;
SIZE_T WINAPI RegionQuery160(LPCVOID at,PMEMORY_BASIC_INFORMATION out,SIZE_T n){++regionQueries160;return VirtualQuery(at,out,n);}
#define VirtualQuery RegionQuery160
#include "ImmediateRegion33.hpp"
#undef VirtualQuery
BOOL WINAPI WorkingQuery160(HANDLE h,PVOID out,DWORD n){++workingQueries160;return QueryWorkingSetEx(h,out,n);}
static bool Region160(uintptr_t p,size_t n)noexcept{return immediate_region33::Readable(p,n);}
static __declspec(noinline) bool Copy160(uintptr_t p,void* out,size_t n)noexcept{__try{std::memcpy(out,reinterpret_cast<void*>(p),n);return true;}__except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION||GetExceptionCode()==EXCEPTION_GUARD_PAGE?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){return false;}}
namespace standalone {
uint32_t game=0;uintptr_t ff=0x50000000;std::atomic<bool> addonActive{true};
namespace PlayerShadowAllocation{std::atomic<DWORD> ownerThread{GetCurrentThreadId()};}
namespace CShadows{uint32_t* pFrameCounter=nullptr;}
namespace FusionBackend130{bool prepared=true;HMODULE pinnedModule=reinterpret_cast<HMODULE>(ff);struct Lease{bool Owns()const noexcept{return true;}}lease;}
namespace Liberty20{bool CopyLocal(uintptr_t at,void* out,size_t n)noexcept{return Copy160(at,out,n);}bool Copy(uintptr_t at,void* out,size_t n)noexcept{++fullReads160;const bool ok=traffic_signal::fresh_page::Readable(at,n,Region160,WorkingQuery160)&&Copy160(at,out,n);if(ok&&mutation160>=at&&mutation160<at+n&&++mutationReads160==2)reinterpret_cast<uint8_t*>(out)[mutation160-at]^=1;return ok;}}
namespace ShadowTrace34{struct Event{uint32_t stage,frame,tick,key;int32_t a,b,c,d;float x,y,z,w;};void Emit(Event)noexcept{++events160;}}
void Event(int,const char*,const char*)noexcept{}
HMODULE Module160(LPCWSTR)noexcept{return FusionBackend130::pinnedModule;}
#define GetModuleHandleW Module160
namespace native_projection66{struct Permission142{bool allowed=true;};inline Permission142 Preflight(uintptr_t,size_t)noexcept{return{};}}
#define LIBERTY_POLE_TEST_PREFLIGHT142
#include "PoleStaticPartition141Runtime.inc"
namespace PoleStaticPartition141{
#include "reclassify158-body.inc"
}
#undef GetModuleHandleW
}
using namespace standalone;
template<class T>void Put160(uintptr_t p,T v){std::memcpy(reinterpret_cast<void*>(p),&v,sizeof(v));}
uint32_t entity160=0,info160=0,stack160=0,pool160=0,flags160=0;
void Init160(){DWORD prev{};assert(VirtualProtect(reinterpret_cast<void*>(entity160),4096,PAGE_READWRITE,&prev));assert(VirtualProtect(reinterpret_cast<void*>(info160),4096,PAGE_READWRITE,&prev));std::memset(reinterpret_cast<void*>(game),0,0x1240000);const uint32_t pose=game+0x9000,bank=game+0x11000;
Put160(game+0x1232c60,pool160);Put160(pool160,entity160);Put160(pool160+4,flags160);Put160(pool160+8,int32_t(1));Put160(pool160+12,int32_t(0x320));Put160(flags160,uint8_t(7));Put160(entity160,game+0xaab0dc);Put160(entity160+0x20,pose);Put160(entity160+0x24,uint32_t(0x4000000));Put160(entity160+0x28,uint32_t(4<<6));Put160(entity160+0x2e,int16_t(1905));Put160(entity160+0x34,uint32_t(3));Put160(entity160+0x38,uint32_t(4));Put160(entity160+0x210,uint32_t(0x2000));Put160(game+0xe95cd8+1905*4,info160);Put160(info160+8,uint32_t(5));Put160(info160+0x3c,pole_caster134::PoleHash);Put160(game+0xaab0dc+0x28,game+0x16d0);Put160(game+0x16d0,uint8_t(0x32));Put160(game+0x16d1,uint8_t(0xc0));Put160(game+0x16d2,uint8_t(0xc3));for(unsigned i=0;i<4;++i)Put160(pose+i*20,float(1));Put160(game+0xc3eed8,bank);Put160(game+0x110e240,uint32_t(1));Put160(bank+0x54,float(20));Put160(bank+0x40,float(1));Put160(bank+0x30,float(1));Put160(bank+0x60,uint32_t(7654));Put160(game+0xd9f1ed+0x110,uint8_t(1));Put160(game+0xd9f1ed+0x110+11,uint32_t(7654));Put160(game+0xd9f1ed+0x110+15,uint32_t(4));CShadows::pFrameCounter=reinterpret_cast<uint32_t*>(game+0x13000);*CShadows::pFrameCounter=20;
FusionBackend130::prepared=true;FusionBackend130::pinnedModule=reinterpret_cast<HMODULE>(ff);PoleStaticPartition141::requested=true;PoleStaticPartition141::prepared=true;PoleStaticPartition141::binding={7654,1,4,20,GetCurrentThreadId(),true,{}};auto& volume=PoleStaticPartition141::binding.volume;volume={bank,1,0,{},true};std::memcpy(volume.packet.data(),reinterpret_cast<void*>(bank),128);regionQueries160=workingQueries160=fullReads160=events160=mutationReads160=0;mutation160=0;}
struct Result160{bool accepted;uint32_t eligibility;unsigned events,reads,working,regions;};
Result160 Run160(bool old,unsigned scenario){Init160();DWORD previous{};if(scenario==1)Put160(info160+0x3c,uint32_t(123));if(scenario==2)Put160(entity160+0x2e,int16_t(-1));if(scenario==3)Put160(entity160+0x2e,int16_t(31000));if(scenario==4)Put160(flags160,uint8_t(0x80));if(scenario==5)Put160(game+0xe95cd8+1905*4,uint32_t(0));if(scenario==6)assert(VirtualProtect(reinterpret_cast<void*>(info160),4096,PAGE_NOACCESS,&previous));if(scenario==7)assert(VirtualProtect(reinterpret_cast<void*>(info160),4096,PAGE_READWRITE|PAGE_GUARD,&previous));if(scenario==8)Put160(entity160+0x210,uint32_t(0x2800));if(scenario==9)Put160(stack160+0x14,uint32_t(1));if(scenario==10)*CShadows::pFrameCounter=21;if(scenario==11)Put160(entity160+0x1bc,uint32_t(3));if(scenario==12)Put160(info160+8,uint32_t(0));if(scenario==13)Put160(game+0x16d0,uint8_t(0x33));if(scenario==14)PoleStaticPartition141::binding.thread++;if(scenario==15)PoleStaticPartition141::prepared=false;if(scenario==16)mutation160=flags160;if(scenario==17)mutation160=game+0xe95cd8+1905*4;if(scenario==18)mutation160=game+0x1232c60;if(scenario==19)mutation160=entity160+0x210;if(scenario==20)FusionBackend130::prepared=false;if(scenario==21)FusionBackend130::pinnedModule=reinterpret_cast<HMODULE>(ff+4);
uint32_t eligibility=999;SetLastError(160);const auto fp=_mm_getcsr();const bool accepted=old?PoleStaticPartition141::ReferenceReclassify158(entity160,4,0,stack160,false,&eligibility):PoleStaticPartition141::Reclassify(entity160,4,0,stack160,false,&eligibility);assert(GetLastError()==160&&_mm_getcsr()==fp);assert(!traffic_signal::fresh_page::operation44&&!immediate_region33::current);if(scenario==7){MEMORY_BASIC_INFORMATION m{};assert(VirtualQuery(reinterpret_cast<void*>(info160),&m,sizeof(m))&&m.Protect&PAGE_GUARD);}return{accepted,eligibility,events160,fullReads160,workingQueries160,regionQueries160};}
int main(){game=uint32_t(uintptr_t(VirtualAlloc(nullptr,0x1240000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE)));assert(game);entity160=game+0x1000;pool160=game+0x3000;info160=game+0x5000;flags160=game+0x7000;stack160=game+0x12000;for(unsigned s=0;s<22;++s){const auto old=Run160(true,s),now=Run160(false,s);assert(old.accepted==now.accepted&&old.eligibility==now.eligibility&&old.events==now.events);if(s==0)assert(now.accepted&&old.reads==now.reads);if(s==1)assert(old.working==5&&now.working==3&&old.reads==5&&now.reads==3);std::cout<<"scenario="<<s<<" decision="<<now.accepted<<" full_reads="<<old.reads<<"->"<<now.reads<<" WorkingSet="<<old.working<<"->"<<now.working<<" region="<<old.regions<<"->"<<now.regions<<"\n";}LARGE_INTEGER frequency{},a{},b{};assert(QueryPerformanceFrequency(&frequency));
for(bool positive:{false,true}){Init160();if(!positive)Put160(info160+0x3c,uint32_t(123));const unsigned rounds=positive?1000:5000;
for(bool old:{true,false,false,true}){regionQueries160=workingQueries160=fullReads160=0;assert(QueryPerformanceCounter(&a));for(unsigned n=0;n<rounds;++n){uint32_t out=999;const bool accepted=old?PoleStaticPartition141::ReferenceReclassify158(entity160,4,0,stack160,false,&out):PoleStaticPartition141::Reclassify(entity160,4,0,stack160,false,&out);assert(accepted==positive&&out==(positive?1u:999u));}assert(QueryPerformanceCounter(&b));std::cout<<"BOUNDED production positive="<<positive<<" old="<<old<<" callbacks="<<rounds<<" working_queries="<<workingQueries160<<" region_queries="<<regionQueries160<<" total_reads="<<fullReads160<<" CPU_ms="<<(double(b.QuadPart-a.QuadPart)*1000/frequency.QuadPart)<<" added_hint_copies=0\n";}}
assert(VirtualFree(reinterpret_cast<void*>(game),0,MEM_RELEASE));std::cout<<"ACTUAL full old/new Reclassify parity; unrelated model total reads5->3, WorkingSet queries5->3, no added hint copies and same full positive authority; supported/full freshness remains unchanged. Not live FPS proof.\n";}
