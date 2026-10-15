#include "ModelEffectObserverState.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstdint>

using namespace model_effect_observer;
static unsigned checks;
#define CHECK(v) do { ++checks; if (!(v)) { std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#v); std::abort(); } } while(false)
static Invocation invocation(unsigned level=0) {
    Invocation c{};
    c.frame=40; c.nonce=level+1; c.preStack=0x9000-level*0x100;
    c.owner=0x10000+level*0x100; c.effect=0x20000+level*0x100;
    c.matrix=0x40000+level*0x100;
    c.ordinal=3; c.modelInfo=0x50000; c.proposedKey=c.owner+c.ordinal;
    c.sampled=true; return c;
}
static ResolveEvidence resolve(const Invocation& c) {
    return {c.frame,c.preStack,DispatchReturn,c.effect,c.matrix,c.owner,c.effect,0x30000+c.nonce*0x100,c.proposedKey};
}
static PointEvidence point(const Invocation& c,unsigned site=DirectPoint,unsigned flags=1) {
    return {c.frame,c.preStack,DispatchReturn,c.effect,resolve(c).descriptor,site,0,flags,(flags&6u)?c.proposedKey:0};
}
int main() {
    static_assert(MaxDepth==8);
    static_assert(PreSite==0x81e19d && PreStackAdjustment==8);
    static_assert(DispatchCall==0x81e1a4 && DispatchReturn==0x81e1a9 && ResolveSite==0x81e244);
    static_assert(PreOrdinalOffset==0x58 && PreModelOffset==0x60 && PreKeyOffset==0x18);
    static_assert(StoreWordCount(WrapperPoint)==16 && StoreKeyOffset(WrapperPoint)==0x3c);
    static_assert(StoreWordCount(DirectPoint)==18 && StoreKeyOffset(DirectPoint)==0x44);
    CHECK(StoreWordCount(0)==0 && StoreKeyOffset(0)==0);
    // Both authenticated pre-CALL packet layouts: direct key is not wrapper's.
    std::array<std::uint32_t,18> packet{}; packet[15]=0x1111; packet[17]=0x2222;
    CHECK(packet[StoreKeyOffset(WrapperPoint)/4]==0x1111);
    CHECK(packet[StoreKeyOffset(DirectPoint)/4]==0x2222);
    const auto c=invocation(); Invocation out{};
    { State s; CHECK(!s.Current() && s.Depth()==0); CHECK(!s.Resolve(resolve(c)));
      CHECK(!s.Point(point(c))); CHECK(s.Leave(c.preStack,c.frame,out)==LeaveResult::Missing); }
    for(auto site : {WrapperPoint,DirectPoint}) for(unsigned flags : {1u,3u,5u,7u,0x41u}) {
        State s; CHECK(s.Enter(c)); CHECK(s.Current()->descriptor==0);
        CHECK(s.Resolve(resolve(c))); CHECK(s.Current()->descriptor==resolve(c).descriptor);
        CHECK(s.Current()->descriptor!=s.Current()->effect); CHECK(s.Point(point(c,site,flags)));
        CHECK(s.Leave(c.preStack,c.frame,out)==LeaveResult::Valid);
        CHECK(out.nonce==c.nonce && out.owner==c.owner && out.descriptor==resolve(c).descriptor && out.submitted && out.resolved);
        CHECK(s.Depth()==0 && !s.Current()); CHECK(!s.Point(point(c,site,flags)));
    }
    // Unsampled calls preserve nesting but cannot acquire resolved/submitted evidence.
    { State s; auto n=c; n.sampled=false; CHECK(s.Enter(n));
      CHECK(!s.Resolve(resolve(n))); CHECK(!s.Point(point(n)));
      CHECK(s.Leave(n.preStack,n.frame,out)==LeaveResult::Valid); CHECK(!out.resolved && !out.submitted); }
    for(auto member : {&ResolveEvidence::frame,&ResolveEvidence::preStack,&ResolveEvidence::returnRva,
            &ResolveEvidence::effect,&ResolveEvidence::matrix,&ResolveEvidence::savedOwner,
            &ResolveEvidence::savedEffect,&ResolveEvidence::proposedKey}) {
        State s; CHECK(s.Enter(c)); auto e=resolve(c); ++(e.*member);
        CHECK(!s.Resolve(e)); CHECK(s.Current()->invalid); CHECK(!s.Resolve(resolve(c)));
        CHECK(!s.Point(point(c))); CHECK(s.Leave(c.preStack,c.frame,out)==LeaveResult::Invalid);
    }
    for(auto member : {&PointEvidence::frame,&PointEvidence::preStack,&PointEvidence::returnRva,
            &PointEvidence::effect,&PointEvidence::descriptor,&PointEvidence::site,&PointEvidence::type,&PointEvidence::key}) {
        State s; CHECK(s.Enter(c)); CHECK(s.Resolve(resolve(c))); auto e=point(c); ++(e.*member);
        CHECK(!s.Point(e)); CHECK(!s.Point(point(c)));
        CHECK(s.Leave(c.preStack,c.frame,out)==LeaveResult::Invalid);
    }
    // Missing/duplicate stages and stale copied descriptor fail closed.
    { State s; CHECK(s.Enter(c)); CHECK(!s.Point(point(c))); CHECK(!s.Resolve(resolve(c))); }
    { State s; CHECK(s.Enter(c)); CHECK(s.Resolve(resolve(c))); CHECK(!s.Resolve(resolve(c))); }
    { State s; CHECK(s.Enter(c)); CHECK(s.Resolve(resolve(c))); CHECK(s.Point(point(c))); CHECK(!s.Point(point(c))); }
    { State s; CHECK(s.Enter(c)); CHECK(s.Resolve(resolve(c))); s.Invalidate(); CHECK(!s.Point(point(c))); }
    // A native shadow bit requires the original key; a cleared bit requires zero.
    for(unsigned flags : {1u,5u}) { State s; CHECK(s.Enter(c)); CHECK(s.Resolve(resolve(c)));
        auto e=point(c,DirectPoint,flags); e.key=(flags&6)?0:c.proposedKey; CHECK(!s.Point(e)); }
    { State s; CHECK(s.Enter(c)); auto e=resolve(c); e.descriptor=0; CHECK(!s.Resolve(e)); }
    for(unsigned mode=0;mode<5;++mode) {
        State s; auto n=c;
        if(mode==0)n.owner=0; if(mode==1)n.effect=0; if(mode==2)n.matrix=0;
        if(mode==3)++n.proposedKey;
        if(mode==4){n.owner=UINT32_MAX;n.ordinal=1;n.proposedKey=0;}
        CHECK(s.Enter(n)); CHECK(s.Current()->invalid); CHECK(!s.Resolve(resolve(n)));
        CHECK(s.Leave(n.preStack,n.frame,out)==LeaveResult::Invalid);
    }
    // Eight genuine nested contexts survive normal unwind; overflow hides all
    // tracked parents until every overflow call returns. No point leaks upward.
    { State s;
      for(unsigned i=0;i<8;++i){auto n=invocation(i);CHECK(s.Enter(n));CHECK(s.Resolve(resolve(n)));}
      CHECK(s.Depth()==8); CHECK(!s.Enter(invocation(8))); CHECK(!s.Enter(invocation(9)));
      CHECK(!s.Current()); CHECK(!s.Resolve(resolve(c))); CHECK(!s.Point(point(c)));
      CHECK(s.Leave(invocation(9).preStack,c.frame,out)==LeaveResult::Overflow); CHECK(!s.Current());
      CHECK(s.Leave(invocation(8).preStack,c.frame,out)==LeaveResult::Overflow);
      for(unsigned i=8;i>0;--i){auto n=invocation(i-1);CHECK(s.Current()->nonce==n.nonce);
        CHECK(s.Point(point(n)));CHECK(s.Leave(n.preStack,n.frame,out)==LeaveResult::Valid);CHECK(out.nonce==n.nonce);}
      CHECK(!s.Current() && s.Depth()==0); }
    for(unsigned mismatch=0;mismatch<2;++mismatch) {
        State s; CHECK(s.Enter(c)); CHECK(s.Resolve(resolve(c)));
        CHECK(s.Leave(c.preStack+(mismatch?4:0),c.frame+(mismatch?0:1),out)==LeaveResult::Invalid);
        CHECK(!s.Current());
    }
    // A mismatched nested return cannot expose a usable stale parent.
    { State s; auto n=invocation(1); CHECK(s.Enter(c)); CHECK(s.Resolve(resolve(c)));
      CHECK(s.Enter(n)); CHECK(s.Resolve(resolve(n)));
      CHECK(s.Leave(c.preStack,c.frame,out)==LeaveResult::Invalid);
      CHECK(s.Current() && s.Current()->invalid); CHECK(!s.Point(point(c)));
      CHECK(s.Leave(c.preStack,c.frame,out)==LeaveResult::Invalid); }
    // Overflow return at a tracked stack address, or another frame, cannot be
    // interpreted as a normal unwind and revive prior evidence.
    for(unsigned mismatch=0;mismatch<2;++mismatch) {
      State s; for(unsigned i=0;i<9;++i)s.Enter(invocation(i));
      const auto stack=mismatch?invocation(7).preStack:invocation(8).preStack;
      s.Leave(stack,c.frame+(mismatch?0:1),out);
      CHECK(!s.Current()); CHECK(!s.Enter(c)); CHECK(!s.Resolve(resolve(c))); CHECK(!s.Point(point(c)));
    }
    std::printf("PASS model observer shared lifecycle: %u checks; both point ABIs, association, nesting, overflow, freshness, key normalization. Runtime gate/patch/FP preservation require integration review.\n",checks);
}
