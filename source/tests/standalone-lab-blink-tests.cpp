#include "core/LabSirenBlinkGrace.hpp"
#include "core/NativeShadowContinuity42.hpp"
#include "core/NativeCacheDependencies44.hpp"
#include "core/ShadowInactiveSlots.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
using lab_siren::BlinkGrace;
using lab_siren::Identity;
using C=fusionfix::shadows::NativeShadowContinuity42;
constexpr Identity owner{100,200,3,4};
BlinkGrace Gap(){BlinkGrace g;g.Begin(true,owner,10,100);g.Finish(900,true,6,0,0,true);g.Begin(true,owner,11,110);g.Finish(900,false,-1,500,55,true);return g;}
int main(){
 auto g=Gap();assert(!g.Reclaim(900,true,500,55,6)); // Never render missing-frame metadata.
 g.Begin(true,owner,12,120);assert(g.Reclaim(900,true,500,55,6));
 assert(!g.Reclaim(901,true,500,55,6));assert(!g.Reclaim(900,false,500,55,6));
 assert(!g.Reclaim(900,true,501,55,6));assert(!g.Reclaim(900,true,500,56,6));assert(!g.Reclaim(900,true,500,55,5));
 C continuity;std::array<C::Identity,7> chosen{};chosen[6]={900,1};continuity.Begin(1,10,100);assert(continuity.Commit(chosen));
 continuity.Begin(1,11,110);chosen[6]={500,55};assert(continuity.Commit(chosen));
 continuity.Begin(1,12,120);auto siren=continuity.Observe({900,1},372,true,false);const auto lamp=continuity.Observe({500,55},103,true,false);
 assert(siren.claim==7&&lamp.claim==6);assert(C::Compare(0,siren,lamp)==2);
 siren.claim=g.Ordinal();assert(C::Compare(0,siren,lamp)==0);
 auto earlier=lamp;earlier.claim=5;assert(C::Compare(0,siren,earlier)==2);
 // Authenticated physical forward private lamp, not any own vehicle emitter.
 auto own=lamp;own.ownBeam=true;own.stableOrder=1;assert(C::Compare(0,siren,own)==2);
 auto ownRear=own;ownRear.stableOrder=0;ownRear.focusTier=1;ownRear.focusDistance=100.f;
 auto impactfulSiren=siren;impactfulSiren.focusTier=1;impactfulSiren.focusDistance=1.f;
 assert(C::Compare(1,impactfulSiren,ownRear)==0); // Own native rear source gets no unconditional pin.
 auto special=lamp;special.flags|=0x400;assert(C::Compare(2,siren,special)==2);
 chosen[6]={900,1};assert(continuity.Commit(chosen));assert(continuity.Observe({500,55},103,true,false).claim==7);
 g.Finish(900,true,6,0,0,true);g.Begin(true,owner,13,130);assert(!g.Reclaim(900,true,500,55,6));
 g=Gap();g.Begin(true,owner,12,120);g.Finish(900,false,-1,500,55,true);g.Begin(true,owner,13,130);assert(!g.Reclaim(900,true,500,55,6)); // Siren off/longer absence.
 g=Gap();g.Begin(false,owner,12,120);assert(!g.Reclaim(900,true,500,55,6));
 for(int field=0;field<4;++field){auto next=owner;if(field==0)++next.owner;if(field==1)++next.pool;if(field==2)++next.slot;if(field==3)++next.reference;g=Gap();g.Begin(true,next,12,120);assert(!g.Reclaim(900,true,500,55,6));}
 g=Gap();g.Begin(true,owner,14,120);assert(!g.Reclaim(900,true,500,55,6));
 g=Gap();g.Begin(true,owner,12,500);assert(!g.Reclaim(900,true,500,55,6));
 g=Gap();g.Begin(true,owner,12,120);g.Finish(900,true,-1,500,55,true);g.Begin(true,owner,13,130);assert(!g.Reclaim(900,true,500,55,6));
 // Grace never transfers cache ownership. Existing dependency policy preserves
 // selected maps and may re-use unneeded maps only through native scheduling.
 std::array<fusionfix::shadows::NativeCacheEntry44,8> cache{};for(auto& e:cache)e={100,1,500,0};
 const auto before=cache;assert(fusionfix::shadows::ProtectNativeCacheChoice44(0,1,cache,255)==-1);
 assert(std::memcmp(cache.data(),before.data(),sizeof(cache))==0);
 // Existing publication cleanup must clear missing inactive identities.
 std::array<unsigned char,0x880> records{};
 const uint32_t key=900;std::memcpy(records.data()+0x110+0xF8,&key,4);
 assert(fusionfix::shadows::ClearInactiveShadowKeys(records.data(),records.size())>=1);
 uint32_t cleared=1;std::memcpy(&cleared,records.data()+0x110+0xF8,4);assert(cleared==0);
 std::cout<<"PASS private blink grace: one-pass return, longer/off release, owner identity replacement, relevance/competitor identity, priority, cache nonownership and inactive-key cleanup\n";
}
