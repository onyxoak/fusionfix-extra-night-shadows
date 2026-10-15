// Physical pole geometry: current wd_local only, native rigid depth dispatch.
// No command/entity/MODE/projection edits; only native dispatch-local depth byte.
namespace PoleDepthRepair157 {
SafetyHookMid dispatchHook;bool requested=false;bool prepared=false;thread_local Binding current;uint32_t registeredLocal{};std::atomic<uint64_t> accepted{0},promoted{0};
std::atomic<uint64_t> dispatch197{0},registerRefused197{0},matchRefused197{0},nonnullOwner197{0};
struct Preserve {DWORD error=GetLastError();fusionfix::shadows::FloatingPointState fp;~Preserve()noexcept{SetLastError(error);}};
int Filter(DWORD code)noexcept{return code==EXCEPTION_ACCESS_VIOLATION||code==EXCEPTION_GUARD_PAGE?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH;}
bool FastLocal()noexcept {
#ifdef LIBERTY_POLE_DEPTH_TEST157
 const Preserve preserve;int32_t epoch{},group{};uint32_t id{};
 return Liberty20::Copy(ff+0x3366f0,&epoch,4)&&epoch<-1&&Liberty20::Copy(ff+0x3366ec,&id,4)&&id<=65535&&Liberty20::Copy(game+0xc6b308,&group,4)&&group>=0&&uint32_t(group)==id;
#else
 __try {const auto epoch=*reinterpret_cast<const int32_t*>(ff+0x3366f0);if(epoch>=-1)return false;const auto id=*reinterpret_cast<const uint32_t*>(ff+0x3366ec);const auto group=*reinterpret_cast<const int32_t*>(game+0xc6b308);return id<=65535&&group>=0&&uint32_t(group)==id;} __except(Filter(GetExceptionCode())){return false;}
#endif
}

bool Read(uint64_t at,void* p,size_t n)noexcept{return pole_caster134::Address(at,n)&&Liberty20::Copy(uintptr_t(at),p,n);}
uint32_t Frame()noexcept{uint32_t f=UINT32_MAX;if(CShadows::pFrameCounter)(void)Read(uintptr_t(CShadows::pFrameCounter),&f,4);return f;}
uint32_t Word157(const uint8_t* p)noexcept {uint32_t v{};std::memcpy(&v,p,4);return v;}
bool Group(uint32_t& id)noexcept {
 int32_t epoch{},group{};uint32_t local{};
 if(!ff||!FusionBackend130::prepared||!FusionBackend130::lease.Owns()||reinterpret_cast<uintptr_t>(FusionBackend130::pinnedModule)!=ff||
  reinterpret_cast<uintptr_t>(GetModuleHandleW(L"GTAIV.EFLC.FusionFix.asi"))!=ff||!Read(ff+0x3366f0,&epoch,4)||epoch>=-1||
  !Read(ff+0x3366ec,&local,4)||local>65535||!Read(game+0xc6b308,&group,4)||group<0||uint32_t(group)!=local)return false;
 id=local;return true;
}
Scope::Scope(void* object)noexcept:previous(current){
 current={};if(!requested||!prepared||!addonActive.load(std::memory_order_acquire)||!FastLocal())return;
 const Preserve preserve;const traffic_signal::fresh_page::Operation44 pages(32);const immediate_region33::Scope regions;
 Binding b{}; // FastLocal already refused ordinary view; cheap model hints precede full group proof.
 const auto p=uintptr_t(object);if(p>UINT32_MAX)return;b.command=uint32_t(p);b.thread=GetCurrentThreadId();
 uint16_t model{};uint32_t mode{};
 if(!Read(p+0x54,&mode,4)||mode!=0||!Read(p+0x58,&model,2)||model>=31000)return;b.model=model;
 const auto slot=uint64_t(game)+0xe95cd8u+b.model*4u;
 if(!Read(slot,&b.info,4)||!Read(uint64_t(b.info)+0x3c,&b.hash,4)||!pole_caster134::SupportedPole(b.hash))return;
 if(!Group(b.localGroup))return;b.frame=Frame();if(b.frame==UINT32_MAX)return;
 if(!Read(uint64_t(b.info)+8,&b.fragment,4)||!b.fragment||!Read(p,b.bytes.data(),b.bytes.size()))return;
 uint16_t actualModel{};uint32_t actualMode{};std::memcpy(&actualModel,b.bytes.data()+0x58,2);std::memcpy(&actualMode,b.bytes.data()+0x54,4);if(actualModel!=b.model||actualMode)return;
 std::array<uint8_t,0x60> again{};uint32_t info{},hash{},fragment{},group{};
 if(!Read(p,again.data(),again.size())||again!=b.bytes||!Read(slot,&info,4)||info!=b.info||!Read(uint64_t(info)+0x3c,&hash,4)||hash!=b.hash||
  !Read(uint64_t(info)+8,&fragment,4)||fragment!=b.fragment||!Group(group)||group!=b.localGroup||Frame()!=b.frame)return;
 b.valid=true;current=b;++accepted;
}
Scope::~Scope()noexcept{current=previous;}
bool BorrowedRead(uint64_t at,void* p,size_t n)noexcept {
 if(!pole_caster134::Address(at,n))return false;
#ifdef LIBERTY_POLE_DEPTH_TEST157
 return Liberty20::Copy(uintptr_t(at),p,n);
#else
 __try {std::memcpy(p,reinterpret_cast<const void*>(uintptr_t(at)),n);return true;}__except(Filter(GetExceptionCode())){return false;}
#endif
}
bool WriteLocal(uintptr_t at)noexcept {__try{*reinterpret_cast<uint8_t*>(at)=1;return true;}__except(Filter(GetExceptionCode())){return false;}}
bool GenericOwner197(uint32_t owner,uint32_t& type)noexcept {
 // Exact native6EC7F7..811 derives EBX only for class0x80. A nonnull
 // TYPE4 pole owner is still the generic drawable route; requiring nullptr
 // incorrectly rejected it. Keep the current native owner/type fresh.
 type=0;return !owner||(BorrowedRead(uint64_t(owner)+0x28,&type,4)&&(type&0x3c0u)!=0x80u);
}
bool MatchBorrowed(const Binding& b,uintptr_t stack)noexcept {
 //Current native4DD0B0 owns this command through every child callback; exact
 //body guard proves it does not free or reuse it. No permission snapshot/cache
 //survives native calls. All USED data below is freshly reread under SEH.
 const auto frame=[]()noexcept{uint32_t f=UINT32_MAX;if(CShadows::pFrameCounter)(void)BorrowedRead(uintptr_t(CShadows::pFrameCounter),&f,4);return f;};
 int32_t epoch{},group{};uint32_t local{},forced{},mode{},owner{},ownerType{},info{},hash{},fragment{};std::array<uint8_t,0x60> command{};
 if(frame()!=b.frame||!BorrowedRead(ff+0x3366f0,&epoch,4)||epoch>=-1||!BorrowedRead(ff+0x3366ec,&local,4)||local!=b.localGroup||
  !BorrowedRead(game+0xc6b308,&group,4)||group<0||uint32_t(group)!=local||!BorrowedRead(game+0x13f59d4,&forced,4)||!forced||
  !BorrowedRead(game+0x11dbdc4,&mode,4)||mode||!BorrowedRead(game+0x11dbdbc,&owner,4)||!GenericOwner197(owner,ownerType)||
  !BorrowedRead(b.command,command.data(),command.size())||command!=b.bytes||!BorrowedRead(uint64_t(game)+0xe95cd8u+b.model*4u,&info,4)||info!=b.info||
  !BorrowedRead(uint64_t(info)+0x3c,&hash,4)||hash!=b.hash||!BorrowedRead(uint64_t(info)+8,&fragment,4)||fragment!=b.fragment)return false;
 uint32_t againForced{},againLocal{},againOwner{},againOwnerType{};int32_t againEpoch{},againGroup{};std::array<uint8_t,0x60> againCommand{};uint8_t flag{};
 if(!BorrowedRead(b.command,againCommand.data(),againCommand.size())||againCommand!=command||!BorrowedRead(game+0x13f59d4,&againForced,4)||againForced!=forced||
  !BorrowedRead(ff+0x3366f0,&againEpoch,4)||againEpoch!=epoch||!BorrowedRead(ff+0x3366ec,&againLocal,4)||againLocal!=local||
  !BorrowedRead(game+0xc6b308,&againGroup,4)||againGroup!=group||
  !BorrowedRead(game+0x11dbdbc,&againOwner,4)||againOwner!=owner||!GenericOwner197(againOwner,againOwnerType)||againOwnerType!=ownerType||
  frame()!=b.frame||!BorrowedRead(uint64_t(stack)+0x30,&flag,1)||flag)return false;
 if(owner)nonnullOwner197.fetch_add(1,std::memory_order_relaxed);
 return true;
}
void Dispatch(SafetyHookContext& regs)noexcept {
 if(!requested||!prepared||!current.valid||!addonActive.load(std::memory_order_acquire))return;
 const Preserve preserve;
 dispatch197.fetch_add(1,std::memory_order_relaxed);
 const auto& b=current;if(GetCurrentThreadId()!=b.thread||uint32_t(regs.ebp)!=0||uint32_t(regs.ebx)!=0||(uint32_t(regs.eax)&0xffu)){
  registerRefused197.fetch_add(1,std::memory_order_relaxed);return;
 }
 if(
  !FusionBackend130::prepared||!FusionBackend130::lease.Owns()||reinterpret_cast<uintptr_t>(FusionBackend130::pinnedModule)!=ff||
  reinterpret_cast<uintptr_t>(GetModuleHandleW(L"GTAIV.EFLC.FusionFix.asi"))!=ff||!MatchBorrowed(b,regs.esp)){
  matchRefused197.fetch_add(1,std::memory_order_relaxed);return;
 }
 // Native6EC830 already writes this same local byte, with unchanged ESP until
 //6EC84A. Thus it is a native-owned writable stack local, not command storage.
 if(!WriteLocal(regs.esp+0x30))return;
 // Native virtual30 argument is a geometry mask:1 skinned,2 rigid.
 // MODE0 already left AL0. Keep it0; only depth=true selects argument2.
 // No entity/command MODE, ownership, LOD, material or map mutation.
 ++promoted;
}
void Publish()noexcept {if(!requested||!prepared)return;static uint32_t last{};const auto now=GetTickCount();if(last&&uint32_t(now-last)<5000)return;last=now;Event(1,"POLE_DEPTH197C_COUNTS",("accepted="+std::to_string(accepted.load())+" dispatch="+std::to_string(dispatch197.load())+" registers_refused="+std::to_string(registerRefused197.load())+" match_refused="+std::to_string(matchRefused197.load())+" nonnull_owner_matched="+std::to_string(nonnullOwner197.load())+" promoted="+std::to_string(promoted.load())+" wd_local_only=1 extra_maps=0 pixels_unverified=1").c_str());}
void PrepareHooks()noexcept {
 if(!requested){prepared=false;return;} // Failed158 renderer experiment archived; no default hook or reads.
 try{
  Require(ff&&FusionBackend130::prepared&&reinterpret_cast<uintptr_t>(FusionBackend130::pinnedModule)==ff&&reinterpret_cast<uintptr_t>(GetModuleHandleW(L"GTAIV.EFLC.FusionFix.asi"))==ff,"pole_depth157_owned_ff");
  constexpr uint32_t begin=0x6ec7b0;constexpr size_t size=0x203;
  const auto live=ReadMemory(game+begin,size);Require(!std::memcmp(live.data(),original->bytes.data()+begin,size),"pole_depth157_native_dispatch");
  const auto getter=ReadMemory(game+0x44940,6);Require(getter[0]==0xa1&&getter[5]==0xc3&&Word157(getter.data()+1)==uint32_t(game+0x13f59d4),"pole_depth157_forced_shader_getter");
  const auto groupGetter=ReadMemory(game+0x2f0b0,6);Require(groupGetter[0]==0xa1&&groupGetter[5]==0xc3&&Word157(groupGetter.data()+1)==uint32_t(game+0xc6b308),"pole_depth157_group_getter");
  const auto name=ReadMemory(ff+0x266868,9);Require(!std::memcmp(name.data(),"wd_local",9),"pole_depth157_local_name");
  auto expected=std::vector<uint8_t>{0x68,0,0,0,0,0xc6,0x45,0xfc,0x08,0xff,0xd7,0x68,0,0,0,0,0xa3,0,0,0,0};
  const uint32_t nameAt=uint32_t(ff+0x266868),guardAt=uint32_t(ff+0x3366f0),idAt=uint32_t(ff+0x3366ec);
  std::memcpy(expected.data()+1,&nameAt,4);std::memcpy(expected.data()+12,&guardAt,4);std::memcpy(expected.data()+17,&idAt,4);
  Require(ReadMemory(ff+0xa7693,expected.size())==expected,"pole_depth157_registered_local_source");
  Prepare(dispatchHook,game+0x6ec84a,Dispatch);
  constexpr std::array<uint8_t,8> bytes{0x8b,0x54,0x24,0x10,0x8b,0x4c,0x24,0x28};const auto stolen=dispatchHook.original_bytes();
  Require(stolen.size()==bytes.size()&&!std::memcmp(stolen.data(),bytes.data(),bytes.size()),"pole_depth157_stolen");
  prepared=true;Event(1,"POLE_DEPTH157_PREPARED","wd_local_only=1 native_virtual30_arg2_rigid=1 MODE0_preserved=1 extra_maps=0");
 }catch(...){dispatchHook={};prepared=false;Event(1,"POLE_DEPTH157_REFUSED","exact_native_or_registered_local_contract_failed=1");}
}
}
