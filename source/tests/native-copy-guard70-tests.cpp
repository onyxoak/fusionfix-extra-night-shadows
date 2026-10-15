#include "NativeCopyGuard70.hpp"
#include <cassert>
#include <iostream>
#include <map>
#include <vector>
using namespace native_copy70;
struct Fixture {
 uint32_t game=0x440000,ff=0x56950000,trampoline=0x430046,relay=trampoline+14;
 std::vector<uint8_t> original=std::vector<uint8_t>(CopySize,0xcc),callback=std::vector<uint8_t>(CallbackSize,0xcc);
 std::map<uint32_t,std::vector<uint8_t>> memory;
 static void Put(std::vector<uint8_t>& b,size_t at,uint32_t n){std::memcpy(b.data()+at,&n,4);}
 static std::vector<uint8_t> Jump(uint32_t at,uint32_t to){std::vector<uint8_t>b(5);b[0]=0xe9;Put(b,1,to-at-5);return b;}
 Fixture(){
  constexpr uint8_t displaced[]{0x8b,0x54,0x24,0x04,0xf3,0x0f,0x10,0x42,0x04};std::copy_n(displaced,9,original.begin());
  auto live=original;const auto jump=Jump(game+CopyRva,relay);std::copy(jump.begin(),jump.end(),live.begin());std::fill(live.begin()+5,live.begin()+9,uint8_t(0x90));memory[game+CopyRva]=live;
  memory[relay]=Jump(relay,ff+CallbackRva);callback[0x57]=0xa1;Put(callback,0x58,ff+AllocationRva);
  constexpr uint8_t invoke[]{0xff,0x75,0x08,0x8b,0xd6,0x8b,0xcf,0xff,0xd0,0x8b,0xf8};std::copy_n(invoke,sizeof(invoke),callback.begin()+0x70);memory[ff+CallbackRva]=callback;
  auto allocation=std::vector<uint8_t>(8);Put(allocation,0,trampoline);Put(allocation,4,19);memory[ff+AllocationRva]=allocation;
  auto continuation=std::vector<uint8_t>(14);std::copy_n(displaced,9,continuation.begin());const auto cont=Jump(trampoline+9,game+CopyRva+9);std::copy(cont.begin(),cont.end(),continuation.begin()+9);memory[trampoline]=continuation;
 }
 Evidence Check(bool identity=true){return Verify(game,ff,identity,original,callback,[&](uint32_t address,size_t size){const auto it=memory.find(address);if(it==memory.end()||size>it->second.size())throw std::runtime_error("fixture_unreadable");return std::vector<uint8_t>(it->second.begin(),it->second.begin()+size);});}
 bool Refuses(){try{Check();return false;}catch(const std::runtime_error&){return true;}}
};
int main(){
 Fixture f;const auto result=f.Check();assert(result.fusionFixOverlay&&result.trampoline==f.trampoline&&result.relay==f.relay);
 {Fixture g;g.memory[g.game+CopyRva]=g.original;assert(!g.Check(false).fusionFixOverlay);}
 {Fixture g;try{g.Check(false);assert(false);}catch(const std::runtime_error&){}}
 for(size_t i=5;i<CopySize;++i){Fixture g;g.memory[g.game+CopyRva][i]^=1;assert(g.Refuses());}
 for(size_t i=0;i<CallbackSize;++i){Fixture g;g.memory[g.ff+CallbackRva][i]^=1;assert(g.Refuses());}
 for(size_t i=0;i<14;++i){Fixture g;g.memory[g.trampoline][i]^=1;assert(g.Refuses());}
 for(size_t i=0;i<8;++i){Fixture g;g.memory[g.ff+AllocationRva][i]^=1;assert(g.Refuses());}
 {Fixture g;g.memory[g.game+CopyRva][0]=0x90;assert(g.Refuses());}
 {Fixture g;g.memory[g.relay][0]=0x90;assert(g.Refuses());}
 {Fixture g;g.memory[g.relay][1]^=1;assert(g.Refuses());}
 {Fixture g;g.original[0]^=1;assert(g.Refuses());}
 {Fixture g;g.memory[g.ff+CallbackRva].clear();assert(g.Refuses());}
 {Fixture g;g.memory.erase(g.trampoline);assert(g.Refuses());}
 // A coincident known-looking callback is insufficient without the actual
 // compiled original-call argument contract and original slot.
 {Fixture g;g.callback[0x70]^=1;g.memory[g.ff+CallbackRva]=g.callback;assert(g.Refuses());}
 {Fixture g;g.callback[0x58]^=1;g.memory[g.ff+CallbackRva]=g.callback;assert(g.Refuses());}
 std::cout<<"native copy70 exact overlay and mutation rejection passed\n";
}
