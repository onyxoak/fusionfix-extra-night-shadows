// OnyxOak Extra Night Shadows Fix standalone CE adapter. GPL-3.0.
// Private candidate: guarded native backend or exact-hash official FusionFix adapters.
// Live compatibility and visual coverage require separate runtime validation.
#include "ExperimentRuntimeOptions.hpp"
#include "GpuProfiling39.hpp"
#include "GpuStageSchedule48.hpp"
#include "GpuResetGuard39.hpp"
#include "GpuResetGate39.hpp"
#include "NativeProjectionRegistry66.hpp"
#include "NativeProjectionRead66.hpp"
#include "VolumetricCompat199.hpp"
#include "core/standalone-shadow-core.cpp"
#include "CaptureDiagnostic44.hpp"
#include "CaptureFramePrelude46.hpp"
#include "MemoryReadProfile46.hpp"
#include "standalone-pe.hpp"
#include "PublicLightingIni93.hpp"
#include "SettingsRuntime117.hpp"
#include "RuntimeFeaturePolicy125.hpp"
namespace preset117 { ReadResult startup;State applied;bool backendLimited{},vehicleDepthUnavailable119{},backendReadFailed119{};void (*refreshApplied119Hook)() noexcept{};HMODULE module{};StartupLease runtimeLease; }
#include "standalone-midstub.hpp"
#include "standalone-ownership.hpp"
#include "standalone-support-log.hpp"
#include "ShortlistHelperRuntime48.hpp"
#include "NativeNightPrerequisite128.hpp"
#include "StartupConflict129.hpp"
#include "StartupGuidance210.hpp"
#include "standalone-warning-policy.hpp"
#include "core/LabSirenBlinkGrace.hpp"
#include "core/LabPoliceClaim.hpp"
#include "core/LabSirenVacancy.hpp"
#include "core/NearbyPoliceShadows.hpp"
#include "core/NearbyPoliceGuard.hpp"
#include <memory>
#include <bit>
#include <cwctype>
namespace standalone {
using namespace onyxoak;
using pe::U32;
constexpr char CandidateBuild32[]="candidate210";
HMODULE self{};
uintptr_t game{},ff{},loop{};
uint8_t *pauseUser{},*pauseScript{};
SafetyHookMid boundary,caster,contact,camera,brightness,warning;
std::atomic<uint32_t> warningHidden{0};
SafetyHookInline playerSubmit,npcSubmit,nativeSubmit;
bool nativeBackend=false;
bool ownExtra=true,ownHeadlight=true,ownVehicle=true;
namespace TailLight20 {
 extern bool requested;
 void OnFree(uint32_t,uint32_t,uint32_t) noexcept;
 void LifecycleFault() noexcept;
 uintptr_t CasterOwner(uintptr_t) noexcept;
 void ValidateQueued(rage::CLightSource*,uint32_t) noexcept;
}
namespace ModelLightShadow20 {
 extern bool requested;
 void OnFree(uint32_t,uint32_t) noexcept;
 void LifecycleFault() noexcept;
}
namespace CoronaLight20 {extern bool requested;}
namespace PoleCasterDiagnostic134 {extern bool requested;void Observe(uintptr_t,uint32_t,bool,bool) noexcept;}
namespace PoleDrawWitness145 {void SunCaster(uintptr_t,uint32_t,uint32_t,uintptr_t,bool)noexcept;}
namespace PoleStaticPartition141 {extern bool requested;bool Reclassify(uintptr_t,uint32_t,uint32_t,uintptr_t,bool,uint32_t*) noexcept;}
int32_t ownWidth=800,ownHeight=600;
namespace Liberty20 {
 extern bool dualRequested,worldRequested;
 bool Split(const std::array<int,16>&) noexcept;
 void PromoteNativeWorld(rage::CLightSource*,uint32_t) noexcept;
 void Result() noexcept;
 void Render(uint32_t,uint32_t,uint32_t,bool) noexcept;
}
std::unique_ptr<SupportLog> log;
std::atomic<bool> attempted{false},failed{false};
std::unique_ptr<pe::Image> original;
std::unique_ptr<pe::Image> bindings;
constexpr char GameHash[]="08759a5516f9837920ea504436236bbab89d0826a8e4d04ff106345177b5345d";
constexpr char FFHash[]="1c953d07661edde3fdf3b3565125d380171defb5c700a841b8f62181d72c873e";
struct FFLayout {
 const char *version,*hash;
 uint32_t process,player,npc,admission,lookup,render,caster,contact,frame,frameData;
 uint32_t width,height,extra,headlight,vehicle,warningPush,warningKey,textLookup;
};
constexpr FFLayout Layout51{"5.1.0",FFHash,0x18af0,0xa7b70,0xa7b60,0x74820,0xa7da0,0xa7e70,0xa6950,0xa6af0,0x83270,0x32b000,0x32b108,0x32b10c,0x32b045,0x32b046,0x2cfbe1,0x731c3,0x257af4,0xdabf0};
constexpr FFLayout Layout50{"5.0.1","6c5ac36c9c6ca4aadb9be2fe4efc4ad8a51ca112673150491294cc9eeb18d7c2",0x18c70,0xa1720,0xa1710,0x6fda0,0xa18e0,0xa19b0,0xa0400,0xa04c0,0,0,0x33ab4c,0x33ab50,0x33ab99,0x33ab9a,0x2dfa39,0x6e843,0x265b2c,0xcdae0};
constexpr FFLayout Layout511{"5.1.1","9f8a42d996e239c23c24432af4b95c568bfd1c501644a13bb7e9a4f51ed7be60",0x18b00,0xa7c40,0xa7c30,0x748c0,0xa7e70,0xa7f40,0xa6a20,0xa6bc0,0x83320,0x332000,0x332108,0x33210c,0x332045,0x332046,0x2d6be1,0x73263,0x25eb94,0xdace0};
const FFLayout* layout=&Layout51;
const FFLayout* SelectLayout(const std::string& hash){for(const auto* item:{&Layout50,&Layout51,&Layout511})if(hash==item->hash)return item;return nullptr;}
#ifdef STANDALONE_OFFLINE
struct RecordedRegion { uintptr_t address;std::vector<uint8_t> bytes; };
std::vector<RecordedRegion> records;
#endif
void Event(unsigned level,const char* code,const std::string& fields={}) noexcept {if(log)log->Event(level,code,fields);}
void Require(bool success,const char* why);
#include "StartupConflict129Runtime.inc"
// Private observation only: copied on the existing guarded selection thread.
// No source writes, new hooks, allocation decisions, or raw-pointer logging.
struct LightAuditSnapshot {
 uint32_t tick{},frame{},total{},count{};
 rage::Vector3 player{};
 std::array<rage::CLightSource,64> lights{};
};
LightAuditSnapshot auditSnapshot;
std::atomic<unsigned> auditState{0}; // 0 free, 1 producer owns, 2 ready, 3 consumer owns
bool auditEnabled=false;
bool diagnosticsEnabled=false,policeTestEnabled=false;
liberty_settings93::ReadResult publicSettings93;
bool hotCountersEnabled=false,ownerTelemetryEnabled=false;
bool nearbyPoliceEnabled=false;
bool NearbyPoint(const rage::CLightSource&) noexcept;
void UpdateNearby() noexcept;
void PromoteNearby(SafetyHookContext&,uint32_t) noexcept;
void BeginNearby(const rage::CLightSource*,uint32_t) noexcept;
int CompareNearby(int,int,int,int) noexcept;
void ApplyNearbyVacancy(SafetyHookContext&) noexcept;
void ObserveNearbyResult() noexcept;
void ObserveNearbyRender(uint32_t,uint32_t,uint32_t,bool) noexcept;
void ObserveLights(const rage::CLightSource* lights,uint32_t count) noexcept {
 if(!auditEnabled||count>4096||!lights)return;
 static uint32_t last{};const auto now=GetTickCount();
 if(now-last<250)return;
 unsigned free=0;if(!auditState.compare_exchange_strong(free,1,std::memory_order_acquire))return;
 last=now;auditSnapshot.tick=now;auditSnapshot.frame=*CShadows::pFrameCounter;
 auditSnapshot.total=count;auditSnapshot.count=0;
 const auto p=PlayerShadowAllocation::state.player;auditSnapshot.player={p.x,p.y,p.z};
 for(uint32_t i=0;i<count&&auditSnapshot.count<auditSnapshot.lights.size();++i){
  const auto& l=lights[i];const float x=l.mPosition.x-p.x,y=l.mPosition.y-p.y,z=l.mPosition.z-p.z;
  if(std::isfinite(x)&&std::isfinite(y)&&std::isfinite(z)&&x*x+y*y+z*z<=40.f*40.f)
   auditSnapshot.lights[auditSnapshot.count++]=l;
 }
 auditState.store(2,std::memory_order_release);
}
// One occupied-vehicle lifetime only. Never rebind or recycle a shadow key.
extern std::atomic<bool> proofKeyChecked;
alignas(64) uint8_t proofIdentity[64]{};
extern std::atomic<bool> vacancyRequested;
std::atomic<uint32_t> policePolicy{0};
std::atomic<bool> proofRequested{false},proofArmed{false},proofRetired{false};
std::atomic<uint32_t> proofOwner{0},proofFrame{0},proofPromoted{0};
std::atomic<uint32_t> proofSelected{0},proofFrames{0},proofMaxSlots{0};
uint32_t proofSlot=0,proofReference=0,proofPool=0;
bool proofBound=false;
preset117::EmergencyPolicy125 CurrentEmergencyPolicy125() {
 if(preset117::startup.ownBackend)return preset117::RuntimeEmergencyPolicy125(nullptr,policeTestEnabled);
 const auto ini=pe::ModulePath(self).replace_extension(L".ini");
 return preset117::RuntimeEmergencyPolicy125(ini.c_str(),policeTestEnabled);
}
uint32_t ProofKey() noexcept {return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(proofIdentity));}
bool IsProofPoint(const rage::CLightSource& l) noexcept {if(nearbyPoliceEnabled)return NearbyPoint(l);return l.mType==0&&static_cast<uint32_t>(l.mCastShadows)==ProofKey();}
void RetireProof() noexcept {proofArmed=false;proofRetired=true;}
void UpdateProof(){
 if(nearbyPoliceEnabled){UpdateNearby();return;}
 static uint32_t checked=0;const auto now=GetTickCount();
 if(now-checked>=500){checked=now;const auto policy125=CurrentEmergencyPolicy125();proofRequested=policy125.proof;policePolicy=policy125.policy;vacancyRequested=policy125.vacancy;}
 if(proofRetired.load())return;
 const auto owner=CPlayer::findPlayerCar?CPlayer::findPlayerCar():0;
 const auto pool=CVehicle::GetVehiclePool();
 lab_siren::Identity current{};
 const bool valid=[&]() noexcept {
  if(!owner||!pool||!pool->m_aStorage||!pool->m_aFlags||pool->m_nSize<=0||pool->m_nSize>4096||pool->m_nStorageSize<0x24||pool->m_nStorageSize>0x10000)return false;
  const auto base=reinterpret_cast<uintptr_t>(pool->m_aStorage);
  if(owner<base||uint64_t(owner-base)>=uint64_t(pool->m_nSize)*pool->m_nStorageSize||(owner-base)%pool->m_nStorageSize)return false;
  const auto slot=static_cast<uint32_t>((owner-base)/pool->m_nStorageSize);
  if(pool->GetIsFree(slot))return false;
  current={owner,reinterpret_cast<uintptr_t>(pool),slot,pool->GetReference(slot)};return true;
 }();
 if(valid&&pool->m_nStorageSize>=0xF1F){
  const auto f19=*reinterpret_cast<const uint8_t*>(owner+0xF19),f1e=*reinterpret_cast<const uint8_t*>(owner+0xF1E);
  const bool on=lab_siren::NativeSirenOn(f19,f1e);static int previous=-1;static uint32_t last=0;
  if(previous!=int(on)||now-last>=2000){char text[200];sprintf_s(text,"frame=%u f19=%u f1e=%u on=%u slot=%u reference=%u mode=%u",*CShadows::pFrameCounter,unsigned(f19),unsigned(f1e),unsigned(on),unsigned(current.slot),unsigned(current.reference),policePolicy.load());Event(1,"SIREN_STATE",text);previous=int(on);last=now;}
 }
 const auto decision=lab_siren::Decide(proofBound,proofRetired.load(),proofRequested.load(),valid,
    {proofOwner.load(),proofPool,proofSlot,proofReference},current);
 if(decision==lab_siren::Decision::Retire){RetireProof();Event(1,"PROOF_RETIRED","vehicle_lifetime_changed restart_required");return;}
 if(decision==lab_siren::Decision::Wait)return;
 if(decision==lab_siren::Decision::Bind){proofSlot=current.slot;proofReference=current.reference;proofPool=static_cast<uint32_t>(current.pool);proofOwner=static_cast<uint32_t>(current.owner);proofBound=true;Event(1,"PROOF_BOUND","one_vehicle_lifetime no_key_reuse");}
 proofFrame=*CShadows::pFrameCounter;proofArmed=proofRequested.load();
}
void NineInput(const uint32_t*) noexcept; void RememberVacancyInput(const uint32_t*) noexcept;
void PromoteProof(SafetyHookContext& regs,uint32_t caller) noexcept {
 if(nearbyPoliceEnabled){PromoteNearby(regs,caller);return;}
 if(!proofKeyChecked.load()||caller!=0x640d0a||!proofArmed.load()||proofRetired.load()||regs.edi!=proofOwner.load())return;
 auto* args=reinterpret_cast<uint32_t*>(regs.esp);
 if(args[2]!=0||args[3]!=0x110||args[16]!=0)return;
 const uint32_t frame=*CShadows::pFrameCounter;
 if(frame-proofFrame.load()>2)return;
 static thread_local uint32_t lastFrame=UINT32_MAX;
 if(lastFrame==frame)return;
 const auto pool=CVehicle::GetVehiclePool();
 if(reinterpret_cast<uintptr_t>(pool)!=proofPool||pool->GetIsFree(proofSlot)||pool->GetReference(proofSlot)!=proofReference){RetireProof();return;}
 // The opaque key has reserved storage for the whole DLL lifetime; never a bool.
 // Only this verified producer's call arguments are changed. Native 7-slot budget remains intact.
 lastFrame=frame;NineInput(args);RememberVacancyInput(args);args[3]|=4u;args[16]=ProofKey();++proofPromoted;
}
#include "proof7-grace.inc"
#include "proof6-transition.inc"
#include "proof8-policy.inc"
#include "proof9-trace.inc"
#include "proof9-vacancy.inc"
#include "police-nearby.inc"
void ObserveProofResult() noexcept {
 if(nearbyPoliceEnabled){ObserveNearbyResult();return;}
 if(!proofOwner.load())return;
 unsigned active=0;bool selected=false;int selectedSlot=-1;bool present=false;
 if(PlayerShadowAllocation::CurrentCount()<=4096)for(unsigned i=0;i<PlayerShadowAllocation::CurrentCount();++i)if(IsProofPoint(PlayerShadowAllocation::CurrentLights()[i])){present=true;break;}
 for(unsigned i=0;i<7;++i){const auto record=game+fusionfix::shadows::ce::allocation::DynamicPassFirstPositionRva+i*0x110;
  if(*reinterpret_cast<const uint8_t*>(record+0x25)){++active;if(*reinterpret_cast<const uint32_t*>(record+0x30)==ProofKey()){selected=true;selectedSlot=static_cast<int>(i);}}}
 NineSelection(); CaptureProofTransition(selected); FinishProofGrace(present,selectedSlot); CapturePolicePacket(present,selectedSlot); ++proofFrames;if(selected)++proofSelected;auto old=proofMaxSlots.load();while(active>old&&!proofMaxSlots.compare_exchange_weak(old,active)){}
}

