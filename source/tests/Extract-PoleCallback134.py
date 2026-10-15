from pathlib import Path
import hashlib,json,sys
source=Path(sys.argv[1]);output=Path(sys.argv[2]);p=source/'standalone-runtime.cpp';raw=p.read_bytes();t=raw.decode().replace('\r\n','\n')
a=t.index('void Caster(SafetyHookContext& regs) {');b=t.index('void Contact(',a)
body=t[a:b];assert body.count('PoleCasterDiagnostic134::Observe(regs.esi,regs.eax,artificial,skip);')==1
assert body.count('regs.eip=game+(skip?0x6e3867:0x6e374f);')==1
output.write_text(body)
inputs={str(p.relative_to(source)):hashlib.sha256(raw).hexdigest().upper()}
for name in ['PoleCasterDiagnostic134Runtime.inc','core/PoleCasterSnapshot134.hpp','core/PoleChildrenSnapshot135.hpp','PoleStaticPartition141Runtime.inc','core/PoleModeContract141.hpp','PoleVolume142Runtime.inc']:
 inputs[name]=hashlib.sha256((source/name).read_bytes()).hexdigest().upper()
output.with_suffix('.inputs.json').write_text(json.dumps(inputs,indent=2)+'\n')
