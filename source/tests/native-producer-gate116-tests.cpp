#include "core/NativeProducerGate116.hpp"
#include <cassert>
#include <bit>
#include <vector>
#include <cstdio>
int main(){
 using namespace native_producer_gate116;
 std::array<std::uint8_t,BodySize> native{};
 std::memcpy(native.data()+FirstRva-BodyRva,FirstBytes.data(),6);
 std::memcpy(native.data()+SecondRva-BodyRva,SecondBytes.data(),6);
 assert(Verify(native,native));
 for(unsigned offset:{0u,FirstRva-BodyRva,FirstRva-BodyRva+5,SecondRva-BodyRva,SecondRva-BodyRva+5,BodySize-1}){
  auto altered=native;altered[offset]^=1;assert(!Verify(native,altered));
 }
 assert(!Verify(std::span(native).first(BodySize-1),native));
 Budget b;for(unsigned i=0;i<64;++i)assert(b.Claim(1000));assert(!b.Claim(1000));assert(b.Claim(1024));assert(!b.Claim(1000));
 Budget wrap;assert(wrap.Claim(0xffffff80));assert(wrap.Claim(0));
 Context c;Registers r{};r.ebp=0x2000f;r.esp=0x1ff20;r.type=2;r.flags=0x41;r.radius=50;r.source={528.4085f,195.7277f,27.3625f};r.planeDistance=-2;r.negativeRadius=-50;
 std::vector<Event> events;auto sink=[&](Event e){events.push_back(e);};
 for(unsigned cfzf:{0u,1u,0x40u,0x45u}){
  r.eflags=cfzf;assert(First(c,r,99,2000,sink));assert(events[events.size()-2].c==int(!(cfzf&1)));assert(c.pending==bool(cfzf&1));
 }
 r.eflags=1;assert(First(c,r,100,3000,sink));auto second=r;second.eax=0;second.eflags=0x40;assert(Second(c,second,3001,sink));assert(events.back().stage==91&&events.back().nonce==100&&events.back().c==1&&!c.pending);
 for(unsigned result:{1u,2u}){assert(First(c,r,101,3000,sink));second.eax=result;second.eflags=0;assert(Second(c,second,3001,sink));assert(events.back().d==int(result)&&events.back().c==0);}
 for(unsigned wrong:{0u,1u,2u,3u}){
  assert(First(c,r,102,3000,sink));auto bad=second;auto tick=3001u;
  if(wrong==0)++bad.ebp;else if(wrong==1)++bad.esp;else if(wrong==2)tick=4001;else bad.eflags=0x40;
  const auto n=events.size();assert(!Second(c,bad,tick,sink)&&events.size()==n&&!c.pending);
 }
 auto malformed=r;malformed.esp++;assert(!First(c,malformed,1,0,sink));malformed=r;malformed.type=3;assert(!First(c,malformed,1,0,sink));
 // Native branch flags retain authority even for unordered/signed-zero inputs.
 r.source[0]=std::bit_cast<float>(0x7fc01234u);r.radius=std::bit_cast<float>(0x80000000u);r.eflags=0x45;
 assert(First(c,r,103,5000,sink));assert(std::bit_cast<unsigned>(events[events.size()-2].x)==0x7fc01234u&&std::bit_cast<unsigned>(events[events.size()-2].w)==0x80000000u);
 // A new first site always invalidates an earlier diagnostic call scope.
 assert(First(c,r,104,5001,sink));r.eflags=0;assert(First(c,r,105,5002,sink));assert(!Second(c,second,5003,sink));
 std::puts("116 exact branch bytes/flags, native stack relation, budget/wrap, TLS numeric binding and immutable raw IEEE samples passed; no renderer change.");
}
