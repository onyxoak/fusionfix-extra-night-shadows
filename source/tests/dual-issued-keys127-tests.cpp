#include "DualIssuedKeys127.hpp"
#include <cassert>
#include <thread>
#include <cstdio>
int main(){
 using dual_keys127::PublishedKeys;
 PublishedKeys<66> issued;
 constexpr uint32_t base=0x20000000;
 for(unsigned i=0;i<66;++i)assert(!issued.Contains(base,base+i*64));
 assert(!issued.PublishPair(0,{0,64}));
 assert(!issued.PublishPair(base+1,{base,base+64}));
 assert(!issued.PublishPair(base,{base+64,base+128}));
 assert(!issued.PublishPair(base,{base,base}));
 assert(!issued.PublishPair(base,{base,base+65}));
 assert(!issued.PublishPair(base,{base,base+128}));
 assert(!issued.PublishPair(base,{base+66*64,base+67*64}));
 assert(!issued.Contains(base,base-1));assert(!issued.Contains(base,base+66*64));
 assert(!issued.Contains(0xffffffc0,0xffffffc0));
 PublishedKeys<66> overflow;
 assert(!overflow.PublishPair(0xffffffc0,{0xffffffc0,0}));
 // Pair publication covers a complete atomic word and its last two bits.
 assert(issued.PublishPair(base,{base+30*64,base+31*64}));
 assert(issued.ContainsPair(base,{base+30*64,base+31*64}));
 assert(!issued.Contains(base,base+32*64));
 for(unsigned offset=1;offset<64;++offset)assert(!issued.Contains(base,base+30*64+offset));
 assert(issued.PublishPair(base,{base+64*64,base+65*64}));
 assert(issued.Contains(base,base+65*64));
 // Concurrent disjoint and repeated publication never clears earlier ownership.
 std::thread a([&]{for(unsigned n=0;n<1000;++n)for(unsigned i=0;i<30;i+=2)
  assert(issued.PublishPair(base,{base+i*64,base+(i+1)*64}));});
 std::thread b([&]{for(unsigned n=0;n<1000;++n)for(unsigned i=32;i<64;i+=2)
  assert(issued.PublishPair(base,{base+i*64,base+(i+1)*64}));});
 a.join();b.join();
 for(unsigned i=0;i<66;++i)assert(issued.Contains(base,base+i*64));
 std::puts("PASS127 atomic pair publication, alignment/range/overflow and concurrent monotone ownership");
}
