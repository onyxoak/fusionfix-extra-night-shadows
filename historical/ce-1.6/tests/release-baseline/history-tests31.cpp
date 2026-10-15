#include "SubmittedHeadlightHistory.hpp"
#include <cassert>
#include <iostream>
using namespace fusionfix::shadows;
int main(){
    SubmittedHeadlightHistory h;
    h.Record(100,40);
    assert(h.Contains(100,40));assert(h.Contains(100,41));assert(h.Contains(100,42));
    assert(!h.Contains(100,43));assert(!h.Contains(101,41));assert(!h.Contains(0,41));
    h.Record(101,41);assert(h.Contains(100,41));assert(h.Contains(101,41));
    h.Reset();assert(!h.Contains(100,41));assert(!h.Contains(101,41));
    h.Record(100,UINT32_MAX);assert(h.Contains(100,0));assert(h.Contains(100,1));assert(!h.Contains(100,2));
    h.Reset();h.Record(100,50);assert(!h.Contains(100,1));
    for(unsigned i=0;i<1000;++i)h.Record(1000+i,i);
    assert(h.Contains(1999,999));assert(h.Contains(1998,999));assert(h.Contains(1997,999));
    assert(!h.Contains(1996,999));assert(!h.Contains(1000,999));
    std::cout<<"PASS headlight provenance survives frame handoff, expires, resets, and stays bounded\n";
}
