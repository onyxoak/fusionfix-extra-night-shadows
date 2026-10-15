#include "core/LabPoliceClaim.hpp"
#include <cassert>
#include <iostream>
using lab_siren::PoliceClaim;
using C=fusionfix::shadows::NativeShadowContinuity42;
int main(){
 using lab_siren::NativeSirenOn;
 assert(NativeSirenOn(0x10,0x20));assert(!NativeSirenOn(0,0x20));assert(!NativeSirenOn(0x30,0x20));assert(!NativeSirenOn(0x10,0));
 constexpr lab_siren::Identity owner{1,2,3,4};PoliceClaim p;
 p.Begin(true,owner,true,true,1,10);assert(p.Observe(true,true));
 p.Begin(true,owner,true,true,2,20);assert(!p.Observe(false,false));assert(p.Active()); // Blink retains metadata only.
 p.Begin(true,owner,true,true,3,30);assert(p.Observe(true,true));
 p.Begin(true,owner,true,true,16,160);assert(!p.Observe(false,false));assert(!p.Active());
 p.Begin(true,owner,true,true,17,170);assert(p.Observe(true,true));
 p.Begin(true,owner,true,false,18,180);assert(!p.Observe(true,true));assert(!p.Active()); // Siren-off independent of intensity.
 p.Begin(true,owner,false,true,19,190);assert(!p.Observe(true,true));
 p.Begin(true,owner,true,true,20,200);assert(!p.Observe(true,false));
 p.Begin(false,owner,true,true,21,210);assert(p.Retired());p.Begin(true,owner,true,true,22,220);assert(!p.Observe(true,true));
 for(int field=0;field<4;++field){PoliceClaim q;auto other=owner;if(field==0)++other.owner;if(field==1)++other.pool;if(field==2)++other.slot;if(field==3)++other.reference;
 q.Begin(true,owner,true,true,1,10);assert(q.Observe(true,true));q.Begin(true,other,true,true,2,20);assert(q.Retired());assert(!q.Observe(true,true));}
 C::Candidate siren{372,7,false,true},lamp{103,6,false,true},traffic{356,6,false,true},own{1380,0,true,true};
 assert(PoliceClaim::Compare(2,true,false,true,siren,lamp)==0);
 assert(PoliceClaim::Compare(2,true,false,true,siren,traffic)==0);
 assert(PoliceClaim::Compare(0,false,true,true,lamp,siren)==2);
 assert(PoliceClaim::Compare(2,true,false,true,siren,own)==2);
 assert(PoliceClaim::Compare(2,true,false,false,siren,lamp)==2);
 assert(PoliceClaim::Compare(1,false,false,true,lamp,traffic)==1);
 auto special=lamp;special.flags|=0x400;assert(PoliceClaim::Compare(2,true,false,true,siren,special)==2);
 // An absent private input cannot enter a current-native candidate list.
 // Compare has no access to indices, cache memory or shadow records.
 auto missing=siren;missing.observed=false;assert(PoliceClaim::Compare(2,true,false,true,missing,lamp)==2);
 std::cout<<"PASS police claim: native flag predicate, dark metadata/no synthetic input, bounded irrelevance, off/mode release, retirement/no revival, one-private comparison, own/special protection and unrelated comparison passthrough\n";
}
