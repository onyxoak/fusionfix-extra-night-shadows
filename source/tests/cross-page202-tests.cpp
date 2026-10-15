#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#include <array>
#include <cassert>
#include <cstring>
#include <cstdio>
#include "ImmediateRegion33.hpp"
#include "FreshReadWindow59.hpp"
#include "FreshReadNativeWindow115.hpp"
#define fresh_certificate59 certificate59_196
#include "certificate59-196-reference202.hpp"
#undef fresh_certificate59
#define fresh_native_certificate115 certificate115_196
#include "certificate115-196-reference202.hpp"
#undef fresh_native_certificate115

unsigned queries{},fallbacks{},queriedPages{};
uintptr_t alterPage{};unsigned mode{};constexpr unsigned Page=4096;
BOOL WINAPI Query(HANDLE process,PVOID buffer,DWORD size){
 ++queries;queriedPages+=size/sizeof(PSAPI_WORKING_SET_EX_INFORMATION);
 if(mode==1)return FALSE;
 const auto result=QueryWorkingSetEx(process,buffer,size);
 auto* rows=static_cast<PSAPI_WORKING_SET_EX_INFORMATION*>(buffer);
 for(unsigned i=0;i<size/sizeof(*rows);++i)if(uintptr_t(rows[i].VirtualAddress)/Page==alterPage/Page){
  if(mode==2)rows[i].VirtualAttributes.Valid=0;
  if(mode==3)rows[i].VirtualAttributes.Locked=1;
  if(mode==4){rows[i].VirtualAttributes.Valid=1;rows[i].VirtualAttributes.Win32Protection=PAGE_READWRITE|PAGE_GUARD;}
 }
 // Deliberately make the first page nonresident and the second explicitly
 // guarded. The known second-page denial must win without a fallback read.
 if(mode==5&&size>=2*sizeof(*rows)){rows[0].VirtualAttributes.Valid=0;rows[1].VirtualAttributes.Valid=1;rows[1].VirtualAttributes.Win32Protection=PAGE_READWRITE|PAGE_GUARD;}
 return result;
}
bool Region(uintptr_t address,size_t size)noexcept{++fallbacks;return immediate_region33::Readable(address,size);}
#define ADAPTER(Name,Namespace,Window) struct Name { \
 using Certificate=traffic_signal::Namespace::Certificate;using Span=traffic_signal::Namespace::Span;using Group=traffic_signal::Namespace::Group; \
 static void Preflight(Certificate& c,const Group* g,unsigned n){traffic_signal::Namespace::Preflight(c,g,n,Region,Query);} \
 static bool Copy(Certificate& c,unsigned g,const Span* s,unsigned n,bool& copied,bool& faulted){return traffic_signal::Namespace::TryCopy(c,g,s,n,copied,faulted);} \
 static auto Fresh(const Group* g,unsigned n){return traffic_signal::Window::Copy(g,n,Region,Query);} };
