#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "core/ShadowTrace34.hpp"
#include <cassert>
int main(){
 const auto dir=std::filesystem::path("capture-protocol44-fixture");std::filesystem::create_directories(dir);
 const auto a=dir/"session-a.st34",b=dir/"session-b.st34";
 assert(!ShadowTrace34::enabled);assert(ShadowTrace34::Start(a,44,GetCurrentProcessId()));
 ShadowTrace34::Emit({35,100,200,42,0,0,4,-1,1,2,3,10});assert(ShadowTrace34::Stop());
 assert(!ShadowTrace34::enabled);assert(std::filesystem::file_size(a)==32+48);
 std::ifstream status(a.string()+".status");std::string line;std::getline(status,line);assert(line.find("enabled=0")!=std::string::npos);
 // Simulate a producer that completed just after stop. A new clip cannot inherit it.
 assert(ShadowTrace34::recorder.Push({35,101,201,42,0,0,4,-1,1,2,3,10}));
 assert(ShadowTrace34::Start(b,44,GetCurrentProcessId()));assert(ShadowTrace34::Stop());
 assert(std::filesystem::file_size(b)==32);
}
