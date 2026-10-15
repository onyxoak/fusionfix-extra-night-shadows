#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <intrin.h>
#include <xmmintrin.h>
#include <atomic>
#include <array>
#include <cassert>
#include <cstdio>
#include <memory>
#include <limits>
#include <cstring>
#include <chrono>
#include <vector>
#include <algorithm>
bool benchmark61=false;volatile float benchmarkSink61=0;
HANDLE gate61{};std::atomic<unsigned> workerVisits61{0},workerChecks61{0};
alignas(16) thread_local unsigned char beforeFx61[512];
void BeforeWorker61(){if(benchmark61)return;workerVisits61.fetch_add(1);assert(WaitForSingleObject(gate61,5000)==WAIT_OBJECT_0);_mm_setcsr(0x9fc0);__asm fninit
 __asm fld1
 SetLastError(813);_fxsave(beforeFx61);}
void AfterWorker61(){if(benchmark61)return;alignas(16)unsigned char after[512];_fxsave(after);assert(GetLastError()==813);assert(!std::memcmp(beforeFx61,after,5));assert(!std::memcmp(beforeFx61+24,after+24,4));assert(!std::memcmp(beforeFx61+32,after+32,10));workerChecks61.fetch_add(1);}
unsigned createCalls61{},failCreate61{};
HANDLE CreateWorker61(LPTHREAD_START_ROUTINE function,void*context){++createCalls61;if(failCreate61&&createCalls61==failCreate61){SetLastError(ERROR_NOT_ENOUGH_MEMORY);return nullptr;}return CreateThread(nullptr,0,function,context,0,nullptr);}
#define CE_PARALLEL_WORKER_BEFORE61 BeforeWorker61
#define CE_PARALLEL_WORKER_AFTER61 AfterWorker61
#define CE_PARALLEL_CREATE_THREAD61(function,context) CreateWorker61(function,context)
#include "core/ParallelSelectionMath61.hpp"
#include "core/InteriorScenePriority211.hpp"
#include "core/IndoorBubble212.hpp"
bool indoorBubble212=false;
bool interiorPriority211=false;
namespace m=fusionfix::shadows::parallel61;using fusionfix::shadows::Vec3;
struct Light61 {Vec3 mPosition{},mDirection{},mColor{};float mRadius{},mIntensity{},mOuterConeAngle{};int mType{},mInteriorIndex{},mRoomIndex{};uint32_t mFlags{};};
struct State61 {fusionfix::shadows::ShadowView view{};Vec3 player{},drivingFocus{};uintptr_t occupiedCar{};unsigned parallelPhases61=3;} state;
bool parallelRequested61=false; std::atomic<m::Service*> parallelService61{nullptr};std::atomic<bool>unsupportedThread{false};float scenePriorityStrength=1.5f;
bool(*labPointSource)(const Light61&)=nullptr;
m::Context ParallelContext61(){m::Context c{};c.view=state.view;c.player=state.player;c.focus=state.drivingFocus;c.driving=state.occupiedCar!=0;c.strength=scenePriorityStrength;m::CaptureControl(c);return c;}
bool parallelMasterEnabled61=false;std::atomic<bool>parallelAttempted61{false};std::atomic<unsigned>parallelStartupStatus61{0};
unsigned pinCalls61{},allocCalls61{};bool failPin61=false,failAlloc61=false;
BOOL Pin61(DWORD,LPCWSTR,HMODULE*module){++pinCalls61;if(failPin61)return FALSE;*module=GetModuleHandleW(nullptr);return TRUE;}
m::Service* Alloc61(){++allocCalls61;return failAlloc61?nullptr:new(std::nothrow)m::Service;}
#define CE_PARALLEL_PIN61(flags,address,module) Pin61(flags,address,module)
#define CE_PARALLEL_ALLOC61() Alloc61()
#include "startup-callsite61.inc"
void StartupTests61(){
 parallelRequested61=true;parallelMasterEnabled61=false;StartParallel61();assert(!parallelAttempted61&&pinCalls61==0&&allocCalls61==0);
 parallelMasterEnabled61=true;parallelRequested61=false;StartParallel61();assert(!parallelAttempted61&&pinCalls61==0);
 parallelRequested61=true;
 for(unsigned failure=0;failure<3;++failure){
  parallelAttempted61=false;parallelStartupStatus61=0;failPin61=failure==0;failAlloc61=failure==1;
  createCalls61=0;failCreate61=failure==2?2:0;SetLastError(917);StartParallel61();
  assert(GetLastError()==917&&parallelAttempted61&&!parallelService61.load()&&parallelStartupStatus61==2+failure);
  const auto pins=pinCalls61,allocs=allocCalls61,threads=createCalls61;
  StartParallel61();assert(pinCalls61==pins&&allocCalls61==allocs&&createCalls61==threads);
 }
 failPin61=false;failAlloc61=false;failCreate61=0;parallelAttempted61=false;StartParallel61();
 assert(parallelStartupStatus61==6&&parallelService61.load());
 auto* service=parallelService61.load();{const fusionfix::shadows::FloatingPointState fp;assert(service->Begin(ParallelContext61()));}
 service->Stop();delete service;parallelService61=nullptr;parallelRequested61=false;
}
float ViewCall61(const Light61&light,unsigned index,uint32_t key,uint32_t flags){
 const fusionfix::shadows::FloatingPointState preserve;
#include "view-callsite61.inc"
 return viewWeight;
}
fusionfix::shadows::SceneReceiverConsequence44 SceneCall61(const Light61&light,unsigned index,uint32_t key,uint64_t generation,float viewWeight,bool volumeVisible,bool indoorSource211=false){
 const fusionfix::shadows::FloatingPointState preserve;const Vec3 source=light.mPosition,direction=light.mDirection;
 const bool visibleReceiverExperiment133=false,sourceSupported133=indoorSource211;const float receiverBenefit133=0.f;
#include "scene-callsite61.inc"
 return consequence;
}
bool Bits(float a,float b){return !std::memcmp(&a,&b,4);}
void EqualConsequence(const fusionfix::shadows::SceneReceiverConsequence44&a,const fusionfix::shadows::SceneReceiverConsequence44&b){assert(a.valid==b.valid&&Bits(a.subjectSupport,b.subjectSupport)&&Bits(a.roadSupport,b.roadSupport)&&Bits(a.consequence,b.consequence)&&Bits(a.score,b.score));}
m::Input ViewInput61(const Light61&l,unsigned index,unsigned key,unsigned flags){m::Input in{};in.slot=index*2;in.key=key;in.phase=0;in.position={l.mPosition.x,l.mPosition.y,l.mPosition.z};in.direction={l.mDirection.x,l.mDirection.y,l.mDirection.z};in.radius=l.mRadius;in.beam=(flags&0x100)!=0;return in;}
void WaitReady61(m::Service&s,const m::Context&c,const m::Input&in){m::Output out{};const auto end=GetTickCount64()+5000;while(!s.TryGet(c,in,out)){assert(GetTickCount64()<end);Sleep(1);}}
void DependencyTests61(m::Service&service,const m::Context&c,const Light61&light){
 auto view=ViewInput61(light,3900,333,0x100);auto scene=view;scene.slot++;scene.phase=1;scene.generationLow=71;scene.generationHigh=17;
 scene.type=2;scene.rgb={.2f,.7f,1};scene.intensity=4;scene.outerCone=.8f;scene.viewWeight=.8f;scene.volumeVisible=1;
 assert(service.Begin(c));assert(service.Record(c,view));assert(service.Record(c,scene));assert(service.Dispatch());
 WaitReady61(service,c,view);WaitReady61(service,c,scene);
 // Each context member independently changed; ignored fields must still give exact Compute output.
 for(unsigned phase=0;phase<2;++phase){
  const auto&in=phase?scene:view;
  for(unsigned field=0;field<12;++field){
   auto next=c;switch(field){case 0:next.view.valid=!c.view.valid;break;
   case 1:next.view.view[0][0]+=1;break;case 2:next.view.projection[0][0]+=1;break;
   case 3:next.player.x+=1;break;case 4:next.player.y=-0.f;break;case 5:next.player.z+=1;break;
   case 6:next.focus.x+=1;break;case 7:next.focus.y+=1;break;case 8:next.focus.z+=1;break;
   case 9:next.driving^=1;break;case 10:next.strength+=1;break;case 11:next.strength=-0.f;break;}
   m::Output out{};const bool matched=service.TryGet(next,in,out);
   assert(matched==(phase?field<3:field>=3));
   if(matched){const auto direct=m::Compute(next,in);assert(Bits(out.viewWeight,direct.viewWeight));EqualConsequence(out.consequence,direct.consequence);}
  }
  for(unsigned control=0;control<2;++control){auto next=c;if(control)next.control^=0x400;else next.mxcsr^=0x8000;m::Output out{};assert(!service.TryGet(next,in,out));}
 }
 // Every initialized Input word is compared, including NaN payload and signed zero bits.
 for(unsigned phase=0;phase<2;++phase){
  const auto&original=phase?scene:view;
  for(unsigned word=0;word<sizeof(m::Input)/4;++word){
   auto changed=original;auto*bytes=reinterpret_cast<unsigned char*>(&changed);bytes[word*4]^=0x80;
   m::Output out{};out.viewWeight=999;assert(!service.TryGet(c,changed,out)&&out.viewWeight==999);
  }
 }
 auto changedContext=c;changedContext.focus.z+=1;assert(service.Begin(c));assert(!service.Record(changedContext,view));assert(!service.Dispatch());
 // Actual worker result published for a static scenery input. A same-valid
 // camera matrix change must refuse that result even with player/focus exact.
 auto staticContext163=c;staticContext163.visibleReceiver133=1;staticContext163.lampReach133=76.2f;
 auto staticInput163=scene;staticInput163.slot=3910;staticInput163.beam=0;staticInput163.type=0;
 staticInput163.position={.5f,0,.3f};staticInput163.radius=.1f;staticInput163.viewWeight=3;
 assert(service.Begin(staticContext163)&&service.Record(staticContext163,staticInput163)&&service.Dispatch());
 WaitReady61(service,staticContext163,staticInput163);m::Output staticOutput163{};
 assert(service.TryGet(staticContext163,staticInput163,staticOutput163));
 for(unsigned matrix163=0;matrix163<2;++matrix163){
  auto moved163=staticContext163;if(matrix163)moved163.view.projection[0][0]*=2;else moved163.view.view[3][0]+=.5f;
  assert(moved163.view.valid==staticContext163.view.valid&&!std::memcmp(&moved163.player,&staticContext163.player,sizeof(moved163.player))&&!std::memcmp(&moved163.focus,&staticContext163.focus,sizeof(moved163.focus)));
  m::Output sentinel163{};sentinel163.viewWeight=913;sentinel163.consequence.score=914;
  assert(!service.TryGet(moved163,staticInput163,sentinel163)&&sentinel163.viewWeight==913&&sentinel163.consequence.score==914);
  assert(m::Compute(moved163,staticInput163).consequence.score!=staticOutput163.consequence.score);
 }
 auto disabled163=staticContext163;disabled163.visibleReceiver133=0;auto movedOff163=disabled163;movedOff163.view.view[3][0]+=.5f;
 assert(m::EqualPhase(disabled163,movedOff163,1)); // Inherited Off dependencies unchanged.
}
struct Other61 {m::Service*s;m::Context c;m::Input in;};
DWORD WINAPI WrongOwner61(void*v){auto&t=*static_cast<Other61*>(v);m::Output out{};assert(!t.s->Begin(t.c));assert(!t.s->Record(t.c,t.in));assert(!t.s->TryGet(t.c,t.in,out));assert(!t.s->Dispatch());t.s->Stop();return 0;}
DWORD WINAPI Startup61(void* p){auto& service=*static_cast<m::Service*>(p);assert(service.Start(2));assert(service.ReleaseProducer());return 0;}
void Benchmark61(){
 benchmark61=true;gate61=CreateEventW(nullptr,TRUE,TRUE,nullptr);auto service=std::make_unique<m::Service>();assert(service->Start(2));parallelService61=service.get();
 state.view.valid=true;for(unsigned i=0;i<4;++i){state.view.view[i][i]=1;state.view.projection[i][i]=1;}state.player={0,0,.3f};state.drivingFocus={6,1,.3f};state.occupiedCar=1;
 const auto initialState=state;
 std::array<Light61,512> lights{};for(unsigned i=0;i<lights.size();++i){auto&l=lights[i];l.mPosition={float(int(i%19)-9),float(int(i%7)-3),.3f};l.mDirection={.1f,-.3f,-1};l.mColor={.2f,.7f,1};l.mRadius=15+float(i%20);l.mIntensity=2+float(i%40);l.mOuterConeAngle=.8f;l.mType=(i%2)?2:0;}
 for(unsigned mode=0;mode<8;++mode){state=initialState;parallelRequested61=(mode&1)!=0;const unsigned changing=mode/2;std::vector<double>times;unsigned active=0;const auto hit0=service->completedHits.load(),miss0=service->serialMisses.load(),submitted0=service->submitted.load();
  for(unsigned pass=0;pass<100;++pass){if(changing&1)state.player.x+=.03125f;if(changing&2)state.view.view[3][0]+=.03125f;auto start=std::chrono::steady_clock::now();{const fusionfix::shadows::FloatingPointState fp;state.parallelPhases61=parallelRequested61?service->BeginStable(ParallelContext61()):0;active+=state.parallelPhases61!=0;
   for(unsigned i=0;i<lights.size();++i){const auto view=ViewCall61(lights[i],i,100+i,i%3?0:0x100);const auto scene=SceneCall61(lights[i],i,100+i,0x100000000ull+i,view,true);benchmarkSink61=view+scene.score;}
   if(state.parallelPhases61)(void)service->Dispatch();}auto end=std::chrono::steady_clock::now();if(pass>=10)times.push_back(std::chrono::duration<double,std::micro>(end-start).count());Sleep(1);}
  std::sort(times.begin(),times.end());std::printf("mode=%u requested=%u changing_mask=%u samples=%zu median_us=%.3f p95_us=%.3f active_passes=%u hits=%u misses=%u submitted=%u\n",mode,parallelRequested61,changing,times.size(),times[times.size()/2],times[times.size()*95/100],active,service->completedHits.load()-hit0,service->serialMisses.load()-miss0,service->submitted.load()-submitted0);
 }
 service->Stop();parallelService61=nullptr;CloseHandle(gate61);
}
int main(){
 if(GetEnvironmentVariableW(L"CE_TIMING61",nullptr,0)){Benchmark61();return 0;}
 StartupTests61();
 gate61=CreateEventW(nullptr,TRUE,TRUE,nullptr);assert(gate61);auto service=std::make_unique<m::Service>();
 SetLastError(431);assert(service->Start(2));assert(GetLastError()==431&&service->Workers()==2);parallelService61=service.get();
 state.view.valid=true;for(unsigned i=0;i<4;++i){state.view.view[i][i]=1;state.view.projection[i][i]=1;}state.player={0,0,.3f};state.drivingFocus={6,1,.3f};state.occupiedCar=1;
 std::array<Light61,128>lights{};std::array<float,128>view{};std::array<fusionfix::shadows::SceneReceiverConsequence44,128>scene{};
 for(unsigned i=0;i<lights.size();++i){auto&l=lights[i];l.mPosition={float(int(i%19)-9),float(int(i%7)-3),.3f};l.mDirection={.1f,-.3f,-1};l.mColor={.2f,.7f,1};l.mRadius=15+float(i%20);l.mIntensity=2+float(i%40);l.mOuterConeAngle=.8f;l.mType=(i%2)?2:0;}
 lights[1].mRadius=0;lights[2].mRadius=-1;lights[3].mIntensity=std::numeric_limits<float>::quiet_NaN();lights[4].mDirection.x=std::numeric_limits<float>::infinity();lights[5].mPosition.x=-0.f;lights[6].mColor.y=std::numeric_limits<float>::quiet_NaN();
 {const fusionfix::shadows::FloatingPointState fp;const auto c=ParallelContext61();assert(service->Begin(c));
  for(unsigned i=0;i<lights.size();++i){view[i]=ViewCall61(lights[i],i,100+i,i%3?0:0x100);scene[i]=SceneCall61(lights[i],i,100+i,0x100000000ull+i,view[i],true);}
  assert(!service->Dispatch()); // OFF callsites record no tape and preserve original output.
  parallelRequested61=true;assert(service->Begin(c));
  for(unsigned i=0;i<lights.size();++i){assert(Bits(view[i],ViewCall61(lights[i],i,100+i,i%3?0:0x100)));EqualConsequence(scene[i],SceneCall61(lights[i],i,100+i,0x100000000ull+i,view[i],true));}
  assert(service->Dispatch());WaitReady61(*service,c,ViewInput61(lights[0],0,100,0x100));assert(service->Begin(c));const auto hits=service->completedHits.load();
  for(unsigned i=0;i<lights.size();++i){assert(Bits(view[i],ViewCall61(lights[i],i,100+i,i%3?0:0x100)));EqualConsequence(scene[i],SceneCall61(lights[i],i,100+i,0x100000000ull+i,view[i],true));}
  assert(service->completedHits==hits+256);
  // Exact numeric/context/key/index/FP changes must immediately use serial fallback.
  const auto input=ViewInput61(lights[0],0,100,0x100);m::Output unchanged{};unchanged.viewWeight=911;
  for(unsigned change=0;change<9;++change){auto next=input;auto nextContext=c;switch(change){case 0:next.position[0]+=.1f;break;case 1:next.direction[0]+=.1f;break;case 2:next.radius+=1;break;case 3:next.key++;break;case 4:next.slot+=2;break;case 5:nextContext.view.projection[0][0]+=1;break;case 6:nextContext.view.view[0][0]+=1;break;case 7:nextContext.mxcsr^=0x8000;break;case 8:nextContext.control^=0x400;break;}assert(!service->TryGet(nextContext,next,unchanged)&&unchanged.viewWeight==911);}
  DependencyTests61(*service,c,lights[0]);
  Other61 other{service.get(),c,input};HANDLE thread=CreateThread(nullptr,0,WrongOwner61,&other,0,nullptr);assert(thread);assert(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);CloseHandle(thread);assert(service->Workers()==2);
 }
 // Fresh serial and worker parity for on-foot and zero scene priority.
 {const fusionfix::shadows::FloatingPointState fp;
  for(unsigned config=0;config<2;++config){state.occupiedCar=0;scenePriorityStrength=config?0.f:1.5f;const auto c=ParallelContext61();
   parallelRequested61=false;const auto v=ViewCall61(lights[0],10,3100,0x100);const auto expected=SceneCall61(lights[0],10,3100,21,v,true);
   parallelRequested61=true;assert(service->Begin(c));EqualConsequence(expected,SceneCall61(lights[0],10,3100,21,v,true));assert(service->Dispatch());
   m::Input in{};in.slot=21;in.key=3100;in.phase=1;in.generationLow=21;in.position={lights[0].mPosition.x,lights[0].mPosition.y,lights[0].mPosition.z};
   in.direction={lights[0].mDirection.x,lights[0].mDirection.y,lights[0].mDirection.z};in.radius=lights[0].mRadius;in.intensity=lights[0].mIntensity;in.type=lights[0].mType;in.outerCone=lights[0].mOuterConeAngle;
   in.rgb={lights[0].mColor.x,lights[0].mColor.y,lights[0].mColor.z};in.viewWeight=v;in.volumeVisible=1;WaitReady61(*service,c,in);
   EqualConsequence(expected,SceneCall61(lights[0],10,3100,21,v,true));
  }state.occupiedCar=1;scenePriorityStrength=1.5f;
 }
 // Both jobs running while blocked: busy dispatch returns immediately and caller result stays serial.
 ResetEvent(gate61);const auto visits=workerVisits61.load();
 {const fusionfix::shadows::FloatingPointState fp;const auto c=ParallelContext61();for(unsigned batch=0;batch<2;++batch){assert(service->Begin(c));assert(service->Record(c,ViewInput61(lights[0],0,900+batch,0)));assert(service->Dispatch());}
  const auto end=GetTickCount64()+5000;while(workerVisits61<visits+2){assert(GetTickCount64()<end);Sleep(1);}assert(service->Begin(c));assert(service->Record(c,ViewInput61(lights[0],0,950,0)));assert(!service->Dispatch());assert(service->busyDrops>=1);
  // Overflow abandons the whole request tape, never changes serial current math.
  assert(service->Begin(c));auto in=ViewInput61(lights[0],0,990,0);for(unsigned i=0;i<m::Slots;++i)assert(service->Record(c,in));assert(!service->Record(c,in));assert(!service->Dispatch());
 }
 SetEvent(gate61);service->Stop();assert(service->Workers()==0);assert(workerChecks61>=3);service.reset();parallelService61=nullptr;
 // Partial thread-creation failure is cleaned up, and the same service can restart.
 auto failed=std::make_unique<m::Service>();createCalls61=0;failCreate61=2;assert(!failed->Start(2)&&failed->Workers()==0);failCreate61=0;assert(failed->Start(1)&&failed->Workers()==1);failed->Stop();
 auto transferred=std::make_unique<m::Service>();HANDLE startup=CreateThread(nullptr,0,Startup61,transferred.get(),0,nullptr);assert(startup);assert(WaitForSingleObject(startup,5000)==WAIT_OBJECT_0);CloseHandle(startup);
 {const fusionfix::shadows::FloatingPointState fp;const auto c=ParallelContext61();assert(transferred->Begin(c));assert(transferred->Record(c,ViewInput61(lights[0],0,2000,0)));assert(transferred->Dispatch());WaitReady61(*transferred,c,ViewInput61(lights[0],0,2000,0));}transferred->Stop();
 auto stable=std::make_unique<m::Service>();assert(stable->Start(1));{const fusionfix::shadows::FloatingPointState fp;auto c=ParallelContext61();assert(stable->BeginStable(c)==0);assert(stable->BeginStable(c)==0);assert(stable->BeginStable(c)==3);c.player.x+=1;assert(stable->BeginStable(c)==1);assert(stable->BeginStable(c)==1);assert(stable->BeginStable(c)==3);c.view.view[0][0]+=1;assert(stable->BeginStable(c)==2);assert(stable->BeginStable(c)==2);assert(stable->BeginStable(c)==3);}stable->Stop();
 // New indoor preference is applied once AFTER either real worker output or
 // serial fallback; driving and unqualified inputs preserve inherited math.
 state.occupiedCar=0;state.view={};state.view.valid=true;
 for(unsigned i=0;i<4;++i)state.view.view[i][i]=1;
 state.view.projection[0][0]=state.view.projection[1][1]=state.view.projection[2][2]=state.view.projection[2][3]=1;
 Light61 room211{};room211.mPosition={0,0,4};room211.mColor={1,.2f,0};room211.mRadius=3;room211.mIntensity=20;room211.mInteriorIndex=19719;room211.mRoomIndex=1;room211.mFlags=0x6f;
 parallelRequested61=false;interiorPriority211=false;
 auto baseline211=SceneCall61(room211,2,711,9,3,true,true);
 interiorPriority211=true;auto indoor211=SceneCall61(room211,2,711,9,3,true,true);assert(indoor211.score<baseline211.score);
 EqualConsequence(baseline211,SceneCall61(room211,2,711,9,3,true,false));
 room211.mInteriorIndex=0;EqualConsequence(baseline211,SceneCall61(room211,2,711,9,3,true,true));room211.mInteriorIndex=19719;
 auto roomWorker211=std::make_unique<m::Service>();assert(roomWorker211->Start(1));parallelService61=roomWorker211.get();parallelRequested61=true;
 {const fusionfix::shadows::FloatingPointState fp;auto c=ParallelContext61();assert(roomWorker211->Begin(c));
  (void)ViewCall61(room211,2,711,room211.mFlags);EqualConsequence(indoor211,SceneCall61(room211,2,711,9,3,true,true));
  assert(roomWorker211->Dispatch());WaitReady61(*roomWorker211,c,ViewInput61(room211,2,711,room211.mFlags));assert(roomWorker211->Begin(c));
  const auto hits=roomWorker211->completedHits.load();EqualConsequence(indoor211,SceneCall61(room211,2,711,9,3,true,true));assert(roomWorker211->completedHits==hits+1);
 }roomWorker211->Stop();parallelService61=nullptr;CloseHandle(gate61);
 std::printf("PASS actual Collect callsites, original worker invariants and indoor qualification/serial/ready-worker parity. No timing/FPS or live integration claimed.\n");
 return 0;
}

