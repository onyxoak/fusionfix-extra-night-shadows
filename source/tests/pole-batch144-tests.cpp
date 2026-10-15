#include <cassert>
#include <map>
#include <vector>
#include <iostream>
#include "core/PoleSnapshotBatch144.hpp"
#define pole_caster134 pole_reference143
#include "pole-snapshot143-reference.hpp"
#undef pole_caster134
namespace {
constexpr uint32_t fixturegame=0x400000,fixtureentity=0x11000000,fixturepool=0x12000000,fixtureflags=0x13000000,fixtureinfo=0x14000000,fixtureassoc=0x15000000;
struct Fixture {
 std::map<uint64_t,std::vector<unsigned char>> memory;unsigned calls=0,mutations=0;uint64_t fault=0,mutate=0;bool wideFault=false;unsigned frame=10;
 template<class T> void Put(uint64_t at,T value){for(auto& [base,bytes]:memory)if(at>=base&&at+sizeof(T)<=base+bytes.size()){std::memcpy(bytes.data()+at-base,&value,sizeof(T));return;}assert(false);}
 void Reset(){memory.clear();calls=mutations=0;fault=mutate=0;wideFault=false;frame=10;
 memory[fixturegame+0x1232c60]=std::vector<unsigned char>(4);Put(fixturegame+0x1232c60,fixturepool);memory[fixturepool]=std::vector<unsigned char>(24);Put(fixturepool,fixtureentity);Put(fixturepool+4,fixtureflags);Put(fixturepool+8,int32_t(4));Put(fixturepool+12,int32_t(0x320));memory[fixtureflags]=std::vector<unsigned char>(4);Put(fixtureflags,uint8_t(7));
 memory[fixtureentity]=std::vector<unsigned char>(0x320);Put(fixtureentity,uint32_t(1));Put(fixtureentity+0x28,uint32_t(4<<6));Put(fixtureentity+0x2e,int16_t(1905));Put(fixtureentity+0x210,uint32_t(3));Put(fixtureentity+0x1bc,fixtureassoc);Put(fixtureentity+0x280,uint32_t(5));
 memory[fixturegame+0xe95cd8+1905*4]=std::vector<unsigned char>(4);Put(fixturegame+0xe95cd8+1905*4,fixtureinfo);memory[fixtureinfo]=std::vector<unsigned char>(0x40);Put(fixtureinfo+0x3c,pole_caster134::PoleHash);Put(fixtureinfo+8,uint32_t(7));memory[fixtureassoc]=std::vector<unsigned char>(0x2c);Put(fixtureassoc+0x28,uint32_t(9));}
 bool Read(uint64_t at,void* out,size_t n){++calls;if(wideFault&&(n==0x284||n==0x40))return false;if(fault>=at&&fault<at+n)return false;
 for(auto& [base,bytes]:memory)if(at>=base&&at+n<=base+bytes.size()){std::memcpy(out,bytes.data()+at-base,n);if(mutate>=at&&mutate<at+n&&++mutations==2)reinterpret_cast<unsigned char*>(out)[mutate-at]^=1;return true;}return false;}
};
}
int main(){Fixture f;const auto read=[&](uint64_t at,void* out,size_t n){return f.Read(at,out,n);};const auto frame=[&](){return f.frame;};
 pole_caster134::Snapshot next{};pole_reference143::Snapshot old{};
 f.Reset();assert(pole_reference143::Capture(read,frame,fixturegame,fixtureentity,10,old));const auto oldCalls=f.calls;
 f.Reset();assert(pole_batch144::Capture(read,frame,fixturegame,fixtureentity,10,next));const auto newCalls=f.calls;static_assert(sizeof(old)==sizeof(next));assert(!std::memcmp(&old,&next,sizeof(old)));assert(newCalls+8==oldCalls);
 for(auto at:{uint64_t(fixturegame)+0x1232c60,fixturepool+0ull,fixtureflags+0ull,fixtureentity+0ull,fixtureentity+0x24ull,fixtureentity+0x210ull,fixtureentity+0x1bcull,fixtureentity+0x280ull,fixtureinfo+8ull,fixtureinfo+0x3cull,fixtureassoc+0x28ull}) {
 f.Reset();f.fault=at;const bool a=pole_reference143::Capture(read,frame,fixturegame,fixtureentity,10,old);f.Reset();f.fault=at;const bool b=pole_batch144::Capture(read,frame,fixturegame,fixtureentity,10,next);assert(a==b&&!b&&next.entity==0);
 f.Reset();f.mutate=at;const bool c=pole_reference143::Capture(read,frame,fixturegame,fixtureentity,10,old);f.Reset();f.mutate=at;const bool d=pole_batch144::Capture(read,frame,fixturegame,fixtureentity,10,next);assert(c==d&&!d&&next.entity==0);
 }
 // Unused newly copied bytes are neither compared nor added as authority.
 f.Reset();f.mutate=fixtureentity+0x100;assert(pole_batch144::Capture(read,frame,fixturegame,fixtureentity,10,next));
 f.Reset();f.mutate=fixtureinfo+0x20;assert(pole_batch144::Capture(read,frame,fixturegame,fixtureentity,10,next));
 f.Reset();f.wideFault=true;assert(pole_batch144::Capture(read,frame,fixturegame,fixtureentity,10,next));
 for(unsigned i=0;i<6;++i){f.Reset();if(i==0)f.frame=11;else if(i==1)f.Put(fixtureflags,uint8_t(0x80));else if(i==2)f.Put(fixtureentity+0x28,uint32_t(3<<6));else if(i==3)f.Put(fixtureentity+0x2e,int16_t(-1));else if(i==4)f.Put(fixtureinfo+0x3c,uint32_t(123));else f.Put(fixturepool+12,int32_t(0x300));assert(!pole_batch144::Capture(read,frame,fixturegame,fixtureentity,10,next)&&next.entity==0);}
 std::cout<<"Original actual Capture calls="<<oldCalls<<" batched="<<newCalls<<" all authority/refusal/mutation tests pass\n";
}


