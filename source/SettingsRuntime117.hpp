#pragma once
#include "PresetIni117.hpp"
#include "PublicStartupDefaults124.hpp"
#include "LiveReach122.hpp"
namespace preset117 {
extern ReadResult startup;
extern State applied;
extern bool backendLimited;
extern bool vehicleDepthUnavailable119,backendReadFailed119;
extern void (*refreshApplied119Hook)() noexcept;
inline void RefreshApplied119() noexcept {if(refreshApplied119Hook)refreshApplied119Hook();}
extern HMODULE module;
inline void (*menuEvent233Hook)(const char*,const char*) noexcept=nullptr;
inline HMODULE (*backendOwner233Hook)() noexcept=nullptr;
inline void MenuEvent233(const char* code,const char* fields) noexcept {if(menuEvent233Hook)menuEvent233Hook(code,fields);}
inline bool UseOwnedBackend117(bool nativeBackend) noexcept {return nativeBackend||startup.ownBackend;}
void EngineActivated117(uintptr_t game,const uint8_t* original,size_t bytes) noexcept;
extern std::atomic<bool> menuPending117;
void RetryMenu117(uintptr_t game,const uint8_t* original,size_t bytes) noexcept;
DWORD FrontendOnly117(HMODULE addon) noexcept;
}
