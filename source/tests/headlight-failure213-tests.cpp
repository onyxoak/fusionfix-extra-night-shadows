#define NOMINMAX
#include "standalone-pe.hpp"
#include "HeadlightFailure213.hpp"
#include <cassert>
#include <iostream>
int wmain(int argc,wchar_t** argv){
 assert(argc==2);using namespace fusionfix::shadows::ce;
 for(uint32_t base:{0x400000u,0x10000000u}){
  onyxoak::pe::Image game(onyxoak::pe::Read(argv[1]),base);
  assert(ValidateMappedImage(game.bytes.data(),game.bytes.size(),base));
  assert(!headlight_failure213::Inspect(game.bytes,base).valid);
  unsigned mutations=0;
  for(auto [at,count]:{std::pair<uint32_t,size_t>{SubmitRva,25},{CallRvas[0]-9,17},{CallRvas[1]-9,17},{CallRvas[2]-9,17},{CallRvas[3]-9,17},{0x769f26,6},{0x4d0f82,6}}){
   for(size_t i=0;i<count;++i){
    auto& b=game.bytes[at+i];uint8_t expected=b;b^=1;
    assert(!ValidateMappedImage(game.bytes.data(),game.bytes.size(),base));
    auto result=headlight_failure213::Inspect(game.bytes,base);assert(result.valid&&result.rva==at+i&&result.expected[0]==expected&&result.actual[0]==b);
    b^=1;++mutations;
   }
  }
  // Even a modified disk baseline cannot hide which canonical instruction failed.
  game.bytes[SubmitRva]=0xe9;auto result=headlight_failure213::Inspect(game.bytes,base);assert(result.valid&&result.rva==SubmitRva&&result.expected[0]==0xff);
  std::cout<<"PASS real CE image at base "<<base<<"; "<<mutations<<" guard-byte changes identified, guards still reject\n";
 }
}
