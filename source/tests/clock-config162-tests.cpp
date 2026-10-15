#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <memory>
#include "ShadowFloatingPointState.hpp"
#include "FreshPageReadable.hpp"
#include "ImmediateRegion33.hpp"
#include "standalone-support-log.hpp"
namespace CShadows{uint32_t frame=19;uint32_t*pFrameCounter=&frame;}
unsigned reads162{};
namespace Liberty20{bool Copy(uintptr_t p,void*o,size_t n)noexcept{++reads162;if(p!=uintptr_t(CShadows::pFrameCounter)||n!=4)return false;std::memcpy(o,CShadows::pFrameCounter,4);return true;}}
std::unique_ptr<onyxoak::SupportLog> log162;
void Event(int level,const char*code,const std::string&fields){if(log162)log162->Event(unsigned(level),code,fields);}
#include "FrameBoundary154Runtime.inc"
#include "clock-config162-extracted.inc"
int main(){const auto ini=std::filesystem::current_path()/"clock162.ini",csv=std::filesystem::current_path()/"quiet162.csv",logs=std::filesystem::current_path()/"quiet162.log";
assert(WritePrivateProfileStringW(L"SUPPORT",L"Logging",L"0",ini.c_str()));log162=std::make_unique<onyxoak::SupportLog>(logs,0);
ConfigureClock162(ini,GetPrivateProfileIntW(L"SUPPORT",L"Logging",0,ini.c_str()));assert(!FrameBoundary154::requested);FrameBoundary154::Prepare();assert(!FrameBoundary154::enabled&&FrameBoundary154::Start()==0&&reads162==0&&!std::filesystem::exists(logs));
assert(WritePrivateProfileStringW(L"SUPPORT",L"FrameBoundaryProfiling154",L"1",ini.c_str()));ConfigureClock162(ini,0);assert(FrameBoundary154::requested);SetLastError(162);const auto fp=_mm_getcsr();FrameBoundary154::Prepare();assert(FrameBoundary154::enabled&&FrameBoundary154::frequency>0&&GetLastError()==162&&_mm_getcsr()==fp&&!std::filesystem::exists(logs));FrameBoundary154::outputPath=csv;const auto start=FrameBoundary154::Start();FrameBoundary154::Sample(start,true);FrameBoundary154::Publish();assert(reads162==1&&FrameBoundary154::written==1&&std::filesystem::exists(csv)&&!std::filesystem::exists(logs));const auto io=log162->Counts150();assert(io.opens==0&&io.flushes==0&&io.sizeQueries==0);
std::ifstream stream(csv);std::string header,row;assert(std::getline(stream,header)&&std::getline(stream,row)&&header.find("engine_loop_interval_ms")!=std::string::npos&&!row.empty());
for(auto value:{L"0",L"2",L"-1",L"garbage"}){assert(WritePrivateProfileStringW(L"SUPPORT",L"FrameBoundaryProfiling154",value,ini.c_str()));FrameBoundary154::enabled=false;ConfigureClock162(ini,0);assert(!FrameBoundary154::requested);FrameBoundary154::Prepare();assert(!FrameBoundary154::enabled);}
assert(WritePrivateProfileStringW(L"SUPPORT",L"FrameBoundaryProfiling154",L"1",ini.c_str()));for(unsigned mode:{0u,1u,2u}){ConfigureClock162(ini,mode);assert(FrameBoundary154::requested);}std::cout<<"PASS162 actual extracted startup config: Logging0 flag1 starts real clock/CSV with zero SupportLog I/O; missing/0/malformed remainsOFF, preserved FP/error and cap suite retained\n";}
