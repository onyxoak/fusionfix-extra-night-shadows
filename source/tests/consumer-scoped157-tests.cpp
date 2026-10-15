#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#include <array>
#include <atomic>
#include <cassert>
#include <bit>
#include <cstring>
#include <iostream>
#include "core/ShadowFloatingPointState.hpp"
#include "safetyhook/context.hpp"
#include "FreshPageReadable.hpp"
#include "ImmediateRegion33.hpp"
using SafetyHookContext=safetyhook::Context32;
namespace ModelEffectObserver {struct LastErrorScope{DWORD value=GetLastError();~LastErrorScope(){SetLastError(value);}};}
namespace CShadows {uint32_t* pFrameCounter;}
namespace ShadowTrace34 {struct Event{uint32_t stage,frame,tick,key;int32_t a,b,c,d;float x,y,z,w;};unsigned applied=0;void Emit(Event){++applied;}}
std::atomic<bool> addonActive{true};uintptr_t game{};
static unsigned queries=0,reads=0,packetReads=0;static uintptr_t packetAddress=0;static bool revoke=false,failQuery=false,guardQuery=false;
static BOOL WINAPI Query157(HANDLE h,PVOID data,DWORD size){++queries;const BOOL ok=QueryWorkingSetEx(h,data,size);if(failQuery)return FALSE;if(guardQuery&&ok)static_cast<PSAPI_WORKING_SET_EX_INFORMATION*>(data)->VirtualAttributes.Win32Protection=PAGE_GUARD|PAGE_READWRITE;return ok;}
static bool Region157(uintptr_t p,size_t n)noexcept{return immediate_region33::Readable(p,n);}
static __declspec(noinline) bool CopyLocal157(uintptr_t p,void* out,size_t n)noexcept{__try{std::memcpy(out,reinterpret_cast<void*>(p),n);return true;}__except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION||GetExceptionCode()==EXCEPTION_GUARD_PAGE?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){return false;}}
namespace Liberty20 {bool Copy(uintptr_t p,void* out,size_t n)noexcept{++reads;if(!traffic_signal::fresh_page::Readable(p,n,Region157,Query157))return false;if(p==packetAddress&&++packetReads==3&&revoke){DWORD old{};assert(VirtualProtect(reinterpret_cast<void*>(p),4096,PAGE_NOACCESS,&old));}return CopyLocal157(p,out,n);}}
#include "ConsumerReachRuntime136.inc"
#define ConsumerReach136 ConsumerBaseline157
#include "consumer156-reference.inc"
#undef ConsumerReach136
static unsigned authorized=0;static bool permit=true,mutate=false,nest=false;static std::array<uintptr_t,5> pages{};
static bool Authorize157(const consumer_shadow75::Packet&,uint32_t)noexcept{++authorized;if(mutate)reinterpret_cast<consumer_shadow75::Packet*>(pages[1])->words[24]++;if(nest){traffic_signal::fresh_page::Operation44 op(32);immediate_region33::Scope regions;uint32_t f{};assert(Liberty20::Copy(pages[0],&f,4));}return permit;}
bool ConsumerReach136::Authorize(const consumer_shadow75::Packet& p,uint32_t f)noexcept{return Authorize157(p,f);}
bool ConsumerBaseline157::Authorize(const consumer_shadow75::Packet& p,uint32_t f)noexcept{return Authorize157(p,f);}
static uintptr_t Esp(){return pages[2]+0x40;}
struct Result{unsigned query,read,auth,event;uint8_t decision;bool operator==(const Result& b)const{return read==b.read&&auth==b.auth&&event==b.event&&decision==b.decision;}};
static void Init(){for(auto p:pages){assert(VirtualAlloc(reinterpret_cast<void*>(p),4096,MEM_COMMIT,PAGE_READWRITE));DWORD old{};assert(VirtualProtect(reinterpret_cast<void*>(p),4096,PAGE_READWRITE,&old));std::memset(reinterpret_cast<void*>(p),0,4096);}*reinterpret_cast<uint32_t*>(pages[0])=1;CShadows::pFrameCounter=reinterpret_cast<uint32_t*>(pages[0]);packetAddress=pages[1];auto& p=*reinterpret_cast<consumer_shadow75::Packet*>(packetAddress);p.words[17]=0;p.words[18]=69;p.words[21]=std::bit_cast<uint32_t>(10.f);p.words[16]=std::bit_cast<uint32_t>(1.f);p.words[24]=123;for(unsigned i=12;i<15;++i)p.words[i]=std::bit_cast<uint32_t>(1.f);const uint32_t src=uint32_t(packetAddress+0x28),cam=uint32_t(pages[3]);const float radius=10,pos=31,scale=.339770854f;std::memcpy(reinterpret_cast<void*>(Esp()+0x1c),&src,4);std::memcpy(reinterpret_cast<void*>(Esp()+0x28),&cam,4);std::memcpy(reinterpret_cast<void*>(Esp()+0x14),&radius,4);*reinterpret_cast<uint8_t*>(Esp()+0xd)=1;std::memcpy(reinterpret_cast<void*>(pages[3]+0x70),&pos,4);std::memcpy(reinterpret_cast<void*>(pages[3]+0x2cc),&scale,4);*reinterpret_cast<float*>(pages[4])=1;game=pages[4]-0xc3efdc;ConsumerReach136::requested=ConsumerBaseline157::requested=true;ConsumerReach136::context={};ConsumerBaseline157::context={};queries=reads=authorized=ShadowTrace34::applied=packetReads=0;permit=true;mutate=nest=revoke=failQuery=guardQuery=false;}
static Result Run(bool old,unsigned scenario){Init();if(scenario==1)permit=false;if(scenario==2)mutate=true;if(scenario==3)revoke=true;if(scenario==4)failQuery=true;if(scenario==5)guardQuery=true;if(scenario==6){DWORD prev{};assert(VirtualProtect(reinterpret_cast<void*>(pages[3]),4096,PAGE_NOACCESS,&prev));}if(scenario==7)nest=true;if(scenario==8)ConsumerReach136::requested=ConsumerBaseline157::requested=false;if(scenario==12)assert(VirtualFree(reinterpret_cast<void*>(pages[3]),4096,MEM_DECOMMIT));if(scenario==13){DWORD prev{};assert(VirtualProtect(reinterpret_cast<void*>(pages[3]),4096,PAGE_READWRITE|PAGE_GUARD,&prev));}if(scenario==9)reinterpret_cast<consumer_shadow75::Packet*>(packetAddress)->words[18]=0x564;
SafetyHookContext l{};l.edi=uint32_t(packetAddress+0x28);l.esp=uint32_t(Esp()-12);l.eax=scenario==10?UINT32_MAX:1;if(old)ConsumerBaseline157::Observe(consumer_shadow75::Stage::Lookup,l);else ConsumerReach136::Observe(consumer_shadow75::Stage::Lookup,l);
SafetyHookContext g{};g.esp=uint32_t(Esp());g.xmm0.f32[0]=.339770854f;g.xmm1.f32[0]=scenario==11?.1f:10.f/31;if(old)ConsumerBaseline157::Observe(consumer_shadow75::Stage::Gate,g);else ConsumerReach136::Observe(consumer_shadow75::Stage::Gate,g);assert(!traffic_signal::fresh_page::operation44&&!immediate_region33::current);return{queries,reads,authorized,ShadowTrace34::applied,*reinterpret_cast<uint8_t*>(Esp()+0xe)};}
int main(){for(auto& p:pages){p=reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(p);}unsigned oldSuccess=0,newSuccess=0;for(unsigned s=0;s<14;++s){const auto old=Run(true,s),now=Run(false,s);assert(old==now);assert(now.query<=old.query);if(s==0){assert(now.decision==1&&now.auth==1&&now.event==1);oldSuccess=old.query;newSuccess=now.query;assert(now.query<old.query);}if(s==2||s==3||s==5||s==6||s==12||s==13)assert(now.decision==0);if(s==8||s==10)assert(now.read==0&&now.query==0);std::cout<<"scenario="<<s<<" reads="<<now.read<<" working_set_queries="<<old.query<<"->"<<now.query<<" decision="<<unsigned(now.decision)<<"\n";}Init();{traffic_signal::fresh_page::Operation44 outer(32);immediate_region33::Scope outerRegion;const auto* ptr=traffic_signal::fresh_page::operation44;const auto* r=immediate_region33::current;SafetyHookContext l{};l.edi=uint32_t(packetAddress+0x28);l.esp=uint32_t(Esp()-12);l.eax=1;ConsumerReach136::Observe(consumer_shadow75::Stage::Lookup,l);assert(traffic_signal::fresh_page::operation44==ptr&&immediate_region33::current==r);}Init();SetLastError(157);const auto mxcsr=_mm_getcsr();SafetyHookContext l{};l.edi=uint32_t(packetAddress+0x28);l.esp=uint32_t(Esp()-12);l.eax=1;ConsumerReach136::Observe(consumer_shadow75::Stage::Lookup,l);assert(GetLastError()==157&&_mm_getcsr()==mxcsr);for(auto p:pages)assert(VirtualFree(reinterpret_cast<void*>(p),0,MEM_RELEASE));std::cout<<"ACTUAL production Observe parity; successful callback OS queries "<<oldSuccess<<" -> "<<newSuccess<<"; permission revocation contained; scopes restored\n";}
