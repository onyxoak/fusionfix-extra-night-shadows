#define NOMINMAX
#include <Windows.h>
#include "../CaptureDiagnostic44.hpp"
#include "../CaptureFramePrelude46.hpp"
#include <cstdio>
#include <cstdlib>
#include <fstream>
static unsigned checks{};
#define CHECK(x) do{++checks;if(!(x)){std::printf("FAIL line %u: %s\n",__LINE__,#x);std::exit(1);}}while(0)
int main(){
 using namespace CaptureDiagnostic44;
 directory=std::filesystem::temp_directory_path()/("LibertyCapture46-test-"+std::to_string(GetCurrentProcessId()));
 std::filesystem::create_directories(directory);
 const auto active=CreateEventW(nullptr,TRUE,FALSE,Name(L"active",GetCurrentProcessId()).c_str());
 const auto heartbeat=CreateEventW(nullptr,FALSE,FALSE,Name(L"heartbeat",GetCurrentProcessId()).c_str());
 CHECK(active&&heartbeat);uint8_t paused=1,unpaused=0;unsigned captureCalls=0,gameCalls=0;int transition=0;
 auto frame=[&](bool failed,bool initialized,const uint8_t* user,const uint8_t* script){
  // Force only the existing throttle's eligible observation; never sleep/change a game.
  lastPoll=GetTickCount()-251;lastOpen=GetTickCount()-1001;
  if(capture_frame46::ContinueGameplay(failed,initialized,user,script,[&]{++captureCalls;transition=Poll();}))++gameCalls;
 };
 CHECK(SetEvent(active));CHECK(SetEvent(heartbeat));lastStart=GetTickCount()-1001;
 frame(false,true,&paused,&unpaused);CHECK(captureCalls==1);CHECK(gameCalls==0);CHECK(transition==1);CHECK(tracing);CHECK(sampleSources.load());
 CHECK(ShadowTrace34::enabled.load());const auto first=ShadowTrace34::basePath;
 // F9 during pause: exact production prelude runs Stop, does not run gameplay.
 CHECK(ResetEvent(active));CHECK(SetEvent(heartbeat));frame(false,true,&paused,&unpaused);
 CHECK(transition==2);CHECK(!tracing);CHECK(!sampleSources.load());CHECK(!ShadowTrace34::enabled.load());CHECK(gameCalls==0);
 std::ifstream status(first.string()+".status");std::string text((std::istreambuf_iterator<char>(status)),{});CHECK(text.find("enabled=0")!=std::string::npos);
 // A script pause has the same lifecycle behavior. Expired collector lease closes it.
 Sleep(2);CHECK(SetEvent(active));CHECK(SetEvent(heartbeat));lastStart=GetTickCount()-1001;
 frame(false,true,&unpaused,&paused);CHECK(transition==1);CHECK(tracing);CHECK(gameCalls==0);
 lease.lastHeartbeat=GetTickCount()-3001;frame(false,true,&unpaused,&paused);
 CHECK(transition==2);CHECK(!tracing);CHECK(!sampleSources.load());CHECK(gameCalls==0);
 const auto prior=captureCalls;frame(true,true,&unpaused,&unpaused);CHECK(captureCalls==prior);CHECK(gameCalls==0);
 frame(false,false,&paused,&unpaused);CHECK(captureCalls==prior);CHECK(gameCalls==0);
 frame(false,true,nullptr,&unpaused);CHECK(captureCalls==prior+1);CHECK(gameCalls==0);
 ResetEvent(active);frame(false,true,&unpaused,&unpaused);CHECK(gameCalls==1);
 // Optional capture exceptions do not unpause a game or escape a noexcept callback.
 CHECK(!capture_frame46::ContinueGameplay(false,true,&paused,&unpaused,[]{throw 1;}));
 CloseEvents();CloseHandle(active);CloseHandle(heartbeat);
 std::printf("PASS %u assertions: actual recorder F9/lease Stop before both pause guards, no paused gameplay\n",checks);
}
