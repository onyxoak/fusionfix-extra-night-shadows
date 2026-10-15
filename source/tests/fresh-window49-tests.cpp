#include "FreshReadWindow49.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
namespace batch=traffic_signal::fresh_batch30;
namespace window=traffic_signal::fresh_window49;
const auto RealQuery=QueryWorkingSetEx;
unsigned queries{},queryPages{},fallbackCalls{};unsigned char* memory{};DWORD oldProtect{};
enum class Mutation{None,NoAccess,Guard,Free};Mutation mutation{};bool failQuery=false;
BOOL WINAPI CountQuery(HANDLE p,PVOID b,DWORD n){++queries;queryPages+=n/sizeof(PSAPI_WORKING_SET_EX_INFORMATION);
 const BOOL ok=failQuery?FALSE:RealQuery(p,b,n);
 if(mutation==Mutation::NoAccess){assert(VirtualProtect(memory,4096,PAGE_NOACCESS,&oldProtect));mutation=Mutation::None;}
 if(mutation==Mutation::Free){assert(VirtualFree(memory,4096,MEM_DECOMMIT));mutation=Mutation::None;}
 if(mutation==Mutation::Guard){assert(VirtualProtect(memory,4096,PAGE_READWRITE|PAGE_GUARD,&oldProtect));mutation=Mutation::None;}
 SetLastError(49);return ok;
}
bool Region(uintptr_t a,size_t n) noexcept{++fallbackCalls;MEMORY_BASIC_INFORMATION m{};
 if(a<0x10000||!n||a>UINTPTR_MAX-n||!VirtualQuery(reinterpret_cast<void*>(a),&m,sizeof(m)))return false;
 if(m.State!=MEM_COMMIT||m.Protect&(PAGE_GUARD|PAGE_NOACCESS))return false;
 switch(m.Protect){case PAGE_READONLY:case PAGE_READWRITE:case PAGE_WRITECOPY:case PAGE_EXECUTE_READ:case PAGE_EXECUTE_READWRITE:case PAGE_EXECUTE_WRITECOPY:break;default:return false;}
 return a>=uintptr_t(m.BaseAddress)&&n<=m.RegionSize-(a-uintptr_t(m.BaseAddress));
}
int main(){
 memory=static_cast<unsigned char*>(VirtualAlloc(nullptr,4096*33,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(memory);
 for(unsigned i=0;i<4096*33;++i)memory[i]=static_cast<unsigned char>(i%251);
 std::array<std::array<unsigned char,216>,16> serial{},wide{};
 std::array<std::array<batch::Span,2>,16> spans{};std::array<window::Group,16> groups{};
 auto configure=[&](bool shared){for(unsigned g=0;g<16;++g){spans[g]={batch::Span{uintptr_t(memory+(shared?0:g*4096)+8),wide[g].data(),108},batch::Span{uintptr_t(memory+(shared?4096:(g+16)*4096)+16),wide[g].data()+108,108}};groups[g]={spans[g].data(),2};}};
 for(bool shared:{false,true}){configure(shared);queries=queryPages=0;
  for(unsigned g=0;g<16;++g){auto input=spans[g];input[0].output=serial[g].data();input[1].output=serial[g].data()+108;
   const traffic_signal::fresh_page::Operation44 operation;assert(batch::Copy(input.data(),2,Region,CountQuery));}
  const auto serialQueries=queries,serialPages=queryPages;queries=queryPages=0;
  const auto error=GetLastError();const auto csr=_mm_getcsr();const auto result=window::Copy(groups.data(),16,Region,CountQuery);
  for(unsigned g=0;g<16;++g)assert(result.copied[g]&&wide[g]==serial[g]);
  assert(queries==1&&result.osQueries==1&&GetLastError()==error&&_mm_getcsr()==csr);
  std::printf("actual Windows production serial Copy vs fresh immediate window (%s): OS queries %u->%u; queried pages %u->%u; bytes %u\n",shared?"shared pages":"distinct pages",serialQueries,queries,serialPages,queryPages,result.copiedBytes);
 }
 configure(false);auto unchanged=wide;queries=0;mutation=Mutation::NoAccess;
 auto result=window::Copy(groups.data(),16,Region,CountQuery);assert(result.faults==1&&!result.copied[0]&&wide[0]==unchanged[0]);for(unsigned g=1;g<16;++g)assert(result.copied[g]);assert(queries==1);
 assert(VirtualProtect(memory,4096,PAGE_READWRITE,&oldProtect));
 // A guard already present is refused without consuming it.
 assert(VirtualProtect(memory,4096,PAGE_READWRITE|PAGE_GUARD,&oldProtect));result=window::Copy(groups.data(),16,Region,CountQuery);assert(!result.copied[0]);MEMORY_BASIC_INFORMATION info{};assert(VirtualQuery(memory,&info,sizeof(info)));assert(info.Protect&PAGE_GUARD);
 assert(VirtualProtect(memory,4096,PAGE_READWRITE,&oldProtect));
 // A new guard after positive QWS is caught once, never retried into acceptance.
 volatile unsigned char warmed=memory[8];(void)warmed;mutation=Mutation::Guard;queries=0;result=window::Copy(groups.data(),16,Region,CountQuery);assert(result.faults==1&&!result.copied[0]&&queries==1);assert(VirtualProtect(memory,4096,PAGE_READWRITE,&oldProtect));
 // Shared PAGE_GUARD is one-shot: only the faulting group is refused.
 configure(true);volatile unsigned char sharedWarm=memory[8];(void)sharedWarm;mutation=Mutation::Guard;result=window::Copy(groups.data(),16,Region,CountQuery);assert(result.faulted[0]&&!result.copied[0]);for(unsigned g=1;g<16;++g)assert(result.copied[g]);
 assert(VirtualQuery(memory,&info,sizeof(info)));assert(!(info.Protect&PAGE_GUARD));assert(VirtualProtect(memory,4096,PAGE_READWRITE,&oldProtect));
 // Shared unmapping after QWS faults every dependent group without outputs.
 mutation=Mutation::Free;unchanged=wide;result=window::Copy(groups.data(),16,Region,CountQuery);assert(result.faults==16);for(unsigned g=0;g<16;++g)assert(!result.copied[g]&&wide[g]==unchanged[g]);
 assert(VirtualAlloc(memory,4096,MEM_COMMIT,PAGE_READWRITE)==memory);for(unsigned i=0;i<4096;++i)memory[i]=static_cast<unsigned char>(i%251);configure(false);
 // Eight-span final full-owner group remains caller-bounded and atomic.
 std::array<batch::Span,8> finalSpans{};std::array<std::array<unsigned char,8>,8> finalBytes{};
 for(unsigned i=0;i<8;++i)finalSpans[i]={uintptr_t(memory+i*16),finalBytes[i].data(),8};window::Group finalGroup{finalSpans.data(),8};result=window::Copy(&finalGroup,1,Region,CountQuery);assert(result.copied[0]&&result.copiedBytes==64);for(unsigned i=0;i<8;++i)assert(!std::memcmp(finalBytes[i].data(),memory+i*16,8));finalGroup.count=9;assert(!window::Copy(&finalGroup,1,Region,CountQuery).copied[0]);
 // Changed descriptor data is reread by the NEXT phase, despite outer Operation44.
 {traffic_signal::fresh_page::Operation44 operation;queries=0;result=window::Copy(groups.data(),16,Region,CountQuery);assert(result.copied[0]);memory[8]=222;result=window::Copy(groups.data(),16,Region,CountQuery);assert(result.copied[0]&&wide[0][0]==222&&queries==2&&operation.count==0);}
 // Query failure uses current committed-region checks, not guessed approval.
 failQuery=true;fallbackCalls=0;result=window::Copy(groups.data(),16,Region,CountQuery);for(bool copied:result.copied)assert(copied);assert(result.fallbacks==32&&fallbackCalls==32);failQuery=false;
 // Per-group malformed input refusal leaves the other source outputs available.
 spans[0][0].address=UINTPTR_MAX-4;result=window::Copy(groups.data(),16,Region,CountQuery);assert(!result.copied[0]);for(unsigned g=1;g<16;++g)assert(result.copied[g]);configure(false);
 // Cross-page span must query current region and reject when second page revoked.
 spans[0][0]={uintptr_t(memory+4096-4),wide[0].data(),8};assert(VirtualProtect(memory+4096,4096,PAGE_NOACCESS,&oldProtect));result=window::Copy(groups.data(),16,Region,CountQuery);assert(!result.copied[0]);assert(VirtualProtect(memory+4096,4096,PAGE_READWRITE,&oldProtect));configure(false);
 assert(window::Copy(groups.data(),17,Region,CountQuery).groups==0);assert(window::Copy(nullptr,1,Region,CountQuery).groups==0);
 assert(VirtualFree(memory,0,MEM_RELEASE));std::puts("narrow read primitive only; no queued-owner decision integration, live frame cost or FPS claim");
}
