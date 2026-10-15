#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <float.h>
#include <array>
#include <atomic>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include "ShadowFloatingPointState.hpp"
#include "FreshPageReadable.hpp"
#include "ImmediateRegion33.hpp"
namespace CShadows{uint32_t frame=17;uint32_t*pFrameCounter=&frame;}
unsigned reads{};
namespace Liberty20{bool Copy(uintptr_t at,void*out,size_t n)noexcept{++reads;SetLastError(999);_mm_setcsr(0x7f80);if(at!=uintptr_t(CShadows::pFrameCounter)||n!=4)return false;memcpy(out,CShadows::pFrameCounter,4);return true;}}
void Event(int,const char*,const std::string&){}
#include "FrameBoundary154Runtime.inc"
int main(){using namespace frame_boundary154;
 SetLastError(73);_mm_setcsr(0x3f80);assert(FrameBoundary154::Start()==0);FrameBoundary154::Sample(10,true);assert(reads==0&&GetLastError()==73&&_mm_getcsr()==0x3f80);
 FrameBoundary154::enabled=true;auto start=FrameBoundary154::Start();assert(start&&GetLastError()==73&&_mm_getcsr()==0x3f80);FrameBoundary154::Sample(start,true);assert(reads==1&&GetLastError()==73&&_mm_getcsr()==0x3f80);
 Record row[8]{};assert(FrameBoundary154::state.Drain(row,8)==1&&row[0].nativeFrame==17&&(row[0].flags&First));FrameBoundary154::Sample(start+20,false);FrameBoundary154::Sample(start+30,true);assert(FrameBoundary154::state.Drain(row,8)==1&&(row[0].flags&PauseGap)&&!row[0].interval);
 State<2>s;s.Sample(7,17,100);s.Sample(7,18,120);s.Sample(7,19,130);assert(s.drops==1&&s.Drain(row,8)==2&&row[1].interval==20&&row[1].nativeStep==1);s.Sample(8,20,140);assert(s.threadRefusals==1);s.Pause(8);assert(s.threadRefusals==2);s.Sample(7,20,125);assert(s.Drain(row,8)==1&&(row[0].flags&ClockDiscontinuity));s.Pause(7);s.Sample(7,UINT32_MAX,150);assert(s.Drain(row,8)==1&&(row[0].flags&PauseGap)&&(row[0].flags&NativeFrameUnavailable));s.Sample(7,1,0);assert(s.clockRefusals==1);
 LARGE_INTEGER f{};assert(QueryPerformanceFrequency(&f));FrameBoundary154::frequency=uint64_t(f.QuadPart);FrameBoundary154::outputPath=std::filesystem::current_path()/"boundary154.csv";FrameBoundary154::lastPublish=0;FrameBoundary154::Sample(start+40,true);FrameBoundary154::Publish();assert(FrameBoundary154::written==1&&std::filesystem::exists(FrameBoundary154::outputPath));std::ifstream file(FrameBoundary154::outputPath);std::string header;getline(file,header);assert(header.find("engine_loop_interval_ms")!=std::string::npos&&header.find("present")==std::string::npos);
 FrameBoundary154::written=65536;FrameBoundary154::Publish();assert(!FrameBoundary154::enabled&&FrameBoundary154::written==65536);
 std::cout<<"PASS154 actual production Start/Sample/Publish disabledzero reads; FP/LastError preserved; frame step/thread/pause/backward/overflow/cap and real CSV\n";
}
