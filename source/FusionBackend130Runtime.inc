// Exact officialFF511 pure-setting callback lease. No raw preference/CFG write.
#include "FusionBackendLease130.hpp"
#include "FusionBackendGuard130.hpp"
#include "BackendVersions213.hpp"
namespace FusionBackend130 {
using namespace fusion_backend130;
uint32_t backendExtraCallbackRva=fusion_backend130::ExtraCallbackRva;
uint32_t backendAdmissionCallbackRva=fusion_backend130::AdmissionCallbackRva;
uint32_t backendExtraRva=fusion_backend130::ExtraRva;
uint32_t backendHeadRva=fusion_backend130::HeadRva;
uint32_t backendVehicleRva=fusion_backend130::VehicleRva;
uint32_t backendListCallbackRva=fusion_backend130::ListCallbackRva;
uint32_t backendVehicleCallbackRva=fusion_backend130::VehicleCallbackRva;
uint32_t backendFlagsCallbackRva=fusion_backend130::FlagsCallbackRva;
uint32_t backendListIndexRva=fusion_backend130::ListIndexRva;
uint32_t backendListFunctionRva=fusion_backend130::ListFunctionRva;

Lease lease;
SafetyHookInline settingHook,admissionHook;
HMODULE pinnedModule{};
bool prepared=false;
bool Limited() noexcept {return prepared&&!lease.Owns();}
thread_local unsigned settingDepth{};
thread_local bool applying=false;
std::atomic<uint32_t> genuineRequests{0},deferredRequests{0},originalCalls{0},refused{0};
bool FieldsWritable() noexcept {
 if(!prepared||!pinnedModule||reinterpret_cast<uintptr_t>(pinnedModule)!=ff)return false;
 for(const auto [rva,n]:{std::pair<uint32_t,size_t>{backendExtraRva,2},{backendVehicleRva,1}}){
  const auto at=ff+rva;MEMORY_BASIC_INFORMATION m{};
  if(VirtualQuery(reinterpret_cast<void*>(at),&m,sizeof(m))!=sizeof(m)||m.State!=MEM_COMMIT||m.Type!=MEM_IMAGE||
    reinterpret_cast<uintptr_t>(m.AllocationBase)!=ff||(m.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;
  const auto p=m.Protect&0xff;
  if(p!=PAGE_READWRITE&&p!=PAGE_WRITECOPY&&p!=PAGE_EXECUTE_READWRITE&&p!=PAGE_EXECUTE_WRITECOPY)return false;
  const auto begin=reinterpret_cast<uintptr_t>(m.BaseAddress);
  if(at<begin||at-begin>m.RegionSize||n>m.RegionSize-(at-begin))return false;
 }return true;
}
// Separate SEH leaf: no C++ objects/destructors across the contained native call.
bool Original(void* self,uint8_t mode) noexcept {
 if(!settingHook)return false;
 const int32_t value=mode;
 ++originalCalls;
 __try {settingHook.unsafe_thiscall<void>(self,&value);return true;}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool Apply(bool force,void* self=nullptr) noexcept {
 const ModelEffectObserver::LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 if(applying||GetCurrentThreadId()!=lease.owner)return false;
 applying=true;
 const bool good=lease.Apply(GetCurrentThreadId(),force,FieldsWritable,[self](uint8_t mode)noexcept{return Original(self,mode);});
 applying=false;
 if(!good)++refused;
 return good;
}
void __fastcall Setting(void* self,void*,const int32_t* pointer) noexcept {
 const ModelEffectObserver::LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 ++settingDepth;
 int32_t mode{};
 const bool copied=pointer&&Liberty20::Copy(reinterpret_cast<uintptr_t>(pointer),&mode,sizeof(mode));
 if(!copied||mode<0||mode>3||!lease.Request(uint8_t(mode))){lease.Close();++refused;}
 else {
  ++genuineRequests;
  if(GetCurrentThreadId()==lease.owner&&settingDepth==1&&!applying){(void)Apply(true,self);}
  else ++deferredRequests; // Numeric latest-mode only; no synchronous FF writes.
 }
 --settingDepth;
}
void __cdecl Admission(SafetyHookContext& regs) noexcept {
 const ModelEffectObserver::LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 if(lease.Owns()&&addonActive.load(std::memory_order_acquire)&&(regs.eax&6u))return;
 admissionHook.unsafe_ccall<void>(&regs);
}
// Service even after failed/!addonActive: a closed partial lease must restore.
void Boundary() noexcept {
 const ModelEffectObserver::LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 if(!prepared||applying||GetCurrentThreadId()!=lease.owner)return;
 if(failed.load(std::memory_order_acquire))lease.Close();
 if(!lease.Pending(GetCurrentThreadId()))return;
 (void)Apply(false);
}
void Close() noexcept {
 lease.Close(); // Admission closes BEFORE restoration or exceptional handling.
 if(prepared&&GetCurrentThreadId()==lease.owner&&!applying)(void)Apply(false);
 // Keep hook/trampoline allocations and dispatch resident. This prevents a
 // disable-before-restore window reopening foreign original writers.
}
bool Commit() noexcept {
 if(!prepared)return true;
 lease.wanted.store(true,std::memory_order_release);
 if(!Apply(true)){Close();return false;}return lease.Owns();
}
// PREPARE_BOUNDARY130: exact guards are startup-only; no per-light hashing.
void PrepareHooks() {
 if(nativeBackend||!preset117::startup.ownBackend||!preset117::startup.state.enabled)return;
 const auto* profile213=BackendProfile218();
 Require(profile213!=nullptr,"fusion_backend130_layout_refused");
 Require(fusionModule213Authenticated,"fusion_backend130_integration_unavailable");
 backendExtraCallbackRva=profile213->extraCallback;
 backendAdmissionCallbackRva=profile213->admission;
 backendExtraRva=profile213->extra;
 backendHeadRva=profile213->head;
 backendVehicleRva=profile213->vehicle;
 backendListCallbackRva=profile213->listCallback;
 backendVehicleCallbackRva=profile213->vehicleCallback;
 backendFlagsCallbackRva=profile213->flagsCallback;
 backendListIndexRva=profile213->listIndex;
 backendListFunctionRva=profile213->listFunction;
 Require(original&&backend_versions213::CallbackBodies(*profile213,uint32_t(ff),Liberty20::Copy),"fusion_backend130_callback_guard");
 const auto native=[&](uint32_t rva,unsigned n){Require(uint64_t(rva)+n<=original->bytes.size(),"fusion_backend130_native_range");return std::span<const uint8_t>(original->bytes.data()+rva,n);};
 const auto read=Liberty20::Copy;
 // FF captures the TWO-argument driver at6E3DE0. Its separately resolved
 // FIVE-argument draw-list pointer still targets the pristine6E3310 routine.
 Require(MidOwner(uint32_t(game),uint32_t(ff),0x6e3de0,backendListCallbackRva,6,native(0x6e3de0,6),MidStubTemplate,false,read),"fusion_backend130_list_owner");
 std::array<uint8_t,16> drawEntry{};
 Require(Liberty20::Copy(game+0x6e3310,drawEntry.data(),drawEntry.size())&&
   !std::memcmp(drawEntry.data(),native(0x6e3310,16).data(),drawEntry.size()),"fusion_backend130_draw_entry");
 Require(MidOwner(uint32_t(game),uint32_t(ff),0x6e4236,backendVehicleCallbackRva,5,native(0x6e4236,5),MidStubTemplate,true,read),"fusion_backend130_vehicle_owner");
 VerifyBranch(0x81e615,backendFlagsCallbackRva,true,6); // All tiers, including Low/Medium.
 uint32_t listFunction{};
 Require(Liberty20::Copy(ff+backendListFunctionRva,&listFunction,4)&&listFunction==game+0x6e3310,"fusion_backend130_list_binding");
 Require(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN|GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
   reinterpret_cast<LPCWSTR>(ff+backendExtraCallbackRva),&pinnedModule)&&reinterpret_cast<uintptr_t>(pinnedModule)==ff,"fusion_backend130_module_pin");
 // Existing post-FF unpaused boundary follows FF's wait of ALL init futures.
 // Repeat the small tuple; never call GetRef or inspect a private CSettings.
 std::array<uint8_t,2> pair{},againPair{};uint8_t vehicle{},againVehicle{};
 Require(Liberty20::Copy(ff+backendExtraRva,pair.data(),2)&&Liberty20::Copy(ff+backendVehicleRva,&vehicle,1)&&
   Liberty20::Copy(ff+backendExtraRva,againPair.data(),2)&&Liberty20::Copy(ff+backendVehicleRva,&againVehicle,1)&&
   pair==againPair&&vehicle==againVehicle,"fusion_backend130_tuple_read");
 const auto mode=Decode(pair[0],pair[1],vehicle);
 Require(mode&&lease.Arm(GetCurrentThreadId(),*mode),"fusion_backend130_tuple_invalid");
 prepared=true;
 Require(FieldsWritable(),"fusion_backend130_fields_unwritable");
 Prepare(settingHook,ff+backendExtraCallbackRva,Setting);
 Prepare(admissionHook,ff+backendAdmissionCallbackRva,Admission);
}
}
