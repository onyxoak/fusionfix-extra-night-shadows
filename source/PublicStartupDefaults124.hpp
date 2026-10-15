#pragma once
#include "PresetIni117.hpp"
namespace preset117 {
// Startup-only file authority. An absent complete profile gets the public
// default; existing legacy/invalid profiles never acquire schema3 authority.
inline ReadResult ReadStartup124(const wchar_t* file) noexcept {
 const DWORD savedError=GetLastError();
 struct ErrorScope {DWORD value;~ErrorScope(){SetLastError(value);}} error{savedError};
 auto refused=[]() noexcept {ReadResult result{};result.unavailable=true;result.invalid=1;result.state.enabled=false;return result;};
 if(!file||!*file)return refused();
 // Hold a read-only startup lease through parsing. No write/delete sharing:
 // the existing file cannot be replaced by an absent/default profile while
 // the profile APIs are reading it. This handle ends before any menu save.
 const HANDLE handle=CreateFileW(file,GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
 if(handle==INVALID_HANDLE_VALUE){
  const DWORD reason=GetLastError();
  // A missing parent/root is not a missing file beside the loaded ASI. It can
  // also be an invalid/unavailable path; only file-not-found authorizes High.
  if(reason!=ERROR_FILE_NOT_FOUND)return refused();
  ReadResult result{};result.state=*Defaults(Preset::High);result.present=true;result.ownBackend=true;return result;
 }
 struct FileScope {HANDLE value;~FileScope(){CloseHandle(value);}} lease{handle};
 BY_HANDLE_FILE_INFORMATION information{};
 if(GetFileType(handle)!=FILE_TYPE_DISK||!GetFileInformationByHandle(handle,&information)||
    (information.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))return refused();
 try{return Read(file);}catch(...){return refused();}
}
inline bool ReadTransitionDiagnostics124(const wchar_t* file) noexcept {
 return GetPrivateProfileIntW(L"DIAGNOSTICS",L"ShadowTransitions",0,file)==1;
}
}
