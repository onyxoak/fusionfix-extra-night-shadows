from pathlib import Path
import hashlib,json,sys
s=Path(sys.argv[1]);out=Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
def part(t,a,b):
 start=t.index(a);return t[start:t.index(b,start)]
a=s/'core/ShadowAllocationRuntime.inl';r=s/'standalone-runtime.cpp';h=s/'core/standalone-shadow-core.cpp'
at=a.read_text();rt=r.read_text();ht=h.read_text()
assert at.index('ownerThread.compare_exchange_strong')<at.index('ApplyLiveReachAtSelection122(current);')<at.index('if(state.depth==1)BeginShortlist48();')
gate=part(at,'    static void ApplyLiveReachAtSelection122(','    static void __cdecl Select()')
boundary=part(rt,'void ApplyLiveReach122Boundary() noexcept {','void RefreshApplied119State() noexcept {')
lamp=part(at,'    static void AdjustLampDistance(','    static void Collect(')
collect=part(at,'        const int feet = fusionfix::shadows::ShadowReachFeet(FusionFixSettings.Get(\n','        float viewWeight{};')
beam=part(ht,'            const int feet = fusionfix::shadows::ShadowReachFeet(FusionFixSettings.Get("PREF_HEADLIGHT_REACH"));','            const bool accepted = selector.Consider(')
(out/'live-reach122-extracted.inc').write_text('#if defined(EXTRACT122_RUNTIME)\n'+boundary+'#endif\n#if defined(EXTRACT122_GATE)\n'+gate+'#endif\n#if defined(EXTRACT122_LAMP)\n'+lamp+'#endif\n#if defined(EXTRACT122_REACH)\nfloat CollectReach122(budget::Kind kind,const rage::CLightSource& light){\n'+collect+'return reach;\n}\nfloat BeamReach122(bool playerHeadlight,float sourceRadius){\n'+beam+'return reach;\n}\n#endif\n')
(out/'live-reach122-extraction.json').write_text(json.dumps({str(p.relative_to(s)):hashlib.sha256(p.read_bytes()).hexdigest().upper() for p in (a,r,h)},indent=2))
