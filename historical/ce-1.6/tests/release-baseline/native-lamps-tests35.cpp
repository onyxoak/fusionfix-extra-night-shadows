#define main OriginalPassTests
#include "D:/GTA-IV-Collection-Lab/view-priority-candidate26/shadow-test/offline/allocation_pass_tests.cpp"
#undef main

static Output Protected(ShadowAllocationPass& pass,const Fixture& f,Output native,Frame frame=At(1)) {
    BeginAndObserve(pass,f,frame);
    CHECK(pass.Commit(f.Base(),f.Count(),native.indices.data(),true));
    CheckCurrentUnique(native,f);
    const auto& published=pass.LastSelection();
    for(unsigned i=0;i<7;++i) {
        CHECK(bool(published.validMask&(1u<<i))==(native.indices[i]>=0));
        if(native.indices[i]>=0) CHECK(published.slots[i].index==unsigned(native.indices[i]));
    }
    return native;
}
int main() {
    // Reproduce the previous transaction replacing a valid native lamp with a
    // more attractive candidate, then require exact native lamp slot survival.
    Fixture f(12);f.candidates[11].distanceSquared=0.1f;
    ShadowAllocationPass oldPass,fixedPass;
    auto old=CommitFixture(oldPass,f,At(1));CHECK(Contains(old,11));
    auto native=f.NativeOutput();
    CHECK(SameBytes(Protected(fixedPass,f,native),native));
    // An own beam cannot evict seven cached lamps, even under full pressure.
    f.candidates[11].kind=Kind::PlayerBeam;
    CHECK(SameBytes(Protected(fixedPass,f,native,At(2)),native));
    // With a native beam slot available, replace that NPC beam with our car.
    f.candidates[6].kind=Kind::OtherBeam;f.candidates[6].distanceSquared=100000.0f;
    auto hybrid=Protected(fixedPass,f,native,At(3));CHECK(Contains(hybrid,11));
    for(unsigned i=0;i<6;++i) CHECK(hybrid.indices[i]==native.indices[i]);
    // No new lamp may occupy an engine hole; distant native lamps remain.
    Fixture lamps(12);lamps.candidates[0].distanceSquared=100000.0f;
    auto holes=lamps.NativeOutput();holes.indices[3]=-1;holes.indices[5]=-1;
    ShadowAllocationPass holesPass;CHECK(SameBytes(Protected(holesPass,lamps,holes),holes));
    // Retired lamps cannot survive via history; native removal is authoritative.
    auto fewer=holes;fewer.indices[0]=-1;
    CHECK(SameBytes(Protected(holesPass,lamps,fewer,At(2)),fewer));
    // Current-record mutation must still fail without touching output bytes.
    BeginAndObserve(holesPass,lamps,At(3));lamps.records[0][0x64]^=1;
    auto invalid=holes;
    CHECK(!holesPass.Commit(lamps.Base(),lamps.Count(),invalid.indices.data(),true));
    CHECK(SameBytes(invalid,holes));
    // Fast movement, shuffled candidate input, changing cache population and
    // strong offscreen/onscreen competition cannot move any native lamp.
    std::mt19937 rng(3501);ShadowAllocationPass stress;
    for(unsigned frame=1;frame<=5000;++frame) {
        Fixture scene(30);
        for(unsigned i=0;i<30;++i) {
            scene.candidates[i].kind=i%5==0?Kind::PlayerBeam:i%3==0?Kind::OtherBeam:Kind::Lamp;
            scene.candidates[i].distanceSquared=float(rng()%100000);
            scene.candidates[i].viewWeight=1.0f+float(rng()%200)/100.0f;
        }
        auto before=scene.NativeOutput();
        for(unsigned i=0;i<7;++i) if(rng()%5==0)before.indices[i]=-1;
        std::shuffle(scene.candidates.begin(),scene.candidates.end(),rng);
        auto after=Protected(stress,scene,before,At(frame,frame*16));
        for(unsigned i=0;i<7;++i) if(before.indices[i]>=0) {
            const auto key=1000u+before.indices[i];
            auto found=std::find_if(scene.candidates.begin(),scene.candidates.end(),[&](auto c){return c.key==key;});
            if(found->kind==Kind::Lamp) CHECK(after.indices[i]==before.indices[i]);
        }
    }
    // Invalid proposed ownership may never weaken the transaction's fallback.
    PlayerShadowBudget::Selection a{},b{};a.safeToApply=b.safeToApply=true;
    a.count=b.count=1;a.validMask=b.validMask=1;
    a.slots[0]={10,0,2,Kind::Lamp};b.slots[0]={11,0,2,Kind::PlayerBeam};
    CHECK(!PreserveNativeLampSlots(a,b).safeToApply);
    b=a;b.validMask=0x81;CHECK(!PreserveNativeLampSlots(a,b).safeToApply);
    // Native player beam keeps the only available beam slot even when the
    // narrower proposed set contains only a competing traffic headlight.
    a={};b={};a.safeToApply=b.safeToApply=true;
    a.count=7;a.validMask=127;b.count=1;b.validMask=1;
    for(unsigned i=0;i<7;++i)a.slots[i]={100+i,0,i,i==6?Kind::PlayerBeam:Kind::Lamp};
    b.slots[0]={200,0,7,Kind::OtherBeam};
    auto own=PreserveNativeLampSlots(a,b);CHECK(own.safeToApply);CHECK(own.slots[6].key==106);
    std::cout<<"PASS native lamp conservation, old-displacement regression, beam competition, holes, removal, transaction failure and 5000 shuffled frames; "<<checks<<" checks\n";
}
