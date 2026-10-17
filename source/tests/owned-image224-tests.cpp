#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "OwnedImageReads224.hpp"
#include "ContainedReads236.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
alignas(4096) unsigned char nativeData224[8192]{};
unsigned permissionCalls224{};
namespace TrafficSignal{
bool Readable(uintptr_t at,size_t n)noexcept{
 ++permissionCalls224;MEMORY_BASIC_INFORMATION m{};
 return n&&at>=0x10000&&at<=UINTPTR_MAX-n&&VirtualQuery(reinterpret_cast<void*>(at),&m,sizeof(m))&&
  m.State==MEM_COMMIT&&owned_image224::ReadProtection(m.Protect)&&at>=uintptr_t(m.BaseAddress)&&
  at-uintptr_t(m.BaseAddress)<=m.RegionSize&&n<=m.RegionSize-(at-uintptr_t(m.BaseAddress));
}
}
namespace Liberty20{
#include "owned-copy224.inc"
}
int main(){
 uint32_t value=224,output{};std::memcpy(nativeData224,&value,4);
 const auto address=uintptr_t(nativeData224);
 assert(Liberty20::Copy(address,&output,4)&&output==224&&permissionCalls224==1);
 const auto base=uintptr_t(GetModuleHandleW(nullptr));
 const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
 const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(base+dos->e_lfanew);
 assert(nt->OptionalHeader.Magic==IMAGE_NT_OPTIONAL_HDR32_MAGIC);
 const auto count=owned_image224::Configure(base,nt->OptionalHeader.SizeOfImage,{IMAGE_FIRST_SECTION(nt),nt->FileHeader.NumberOfSections});
 assert(count&&owned_image224::Contains(address,sizeof(nativeData224)));
 const unsigned before=permissionCalls224;
 for(unsigned i=0;i<100000;++i){std::memcpy(nativeData224,&i,4);assert(Liberty20::Copy(address,&output,4)&&output==i);}
 assert(permissionCalls224==before);
 assert(!owned_image224::Contains(1,4)&&!owned_image224::Contains(address,0)&&!owned_image224::Contains(UINTPTR_MAX-1,4));
 const auto* code=reinterpret_cast<const void*>(&main);
 assert(!owned_image224::Contains(uintptr_t(code),1));
 auto* heap=VirtualAlloc(nullptr,8192,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);assert(heap);
 assert(!owned_image224::Contains(uintptr_t(heap),4));
 value=777;std::memcpy(heap,&value,4);assert(Liberty20::Copy(uintptr_t(heap),&output,4)&&output==777&&permissionCalls224==before+1);
 DWORD previous{};
 assert(VirtualProtect(nativeData224+4096,4096,PAGE_NOACCESS,&previous));
 assert(!Liberty20::Copy(address+4096,&output,4));
 assert(VirtualProtect(nativeData224+4096,4096,previous,&previous));
 assert(VirtualProtect(nativeData224+4096,4096,PAGE_READWRITE|PAGE_GUARD,&previous));
 SetLastError(224);assert(!Liberty20::Copy(address+4096,&output,4));assert(GetLastError()==224);
 MEMORY_BASIC_INFORMATION guard{};assert(VirtualQuery(nativeData224+4096,&guard,sizeof(guard))&&(guard.Protect&PAGE_GUARD));
 assert(VirtualProtect(nativeData224+4096,4096,PAGE_READWRITE,&previous));
 assert(VirtualFree(heap,0,MEM_RELEASE));
 std::cout<<"PASS actual Copy/CopyLocal: 100,000 fresh native-data copies without permission syscalls; heap/code excluded, range overflow refused, changed noaccess contained and guard restored. Not game FPS proof.\n";
}
