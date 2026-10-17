#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cstring>
#include <iostream>
#include "core/VehicleBoxFallback216.hpp"
#include "core/ShadowFloatingPointState.hpp"
namespace rage{struct VehiclePool{void* m_aStorage{};void* m_aFlags{};int m_nSize{},m_nStorageSize{};};}
namespace ModelEffectObserver{struct LastErrorScope{DWORD previous=GetLastError();~LastErrorScope(){SetLastError(previous);}};}
std::atomic<bool> addonActive{true};
namespace CShadows{uint32_t frame=80;uint32_t* pFrameCounter=&frame;}
namespace VehicleBoxes216{
std::atomic<bool> active{true};uint32_t owner=GetCurrentThreadId();
unsigned pools{},copies{},frameReads{};std::atomic<uint32_t> excluded{0};uint8_t liveFlag=3;
vehicle_box216::Settings settings;vehicle_box216::State state;
const vehicle_box216::Car fixtureCar224{{0,0,1},2,1,1,0,1,0x100640,2,3};
bool Pool(rage::VehiclePool& p)noexcept{++pools;p={reinterpret_cast<void*>(0x100000),reinterpret_cast<void*>(0x200000),16,0x320};return true;}
template<class T>bool Copy(uintptr_t at,T& out)noexcept{
 ++copies;
 if(at==uintptr_t(CShadows::pFrameCounter)&&sizeof(T)==4){++frameReads;std::memcpy(&out,CShadows::pFrameCounter,4);return true;}
 if(at==0x200002&&sizeof(T)==1){std::memcpy(&out,&liveFlag,1);return true;}
 if(at==fixtureCar224.owner+0xf50&&sizeof(T)==36){std::array<uint32_t,9> occupants{0x300000};std::memcpy(&out,occupants.data(),36);return true;}
 return false;
}
#include "fallback-validate224.inc"
#include "fallback-exclude224.inc"
}
int main(){
 using namespace VehicleBoxes216;
 // Actual production negative identity gate must avoid all pool/memory work.
 for(unsigned i=0;i<10000;++i){assert(!Validate(fixtureCar224,0x400000+i*0x320,2));assert(!Validate(fixtureCar224,fixtureCar224.owner,1));}
 assert(pools==0&&copies==0);
 assert(Validate(fixtureCar224,fixtureCar224.owner,2));assert(pools==1);
 liveFlag=4;assert(!Validate(fixtureCar224,fixtureCar224.owner,2));liveFlag=3;
 assert(Validate(fixtureCar224,0x300000,3));assert(!Validate(fixtureCar224,0x300004,3));
 settings.perCar=0;
 const auto start=GetTickCount()-1600u;
 vehicle_box216::Light lamp{{0,0,8},20,10,9,4,0,3,1};
 for(unsigned frame=1;frame<=100;++frame){state.Begin(frame,start+frame*16,0);state.Select(lamp,&fixtureCar224,1,{},{},false,settings,Validate);}
 CShadows::frame=100;
 assert(state.MayContainOwner224(fixtureCar224.owner));
 uint32_t absent=0x70000000;while(state.MayContainOwner224(absent))absent+=0x320;
 pools=copies=frameReads=0;SetLastError(224);
 assert(!Exclude(9,absent,2));assert(GetLastError()==224);
 assert(pools==0&&copies==0&&frameReads==0);
 assert(Exclude(9,fixtureCar224.owner,2));assert(pools>0&&frameReads==1);
 liveFlag=4;assert(!Exclude(9,fixtureCar224.owner,2));liveFlag=3;
 assert(Exclude(9,0x300000,3));
 state.Begin(1,GetTickCount()+5000,0);assert(!state.MayContainOwner224(fixtureCar224.owner));
 std::cout<<"PASS actual fallback Validate/Exclude: 20,000 negative calls avoid pool reads; absence filter avoids frame query; matching owner and occupant keep fresh generation checks. Not game FPS proof.\n";
}
