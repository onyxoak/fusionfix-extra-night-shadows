#include "ShadowLookupValidation.hpp"
#include <cassert>
#include <iostream>
using namespace fusionfix::shadows;
int main() {
    ShadowLookupSnapshot s{};
    unsigned cases=0;
    for (int slot=1;slot<8;++slot) for (int cache=-1;cache<8;++cache) {
        s={};s.dynamicKeys[slot]=123;s.dynamicActive[slot]=1;
        assert(ValidateShadowLookup(slot,123,cache,s)==slot);++cases;
        s.dynamicActive[slot]=0;
        assert(ValidateShadowLookup(slot,123,cache,s)==-1);++cases;
        if (cache>=0) {
            s.staticKeys[cache]=123;
            assert(ValidateShadowLookup(slot,123,cache,s)==cache+8);++cases;
            s.staticKeys[cache]=456;
            assert(ValidateShadowLookup(slot,123,cache,s)==-1);++cases;
        }
    }
    for(int i=0;i<8;++i) {
        s={};s.staticKeys[i]=123;
        assert(ValidateShadowLookup(i+8,123,i,s)==i+8);++cases;
        // The native lookup accepts this nonzero cache entry even after reuse.
        s.staticKeys[i]=456;
        assert(ValidateShadowLookup(i+8,123,i,s)==-1);++cases;
        s.staticKeys[i]=123;
        assert(ValidateShadowLookup(i+8,123,i,s)==i+8);++cases;
    }
    for(int i=1;i<8;++i) for(int cache=0;cache<8;++cache) {
        s={};s.dynamicKeys[i]=123;s.dynamicActive[i]=1;s.dynamicCache[i]=cache;
        s.staticKeys[cache]=123;
        assert(ValidateShadowLookup(i,123,cache,s)==i);++cases;
        // Slot zero's native cache refresh may recycle a static entry that a
        // selected dynamic light still references. Matching dynamic key alone
        // must not permit using the wrong cached static geometry.
        s.staticKeys[cache]=456;
        assert(ValidateShadowLookup(i,123,cache,s)==-1);++cases;
        s.dynamicCache[i]=9;
        assert(ValidateShadowLookup(i,123,cache,s)==-1);++cases;
        s.dynamicCache[i]=-1;
        assert(ValidateShadowLookup(i,123,cache,s)==i);++cases;
    }
    for (int sentinel : {-1,0}) {
        assert(ValidateShadowLookup(sentinel,123,0,s)==sentinel);++cases;
    }
    assert(ValidateShadowLookup(16,123,0,s)==-1);
    assert(ValidateShadowLookup(8,0,0,s)==8);
    assert(ValidateShadowLookup(8,UINT32_MAX,0,s)==8);
    std::cout << "PASS lookup identity, recycling, inactive-map and fallback cases: " << cases+3 << '\n';
}
