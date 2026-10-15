#include "standalone-ownership.hpp"
#include <array>
#include <cassert>
#include <iostream>
int main(){
 using namespace onyxoak;
 // Actual build15 failure: the ten-byte DF8 stub overlapped the six-byte
 // emergency DFC stub. A shared DFC callback plus cache E29 is disjoint.
 std::array<PreparedHookRange,2> overlap{{{0x527df8,10},{0x527dfc,6}}};
 assert(!DisjointPreparedHooks(overlap));
 std::array<PreparedHookRange,2> shared{{{0x527dfc,6},{0x527e29,9}}};
 assert(DisjointPreparedHooks(shared));
 shared[1]={UINTPTR_MAX-2,5};assert(!DisjointPreparedHooks(shared));
 std::vector<uint8_t> image(100,0x90);
 OwnedPatch a{PatchKind::HeadlightCall,10,{0xe8,1,2,3,4},{0xe8,5,6,7,8},true};
 std::copy(a.expectedInstalled.begin(),a.expectedInstalled.end(),image.begin()+10);
 std::vector<OwnedPatch> p{a};const auto original=image;
 auto normalized=NormalizeForOriginalGuards(image,p,true,true);
 assert(normalized&&image==original&&normalized.bytes[11]==5&&normalized.bytes[9]==0x90&&normalized.bytes[15]==0x90);
 assert(!NormalizeForOriginalGuards(image,p,false,true));
 assert(!NormalizeForOriginalGuards(image,p,true,false));
 p[0].ownershipVerified=false;assert(!NormalizeForOriginalGuards(image,p,true,true));p[0].ownershipVerified=true;
 image[12]^=1;assert(!NormalizeForOriginalGuards(image,p,true,true));image=original;
 p.push_back(a);assert(!NormalizeForOriginalGuards(image,p,true,true));p.resize(1);
 p[0].rva=99;assert(!NormalizeForOriginalGuards(image,p,true,true));p[0]=a;
 p[0].originalRelocated.pop_back();assert(!NormalizeForOriginalGuards(image,p,true,true));
 for(size_t fail=0;fail<14;++fail){ActivationGate gate(14);for(size_t i=0;i<fail;++i)assert(gate.HookEnabled(true));assert(!gate.HookEnabled(false));assert(!gate.HookEnabled(true));assert(!gate.Commit());assert(!gate.Active());gate.Abort();assert(!gate.Active());}
 ActivationGate gate(14);for(unsigned i=0;i<14;++i){assert(gate.HookEnabled(true));assert(!gate.Active());}assert(gate.Commit());gate.Abort();assert(!gate.Active());
 std::cout<<"PASS ownership normalization: exact known patches only, unchanged guards preserved, no live writes, readiness/identity/foreign/mutation/overlap/range rejection, activation failure at all fourteen stages\n";
}

