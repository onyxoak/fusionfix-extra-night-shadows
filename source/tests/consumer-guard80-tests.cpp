#include "core/ConsumerGuard80.hpp"
#include <array>
#include <cassert>
int main(){
 using namespace consumer_guard80;
 std::array<uint8_t,0x5f5> expected{};
 for(unsigned i=0;i<expected.size();++i)expected[i]=uint8_t((i*31u)^0x47u);
 expected[0x3c6]=expected[0x5ce]=0x74;
 auto actual=expected;
 assert(Verify(expected,actual,false)==Body::Pristine);
 assert(Verify(expected,actual,true)==Body::Pristine);
 actual[0x3c6]=actual[0x5ce]=0xeb;
 assert(Verify(expected,actual,true)==Body::FusionFix511);
 assert(Verify(expected,actual,false)==Body::Refused);
 for(unsigned i=0;i<actual.size();++i){auto changed=actual;changed[i]^=1;
  assert(Verify(expected,changed,true)==Body::Refused);}
 auto partial=expected;partial[0x3c6]=0xeb;
 assert(Verify(expected,partial,true)==Body::Refused);
 partial=expected;partial[0x5ce]=0xeb;
 assert(Verify(expected,partial,true)==Body::Refused);
 assert(Verify(std::span<const uint8_t>(expected.data(),expected.size()-1),actual,true)==Body::Refused);
 auto wrongOriginal=expected;wrongOriginal[0x3c6]=0x75;
 assert(Verify(wrongOriginal,actual,true)==Body::Refused);
}