std::atomic<bool> proofKeyChecked{false};
void ObserveLabLights(const rage::CLightSource* lights,uint32_t count) noexcept {
 if(nearbyPoliceEnabled){BeginNearby(lights,count);ObserveLights(lights,count);return;}
 BeginProofGrace(); BeginPoliceClaim();
 if(!proofPromoted.load()&&!proofRetired.load()&&count<=4096){
  bool collision=false;for(uint32_t i=0;i<count;++i)if(static_cast<uint32_t>(lights[i].mCastShadows)==ProofKey())collision=true;
  for(unsigned i=0;i<8;++i)if(*reinterpret_cast<const uint32_t*>(game+fusionfix::shadows::ce::allocation::CacheAge0Rva+i*0x100+4)==ProofKey())collision=true;
  for(unsigned i=0;i<7;++i)if(*reinterpret_cast<const uint32_t*>(game+fusionfix::shadows::ce::allocation::DynamicPassFirstPositionRva+i*0x110+0x30)==ProofKey())collision=true;
  if(collision)RetireProof();else proofKeyChecked=true;
 }
 ObserveLights(lights,count);
}
void MeasureProofCadence(){
 if(!hotCountersEnabled)return;
 static LARGE_INTEGER frequency{},last{};static std::array<double,4096> samples{};static size_t used=0;static uint32_t begin=0;static int phase=-1;
 LARGE_INTEGER now{};QueryPerformanceCounter(&now);if(!frequency.QuadPart)QueryPerformanceFrequency(&frequency);
 const int current=static_cast<int>(LabPhase());const auto tick=GetTickCount();
 if(current!=phase){phase=current;used=0;last=now;begin=tick;return;}
 if(last.QuadPart&&used<samples.size())samples[used++]=1000.0*double(now.QuadPart-last.QuadPart)/double(frequency.QuadPart);
 last=now;if(tick-begin<5000||used<2)return;
 auto sorted=samples;std::sort(sorted.begin(),sorted.begin()+used);double sum=0;for(size_t i=0;i<used;++i)sum+=samples[i];
 char text[250];sprintf_s(text,"phase=%d n=%zu mean_ms=%.3f p50_ms=%.3f p95_ms=%.3f max_ms=%.3f game_loop_intervals_not_gpu_timing",phase,used,sum/used,sorted[used/2],sorted[(used-1)*95/100],sorted[used-1]);Event(2,"PROOF_CADENCE",text);used=0;begin=tick;
}

// Clone-only producer observation. No writes to arguments, lights or pool.
SafetyHookMid ownerProbe;
bool ownerAuditEnabled=false;
constexpr std::array<uint32_t,6> sirenSites{0x640d0a,0x64126e,0x64161d,0x6418f0,0x642053,0x6425ab};
struct OwnerObservation {uint32_t tick{},site{},owner{},type{},flags{},key{};float position[3]{},color[3]{},radius{},intensity{};};
struct OwnerMailbox {std::atomic<unsigned> state{0};OwnerObservation value{};};
std::array<OwnerMailbox,6> ownerMailboxes;
void TrafficObserve(SafetyHookContext&,uint32_t) noexcept;
void TrafficPoll();
void ObserveOwner(SafetyHookContext& regs) noexcept {
 const fusionfix::shadows::FloatingPointState fp;
 if(!ownerAuditEnabled)return;
 const auto* stack=reinterpret_cast<const uint32_t*>(regs.esp);
 const uint32_t caller=stack[0]-static_cast<uint32_t>(game)-5;
 TrafficObserve(regs,caller);
 size_t index=0;while(index<sirenSites.size()&&sirenSites[index]!=caller)++index;
 if(index==sirenSites.size())return;
 if(policeTestEnabled)PromoteProof(regs,caller);
 if(!ownerTelemetryEnabled)return;
 auto& box=ownerMailboxes[index];unsigned empty=0;
 if(!box.state.compare_exchange_strong(empty,1,std::memory_order_acquire))return;
 const uint32_t now=GetTickCount();
 if(now-box.value.tick<250){box.state.store(0,std::memory_order_release);return;}
 auto& o=box.value;o.tick=now;o.site=caller;o.owner=regs.edi;
 o.type=stack[2];o.flags=stack[3];o.key=stack[16];
 // Exact guarded wrapper ABI: direction, tangent, position, RGB, intensity.
 memcpy(o.position,reinterpret_cast<const void*>(stack[6]),12);
 memcpy(o.color,reinterpret_cast<const void*>(stack[7]),12);
 memcpy(&o.intensity,&stack[8],4);memcpy(&o.radius,&stack[11],4);
 box.state.store(2,std::memory_order_release);
}
void PublishOwnerAudit(){
 if(!ownerTelemetryEnabled)return;
 const auto pool=CVehicle::GetVehiclePool();
 const bool valid=pool&&pool->m_aStorage&&pool->m_aFlags&&pool->m_nSize>0&&pool->m_nSize<=4096&&pool->m_nStorageSize>=0x24&&pool->m_nStorageSize<=0x10000;
 for(auto& box:ownerMailboxes){unsigned ready=2;if(!box.state.compare_exchange_strong(ready,3,std::memory_order_acquire))continue;
  const auto o=box.value;box.state.store(0,std::memory_order_release);
  int slot=-1,reference=-1;bool occupied=false;
  if(valid){const auto base=reinterpret_cast<uintptr_t>(pool->m_aStorage);const auto bytes=uint64_t(pool->m_nSize)*pool->m_nStorageSize;
   if(o.owner>=base&&uint64_t(o.owner-base)<bytes&&(o.owner-base)%pool->m_nStorageSize==0){slot=static_cast<int>((o.owner-base)/pool->m_nStorageSize);if(!pool->GetIsFree(slot)){reference=pool->GetReference(slot);occupied=CPlayer::findPlayerCar&&CPlayer::findPlayerCar()==o.owner;}}}
  char text[450];sprintf_s(text,"tick=%u site=%x pool_slot=%d reference=%d occupied=%d type=%u flags=%u input_key=%u pos=%.3f,%.3f,%.3f rgb=%.3f,%.3f,%.3f radius=%.3f intensity=%.5f",o.tick,o.site,slot,reference,occupied?1:0,o.type,o.flags,o.key,o.position[0],o.position[1],o.position[2],o.color[0],o.color[1],o.color[2],o.radius,o.intensity);
  Event(2,"SIREN_OWNER",text);
 }
}

void PublishLightAudit() {
 if(!auditEnabled)return;
 unsigned ready=2;if(!auditState.compare_exchange_strong(ready,3,std::memory_order_acquire))return;
 const auto snapshot=auditSnapshot;auditState.store(0,std::memory_order_release);
 char text[500];
 sprintf_s(text,"tick=%u frame=%u submitted=%u nearby=%u cap=64 radius=40",snapshot.tick,snapshot.frame,snapshot.total,snapshot.count);
 Event(2,"LIGHT_AUDIT_SAMPLE",text);
 for(uint32_t i=0;i<snapshot.count;++i){const auto& l=snapshot.lights[i];
  sprintf_s(text,"tick=%u key=%u type=%u flags=%u cache=%d pos=%.3f,%.3f,%.3f delta=%.3f,%.3f,%.3f rgb=%.3f,%.3f,%.3f intensity=%.3f radius=%.3f cone=%.3f corona=%d projection=%d",
   snapshot.tick,static_cast<uint32_t>(l.mCastShadows),static_cast<uint32_t>(l.mType),l.mFlags,l.mShadowCacheIndex,
   l.mPosition.x,l.mPosition.y,l.mPosition.z,l.mPosition.x-snapshot.player.x,l.mPosition.y-snapshot.player.y,l.mPosition.z-snapshot.player.z,
   l.mColor.x,l.mColor.y,l.mColor.z,l.mIntensity,l.mRadius,l.mOuterConeAngle,l.mCoronaTexHash,l.mProjTexHash);
  Event(2,"LIGHT_AUDIT",text);
 }
}

std::vector<uint8_t> ReadMemory(uintptr_t address,size_t size) {
#ifdef STANDALONE_OFFLINE
 for(const auto& r:records)if(address>=r.address&&address-r.address<=r.bytes.size()&&size<=r.bytes.size()-(address-r.address))return {r.bytes.begin()+address-r.address,r.bytes.begin()+address-r.address+size};
 throw std::runtime_error("offline_capture_range_missing");
#else
 std::vector<uint8_t> result(size);SIZE_T count{};
 if(!ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),result.data(),size,&count)||count!=size)throw std::runtime_error("unreadable_code");return result;
