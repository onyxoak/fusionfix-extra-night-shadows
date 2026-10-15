#include <windows.h>
#include "TrafficPoolHookGuard.hpp"
#include "FusionHooks213.hpp"
#include "native-pool71.hpp"
#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace fixture {
using traffic_signal::Word;
constexpr uintptr_t game=0x400000,ff=0x10000000,relay=0x300000,stub=0x301000,trampoline=0x302000;
std::map<uint32_t,std::vector<uint8_t>> memory;
std::vector<uint8_t> ffImage(0xa0000);
unsigned reads{},moduleReads{},fileReads{},queries{};
bool modulePresent{},executable=true;
std::string event,reason;
void Put(std::vector<uint8_t>& b,size_t at,uint32_t n){std::memcpy(b.data()+at,&n,4);}
std::vector<uint8_t> Jump(uint32_t at,uint32_t to){std::vector<uint8_t>b(5);b[0]=0xe9;Put(b,1,to-at-5);return b;}
HMODULE Module(const wchar_t*){++moduleReads;return modulePresent?reinterpret_cast<HMODULE>(ff):nullptr;}
size_t Query(const void* address,MEMORY_BASIC_INFORMATION* info,size_t){
 ++queries;info->State=MEM_COMMIT;info->Protect=executable?PAGE_EXECUTE_READ:PAGE_NOACCESS;
 info->BaseAddress=const_cast<void*>(address);info->RegionSize=0x100000;return sizeof(*info);
}
}
namespace test_runtime {
namespace pe {
struct Image {std::vector<uint8_t> bytes;Image()=default;Image(const std::vector<uint8_t>& input,uintptr_t):bytes(input){}};
std::string ModulePath(HMODULE){return "fixture-module";}
std::vector<uint8_t> Read(const std::string&){++fixture::fileReads;return fixture::ffImage;}
std::string Hash(const std::vector<uint8_t>&){return "fixture-hash-not-runtime-identity";}
}
uintptr_t game=fixture::game,ff=fixture::ff;
bool nativeBackend=true;
bool fusionCallbacks213Authenticated=true;
const auto* fusionHooks213=fusion_hooks213::Find("5.1.1");
struct Layout213 {const char* version="5.1.1";} layout213;
const auto* layout=&layout213;
std::string CallbackOwner213(uint32_t){return "callback_module=fixture";}
void RecordStartupExtra213(std::string_view){}
std::unique_ptr<pe::Image> original=std::make_unique<pe::Image>();
namespace TrafficSignal {bool requested=true,authenticated=false;}
namespace ModelLightShadow20 {bool requested=false;}
namespace CoronaLight20 {bool requested=false;}
void Require(bool value,const char* reason){traffic_signal::Need(value,reason);}
void Event(unsigned,const char* name,const std::string& fields){fixture::event=name;fixture::reason=fields;}
std::vector<uint8_t> ReadMemory(uintptr_t address,size_t size){
 ++fixture::reads;const auto found=fixture::memory.find(uint32_t(address));
 if(found==fixture::memory.end()||found->second.size()<size)throw std::runtime_error("fixture_unreadable");
 return {found->second.begin(),found->second.begin()+size};
}
#define GetModuleHandleW fixture::Module
#define VirtualQuery fixture::Query
#include "traffic-auth71-extracted.inc"
#undef GetModuleHandleW
#undef VirtualQuery
}
void Reset(bool native=true){
 using namespace fixture;using namespace traffic_signal;
 memory.clear();event.clear();reason.clear();reads=moduleReads=fileReads=queries=0;
 executable=true;modulePresent=!native;test_runtime::nativeBackend=native;
 test_runtime::TrafficSignal::requested=true;test_runtime::TrafficSignal::authenticated=false;
 auto& bytes=test_runtime::original->bytes;bytes.assign(0x930000,0xcc);
 std::copy(NativePool71.begin(),NativePool71.end(),bytes.begin()+PoolCtorRva);
 memory[uint32_t(game+PoolCtorRva)]={NativePool71.begin(),NativePool71.end()};
 for(const auto [rva,size]:{std::pair<uint32_t,size_t>{0x9208f0,0x747},{0x921040,0x197},{0x70ba00,0x22},{0x60d370,0x32},{0x714fc0,0x35},
   {0x6bcf33,0x70},{0x6bd2c0,0x73},{0x6c2dd0,0x98},{0x4d0aa0,0xff},{0x4d0bc0,0xf2}})
  memory[uint32_t(game+rva)]={bytes.begin()+rva,bytes.begin()+rva+size};
 if(native)return;
 auto live=memory[uint32_t(game+PoolCtorRva)];auto jump=Jump(uint32_t(game+PoolHookRva),relay);
 std::copy(jump.begin(),jump.end(),live.begin()+10);memory[uint32_t(game+PoolCtorRva)]=live;
 memory[relay]=Jump(relay,stub);
 auto code=std::vector<uint8_t>(onyxoak::MidStubTemplate.begin(),onyxoak::MidStubTemplate.end());
 Put(code,2,stub+167);Put(code,89,stub+163);Put(code,163,uint32_t(ff+PoolCallbackRva));Put(code,167,trampoline);memory[stub]=code;
 const auto callback=Jump(uint32_t(ff+PoolCallbackRva),uint32_t(ff+PoolCallbackBodyRva));
 memory[uint32_t(ff+PoolCallbackRva)]=callback;std::copy(callback.begin(),callback.end(),ffImage.begin()+PoolCallbackRva);
 const auto body=std::vector<uint8_t>(0xae,0xcc);memory[uint32_t(ff+PoolCallbackBodyRva)]=body;std::copy(body.begin(),body.end(),ffImage.begin()+PoolCallbackBodyRva);
 auto continuation=std::vector<uint8_t>(10,0x90);jump=Jump(trampoline+5,uint32_t(game+PoolHookRva+5));std::copy(jump.begin(),jump.end(),continuation.begin()+5);memory[trampoline]=continuation;
}
int main(){
 using namespace fixture;using namespace test_runtime;using namespace traffic_signal;
 Reset();const auto initial=memory;TrafficAuthenticate();
 assert(TrafficSignal::authenticated&&event=="TRAFFIC_NATIVE_POOL_AUTHENTICATED");
 assert(moduleReads==0&&fileReads==0&&reads==11&&queries==1&&memory==initial);
 for(size_t i=0;i<102;++i){Reset();memory[uint32_t(fixture::game+PoolCtorRva)][i]^=1;TrafficAuthenticate();
  assert(!TrafficSignal::authenticated&&event=="TRAFFIC_OBSERVER_REFUSED"&&reason=="traffic_native_pool_ctor_changed");}
 unsigned lifecycleMutations=0;
 Reset();std::vector<std::pair<uint32_t,size_t>> ranges;for(const auto& [address,bytes]:memory)
  if(address!=uint32_t(fixture::game+PoolCtorRva))ranges.emplace_back(address,bytes.size());
 for(const auto [address,size]:ranges)for(size_t i=0;i<size;++i){Reset();memory[address][i]^=1;TrafficAuthenticate();
  assert(!TrafficSignal::authenticated&&event=="TRAFFIC_OBSERVER_REFUSED"&&reason=="traffic_native_lifecycle_bytes_changed");++lifecycleMutations;}
 Reset();executable=false;TrafficAuthenticate();assert(!TrafficSignal::authenticated&&reason=="traffic_pool_code_not_executable");
 Reset();memory.erase(uint32_t(fixture::game+PoolCtorRva));TrafficAuthenticate();assert(!TrafficSignal::authenticated&&reason=="fixture_unreadable");
 Reset();test_runtime::original->bytes[PoolCtorRva+10]^=1;TrafficAuthenticate();assert(!TrafficSignal::authenticated&&reason=="traffic_native_pool_original_instructions_changed");
 Reset();TrafficSignal::requested=false;TrafficAuthenticate();assert(reads==0&&queries==0&&moduleReads==0&&fileReads==0);
 Reset(false);TrafficAuthenticate();assert(TrafficSignal::authenticated&&event=="TRAFFIC_POOL_HOOK_AUTHENTICATED"&&moduleReads==1&&memory[uint32_t(fixture::game+PoolCtorRva)][10]==0xe9);
 Reset(false);modulePresent=false;TrafficAuthenticate();assert(!TrafficSignal::authenticated&&reason=="traffic_pool_FusionFix_module_changed");
 std::cout<<"PASS actual extracted TrafficAuthenticate native exact ctor, "<<lifecycleMutations<<" lifecycle byte faults, 102 ctor byte faults, no FF reads, no memory writes, OFF, permission/unreadable/original faults, FF overlay parity\n";
}
