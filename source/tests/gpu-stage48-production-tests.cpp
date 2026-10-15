#include "GpuQueryRing39.hpp"
#include "GpuStageSchedule48.hpp"
#include "GpuResetGate39.hpp"
#include <windows.h>
#include <atomic>
#include <mutex>
#include <cassert>
#include <cstring>
#include <vector>
#include <cstdio>
using IDirect3DDevice9=int;
namespace Fixture {
struct Backend {
 using Device=IDirect3DDevice9*;using Query=int*;
 static inline unsigned issues=0,reads=0,allocations=0,releases=0;
 static inline bool fail=false,pending=false;
 bool Supports(Device,int){return true;}
 bool Create(Device,int type,Query& q){q=new int(type);++allocations;return true;}
 void Release(Query& q){if(q){delete q;q=nullptr;++releases;}}
 bool Issue(Query,unsigned flags){assert(flags==1||flags==2);++issues;return !fail;}
 int Get(Query q,void* out,unsigned n,unsigned flags){assert(flags==0);++reads;if(pending)return 1;std::memset(out,0,n);if(*q==12)*static_cast<uint64_t*>(out)=1000;return 0;}
};
bool shadowStages=false;std::atomic<bool> enabled{false},stopRequested{false},unavailable{false};
std::atomic<uint32_t> ownerThread{0},reason{0};
std::atomic<uint64_t> stageRefused{0},contention{0},stageObserved{0},stageNested{0},stageBudgetSkipped{0};
gpu48::StageSchedule stageSchedule;gpu39::ResetGate resetGate;std::mutex measurementMutex;
gpu39::Ring<Backend> ring;bool insideScene=false;uint32_t generation=1;uint64_t interval=16;
IDirect3DDevice9* boundFacade=reinterpret_cast<IDirect3DDevice9*>(0x1000);
uintptr_t game=0x400000;unsigned copies=0;bool fixtureCurrent=true;uint32_t proxy=0x2000,fixtureKind=4,fixtureSlot=3,fixtureFrame=777;
namespace CShadows {inline void* pFrameCounter=reinterpret_cast<void*>(0x3000);}
namespace Liberty20 {
bool Copy(uintptr_t address,void* out,size_t n){assert(n==4);++copies;uint32_t value=0;
 if(address==0x1000+0x11ac)value=proxy;
 else if(address==0x5000+0x940)value=fixtureSlot;
 else if(address==game+0xd9f1fc+fixtureSlot*0x110)value=fixtureKind;
 else if(address==0x3000)value=fixtureFrame;
 else return false;
 std::memcpy(out,&value,4);return true;
}}
inline unsigned preservedScopes=0;
struct StateScope {~StateScope(){++preservedScopes;}};
bool Current(IDirect3DDevice9* d){return fixtureCurrent&&d==boundFacade;}
void Stats(){}
#include "gpu-stage48-extracted.inc"
void Start(uint64_t i=16){ring.Stop();stageSchedule.Reset();shadowStages=true;enabled=true;stopRequested=false;unavailable=false;fixtureCurrent=true;proxy=0x2000;fixtureKind=4;fixtureSlot=3;generation=1;interval=i;ownerThread=GetCurrentThreadId();insideScene=true;assert(ring.Bind(reinterpret_cast<IDirect3DDevice9*>(uintptr_t(proxy))));stageSchedule.BeginScene(i,generation);}
void RenderOnce(unsigned& originals){StageScope span(reinterpret_cast<void*>(0x5000));++originals;}
}
int main(){using namespace Fixture;unsigned originals=0;
 RenderOnce(originals);assert(originals==1&&copies==0&&Backend::issues==0&&Backend::allocations==0);
 Start();auto before=Backend::issues;{StageScope span(reinterpret_cast<void*>(0x5000));assert(span.query>=0);assert(measurementMutex.try_lock());measurementMutex.unlock();++originals;
  {StageScope nested(reinterpret_cast<void*>(0x5000));assert(nested.query==-1);++originals;}assert(Backend::issues==before+3);}
 assert(Backend::issues==before+5);int emitted=0;ring.Poll(19,[&](auto r){++emitted;assert(r.tag.stageKind==4&&r.tag.mapSlot==3&&r.tag.callbackOrdinal==1&&r.tag.nativeFrame==777&&r.tag.deviceGeneration==1);});assert(emitted==1);
 Start();before=Backend::issues;for(int i=0;i<9;++i)RenderOnce(originals);assert(Backend::issues==before+35);assert(stageSchedule.budgetSkipped>=2);
 Start(17);before=Backend::issues;RenderOnce(originals);assert(Backend::issues==before);
 Start();before=Backend::issues;ownerThread=GetCurrentThreadId()+1;RenderOnce(originals);assert(Backend::issues==before);ownerThread=GetCurrentThreadId();insideScene=false;RenderOnce(originals);assert(Backend::issues==before);
 Start();before=Backend::issues;resetGate.Begin();RenderOnce(originals);assert(Backend::issues==before);resetGate.End();
 Start();before=Backend::issues;{StageScope span(reinterpret_cast<void*>(0x5000));assert(span.query>=0);ring.Stop();stageSchedule.Reset();++generation;insideScene=false;}assert(Backend::issues==before+3);assert(!stopRequested);
 Start();before=Backend::issues;{StageScope span(reinterpret_cast<void*>(0x5000));proxy=0x6000;}assert(Backend::issues==before+3&&stopRequested);
 Start();before=Backend::issues;{StageScope span(reinterpret_cast<void*>(0x5000));enabled=false;}assert(Backend::issues==before+3&&stopRequested);
 Start();Backend::fail=true;auto calls=originals;RenderOnce(originals);assert(originals==calls+1&&!ring.Bound()&&stopRequested);Backend::fail=false;
 Start();Backend::pending=true;before=Backend::issues;
 for(unsigned scene=0;scene<3;++scene){interval=16+scene*16;stageSchedule.BeginScene(interval,generation);for(int i=0;i<7;++i)RenderOnce(originals);ring.Poll(interval,[](auto){assert(false);});}
 assert(ring.stats.full>0&&Backend::issues==before+16*5);Backend::pending=false;
 Start();fixtureKind=99;{StageScope span(reinterpret_cast<void*>(0x5000));assert(span.query>=0);}emitted=0;ring.Poll(19,[&](auto r){++emitted;assert(r.tag.stageKind==0);});assert(emitted==1);
 ring.Stop();assert(Backend::allocations==Backend::releases);
 std::puts("gpu-stage48 extracted production: default-off, original calls, locks, nested budget, tags, thread, reset, device epoch, disable, failure, full/pending checked");
}