#endif
}
uintptr_t Branch(uintptr_t site,uint8_t opcode=0xe9) {
 const auto b=ReadMemory(site,5);if(b[0]!=opcode)throw std::runtime_error("unexpected_branch_opcode");return static_cast<uint32_t>(site+5+U32(b.data()+1));
}
void Require(bool success,const char* why){if(!success)throw std::runtime_error(why);}
// Establish each exact known overlay, including heap relay ownership. An E9
// alone never licenses replacing an unknown mod's branch.
void VerifyBranch(uint32_t rva,uint32_t callbackRva,bool mid,size_t overwrite,uint8_t opcode=0xe9) {
 auto bytes=ReadMemory(game+rva,overwrite);
 for(size_t i=5;i<overwrite;++i)Require(bytes[i]==0x90,"foreign_patch_padding");
 auto at=Branch(game+rva,opcode);
 if(!mid) {
  if(at!=ff+callbackRva)at=Branch(at);
  Require(at==ff+callbackRva,"foreign_inline_owner");return;
 }
 // Audited FF SafetyHook uses one relay E9 before its generated mid stub.
 at=Branch(at);
 const auto stub=ReadMemory(at,171);
 for(size_t i=0;i<171;++i){if((i>=2&&i<6)||(i>=89&&i<93)||i>=163)continue;Require(stub[i]==MidStubTemplate[i],"foreign_mid_stub");}
 Require(U32(stub.data()+2)==at+167&&U32(stub.data()+89)==at+163,"foreign_mid_pointer_slots");
 Require(U32(stub.data()+163)==ff+callbackRva,"foreign_mid_callback");
 const auto trampoline=U32(stub.data()+167);
 const auto native=ReadMemory(trampoline,16);
 size_t displaced=5;
 if(rva==0x527bd4){const uint8_t expected[]{0xa8,6,0x90,0x90,0x90};Require(!memcmp(native.data(),expected,5),"admission_continuation_changed");}
 else if(rva==0x6e3736||rva==0x4d0f82){for(size_t n=0;n<5;++n)Require(native[n]==0x90,"nop_continuation_changed");}
 else {displaced=rva==0x81e615?6:8;Require(!memcmp(native.data(),original->bytes.data()+rva,displaced),"native_continuation_changed");}
 Require(native[displaced]==0xe9,"continuation_branch_changed");
 Require(static_cast<uint32_t>(trampoline+displaced+5+U32(native.data()+displaced+1))==game+rva+displaced,"continuation_destination_changed");
}
std::vector<uint8_t> GuardedImage() {
 auto live=original->bytes;
 const auto headers=ReadMemory(game,4096);std::copy(headers.begin(),headers.end(),live.begin());
 for(const auto& sec:original->sections)if(sec.Characteristics&IMAGE_SCN_MEM_EXECUTE){const auto actual=ReadMemory(game+sec.VirtualAddress,sec.Misc.VirtualSize);std::copy(actual.begin(),actual.end(),live.begin()+sec.VirtualAddress);}
 struct Entry{PatchKind kind;uint32_t rva,callback;size_t length;bool mid;uint8_t op;};
 const Entry entries[]{
  {PatchKind::HeadlightCall,0x63dfea,layout->player,5,false,0xe8},{PatchKind::HeadlightCall,0x63e1ca,layout->player,5,false,0xe8},
  {PatchKind::HeadlightCall,0x63dfff,layout->npc,5,false,0xe8},{PatchKind::HeadlightCall,0x63e1df,layout->npc,5,false,0xe8},
  {PatchKind::Admission,0x527bd4,layout->admission,8,true,0xe9},{PatchKind::Lookup,0x525db0,layout->lookup,7,false,0xe9},
  {PatchKind::RenderScope,0x977a00,layout->render,10,false,0xe9},{PatchKind::CasterBranch,0x6e3736,layout->caster,25,true,0xe9},
  {PatchKind::ContactShadow,0x63de5e,layout->contact,8,true,0xe9},
  // Official FramerateVigilante replaces INC with a callback which increments
  // this same native counter plus its separate logical counter. Never touch it.
  {PatchKind::FrameCounter,0x4d0f82,layout->frame,6,true,0xe9}
 };
 std::vector<OwnedPatch> patches;
 if(nativeBackend)return live; // Original guards still authenticate all used ranges.
 for(const auto& e:entries){if(!e.callback)continue;VerifyBranch(e.rva,e.callback,e.mid,e.length,e.op);OwnedPatch p;p.kind=e.kind;p.rva=e.rva;p.expectedInstalled.assign(live.begin()+e.rva,live.begin()+e.rva+e.length);p.originalRelocated.assign(original->bytes.begin()+e.rva,original->bytes.begin()+e.rva+e.length);p.ownershipVerified=true;patches.push_back(std::move(p));}
 if(layout->frame){const auto counter=ReadMemory(ff+layout->frameData,4);Require(U32(counter.data())==game+fusionfix::shadows::ce::FrameCounterRva,"official_frame_counter_binding_changed");}
 const auto normalized=NormalizeForOriginalGuards(live,patches,true,true);Require(bool(normalized),"overlay_normalization_failed");return normalized.bytes;
}
uintptr_t OriginalCall(size_t rva);
void VerifyOfficial() {
 const auto module=GetModuleHandleW(L"GTAIV.EFLC.FusionFix.asi");if(!module){nativeBackend=true;Require(Branch(loop,0xe8)==OriginalCall(uint32_t(loop-game)),"foreign_native_game_process_hook");return;}ff=reinterpret_cast<uintptr_t>(module);
 const auto raw=pe::Read(pe::ModulePath(module));
 const auto process=Branch(loop,0xe8);layout=nullptr;
 for(const auto* candidate:{&Layout50,&Layout51,&Layout511})if(process==ff+candidate->process){layout=candidate;break;}
 Require(layout!=nullptr,"unsupported_FusionFix_process_layout");
 pe::Image expected(raw,ff);
 for(const auto& sec:expected.sections)if(sec.Characteristics&IMAGE_SCN_MEM_EXECUTE){const auto actual=ReadMemory(ff+sec.VirtualAddress,sec.Misc.VirtualSize);Require(!memcmp(actual.data(),expected.bytes.data()+sec.VirtualAddress,actual.size()),"FusionFix_code_modified");}
 Require(Branch(loop,0xe8)==ff+layout->process,"foreign_game_process_hook");
 shortlist_helper48::runtime::binding={game,ff,original->bytes.size(),expected.bytes.size(),
  layout==&Layout511&&pe::Hash(raw)==Layout511.hash};
}
#include "VolumetricCompat199Runtime.inc"
uintptr_t Operand(const char* signature,unsigned offset){return U32(bindings->bytes.data()+bindings->Find(signature)+offset);}
size_t Binding(const char* first,const char* second){
 try{return bindings->Find(first);}catch(const std::runtime_error& e){if(std::string(e.what()).rfind("pattern count=0:",0)!=0)throw;return bindings->Find(second);}
}
uintptr_t Operand(const char* first,const char* second,unsigned offset){const auto value=U32(bindings->bytes.data()+Binding(first,second)+offset);Require(value>=game&&value<=game+bindings->bytes.size()-4,"game_data_binding_outside_image");return value;}
uintptr_t OriginalCall(size_t rva){Require(original->bytes[rva]==0xe8,"bad_game_call");return static_cast<uint32_t>(game+rva+5+U32(original->bytes.data()+rva+1));}
uintptr_t OfficialGameData(uint32_t rva){const auto bytes=ReadMemory(ff+rva,4);const auto value=U32(bytes.data());Require(value>=game&&value<=game+original->bytes.size()-4&&(value&3)==0,"official_game_data_binding_invalid");return value;}
void ResolveGame() {
 const auto call=original->Find("E8 ? ? ? ? 85 C0 74 29 6A 00");
 CPlayer::getLocalPlayerPed=reinterpret_cast<uintptr_t(*)()>(OriginalCall(call));CPlayer::findPlayerCar=reinterpret_cast<uintptr_t(*)()>(OriginalCall(call+11));
 // Validate function entries used by this adapter; unrelated redirects fail closed.
 for(const auto address:{reinterpret_cast<uintptr_t>(CPlayer::getLocalPlayerPed),reinterpret_cast<uintptr_t>(CPlayer::findPlayerCar)}){const auto code=ReadMemory(address,16);Require(!memcmp(code.data(),original->bytes.data()+address-game,16),"game_player_entry_modified");}
 CTimer::m_snTimeInMilliseconds=reinterpret_cast<int32_t*>(Operand("A1 ? ? ? ? A3 ? ? ? ? EB 3A","A1 ? ? ? ? 39 05 ? ? ? ? 76 1F",1));
 CVehicle::pool=reinterpret_cast<rage::VehiclePool**>(Operand("8B 15 ? ? ? ? 46 3B 72 ? 7C ? 5E","8B 3D ? ? ? ? 8B CE FF D2 6A ? 6A ? 6A ? EB",2));
 rage::pCurrentViewport=reinterpret_cast<rage::grcViewport**>(Operand("8B 35 ? ? ? ? 75 14","8B 3D ? ? ? ? 75 14 6A 00",2));
 // FF's window patches replace the patterns after resolving these plain
 // game pointers. Exact 5.1 stores are audited at FF RVAs 5D93A and 5D97D.
 // This does not depend on the CSettings ABI or copy a user setting.
 if(nativeBackend){rage::grcDevice::ms_nActiveWidth=&ownWidth;rage::grcDevice::ms_nActiveHeight=&ownHeight;}
 else {rage::grcDevice::ms_nActiveWidth=reinterpret_cast<int32_t*>(OfficialGameData(layout->width));
 rage::grcDevice::ms_nActiveHeight=reinterpret_cast<int32_t*>(OfficialGameData(layout->height));}
 Require(*rage::grcDevice::ms_nActiveWidth>=16&&*rage::grcDevice::ms_nActiveWidth<=16384&&*rage::grcDevice::ms_nActiveHeight>=16&&*rage::grcDevice::ms_nActiveHeight<=16384,"active_dimensions_invalid");
 if(preset117::UseOwnedBackend117(nativeBackend)){bExtraNightShadows.value=&ownExtra;bHeadlightShadows.value=&ownHeadlight;bVehicleNightShadows.value=&ownVehicle;}
 else {bExtraNightShadows.value=reinterpret_cast<const bool*>(ff+layout->extra);bHeadlightShadows.value=reinterpret_cast<const bool*>(ff+layout->headlight);bVehicleNightShadows.value=reinterpret_cast<const bool*>(ff+layout->vehicle);}
 bExtraNightShadows.configured=&ownExtra;bHeadlightShadows.configured=&ownHeadlight;bVehicleNightShadows.configured=&ownVehicle;
 CShadows::pFrameCounter=reinterpret_cast<uint32_t*>(game+fusionfix::shadows::ce::FrameCounterRva);
 CShadows::hbStoreStaticShadow.fun=reinterpret_cast<CShadows::SubmitHeadlight>(game+fusionfix::shadows::ce::SubmitRva);
 PlayerShadowAllocation::gameBase=game;OwnHeadlightCaster::base=game;ShadowLookupGuard::base=game;
}
#define SUBMIT_ARGS int a1,int a2,uint32_t flags,int a4,int a5,int a6,int a7,int a8,int a9,int a10,int a11,int a12,int a13,int a14,int a15,int key
#define SUBMIT_PASS a1,a2,flags,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,key
void __cdecl Player(SUBMIT_ARGS){if(!addonActive.load(std::memory_order_acquire)){playerSubmit.ccall<void>(SUBMIT_PASS);return;}if(Liberty20::Split({a1,a2,int(flags),a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,key}))return;CShadows::StoreStaticShadowPlayerDriving(SUBMIT_PASS);}
void __cdecl NPC(SUBMIT_ARGS){if(!addonActive.load(std::memory_order_acquire)){npcSubmit.ccall<void>(SUBMIT_PASS);return;}
 if(a2==2&&flags==0x104&&TailLight20::CasterOwner(uint32_t(key))){CShadows::hbStoreStaticShadow.fun(SUBMIT_PASS);return;}
 if(Liberty20::Split({a1,a2,int(flags),a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,key}))return;CShadows::StoreStaticShadowNPC(SUBMIT_PASS);}
