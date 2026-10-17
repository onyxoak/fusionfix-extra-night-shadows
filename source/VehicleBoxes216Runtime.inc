// Vehicle fallback integration for MrBulkinAleksey's rounded-box method.
// GPL-3.0; see ATTRIBUTION.md. Numeric state is in VehicleBoxFallback216.hpp.
#include "core/VehicleBoxFallback216.hpp"
#include "PixelConstants225.hpp"
namespace VehicleBoxes216 {
bool requested=false,prepared=false;
std::atomic<bool> zeroLightGuard227{true};
std::atomic<bool> active{false};
SafetyHookMid handover;
vehicle_box216::Settings settings;
uint32_t* builtList=nullptr;uint32_t* drawnList=nullptr;
struct Snapshot {uint32_t list{},sequence{},tick{},frame{},count{};vehicle_box216::V focus{};std::array<vehicle_box216::Car,160> cars{};};
struct Cell {std::atomic_flag busy=ATOMIC_FLAG_INIT;Snapshot value{};};
std::array<Cell,8> snapshots;
unsigned nextSnapshot{};uint32_t sequence{};
thread_local Snapshot current;
thread_local vehicle_box216::State state;
thread_local uint32_t copiedFrame=UINT32_MAX,owner{};
std::atomic<uint32_t> captures{},draws{},excluded{},refused{},matched{},unmatched{};
template<class T> bool Copy(uintptr_t p,T& t) noexcept {return Liberty20::Copy(p,&t,sizeof(t));}
bool Pool(rage::VehiclePool& pool) noexcept {
 uint32_t address{};
 return CVehicle::pool&&Copy(uintptr_t(CVehicle::pool),address)&&address>=0x10000&&Copy(address,pool)&&
  pool.m_aStorage&&pool.m_aFlags&&pool.m_nSize>0&&pool.m_nSize<=4096&&pool.m_nStorageSize>=0x24&&pool.m_nStorageSize<=0x10000&&
  uint64_t(uintptr_t(pool.m_aStorage))+uint64_t(pool.m_nSize)*uint32_t(pool.m_nStorageSize)<=UINT32_MAX;
}
bool Validate(const vehicle_box216::Car& car,uint32_t entity,uint32_t type) noexcept {
 if(work_avoidance224::Enabled()&&((type!=2&&type!=3)||(type==2&&entity!=car.owner)))return false;
 rage::VehiclePool pool{};uint8_t flag{},after{};
 if(!Pool(pool)||car.slot>=uint32_t(pool.m_nSize)||
    uint64_t(uintptr_t(pool.m_aStorage))+uint64_t(car.slot)*uint32_t(pool.m_nStorageSize)!=car.owner||
    !Copy(uintptr_t(pool.m_aFlags)+car.slot,flag)||flag!=car.generation||(flag&0x80u))return false;
 bool result=type==2&&entity==car.owner;
 if(type==3){std::array<uint32_t,9> occupants{};
  if(!Copy(uintptr_t(car.owner)+0xf50,occupants))return false;
  result=std::find(occupants.begin(),occupants.end(),entity)!=occupants.end();
 }
 return result&&Copy(uintptr_t(pool.m_aFlags)+car.slot,after)&&after==flag;
}
bool Position(uintptr_t entity,vehicle_box216::V& p) noexcept {
 uint32_t matrix{};return entity>=0x10000&&Copy(entity+0x20,matrix)&&matrix>=0x10000&&Copy(matrix+0x30,p)&&vehicle_box216::Finite(p);
}
void Capture(SafetyHookContext&) noexcept {
 if(!active.load(std::memory_order_acquire)||!addonActive.load(std::memory_order_acquire)||!builtList)return;
 const ModelEffectObserver::LastErrorScope error;const fusionfix::shadows::FloatingPointState fp;
 const traffic_signal::fresh_page::Operation44 pages216(32);const immediate_region33::Scope regions216;
 auto& cell=snapshots[nextSnapshot++%snapshots.size()];if(cell.busy.test_and_set(std::memory_order_acquire))return;
 struct Release {Cell& cell;~Release(){cell.busy.clear(std::memory_order_release);}}release{cell};
 auto& next=cell.value;next={};next.tick=GetTickCount();next.sequence=++sequence;
 if(!Copy(uintptr_t(builtList),next.list)||!next.list||!CShadows::pFrameCounter||!Copy(uintptr_t(CShadows::pFrameCounter),next.frame)||
    !CPlayer::getLocalPlayerPed||!Position(CPlayer::getLocalPlayerPed(),next.focus))return;
 rage::VehiclePool pool{};if(!Pool(pool)){next.list=0;return;}
 std::array<std::pair<float,vehicle_box216::Car>,160> nearest{};unsigned count=0;
 for(int i=0;i<pool.m_nSize;++i){
  uint8_t flag{};if(!Copy(uintptr_t(pool.m_aFlags)+i,flag)||(flag&0x80u))continue;
  const auto address=uintptr_t(pool.m_aStorage)+uintptr_t(i)*pool.m_nStorageSize;
  uint32_t matrix{},info{};int16_t model{};std::array<float,16> pose{};vehicle_box216::V lo{},hi{};
  if(!Copy(address+0x20,matrix)||matrix<0x10000||!Copy(matrix,pose)||!Copy(address+0x2e,model)||model<0||model>=31000||
     !Copy(game+0xe95cd8+uint32_t(model)*4,info)||info<0x10000||!Copy(info+0x20,lo)||!Copy(info+0x30,hi))continue;
  vehicle_box216::Car car{};car.width=(hi.x-lo.x)*.5f;car.length=(hi.y-lo.y)*.5f;car.height=(hi.z-lo.z)*.5f;
  if(!vehicle_box216::Finite(lo)||!vehicle_box216::Finite(hi)||!(car.width>.1f&&car.width<2&&car.length>.2f&&car.length<10&&car.height>.1f&&car.height<2.5f))continue;
  bool finite=true;for(float f:pose)finite&=std::isfinite(f);if(!finite)continue;
  const float flat=std::sqrt(pose[4]*pose[4]+pose[5]*pose[5]);if(flat<=.5f)continue;
  const float x=(lo.x+hi.x)*.5f,y=(lo.y+hi.y)*.5f,z=(lo.z+hi.z)*.5f;
  car.centre={pose[12]+pose[0]*x+pose[4]*y+pose[8]*z,pose[13]+pose[1]*x+pose[5]*y+pose[9]*z,pose[14]+pose[2]*x+pose[6]*y+pose[10]*z};
  car.fx=pose[4]/flat;car.fy=pose[5]/flat;car.owner=uint32_t(address);car.slot=uint32_t(i);car.generation=flag;
  if(!vehicle_box216::Finite(car.centre)||!Validate(car,car.owner,2))continue;
  const float score=vehicle_box216::D2(car.centre,next.focus);
  if(score>100.f*100.f)continue;
  unsigned at=count;
  if(count<nearest.size())++count;
  else {at=unsigned(std::max_element(nearest.begin(),nearest.end(),[](auto a,auto b){return a.first<b.first;})-nearest.begin());if(nearest[at].first<=score)continue;}
  nearest[at]={score,car};
 }
 next.count=count;for(unsigned i=0;i<count;++i)next.cars[i]=nearest[i].second;
 uint32_t still{};if(!Copy(uintptr_t(builtList),still)||still!=next.list){next={};return;}
 ++captures;
}
bool CurrentSnapshot(uint32_t frame,uint32_t now) noexcept {
 uint32_t list{};if(!drawnList||!Copy(uintptr_t(drawnList),list)||!list)return false;
 if(copiedFrame!=frame||current.list!=list){
  uint32_t best=0;bool found=false;
  for(auto& cell:snapshots){if(cell.busy.test_and_set(std::memory_order_acquire))continue;
   const auto& s=cell.value;
   if(s.list==list&&s.count<=160&&now-s.tick<150u&&(!found||int32_t(s.sequence-best)>0)){current=s;best=s.sequence;found=true;}
   cell.busy.clear(std::memory_order_release);
  }
  copiedFrame=frame;if(found)++matched;else {current={};++unmatched;return false;}
 }
 return current.list==list&&current.count<=160&&now-current.tick<150u;
}
IDirect3DDevice9* Device() noexcept {
 const traffic_signal::fresh_page::LocalReadScope233 deviceReads233;

 uint32_t pointer{},vt{},method{};
 if(!Copy(game+0x13ed8d8,pointer)||pointer<0x10000||!Copy(pointer,vt)||!Copy(vt+109*4,method)||method!=game+0x225b0)return nullptr;
 std::array<uint8_t,pixel_constants225::CachedBody.size()> code{};
 if(!Liberty20::Copy(method,code.data(),code.size())||
    (!pixel_constants225::Cached(code)&&!gpu39::Forwarder(std::span<const uint8_t>(code.data(),22),109*4)))return nullptr;
 IDirect3DDevice9* proxy{};auto* device=reinterpret_cast<IDirect3DDevice9*>(uintptr_t(pointer));
 return GpuProfiling39::Current(device)&&GpuProfiling39::DeviceSlots(device,proxy)?device:nullptr;
}
bool Mapped(const rage::CLightSource& light) noexcept {
 if(!(light.mFlags&4u)||!light.mCastShadows)return false;
 uint32_t bank{};if(!Copy(game+0xd74794,bank)||bank>1)return false;
 for(unsigned slot=1;slot<=7;++slot){uint32_t key{};uint8_t activeSlot{};
  const auto at=game+0xd9faed+bank*0x880+slot*0x110;
  if(Copy(at,activeSlot)&&activeSlot==1&&Copy(at+11,key)&&key==uint32_t(light.mCastShadows))return true;
 }
 return false;
}
void Lookup(SafetyHookContext& regs) noexcept {
 if(!active.load(std::memory_order_acquire))return;
 const ModelEffectObserver::LastErrorScope error;const fusionfix::shadows::FloatingPointState fp;
 const traffic_signal::fresh_page::Operation44 pages216(32);const immediate_region33::Scope regions216;
 const auto thread=GetCurrentThreadId();if(owner&&owner!=thread){++refused;return;}owner=thread;
 auto* device=Device();if(!device){++refused;return;}
 const float off[4]{};if(FAILED(device->SetPixelShaderConstantF(150,off,1))){++refused;return;}
 if(!addonActive.load(std::memory_order_acquire)||!PlayerShadowAllocation::Enabled()||!bExtraNightShadows||regs.edi<0x10028||!CShadows::pFrameCounter)return;
 uint32_t frame{};rage::CLightSource light{};
 if(!Copy(uintptr_t(CShadows::pFrameCounter),frame)||!Copy(uintptr_t(regs.edi)-0x28,light))return;
 const auto now=GetTickCount();if(!CurrentSnapshot(frame,now))return;
 state.Begin(frame,now,settings.perCar);
 vehicle_box216::V eye=current.focus;
 const auto* vp=PlayerShadowAllocation::SceneViewport();
 // Camera position is supplied by the already copied gameplay view when valid.
 if(vp){fusionfix::shadows::ShadowView view=PlayerShadowAllocation::ReadGameplayView();if(view.valid){float e[3]{};for(unsigned i=0;i<3;++i)for(unsigned j=0;j<3;++j)e[i]-=view.view[3][j]*view.view[i][j];vehicle_box216::V camera{e[0],e[1],e[2]};if(vehicle_box216::Finite(camera))eye=camera;}}
 vehicle_box216::Light numeric{{light.mPosition.x,light.mPosition.y,light.mPosition.z},light.mRadius,light.mIntensity,
  uint32_t(light.mCastShadows),light.mFlags,uint32_t(light.mType),uint32_t(light.mInteriorIndex),uint32_t(light.mRoomIndex)};
 const auto out=state.Select(numeric,current.cars.data(),current.count,current.focus,eye,Mapped(light),settings,Validate);
 if(!out.count)return;
 if(FAILED(device->SetPixelShaderConstantF(143,out.constants.data(),7))){++refused;return;}
 const float on[4]{216.f,-216.f,1.f,zeroLightGuard227.load(std::memory_order_relaxed)?1.f:0.f};
 if(SUCCEEDED(device->SetPixelShaderConstantF(150,on,1)))++draws;else ++refused;
}
bool Exclude(uint32_t key,uintptr_t entity,uint32_t type) noexcept {
 if(!active.load(std::memory_order_acquire)||!addonActive.load(std::memory_order_acquire)||GetCurrentThreadId()!=owner||entity>UINT32_MAX)return false;
 if(type!=2&&type!=3)return false;
 if(work_avoidance224::Enabled()&&type==2&&!state.MayContainOwner224(uint32_t(entity)))return false;
 const ModelEffectObserver::LastErrorScope error;const fusionfix::shadows::FloatingPointState fp;
 uint32_t frame{};if(!CShadows::pFrameCounter||!Copy(uintptr_t(CShadows::pFrameCounter),frame))return false;
 state.Begin(frame,GetTickCount(),settings.perCar);
 if(state.Exclude(key,uint32_t(entity),type,Validate)){++excluded;return true;}return false;
}
void PrepareHooks() noexcept {
 if(!requested)return;
 try {
  Require(ConsumerShadow75::prepared,"boxes216_consumer_unavailable");
  constexpr uint32_t rva=0x6c2dd0,size=0x40;
  const auto bytes=ReadMemory(game+rva,size);
  Require(original&&rva+size<=original->bytes.size()&&!memcmp(bytes.data(),original->bytes.data()+rva,size),"boxes216_handover_changed");
  constexpr uint8_t prefix[]{0x8b,0x0d};Require(!memcmp(bytes.data(),prefix,2)&&bytes[6]==0x33&&bytes[7]==0xc0&&bytes[15]==0xa1,"boxes216_handover_layout");
  uint32_t filled{};memcpy(&filled,bytes.data()+16,4);Require(filled==game+0xc3eedc,"boxes216_handover_list");
  constexpr uint32_t drawnRva=0x6bece7;const auto drawBytes=ReadMemory(game+drawnRva,16);
  Require(drawnRva+16<=original->bytes.size()&&!memcmp(drawBytes.data(),original->bytes.data()+drawnRva,16),"boxes216_drawn_list_changed");
  uint32_t drawnAddress{};memcpy(&drawnAddress,drawBytes.data()+8,4);
  Require(drawBytes[0]==0x81&&drawBytes[1]==0xc6&&drawBytes[6]==0x89&&drawBytes[7]==0x35&&drawnAddress==game+0xc3eed0,"boxes216_drawn_list_layout");
  builtList=reinterpret_cast<uint32_t*>(uintptr_t(filled));drawnList=reinterpret_cast<uint32_t*>(game+0xc3eed0);
  const auto shader=pe::ModulePath(GetModuleHandleW(nullptr)).parent_path()/L"update/common/shaders/win32_30/deferred_lighting.fxc";
  const auto shaderHash227=pe::Hash(pe::Read(shader));
  Require(box_shader216::SupportsFallback(shaderHash227),"boxes216_paired_shader_missing");
  Prepare(handover,game+rva,Capture);Require(handover.original_bytes().size()==6,"boxes216_handover_span");prepared=true;
 }catch(const std::exception& e){Event(1,"VEHICLE_BOX216_REFUSED",e.what());prepared=false;}
}
void Disable() noexcept {active=false;OwnHeadlightCaster::boxExclude216=nullptr;if(handover)(void)handover.disable();prepared=false;}
void Enable() noexcept {
 if(!requested||!prepared||!ConsumerShadow75::active.load())return;
 if(handover.enable()){active=true;OwnHeadlightCaster::boxExclude216=Exclude;Event(1,"VEHICLE_BOX216_ACTIVE","credit=MrBulkinAleksey fallback=rounded_boxes maps_added=0 per_light_max=3 per_car_max=2 runtime_pixels_unverified=1");}
}
void Publish() {
 static uint32_t last{};if(!requested||GetTickCount()-last<5000u)return;last=GetTickCount();
 Event(1,"VEHICLE_BOX216_COUNTS","active="+std::to_string(active.load())+" captures="+std::to_string(captures.load())+" matched="+std::to_string(matched.load())+" unmatched="+std::to_string(unmatched.load())+" draws="+std::to_string(draws.load())+" excluded="+std::to_string(excluded.load())+" refused="+std::to_string(refused.load())+" zero_light_guard227="+std::to_string(zeroLightGuard227.load()));
}
}

extern "C" __declspec(dllexport) BOOL __cdecl LibertySetZeroLightGuard227(BOOL on){
 if(!ReceiverCull226::experimental||!VehicleBoxes216::active.load()||GetCurrentThreadId()!=PlayerShadowAllocation::ownerThread.load())return FALSE;
 VehicleBoxes216::zeroLightGuard227.store(on!=0,std::memory_order_release);return TRUE;
}
