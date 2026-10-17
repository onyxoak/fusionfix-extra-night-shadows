// Private main-view prototype using native model/physical bounds. Native cache
// construction, map selection/resolution/refresh and illumination stay.
#include "core/ReceiverCasterCull226.hpp"
#include "core/NativeBounds229.hpp"
namespace ReceiverCull226 {
bool requested242=false;
std::atomic<bool> tightHull241{false};std::atomic<bool> mode{false};bool prepared=false,experimental=false;
bool nativeBounds229=false;
struct Binding {bool valid{};uint32_t frame{},thread{};receiver_cull226::Volume volume{};uint32_t seen{},culled{},unknown{};std::array<uint32_t,4> types{},typeCulled{};};
thread_local Binding binding;
std::atomic<uint32_t> scopes{},validScopes{},seen{},culled{},unknown{},scopeThread{},allocationThread{};
std::array<std::atomic<uint32_t>,8> reasons{};
std::array<std::atomic<uint32_t>,4> types229{},culledTypes229{};
struct Preserve {DWORD error=GetLastError();fusionfix::shadows::FloatingPointState fp;~Preserve()noexcept{SetLastError(error);}};
bool Visit(uintptr_t entity,uint32_t type) noexcept {
 if(type!=1||!binding.valid||binding.thread!=GetCurrentThreadId()||!CShadows::pFrameCounter||*CShadows::pFrameCounter!=binding.frame)return false;
 const Preserve preserve;++binding.seen;
 // The pointer is the currently visited native entity, not an asynchronously
 // retained owner. SEH-contained copies retain the native draw on a read fault.
 std::array<uint8_t,0x34> body{};std::array<float,16> matrix{};
 auto read=[](uintptr_t at,void* out,size_t n)noexcept{return at>=0x10000&&at<=UINT32_MAX-n&&Liberty20::CopyLocal(at,out,n);};
 if(!read(entity,body.data(),body.size())){++binding.unknown;return false;}
 uint32_t vt{},pose{},nativeType{};int16_t model{};
 memcpy(&vt,body.data(),4);memcpy(&pose,body.data()+0x20,4);memcpy(&nativeType,body.data()+0x28,4);memcpy(&model,body.data()+0x2e,2);
 const auto entityType=(nativeType>>6)&15;
 if(entityType<1||entityType>4||model<0||model>=31000){++binding.unknown;return false;}
 ++binding.types[entityType-1];
 uint32_t sphere{},info{};std::array<float,8> bounds{};
 if(!read(uintptr_t(vt)+0x5c,&sphere,4)){++binding.unknown;return false;}
 if(nativeBounds229&&sphere==game+0x711140){
  receiver_bounds229::Snapshot value{},again{};
  if(!receiver_bounds229::Capture(read,game,entity,value)){++binding.unknown;return false;}
  if(!receiver_bounds229::Outside(binding.volume,receiver_bounds229::Enclose(value)))return false;
  if(!receiver_bounds229::Capture(read,game,entity,again)||!(value==again)||*CShadows::pFrameCounter!=binding.frame){++binding.unknown;return false;}
  ++binding.culled;++binding.typeCulled[entityType-1];return true;
 }
 if(entityType!=1||sphere!=game+0x6314e0||
    !read(game+0xe95cd8+uint32_t(model)*4,&info,4)||!read(uintptr_t(info)+0x20,bounds.data(),sizeof(bounds))){++binding.unknown;return false;}
 const receiver_cull226::V lo{bounds[0],bounds[1],bounds[2]},hi{bounds[4],bounds[5],bounds[6]};bool reject=false;
 if(pose){
  if(!read(pose,matrix.data(),sizeof(matrix))){++binding.unknown;return false;}
  // Native CEntity transforms use four XYZ vectors at16-byte strides; their
  // padding words are not projective coefficients.6314E0 and the native model
  // transform use precisely these twelve XYZ values. Keep raw padding for the
  // fresh-state comparison, and canonicalize only this numeric local copy.
  auto affine=matrix;affine[3]=affine[7]=affine[11]=0.f;affine[15]=1.f;
  reject=receiver_cull226::OutsideBox(binding.volume,lo,hi,affine);
 }else{
  float p[3]{};memcpy(p,body.data()+0x10,sizeof(p));reject=receiver_cull226::OutsideRotatedOrigin(binding.volume,lo,hi,{p[0],p[1],p[2]});
 }
 if(!reject)return false;
 // Re-read state before omitting a native draw. No persistent entity or model
 // pointer cache; streaming/replacements/poses cannot authorize later frames.
 std::array<uint8_t,0x34> again{};std::array<float,8> againBounds{};uint32_t againInfo{};
 if(!read(entity,again.data(),again.size())||again!=body||!read(game+0xe95cd8+uint32_t(model)*4,&againInfo,4)||againInfo!=info||
    !read(uintptr_t(info)+0x20,againBounds.data(),sizeof(againBounds))||againBounds!=bounds){++binding.unknown;return false;}
 if(pose){std::array<float,16> againMatrix{};if(!read(pose,againMatrix.data(),sizeof(againMatrix))||againMatrix!=matrix){++binding.unknown;return false;}}
 if(*CShadows::pFrameCounter!=binding.frame){++binding.unknown;return false;}
 ++binding.culled;++binding.typeCulled[entityType-1];return true;
}
void EnableConfigured242() noexcept {
 if(!prepared||!requested242)return;
 tightHull241.store(true,std::memory_order_release);
 mode.store(true,std::memory_order_release);
}
void Prepare() noexcept {
 try {
  wchar_t path[32768]{};GetModuleFileNameW(nullptr,path,32768);
  experimental=requested242||_wcsicmp(path,L"D:\\Codex-Work\\Libertys-Shadows-2.0\\game\\GTAIV\\GTAIV.exe")==0;
  if(!experimental)return; // Outside the lab, startup requires the explicit INI opt-in.
  const auto code=ReadMemory(game+0x6314e0,0x11f);
  Require(original&&0x6315ff<original->bytes.size()&&!memcmp(code.data(),original->bytes.data()+0x6314e0,code.size()),"receiver226_building_bound_changed");
  const auto physical=ReadMemory(game+0x711140,0x110);
  Require(original&&0x711250<original->bytes.size()&&!memcmp(physical.data(),original->bytes.data()+0x711140,physical.size()),"receiver229_physical_bound_changed");
  nativeBounds229=true;
  receiverCull226=Visit;prepared=true;EnableConfigured242();
  Event(1,"RECEIVER_CULL226_PREPARED","native_model_and_physical_bounds229=1 main_view_hull=1 receiver_reflection_validation_pending=1");
 if(requested242)Event(1,"CASTER_CULL242_CONFIG","enabled=1 tight_hull=1 restart_required=1");
 }catch(const std::exception& e){prepared=false;mode.store(false);tightHull241.store(false);Event(1,"RECEIVER_CULL226_REFUSED",e.what());}
}
class Scope {
 Binding previous;
public:
 explicit Scope(void* object)noexcept:previous(binding){
  binding={};if(!mode.load(std::memory_order_acquire)||!prepared||!WorldGeometry20::active.load(std::memory_order_acquire)||!addonActive.load(std::memory_order_acquire)||!object)return;
  const Preserve preserve;++scopes;
  uint32_t slot{},key{},kind{};uint8_t active{},deferred{};float source[3]{};
  const auto thread=GetCurrentThreadId();
  scopeThread.store(thread,std::memory_order_relaxed);allocationThread.store(PlayerShadowAllocation::ownerThread.load(std::memory_order_relaxed),std::memory_order_relaxed);
  // Native rendering can execute on a different thread from allocation. The
  // TLS scope belongs to this synchronous render call, not to the allocator.
  if(!CShadows::pFrameCounter){++reasons[0];return;}
  if(!Liberty20::CopyLocal(game+0x1193312,&deferred,1)||deferred){++reasons[1];return;}
  if(!Liberty20::CopyLocal(uintptr_t(object)+0x940,&slot,4)||slot<1||slot>7){++reasons[2];return;}
  const auto map=slot*0x110u;
  if(!Liberty20::CopyLocal(game+0xd9f1ed+map,&active,1)||active!=1){++reasons[3];return;}
  if(!Liberty20::CopyLocal(game+0xd9f1fc+map,&kind,4)||(kind!=4&&kind!=5)){++reasons[4];return;}
  if(!Liberty20::CopyLocal(game+0xd9f1f8+map,&key,4)||!key||key==UINT32_MAX||
     !Liberty20::CopyLocal(game+0xd9f1c0+map,source,sizeof(source))){++reasons[5];return;}
  //978410 copies this pass's scene viewport to+4A0 before constructing its
  //light viewport at+B0. Use that queued scene, never a later main-thread view.
  const auto frame=*CShadows::pFrameCounter;
  const auto view=PlayerShadowAllocation::ReadView(reinterpret_cast<const rage::grcViewport*>(uintptr_t(object)+0x4a0));
  if(!view.valid){++reasons[6];return;}
  auto volume=receiver_cull226::Prepare(view,{source[0],source[1],source[2]});
  if(!volume.count||*CShadows::pFrameCounter!=frame){++reasons[7];return;}
  binding.valid=true;binding.frame=frame;binding.thread=thread;binding.volume=volume;++validScopes;
 }
 ~Scope()noexcept{seen.fetch_add(binding.seen,std::memory_order_relaxed);culled.fetch_add(binding.culled,std::memory_order_relaxed);unknown.fetch_add(binding.unknown,std::memory_order_relaxed);for(unsigned i=0;i<4;++i){types229[i].fetch_add(binding.types[i],std::memory_order_relaxed);culledTypes229[i].fetch_add(binding.typeCulled[i],std::memory_order_relaxed);}binding=previous;}
 Scope(const Scope&)=delete;Scope&operator=(const Scope&)=delete;
};
void Publish(){
 static uint32_t last{};const auto now=GetTickCount();if(!prepared||now-last<5000)return;last=now;
 Event(1,"RECEIVER_CULL226_COUNTS","enabled="+std::to_string(mode.load())+" scopes="+std::to_string(scopes.load())+" valid="+std::to_string(validScopes.load())+" native_draw_candidates="+std::to_string(seen.load())+" culled="+std::to_string(culled.load())+" unknown="+std::to_string(unknown.load())+" render_thread="+std::to_string(scopeThread.load())+" allocator_thread="+std::to_string(allocationThread.load())+
  " no_frame="+std::to_string(reasons[0].load())+" deferred="+std::to_string(reasons[1].load())+" slot="+std::to_string(reasons[2].load())+" inactive="+std::to_string(reasons[3].load())+
  " kind="+std::to_string(reasons[4].load())+" packet="+std::to_string(reasons[5].load())+" view="+std::to_string(reasons[6].load())+" changed="+std::to_string(reasons[7].load()));
 for(unsigned i=0;i<4;++i)Event(1,"NATIVE_BOUND229_COUNTS","type="+std::to_string(i+1)+" visited="+std::to_string(types229[i].load())+" culled="+std::to_string(culledTypes229[i].load()));
}
}
extern "C" __declspec(dllexport) BOOL __cdecl LibertySetReceiverCull226(BOOL on){
 if(!ReceiverCull226::experimental||!ReceiverCull226::prepared||!addonActive.load()||GetCurrentThreadId()!=PlayerShadowAllocation::ownerThread.load())return FALSE;
 ReceiverCull226::mode.store(on!=0,std::memory_order_release);return TRUE;
}

extern "C" __declspec(dllexport) BOOL __cdecl LibertySetReceiverHull241(BOOL on){
 if(!ReceiverCull226::experimental||!ReceiverCull226::prepared||!addonActive.load()||GetCurrentThreadId()!=PlayerShadowAllocation::ownerThread.load())return FALSE;
 ReceiverCull226::tightHull241.store(on!=0,std::memory_order_release);return TRUE;
}
