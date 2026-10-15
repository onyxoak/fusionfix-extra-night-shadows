#include "FreshReadBatch30.hpp"
#include <cassert>
#include <cstdio>
namespace fp=traffic_signal::fresh_page;
unsigned queryCount147=0;
BOOL WINAPI Query147(HANDLE,PVOID memory,DWORD n){++queryCount147;auto* entries=static_cast<PSAPI_WORKING_SET_EX_INFORMATION*>(memory);for(unsigned i=0;i<n/sizeof(*entries);++i){entries[i].VirtualAttributes.Valid=1;entries[i].VirtualAttributes.Win32Protection=PAGE_READWRITE;}return TRUE;}
bool Refuse147(uintptr_t,size_t)noexcept{return false;}
unsigned Run147(unsigned capacity){queryCount147=0;{fp::Operation44 operation(capacity);for(unsigned round=0;round<3;++round)for(unsigned i=0;i<32;++i)assert(fp::Readable(0x100000+i*4096,4,Refuse147,Query147));assert(operation.count==capacity);}assert(fp::operation44==nullptr);return queryCount147;}
BOOL WINAPI ActualQuery147(HANDLE process,PVOID memory,DWORD n){++queryCount147;return QueryWorkingSetEx(process,memory,n);}
double ActualCost147(unsigned capacity,unsigned char* bytes){LARGE_INTEGER begin{},end{},frequency{};QueryPerformanceFrequency(&frequency);QueryPerformanceCounter(&begin);queryCount147=0;
 for(unsigned iteration=0;iteration<200;++iteration){fp::Operation44 operation(capacity);for(unsigned round=0;round<3;++round)for(unsigned i=0;i<32;++i)assert(fp::Readable(uintptr_t(bytes)+i*4096,4,Refuse147,ActualQuery147));}
 QueryPerformanceCounter(&end);return double(end.QuadPart-begin.QuadPart)*1000.0/double(frequency.QuadPart);}
int main(){const auto eight=Run147(8),thirtyTwo=Run147(32);assert(eight==80&&thirtyTwo==32);
 {fp::Operation44 outer(32);for(unsigned i=0;i<32;++i)assert(fp::Readable(0x100000+i*4096,4,Refuse147,Query147));const auto count=queryCount147;{fp::Operation44 nested(32);assert(fp::Readable(0x100000,4,Refuse147,Query147));assert(queryCount147==count+1);}assert(fp::Readable(0x100000,4,Refuse147,Query147));assert(queryCount147==count+1);
 assert(fp::Readable(0x200000,4,Refuse147,Query147));assert(fp::Readable(0x200000,4,Refuse147,Query147));assert(queryCount147==count+3);}
 assert(!fp::Known44(0x100000,4,4096,Query147));assert(!fp::Readable(UINTPTR_MAX-1,4,Refuse147,Query147));
 {fp::Operation44 zero(0);assert(fp::Readable(0x100000,4,Refuse147,Query147));assert(zero.count==0);}
 {fp::Operation44 cap(1000);assert(cap.capacity==32);}
 std::printf("Read-only permission-scope147: original8=%u extended32=%u queries; overflow/nesting/return boundaries preserved\n",eight,thirtyTwo);
 auto* bytes=static_cast<unsigned char*>(VirtualAlloc(nullptr,32*4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(bytes);for(unsigned i=0;i<32;++i)bytes[i*4096]=1;
 const auto oldMs=ActualCost147(8,bytes);const auto oldQueries=queryCount147;const auto newMs=ActualCost147(32,bytes);const auto newQueries=queryCount147;assert(oldQueries==16000&&newQueries==6400);VirtualFree(bytes,0,MEM_RELEASE);
 std::printf("Actual OS production-readable fixture200 operations: capacity8 %.3fms %uqueries; capacity32 %.3fms %uqueries. Synthetic repeated32pages, not gameplay/FPS proof.\n",oldMs,oldQueries,newMs,newQueries);
}