void __cdecl NativeStore(SUBMIT_ARGS){
 if(addonActive.load(std::memory_order_acquire)&&a2==2&&flags==0x104&&TailLight20::CasterOwner(uint32_t(key))){nativeSubmit.ccall<void>(SUBMIT_PASS);return;}
 if(!addonActive.load(std::memory_order_acquire)){nativeSubmit.ccall<void>(SUBMIT_PASS);return;}
 if(a2==2&&(flags==0x504||flags==0x104)){
  if(Liberty20::Split({a1,a2,int(flags),a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,key}))return;
  if(flags==0x504)CShadows::StoreStaticShadowPlayerDriving(SUBMIT_PASS);
  else CShadows::StoreStaticShadowNPC(SUBMIT_PASS);
 }else nativeSubmit.ccall<void>(SUBMIT_PASS);
}
int __cdecl Lookup(int a,int b,int c){if(!addonActive.load(std::memory_order_acquire))return shsub_925DB0.ccall<int>(a,b,c);return sub_925DB0(a,b,c);}
void __fastcall Render(void* object,void* edx);
void Caster(SafetyHookContext& regs) {
 if(!addonActive.load(std::memory_order_acquire))return;
 // Ordinary geometry cannot be an own vehicle/occupant. With the optional
 // pole observers/override and visit counters off, every remaining check is
 // a no-op. Keep the native allow continuation without touching FP state.
 if(regs.eax!=2&&regs.eax!=3&&!OwnHeadlightCaster::diagnosticCounters&&
    !PoleCasterDiagnostic134::requested&&!PoleStaticPartition141::requested){
  regs.eip=game+0x6e374f;return;
 }
 const fusionfix::shadows::FloatingPointState fp;
 const bool artificial=!*reinterpret_cast<const uint8_t*>(regs.esp+0x0b);
 bool skip=OwnHeadlightCaster::Exclude(regs.esi,regs.eax,artificial);
 if(!bVehicleNightShadows&&regs.eax==3&&(*reinterpret_cast<const uint8_t*>(regs.esi+620)&4)&&artificial)skip=true;
 PoleCasterDiagnostic134::Observe(regs.esi,regs.eax,artificial,skip);
 PoleDrawWitness145::SunCaster(regs.esi,uint32_t(regs.eax),uint32_t(regs.edx),regs.ebp,skip);
 uint32_t eligibility142{};
 if(PoleStaticPartition141::Reclassify(regs.esi,uint32_t(regs.eax),uint32_t(regs.edx),regs.ebp,skip,&eligibility142)){regs.eax=eligibility142;regs.eip=game+0x6e3767;return;}
 regs.eip=game+(skip?0x6e3867:0x6e374f);
}
void Contact(SafetyHookContext& regs) {
 if(!addonActive.load(std::memory_order_acquire))return;
 if(bHeadlightShadows&&!bVehicleNightShadows)regs.xmm2.f32[0]*=3.0f;
 *reinterpret_cast<uint32_t*>(regs.esp+0x28)=0; // Exact displaced native instruction.
 regs.eip=game+0x63de66; // Skip stock compensation, already handled once above.
}
void Brightness(SafetyHookContext& regs){if(!addonActive.load(std::memory_order_acquire))return;const fusionfix::shadows::FloatingPointState fp;const DWORD error=GetLastError();HeadlightEnhancement::RetainAfterExit(regs);const float intensity=regs.xmm1.f32[0];if(std::isfinite(intensity)&&intensity>=0)regs.xmm1.f32[0]=intensity*.85f;SetLastError(error);}
void Warning(SafetyHookContext& regs) {
 if(!addonActive.load(std::memory_order_acquire))return;
 if(SuppressStockShadowWarning(true,PlayerShadowAllocation::ready.load(std::memory_order_acquire),
    PlayerShadowAllocation::publicationEnabled,PlayerShadowAllocation::unsupportedThread.load(std::memory_order_relaxed),
    OwnHeadlightCaster::enabled.load(std::memory_order_acquire),bExtraNightShadows,bHeadlightShadows,bVehicleNightShadows)){
  static constexpr wchar_t empty[]=L"";
  regs.eax=reinterpret_cast<uintptr_t>(empty);++warningHidden;
 }
}
void Camera(SafetyHookContext& regs) {
 if(!addonActive.load(std::memory_order_acquire))return;
 const fusionfix::shadows::FloatingPointState fp;
 const auto* viewport=reinterpret_cast<const rage::grcViewport*>(regs.ecx);
 if(viewport!=PlayerShadowAllocation::SceneViewport())return;
 const auto view=PlayerShadowAllocation::ReadView(viewport);
 if(view.valid&&!PlayerShadowAllocation::gameplayViewLock.test_and_set(std::memory_order_acquire)){
  PlayerShadowAllocation::gameplayView=view;PlayerShadowAllocation::gameplayViewFrame=*CShadows::pFrameCounter;PlayerShadowAllocation::gameplayViewLock.clear(std::memory_order_release);
 }
}
template<class Hook,class Function> void Prepare(Hook& hook,uintptr_t target,Function function) {
 auto result=Hook::create(target,function,Hook::StartDisabled);Require(bool(result),"hook_prepare_failed");hook=std::move(*result);
}
#include "traffic-signal-observer.inc"
#include "LibertyLighting20Runtime.inc"
#include "PoleCasterDiagnostic134Runtime.inc"
#include "TailLightRuntime20.inc"
#include "ModelLightShadowRuntime20.inc"
#include "ModelLightCatalogRuntime20.inc"
#include "NativeProjectionRuntime105.inc"
#include "EffectAttribution199CRuntime.inc"
#include "ModelReferenceQueue189Runtime.inc"
#include "ModelProjectionBridge69.inc"
#include "ConsumerReachRuntime136.inc"
#include "SceneryFade198BRuntime.inc"
#include "ConsumerShadow75Runtime.inc"
#include "NativeProducerGate116Runtime.inc"
#include "CoronaLightRuntime20.inc"
#include "WorldGeometryCatalogRuntime20.inc"
#include "PresentFrameTime151Runtime.inc"
#include "FrameBoundary154Runtime.inc"
#include "GpuProfilingRuntime39.inc"
#include "LightingCapabilities16.inc"
#include "NativeNightPrerequisite128Runtime.inc"
#include "FusionBackend130Runtime.inc"
#include "PoleStaticPartition141Runtime.inc"
#include "PoleDrawWitness145Runtime.inc"
#include "PoleDepthRepair157Runtime.inc"
#include "MapContribution198CRuntime.inc"
uintptr_t LightCasterOwner(uintptr_t key) noexcept {
 const auto beam=Liberty20::BeamOwner(key);
 return beam?beam:TailLight20::CasterOwner(key);
}
std::atomic<uint32_t> projectionRankingChecks133{0},projectionRankingQueries133{0},projectionRankingAccepted133{0},projectionRankingRefused133{0};
void PublishVisibleReceiver133() {
 if(!PlayerShadowAllocation::visibleReceiverExperiment133||!hotCountersEnabled||!log||!log->Enabled())return;
 static std::atomic<uint32_t> previous{0};const auto tick=GetTickCount();auto before=previous.load(std::memory_order_relaxed);
 if(uint32_t(tick-before)<5000u||!previous.compare_exchange_strong(before,tick,std::memory_order_relaxed))return;
 Event(2,"VISIBLE_RECEIVER133","projected_checks="+std::to_string(projectionRankingChecks133.load())+
  " projected_permission_queries="+std::to_string(projectionRankingQueries133.load())+
  " projected_accepted="+std::to_string(projectionRankingAccepted133.load())+
  " projected_refused="+std::to_string(projectionRankingRefused133.load())+" cumulative_non_atomic=1 ranking_only=1 no_fps_claim=1");
}
// RECEIVER133_SOURCE_BEGIN
bool VisibleReceiverSource133(const rage::CLightSource& light,unsigned index,rage::CLightSource* lights,uint32_t count,uint32_t frame) noexcept {
 if(!lights||count>640||index>=count||uint64_t(reinterpret_cast<uintptr_t>(&light))!=
    uint64_t(reinterpret_cast<uintptr_t>(lights))+uint64_t(index)*sizeof(rage::CLightSource))return false;
 const auto key=uint32_t(light.mCastShadows);
 if(!key||(light.mFlags&0x500u)||TailLight20::Contains(key)||
    Liberty20::publishedKeys127.Contains(uint32_t(reinterpret_cast<uintptr_t>(Liberty20::keyStorage)),key))return false;
 if(ModelLightShadow20::active.load(std::memory_order_acquire)&&ModelLightShadow20::registry.Contains(key))return true;
 if(TrafficSignal::LampGeneration(light)!=0)return true;
 // Generic current projector, not a physical family. This path is called
 // only after a positive current numeric receiver-benefit prefilter.
 if(NativeProjection66::Classify(index,key,lights,count,frame)!=1||
    NativeProjection66::sidecar.decisions[index]!=native_projection66::Decision::Valid)return false;
 const ModelEffectObserver::LastErrorScope error;
 ++projectionRankingChecks133;unsigned queries{};NativeProjection66::Packet packet{};
 const bool copied=NativeProjection66::CopyPacket(light,packet,true,&queries);
 projectionRankingQueries133.fetch_add(queries,std::memory_order_relaxed);
 const bool valid=copied&&packet[24]==key&&
   fusionfix::shadows::VisibleReceiverEmission133(std::bit_cast<float>(packet[16]),
    {std::bit_cast<float>(packet[12]),std::bit_cast<float>(packet[13]),std::bit_cast<float>(packet[14])})&&
   NativeProjection66::active.load(std::memory_order_acquire)&&
   NativeProjection66::sidecar.Current(index,packet,reinterpret_cast<uintptr_t>(lights),count,frame,0)==native_projection66::Decision::Valid;
 if(valid)++projectionRankingAccepted133;else ++projectionRankingRefused133;
 return valid; // stack0 leaves insertion anchor untouched; later witness remains.
}
// RECEIVER133_SOURCE_END
// Functional consumer136 permission: complete current packet, unique bank key,
// fresh source family/sidecar, positive emission and current gameplay view.
// No historical key or selected slot can substitute for these witnesses.
bool ConsumerReach136::Authorize(const consumer_shadow75::Packet& packet,uint32_t frame) noexcept {
 if(!requested||!addonActive.load(std::memory_order_acquire)||!CShadows::pFrameCounter||
    *CShadows::pFrameCounter!=frame||!PlayerShadowAllocation::visibleReceiverExperiment133)return false;
 auto* bank=PlayerShadowAllocation::CurrentLights();const auto count=PlayerShadowAllocation::CurrentCount();
 if(!bank||!count||count>640)return false;
 // Numeric snapshots locate a candidate only. Fresh copies below authorize it.
 thread_local std::array<rage::CLightSource,640> snapshot;
 if(!Liberty20::Copy(reinterpret_cast<uintptr_t>(bank),snapshot.data(),count*sizeof(rage::CLightSource)))return false;
 unsigned match=count,matches=0;
 for(unsigned i=0;i<count;++i)if(uint32_t(snapshot[i].mCastShadows)==packet.words[24]){match=i;++matches;}
 if(matches!=1)return false;
 rage::CLightSource light{};
 if(!Liberty20::Copy(reinterpret_cast<uintptr_t>(bank+match),&light,sizeof(light))||
    std::memcmp(&light,packet.words.data(),sizeof(packet))||
    !VisibleReceiverSource133(bank[match],match,bank,count,frame)||
    !fusionfix::shadows::VisibleReceiverEmission133(light.mIntensity,{light.mColor.x,light.mColor.y,light.mColor.z}))return false;
 // Use the authenticated gameplay viewport, never a shadow/reflection viewport.
 const auto* viewport=PlayerShadowAllocation::SceneViewport();rage::grcViewport copiedView{};
 if(!viewport||!Liberty20::Copy(reinterpret_cast<uintptr_t>(viewport),&copiedView,sizeof(copiedView)))return false;
 const auto view=PlayerShadowAllocation::ReadView(&copiedView);
 if(!view.valid||!fusionfix::shadows::ShadowVolumeMayReachView(
    fusionfix::shadows::PrepareShadowVolumeFrustum20(view),
    {light.mPosition.x,light.mPosition.y,light.mPosition.z},light.mRadius))return false;
 // Final fresh owner/packet/scene guard follows every potentially nested read.
 rage::CLightSource finalLight{};
 return *CShadows::pFrameCounter==frame&&PlayerShadowAllocation::CurrentLights()==bank&&
    PlayerShadowAllocation::CurrentCount()==count&&PlayerShadowAllocation::SceneViewport()==viewport&&
    Liberty20::Copy(reinterpret_cast<uintptr_t>(bank+match),&finalLight,sizeof(finalLight))&&
    !std::memcmp(&finalLight,packet.words.data(),sizeof(packet))&&
    VisibleReceiverSource133(bank[match],match,bank,count,frame);
}
void ApplyLiveReach122Boundary() noexcept {
 const ModelEffectObserver::LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 if(!preset117::startup.present||!CShadows::pFrameCounter||!addonActive.load(std::memory_order_acquire))return;
 preset117::liveReach122.Boundary(preset117::startup.state,*CShadows::pFrameCounter,true,
  [](unsigned lamp,unsigned headlight) noexcept {(void)FusionFixSettings.SetReachPair122(lamp,headlight);});
}
void RefreshApplied119State() noexcept {
 const ModelEffectObserver::LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 if(!preset117::startup.present)return;
 NativeNightPrerequisite128::Refresh();
 if(!addonActive.load(std::memory_order_acquire)){
  // Off has no installed refresh hook. If an active manager later fails,
  // report that actual state while retaining the user's requested settings.
  if(preset117::applied.enabled){preset117::applied.enabled=false;preset117::backendLimited=true;}
  return;
 }
 auto actual=preset117::liveReach122.Acknowledged(preset117::startup.state);
 actual.features={Liberty20::dualActive.load(),TailLight20::active.load(),TrafficSignal::enabled,nearbyPoliceEnabled&&bool(ownerProbe),ModelLightShadow20::active.load()&&NativeProjection66::active.load()};
 if(!bExtraNightShadows)actual.features[size_t(preset117::Feature::WorldLighting)]=false;
 if(!bHeadlightShadows||!bVehicleNightShadows)actual.features[size_t(preset117::Feature::DualHeadlights)]=false;
 preset117::vehicleDepthUnavailable119=false;preset117::backendReadFailed119=false;
 // The officialFF511 local vehicle-depth hook separately reads this plain
 // gate (nightshadows.ixx121). OwnedSchema3 scalars cannot enable it.
 // No privateCSettings access or write: layout/module were authenticated.
 uint8_t vehicleGate{};
 const bool gateRead=!nativeBackend&&layout&&ff&&reinterpret_cast<uintptr_t>(GetModuleHandleW(L"GTAIV.EFLC.FusionFix.asi"))==ff&&Liberty20::Copy(ff+layout->vehicle,&vehicleGate,1)&&vehicleGate<=1;
 preset117::backendReadFailed119=!gateRead;
 preset117::vehicleDepthUnavailable119=gateRead&&!vehicleGate;
 if(!gateRead||!vehicleGate){actual.features[size_t(preset117::Feature::DualHeadlights)]=false;actual.features[size_t(preset117::Feature::TailLights)]=false;}
 preset117::backendLimited=actual.features!=preset117::startup.state.features||!bExtraNightShadows||!bHeadlightShadows||!gateRead||!vehicleGate||FusionBackend130::Limited();
 if(preset117::backendLimited)actual.preset=preset117::Preset::Custom;
 preset117::applied=actual;
}
void PrepareLightSources(rage::CLightSource* lights,uint32_t count) noexcept {
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::Sources,32);
 if(preset117::startup.present){
  if(TrafficSignal::requested)TrafficSignal::ValidateQueued(lights,count);
  if(TailLight20::requested)TailLight20::ValidateQueued(lights,count);
  if(ModelLightShadow20::requested||CoronaLight20::requested)ModelLightShadow20::ValidateQueued(lights,count);
  if(Liberty20::worldRequested)Liberty20::PromoteNativeWorld(lights,count);
  if(NativeProjection66::requested)NativeProjection66::ValidateQueued(lights,count);
  return;
 }
 TrafficSignal::ValidateQueued(lights,count);
 TailLight20::ValidateQueued(lights,count);
 ModelLightShadow20::ValidateQueued(lights,count);
 Liberty20::PromoteNativeWorld(lights,count);
 NativeProjection66::ValidateQueued(lights,count);
}
void __fastcall Render(void* object,void* edx){
 if(!addonActive.load(std::memory_order_acquire)){shsub_D77A00.unsafe_fastcall(object,edx);return;}
 auto phase=shadow_cpu29::Phase::RenderOther;
 if(shadow_cpu29::enabled&&object){
  const auto slot=*reinterpret_cast<const uint32_t*>(uintptr_t(object)+0x940);
  if(slot>=1&&slot<=7){
   const auto kind=*reinterpret_cast<const uint32_t*>(game+0xd9f1fc+slot*0x110);
   if(kind==3)phase=shadow_cpu29::Phase::RenderStatic;
   else if(kind==4)phase=shadow_cpu29::Phase::RenderCachedDynamic;
   else if(kind==5)phase=shadow_cpu29::Phase::RenderUncached;
  }
 }
 const shadow_cpu29::Scope cpu(phase,16);
 const MapContribution198C::RenderScope contribution198C;
 const GpuProfiling39::StageScope gpuSpan(object);
 const PoleCasterDiagnostic134::Scope poleDiagnostic134(object);
 const PoleStaticPartition141::Scope polePartition141(object);
 const WorldGeometry20::Scope geometry(object);sub_D77A00(object,edx);
}
void PreCacheSelection(SafetyHookContext& regs) noexcept {
 // One hook owns this native boundary. Emergency vacancy runs first; stable
 // ordinals operate on precisely the remaining native-selected winners.
 if(policeTestEnabled)ApplyVacancy(regs);
 PlayerShadowAllocation::StabilizeMapOrdinals(regs);
}
void DisableAll() noexcept {
 EffectAttribution199C::Disable();
 ModelReferenceQueue189::Disable();
 FusionBackend130::Close();
 SceneryFade198B::Disable();NativeProducerGate116::Disable();ConsumerShadow75::Disable();
 NativeProjection66::Disable();
 FrameBoundary154::enabled=false;FrameTime151::Disable();GpuProfiling39::Disable();
 CoronaLight20::Disable();
 ModelLightShadow20::active=false;TrafficSignal::ownerClass109Active=false;TrafficSignal::ownerClass114Active=false;
 TailLight20::Disable();Liberty20::Disable();ModelEffectObserver::DisableHooks();WorldGeometry20::Disable();if(nativeSubmit)(void)nativeSubmit.disable();
 if(TrafficSignal::freeHook)(void)TrafficSignal::freeHook.disable();
 if(TrafficSignal::timerRestoreHook)(void)TrafficSignal::timerRestoreHook.disable();TrafficSignal::enabled=false;
 addonActive.store(false,std::memory_order_release);PlayerShadowAllocation::ready=false;OwnHeadlightCaster::enabled=false;NearbyVehicleLighting36::enabled=false;
 if(ownerProbe)(void)ownerProbe.disable();if(vacancyHook)(void)vacancyHook.disable();
 // Retain trampoline allocations even after rollback, protecting in-flight callers.
 for(auto* hook:{&playerSubmit,&npcSubmit,&shsub_925DB0,&shsub_D77A00,&PlayerShadowAllocation::selectionHook})if(*hook)(void)hook->disable();
 for(auto* hook:{&caster,&contact,&camera,&brightness,&warning,&PlayerShadowAllocation::collectHook,&PlayerShadowAllocation::finalizeHook,&PlayerShadowAllocation::lampDistanceHook,&PlayerShadowAllocation::compareResultHook,&PlayerShadowAllocation::cacheResultHook,&PlayerShadowAllocation::stableOrdinalsHook})if(*hook)(void)hook->disable();
}
void Install() {
 VerifyLightingCapabilities(pe::Read(pe::ModulePath(nullptr)));
 VerifyOfficial();auto image=GuardedImage();VolumetricCompat199::AuthenticateAndNormalize(image);FusionBackend130::PrepareHooks();PoleStaticPartition141::Prepare(image);PoleDepthRepair157::PrepareHooks();PoleDrawWitness145::PrepareHooks();
 Liberty20::PrepareHooks(image);
 if(ModelLightShadow20::requested||CoronaLight20::requested)Require(ModelLightShadow20::registry.Configure(uint32_t(reinterpret_cast<uintptr_t>(ModelLightShadow20::keyStorage))),"model_shadow_key_storage_invalid");
 TailLight20::PrepareHooks(image);
 TrafficAuthenticate(); // Authenticate official pool overlay before private hooks.
 if(ModelLightShadow20::requested||CoronaLight20::requested){
 try {
  Require(TrafficSignal::authenticated,"native_class5_pool_layout_not_authenticated");
  native_owner109::Authenticate(image,[](uint32_t rva,uint32_t size){return ReadMemory(game+rva,size);});
  TrafficSignal::ownerClass109Authenticated=true;
  Event(1,"MODEL_OWNER_CLASS109_AUTHENTICATED","entity_type=5 pool_rva=126d9ec stride=128 native_inline_position=1 existing_free_boundary=70ba07");
 }catch(const std::exception& e){
  TrafficSignal::ownerClass109Authenticated=false;Event(1,"MODEL_OWNER_CLASS109_REFUSED",e.what());
 }
 try {
  Require(TrafficSignal::authenticated,"native_instance114_pool_layout_not_authenticated");
  native_owner114::Authenticate(image,[](uint32_t rva,uint32_t size){return ReadMemory(game+rva,size);});
  TrafficSignal::ownerClass114Authenticated=true;
  Event(1,"MODEL_OWNER_CLASS114_AUTHENTICATED","entity_type=1 vtable_rva=ad46a4 pool_rva=12f7d60 stride=272 existing_free_boundary=70ba07 native_emission_unchanged=1");
 }catch(const std::exception& e){TrafficSignal::ownerClass114Authenticated=false;Event(1,"MODEL_OWNER_CLASS114_REFUSED",e.what());}
 }
 // Support detail contains only code-relative offsets and numeric bytes.
 // This runs once, with explicit Logging=2, never in a shadow hot path.
 if(!fusionfix::shadows::ce::ValidateMappedImage(image.data(),image.size(),game)){
  const auto nt=U32(image.data()+0x3c);
  Event(2,"HEADLIGHT_HEADER","size="+std::to_string(image.size())+" timestamp="+std::to_string(U32(image.data()+nt+8))+" imageSize="+std::to_string(U32(image.data()+nt+24+56)));
  const std::pair<uint32_t,size_t> ranges[]={{0x6bcc50,24},{0x63dfe1,17},{0x63e1c1,17},{0x63dff6,17},{0x63e1d6,17},{0x769f26,6},{0x4d0f82,6}};
  for(const auto& [rva,size]:ranges){std::string mismatch="rva="+std::to_string(rva);for(size_t n=0;n<size;++n)if(image[rva+n]!=original->bytes[rva+n])mismatch+=" offset="+std::to_string(n)+" expected="+std::to_string(original->bytes[rva+n])+" actual="+std::to_string(image[rva+n]);Event(2,"HEADLIGHT_COMPARE",mismatch);}
 }
 Require(fusionfix::shadows::ce::ValidateMappedImage(image.data(),image.size(),game),"headlight_guard_failed");
 Require(fusionfix::shadows::ce::allocation::ValidateMappedImage(image.data(),image.size(),game),"allocator_guard_failed");
 Require(fusionfix::shadows::ce::casterguard::Validate(image.data(),image.size(),game),"caster_guard_failed");
 // The original lookup validator is pointer-based; validate its exact bytes
 // against the normalized copy using the real relocation base.
 for(unsigned i=0;i<sizeof(shadow_lookup_layout::Original);){bool reloc=false;for(auto r:shadow_lookup_layout::Relocs)if(i==r)reloc=true;
  if(reloc){Require(U32(image.data()+0x525db0+i)==U32(shadow_lookup_layout::Original+i)-0x400000+game,"lookup_relocation_guard_failed");i+=4;}
  else {Require(image[0x525db0+i]==shadow_lookup_layout::Original[i],"lookup_guard_failed");++i;}}
 // Steam restores some low-RVA code only after launch. Resolve those game
 // globals from the verified post-init image, never encrypted disk bytes.
 bindings=std::make_unique<pe::Image>(*original);bindings->bytes=image;ResolveGame();
 // Additional CE44 hooks retain their full original guards.
 const auto cameraCode=ReadMemory(game+0x30c6f,8);
 Require(cameraCode[0]==0x89&&cameraCode[1]==0x0d&&U32(cameraCode.data()+2)==reinterpret_cast<uintptr_t>(rage::pCurrentViewport)&&cameraCode[6]==0xc6&&cameraCode[7]==5,"camera_guard_failed");
 constexpr uint8_t driver[]{0x74,0x33,0x8B,0x8E,0x50,0x0F,0,0,0x85,0xC9,0x74,0x29,0x80,0xB9,0x18,0x02,0,0,0,0x75,0x20,0x80,0xB9,0x19,0x02,0,0,0,0x74,0x17};
 const auto driverCode=ReadMemory(game+0x63fb42,sizeof(driver));const auto brightnessCode=ReadMemory(game+0x63fb77,7);
 Require(!memcmp(driverCode.data(),driver,sizeof(driver))&&brightnessCode[0]==0x80&&brightnessCode[1]==0x3d&&U32(brightnessCode.data()+2)==game+0xc3cc49&&brightnessCode[6]==0,"brightness_guard_failed");
 if(!nativeBackend){
 const auto warningCall=ReadMemory(ff+layout->warningPush,20);
 const uint8_t warningAfter[]{0x8b,0xf0,0x83,0xc4,0x04,0x66,0x83,0x3e,0x00};
 Require(warningCall[0]==0x68&&U32(warningCall.data()+1)==ff+layout->warningKey&&Branch(ff+(layout->warningPush+5),0xe8)==ff+layout->textLookup&&
   !memcmp(warningCall.data()+10,warningAfter,sizeof(warningAfter)),"warning_site_guard_failed");
 }
 using namespace PlayerShadowAllocation;
 Prepare(selectionHook,game+allocation::SelectionRva,Select);Prepare(collectHook,game+allocation::CollectRva,Collect);
 Prepare(finalizeHook,game+allocation::FinalizeRva,Finalize);Prepare(lampDistanceHook,game+allocation::LampDistanceRva,AdjustLampDistance);
 Prepare(compareResultHook,game+allocation::CompareResultRva,CompareNativeCandidates);Prepare(cacheResultHook,game+allocation::CacheResultRva,ProtectCacheDependencies);
 Prepare(stableOrdinalsHook,game+allocation::StableOrdinalsRva,PreCacheSelection);
 if(nativeBackend){Prepare(nativeSubmit,game+fusionfix::shadows::ce::SubmitRva,NativeStore);
  CShadows::hbStoreStaticShadow.fun=reinterpret_cast<CShadows::SubmitHeadlight>(nativeSubmit.trampoline().address());}
 else {Prepare(playerSubmit,ff+layout->player,Player);Prepare(npcSubmit,ff+layout->npc,NPC);}
 Prepare(shsub_925DB0,game+0x525db0,Lookup);Prepare(shsub_D77A00,game+0x977a00,Render);
 Prepare(caster,game+0x6e3736,Caster);Prepare(contact,game+0x63de5e,Contact);
 Prepare(camera,game+0x30c6f,Camera);Prepare(brightness,game+0x63fb77,Brightness);
 if(!nativeBackend)Prepare(warning,ff+(layout->warningPush+10),Warning);
 ModelEffectObserver::PrepareHooks();
 CoronaLight20::PrepareHooks();
 WorldGeometry20::PrepareHooks();
 ModelReferenceQueue189::PrepareHooks();
 NativeProjection66::PrepareHooks();ArcadePlacement197::Prepare();
 EffectAttribution199C::PrepareHooks();
 ConsumerShadow75::PrepareHooks();SceneryFade198B::PrepareHooks(ConsumerShadow75::prepared);NativeProducerGate116::PrepareHooks();
 if(ownerAuditEnabled){
  // Entire wrapper and candidate producer bodies must remain the exact mapped executable.
  const std::pair<uint32_t,size_t> ranges[]={{0x6bcc50,0x7f},{0x640790,0x1e20},{0x644b36,0x35},{0x640620,0x167}};
  for(const auto& [rva,size]:ranges){const auto live=ReadMemory(game+rva,size);Require(!memcmp(live.data(),original->bytes.data()+rva,size),"siren_producer_guard_failed");}
  for(const auto rva:lab_siren::PoliceSites)Require(OriginalCall(rva)==game+0x6bcc50,"siren_callsite_target_failed");
  if(nearbyPoliceEnabled)Require(lab_siren::PoliceImageGuard(image,original->bytes,static_cast<uint32_t>(game)),"police_model_table_guard_failed");
  const auto vacancyBytes=ReadMemory(game+0x527dfc,6);Require(!memcmp(vacancyBytes.data(),original->bytes.data()+0x527dfc,6),"vacancy_pre_cache_guard_failed");
  Require(allocation::StableOrdinalsRva==0x527dfc,"pre_cache_shared_boundary_changed");
  Prepare(ownerProbe,game+0x6bcc50,ObserveOwner);
  Require(bool(ownerProbe.enable()),"siren_probe_enable_failed");
 }
 const std::array inlinePlan{&ModelReferenceQueue189::drainHook,&ModelReferenceQueue189::hook,&nativeSubmit,&playerSubmit,&npcSubmit,&shsub_925DB0,&shsub_D77A00,&selectionHook,&FusionBackend130::settingHook,&FusionBackend130::admissionHook,&PoleDrawWitness145::consumerHook};
 const std::array midPlan{&caster,&contact,&camera,&brightness,&warning,&collectHook,&finalizeHook,&lampDistanceHook,&compareResultHook,&cacheResultHook,&stableOrdinalsHook,&PoleDrawWitness145::objectHook,&PoleDrawWitness145::constructorHook,&PoleDrawWitness145::childHook,&PoleDepthRepair157::dispatchHook};
 unsigned requiredHooks=0;
 std::vector<PreparedHookRange> preparedRanges;
 for(const auto* hook:inlinePlan)if(*hook)preparedRanges.push_back({hook->target_address(),hook->original_bytes().size()});
 for(const auto* hook:midPlan)if(*hook)preparedRanges.push_back({hook->target_address(),hook->original_bytes().size()});
 if(ownerProbe)preparedRanges.push_back({ownerProbe.target_address(),ownerProbe.original_bytes().size()});
 if(NativeProjection66::constructorHook)preparedRanges.push_back({NativeProjection66::constructorHook.target_address(),NativeProjection66::constructorHook.original_bytes().size()});
 for(const auto* hook:{&Liberty20::dualPre,&Liberty20::dualPost,&Liberty20::singlePre,&Liberty20::freeObserver,
     &TailLight20::combinedHook,&TailLight20::singleHook,&ModelEffectObserver::preHook,&ModelEffectObserver::postHook,
     &ModelEffectObserver::resolveHook,&ModelEffectObserver::wrapperHook,&ModelEffectObserver::directHook,
     &ModelEffectObserver::wrapperSpotHook,&ModelEffectObserver::directSpotHook,
     &WorldGeometry20::collection,&WorldGeometry20::draw,&WorldGeometry20::mask,&CoronaLight20::activeHook,
     &ConsumerShadow75::lookupHook,&ConsumerShadow75::gateHook,&ConsumerShadow75::dispatchHook,
     &NativeProducerGate116::planeHook,&NativeProducerGate116::sphereHook,&EffectAttribution199C::insertionHook,&SceneryFade198B::binderHook})
  if(*hook)preparedRanges.push_back({hook->target_address(),hook->original_bytes().size()});
 Require(DisjointPreparedHooks(preparedRanges),"prepared_hook_ranges_overlap");
 GpuProfiling39::PrepareHooks(preparedRanges);
 FrameTime151::PrepareHooks(preparedRanges,GpuProfiling39::prepared);FrameBoundary154::Prepare();
 for(const auto* hook:inlinePlan)if(*hook)++requiredHooks;
 for(const auto* hook:midPlan)if(*hook)++requiredHooks;
 ActivationGate gate(requiredHooks);
 for(auto* hook:inlinePlan)if(*hook)Require(gate.HookEnabled(bool(hook->enable())),"inline_enable_failed");
 for(auto* hook:midPlan)if(*hook)Require(gate.HookEnabled(bool(hook->enable())),"mid_enable_failed");
 Require(gate.Commit(),"activation_incomplete");
 Require(FusionBackend130::Commit(),"fusion_backend130_commit_refused");
 requiredHooks+=GpuProfiling39::EnableHooks();
 requiredHooks+=FrameTime151::Enable(GpuProfiling39::enabled.load());
 cameraPriority=true;nativeLampPriority=true;publicationEnabled=true;ShadowLookupGuard::ready=true;HeadlightEnhancement::imageBase=game;
 bCloseHeadlightRelevance=true;bTrafficSelfShadowFix=true;
 OwnHeadlightCaster::enabled.store(true,std::memory_order_release);NearbyVehicleLighting36::enabled.store(true,std::memory_order_release);ready.store(true,std::memory_order_release);
 addonActive.store(true,std::memory_order_release);Event(1,"ACTIVE",std::string("build=")+CandidateBuild32+" game=1.2.0.59 backend="+(nativeBackend?"native":layout->version)+" required_hooks="+std::to_string(requiredHooks)+" shared_pre_cache=1");
 TrafficInstall();
 Liberty20::EnableHooks();TailLight20::EnableHooks(bool(Liberty20::freeObserver));ModelEffectObserver::EnableHooks();WorldGeometry20::EnableHooks();
 OwnHeadlightCaster::additionalCasterOwner=LightCasterOwner;
 ModelLightShadow20::active=(ModelLightShadow20::requested||CoronaLight20::requested)&&ModelEffectObserver::enabled.load()&&bool(Liberty20::freeObserver);
 TrafficSignal::ownerClass109Active=TrafficSignal::ownerClass109Authenticated&&ModelLightShadow20::active.load();
 TrafficSignal::ownerClass114Active=TrafficSignal::ownerClass114Authenticated&&ModelLightShadow20::active.load();
 CoronaLight20::EnableHooks();
 WireModelProjection69();
 NativeProjection66::Enable();
 EffectAttribution199C::Enable();
 ConsumerShadow75::Enable();SceneryFade198B::Enable(ConsumerShadow75::active.load(std::memory_order_acquire));NativeProducerGate116::Enable();
 preset117::refreshApplied119Hook=RefreshApplied119State;
 NativeNightPrerequisite128::Prepare();
 RefreshApplied119State();
 preset117::EngineActivated117(game,original->bytes.data(),original->bytes.size());
 PlayerShadowAllocation::bulkModelSources48=(!preset117::startup.present||ModelLightShadow20::requested||CoronaLight20::requested)?ModelLightShadow20::ValidateQueued:nullptr;
 PlayerShadowAllocation::validateModelSource48=(!preset117::startup.present||ModelLightShadow20::requested||CoronaLight20::requested)?ModelLightShadow20::ValidateOne48:nullptr;
 PlayerShadowAllocation::shortlistGetterSafe48=shortlist_helper48::runtime::GetterSafe48;
 // Keep the new native insertion path inactive until its reader-bank and
 // prefix callgraph contracts have been authenticated from live evidence.
 PlayerShadowAllocation::shortlistRequested48=false;
 Event(1,"MODEL_SHORTLIST48","requested=0 reason=native_bank_lifetime_proof_pending bulk_validation_preserved=1");
 PlayerShadowAllocation::projectionClass66=(!preset117::startup.present||NativeProjection66::requested)?NativeProjection66::Classify:nullptr;
 PlayerShadowAllocation::projectionCollect66=(!preset117::startup.present||NativeProjection66::requested)?NativeProjection66::CollectDecision:nullptr;
 PlayerShadowAllocation::projectionAlias66=(!preset117::startup.present||NativeProjection66::requested)?NativeProjection66::Alias:nullptr;
 PlayerShadowAllocation::prepareSources=(!preset117::startup.present||PlayerShadowAllocation::additionalFeatures||Liberty20::worldRequested||NativeProjection66::requested)?PrepareLightSources:nullptr;
 PlayerShadowAllocation::liveReachBoundary122=preset117::startup.present?ApplyLiveReach122Boundary:nullptr;
}
void PublishCpuProfile29() {
 if(!shadow_cpu29::enabled)return;
 static uint32_t previousTick{},previousFrame{};
 static std::array<uint32_t,unsigned(shadow_cpu29::Phase::Count)> previousCalls{},previousSamples{};
 static std::array<uint64_t,unsigned(shadow_cpu29::Phase::Count)> previousTicks{};
 const uint32_t tick=GetTickCount();const uint32_t frame=*CShadows::pFrameCounter;
 if(!previousTick){previousTick=tick;previousFrame=frame;return;}
 if(uint32_t(tick-previousTick)<5000)return;
 const auto frames=uint32_t(frame-previousFrame),elapsed=uint32_t(tick-previousTick);
 for(unsigned i=0;i<unsigned(shadow_cpu29::Phase::Count);++i){
  const auto& c=shadow_cpu29::counters[i];
  // Read-only aggregate snapshots may straddle a concurrent sample. This is
  // approximate CPU-side attribution, never a GPU execution measurement.
  const auto calls=c.calls.load(std::memory_order_relaxed),samples=c.samples.load(std::memory_order_relaxed);
  const auto ticks=c.ticks.load(std::memory_order_relaxed);
  const auto dc=uint32_t(calls-previousCalls[i]),ds=uint32_t(samples-previousSamples[i]);
  const auto dt=ticks-previousTicks[i];
  previousCalls[i]=calls;previousSamples[i]=samples;previousTicks[i]=ticks;
  if(!ds)continue;
  const double sampleMs=1000.0*double(dt)/double(shadow_cpu29::frequency)/ds;
  const double estimate=sampleMs*dc;
  char text[480];sprintf_s(text,"phase=%s elapsed_ms=%u native_frames=%u calls=%u samples=%u mean_call_ms=%.6f estimated_ms_per_native_frame=%.6f maximum_sample_ms=%.6f sampled_inclusive_cpu_elapsed=1 nested_phases_not_additive=1 gpu_execution_measured=0",
   shadow_cpu29::Names[i],elapsed,frames,dc,ds,sampleMs,frames?estimate/frames:0.0,
   1000.0*double(c.maximum.load(std::memory_order_relaxed))/double(shadow_cpu29::frequency));
  Event(1,"CPU_PROFILE29",text);
 }
 previousTick=tick;previousFrame=frame;
}
void PublishMemoryReads46() noexcept {
 if(!memory_read46::enabled||!log||!log->Enabled())return;
 // Aggregate snapshots have bounded worker-thread lag and can straddle calls.
 // They count actual working-set queries, not GPU cost or complete frame time.
 static std::atomic_flag publishing=ATOMIC_FLAG_INIT;
 if(publishing.test_and_set(std::memory_order_acquire))return;
 struct Release {std::atomic_flag& flag;~Release(){flag.clear(std::memory_order_release);}} release{publishing};
 static uint32_t previousTick{};static memory_read46::Values previous{};
 const auto tick=GetTickCount();
 if(!previousTick){previousTick=tick;previous=memory_read46::Snapshot();return;}
 if(uint32_t(tick-previousTick)<5000)return;
 try {
  const auto values=memory_read46::Snapshot();memory_read46::Values delta{};
  for(unsigned i=0;i<memory_read46::Count;++i)delta[i]=values[i]-previous[i];
  const auto value=[&](memory_read46::Metric metric){return static_cast<unsigned long long>(delta[unsigned(metric)]);};
  char text[1100];
  sprintf_s(text,"elapsed_ms=%u native_frame=%u scalar_helpers=%llu batch_helpers=%llu scalar_os_queries=%llu batch_os_queries=%llu queried_pages=%llu scalar_cache_hits=%llu batch_span_cache_hits=%llu fallback_requests=%llu batch_read_faults=%llu operations=%llu nested_operations=%llu cumulative_os_queries=%llu worker_tail_max_helpers=4095 thread_exit_tail_unreported=1 snapshot_non_atomic=1 fallback_requests_not_os_queries=1 cpu_duration_measured=0 gpu_execution_measured=0",
   uint32_t(tick-previousTick),*CShadows::pFrameCounter,
   value(memory_read46::Metric::ScalarHelpers),value(memory_read46::Metric::BatchHelpers),
   value(memory_read46::Metric::ScalarOSQueries),value(memory_read46::Metric::BatchOSQueries),value(memory_read46::Metric::QueriedPages),
   value(memory_read46::Metric::ScalarCacheHits),value(memory_read46::Metric::BatchSpanCacheHits),
   value(memory_read46::Metric::FallbackRequests),value(memory_read46::Metric::BatchReadFaults),
   value(memory_read46::Metric::Operations),value(memory_read46::Metric::NestedOperations),
   static_cast<unsigned long long>(values[unsigned(memory_read46::Metric::ScalarOSQueries)]+values[unsigned(memory_read46::Metric::BatchOSQueries)]));
  Event(1,"MEMORY_READ_PROFILE46",text);previous=values;previousTick=tick;
 }catch(...){/* Optional logging cannot escape a game callback. */}
}
void PublishRuntimeControls38() noexcept {
 // These are effective inherited menu flags and startup family requests, not
 // proof of native shadow-quality state or completed GPU rendering.
 if(!log||!log->Enabled())return;
 static uint32_t last{};const auto now=GetTickCount();
 if(last&&uint32_t(now-last)<5000)return;last=now;
 try {
  char text[640];
  sprintf_s(text,"frame=%u effective_extra=%u effective_headlight=%u effective_vehicle=%u additional_features=%u allocator_enabled=%u model_requested=%u model_active=%u corona_requested=%u corona_active=%u tail_requested=%u traffic_requested=%u emergency_requested=%u cpu_profiling=%u menu_off_is_not_addon_off=1 gpu_output_proof=0",
   *CShadows::pFrameCounter,unsigned(bool(bExtraNightShadows)),unsigned(bool(bHeadlightShadows)),unsigned(bool(bVehicleNightShadows)),
   unsigned(PlayerShadowAllocation::additionalFeatures),unsigned(PlayerShadowAllocation::Enabled()),
   unsigned(ModelLightShadow20::requested),unsigned(ModelLightShadow20::active.load(std::memory_order_acquire)),
   unsigned(CoronaLight20::requested),unsigned(CoronaLight20::active.load(std::memory_order_acquire)),
   unsigned(TailLight20::requested),unsigned(TrafficSignal::requested),unsigned(nearbyPoliceEnabled),unsigned(shadow_cpu29::enabled));
  Event(1,"RUNTIME_CONTROLS38",text);
 }catch(...){/* Optional logging cannot escape a game callback. */}
}
// Small capture-only instruction snapshots resolve the remaining packed-exe
// helper ambiguity. Never read code or log this data in clean timing runs.
void PublishSelectionHelpers48() noexcept {
 static bool wasCapturing=false;static unsigned samples=0;static uint32_t last=0;
 const bool capturing=CaptureDiagnostic44::sampleSources.load(std::memory_order_relaxed);
 if(!capturing){wasCapturing=false;return;}
 if(!wasCapturing){wasCapturing=true;samples=0;last=0;}
 const auto now=GetTickCount();if(samples>=2||(last&&now-last<5000))return;
 last=now;++samples;
 try {
  constexpr char hex[]="0123456789abcdef";
  for(const uint32_t rva:{0x25480u,0x20850u}){
   const auto bytes=ReadMemory(game+rva,1024);
   for(unsigned offset=0;offset<bytes.size();offset+=128){
    std::string encoded;encoded.reserve(256);
    for(unsigned i=offset;i<offset+128&&i<bytes.size();++i){encoded+=hex[bytes[i]>>4];encoded+=hex[bytes[i]&15];}
    Event(1,"SELECTION_HELPER_BYTES48","sample="+std::to_string(samples)+" base="+std::to_string(game)+" rva="+std::to_string(rva)+
     " offset="+std::to_string(offset)+" bytes="+encoded+" captured_instruction_evidence_not_ownership_proof=1");
   }
  }
 }catch(...){Event(1,"SELECTION_HELPER_UNAVAILABLE48","bounded_optional_read_failed=1");}
}
void PublishShadowTransitions69() {
 static uint32_t last{};
 if(!PlayerShadowAllocation::transitionDetectorEnabled69)return;
 const auto now=GetTickCount();if(uint32_t(now-last)<5000)return;last=now;
 shadow_transition69::Event event{};
 for(unsigned i=0;i<16&&PlayerShadowAllocation::transitionEvents69.Pop(event);++i)
  Event(1,"SHADOW_TRANSITION69","kind="+std::to_string(unsigned(event.kind))+" frame="+std::to_string(event.frame)+
   " source_tick="+std::to_string(event.tick)+" key="+std::to_string(event.source.key)+
   " generation="+std::to_string(event.source.generation)+" replacement="+std::to_string(event.replacement.key)+
   " slot="+std::to_string(event.slot)+" return_ms="+std::to_string(event.elapsed)+
   " dropped_events="+std::to_string(PlayerShadowAllocation::transitionEvents69.dropped.load())+
   " allocator_evidence_only=1 pixel_flicker_confirmed=0");
}
void OnFrame(SafetyHookContext&) noexcept {
 FusionBackend130::Boundary(); // Restore/defer service MUST precede failed return.
 const auto frameStart154=FrameBoundary154::Start();
 if(addonActive.load(std::memory_order_acquire)&&original&&preset117::menuPending117.load(std::memory_order_acquire))preset117::RetryMenu117(game,original->bytes.data(),original->bytes.size());
 const fusionfix::shadows::FloatingPointState fp;
 if(!capture_frame46::ContinueGameplay(failed.load(),addonActive.load(std::memory_order_acquire),pauseUser,pauseScript,[]{
  const auto transition=CaptureDiagnostic44::Poll();
  EffectAttribution199C::CaptureTransition(transition);
  if(transition)Event(1,"CAPTURE_DIAGNOSTIC44","transition="+std::to_string(transition)+" sampled_sources="+std::to_string(CaptureDiagnostic44::sampleSources.load())+" trace="+std::to_string(ShadowTrace34::enabled.load())+" optional_not_clean_timing=1");
 })){FrameBoundary154::Sample(frameStart154,false);return;}
 FrameBoundary154::Sample(frameStart154,true);
 if(addonActive.load(std::memory_order_acquire)){
  if(nativeBackend){const auto* viewport=PlayerShadowAllocation::SceneViewport();
   if(viewport&&Liberty20::Copy(uintptr_t(viewport)+0x2b0,&ownWidth,4)&&Liberty20::Copy(uintptr_t(viewport)+0x2b4,&ownHeight,4)){
    if(ownWidth<16||ownWidth>16384||ownHeight<16||ownHeight>16384){ownWidth=800;ownHeight=600;}
   }
  }
  ModelReferenceQueue189::Publish();PublishSelectionHelpers48();PoleDepthRepair157::Publish();FrameBoundary154::Publish();FrameTime151::Publish();GpuProfiling39::Publish();PublishRuntimeControls38();PublishCpuProfile29();PublishMemoryReads46();Liberty20::Publish();TailLight20::Publish();ModelEffectObserver::Publish();ModelLightShadow20::Publish();WorldGeometry20::Publish();CoronaLight20::Publish();NativeProjection66::Publish();ArcadePlacement197::Publish();PublishVisibleReceiver133();ConsumerShadow75::Publish();NativeProducerGate116::Publish();
  PublishShadowTransitions69();
  NearbyVehicleLighting36::Update(); if(policeTestEnabled){UpdateProof();} TrafficPoll(); if(diagnosticsEnabled){PublishNine(); MeasureProofCadence(); PublishChurn(); PublishPolicePackets(); PublishProofTransition(); PublishLightAudit(); PublishOwnerAudit();PublishCombinedStatus();}
  static uint64_t last{};const auto now=GetTickCount64();if(hotCountersEnabled&&now-last>=5000){last=now;Event(2,"COUNTERS","passes="+std::to_string(PlayerShadowAllocation::appliedPasses.load())+" caster="+std::to_string(OwnHeadlightCaster::casterVisits.load())+" own="+std::to_string(OwnHeadlightCaster::ownPasses.load())+" deferred="+std::to_string(OwnHeadlightCaster::deferredPasses.load())+" proofRequested="+std::to_string(proofRequested.load())+" proofRetired="+std::to_string(proofRetired.load())+" proofPromoted="+std::to_string(proofPromoted.load())+" proofSelected="+std::to_string(proofSelected.load())+" proofRenderCalls="+std::to_string(proofRenderCalls.load())+" policeMode="+std::to_string(policePolicy.load())+" policeOverrides="+std::to_string(policeOverrides.load())+" vacancy="+std::to_string(vacancyRequested.load())+" vacancySuppressed="+std::to_string(vacancySuppressed.load())+" packetDropped="+std::to_string(policeDropped.load())+" graceComparisons="+std::to_string(proofGraceComparisons.load())+" proofFrames="+std::to_string(proofFrames.load())+" maxSlots="+std::to_string(proofMaxSlots.load())+" collected="+std::to_string(PlayerShadowAllocation::labCollected.load())+" observed="+std::to_string(PlayerShadowAllocation::labObserved.load())+" nativeChosen="+std::to_string(PlayerShadowAllocation::labNativeChosen.load())+" finalChosen="+std::to_string(PlayerShadowAllocation::labFinalChosen.load())+" cmp0="+std::to_string(PlayerShadowAllocation::labComparisons[0].load())+" cmp1="+std::to_string(PlayerShadowAllocation::labComparisons[1].load())+" cmp2="+std::to_string(PlayerShadowAllocation::labComparisons[2].load())+" result0="+std::to_string(PlayerShadowAllocation::labResults[0].load())+" result1="+std::to_string(PlayerShadowAllocation::labResults[1].load())+" result2="+std::to_string(PlayerShadowAllocation::labResults[2].load())+" claimLoss="+std::to_string(PlayerShadowAllocation::labClaimLoss.load())+" ownLoss="+std::to_string(PlayerShadowAllocation::labOwnLoss.load())+" warningHidden="+std::to_string(warningHidden.load()));}return;
 }
 if(attempted.exchange(true))return;
 try{Install();}catch(const std::exception& e){DisableAll();failed=true;Event(1,"DISABLED",e.what());StartupFailure129(e.what());}
}
void PrepareNativeBaseModelStore(const std::filesystem::path& ini) {
 if(GetModuleHandleW(L"GTAIV.EFLC.FusionFix.asi")||GetPrivateProfileIntW(L"LIGHTING20",L"NativeBaseModelStore",0,ini.c_str())!=1)return;
 // Two captured native startup failures reached entry 101 in a capacity-100
 // base-model store, before lighting activation. This copy's asset set needs
 // the same 200 base entries it had with official ExtendedLimits. Other
 // pools/stores are deliberately outside this narrowly authenticated repair.
 std::array<uint32_t,6> store{};
 Require(Liberty20::Copy(game+0xc3ad88,store.data(),sizeof(store)),"native_base_store_unreadable");
 Require(store==std::array<uint32_t,6>{100,0,0,205,0,0},"native_model_stores_already_initialized_or_modified");
 MEMORY_BASIC_INFORMATION mbi{};
 Require(VirtualQuery(reinterpret_cast<void*>(game+0xc3ad88),&mbi,sizeof(mbi))==sizeof(mbi)&&
  mbi.State==MEM_COMMIT&&mbi.Type==MEM_IMAGE&&mbi.AllocationBase==reinterpret_cast<void*>(game)&&
  !(mbi.Protect&(PAGE_GUARD|PAGE_NOACCESS)),"native_base_store_not_image_data");
 DWORD oldProtect{},ignored{};
 auto* address=reinterpret_cast<uint32_t*>(game+0xc3ad88);
 Require(VirtualProtect(address,sizeof(store),PAGE_READWRITE,&oldProtect)!=0,"native_base_store_protection_failed");
 const auto previous=InterlockedCompareExchange(reinterpret_cast<volatile LONG*>(address),200,100);
 const auto previousInstance=InterlockedCompareExchange(reinterpret_cast<volatile LONG*>(address+3),410,205);
 const bool restored=VirtualProtect(address,sizeof(store),oldProtect,&ignored)!=0;
 Require(previous==100&&previousInstance==205&&restored,"native_base_store_change_or_protection_restore_failed");
 Event(1,"NATIVE_MODEL_STORES","base=100->200 instance=205->410 allocated=0 storage=0 runtime_only other_stores_unchanged=1");
}
DWORD StartShadow117(const std::filesystem::path& ini) {
 try {
  const auto mode=GetPrivateProfileIntW(L"SUPPORT",L"Logging",0,ini.c_str());
  shadow_cpu29::Configure(mode!=0&&GetPrivateProfileIntW(L"SUPPORT",L"CpuProfiling",1,ini.c_str())==1);
  memory_read46::Configure(mode!=0&&GetPrivateProfileIntW(L"SUPPORT",L"MemoryProfiling",0,ini.c_str())==1);
  traffic_signal::fresh_page::Configure(GetPrivateProfileIntW(L"SUPPORT",L"WorkingSetValidation",1,ini.c_str())==0);
  ModelLightShadow20::ConfigureQueuedScratch59(ini.c_str());
  publicSettings93=liberty_settings93::ReadPublic(ini.c_str());
  log=std::make_unique<SupportLog>(pe::ModulePath(self).replace_extension(L".log"),mode,GetPrivateProfileIntW(L"SUPPORT",L"MaxLogKiB",256,ini.c_str()));
  FrameTime151::requested=mode!=0&&GetPrivateProfileIntW(L"SUPPORT",L"FrameTimeProfiling151",0,ini.c_str())==1;
  FrameBoundary154::requested=GetPrivateProfileIntW(L"SUPPORT",L"FrameBoundaryProfiling154",0,ini.c_str())==1;
  GpuProfiling39::requested=mode!=0&&GetPrivateProfileIntW(L"SUPPORT",L"GpuProfiling",0,ini.c_str())==1;
  GpuProfiling39::shadowStages=GpuProfiling39::requested&&GetPrivateProfileIntW(L"SUPPORT",L"GpuShadowStages",0,ini.c_str())==1;
  Event(1,"START",std::string("build=")+CandidateBuild32);
  Event(1,"GPU_REQUESTED48",GpuProfiling39::requested?(GpuProfiling39::shadowStages?"requested=1 span=shadow_command_span":"requested=1 span=BeginScene_EndScene"):"requested=0 default_off=1");
  if(memory_read46::enabled)Event(1,"MEMORY_READ_PROFILE46_CONFIG","enabled=1 startup_opt_in=1 helper_tls_flush=4096 worker_tail_max_helpers=4095 thread_exit_tail_unreported=1 publish_interval_ms=5000 actual_working_set_queries=1 fallback_requests_not_os_queries=1");
  Event(1,"VALIDATION31",traffic_signal::fresh_page::preferRegion?"fresh_region=1 current_permissions owner_checks_unchanged=1":"fresh_region=0 working_set_override=1");
  Liberty20::dualRequested=GetPrivateProfileIntW(L"LIGHTING20",L"DualHeadlights",0,ini.c_str())==1;
  Liberty20::worldRequested=GetPrivateProfileIntW(L"LIGHTING20",L"NativeKeyedWorldShadows",0,ini.c_str())==1;
  TailLight20::requested=GetPrivateProfileIntW(L"LIGHTING20",L"TailLightShadows",0,ini.c_str())==1;
  WorldGeometry20::requested=GetPrivateProfileIntW(L"LIGHTING20",L"WorldGeometryCasters",0,ini.c_str())==1;
  ArcadePlacement197::requested=GetPrivateProfileIntW(L"SUPPORT",L"ArcadePlacement197",0,ini.c_str())==1;
  NativeProjection66::requested=GetPrivateProfileIntW(L"LAB",L"NativeProjection66",0,ini.c_str())==1;
  NativeProjection66::replacementRequested=GetPrivateProfileIntW(L"LAB",L"ReplaceModelProjection66",0,ini.c_str())==1;
  PlayerShadowAllocation::transitionDetectorEnabled69=preset117::ReadTransitionDiagnostics124(ini.c_str());
  MapContribution198C::requested=GetPrivateProfileIntW(L"SUPPORT",L"MapContribution198C",0,ini.c_str())==1;
  ModelLightShadow20::requested=GetPrivateProfileIntW(L"LIGHTING20",L"ModelLightShadows",0,ini.c_str())==1;
  CoronaLight20::requested=GetPrivateProfileIntW(L"LIGHTING20",L"CoronaLightShadows",0,ini.c_str())==1;
  const auto configuredFloat=[&](const wchar_t* key,float fallback,float minimum,float maximum){
   wchar_t buffer[64]{};if(!GetPrivateProfileStringW(L"LIGHTING20",key,L"",buffer,64,ini.c_str()))return fallback;
   wchar_t* end{};const auto value=std::wcstof(buffer,&end);
   if(end==buffer||!std::isfinite(value))return fallback;
   while(*end&&std::iswspace(*end))++end;if(*end)return fallback;
   return std::clamp(value,minimum,maximum);
  };
  auto& glow=CoronaLight20::settings;
  PlayerShadowAllocation::parallelRequested61=GetPrivateProfileIntW(L"SUPPORT",L"ParallelSelectionMath",0,ini.c_str())==1;
  PlayerShadowAllocation::signalShadowPriority=configuredFloat(L"SignalShadowPriority",1.5f,1.f,4.f);
  PlayerShadowAllocation::scenePriorityStrength=configuredFloat(L"ScenePriorityStrength",1.5f,0.f,4.f);
  PlayerShadowAllocation::scenePriorityReleaseMs=configuredFloat(L"ScenePriorityReleaseMs",450.f,50.f,1500.f);
  PlayerShadowAllocation::visibleReceiverExperiment133=GetPrivateProfileIntW(L"LAB",L"VisibleReceiverRanking133",0,ini.c_str())==1;
  PlayerShadowAllocation::visibleReceiverSource133=VisibleReceiverSource133;
  glow.allowApproximation=GetPrivateProfileIntW(L"LIGHTING20",L"CoronaApproximation",1,ini.c_str())==1;
  glow.radiusPerAuthoredSize=configuredFloat(L"CoronaRadiusPerAuthoredSize",12.f,.001f,1024.f);
  glow.fallbackIntensity=configuredFloat(L"CoronaFallbackIntensity",1.f,.001f,1024.f);
  glow.intensityScale=configuredFloat(L"CoronaIntensityScale",1.f,.001f,128.f);
  glow.maxRadius=configuredFloat(L"CoronaMaxRadius",128.f,.01f,4096.f);
  glow.minRadius=std::min(configuredFloat(L"CoronaMinRadius",.1f,.001f,4096.f),glow.maxRadius);
  glow.maxIntensity=configuredFloat(L"CoronaMaxIntensity",64.f,.001f,1024.f);
  ModelEffectObserver::requested=ModelLightShadow20::requested||CoronaLight20::requested||(mode!=0&&GetPrivateProfileIntW(L"LIGHTING20",L"ModelEffectCatalog",0,ini.c_str())==1);
  ownExtra=GetPrivateProfileIntW(L"SHADOWS",L"ExtraNightShadows",1,ini.c_str())==1;
  ownHeadlight=GetPrivateProfileIntW(L"SHADOWS",L"HeadlightShadows",1,ini.c_str())==1;
  ownVehicle=GetPrivateProfileIntW(L"SHADOWS",L"VehicleNightShadows",1,ini.c_str())==1;
  Liberty20::dualRequested=Liberty20::dualRequested&&ownHeadlight&&ownVehicle;
  Liberty20::worldRequested=Liberty20::worldRequested&&ownExtra;
  PlayerShadowAllocation::additionalFeatures=TailLight20::requested||ModelLightShadow20::requested||CoronaLight20::requested;
  const auto options=experiment_perf::Configure(mode,GetPrivateProfileIntW(L"LAB",L"TrafficObserver",0,ini.c_str())==1,GetPrivateProfileIntW(L"SUPPORT",L"DiagnosticCounters",0,ini.c_str())==1,GetPrivateProfileIntW(L"LAB",L"ObserveOwners",0,ini.c_str())==1);
  TrafficSignal::requested=GetPrivateProfileIntW(L"LIGHTING20",L"TrafficSignalShadows",options.traffic?1:0,ini.c_str())==1;
  TrafficSignal::promotionRequested=TrafficSignal::requested&&GetPrivateProfileIntW(L"LIGHTING20",L"TrafficSignalShadows",GetPrivateProfileIntW(L"LAB",L"TrafficPromote",0,ini.c_str()),ini.c_str())==1;
  Require(TrafficSignal::registry.Configure(TrafficSignal::PrivateKey(),ProofKey()),"traffic_reserved_key_collision");
  for(unsigned i=0;i<nearbyKeys.size();++i)Require(TrafficSignal::registry.IndexOfKey(NearbyKey(i))==UINT32_MAX,"traffic_emergency_reserved_key_collision");
  PoleCasterDiagnostic134::requested=mode!=0&&GetPrivateProfileIntW(L"SUPPORT",L"PoleCasterDiagnostic134",0,ini.c_str())==1;
  PoleDepthRepair157::requested=mode!=0&&GetPrivateProfileIntW(L"SUPPORT",L"PoleDepthRepair197C",0,ini.c_str())==1;
  PoleStaticPartition141::requested=mode!=0&&GetPrivateProfileIntW(L"SUPPORT",L"PoleStaticAdmission197C",0,ini.c_str())==1;
  CaptureDiagnostic44::directory=ini.parent_path();
  NativeProducerGate116::requested=GetPrivateProfileIntW(L"SUPPORT",L"NativeProducerGateDiagnostics116",0,ini.c_str())==1;
  EffectAttribution199C::requested=GetPrivateProfileIntW(L"SUPPORT",L"EffectProducerDiagnostics199C",0,ini.c_str())==1;
  ConsumerReach136::requested=GetPrivateProfileIntW(L"LAB",L"StaticConsumerReach136",0,ini.c_str())==1;
  ConsumerReach136::retentionRequested198=GetPrivateProfileIntW(L"LAB",L"SceneryShadowRetention198A",0,ini.c_str())==1;
  SceneryFade198B::requested=GetPrivateProfileIntW(L"LAB",L"SceneryShadowFade198B",0,ini.c_str())==1;
  ConsumerShadow75::requested=ConsumerReach136::requested||GetPrivateProfileIntW(L"SUPPORT",L"ConsumerShadowDiagnostics",0,ini.c_str())==1;
  diagnosticsEnabled=options.detail;hotCountersEnabled=options.counters;ownerTelemetryEnabled=options.ownerTelemetry;OwnHeadlightCaster::diagnosticCounters=options.counters;
  policeTestEnabled=GetPrivateProfileIntW(L"LIGHTING20",L"EmergencyLightShadows",GetPrivateProfileIntW(L"LAB",L"PoliceTest",0,ini.c_str()),ini.c_str())==1;
  nearbyPoliceEnabled=policeTestEnabled&&GetPrivateProfileIntW(L"LIGHTING20",L"EmergencyLightShadows",GetPrivateProfileIntW(L"LAB",L"PoliceNearby",0,ini.c_str()),ini.c_str())==1;
  if(publicSettings93.present&&!preset117::startup.present){
   const auto p=liberty_settings93::Requests(publicSettings93.state);
   Liberty20::dualRequested=p.dual&&ownHeadlight&&ownVehicle;
   Liberty20::worldRequested=p.keyedWorld&&ownExtra;
   TailLight20::requested=p.tail;WorldGeometry20::requested=p.worldGeometry;
   ModelLightShadow20::requested=p.model;CoronaLight20::requested=p.corona;
   NativeProjection66::requested=p.projection;NativeProjection66::replacementRequested=p.projection;
   ModelEffectObserver::requested=p.model||p.corona||(mode!=0&&GetPrivateProfileIntW(L"LIGHTING20",L"ModelEffectCatalog",0,ini.c_str())==1);
   TrafficSignal::requested=p.traffic;TrafficSignal::promotionRequested=p.traffic;
   policeTestEnabled=nearbyPoliceEnabled=p.emergency;
   PlayerShadowAllocation::additionalFeatures=p.tail||p.model||p.corona||p.traffic||p.emergency;
   Event(1,"PUBLIC_SETTINGS93","preset="+std::to_string(unsigned(publicSettings93.state.preset))+
    " invalid_values="+std::to_string(publicSettings93.invalid)+" startup_applied=1 menu_verified=0");
  }
  if(preset117::startup.present){
   const auto p=preset117::Requests(preset117::startup.state);
   if(preset117::startup.ownBackend)ownExtra=ownHeadlight=ownVehicle=p.base;
   Liberty20::dualRequested=p.dual&&ownHeadlight&&ownVehicle;Liberty20::worldRequested=p.keyedWorld&&ownExtra;
   TailLight20::requested=p.tail;WorldGeometry20::requested=p.worldGeometry;
   ModelLightShadow20::requested=p.model;CoronaLight20::requested=p.corona;
   NativeProjection66::requested=p.projection;NativeProjection66::replacementRequested=p.projection;
   ModelEffectObserver::requested=p.model||p.corona;
   TrafficSignal::requested=TrafficSignal::promotionRequested=p.traffic;
   policeTestEnabled=nearbyPoliceEnabled=p.emergency;
   PlayerShadowAllocation::additionalFeatures=p.tail||p.model||p.corona||p.traffic||p.emergency;
   // Named lower tiers bypass enhanced catalog/owner/profiling branches even
   // when retained SUPPORT/LAB keys were written by an older full profile.
   if(!p.model&&!p.corona){ConsumerReach136::requested=false;ConsumerShadow75::requested=false;NativeProducerGate116::requested=false;}
   Event(1,"SETTINGS117","schema=3 single_shared_manager=1 restart_required=1 migrated="+std::to_string(preset117::startup.migrated)+" invalid_values="+std::to_string(preset117::startup.invalid));
  }
  PlayerShadowAllocation::additionalFeatures=PlayerShadowAllocation::additionalFeatures||TrafficSignal::requested||nearbyPoliceEnabled;
  Event(1,"FEATURES",std::string("traffic=")+std::to_string(TrafficSignal::requested)+" nearby_emergency="+std::to_string(nearbyPoliceEnabled)+" diagnostic_counters="+std::to_string(hotCountersEnabled)+" owner_telemetry="+std::to_string(ownerTelemetryEnabled)+" quality_unchanged=1 native_slots=7 no_time_expiry=1");
  auditEnabled=!preset117::startup.present && mode==2 && GetPrivateProfileIntW(L"LAB",L"ObserveLights",0,ini.c_str())==1;
  ownerAuditEnabled=TrafficSignal::requested||policeTestEnabled || (!preset117::startup.present && mode==2 && GetPrivateProfileIntW(L"LAB",L"ObserveOwners",0,ini.c_str())==1);
  if(ownerAuditEnabled){PlayerShadowAllocation::observeShadowResult=PrivateResults;OwnHeadlightCaster::labRenderObserver=PrivateRender;}
  else if(Liberty20::dualRequested){PlayerShadowAllocation::observeShadowResult=Liberty20::Result;OwnHeadlightCaster::labRenderObserver=Liberty20::Render;}
  MapContribution198C::Wire();
  if(policeTestEnabled){PlayerShadowAllocation::labPointSource=IsProofPoint;PlayerShadowAllocation::labCompareGrace=ComparePoliceClaim;PlayerShadowAllocation::labGracePairNeutral108=NearbyGracePairNeutral108;}
  if(TrafficSignal::requested)PlayerShadowAllocation::labLampGeneration=TrafficSignal::LampGeneration;
  if(auditEnabled || policeTestEnabled) PlayerShadowAllocation::observeSubmittedLights=ObserveLabLights;
  FusionFixSettings.lampReach=std::clamp<int>(GetPrivateProfileIntW(L"SHADOWS",L"LamppostShadowReach",40,ini.c_str()),0,100);
  FusionFixSettings.headlightReach=std::clamp<int>(GetPrivateProfileIntW(L"SHADOWS",L"HeadlightShadowReach",10,ini.c_str()),0,100);
  if(publicSettings93.present&&!preset117::startup.present){
   FusionFixSettings.lampReach=int(publicSettings93.state.lampReachSteps);
   FusionFixSettings.headlightReach=int(publicSettings93.state.headlightReachSteps);
  }
  if(preset117::startup.present){FusionFixSettings.lampReach=int(preset117::startup.state.lampReachSteps);FusionFixSettings.headlightReach=int(preset117::startup.state.headlightReachSteps);}
  game=reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));const auto raw=pe::Read(pe::ModulePath(nullptr));original=std::make_unique<pe::Image>(raw,game);
  PlayerShadowAllocation::parallelMasterEnabled61=true;
  PlayerShadowAllocation::StartParallel61(); // initialization worker after master gate; no startup/join inside render hooks
  if(!preset117::startup.present||preset117::Enabled(preset117::startup.state,preset117::Feature::WorldLighting))PrepareNativeBaseModelStore(ini);
  const auto call=original->Find("E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? B9");loop=game+call;
  const auto pause=original->Pause();pauseUser=reinterpret_cast<uint8_t*>(U32(original->bytes.data()+pause+2));pauseScript=reinterpret_cast<uint8_t*>(U32(original->bytes.data()+pause+8));
  for(unsigned attempt=0;attempt<2400;++attempt){const auto bytes=ReadMemory(loop+5,16);if(!memcmp(bytes.data(),original->bytes.data()+call+5,16)){boundary=safetyhook::create_mid(loop+5,OnFrame);Require(bool(boundary),"boundary_hook_failed");Event(1,"WAITING","post_FusionFix_unpaused_frame");return 0;}Sleep(50);}
  throw std::runtime_error("unpacked_boundary_timeout");
 }catch(const std::exception& e){failed=true;Event(1,"DISABLED",e.what());StartupFailure129(e.what());}return 0;
}
DWORD WINAPI Start(void*) {
 try {
  const auto ini=pe::ModulePath(self).replace_extension(L".ini");
  preset117::module=self;preset117::startup=preset117::ReadStartup124(ini.c_str());
  auto effective=preset117::startup.state;
  // ExplicitSchema3 owns its master. Absent/legacy settings honor the existing
  // master BEFORE profiling, enhanced config, preparation or worker startup.
  if(!preset117::startup.ownBackend&&!GetPrivateProfileIntW(L"SHADOWS",L"Enabled",1,ini.c_str()))effective.enabled=false;
  if(preset117::startup.unavailable){effective.enabled=false;OutputDebugStringW(L"Liberty's Shadows: unsupported settings schema/preset; lighting remains original. Correct LIBERTY_SHADOWS in the INI.\n");}
  preset117::applied=effective;
  return preset117::RouteStartup117(effective,preset117::runtimeLease,
   [&]{OutputDebugStringW(L"Liberty's Shadows: Off; frontend only, no lighting services.\n");return preset117::FrontendOnly117(self);},
   [&]{return StartShadow117(ini);});
 }catch(const std::exception& e){failed=true;OutputDebugStringA(e.what());StartupFailure129(e.what());return 1;}
}

}
extern "C" __declspec(dllexport) void InitializeASI(){static std::once_flag once;std::call_once(once,[]{if(auto h=CreateThread(nullptr,0,standalone::Start,nullptr,0,nullptr))CloseHandle(h);});}
BOOL WINAPI DllMain(HMODULE h,DWORD reason,void*){if(reason==DLL_PROCESS_ATTACH){standalone::self=h;DisableThreadLibraryCalls(h);}return TRUE;}



























