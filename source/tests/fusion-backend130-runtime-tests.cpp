#define NOMINMAX
#include <Windows.h>
#include <intrin.h>
#include <cassert>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <vector>
#include <stdexcept>
#include "FusionBackendGuard130.hpp"
#include "standalone-midstub.hpp"
#include "external/safetyhook/include/safetyhook/context.hpp"
using SafetyHookContext=safetyhook::Context32;
uintptr_t ff{};
uintptr_t game{};
using namespace onyxoak;
bool nativeBackend=false;
struct Layout130{};Layout130 Layout511;Layout130* layout=&Layout511;
namespace preset117 {struct Startup130{bool ownBackend=true;struct State130{bool enabled=true;}state;}startup;}
namespace shortlist_helper48::runtime {struct Binding130{bool exact511Authenticated=true;}binding;}
struct Original130{std::vector<uint8_t> bytes;};Original130 original130;Original130* original=&original130;
std::atomic<bool> addonActive{false},failed{false};
using Pure130=void(__fastcall*)(void*,void*,const int32_t*);
Pure130 pure130{};
unsigned queries130=0,reads130=0,admissionOriginal130=0;
bool recurse130=false,faultAfterPreflight130=false;
bool pinGood130=true;unsigned prepares130=0;
uintptr_t originalEip130=0x12340000;
void Reenter130();
struct SafetyHookInline {
 explicit operator bool()const noexcept{return pure130!=nullptr;}
 template<class R>R unsafe_thiscall(void* self,const int32_t* value){
  if(faultAfterPreflight130){faultAfterPreflight130=false;DWORD old{};VirtualProtect(reinterpret_cast<void*>(ff+fusion_backend130::VehicleRva),1,PAGE_NOACCESS,&old);}
  pure130(self,nullptr,value);if(recurse130){recurse130=false;Reenter130();}
 }
 template<class R>R unsafe_ccall(SafetyHookContext* regs){++admissionOriginal130;regs->eip=originalEip130;}
};
template<class Hook,class Function>void Prepare(Hook&,uintptr_t,Function){++prepares130;}
void Require(bool good,const char* reason){if(!good)throw std::runtime_error(reason);}
uint32_t U32(const uint8_t* p){return fusion_backend130::Word(p);}
namespace ModelEffectObserver {struct LastErrorScope {DWORD saved=GetLastError();~LastErrorScope(){SetLastError(saved);}};}
namespace fusionfix::shadows {struct FloatingPointState {alignas(16)uint8_t fp[512];FloatingPointState(){_fxsave(fp);}~FloatingPointState(){_fxrstor(fp);}};}
namespace Liberty20 {
bool Copy(uintptr_t at,void* out,size_t n)noexcept {
 ++reads130;MEMORY_BASIC_INFORMATION m{};
 if(!VirtualQuery(reinterpret_cast<void*>(at),&m,sizeof(m))||m.State!=MEM_COMMIT||(m.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;
 __try{std::memcpy(out,reinterpret_cast<void*>(at),n);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
}
std::vector<uint8_t> ReadMemory(uintptr_t at,size_t n){std::vector<uint8_t> out(n);Require(Liberty20::Copy(at,out.data(),n),"fixture_unreadable");return out;}
uintptr_t Branch(uintptr_t at,uint8_t opcode=0xe9){const auto b=ReadMemory(at,5);Require(b[0]==opcode,"fixture_opcode");return uint32_t(at+5+U32(b.data()+1));}
#include "fusion-backend130-verify.inc"
BOOL WINAPI Pin130(DWORD,LPCWSTR,HMODULE* out){if(!pinGood130)return FALSE;*out=reinterpret_cast<HMODULE>(ff);return TRUE;}
SIZE_T WINAPI Query130(LPCVOID at,PMEMORY_BASIC_INFORMATION out,SIZE_T n){
 ++queries130;const auto result=::VirtualQuery(at,out,n);
 // Positive module identity is fixture-bound; actual PAGE protections stay real.
 if(result&&reinterpret_cast<uintptr_t>(out->AllocationBase)==ff)out->Type=MEM_IMAGE;
 return result;
}
#define VirtualQuery Query130
#define GetModuleHandleExW Pin130
#include "fusion-backend130-runtime.inc"
#undef GetModuleHandleExW
#undef VirtualQuery
void Reenter130(){int32_t nested=2;FusionBackend130::Setting(nullptr,nullptr,&nested);}
static DWORD WINAPI Foreign130(void* opaque){const auto mode=*static_cast<int32_t*>(opaque);FusionBackend130::Setting(nullptr,nullptr,&mode);return 0;}
static std::array<uint8_t,3> Tuple130(){return {*reinterpret_cast<uint8_t*>(ff+fusion_backend130::ExtraRva),*reinterpret_cast<uint8_t*>(ff+fusion_backend130::HeadRva),*reinterpret_cast<uint8_t*>(ff+fusion_backend130::VehicleRva)};}
static void Reset130(uint8_t mode){
 auto& state=FusionBackend130::lease;
 state.armed=false;state.wanted=false;state.owned=false;state.owner=0;state.processed=UINT64_MAX;state.processedWanted=false;state.dirty=false;state.requested=0;
 int32_t native=mode;pure130(reinterpret_cast<void*>(0x12345678),nullptr,&native);
 assert(state.Arm(GetCurrentThreadId(),mode));FusionBackend130::prepared=true;FusionBackend130::pinnedModule=reinterpret_cast<HMODULE>(ff);
 addonActive=false;failed=false;FusionBackend130::genuineRequests=0;FusionBackend130::deferredRequests=0;FusionBackend130::originalCalls=0;FusionBackend130::refused=0;
 queries130=reads130=admissionOriginal130=0;
}
static std::vector<void*> allocations130;
static uintptr_t InstallMid130(uint32_t rva,uint32_t callback,unsigned span,bool vehicle){
 auto* memory=static_cast<uint8_t*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));assert(memory);allocations130.push_back(memory);
 const auto relay=reinterpret_cast<uintptr_t>(memory),stub=relay+64,tramp=relay+512;
 auto store=[](void* at,uint32_t value){std::memcpy(at,&value,4);};
 auto* installed=reinterpret_cast<uint8_t*>(game+rva);installed[0]=0xe9;store(installed+1,uint32_t(relay-game-rva-5));for(unsigned n=5;n<span;++n)installed[n]=0x90;
 memory[0]=0xe9;store(memory+1,uint32_t(stub-relay-5));std::memcpy(memory+64,MidStubTemplate.data(),171);
 store(memory+66,uint32_t(stub+167));store(memory+153,uint32_t(stub+163));store(memory+227,uint32_t(ff+callback));store(memory+231,uint32_t(tramp));
 unsigned displaced=span;
 if(vehicle){memory[512]=0x0f;memory[513]=0x84;store(memory+514,uint32_t(game+0x6e4255-tramp-6));memory[518]=0x53;memory[519]=0x6a;memory[520]=0;displaced=9;}
 else std::memcpy(memory+512,original->bytes.data()+rva,span);
 memory[512+displaced]=0xe9;store(memory+513+displaced,uint32_t(game+rva+span-tramp-displaced-5));return tramp;
}
int main(){
 static_assert(sizeof(void*)==4);
 ff=reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr,0x400000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(ff);
 auto* machine=static_cast<uint8_t*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));assert(machine);
 std::memcpy(machine,fusion_backend130::ExtraBody.data(),fusion_backend130::ExtraBody.size());
 for(unsigned at:{0x21u,0x26u,0x2cu}){const auto value=fusion_backend130::Word(machine+at)-fusion_backend130::PreferredFF+uint32_t(ff);std::memcpy(machine+at,&value,4);}
 FlushInstructionCache(GetCurrentProcess(),machine,fusion_backend130::ExtraBody.size());pure130=reinterpret_cast<Pure130>(machine);
 // Machine-execute the EXACT official FF callback, including RET4/forwarding pointer.
 constexpr std::array<std::array<uint8_t,3>,4> tuples{{{{0,0,1}},{{1,0,1}},{{1,1,0}},{{1,1,1}}}};
 for(int32_t mode130=0;mode130<4;++mode130){
  DWORD before130=0,after130=0;void* code130=machine;const int32_t* pointer130=&mode130;
  __asm{
   mov before130,esp
   mov ecx,13572468h
   mov edx,24681357h
   push pointer130
   call code130
   mov after130,esp
  }
  assert(before130==after130&&Tuple130()==tuples[size_t(mode130)]);
 }
 // True startupOff: actual production boundary makes ZERO OS queries/calls/reads.
 FusionBackend130::prepared=false;FusionBackend130::Boundary();assert(queries130==0&&reads130==0&&FusionBackend130::originalCalls==0);
 // FULL actual Prepare, not a mirror: hash/owners/binding/write denial occurs
 // before any callback or effective tuple mutation. Module pin/image identity
 // are mocked; mid bytes/relocations/read and real page protection are actual.
 game=reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr,0x820000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(game);original->bytes.resize(0x820000);
 const std::array<uint8_t,6> listNative130{{0x51,0x53,0x8b,0x5c,0x24,0x10}}; // actual live131 capture driver
 std::array<uint8_t,16> drawNative131{{0x55,0x8b,0xec,0x83,0xe4,0xf0,0x83,0xec,0x28,0x80,0x3d,0,0,0,0,0}};
 const uint32_t drawScalar131=uint32_t(game+0x1193312);std::memcpy(drawNative131.data()+11,&drawScalar131,4);
 const std::array<uint8_t,5> vehicleNative130{{0x74,0x1d,0x53,0x6a,0}};
 const std::array<uint8_t,6> flagsNative130{{0x8b,0x86,0x4c,0,0,0}}; // guard fixture native six-byte instruction
 std::memcpy(original->bytes.data()+0x6e3de0,listNative130.data(),6);std::memcpy(original->bytes.data()+0x6e3310,drawNative131.data(),16);
 std::memcpy(reinterpret_cast<void*>(game+0x6e3310),drawNative131.data(),16);
 std::memcpy(original->bytes.data()+0x6e4236,vehicleNative130.data(),5);std::memcpy(original->bytes.data()+0x81e615,flagsNative130.data(),6);
 const auto listTramp130=InstallMid130(0x6e3de0,fusion_backend130::ListCallbackRva,6,false);
 const auto vehicleTramp130=InstallMid130(0x6e4236,fusion_backend130::VehicleCallbackRva,5,true);
 const auto flagsTramp130=InstallMid130(0x81e615,fusion_backend130::FlagsCallbackRva,6,false);
 auto* callbackImage130=reinterpret_cast<uint8_t*>(ff+fusion_backend130::ExtraCallbackRva);
 std::memcpy(callbackImage130,machine,fusion_backend130::ExtraBody.size());
 auto* listImage130=reinterpret_cast<uint8_t*>(ff+fusion_backend130::ListCallbackRva);std::memcpy(listImage130,fusion_backend130::ListBody.data(),fusion_backend130::ListBody.size());
 uint32_t operand130=uint32_t(ff+fusion_backend130::ListIndexRva);std::memcpy(listImage130+0x10,&operand130,4);
 auto* listPointer130=reinterpret_cast<uint32_t*>(ff+fusion_backend130::ListFunctionRva);*listPointer130=uint32_t(game+0x6e3310);
 auto beforePrepare130=[](){Reset130(0);FusionBackend130::lease.armed=false;FusionBackend130::prepared=false;FusionBackend130::pinnedModule=nullptr;prepares130=0;};
 auto refusesPrepare130=[&](){bool thrown=false;try{FusionBackend130::PrepareHooks();}catch(const std::runtime_error&){thrown=true;}assert(thrown&&prepares130==0&&FusionBackend130::originalCalls==0&&Tuple130()==tuples[0]);FusionBackend130::Close();assert(FusionBackend130::originalCalls==0);};
 beforePrepare130();shortlist_helper48::runtime::binding.exact511Authenticated=false;refusesPrepare130();shortlist_helper48::runtime::binding.exact511Authenticated=true;
 beforePrepare130();++*listPointer130;refusesPrepare130();--*listPointer130;
 beforePrepare130();*reinterpret_cast<uint8_t*>(game+0x6e3310+15)^=1;refusesPrepare130();*reinterpret_cast<uint8_t*>(game+0x6e3310+15)^=1;
 // Explicitly reject130's wrong topology: capture hook at the distinct draw
 // routine cannot authorize the required live6E3DE0 capture contract.
 beforePrepare130();std::array<uint8_t,6> savedCapture131{};std::memcpy(savedCapture131.data(),reinterpret_cast<void*>(game+0x6e3de0),6);
 std::memcpy(reinterpret_cast<void*>(game+0x6e3de0),listNative130.data(),6);
 (void)InstallMid130(0x6e3310,fusion_backend130::ListCallbackRva,6,false);refusesPrepare130();
 std::memcpy(reinterpret_cast<void*>(game+0x6e3de0),savedCapture131.data(),6);std::memcpy(reinterpret_cast<void*>(game+0x6e3310),drawNative131.data(),16);
 beforePrepare130();callbackImage130[fusion_backend130::ExtraBody.size()-1]^=1;refusesPrepare130();callbackImage130[fusion_backend130::ExtraBody.size()-1]^=1;
 for(const auto trampCase130:{listTramp130,vehicleTramp130,flagsTramp130}){beforePrepare130();*reinterpret_cast<uint8_t*>(trampCase130)^=1;refusesPrepare130();*reinterpret_cast<uint8_t*>(trampCase130)^=1;}
 beforePrepare130();pinGood130=false;refusesPrepare130();pinGood130=true;
 beforePrepare130();DWORD codeProtect130{};assert(VirtualProtect(callbackImage130,1,PAGE_READWRITE|PAGE_GUARD,&codeProtect130));
 bool bodyDenied130=false;try{FusionBackend130::PrepareHooks();}catch(const std::runtime_error&){bodyDenied130=true;}assert(bodyDenied130&&FusionBackend130::originalCalls==0&&prepares130==0);
 MEMORY_BASIC_INFORMATION bodyGuard130{};assert(::VirtualQuery(callbackImage130,&bodyGuard130,sizeof(bodyGuard130))&&bool(bodyGuard130.Protect&PAGE_GUARD));assert(VirtualProtect(callbackImage130,1,codeProtect130,&codeProtect130));
 beforePrepare130();FusionBackend130::PrepareHooks();assert(prepares130==2&&FusionBackend130::originalCalls==0&&Tuple130()==tuples[0]);FusionBackend130::Close();assert(FusionBackend130::originalCalls==0);
 beforePrepare130();preset117::startup.state.enabled=false;FusionBackend130::PrepareHooks();assert(prepares130==0&&FusionBackend130::originalCalls==0);preset117::startup.state.enabled=true;
 // Execute the exact171-byte FF mid-context stub into the ACTUAL production
 // Admission, followed by authenticated TEST AL,6 + NOP continuation. FF's
 // original mode/interior decision is mocked, but its context/stack ABI and
 // native TEST execution are real. No synthetic packet/register mutation.
 auto put130=[](uint8_t* at,uint32_t value){std::memcpy(at,&value,4);};
 auto* mid130=machine+256;auto* continuation130=machine+768;
 std::memcpy(mid130,MidStubTemplate.data(),171);put130(mid130+2,uint32_t(reinterpret_cast<uintptr_t>(mid130+167)));put130(mid130+89,uint32_t(reinterpret_cast<uintptr_t>(mid130+163)));
 put130(mid130+163,uint32_t(reinterpret_cast<uintptr_t>(&FusionBackend130::Admission)));put130(mid130+167,uint32_t(reinterpret_cast<uintptr_t>(continuation130)));
 const std::array<uint8_t,5> testContinuation130{{0xa8,0x06,0x90,0x90,0x90}};std::memcpy(continuation130,testContinuation130.data(),5);continuation130[5]=0xe9;put130(continuation130+6,0);continuation130[10]=0x90;continuation130[11]=0x90;continuation130[12]=0x90;continuation130[13]=0xc3;continuation130[14]=0xc3;
 FlushInstructionCache(GetCurrentProcess(),mid130,1024);originalEip130=reinterpret_cast<uintptr_t>(continuation130+14);
 Reset130(0);assert(FusionBackend130::Commit());addonActive=true;DWORD midBefore130{},midAfter130{},result130{},flags130{};void* entry130=mid130;
 __asm{
  mov midBefore130,esp
  mov eax,4
  stc
  call entry130
  mov result130,eax
  pushfd
  pop flags130
  mov midAfter130,esp
 }
 assert(midBefore130==midAfter130&&result130==4&&!(flags130&1)&&admissionOriginal130==0);
 __asm{
  mov midBefore130,esp
  xor eax,eax
  stc
  call entry130
  mov result130,eax
  pushfd
  pop flags130
  mov midAfter130,esp
 }
 assert(midBefore130==midAfter130&&result130==0&&(flags130&1)&&admissionOriginal130==1);
 FusionBackend130::Close();originalEip130=0x12340000;
 for(uint8_t initial130=0;initial130<4;++initial130){
  Reset130(initial130);SetLastError(0x519077);const auto control130=_mm_getcsr();
  assert(FusionBackend130::Commit());addonActive=true;assert(Tuple130()==tuples[3]&&GetLastError()==0x519077&&_mm_getcsr()==control130);
  SafetyHookContext context{};context.eax=4;context.eip=0xabcde000;const auto beforeContext=context;
  FusionBackend130::Admission(context);assert(std::memcmp(&context,&beforeContext,sizeof(context))==0&&admissionOriginal130==0);
  context.eax=0;FusionBackend130::Admission(context);assert(admissionOriginal130==1);
  int32_t user130=1;FusionBackend130::Setting(reinterpret_cast<void*>(0x76543210),nullptr,&user130);assert(Tuple130()==tuples[3]&&FusionBackend130::genuineRequests==1);
  FusionBackend130::Close();assert(Tuple130()==tuples[1]&&!FusionBackend130::lease.Owns());
 }
 // Foreign requests are numeric/deferred, no original writer on that thread.
 Reset130(0);assert(FusionBackend130::Commit());addonActive=true;int32_t foreign130=2;
 const auto callsBefore130=FusionBackend130::originalCalls.load();auto worker130=CreateThread(nullptr,0,Foreign130,&foreign130,0,nullptr);assert(worker130);assert(WaitForSingleObject(worker130,5000)==WAIT_OBJECT_0);CloseHandle(worker130);
 assert(Tuple130()==tuples[3]&&FusionBackend130::deferredRequests==1&&FusionBackend130::originalCalls==callsBefore130);
 FusionBackend130::Boundary();assert(FusionBackend130::originalCalls==callsBefore130); // coalesced idempotent3
 failed=true;addonActive=false;FusionBackend130::Boundary();assert(!FusionBackend130::lease.Owns()&&Tuple130()==tuples[2]);
 // Reentrant request stays numeric and becomes the latest restore mode.
 Reset130(0);assert(FusionBackend130::Commit());addonActive=true;recurse130=true;int32_t own130=1;
 FusionBackend130::Setting(nullptr,nullptr,&own130);assert(Tuple130()==tuples[3]&&FusionBackend130::deferredRequests==1);
 FusionBackend130::Close();assert(Tuple130()==tuples[2]);
 // Fresh write protection, including guard preservation, on initial/restore.
 Reset130(0);DWORD protection130{};assert(VirtualProtect(reinterpret_cast<void*>(ff+fusion_backend130::ExtraRva),2,PAGE_READWRITE|PAGE_GUARD,&protection130));
 assert(!FusionBackend130::Commit()&&!FusionBackend130::lease.Owns()&&FusionBackend130::originalCalls==0);
 MEMORY_BASIC_INFORMATION guard130{};assert(::VirtualQuery(reinterpret_cast<void*>(ff+fusion_backend130::ExtraRva),&guard130,sizeof(guard130))&&bool(guard130.Protect&PAGE_GUARD));
 assert(VirtualProtect(reinterpret_cast<void*>(ff+fusion_backend130::ExtraRva),2,protection130,&protection130));
 FusionBackend130::Boundary();assert(Tuple130()==tuples[0]);
 Reset130(0);assert(FusionBackend130::Commit());addonActive=true;
 assert(VirtualProtect(reinterpret_cast<void*>(ff+fusion_backend130::VehicleRva),1,PAGE_NOACCESS,&protection130));FusionBackend130::Close();
 assert(!FusionBackend130::lease.Owns());assert(VirtualProtect(reinterpret_cast<void*>(ff+fusion_backend130::VehicleRva),1,protection130,&protection130));
 failed=true;addonActive=false;FusionBackend130::Boundary();assert(Tuple130()==tuples[0]);
 // Race after preflight: actual machine body partially writes then SEH fails;
 // admission closes and next valid owner boundary restores before failed exit.
 Reset130(0);faultAfterPreflight130=true;assert(!FusionBackend130::Commit());
 assert(!FusionBackend130::lease.Owns());assert(VirtualProtect(reinterpret_cast<void*>(ff+fusion_backend130::VehicleRva),1,PAGE_READWRITE,&protection130));failed=true;FusionBackend130::Boundary();assert(Tuple130()==tuples[0]);
 // Invalid pointer/mode never gains admission or calls a foreign original.
 Reset130(1);assert(FusionBackend130::Commit());addonActive=true;int32_t invalid130=9;FusionBackend130::Setting(nullptr,nullptr,&invalid130);assert(!FusionBackend130::lease.Owns());FusionBackend130::Boundary();assert(Tuple130()==tuples[1]);
 std::puts("PASS130 actual x86 FF pointer/RET4 bytes, production callback/admission/deferred/failure boundary, Off zero OS reads, guard/noaccess and partial-call restoration");
 for(auto* allocated130:allocations130)VirtualFree(allocated130,0,MEM_RELEASE);VirtualFree(reinterpret_cast<void*>(game),0,MEM_RELEASE);
 VirtualFree(machine,0,MEM_RELEASE);VirtualFree(reinterpret_cast<void*>(ff),0,MEM_RELEASE);
 return 0; // MSVC inline-asm main must not inherit VirtualFree's EAX=1.
}

