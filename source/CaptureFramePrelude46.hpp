#pragma once
#include <cstdint>
namespace capture_frame46 {
// Production frame entry ordering: recorder lifecycle is independent of gameplay pause.
// An active addon implies recorder configuration completed; no recorder work before init.
template<class Capture> bool ContinueGameplay(bool failed,bool initialized,
 const uint8_t* pauseUser,const uint8_t* pauseScript,Capture&& capture) noexcept {
 if(failed)return false;
 if(initialized){try{capture();}catch(...){/* Optional diagnostics cannot escape callback. */}}
 return pauseUser&&pauseScript&&!*pauseUser&&!*pauseScript;
}
}
