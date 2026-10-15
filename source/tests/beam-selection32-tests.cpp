#include <cassert>
#include <iostream>
#include "core/ShadowAdapterCE.hpp"
using namespace fusionfix::shadows::ce;
unsigned combined{},legacyOwner{},legacyOrder{},legacyBeam{};
BeamPriorityEvidence32 Evidence(uintptr_t key) noexcept {++combined;return key==5000?BeamPriorityEvidence32{1000,2}:BeamPriorityEvidence32{};}
bool OldBeam(uintptr_t key,uintptr_t car) noexcept {++legacyBeam;return key==5000&&car==1000;}
uintptr_t OldOwner(uintptr_t key) noexcept {++legacyOwner;return key==5000?1000:0;}
uint32_t OldOrder(uintptr_t key) noexcept {++legacyOrder;return key==5000?2:0;}
int main(){
 AdditionalVehicleBeamPriority32=Evidence;AdditionalVehicleBeam=OldBeam;AdditionalVehicleBeamOwner=OldOwner;AdditionalVehicleBeamOrder=OldOrder;
 auto own=ResolveBeamSelection32(5000,1000);assert(own.own&&own.privateOwner==1000&&own.order==2);
 auto traffic=ResolveBeamSelection32(5000,2000);assert(!traffic.own&&traffic.privateOwner==1000&&traffic.order==2);
 auto foot=ResolveBeamSelection32(5000,0);assert(!foot.own&&foot.privateOwner==1000);
 auto missing=ResolveBeamSelection32(6000,1000);assert(!missing.own&&!missing.privateOwner&&!missing.order);
 auto native=ResolveBeamSelection32(1000,1000);assert(native.own&&!native.privateOwner&&!native.order);
 auto nativeSingle=ResolveBeamSelection32(1001,1000);assert(nativeSingle.own);
 assert(combined==6&&legacyOwner==0&&legacyOrder==0&&legacyBeam==0);
 AdditionalVehicleBeamPriority32=nullptr;
 own=ResolveBeamSelection32(5000,1000);assert(own.own&&own.order==2);
 traffic=ResolveBeamSelection32(5000,2000);assert(!traffic.own&&traffic.privateOwner==1000);
 foot=ResolveBeamSelection32(5000,0);assert(!foot.own&&foot.privateOwner==1000);
 assert(legacyOrder==1&&legacyOwner==2&&legacyBeam==2);
 std::cout<<"Combined callback once per selection, own/traffic/on-foot/native and legacy fallback passed.\n";
}
