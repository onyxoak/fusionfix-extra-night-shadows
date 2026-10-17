// Physical pole geometry: current wd_local only, native rigid depth dispatch.
// Native dispatch-local depth and material argument repair; shared state is unchanged.
namespace PoleDepthRepair157 {
uint32_t localEpochRva244=0,localGroupRva244=0;
bool (*discoverLocal244)(uint32_t&,uint32_t&) noexcept=nullptr;
SafetyHookMid dispatchHook,geometryHook251;bool requested=false;bool prepared=false;thread_local Binding current;uint32_t registeredLocal{};std::atomic<uint64_t> accepted{0},promoted{0};
std::atomic<uint64_t> dispatch197{0},registerRefused197{0},matchRefused197{0},nonnullOwner197{0};
// Sample actual callback refusals without changing the native drawing decision.
// Zero is success; each nonzero bucket identifies the first failed contract.
std::array<std::atomic<uint32_t>,16> refusal245{};
std::atomic<uint32_t> lastReason245{0},lastValue245{0};
thread_local bool sampleRefusal245=false;
bool Reject245(unsigned reason,uint32_t value=0) noexcept {
 if(sampleRefusal245&&reason<refusal245.size()){
  refusal245[reason].fetch_add(1,std::memory_order_relaxed);
  lastValue245.store(value,std::memory_order_relaxed);lastReason245.store(reason,std::memory_order_relaxed);
 }
 return false;
}
struct Preserve {DWORD error=GetLastError();fusionfix::shadows::FloatingPointState fp;~Preserve()noexcept{SetLastError(error);}};
int Filter(DWORD code)noexcept{return code==EXCEPTION_ACCESS_VIOLATION||code==EXCEPTION_GUARD_PAGE?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH;}
bool FastLocal()noexcept {
#ifdef LIBERTY_POLE_DEPTH_TEST157
 const Preserve preserve;int32_t epoch{},group{};uint32_t id{};
 return Liberty20::Copy(ff+localEpochRva244,&epoch,4)&&epoch<-1&&Liberty20::Copy(ff+localGroupRva244,&id,4)&&id<=65535&&Liberty20::Copy(game+0xc6b308,&group,4)&&group>=0&&uint32_t(group)==id;
#else
 __try {const auto epoch=*reinterpret_cast<const int32_t*>(ff+localEpochRva244);if(epoch>=-1)return false;const auto id=*reinterpret_cast<const uint32_t*>(ff+localGroupRva244);const auto group=*reinterpret_cast<const int32_t*>(game+0xc6b308);return id<=65535&&group>=0&&uint32_t(group)==id;} __except(Filter(GetExceptionCode())){return false;}
#endif
}

bool Read(uint64_t at,void* p,size_t n)noexcept{return pole_caster134::Address(at,n)&&Liberty20::Copy(uintptr_t(at),p,n);}
uint32_t Frame()noexcept{uint32_t f=UINT32_MAX;if(CShadows::pFrameCounter)(void)Read(uintptr_t(CShadows::pFrameCounter),&f,4);return f;}
uint32_t Word157(const uint8_t* p)noexcept {uint32_t v{};std::memcpy(&v,p,4);return v;}
bool Group(uint32_t& id)noexcept {
 int32_t epoch{},group{};uint32_t local{};
 if(!ff||!FusionBackend130::prepared||!FusionBackend130::lease.Owns()||reinterpret_cast<uintptr_t>(FusionBackend130::pinnedModule)!=ff||
  reinterpret_cast<uintptr_t>(pe::ModuleAt(ff))!=ff||!Read(ff+localEpochRva244,&epoch,4)||epoch>=-1||
  !Read(ff+localGroupRva244,&local,4)||local>65535||!Read(game+0xc6b308,&group,4)||group<0||uint32_t(group)!=local)return false;
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
struct Material247 {uint32_t filter{},bucketMask{},drawable{},shaderGroup{};};
std::atomic<uint32_t> materialSamples247{},materialUnknown247{},materialMatching247{},materialRejected247{};
std::atomic<uint32_t> lastFilter247{},lastBuckets247{},lastDrawable247{},lastModel247{};
template<class Read> bool ReadMaterial247(Read&& read,const SafetyHookContext& regs,Material247& out) noexcept {
 out={};uint32_t context{},array{},groupAgain{};uint16_t count{},countAgain{};
 out.drawable=uint32_t(regs.esi);
 if(!out.drawable||!read(uint64_t(regs.esp)+0x10,&context,4)||!context||
  !read(uint64_t(context)+8,&out.filter,4)||!read(uint64_t(out.drawable)+8,&out.shaderGroup,4)||!out.shaderGroup||
  !read(uint64_t(out.shaderGroup)+8,&array,4)||!array||
  !read(uint64_t(out.shaderGroup)+0xc,&count,2)||!count||count>128)return false;
 for(unsigned i=0;i<count;++i){uint32_t shader{},shaderAgain{};uint8_t bucket{},again{};
  if(!read(uint64_t(array)+i*4,&shader,4)||!shader||!read(uint64_t(shader)+9,&bucket,1)||bucket>=32||
   !read(uint64_t(shader)+9,&again,1)||again!=bucket||!read(uint64_t(array)+i*4,&shaderAgain,4)||shaderAgain!=shader)return false;
  out.bucketMask|=1u<<bucket;
 }
 uint32_t arrayAgain{},contextAgain{},filterAgain{};
 return read(uint64_t(regs.esp)+0x10,&contextAgain,4)&&contextAgain==context&&
  read(uint64_t(context)+8,&filterAgain,4)&&filterAgain==out.filter&&
  read(uint64_t(out.drawable)+8,&groupAgain,4)&&groupAgain==out.shaderGroup&&
  read(uint64_t(out.shaderGroup)+8,&arrayAgain,4)&&arrayAgain==array&&
  read(uint64_t(out.shaderGroup)+0xc,&countAgain,2)&&countAgain==count;
}
void ObserveMaterial247(const SafetyHookContext& regs) noexcept {
 if(!sampleRefusal245)return;Material247 m{};
 if(!ReadMaterial247(BorrowedRead,regs,m)){++materialUnknown247;return;}
 ++materialSamples247;lastFilter247.store(m.filter,std::memory_order_relaxed);
 lastBuckets247.store(m.bucketMask,std::memory_order_relaxed);lastDrawable247.store(m.drawable,std::memory_order_relaxed);
 lastModel247.store(current.model,std::memory_order_relaxed);
 if(m.filter==UINT32_MAX||(m.filter<32&&(m.bucketMask&(1u<<m.filter))))++materialMatching247;
 else ++materialRejected247;
}
std::atomic<uint32_t> materialRouted250{},materialKept250{},materialUnknown250{};
void RouteMaterial250(SafetyHookContext& regs) noexcept {
 // At native6EC855 EDX holds the freshly loaded material filter. Route only
 // authenticated pole depth calls whose current group contains category4 and no native category0.
 // Changing this argument leaves shared contexts, shaders and ordinary draws intact.
 if(uint32_t(regs.edx)!=0){++materialKept250;return;}
 Material247 m{};
 if(!ReadMaterial247(BorrowedRead,regs,m)){++materialUnknown250;return;}
 if(m.filter==0&&(m.bucketMask&(1u<<4))&&!(m.bucketMask&1u)){regs.edx=4;++materialRouted250;}
 else ++materialKept250;
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
 if(frame()!=b.frame)return Reject245(1);
 if(!BorrowedRead(ff+localEpochRva244,&epoch,4)||epoch>=-1)return Reject245(2,uint32_t(epoch));
 if(!BorrowedRead(ff+localGroupRva244,&local,4)||local!=b.localGroup)return Reject245(3,local);
 if(!BorrowedRead(game+0xc6b308,&group,4)||group<0||uint32_t(group)!=local)return Reject245(4,uint32_t(group));
 // The authenticated wd_local pass can use the active shader group without
 // the separate native forced-shader pointer. Accept its stable null value;
 // current group, command, owner and native dispatch still authorize scope.
 if(!BorrowedRead(game+0x13f59d4,&forced,4))return Reject245(5,forced);
 if(!BorrowedRead(game+0x11dbdc4,&mode,4)||mode)return Reject245(6,mode);
 if(!BorrowedRead(game+0x11dbdbc,&owner,4)||!GenericOwner197(owner,ownerType))return Reject245(7,ownerType);
 if(!BorrowedRead(b.command,command.data(),command.size())||command!=b.bytes)return Reject245(8);
 if(!BorrowedRead(uint64_t(game)+0xe95cd8u+b.model*4u,&info,4)||info!=b.info)return Reject245(9,info);
 if(!BorrowedRead(uint64_t(info)+0x3c,&hash,4)||hash!=b.hash||!BorrowedRead(uint64_t(info)+8,&fragment,4)||fragment!=b.fragment)return Reject245(10,hash);
 uint32_t againForced{},againLocal{},againOwner{},againOwnerType{};int32_t againEpoch{},againGroup{};std::array<uint8_t,0x60> againCommand{};uint8_t flag{};
 if(!BorrowedRead(b.command,againCommand.data(),againCommand.size())||againCommand!=command||!BorrowedRead(game+0x13f59d4,&againForced,4)||againForced!=forced||
  !BorrowedRead(ff+localEpochRva244,&againEpoch,4)||againEpoch!=epoch||!BorrowedRead(ff+localGroupRva244,&againLocal,4)||againLocal!=local||
  !BorrowedRead(game+0xc6b308,&againGroup,4)||againGroup!=group||
  !BorrowedRead(game+0x11dbdbc,&againOwner,4)||againOwner!=owner||!GenericOwner197(againOwner,againOwnerType)||againOwnerType!=ownerType||
  frame()!=b.frame)return Reject245(11);
 if(!BorrowedRead(uint64_t(stack)+0x30,&flag,1)||flag)return Reject245(12,flag);
 if(owner)nonnullOwner197.fetch_add(1,std::memory_order_relaxed);
 return true;
}
void Dispatch(SafetyHookContext& regs)noexcept {
 if(!requested||!prepared||!current.valid||!addonActive.load(std::memory_order_acquire))return;
 const Preserve preserve;
 const auto dispatchNumber245=dispatch197.fetch_add(1,std::memory_order_relaxed);
 #ifdef LIBERTY_POLE_DEPTH_TEST157
 sampleRefusal245=(dispatchNumber245&255u)==0;
#else
 sampleRefusal245=false;
#endif
 const auto& b=current;if(GetCurrentThreadId()!=b.thread||uint32_t(regs.ebp)!=0||uint32_t(regs.ebx)!=0||(uint32_t(regs.eax)&0xffu)){
  registerRefused197.fetch_add(1,std::memory_order_relaxed);return;
 }
 if(
  !FusionBackend130::prepared||!FusionBackend130::lease.Owns()||reinterpret_cast<uintptr_t>(FusionBackend130::pinnedModule)!=ff||
  reinterpret_cast<uintptr_t>(pe::ModuleAt(ff))!=ff||!MatchBorrowed(b,regs.esp)){
  matchRefused197.fetch_add(1,std::memory_order_relaxed);return;
 }
 // Native6EC830 already writes this same local byte, with unchanged ESP until
 //6EC855. Thus it is a native-owned writable stack local, not command storage.
 // A null forced shader requires normal per-material technique setup.
 // Preserve the native depth byte: forcing the fast depth method bypasses
 // that setup and can draw rigid poles with the previously bound skinned VS.
 ObserveMaterial247(regs);
 RouteMaterial250(regs);
 // Native virtual30 argument is a geometry mask:1 skinned,2 rigid.
 // MODE0 already left AL0. Keep it0; only depth=true selects argument2.
 // No entity/command MODE, ownership, LOD, shared material or map mutation.
 ++promoted;
}
std::atomic<uint32_t> geometryCalls251{},geometryUnknown251{},geometryFilter251{},geometryModel251{},geometryFunction251{};
std::atomic<uint32_t> geometryX251{},geometryY251{},geometryZ251{};
void Geometry251(SafetyHookContext& regs) noexcept {
 if(!requested||!prepared||!current.valid||!addonActive.load(std::memory_order_acquire)||GetCurrentThreadId()!=current.thread||!FastLocal())return;
 const Preserve preserve;const auto count=geometryCalls251.fetch_add(1,std::memory_order_relaxed);
 if(count&255u)return;
 uint32_t filter{},vt{},fn{},geometry{};
 geometry=uint32_t(regs.ecx);filter=UINT32_MAX;
 if(!BorrowedRead(geometry,&vt,4)||!BorrowedRead(uint64_t(vt)+8,&fn,4)){++geometryUnknown251;return;}
 geometryFilter251.store(filter,std::memory_order_relaxed);geometryModel251.store(current.model,std::memory_order_relaxed);
 geometryFunction251.store(fn>=game?uint32_t(fn-game):fn,std::memory_order_relaxed);
 geometryX251.store(Word157(current.bytes.data()+0x40),std::memory_order_relaxed);
 geometryY251.store(Word157(current.bytes.data()+0x44),std::memory_order_relaxed);
 geometryZ251.store(Word157(current.bytes.data()+0x48),std::memory_order_relaxed);
}
void Publish()noexcept {
#ifndef LIBERTY_POLE_DEPTH_TEST157
 return; // Temporary pole tracing is disabled for gameplay.
#endif
 if(!requested||!prepared)return;static uint32_t last{};const auto now=GetTickCount();if(last&&uint32_t(now-last)<5000)return;last=now;
 Event(1,"POLE_DEPTH197C_COUNTS",("accepted="+std::to_string(accepted.load())+" dispatch="+std::to_string(dispatch197.load())+" registers_refused="+std::to_string(registerRefused197.load())+" match_refused="+std::to_string(matchRefused197.load())+" nonnull_owner_matched="+std::to_string(nonnullOwner197.load())+" promoted="+std::to_string(promoted.load())+" wd_local_only=1 native_depth_flag_preserved=1 extra_maps=0 pixels_unverified=1").c_str());
 Event(1,"POLE_FILTER250_COUNTS",("routed="+std::to_string(materialRouted250.load())+" unchanged="+std::to_string(materialKept250.load())+" unknown="+std::to_string(materialUnknown250.load())+" argument_only=1 physical_category4=1 pixels_unverified=1").c_str());
 Event(1,"POLE_GEOMETRY254_COUNTS",("calls="+std::to_string(geometryCalls251.load())+" unknown="+std::to_string(geometryUnknown251.load())+" filter="+std::to_string(geometryFilter251.load())+" model="+std::to_string(geometryModel251.load())+" fn_rva="+std::to_string(geometryFunction251.load())+" x_bits="+std::to_string(geometryX251.load())+" y_bits="+std::to_string(geometryY251.load())+" z_bits="+std::to_string(geometryZ251.load())+" cpu_geometry_not_gpu_proof=1").c_str());
 static constexpr std::array<const char*,13> names{"none","frame","epoch","local_group","draw_group","forced_shader","command_mode","owner","command_bytes","model_info","fragment","changed_during_check","depth_flag"};
 std::string detail="sample_every=256";
 for(unsigned i=1;i<names.size();++i)detail+=" "+std::string(names[i])+"="+std::to_string(refusal245[i].load(std::memory_order_relaxed));
 detail+=" last_reason="+std::to_string(lastReason245.load(std::memory_order_relaxed))+" last_value="+std::to_string(lastValue245.load(std::memory_order_relaxed));
 Event(1,"POLE_REFUSAL245_COUNTS",detail.c_str());
 Event(1,"POLE_MATERIAL247_COUNTS",("samples="+std::to_string(materialSamples247.load())+" unreadable_or_changed="+std::to_string(materialUnknown247.load())+" group_has_matching_category="+std::to_string(materialMatching247.load())+" group_has_no_matching_category="+std::to_string(materialRejected247.load())+" filter="+std::to_string(lastFilter247.load())+" buckets_mask="+std::to_string(lastBuckets247.load())+" drawable="+std::to_string(lastDrawable247.load())+" model="+std::to_string(lastModel247.load())+" shader_group_not_geometry_or_gpu_proof=1").c_str());
}
void PrepareHooks()noexcept {
 if(!requested){prepared=false;return;} // Failed158 renderer experiment archived; no default hook or reads.
 try{
  Require(ff&&FusionBackend130::prepared&&reinterpret_cast<uintptr_t>(FusionBackend130::pinnedModule)==ff&&reinterpret_cast<uintptr_t>(pe::ModuleAt(ff))==ff,"pole_depth157_owned_ff");
  constexpr uint32_t begin=0x6ec7b0;constexpr size_t size=0x203;
  const auto live=ReadMemory(game+begin,size);Require(!std::memcmp(live.data(),original->bytes.data()+begin,size),"pole_depth157_native_dispatch");
  const auto getter=ReadMemory(game+0x44940,6);Require(getter[0]==0xa1&&getter[5]==0xc3&&Word157(getter.data()+1)==uint32_t(game+0x13f59d4),"pole_depth157_forced_shader_getter");
  const auto groupGetter=ReadMemory(game+0x2f0b0,6);Require(groupGetter[0]==0xa1&&groupGetter[5]==0xc3&&Word157(groupGetter.data()+1)==uint32_t(game+0xc6b308),"pole_depth157_group_getter");
  Require(discoverLocal244&&discoverLocal244(localEpochRva244,localGroupRva244),"pole_depth244_registered_local_source_unavailable");
#ifdef LIBERTY_POLE_DEPTH_TEST157
// Observe the geometry function entry for both regular and fast-depth draws.
  // No alternate native branch may enter the displaced entry instructions.
  const auto geometryBytes251=ReadMemory(game+0x63490,6);
  constexpr std::array<uint8_t,6> exactGeometry251{0x57,0x8b,0xf9,0x8b,0x57,0x04};
  Require(!std::memcmp(geometryBytes251.data(),exactGeometry251.data(),6),"pole_geometry251_native_call");
#endif
  Prepare(dispatchHook,game+0x6ec855,Dispatch);
#ifdef LIBERTY_POLE_DEPTH_TEST157
  Prepare(geometryHook251,game+0x63490,Geometry251);
  Require(geometryHook251.original_bytes().size()==6&&!std::memcmp(geometryHook251.original_bytes().data(),exactGeometry251.data(),6),"pole_geometry251_stolen");
#endif
  constexpr std::array<uint8_t,8> bytes{0x85,0xc9,0x0f,0x84,0x83,0x00,0x00,0x00};const auto stolen=dispatchHook.original_bytes();
  Require(stolen.size()==bytes.size()&&!std::memcmp(stolen.data(),bytes.data(),bytes.size()),"pole_depth157_stolen");
  prepared=true;Event(1,"POLE_DEPTH157_PREPARED","wd_local_only=1 native_material_path_preserved=1 MODE0_preserved=1 extra_maps=0");
 }catch(...){dispatchHook={};geometryHook251={};prepared=false;Event(1,"POLE_DEPTH157_REFUSED","exact_native_or_registered_local_contract_failed=1");}
}
}