ADAPTER(New59,fresh_certificate59,fresh_window59)
ADAPTER(Old59,certificate59_196,fresh_window59)
ADAPTER(New115,fresh_native_certificate115,fresh_native_window115)
ADAPTER(Old115,certificate115_196,fresh_native_window115)
#undef ADAPTER
struct Observation{unsigned queries{},fallbacks{},pages{},stages{};std::array<std::array<uint8_t,128>,8> bytes{};};
template<class Policy> Observation Sequence(uint8_t* memory){
 typename Policy::Certificate certificate{};std::array<typename Policy::Span,8> spans{};std::array<typename Policy::Group,8> groups{};
 Observation out;for(unsigned i=0;i<8;++i){spans[i]={uintptr_t(i?memory+8192+i*128:memory+4096-64),out.bytes[i].data(),128};groups[i]={&spans[i],1};}
 mode=queries=fallbacks=queriedPages=0;Policy::Preflight(certificate,groups.data(),8);
 memory[4096]=0x91; // The permission preflight contains no copied data.
 for(unsigned i=0;i<8;++i){bool copied{},faulted{};if(!Policy::Copy(certificate,i,&spans[i],1,copied,faulted)){
   const auto fresh=Policy::Fresh(&groups[i],1);copied=fresh.copied[0];faulted=fresh.faulted[0];
  }assert(copied&&!faulted);}
 assert(out.bytes[0][64]==0x91);out.queries=queries;out.fallbacks=fallbacks;out.pages=queriedPages;out.stages=certificate.freshStages;return out;
}
template<class Policy> void Refusals(uint8_t* memory){
 typename Policy::Certificate certificate{};std::array<uint8_t,128> output{};output.fill(0xee);
 typename Policy::Span span{uintptr_t(memory+4096-64),output.data(),128};typename Policy::Group group{&span,1};bool copied{},faulted{};
 for(unsigned queryMode:{1u,2u,3u}){mode=queryMode;alterPage=uintptr_t(memory+4096);queries=fallbacks=0;
  Policy::Preflight(certificate,&group,1);assert(certificate.active&&certificate.allowed[0]&&!certificate.reusable[0]&&fallbacks==1);
  assert(!Policy::Copy(certificate,0,&span,1,copied,faulted)&&!certificate.active&&!copied&&!faulted);
 }
 for(unsigned queryMode:{4u,5u}){mode=queryMode;alterPage=uintptr_t(memory+4096);queries=fallbacks=0;
  Policy::Preflight(certificate,&group,1);assert(!certificate.allowed[0]&&fallbacks==0);
  assert(!Policy::Copy(certificate,0,&span,1,copied,faulted)&&!copied&&!faulted);
 }
 mode=0;
 for(DWORD protection:{DWORD(PAGE_NOACCESS),DWORD(PAGE_READWRITE|PAGE_GUARD)}){
  DWORD prior{};assert(VirtualProtect(memory+4096,4096,protection,&prior));
  Policy::Preflight(certificate,&group,1);assert(!certificate.allowed[0]);
  assert(!Policy::Copy(certificate,0,&span,1,copied,faulted)&&!copied&&!faulted);
  MEMORY_BASIC_INFORMATION info{};assert(VirtualQuery(memory+4096,&info,sizeof(info)));
  assert(protection!=DWORD(PAGE_READWRITE|PAGE_GUARD)||(info.Protect&PAGE_GUARD));
  assert(VirtualProtect(memory+4096,4096,PAGE_READWRITE,&prior));
 }
 // Real mapping changes after positive preflight remain contained by the
 // actual SEH stage, without exposing its partial internal staging bytes.
 for(DWORD protection:{DWORD(PAGE_NOACCESS),DWORD(PAGE_READWRITE|PAGE_GUARD)}){
  std::memset(memory,0x17,12288);Policy::Preflight(certificate,&group,1);assert(certificate.reusable[0]);
  DWORD prior{};assert(VirtualProtect(memory+4096,4096,protection,&prior));
  assert(Policy::Copy(certificate,0,&span,1,copied,faulted)&&!copied&&faulted&&!certificate.active&&certificate.stageFaults==1);
  for(auto byte:output)assert(byte==0xee);
  assert(!Policy::Copy(certificate,0,&span,1,copied,faulted));
  assert(VirtualProtect(memory+4096,4096,PAGE_READWRITE,&prior));
 }
 std::memset(memory,0x17,12288);Policy::Preflight(certificate,&group,1);span.size=127;
 assert(!Policy::Copy(certificate,0,&span,1,copied,faulted)&&!certificate.active);span.size=128;
 // A new callback cannot reuse an older positive permission observation.
 DWORD prior{};assert(VirtualProtect(memory+4096,4096,PAGE_NOACCESS,&prior));
 Policy::Preflight(certificate,&group,1);assert(!certificate.allowed[0]);assert(VirtualProtect(memory+4096,4096,PAGE_READWRITE,&prior));
}
int main(){
 auto* memory=static_cast<uint8_t*>(VirtualAlloc(nullptr,12288,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(memory);std::memset(memory,0x17,12288);
 memory_read46::enabled=true;SetLastError(0x202);const auto csr=_mm_getcsr();
 const auto old59=Sequence<Old59>(memory),new59=Sequence<New59>(memory),old115=Sequence<Old115>(memory),new115=Sequence<New115>(memory);
 for(auto pair:{std::pair{old59,new59},std::pair{old115,new115}}){assert(pair.first.bytes==pair.second.bytes);assert(pair.first.queries==8&&pair.second.queries==1);assert(pair.first.fallbacks==2&&pair.second.fallbacks==0);assert(pair.second.stages==8);}
 Refusals<New59>(memory);Refusals<New115>(memory);
 assert(GetLastError()==0x202&&_mm_getcsr()==csr);assert(VirtualFree(memory,0,MEM_RELEASE));
 std::printf("actual Certificate59 and NativeCertificate115: cross-page first group +7 ordinary groups, QWS %u->%u, fallback %u->%u; identical fresh bytes. Both-page guard/noaccess, invalid/exotic/query-failure fallback, terminal SEH revocation, span binding and fresh-domain denial passed. No runtime/FPS proof.\n",old115.queries,new115.queries,old115.fallbacks,new115.fallbacks);
}
