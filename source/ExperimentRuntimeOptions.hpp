#pragma once
#include "core/NearbyPoliceShadows.hpp"
#include <cstdint>
namespace experiment_perf {
struct Options {bool traffic{},detail{},counters{},ownerTelemetry{};};
constexpr Options Configure(unsigned logging,bool trafficObserver,bool counters,bool observeOwners) noexcept {
 return {trafficObserver,logging==2,logging==2&&counters,logging==2&&observeOwners};
}
constexpr unsigned ValidationVisits(unsigned used) noexcept {return used<32?used:32;}
constexpr unsigned TokenIndex(uint32_t key,uint32_t base,unsigned tokens) noexcept {
 if(key<base||((key-base)&63u))return tokens;
 const auto index=(key-base)/64;return index<tokens?index:tokens;
}
template<class Counter> inline void Count(bool enabled,Counter& counter) noexcept {if(enabled)++counter;}
template<class A,class B>
int CompareNearby(int unchanged,bool ap,bool bp,unsigned ar,unsigned br,bool aObserved,bool bObserved,
 bool aRelevant,bool bRelevant,bool aOwn,bool bOwn,bool aSpecial,bool bSpecial,A&& visibleA,B&& visibleB) noexcept {
 if(ap==bp)return lab_siren::NearbyPolice::Compare(unchanged,ap,bp,ar,br,aObserved,bObserved,aRelevant,bRelevant,aOwn,bOwn,aSpecial,bSpecial,false,false);
 return lab_siren::NearbyPolice::Compare(unchanged,ap,bp,ar,br,aObserved,bObserved,aRelevant,bRelevant,aOwn,bOwn,aSpecial,bSpecial,visibleA(),visibleB());
}
}
