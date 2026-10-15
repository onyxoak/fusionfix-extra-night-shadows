#define NOMINMAX
#include <windows.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
#include "SettingsRuntime117.hpp"
#include "NativeGraphicsMenu117.hpp"
#include "core/standalone-game-abi.hpp"
#include "core/ShadowFloatingPointState.hpp"
#include "core/ShadowReach.hpp"
#include "core/ShadowLightGeometry.hpp"
#include "core/NativeLampDistance38.hpp"
#include "core/VisibleReceiverRanking133.hpp"
#include "core/PlayerShadowBudget.hpp"
#include "core/ShadowAllocationPass.hpp"
namespace preset117 {ReadResult startup;}
namespace ModelEffectObserver {struct LastErrorScope {DWORD e=GetLastError();~LastErrorScope(){SetLastError(e);}};}
namespace CShadows {uint32_t frame{};uint32_t* pFrameCounter=&frame;}
namespace fusionfix::shadows {bool ShadowVolumeMayReachView(bool,Vec3,float){return true;}}
#define EXTRACT122_RUNTIME
#include <live-reach122-extracted.inc>
#undef EXTRACT122_RUNTIME
namespace gate122 {
 struct {unsigned depth=1;} state;
 std::atomic<bool> unsupportedThread{false};std::atomic<DWORD> ownerThread{0};
 bool enabled=true;bool Enabled(){return enabled;}
 void (*liveReachBoundary122)() noexcept=ApplyLiveReach122Boundary;
#define EXTRACT122_GATE
#include <live-reach122-extracted.inc>
#undef EXTRACT122_GATE
}
namespace caller122 {
 using namespace fusionfix::shadows;
 namespace budget=fusionfix::shadows::budget;
 using FloatingPointState=fusionfix::shadows::FloatingPointState;
 struct SafetyHookContext {uintptr_t esp{},edi{},esi{};struct {float f32[4]{};} xmm0;};
 rage::CLightSource fixtureLights[640]{};uint32_t fixtureCount=1;rage::CLightSource* CurrentLights(){return fixtureLights;}uint32_t CurrentCount(){return fixtureCount;}
 bool nativeLampPriority=true,publicationEnabled=true;bool Enabled(){return true;}unsigned lampDistanceAdjusted{};
 bool visibleReceiverExperiment133=false;float scenePriorityStrength=1.5f;
 bool (*visibleReceiverSource133)(const rage::CLightSource&,unsigned,rage::CLightSource*,uint32_t,uint32_t) noexcept=nullptr;
 struct State {
  unsigned depth=1;uint32_t frame=1;ShadowView view{};Vec3 player{};bool occupiedCar{};
  struct {bool Active(){return true;}}pass;
  struct {bool Ambiguous(uint32_t){return false;}}keyMultiplicity;
  struct {bool Retained(uint32_t,uint64_t,bool){return false;}}lampContinuity;
  bool volumeFrustum{};
 } state;
 uint64_t LampGeometry(const rage::CLightSource&){return 1;}
#define EXTRACT122_LAMP
#define EXTRACT122_REACH
#include <live-reach122-extracted.inc>
#undef EXTRACT122_LAMP
#undef EXTRACT122_REACH
}
namespace baseline122 {
 using namespace caller122;
 struct LegacySettings {std::atomic<int> lamp{40},head{10};int Get(std::string_view n){return n=="PREF_LAMP_REACH"?lamp.load(std::memory_order_relaxed):head.load(std::memory_order_relaxed);}} oldSettings;
#define FusionFixSettings oldSettings
#define EXTRACT122_LAMP
#define EXTRACT122_REACH
#include <live-reach122-extracted.inc>
#undef FusionFixSettings
#undef EXTRACT122_LAMP
#undef EXTRACT122_REACH
}
namespace startup122 {
 using namespace caller122;
 AddonSettings startupSettings;
#define FusionFixSettings startupSettings
#define EXTRACT122_LAMP
#define EXTRACT122_REACH
#include <live-reach122-extracted.inc>
#undef FusionFixSettings
#undef EXTRACT122_LAMP
#undef EXTRACT122_REACH
}
void Benchmark122(){
 using namespace caller122;
 const auto run=[](unsigned population,unsigned mode){
  fixtureCount=population;FusionFixSettings.SetReachPair122(mode==3?60:40,mode==3?15:10);
  for(unsigned i=0;i<population;++i){fixtureLights[i]={};fixtureLights[i].mPosition={float(10+i%95),0,0};fixtureLights[i].mFlags=4;fixtureLights[i].mRadius=20;fixtureLights[i].mCastShadows=int(100+i);}
  budget::ShadowAllocationPass pass;uint64_t digest{};
  const auto start=std::chrono::steady_clock::now();
  for(unsigned frame=1;frame<=256;++frame){pass.Begin({frame,frame,1,false},fixtureLights,population);
   uint32_t stack[8]{};stack[7]=4;
   for(unsigned i=0;i<population;++i){stack[5]=i;SafetyHookContext regs{reinterpret_cast<uintptr_t>(stack),reinterpret_cast<uintptr_t>(fixtureLights),uintptr_t(i)*sizeof(rage::CLightSource),{}};regs.xmm0.f32[0]=float(i+1);
    if(mode==0)baseline122::AdjustLampDistance(regs);else if(mode==1)startup122::AdjustLampDistance(regs);else caller122::AdjustLampDistance(regs);
    const float reach=mode==0?baseline122::CollectReach122(budget::Kind::Lamp,fixtureLights[i]):mode==1?startup122::CollectReach122(budget::Kind::Lamp,fixtureLights[i]):caller122::CollectReach122(budget::Kind::Lamp,fixtureLights[i]);
    const float d=fixtureLights[i].mPosition.x;
    pass.Observe({uint32_t(fixtureLights[i].mCastShadows),i,budget::Kind::Lamp,d*d,false,true,1,3,reach*reach},&fixtureLights[i]);
    digest+=uint64_t(regs.xmm0.f32[0]*1000);
   }
   int32_t indices[7]{-1,-1,-1,-1,-1,-1,-1};assert(pass.Commit(fixtureLights,population,indices));for(auto index:indices)digest+=uint64_t(index+1);
  }
  return std::pair{std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count()/256,digest};
 };
 for(unsigned population:{110u,500u}){std::array<double,16> a{},b{},c{},v{};for(unsigned pair=0;pair<16;++pair){const auto x=run(population,0),y=run(population,1),z=run(population,2),w=run(population,3);assert(x.second==y.second&&x.second==z.second);a[pair]=x.first;b[pair]=y.first;c[pair]=z.first;v[pair]=w.first;
   std::cout<<"PAIR "<<population<<' '<<pair<<' '<<a[pair]<<' '<<b[pair]<<' '<<c[pair]<<' '<<v[pair]<<'\n';}
  std::sort(a.begin(),a.end());std::sort(b.begin(),b.end());std::sort(c.begin(),c.end());std::sort(v.begin(),v.end());std::cout<<"MEDIAN "<<population<<' '<<(a[7]+a[8])/2<<' '<<(b[7]+b[8])/2<<' '<<(c[7]+c[8])/2<<' '<<(v[7]+v[8])/2<<'\n';
 }
}
int main(int argc,char** argv){
 using namespace preset117;
 const auto high=*Defaults(Preset::High),low=*Defaults(Preset::Low);
 auto customLive=high;assert(ChangeReach(customLive,true,60)&&ChangeReach(customLive,false,15));
 assert(!Defaults(Preset::VeryHigh));
 assert(high.lampReachSteps==50&&high.headlightReachSteps==10&&customLive.features==high.features&&customLive.lampReachSteps==60&&customLive.headlightReachSteps==15);
 for(int i=-5;i<=40;++i)assert(fusionfix::shadows::ShadowReachFeet(i)==(i<0?0:i)*5);
 assert(fusionfix::shadows::ShadowReachFeet(60)==300&&fusionfix::shadows::ShadowReachFeet(101)==500);
 wchar_t tmp[32768]{};assert(GetTempPathW(32768,tmp));const auto ini=std::filesystem::path(tmp)/(L"ls-reach122-"+std::to_wstring(GetCurrentProcessId())+L".ini");assert(!std::filesystem::exists(ini));
 for(auto s:{high,customLive,low})assert(Save(ini,s)&&Read(ini.c_str()).state==s);
 auto custom=customLive;ChangeReach(custom,true,100);assert(Save(ini,custom)&&Read(ini.c_str()).state==custom);
 assert(Save(ini,high));assert(WritePrivateProfileStringW(liberty_settings93::Section,L"Preset",L"Very High",ini.c_str()));assert(Read(ini.c_str()).state==high&&Read(ini.c_str()).migrated);
 std::filesystem::remove(ini);
 native_graphics117::Controller controller(high,customLive,false);controller.ObserveNight128(native_night128::State::Enabled);assert(controller.Pending()==customLive&&controller.Applied()==high);
 assert(!controller.Persist([](const State&){return false;}));assert(controller.QualityValue121()==L"Custom (save failed)");
 assert(controller.Persist([](const State&){return true;}));assert(controller.QualityValue121()==L"Custom (applying)");
 startup.present=true;startup.state=high;addonActive=true;gate122::ownerThread=GetCurrentThreadId();
 // Actual headlight consumer preceding SelectF sees old reach. LaterCollectF
 // sees published reach; only nextframe's headlight + SelectF+1 may acknowledge.
 assert(caller122::BeamReach122(true,200)==50*.3048f);
 const auto error=0x12200001u;SetLastError(error);const auto mx=_mm_getcsr();
 CShadows::frame=1;gate122::ApplyLiveReachAtSelection122(GetCurrentThreadId());
 assert(GetLastError()==error&&_mm_getcsr()==mx&&FusionFixSettings.Get("PREF_LAMP_REACH")==60&&liveReach122.Acknowledged(high)==high);
 assert(caller122::CollectReach122(fusionfix::shadows::budget::Kind::Lamp,caller122::fixtureLights[0])==300*.3048f);
 CShadows::frame=2;assert(caller122::BeamReach122(true,200)==75*.3048f);gate122::ApplyLiveReachAtSelection122(GetCurrentThreadId());assert(liveReach122.Acknowledged(high)==customLive);
 controller.ObserveApplied119(customLive,false,false,false);assert(!controller.RestartRequired()&&controller.QualityValue121()==L"Custom");
 assert(liveReach122.Request(high));gate122::state.depth=2;CShadows::frame=3;gate122::ApplyLiveReachAtSelection122(GetCurrentThreadId());assert(FusionFixSettings.Get("PREF_LAMP_REACH")==60);
 gate122::state.depth=1;gate122::ApplyLiveReachAtSelection122(GetCurrentThreadId()+1);assert(FusionFixSettings.Get("PREF_LAMP_REACH")==60);
 gate122::unsupportedThread=true;gate122::ApplyLiveReachAtSelection122(GetCurrentThreadId());assert(FusionFixSettings.Get("PREF_LAMP_REACH")==60);gate122::unsupportedThread=false;
 gate122::enabled=false;gate122::ApplyLiveReachAtSelection122(GetCurrentThreadId());assert(FusionFixSettings.Get("PREF_LAMP_REACH")==60);gate122::enabled=true;
 gate122::ApplyLiveReachAtSelection122(GetCurrentThreadId());assert(FusionFixSettings.Get("PREF_LAMP_REACH")==50);
 assert(liveReach122.Request(customLive));gate122::ApplyLiveReachAtSelection122(GetCurrentThreadId());assert(FusionFixSettings.Get("PREF_LAMP_REACH")==50);CShadows::frame=4;gate122::ApplyLiveReachAtSelection122(GetCurrentThreadId());assert(FusionFixSettings.Get("PREF_LAMP_REACH")==60);
 assert(liveReach122.Request(low));CShadows::frame=5;gate122::ApplyLiveReachAtSelection122(GetCurrentThreadId());assert(FusionFixSettings.Get("PREF_LAMP_REACH")==60&&liveReach122.Acknowledged(high)==customLive);
 auto off=high;off.enabled=false;assert(liveReach122.Request(off));CShadows::frame=6;gate122::ApplyLiveReachAtSelection122(GetCurrentThreadId());assert(FusionFixSettings.Get("PREF_LAMP_REACH")==60);
 auto invalid=customLive;invalid.lampReachSteps=101;assert(!liveReach122.Request(invalid)&&!FusionFixSettings.SetReachPair122(101,15));
 // Actual pre-cache distance callback: the same80m visible lamp gains priority
 // at300ft. Packet radius/intensity/type/flags/key remain byte-identical.
 using namespace caller122;state.view.valid=true;for(unsigned i=0;i<4;++i){state.view.view[i][i]=1;state.view.projection[i][i]=1;}state.view.projection[0][0]=.001f;
 fixtureLights[0].mPosition={80,0,0};fixtureLights[0].mFlags=4;fixtureLights[0].mRadius=20;fixtureLights[0].mIntensity=3;fixtureLights[0].mCastShadows=44;
 const auto before=fixtureLights[0];uint32_t stack[8]{};stack[5]=0;stack[7]=4;SafetyHookContext regs{reinterpret_cast<uintptr_t>(stack),reinterpret_cast<uintptr_t>(fixtureLights),0,{}};
 FusionFixSettings.SetReachPair122(40,10);regs.xmm0.f32[0]=100;AdjustLampDistance(regs);assert(regs.xmm0.f32[0]==100);
 FusionFixSettings.SetReachPair122(60,15);regs.xmm0.f32[0]=100;AdjustLampDistance(regs);assert(regs.xmm0.f32[0]<100&&!std::memcmp(&before,fixtureLights,sizeof(before)));
 assert(CollectReach122(budget::Kind::Lamp,fixtureLights[0])>80&&BeamReach122(true,200)<23&&BeamReach122(false,200)==200);
 FusionFixSettings.SetReachPair122(40,10);assert(CollectReach122(budget::Kind::Lamp,fixtureLights[0])<80&&BeamReach122(true,200)<16);
 budget::PlayerShadowBudget selection;selection.BeginPass({1,1,1,false});assert(!selection.Add({44,0,budget::Kind::Lamp,6400,false,true,1,3,CollectReach122(budget::Kind::Lamp,fixtureLights[0])*CollectReach122(budget::Kind::Lamp,fixtureLights[0])}));
 FusionFixSettings.SetReachPair122(60,15);selection.BeginPass({2,2,1,false});assert(selection.Add({44,0,budget::Kind::Lamp,6400,false,true,1,3,CollectReach122(budget::Kind::Lamp,fixtureLights[0])*CollectReach122(budget::Kind::Lamp,fixtureLights[0])}));
 if(argc==2&&std::string_view(argv[1])=="--benchmark")Benchmark122();
 std::cout<<"PASS live122 production gate/runtime/precache/collect/headlight consumption, three-tier INI/retired-VeryHigh migration/advanced Custom reach and truthful acknowledgment\n";
}
