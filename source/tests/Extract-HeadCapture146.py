from pathlib import Path
import sys
source=Path(sys.argv[1]);out=Path(sys.argv[2]);text=(source/'core/ShadowCasterRuntime.inl').read_text();body=text.split('namespace ShadowDiagnostics')[0];assert 'singleWitness146' in body;out.write_text(body.replace('namespace OwnHeadlightCaster','namespace Candidate146',1))
