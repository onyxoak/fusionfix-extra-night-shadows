#include "FreshReadBatch30.hpp"
#include "ImmediateRegion33.hpp"
#include "core/PoleSnapshotBatch144.hpp"
#include <cassert>
#include <set>
#include <cstdio>
namespace fp=traffic_signal::fresh_page;
unsigned queries147b=0;std::set<uintptr_t> touched147b;
BOOL WINAPI Query147b(HANDLE process,PVOID out,DWORD n){++queries147b;return QueryWorkingSetEx(process,out,n);}
bool Region147b(uintptr_t at,size_t n)noexcept{MEMORY_BASIC_INFORMATION m{};return VirtualQuery(reinterpret_cast<void*>(at),&m,sizeof(m))&&immediate_region33::Contains(m,at,n)&&immediate_region33::Allowed(m);}
template<class T> void Put147b(uintptr_t at,T value){std::memcpy(reinterpret_cast<void*>(at),&value,sizeof(value));}
int main(){
 auto* bytes=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x1240000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(bytes);const auto game=uint32_t(uintptr_t(bytes));
 const uint32_t entity=game+0x1000,pool=game+0x3000,flags=game+0x5000,info=game+0x7000,associated=game+0x9000,pose=game+0xb000,predicate=game+0xd000,frameAt=game+0xf000;
 for(unsigned at:{0x1000u,0x3000u,0x5000u,0x7000u,0x9000u,0xb000u,0xd000u,0xf000u,0x1232000u,0xe97000u,0xd9f000u,0x1193000u})bytes[at]=0;
 Put147b(game+0x1232c60,pool);Put147b(pool,entity);Put147b(pool+4,flags);Put147b(pool+8,int32_t(1));Put147b(pool+12,int32_t(0x320));Put147b(flags,uint8_t(7));
 Put147b(entity,uint32_t(123));Put147b(entity+0x28,uint32_t(4<<6));Put147b(entity+0x2e,int16_t(1905));Put147b(entity+0x1bc,associated);Put147b(associated+0x28,uint32_t(1));Put147b(game+0xe95cd8+1905*4,info);Put147b(info+0x3c,pole_caster134::PoleHash);Put147b(info+8,uint32_t(444));Put147b(frameAt,uint32_t(10));
 const auto read=[&](uint64_t at,void* out,size_t n)noexcept {touched147b.insert(uintptr_t(at)/4096);traffic_signal::fresh_batch30::Span span{uintptr_t(at),out,n};return traffic_signal::fresh_batch30::Copy(&span,1,Region147b,Query147b);};
 const auto frame=[&]()noexcept {uint32_t v{};return read(frameAt,&v,4)?v:UINT32_MAX;};
 auto run=[&](unsigned capacity){queries147b=0;touched147b.clear();pole_caster134::Snapshot a{},b{};std::array<unsigned char,64> scratch{};
 {fp::Operation44 scope(capacity);
 // Same synchronous Reclassify boundaries around the actual production
 // Snapshot calls: frame/map/deferred/pose/predicate and final fresh rereads.
 assert(read(game+0xd9f1ed+0x110,scratch.data(),19));assert(read(game+0x1193312,scratch.data(),1));assert(read(pose,scratch.data(),64));assert(read(predicate,scratch.data(),3));
 assert(pole_batch144::Capture(read,frame,game,entity,10,a));assert(pole_batch144::Capture(read,frame,game,entity,10,b));assert(!std::memcmp(&a,&b,sizeof(a)));
 assert(read(predicate,scratch.data(),3));assert(read(game+0xd9f1ed+0x110,scratch.data(),19));assert(read(pose,scratch.data(),64));assert(frame()==10);}
 return queries147b;};
 const auto old=run(8),fresh=run(32);assert(touched147b.size()>8&&fresh<old);assert(fp::operation44==nullptr);
 std::printf("Production poleSnapshot plus literal synchronous map/frame/pose/predicate read boundaries: uniquePages=%zu capacity8queries=%u capacity32queries=%u; exactSnapshot outputequal. Not fullrenderer/FPS proof.\n",touched147b.size(),old,fresh);
 VirtualFree(bytes,0,MEM_RELEASE);
}

