#include "FusionBackendLease130.hpp"
#include "FusionBackendGuard130.hpp"
#include <cassert>
#include <array>
#include <vector>
#include <map>
#include <cstring>
#include <cstdio>
using namespace fusion_backend130;
struct Memory130 {
 std::map<uint32_t,std::vector<uint8_t>> regions;
 bool Read(uint32_t at,void* out,size_t n) const noexcept {
  for(const auto& [base,bytes]:regions)if(at>=base&&uint64_t(at-base)+n<=bytes.size()){std::memcpy(out,bytes.data()+at-base,n);return true;}return false;
 }
 void Put(uint32_t at,uint32_t value){std::memcpy(regions.at(at).data(),&value,4);}
};
static void Store130(uint8_t* at,uint32_t value){std::memcpy(at,&value,4);}
int main(){
 for(unsigned mode=0;mode<4;++mode){
  constexpr std::array<std::array<uint8_t,3>,4> tuples{{{{0,0,1}},{{1,0,1}},{{1,1,0}},{{1,1,1}}}};
  assert(Decode(tuples[mode][0],tuples[mode][1],tuples[mode][2])==mode);
  Lease state;unsigned writes=0;uint8_t effective=uint8_t(mode);
  auto writable=[]()noexcept{return true;};auto original=[&](uint8_t n)noexcept{++writes;effective=n;return true;};
  assert(!state.Owns()&&!state.Pending(7));assert(state.Arm(7,uint8_t(mode))&&!state.Owns());
  assert(state.Apply(7,false,writable,original)&&writes==0); // precommit/Off has no writes
  state.wanted=true;assert(state.Apply(7,true,writable,original)&&effective==3&&state.Owns());
  assert(state.Request(2));assert(state.Apply(8,false,writable,original)==false&&effective==3);
  const auto before=writes;assert(state.Apply(7,false,writable,original)&&writes==before&&state.Owns()); // coalesced exceptional request
  state.Close();assert(!state.Owns()&&state.Apply(7,false,writable,original)&&effective==2);
  assert(state.Request(0)&&state.Apply(7,true,writable,original)&&effective==0&&!state.Owns());
 }
 assert(!Decode(0,1,1)&&!Decode(1,0,0)&&!Decode(2,1,1));
 Lease denied;assert(denied.Arm(1,0));denied.wanted=true;unsigned deniedCalls=0;
 assert(!denied.Apply(1,true,[]()noexcept{return false;},[&](uint8_t)noexcept{++deniedCalls;return true;})&&!denied.Owns()&&deniedCalls==0);
 assert(!denied.Request(4));
 // Real relocated FF callback bytes and ALL operand/end-byte mutations.
 const uint32_t ffBase130=0x31000000,gameBase130=0x00400000;
 Memory130 memory;memory.regions[ffBase130+ExtraCallbackRva]={ExtraBody.begin(),ExtraBody.end()};
 memory.regions[ffBase130+ListCallbackRva]={ListBody.begin(),ListBody.end()};
 for(unsigned at:{0x21u,0x26u,0x2cu})Store130(memory.regions.at(ffBase130+ExtraCallbackRva).data()+at,Word(ExtraBody.data()+at)-PreferredFF+ffBase130);
 Store130(memory.regions.at(ffBase130+ListCallbackRva).data()+0x10,ffBase130+ListIndexRva);
 auto read=[&](uint32_t a,void* out,size_t n)noexcept{return memory.Read(a,out,n);};assert(CallbackBodies(ffBase130,read));
 for(size_t at=0;at<ExtraBody.size();++at){auto& byte=memory.regions.at(ffBase130+ExtraCallbackRva)[at];byte^=1;assert(!CallbackBodies(ffBase130,read));byte^=1;}
 // Model the exact existing FF171-byte stub template; dynamic slots are bound.
 std::array<uint8_t,171> stubTemplate{};for(unsigned at=0;at<163;++at)stubTemplate[at]=uint8_t(at);
 const uint32_t relay130=0x11000000,stub130=0x11001000,tramp130=0x11002000;
 memory.regions[gameBase130+0x6e4236]=std::vector<uint8_t>(5);auto& installed=memory.regions.at(gameBase130+0x6e4236);installed[0]=0xe9;Store130(installed.data()+1,relay130-(gameBase130+0x6e4236+5));
 memory.regions[relay130]=std::vector<uint8_t>(5);memory.regions[relay130][0]=0xe9;Store130(memory.regions[relay130].data()+1,stub130-relay130-5);
 memory.regions[stub130]={stubTemplate.begin(),stubTemplate.end()};auto& stub=memory.regions[stub130];Store130(stub.data()+2,stub130+167);Store130(stub.data()+89,stub130+163);Store130(stub.data()+163,ffBase130+VehicleCallbackRva);Store130(stub.data()+167,tramp130);
 memory.regions[tramp130]=std::vector<uint8_t>(16);auto& code=memory.regions[tramp130];code[0]=0x0f;code[1]=0x84;Store130(code.data()+2,gameBase130+0x6e4255-tramp130-6);code[6]=0x53;code[7]=0x6a;code[9]=0xe9;Store130(code.data()+10,gameBase130+0x6e423b-tramp130-14);
 constexpr std::array<uint8_t,5> native{{0x74,0x1d,0x53,0x6a,0}};
 auto good=[&](){return MidOwner(gameBase130,ffBase130,0x6e4236,VehicleCallbackRva,5,native,stubTemplate,true,read);};assert(good());
 for(unsigned at=0;at<14;++at){code[at]^=1;assert(!good());code[at]^=1;}
 stub[163]^=1;assert(!good());stub[163]^=1;assert(good());
 assert(!MidOwner(gameBase130,ffBase130,0x6e4236,VehicleCallbackRva,5,native,stubTemplate,false,read));
 std::puts("PASS130 mode0..3, precommit/Off zero stores, numeric coalescing/restoration, malformed tuples, exact callback endpoints and relocated vehicle JE ownership");
}
