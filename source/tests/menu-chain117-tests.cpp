#include "MenuChainGuard117.hpp"
#include <algorithm>
#include <cassert>
#include <map>
#include <vector>
struct Reader {
 std::map<uint32_t,std::vector<uint8_t>> blocks;
 bool denied{};
 bool Executable(uint32_t at,size_t bytes) noexcept{return !denied&&blocks.count(at)&&blocks[at].size()>=bytes;}
 bool Copy(uint32_t at,void* output,size_t bytes) noexcept {
  if(!Executable(at,bytes))return false;std::memcpy(output,blocks[at].data(),bytes);return true;
 }
};
std::vector<uint8_t> Jump(uint32_t from,uint32_t to){std::vector<uint8_t> bytes(32,0x90);bytes[0]=0xe9;const uint32_t delta=to-from-5;std::memcpy(bytes.data()+1,&delta,4);return bytes;}
int main(){
 constexpr uint32_t nativeAt=0x10000,thunk=0x20000,ff=0x30000;
 std::array<uint8_t,32> native{},expected{};native.fill(0x53);expected.fill(0x55);
 Reader r;r.blocks[nativeAt]={native.begin(),native.end()};r.blocks[ff]={expected.begin(),expected.end()};
 auto check=[&]{return menu_chain117::Authenticate(r,nativeAt,native,ff,expected,6);};
 auto patch=[&](uint32_t destination){auto bytes=Jump(nativeAt,destination);std::copy(native.begin()+6,native.end(),bytes.begin()+6);return bytes;};
 assert(check().owner==menu_chain117::Owner::Native);
 r.blocks[nativeAt]=patch(thunk);r.blocks[thunk]=Jump(thunk,ff);
 auto valid=check();assert(valid.owner==menu_chain117::Owner::FusionFix&&valid.first==thunk&&valid.destination==ff&&valid.hops==2);
 // The bytes following a near-thunk jump need not be the original prologue.
 for(unsigned n=5;n<32;++n)r.blocks[thunk][n]=uint8_t(n);assert(check().owner==menu_chain117::Owner::FusionFix);
 r.blocks[nativeAt]=patch(ff);assert(check().owner==menu_chain117::Owner::FusionFix&&check().hops==1);
 r.blocks[nativeAt]=patch(thunk);
 for(unsigned n=5;n<32;++n){r.blocks[nativeAt][n]^=1;assert(check().owner==menu_chain117::Owner::Refused);r.blocks[nativeAt][n]^=1;}
 for(unsigned n=0;n<expected.size();++n){r.blocks[ff][n]^=1;assert(check().owner==menu_chain117::Owner::Refused);r.blocks[ff][n]^=1;}
 r.blocks[thunk]=Jump(thunk,0x40000);assert(check().owner==menu_chain117::Owner::Refused);
 r.blocks[0x40000]=Jump(0x40000,ff);assert(check().owner==menu_chain117::Owner::Refused); // Three hops not authorized.
 r.blocks[thunk]=Jump(thunk,nativeAt);assert(check().owner==menu_chain117::Owner::Refused);
 r.blocks[thunk]=Jump(thunk,ff);r.blocks[thunk][0]=0xe8;assert(check().owner==menu_chain117::Owner::Refused);
 r.blocks[thunk]=Jump(thunk,ff);r.denied=true;assert(check().owner==menu_chain117::Owner::Refused);
 r.denied=false;assert(menu_chain117::Authenticate(r,nativeAt,native,0,expected).owner==menu_chain117::Owner::Refused);
}
