#include "ShadowCrashTrace30.hpp"
#include <cassert>
#include <iostream>
static bool RaiseHandledFault() {
    __try {
        ULONG_PTR args[2]{0,0xDEAD0000};
        RaiseException(EXCEPTION_ACCESS_VIOLATION,0,2,args);
    } __except(EXCEPTION_EXECUTE_HANDLER) {return true;}
    return false;
}
int wmain(int argc,wchar_t** argv) {
    assert(argc==2);
    assert(shadow_crash_trace::Install(argv[1]));
    assert(RaiseHandledFault()); // observer must preserve normal exception delivery
    shadow_crash_trace::Remove();
    auto f=CreateFileW(argv[1],GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr);
    assert(f!=INVALID_HANDLE_VALUE);
    shadow_crash_trace::Record r{};DWORD read=0;
    assert(ReadFile(f,&r,sizeof(r),&read,nullptr)&&read==sizeof(r));
    assert(r.magic==0x30544353&&r.code==EXCEPTION_ACCESS_VIOLATION&&r.target==0xDEAD0000);
    assert(r.gameBase&&r.eip&&r.esp&&r.stackBytes>0&&r.stackBytes<=sizeof(r.stack));
    assert(GetFileSize(f,nullptr)==sizeof(r));
    CloseHandle(f);
    std::cout<<"Passive exception observer: context recorded; handled exception still delivered; bounded record verified\n";
}
