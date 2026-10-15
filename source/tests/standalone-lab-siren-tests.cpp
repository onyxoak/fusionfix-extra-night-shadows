#include "core/LabSirenLifetime.hpp"
#include <cassert>
#include <iostream>
#include <array>
using namespace lab_siren;
int main() {
    const Identity original{0x10000,0x20000,39,3};
    assert(Decide(false,false,false,true,{},original)==Decision::Wait);
    assert(Decide(false,false,true,false,{},original)==Decision::Wait);
    assert(Decide(false,false,true,true,{},original)==Decision::Bind);
    // Different geometry hashes stand for motion and alternating lamp centers.
    for (auto geometry : {1ull,2ull,0x1234567812345678ull,0xFFFFFFFFFFFFFFFFull}) {
        assert(Generation(true,geometry)==1);
        assert(Generation(false,geometry)==geometry);
        assert(Decide(true,false,true,true,original,original)==Decision::Keep);
        assert(Decide(true,false,false,true,original,original)==Decision::Keep);
    }
    std::array<Identity,4> replacements{original,original,original,original};
    ++replacements[0].owner; ++replacements[1].pool;
    ++replacements[2].slot; ++replacements[3].reference;
    for (auto replacement : replacements) {
        bool retired=Decide(true,false,true,true,original,replacement)==Decision::Retire;
        assert(retired);
        // Even an apparently identical old identity cannot revive a retired key.
        assert(Decide(true,retired,true,true,original,original)==Decision::Retire);
        assert(Decide(true,retired,false,true,original,replacement)==Decision::Retire);
    }
    assert(Decide(true,false,true,false,original,{})==Decision::Retire);
    assert(Decide(true,false,false,false,original,{})==Decision::Retire);
    std::cout << "PASS private siren: movement/flash generation, normal geometry passthrough, off/on identity, owner/pool/slot/reference replacement, missing-owner retirement, no revival\n";
}
