#include "EnginePlatform117.hpp"
#include "NativeGraphicsMenu117.hpp"
#include "MenuChainGuard117.hpp"
#include "MenuSnapshot117.hpp"
// Opt-in experimental frontend integration. No UI failure disables lighting.
namespace native_ui117 {
using namespace native_graphics117;
namespace image117 {
#include "standalone-pe.hpp"
}
struct Preserve {
 alignas(16) unsigned char fp[512];DWORD error=GetLastError();
 Preserve() noexcept {_fxsave(fp);} ~Preserve(){_fxrstor(fp);SetLastError(error);}
};
struct Memory {
 static bool Permissions(uint32_t address,size_t bytes,bool write=false,bool execute=false) noexcept {
  if(!Range(address,bytes))return false;const uint64_t end=uint64_t(address)+bytes;uint64_t at=address;
  while(at<end){MEMORY_BASIC_INFORMATION r{};
   if(!VirtualQuery(reinterpret_cast<void*>(uintptr_t(at)),&r,sizeof(r))||r.State!=MEM_COMMIT||(r.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;
   const auto p=r.Protect&0xff;
   const bool read=p==PAGE_READONLY||p==PAGE_READWRITE||p==PAGE_WRITECOPY||p==PAGE_EXECUTE_READ||p==PAGE_EXECUTE_READWRITE||p==PAGE_EXECUTE_WRITECOPY;
   if(!read||(write&&p!=PAGE_READWRITE&&p!=PAGE_WRITECOPY&&p!=PAGE_EXECUTE_READWRITE&&p!=PAGE_EXECUTE_WRITECOPY)||
      (execute&&p!=PAGE_EXECUTE_READ&&p!=PAGE_EXECUTE_READWRITE&&p!=PAGE_EXECUTE_WRITECOPY))return false;
   const uint64_t next=uint64_t(reinterpret_cast<uintptr_t>(r.BaseAddress))+r.RegionSize;if(next<=at)return false;at=next;
  }return true;
 }
 bool Copy(uint32_t at,void* out,size_t n) noexcept{return Read(at,out,n);}
 bool Executable(uint32_t at,size_t n) noexcept{return Permissions(at,n,false,true);}
 bool Read(uint32_t at,void* out,size_t n) noexcept {
  if(!out||!Permissions(at,n))return false;
  __try{std::memcpy(out,reinterpret_cast<void*>(uintptr_t(at)),n);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
 }
 bool Write(uint32_t at,const void* input,size_t n) noexcept {
  if(!input||!Permissions(at,n,true))return false;
  __try{std::memcpy(reinterpret_cast<void*>(uintptr_t(at)),input,n);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
 }
};
SafetyHookMid fillBoundary;
SafetyHookInline scrollHook,selectHook,processHook,valueHook,labelHook;
std::atomic<bool> active{false};std::once_flag once;
std::optional<Controller> controller;std::filesystem::path ini;Memory memory;
uint32_t game{},screenScalar{},screenTables{},fillEntry{},appendEntry{};
DWORD frontendThread{};uint64_t generationSerial{};
std::array<Receipt,2> receipts{};std::array<std::array<wchar_t,66>,ControlCount> values{};
std::array<Snapshot,2> snapshots{};
thread_local InputRouter input;bool publishing{},blocked{};
thread_local unsigned processDepth{};
using Fill=int32_t(__cdecl*)(int32_t);using Append=uint32_t(__thiscall*)(void*,int32_t);
std::optional<unsigned> Slot(int32_t screen) noexcept {if(screen==49)return 0;if(screen==62)return 1;return {};}
std::optional<int32_t> Current() noexcept {
 int32_t s{};if(!active.load(std::memory_order_acquire)||!memory.Read(screenScalar,&s,4)||!Slot(s))return {};return s;
}
uint32_t Address(int32_t s) noexcept{return screenTables+uint32_t(s)*sizeof(Screen32);}
void UpdateValues() noexcept {
 if(!controller)return;
 preset117::RefreshApplied119();
 controller->ObserveApplied119(preset117::applied,preset117::backendLimited,preset117::vehicleDepthUnavailable119,preset117::backendReadFailed119);
 controller->ObserveNight128(native_night128::currentState.load(std::memory_order_acquire));
 try{for(size_t n=0;n<ControlCount;++n){const auto text=Control(n)==Control::Preset?controller->QualityValue121():controller->Value(Control(n));if(text.size()>=values[n].size())return;
  std::copy(text.begin(),text.end(),values[n].begin());values[n][text.size()]=0;
 }}catch(...){OutputDebugStringW(L"Liberty's Shadows: menu text update refused.\n");}
}
std::optional<Control> Owned(int32_t row,int32_t s) noexcept {
 if(blocked||!controller||GetCurrentThreadId()!=frontendThread||row<0)return {};
 const auto current=Current();const auto slot=Slot(s);if(!current||*current!=s||!slot)return {};
 const auto& receipt=receipts[*slot];return receipt.Own(memory,Address(s),receipt.generation,unsigned(row));
}
uint32_t AppendOwned(uint32_t descriptor,int argument) noexcept {
 if(!Memory::Permissions(descriptor,sizeof(Array32),true))return 0;
 __try{return reinterpret_cast<Append>(uintptr_t(appendEntry))(reinterpret_cast<void*>(uintptr_t(descriptor)),argument);}__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
void OnFill(SafetyHookContext&) noexcept {
 const Preserve preserve;if(blocked||publishing||!active.load(std::memory_order_acquire))return;
 const auto current=Current();if(!current)return;const auto thread=GetCurrentThreadId();if(!frontendThread)frontendThread=thread;if(thread!=frontendThread)return;
 publishing=true;const auto slot=*Slot(*current);auto& previous=receipts[slot];auto& snapshot=snapshots[slot];Screen32 now{};
 if(memory.Read(Address(*current),&now,sizeof(now))&&Graphics(now)&&ArrayValid(now.options)){
  std::array<Option,RowLimit> rows{};
  const bool copied=memory.Read(now.options.data,rows.data(),size_t(now.options.count)*sizeof(Option));
  const bool same=previous.active&&previous.screen==Address(*current)&&previous.array==now.options;
  if(!same&&copied&&snapshot.CanRenew(std::span(rows).first(now.options.count))){
   const auto plan=MakePlan(std::span(rows).first(now.options.count),true);
   previous={Address(*current),++generationSerial,now.options,plan->first,true};snapshot.Capture(std::span(rows).first(now.options.count));UpdateValues();publishing=false;return;
  }
  const auto generation=same?previous.generation:++generationSerial;
  const auto result=Publish(memory,Address(*current),generation,true,same,AppendOwned);
  if(result.result==Publication::Published||result.result==Publication::AlreadyPublished){previous=result.receipt;
   if(memory.Read(previous.array.data,rows.data(),size_t(previous.array.count)*sizeof(Option)))snapshot.Capture(std::span(rows).first(previous.array.count));else {previous={};snapshot.active=false;}
   UpdateValues();}
  else {previous={};snapshot.active=false;if(result.result==Publication::OwnershipLost)blocked=true;
   OutputDebugStringW(L"Liberty's Shadows: Graphics rows refused; lighting remains active.\n");}
 }publishing=false;
}
uint8_t __cdecl Scroll(int32_t row,int32_t accepted,int32_t direction,uint8_t* adjusted) noexcept {
 {const Preserve preserve;const auto current=Current();
  if(current&&controller){const auto& receipt=receipts[*Slot(*current)];const auto result=input.Scroll(*controller,Owned(row,*current),row,accepted,direction,Address(*current),receipt.generation,true);if(result.handled){if(result.change==Change::Changed)controller->Persist([](const preset117::State& pending){return preset117::Save(ini,pending);});return result.nativeReturn;}}}
 return scrollHook.unsafe_ccall<uint8_t>(row,accepted,direction,adjusted);
}
uint8_t __cdecl Select(int32_t menu,int32_t row,uint8_t adjusted) noexcept {
 {const Preserve preserve;const auto current=Current();
  if(current&&controller){const auto& receipt=receipts[*Slot(*current)];const auto result=input.Select(*controller,Owned(row,*current),menu,row,adjusted,Address(*current),receipt.generation,true);
   if(result.handled){if(result.change==Change::SaveRequested||result.change==Change::Changed)controller->Persist([](const preset117::State& pending){return preset117::Save(ini,pending);});return result.nativeReturn;}}}
 return selectHook.unsafe_ccall<uint8_t>(menu,row,adjusted);
}
uint8_t __cdecl Process(int32_t menu) noexcept {
 std::optional<int32_t> current;Receipt receipt{};bool routed=false;
 {const Preserve preserve;current=Current();
  if(current&&controller&&!blocked&&GetCurrentThreadId()==frontendThread){receipt=receipts[*Slot(*current)];routed=bool(Owned(receipt.first,*current));}}
 if(routed){
  bool refresh=false,outer=false;std::optional<InputRouter::Scope> scope;
  {const Preserve preserve;scope.emplace(input,menu,Address(*current),receipt.generation,receipt.active);outer=processDepth++==0;}
  const auto result=processHook.unsafe_ccall<uint8_t>(menu);
  {const Preserve preserve;refresh=scope->NeedsRefresh();scope.reset();--processDepth;
   if(refresh&&outer){const auto after=Current();
   if(after&&*after==*current){const auto& now=receipts[*Slot(*after)];
    if(now.active&&now.generation==receipt.generation&&now.array==receipt.array&&Owned(receipt.first,*after)){UpdateValues();reinterpret_cast<Fill>(uintptr_t(fillEntry))(menu);}}
   }
  }return result;
 }return processHook.unsafe_ccall<uint8_t>(menu);
}
wchar_t* __cdecl Value(int32_t menu,int32_t screen,int32_t mode,int32_t row) noexcept {
 {const Preserve preserve;if(mode==1&&menu>=0){const auto owned=Owned(row,screen);if(owned){if(*owned==Control::Status||*owned==Control::Preset)UpdateValues();return values[size_t(*owned)].data();}}}
 return valueHook.unsafe_ccall<wchar_t*>(menu,screen,mode,row);
}
const char* __cdecl Label(const char* label,wchar_t* destination) noexcept {
 {const Preserve preserve;const auto current=Current();
  if(current&&label&&destination&&frontendThread==GetCurrentThreadId()){
   const auto& receipt=receipts[*Slot(*current)];
   for(unsigned n=0;n<PublishedControlCount121;++n){const auto published=PublishedControl121(n);const auto row=receipt.first+n;const auto address=uint64_t(receipt.array.data)+uint64_t(row)*sizeof(Option)+offsetof(Option,label);
    if(address==reinterpret_cast<uintptr_t>(label)&&Owned(int32_t(row),*current)==published){
     const auto text=Labels[size_t(published)];if(text.size()>=66)break;std::array<wchar_t,66> copy{};std::copy(text.begin(),text.end(),copy.begin());
     if(memory.Write(uint32_t(reinterpret_cast<uintptr_t>(destination)),copy.data(),(text.size()+1)*sizeof(wchar_t)))return nullptr;break;
    }
   }
  }}return labelHook.unsafe_ccall<const char*>(label,destination);
}
void Require(bool good,const char* why){if(!good)throw std::runtime_error(why);}
void Refuse() noexcept {
 active=false;for(auto* hook:{&scrollHook,&selectHook,&processHook,&valueHook,&labelHook})if(*hook)(void)hook->disable();if(fillBoundary)(void)fillBoundary.disable();
}
void Prepare(uintptr_t base,const uint8_t* original,size_t bytes,HMODULE addon) {
 Require(base<=UINT32_MAX&&bytes==0x1be6400&&original,"menu_native_image_layout");game=uint32_t(base);
 // The UI has its own verified family; refusal never tightens High's lighting detector.
 const auto raw=image117::onyxoak::pe::Read(image117::onyxoak::pe::ModulePath(nullptr));
 Require(compatibility117::Accept(raw,"08759a5516f9837920ea504436236bbab89d0826a8e4d04ff106345177b5345d"),"menu_CE_adapter_not_verified");
 // Without explicit public settings, legacy flags may differ from High defaults.
 Require(preset117::startup.present&&!preset117::startup.invalid,"menu_applied_settings_snapshot_missing");
 ini=image117::onyxoak::pe::ModulePath(addon).replace_extension(L".ini");controller.emplace(preset117::applied, preset117::startup.state, preset117::backendLimited);UpdateValues();
 auto expected=[&](uint32_t rva,unsigned n){Require(uint64_t(rva)+n<=bytes,"menu_expected_range");return std::span(original+rva,n);};
 uint32_t ffBase{};std::unique_ptr<image117::onyxoak::pe::Image> ffImage;
 if(const auto ff=GetModuleHandleW(L"GTAIV.EFLC.FusionFix.asi")){
  const auto rawFF=image117::onyxoak::pe::Read(image117::onyxoak::pe::ModulePath(ff));
  Require(compatibility117::Hash(rawFF)=="9f8a42d996e239c23c24432af4b95c568bfd1c501644a13bb7e9a4f51ed7be60","menu_FusionFix_adapter_not_verified");
  ffBase=uint32_t(reinterpret_cast<uintptr_t>(ff));ffImage=std::make_unique<image117::onyxoak::pe::Image>(rawFF,ffBase);
 }
 struct Entry {uint32_t nativeRva,ffRva;unsigned patch;};
 constexpr std::array<Entry,4> entries{{{0x4b5e30,0xd1410,6},{0x4bfe90,0xd6e90,5},{0x4c0020,0xd6f80,8},{0x4bae80,0xd6860,6}}};
 for(const auto& entry:entries){const auto foreign=ffImage?std::span(ffImage->bytes).subspan(entry.ffRva,32):std::span<uint8_t>{};
  const auto result=menu_chain117::Authenticate(memory,game+entry.nativeRva,expected(entry.nativeRva,32),ffBase?ffBase+entry.ffRva:0,foreign,entry.patch);
  Require(result.owner==(ffBase?menu_chain117::Owner::FusionFix:menu_chain117::Owner::Native),"menu_entry_owner_not_ready_or_refused");}
 for(const auto rva:{0x4b5e39u,0x4b7fe0u,0x50a3f0u,0x1beed0u}){std::array<uint8_t,32> live{};
  Require(memory.Executable(game+rva,32)&&memory.Read(game+rva,live.data(),32)&&!std::memcmp(live.data(),expected(rva,32).data(),32),"menu_native_site_changed");}
 Require(original[0x4b5e39]==0xa1,"menu_fill_boundary_instruction");uint32_t scalar{};std::memcpy(&scalar,original+0x4b5e3a,4);Require(scalar==game+0xd60c40,"menu_screen_scalar_binding");
 screenScalar=scalar;screenTables=game+0x15d3390;fillEntry=game+0x4b5e30;appendEntry=game+0x1beed0;
 // XML/native arrays need not exist at lighting activation. The authenticated
 // inner Fill boundary validates their actual ready lifetime before writing.
 auto mid=SafetyHookMid::create(reinterpret_cast<void*>(uintptr_t(game+0x4b5e39)),OnFill,SafetyHookMid::StartDisabled);Require(bool(mid),"menu_fill_hook_prepare");fillBoundary=std::move(*mid);
 auto hook=[&](SafetyHookInline& destination,uint32_t rva,auto callback){auto ready=SafetyHookInline::create(reinterpret_cast<void*>(uintptr_t(game+rva)),callback,SafetyHookInline::StartDisabled);Require(bool(ready),"menu_hook_prepare");destination=std::move(*ready);};
 hook(scrollHook,0x4bfe90,Scroll);hook(selectHook,0x4c0020,Select);hook(processHook,0x4bae80,Process);hook(valueHook,0x4b7fe0,Value);hook(labelHook,0x50a3f0,Label);
 for(auto* owned:{&scrollHook,&selectHook,&processHook,&valueHook,&labelHook})Require(bool(owned->enable()),"menu_inline_enable");Require(bool(fillBoundary.enable()),"menu_fill_enable");active.store(true,std::memory_order_release);
 OutputDebugStringW(L"Liberty's Shadows: experimental Graphics adapter armed; live validation pending.\n");
}
}
namespace preset117 {
extern HMODULE module;
std::atomic<bool> menuPending117{false};
std::atomic_flag menuAttemptLock117=ATOMIC_FLAG_INIT;
uint64_t menuLastAttempt117{};unsigned menuAttempts117{};
void RetryMenu117(uintptr_t game,const uint8_t* original,size_t bytes) noexcept {
 const native_ui117::Preserve preserve;
 if(!menuPending117.load(std::memory_order_acquire))return;
 if(menuAttemptLock117.test_and_set(std::memory_order_acquire))return;
 struct Unlock {~Unlock(){menuAttemptLock117.clear(std::memory_order_release);}} unlock;
 if(!menuPending117.load(std::memory_order_acquire))return;
 const auto now=GetTickCount64();if(menuAttempts117&&now-menuLastAttempt117<500)return;menuLastAttempt117=now;++menuAttempts117;
 if(GetModuleHandleW(L"GTAIV.EFLC.FusionFix.asi")){
  // FF asynchronously installs its frontend hooks. Do not arm competing UI
  // hooks before its known entry chains exist. This bounded poll does no I/O.
  bool ready=true;for(const auto rva:{0x4b5e30u,0x4bfe90u,0x4c0020u,0x4bae80u}){uint8_t opcode{};
   ready&=native_ui117::memory.Read(uint32_t(game+rva),&opcode,1)&&opcode==0xe9;
  }
  if(!ready&&menuAttempts117<40)return;
  if(!ready){menuPending117=false;OutputDebugStringW(L"Liberty's Shadows: frontend owner not ready; lighting stays active.\n");return;}
 }
 menuPending117=false;
 try{std::call_once(native_ui117::once,[&]{try{native_ui117::Prepare(game,original,bytes,module);}catch(...){native_ui117::Refuse();OutputDebugStringW(L"Liberty's Shadows: Graphics integration refused; lighting stays active.\n");}});}catch(...){native_ui117::Refuse();}
}
void EngineActivated117(uintptr_t game,const uint8_t* original,size_t bytes) noexcept {
 const native_ui117::Preserve preserve;
 try{wchar_t file[32768]{};if(!GetModuleFileNameW(module,file,32768))return;const auto ini=std::filesystem::path(file).replace_extension(L".ini");
  if(::GetPrivateProfileIntW(L"LIBERTY_SHADOWS",L"GraphicsMenu",startup.ownBackend?1:0,ini.c_str())!=1)return;
  menuPending117.store(true,std::memory_order_release);RetryMenu117(game,original,bytes);
 }catch(...){native_ui117::Refuse();}
}
DWORD FrontendOnly117(HMODULE addon) noexcept {
 // Reuse the loader's bounded startup worker. Off never starts a lighting
 // manager, preparation service, shadow frame observer or selection worker.
 const native_ui117::Preserve preserve;
 try{
  const auto ini=native_ui117::image117::onyxoak::pe::ModulePath(addon).replace_extension(L".ini");
  if(::GetPrivateProfileIntW(L"LIBERTY_SHADOWS",L"GraphicsMenu",startup.ownBackend?1:0,ini.c_str())!=1)return 0;
  const auto game=reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
  const auto raw=native_ui117::image117::onyxoak::pe::Read(native_ui117::image117::onyxoak::pe::ModulePath(nullptr));
  if(!compatibility117::Accept(raw,"08759a5516f9837920ea504436236bbab89d0826a8e4d04ff106345177b5345d"))return 1;
  native_ui117::image117::onyxoak::pe::Image image(raw,game);
  constexpr std::array<uint32_t,4> entries{0x4b5e30,0x4bfe90,0x4c0020,0x4bae80};
  bool ready=false;
  for(unsigned attempt=0;attempt<400&&!ready;++attempt){
   ready=true;const bool ff=GetModuleHandleW(L"GTAIV.EFLC.FusionFix.asi")!=nullptr;
   for(auto rva:entries){std::array<uint8_t,32> live{};
    const bool read=native_ui117::memory.Read(uint32_t(game+rva),live.data(),live.size());
    ready&=read&&(ff?live[0]==0xe9:!std::memcmp(live.data(),image.bytes.data()+rva,live.size()));
   }
   if(!ready)Sleep(50);
  }
  if(!ready)return 1;
  EngineActivated117(game,image.bytes.data(),image.bytes.size());
  // Pending readiness must finish here: there is deliberately no shadow
  // OnFrame hook in this path. Successful preparation leaves only UI hooks.
  for(unsigned n=0;n<400&&menuPending117.load(std::memory_order_acquire);++n){RetryMenu117(game,image.bytes.data(),image.bytes.size());if(menuPending117.load(std::memory_order_acquire))Sleep(50);}
  return native_ui117::active.load(std::memory_order_acquire)?0:1;
 }catch(...){native_ui117::Refuse();return 1;}
}
}
