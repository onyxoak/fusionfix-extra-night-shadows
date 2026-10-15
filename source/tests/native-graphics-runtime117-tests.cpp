#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <intrin.h>
#include <xmmintrin.h>
#include "NativeGraphicsMenu117.hpp"
#include <cassert>
#include <filesystem>
#include <functional>
#include <iostream>
#include <type_traits>
#include <vector>

namespace fixture117 {
constexpr DWORD EntryError=0x61700001,HelperError=0x61700002,OriginalError=0x61700003,FillError=0x61700004;
alignas(16) unsigned char entryFp[512],originalFp[512],afterFp[512];
uint32_t resultBits{},beforeEsp{},afterEsp{},beforeEbp{},afterEbp{},afterEbx{},afterEsi{},afterEdi{},afterError{};
unsigned fillCalls{},saveCalls{};int32_t filledMenu=-1;bool saveGood=true;
preset117::State savedState{};
enum class Role:unsigned {Scroll,Select,Process,Value,Label};
struct Call {Role role{};std::vector<uint32_t> arguments;DWORD incomingError{};unsigned incomingMxcsr{};};
std::vector<Call> calls;
std::function<void()> processBody;
bool alterScreenOnFill{};
void Environment(unsigned round,DWORD error) noexcept {
 unsigned mx=_mm_getcsr();mx=(mx&~0x6000u)|(round&0x6000u);_mm_setcsr(mx);
 // Real x87 control-word state, independent of the SSE control register.
 unsigned short cw{};__asm fnstcw cw
 cw=static_cast<unsigned short>((cw&~0x0c00u)|((round>>3)&0x0c00u));__asm fldcw cw
 SetLastError(error);
}
void Entry() noexcept {
 __asm {fninit}
 __asm {fld1}
 __asm {fldpi}
 __asm {fldl2e}
 Environment(0,EntryError);_fxsave(entryFp);
}
void Helper() noexcept {
 __asm {fninit}
 __asm {fldln2}
 Environment(0x6000,HelperError);
}
void Original() noexcept {
 __asm {fninit}
 __asm {fld1}
 __asm {fldl2t}
 Environment(0x4000,OriginalError);_fxsave(originalFp);
}
template<class T> uint32_t Bits(T value) noexcept {if constexpr(std::is_pointer_v<T>)return uint32_t(reinterpret_cast<uintptr_t>(value));else return uint32_t(value);}
struct Hook {
 Role role{};
 template<class R,class... Args> R unsafe_ccall(Args... args) {
  const auto error=GetLastError();const auto mxcsr=_mm_getcsr();calls.push_back({role,{Bits(args)...},error,mxcsr});
  if(role==Role::Process&&processBody)processBody();Original();
  if constexpr(std::is_pointer_v<R>){return reinterpret_cast<R>(uintptr_t(role==Role::Value?0x17601000:0x17602000));}
  else return R(0xa0u+unsigned(role));
 }
};
int32_t __cdecl Fill(int32_t menu) noexcept {++fillCalls;filledMenu=menu;Environment(0x6000,FillError);return 0x177;
}
// Capture stack, callee-saved GPRs, return and FP state immediately at the
// actual cdecl callback boundary; assertion machinery runs afterward.
__declspec(naked) void Probe(uint32_t,const uint32_t*,unsigned) {
 __asm {
  push ebp
  mov ebp,esp
  push ebx
  push esi
  push edi
  mov beforeEbp,ebp
  mov beforeEsp,esp
  mov ebx,061700011h
  mov esi,061700022h
  mov edi,061700033h
  mov eax,[ebp+12]
  mov edx,[ebp+16]
 args_loop:
  test edx,edx
  jz do_call
  dec edx
  push dword ptr [eax+edx*4]
  jmp args_loop
 do_call:
  mov ecx,[ebp+8]
  call ecx
  mov resultBits,eax
  fxsave afterFp
  mov afterEbp,ebp
  mov afterEbx,ebx
  mov afterEsi,esi
  mov afterEdi,edi
  mov edx,[ebp+16]
  shl edx,2
  add esp,edx
  mov afterEsp,esp
  call GetLastError
  mov afterError,eax
  pop edi
  pop esi
  pop ebx
  pop ebp
  ret
 }
}
void Abi() {assert(beforeEsp==afterEsp&&beforeEbp==afterEbp&&afterEbx==0x61700011&&afterEsi==0x61700022&&afterEdi==0x61700033);}
void Fp(const unsigned char* expected,DWORD error) {
 Abi();assert(afterError==error);
 // x87 control/status/tag and MXCSR. Instruction/data pointers differ when
 // ordinary host assertion code runs; do not call them a renderer witness.
 assert(!std::memcmp(afterFp,expected,5));assert(!std::memcmp(afterFp+24,expected+24,4));
 for(unsigned n=0;n<8;++n)assert(!std::memcmp(afterFp+32+n*16,expected+32+n*16,10));
 if(std::memcmp(afterFp+160,expected+160,128)){
  std::cerr<<"FP register mismatch: calls="<<calls.size()<<" last_role="<<(calls.empty()?99u:unsigned(calls.back().role))<<" return="<<resultBits<<" error="<<afterError<<"\n";
  for(unsigned i=0;i<8;++i)if(std::memcmp(afterFp+160+i*16,expected+160+i*16,16))std::cerr<<"XMM"<<i<<" changed\n";
  assert(false);
 }
}
template<class Function> void Invoke(Function function,std::initializer_list<uint32_t> arguments) {
 std::vector<uint32_t> args(arguments);Entry();Probe(uint32_t(reinterpret_cast<uintptr_t>(function)),args.data(),unsigned(args.size()));
}
}
namespace preset117 {
State applied{};bool backendLimited{},vehicleDepthUnavailable119{},backendReadFailed119{};unsigned refreshCalls119{};
void RefreshApplied119() noexcept {++refreshCalls119;fixture117::Helper();}
bool Save(const std::filesystem::path&,const State& state) noexcept {++fixture117::saveCalls;fixture117::savedState=state;fixture117::Helper();return fixture117::saveGood;}
}
namespace native_ui117 {
using namespace native_graphics117;
#define NATIVE117_EXTRACT_MEMORY
#include <native-graphics-runtime117-extracted.inc>
#undef NATIVE117_EXTRACT_MEMORY
struct Memory:RuntimeMemory {
 unsigned reads{},writes{};
 bool Read(uint32_t at,void* out,size_t n) noexcept {++reads;const bool result=RuntimeMemory::Read(at,out,n);fixture117::Helper();return result;}
 bool Write(uint32_t at,const void* in,size_t n) noexcept {++writes;const bool result=RuntimeMemory::Write(at,in,n);fixture117::Helper();return result;}
};
fixture117::Hook scrollHook{fixture117::Role::Scroll},selectHook{fixture117::Role::Select},processHook{fixture117::Role::Process},valueHook{fixture117::Role::Value},labelHook{fixture117::Role::Label};
std::atomic<bool> active{false};std::optional<Controller> controller;std::filesystem::path ini;Memory memory;
uint32_t game{},screenScalar{},screenTables{},fillEntry{},appendEntry{};
DWORD frontendThread{};uint64_t generationSerial{};
std::array<Receipt,2> receipts{};std::array<std::array<wchar_t,66>,ControlCount> values{};
thread_local InputRouter input;bool publishing{},blocked{};thread_local unsigned processDepth{};
using Fill=int32_t(__cdecl*)(int32_t);using Append=uint32_t(__thiscall*)(void*,int32_t);
#define NATIVE117_EXTRACT_CALLBACKS
#include <native-graphics-runtime117-extracted.inc>
#undef NATIVE117_EXTRACT_CALLBACKS
}

