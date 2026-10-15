#include "NativeGraphicsMenu117.hpp"
#include <map>
#include <vector>
#include <cassert>
#include <cstdio>
namespace n=native_night128;
namespace g=shortlist_helper48;
std::map<uintptr_t,std::vector<uint8_t>> memory;
unsigned reads{},codeReads{};bool changePointer{};
constexpr uintptr_t tls=0x200000,block=0x210000,quality=0x340000;
const g::Binding binding{0x440000,0x56d00000,0x1be6400,0x400000,true};
template<class T>void Put(uintptr_t at,T value){auto& bytes=memory[at];bytes.resize(sizeof(value));std::memcpy(bytes.data(),&value,sizeof(value));}
bool Read(uintptr_t at,void* out,size_t size) noexcept {
 ++reads;if(at==binding.game+n::NativeGetterRva||at==binding.fusion+g::FusionReadRva)++codeReads;
 for(auto& [start,data]:memory)if(at>=start&&at-start<=data.size()&&size<=data.size()-(at-start)){
  std::memcpy(out,data.data()+at-start,size);
  if(changePointer&&at==binding.game+quality){Put<uint32_t>(binding.fusion+n::QualityPointerRva,uint32_t(binding.game+quality+4));changePointer=false;}
  return true;
 }return false;
}
void Jump(uintptr_t at,uintptr_t destination){memory[at].resize(6);memory[at][0]=0xe9;
 const auto delta=int32_t(destination-at-5);std::memcpy(memory[at].data()+1,&delta,4);memory[at][5]=0x90;}
std::array<uint8_t,n::NativeGetterBytes> Setup(){
 memory.clear();reads=codeReads=0;changePointer=false;
 std::array<uint8_t,n::NativeGetterBytes> original{};original[0]=0x8b;original[1]=0x0d;
 original[6]=0x85;original[7]=0xc9;original[39]=0xc3;
 memory[binding.game+n::NativeGetterRva]={original.begin(),original.end()};
 std::array<uint8_t,6> jump{};const auto relay=0x310000u;Jump(relay,binding.fusion+g::FusionReadRva);
 jump[0]=0xe9;jump[5]=0x90;const auto delta=int32_t(relay-(binding.game+n::NativeGetterRva)-5);std::memcpy(jump.data()+1,&delta,4);
 std::memcpy(memory[binding.game+n::NativeGetterRva].data(),jump.data(),6);
 memory[binding.fusion+g::FusionReadRva]={std::begin(g::FusionReadBytes),std::end(g::FusionReadBytes)};
 for(const auto offset:g::FusionReadRelocs){uint32_t value{};auto* p=memory[binding.fusion+g::FusionReadRva].data()+offset;
  std::memcpy(&value,p,4);value=uint32_t(value-0x56d00000+binding.fusion);std::memcpy(p,&value,4);}
 Put<uint32_t>(binding.fusion+n::TlsIndexRva,3);Put<uint32_t>(tls+12,block);
 Put<int32_t>(binding.fusion+n::EpochRva,INT32_MIN+4);Put<int32_t>(block+0x10,INT32_MIN+8);
 Put<uint32_t>(binding.fusion+n::QualityPointerRva,uint32_t(binding.game+quality));Put<uint32_t>(binding.game+quality,0);
 return original;
}
int main(){
 auto original=Setup();auto route=n::Authenticate(binding,original,Read);assert(route.authenticated);
 codeReads=0;for(uint32_t value=0;value<=3;++value){Put(binding.game+quality,value);const auto result=n::Observe(route,Read,tls);
  assert(result.state==(value?n::State::Enabled:n::State::Off)&&result.quality==int32_t(value));}
 assert(!codeReads); // Full code authentication is not repeated on UI refresh.
 original=Setup();auto wrong=binding;wrong.exact511Authenticated=false;assert(!n::Authenticate(wrong,original,Read).authenticated);
 for(unsigned byte:{6u,39u}){original=Setup();memory[binding.game+n::NativeGetterRva][byte]^=1;assert(!n::Authenticate(binding,original,Read).authenticated);}
 original=Setup();memory[binding.fusion+g::FusionReadRva].back()^=1;assert(!n::Authenticate(binding,original,Read).authenticated);
 original=Setup();Jump(0x310000,binding.fusion+g::FusionReadRva+1);assert(!n::Authenticate(binding,original,Read).authenticated);
 original=Setup();route=n::Authenticate(binding,original,Read);Put<int32_t>(binding.fusion+n::EpochRva,-1);assert(n::Observe(route,Read,tls).reason==n::Reason::Initializing);
 Put<int32_t>(binding.fusion+n::EpochRva,INT32_MIN+9);assert(n::Observe(route,Read,tls).reason==n::Reason::Initializing);
 original=Setup();route=n::Authenticate(binding,original,Read);Put<uint32_t>(binding.fusion+n::QualityPointerRva,0x100000);assert(n::Observe(route,Read,tls).reason==n::Reason::Pointer);
 original=Setup();route=n::Authenticate(binding,original,Read);Put<uint32_t>(binding.game+quality,4);assert(n::Observe(route,Read,tls).state==n::State::Unknown);
 original=Setup();route=n::Authenticate(binding,original,Read);memory.erase(binding.game+quality);assert(n::Observe(route,Read,tls).reason==n::Reason::ReadFault);
 original=Setup();route=n::Authenticate(binding,original,Read);changePointer=true;assert(n::Observe(route,Read,tls).reason==n::Reason::Changed);
 assert(n::Observe({},Read,tls).state==n::State::Unknown);
 const auto high=*preset117::Defaults(preset117::Preset::High);
 native_graphics117::Controller controller(high);assert(controller.Pending()==high);
 assert(controller.QualityValue121()==L"High (Night state unknown)");
 controller.ObserveNight128(n::State::Off);assert(controller.QualityValue121()==L"High (Night Shadows Off)");
 assert(controller.Value(native_graphics117::Control::Status)==L"Enable GTA IV Night Shadows in Graphics");
 assert(controller.Pending()==high&&controller.Applied()==high);
 controller.ObserveNight128(n::State::Enabled);assert(controller.QualityValue121()==L"High"&&controller.Value(native_graphics117::Control::Status)==L"Applied");
 const auto low=*preset117::Defaults(preset117::Preset::Low);native_graphics117::Controller restart(low,high,false);
 restart.ObserveNight128(n::State::Off);assert(restart.QualityValue121()==L"High (Night Shadows Off; restart required)");
 assert(restart.Pending()==high&&restart.Applied()==low&&restart.RestartRequired());
 auto off=high;off.enabled=false;native_graphics117::Controller disabled(off);
 disabled.ObserveNight128(n::State::Off);assert(disabled.QualityValue121()==L"Off");
 std::puts("PASS128 full getter ownership/endpoint guards, fresh Off/on/unknown/init/race states, no repeated code reads, High and restart request preserved");
}
