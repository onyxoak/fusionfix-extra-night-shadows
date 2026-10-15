#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <string>
#include "ShadowFloatingPointState.hpp"
#include "WorldCasterMask20.hpp"
#include "safetyhook/context.hpp"
struct SafetyHookMid{};
using SafetyHookContext=safetyhook::Context32;
uint32_t game=0;std::atomic<bool>addonActive{true};bool hotCountersEnabled=false;unsigned copies162=0,ticks162=0,events162=0;
namespace Liberty20{bool Copy(uintptr_t at,void*out,size_t n)noexcept{++copies162;if(at<game||at+n>uint64_t(game)+0x1240000)return false;std::memcpy(out,reinterpret_cast<void*>(at),n);return true;}}
DWORD Clock162()noexcept{++ticks162;return 6000;}
void Event(int,const char*,const std::string&){++events162;}
#define GetTickCount Clock162
#include "world-callbacks162-extracted.inc"
namespace WorldGeometry20{
#include "world-publish162-extracted.inc"
}
#undef GetTickCount
template<class T>void Put162(uintptr_t p,T v){std::memcpy(reinterpret_cast<void*>(p),&v,sizeof(v));}
int main(){game=uint32_t(uintptr_t(VirtualAlloc(nullptr,0x1240000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE)));assert(game);std::memset(reinterpret_cast<void*>(game),0,0x1240000);const auto object=game+0x1000;Put162(object+0x940,uint32_t(1));Put162(game+0xd9f1ed+0x110,uint8_t(1));Put162(game+0xd9f1ed+0x110+11,uint32_t(7654));Put162(game+0xd9f1ed+0x110+15,uint32_t(4));Put162(object+0x15,uint16_t(0x100));Put162(object+0x8e8,uint32_t(0x80));WorldGeometry20::active=true;WorldGeometry20::requested=true;
SafetyHookContext mask{};mask.edi=object;mask.eax=world_caster_mask20::NativeDynamicMask;const auto beforeMask=mask;WorldGeometry20::IncludeStaticMask(mask);auto expectedMask=beforeMask;expectedMask.eax=world_caster_mask20::IncludeStatic(expectedMask.eax);assert(!std::memcmp(&mask,&expectedMask,sizeof(mask))&&WorldGeometry20::staticMasks==0);
SafetyHookContext collect{};collect.esi=object;const auto beforeCollect=collect;WorldGeometry20::Collect(collect);assert(!std::memcmp(&collect,&beforeCollect,sizeof(collect))&&*reinterpret_cast<uint16_t*>(object+0x15)==0x101&&WorldGeometry20::collections==0);
SafetyHookContext draw{};draw.ebx=UINT32_MAX;const auto beforeDraw=draw;{WorldGeometry20::Scope scope(reinterpret_cast<void*>(object));assert(WorldGeometry20::dynamicScope);const auto beforeCopies=copies162;WorldGeometry20::Draw(draw);assert(copies162==beforeCopies);}auto expectedDraw=beforeDraw;expectedDraw.eip=game+0x6e426a;assert(!std::memcmp(&draw,&expectedDraw,sizeof(draw))&&WorldGeometry20::draws==0);
Put162(game+0x1193312,uint8_t(1));mask=beforeMask;WorldGeometry20::IncludeStaticMask(mask);assert(mask.eax==beforeMask.eax&&WorldGeometry20::deferredRefused==0);Put162(game+0x1193312,uint8_t(0));WorldGeometry20::Collect(collect);assert(WorldGeometry20::invalidRefused==0);
WorldGeometry20::Publish();assert(ticks162==0&&events162==0);
hotCountersEnabled=true;mask=beforeMask;WorldGeometry20::IncludeStaticMask(mask);Put162(object+0x15,uint16_t(0x100));WorldGeometry20::Collect(collect);draw=beforeDraw;{WorldGeometry20::Scope scope(reinterpret_cast<void*>(object));WorldGeometry20::Draw(draw);}assert(mask.eax==expectedMask.eax&&draw.eip==expectedDraw.eip&&*reinterpret_cast<uint16_t*>(object+0x15)==0x101&&WorldGeometry20::staticMasks==1&&WorldGeometry20::collections==1&&WorldGeometry20::draws==1&&WorldGeometry20::sampledLists==1&&WorldGeometry20::resolvedStaticMasks==1);WorldGeometry20::Publish();assert(events162==9&&ticks162==1);hotCountersEnabled=false;WorldGeometry20::Publish();assert(events162==9&&ticks162==1);assert(VirtualFree(reinterpret_cast<void*>(game),0,MEM_RELEASE));std::cout<<"PASS162 actual world callback/publish bodies: countersOFF preserves mask/EIP/nativecollection writes, skips atomics/census/tick/strings; countersON original sample cadence retained\n";}
