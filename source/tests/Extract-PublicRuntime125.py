from pathlib import Path
import sys,hashlib,json
source=Path(sys.argv[1]);output=Path(sys.argv[2]);inputs={}
def cut(name,start,end):
 p=source/name;raw=p.read_bytes();inputs[name]=hashlib.sha256(raw).hexdigest().upper();text=raw.decode().replace('\r\n','\n');a=text.index(start);return text[a:text.index(end,a)]
body=cut('standalone-runtime.cpp','preset117::EmergencyPolicy125 CurrentEmergencyPolicy125()','uint32_t ProofKey()')
body+=cut('police-nearby.inc','void UpdateNearby()',' NearbyLock lock;')+'\n}\n'
body+=cut('standalone-runtime.cpp','void UpdateProof(){',' if(proofRetired.load())')+'\n}\n'
body+='namespace TrafficSignal {\n'+cut('traffic-signal-observer.inc','void Poll() {',' if(!registry.Disabled()&&enabled.load())for')+'\n}\n}\n'
body+='void ConfigurePublic125(){\n'+cut('standalone-runtime.cpp','  if(preset117::startup.present){\n   const auto p=preset117::Requests','  PlayerShadowAllocation::additionalFeatures=PlayerShadowAllocation::additionalFeatures||')+'\n}\n'
output.write_text('// Actual production recurring config prefixes; native owner/lifetime bodies are unchanged and outside this fixture.\n'+body)
for name in ['RuntimeFeaturePolicy125.hpp','PresetGeneration117.hpp','PresetIni117.hpp']:inputs[name]=hashlib.sha256((source/name).read_bytes()).hexdigest().upper()
output.with_suffix('.inputs.json').write_text(json.dumps(inputs,indent=2))
