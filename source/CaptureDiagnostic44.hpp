#pragma once
#include "CaptureLease44.hpp"
#include "core/ShadowTrace34.hpp"
#include <windows.h>
#include <atomic>
#include <filesystem>
#include <string>
namespace CaptureDiagnostic44 {
inline std::atomic<bool> sampleSources{false};
inline HANDLE activeEvent{},heartbeatEvent{};
inline Lease lease;
inline std::filesystem::path directory;
inline uint32_t lastPoll{},lastOpen{},lastStart{},lastFlush{};
inline bool tracing{};
inline std::wstring Name(const wchar_t* kind,uint32_t pid){return std::wstring(L"Local\\LibertyShadows.Capture44.")+std::to_wstring(pid)+L"."+kind;}
inline void CloseEvents() noexcept {
 if(activeEvent)CloseHandle(activeEvent);if(heartbeatEvent)CloseHandle(heartbeatEvent);
 activeEvent=heartbeatEvent=nullptr;lease={};
}
// Optional recorder only. One nonblocking event poll per250ms, event discovery
// at most once/sec; no process writes, new hooks or permission queries.
// Called by the frame prelude even during user/script pause. If the native frame
// boundary stops entirely (suspend/hang/crash), the collector retains pending
// ownership and never attaches a late-closed file to a subsequent clip.
inline int Poll() noexcept {
 const auto now=GetTickCount();if(uint32_t(now-lastPoll)<250)return 0;lastPoll=now;
 try {
  if(!activeEvent||!heartbeatEvent){
   if(uint32_t(now-lastOpen)<1000)return 0;lastOpen=now;CloseEvents();
   activeEvent=OpenEventW(SYNCHRONIZE,FALSE,Name(L"active",GetCurrentProcessId()).c_str());
   heartbeatEvent=OpenEventW(SYNCHRONIZE,FALSE,Name(L"heartbeat",GetCurrentProcessId()).c_str());
   if(!activeEvent||!heartbeatEvent){CloseEvents();return 0;}
  }
  const bool beat=WaitForSingleObject(heartbeatEvent,0)==WAIT_OBJECT_0;
  const bool requested=WaitForSingleObject(activeEvent,0)==WAIT_OBJECT_0;
  const bool alive=lease.Update(now,beat,requested);
  if(alive&&!tracing&&uint32_t(now-lastStart)>=1000){
   lastStart=now;
   const auto path=directory/("LibertyShadows.capture44."+std::to_string(GetCurrentProcessId())+"."+std::to_string(GetTickCount64())+".st34");
   tracing=ShadowTrace34::Start(path,44,GetCurrentProcessId());sampleSources.store(tracing,std::memory_order_release);
   return tracing?1:-1;
  }
  if(!alive&&tracing){
   sampleSources.store(false,std::memory_order_release);
   if(ShadowTrace34::Stop()){tracing=false;if(!requested||uint32_t(now-lease.lastHeartbeat)>3000)CloseEvents();return 2;}
  }
  if(tracing&&uint32_t(now-lastFlush)>=1000){lastFlush=now;ShadowTrace34::Flush();}
 }catch(...){sampleSources=false;if(ShadowTrace34::Stop())tracing=false;return -1;}
 return 0;
}
}