namespace fixture117 {
using namespace native_ui117;
uint8_t* allocation{};
const auto high=*preset117::Defaults(preset117::Preset::High);
constexpr uint32_t OwnRow(Control control) noexcept {assert(control==Control::Preset);return 15;}
void Reset() {
 if(!allocation)allocation=static_cast<uint8_t*>(VirtualAlloc(nullptr,0x3000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(allocation);
 DWORD old{};assert(VirtualProtect(allocation,0x3000,PAGE_READWRITE,&old));std::memset(allocation,0,0x3000);
 screenTables=uint32_t(reinterpret_cast<uintptr_t>(allocation));screenScalar=screenTables+0xf00;
 *reinterpret_cast<int32_t*>(allocation+0xf00)=49;
 for(unsigned slot=0;slot<2;++slot){const auto screen=slot?62:49;auto* descriptor=reinterpret_cast<Screen32*>(allocation+screen*sizeof(Screen32));std::memcpy(descriptor->header,"MH_GFX",6);
  descriptor->options={screenTables+0x1000+slot*0x1000,uint16_t(26),48};auto* rows=reinterpret_cast<Option*>(uintptr_t(descriptor->options.data));
  for(unsigned row=0;row<25;++row){rows[row].action=1;rows[row].label[0]='X';rows[row].preference=int16_t(row);}
  for(size_t n=0;n<PublishedControlCount121;++n)rows[15+n]=Row(PublishedControl121(n));rows[25].action=46;
  std::memset(rows[14].label,0,16);std::memcpy(rows[14].label,"Antialiasing",12);
  std::memset(rows[16].label,0,16);std::memcpy(rows[16].label,"Volumetric Fog",14);
  receipts[slot]={screenTables+uint32_t(screen)*sizeof(Screen32),1,descriptor->options,15,true};
 }
 preset117::applied=high;preset117::backendLimited=preset117::vehicleDepthUnavailable119=preset117::backendReadFailed119=false;preset117::refreshCalls119=0;
 controller.emplace(high);active=true;blocked=false;frontendThread=GetCurrentThreadId();input=InputRouter{};processDepth=0;generationSerial=1;
 fillEntry=uint32_t(reinterpret_cast<uintptr_t>(&Fill));fillCalls=saveCalls=0;filledMenu=-1;saveGood=true;calls.clear();processBody={};memory.reads=memory.writes=0;UpdateValues();Entry();
}
template<class R> R CheckArgumentReturn(Role role,unsigned callsBefore) {
 assert(calls.size()==callsBefore+1&&calls.back().role==role);Fp(originalFp,OriginalError);assert(calls.back().incomingError==EntryError);
 unsigned entryMxcsr{};std::memcpy(&entryMxcsr,entryFp+24,sizeof(entryMxcsr));assert(calls.back().incomingMxcsr==entryMxcsr);
 if constexpr(std::is_pointer_v<R>)return reinterpret_cast<R>(uintptr_t(resultBits));else {assert(uint8_t(resultBits)==uint8_t(0xa0u+unsigned(role)));return R(resultBits);}
}
}

int main() {
 using namespace native_ui117;using namespace fixture117;
 Reset();uint8_t adjusted=0x5a;
 Invoke(&Scroll,{0,0xffffffffu,0xfffffffeu,uint32_t(reinterpret_cast<uintptr_t>(&adjusted))});CheckArgumentReturn<uint8_t>(Role::Scroll,0);
 assert(calls.back().arguments==std::vector<uint32_t>({0,0xffffffffu,0xfffffffeu,uint32_t(reinterpret_cast<uintptr_t>(&adjusted))})&&adjusted==0x5a);
 Invoke(&Select,{0,0,0xe7});CheckArgumentReturn<uint8_t>(Role::Select,1);assert(calls.back().arguments==std::vector<uint32_t>({0,0,0xe7}));
 Invoke(&Value,{0,49,2,15});assert(CheckArgumentReturn<wchar_t*>(Role::Value,2)==reinterpret_cast<wchar_t*>(uintptr_t(0x17601000)));
 const char foreign[]="FOREIGN";wchar_t text[66]{};
 Invoke(&Label,{uint32_t(reinterpret_cast<uintptr_t>(foreign)),uint32_t(reinterpret_cast<uintptr_t>(text))});assert(CheckArgumentReturn<const char*>(Role::Label,3)==reinterpret_cast<const char*>(uintptr_t(0x17602000)));
 // Owned rendering routes no original call and returns stable addon strings.
 Reset();Invoke(&Value,{0,49,1,15});assert(calls.empty()&&reinterpret_cast<wchar_t*>(uintptr_t(resultBits))==values[size_t(Control::Preset)].data());Fp(entryFp,EntryError);
 const auto& receipt=receipts[0];const auto* ownLabel=reinterpret_cast<const char*>(uintptr_t(receipt.array.data+15*sizeof(Option)+offsetof(Option,label)));
 Invoke(&Label,{uint32_t(reinterpret_cast<uintptr_t>(ownLabel)),uint32_t(reinterpret_cast<uintptr_t>(text))});assert(calls.empty()&&resultBits==0&&std::wstring(text)==Labels[size_t(Control::Preset)]);Fp(entryFp,EntryError);
 // No process context: own-looking inputs still run the existing chain once.
 Invoke(&Scroll,{15,uint32_t(-92),1,uint32_t(reinterpret_cast<uintptr_t>(&adjusted))});CheckArgumentReturn<uint8_t>(Role::Scroll,0);
 // Active production Process wraps actual owned Scroll/Select events.
 Reset();adjusted=0;
 processBody=[&]{assert(Scroll(int32_t(OwnRow(Control::Preset)),-92,-1,&adjusted)==0);};
 Invoke(&Process,{0});CheckArgumentReturn<uint8_t>(Role::Process,0);assert(controller->Pending().preset==preset117::Preset::Medium&&controller->Applied()==high&&controller->Saved()&&saveCalls==1&&fillCalls==1&&filledMenu==0&&adjusted==0);
 Reset();processBody=[&]{const auto row=int32_t(OwnRow(Control::Preset));assert(Scroll(row,row,1,&adjusted)==0);assert(Select(0,row,0)==0);};
 Invoke(&Process,{0});CheckArgumentReturn<uint8_t>(Role::Process,0);assert(!controller->Pending().enabled&&controller->Applied()==high&&controller->RestartRequired()&&controller->Saved()&&saveCalls==1&&fillCalls==1);
 processBody=[&]{assert(Scroll(15,-92,1,&adjusted)==0);};
 Invoke(&Process,{0});CheckArgumentReturn<uint8_t>(Role::Process,1);assert(controller->Pending()==*preset117::Defaults(preset117::Preset::Low)&&saveCalls==2&&controller->Saved()&&controller->Applied()==high&&fillCalls==2);
 Reset();saveGood=false;processBody=[&]{assert(Scroll(15,-92,-1,&adjusted)==0);};
 Invoke(&Process,{0});CheckArgumentReturn<uint8_t>(Role::Process,0);assert(saveCalls==1&&controller->SaveFailed()&&controller->Applied()==high&&fillCalls==1&&controller->Pending()==savedState&&controller->QualityValue121()==L"Medium (save failed)");
 Reset();processBody=[&]{assert(Select(0,15,0)==0);};Invoke(&Process,{0});CheckArgumentReturn<uint8_t>(Role::Process,0);assert(!saveCalls&&!fillCalls&&controller->Pending()==high);
 Reset();processBody=[&]{assert(Scroll(15,-92,1,&adjusted)==0);*reinterpret_cast<int32_t*>(allocation+0xf00)=62;};
 Invoke(&Process,{0});CheckArgumentReturn<uint8_t>(Role::Process,0);assert(fillCalls==0);
 Reset();processBody=[&]{assert(Scroll(15,-92,1,&adjusted)==0);++receipts[0].generation;};
 Invoke(&Process,{0});CheckArgumentReturn<uint8_t>(Role::Process,0);assert(fillCalls==0);
 Reset();frontendThread=GetCurrentThreadId()+1;Invoke(&Value,{0,49,1,15});CheckArgumentReturn<wchar_t*>(Role::Value,0);assert(controller->Applied()==high);
 Reset();active=false;const auto reads=memory.reads;Invoke(&Process,{0});CheckArgumentReturn<uint8_t>(Role::Process,0);assert(memory.reads==reads&&fillCalls==0);
 // An ownedstatus refresh observes the backend without losing pending edits,
 // save state, stabletext ownership or callerFP/LastError.
 Reset();controller->Dispatch(Control::WorldLighting,Action::Activate);const auto pending119=controller->Pending();const auto refresh119=preset117::refreshCalls119;
 preset117::backendLimited=preset117::vehicleDepthUnavailable119=true;preset117::applied.features[0]=false;
 Invoke(&Value,{0,49,1,OwnRow(Control::Preset)});assert(calls.empty()&&std::wstring(reinterpret_cast<wchar_t*>(uintptr_t(resultBits)))==L"Custom (enable FF vehicle shadows)"&&preset117::refreshCalls119==refresh119+1&&controller->Pending()==pending119&&!controller->Saved());Fp(entryFp,EntryError);
 // Real PAGE_GUARD/NOACCESS must remain untouched when owned proof refuses.
 Reset();DWORD prior{};assert(VirtualProtect(allocation+0x1000,0x1000,PAGE_READWRITE|PAGE_GUARD,&prior));
 Invoke(&Value,{0,49,1,15});CheckArgumentReturn<wchar_t*>(Role::Value,0);MEMORY_BASIC_INFORMATION info{};assert(VirtualQuery(allocation+0x1000,&info,sizeof(info))&&info.Protect&PAGE_GUARD);
 Reset();assert(VirtualProtect(allocation+0x1000,0x1000,PAGE_NOACCESS,&prior));Invoke(&Value,{0,49,1,15});CheckArgumentReturn<wchar_t*>(Role::Value,0);
 Reset();auto* row=reinterpret_cast<Option*>(uintptr_t(receipts[0].array.data))+15;row->padding=1;Invoke(&Value,{0,49,1,15});CheckArgumentReturn<wchar_t*>(Role::Value,0);
 Reset();unsigned level=0;processBody=[&]{if(level++==0){Process(0);}else {assert(Scroll(int32_t(OwnRow(Control::Preset)),-92,-1,&adjusted)==0);}--level;};
 Invoke(&Process,{0});assert(calls.size()==2&&calls[0].role==Role::Process&&calls[1].role==Role::Process);Fp(originalFp,OriginalError);assert(fillCalls==1&&controller->Pending().preset==preset117::Preset::Medium);
 assert(VirtualFree(allocation,0,MEM_RELEASE));allocation=nullptr;
 std::cout<<"native graphics117 actual extracted callbacks: cdecl/returns/ESP/callee GPR/FP/LastError/routing/refresh/guard/source/save tests PASS; no live UI or FF runtime claim\n";
 return 0;
}
