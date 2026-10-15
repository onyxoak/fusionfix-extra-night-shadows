from pathlib import Path
import sys, hashlib, json
s=Path(sys.argv[1]);entry=(s/'standalone-runtime.cpp').read_text();active=(s/'NativeProjectionRuntime105.inc').read_text();rt=(s/'EffectAttribution199CRuntime.inc').read_text()
assert '#include "NativeProjectionRuntime105.inc"' in entry and '#include "NativeProjectionRuntime66.inc"' not in entry
assert '#include "EffectAttribution199CRuntime.inc"' in entry
assert 'L"EffectProducerDiagnostics199C",0' in entry
assert 'std::atomic<DiagnosticPacket199C> diagnosticPacket199c{nullptr};' in active
assert 'if(const auto observer=diagnosticPacket199c.load(std::memory_order_acquire))observer(i,lights,count,frame,packet);' in active
for call in ['PrepareHooks();','Enable();','Disable();','CaptureTransition(transition);']:
    assert 'EffectAttribution199C::'+call in entry
assert '&EffectAttribution199C::insertionHook' in entry
assert rt.index('if(!attemptBudget.Claim(tick))return;') < rt.index('effect_attribution199c::Decode(')
assert rt.count('Prepare(insertionHook,')==1 and 'unsafe_thiscall' not in rt and 'unsafe_fastcall' not in rt
files=['standalone-runtime.cpp','NativeProjectionRuntime105.inc','EffectAttribution199CRuntime.inc','EffectAttribution199C.hpp']
Path('production-inputs199c.json').write_text(json.dumps({p:hashlib.sha256((s/p).read_bytes()).hexdigest().upper() for p in files},indent=2))
print('199c active entry/105 callback, default-off, bounded-attempt and single diagnostic hook wiring passed')
