"""Apply the A1 attribution source changes to the matching CE 1.5 / legacy 1.1 source."""
from pathlib import Path
import argparse,json,hashlib
parser=argparse.ArgumentParser()
parser.add_argument('edition',choices=['CE','1080'])
parser.add_argument('source_root',type=Path)
args=parser.parse_args()
root=args.source_root.resolve();here=Path(__file__).resolve().parent
entries=json.loads((here/'source-notices.json').read_text(encoding='utf-8'))[args.edition]
sha=lambda b:hashlib.sha256(b).hexdigest()
pending=[]
for e in entries:
 p=(root/e['path']).resolve()
 if not p.is_relative_to(root):raise ValueError('Path outside source root')
 old=p.read_bytes()
 if sha(old)==e['afterSHA256']:continue
 if sha(old)!=e['beforeSHA256']:raise ValueError(f'Wrong source revision: {p}. No changes written.')
 new=e['replacement'].encode(e['encoding']) if 'replacement' in e else e['prefix'].encode()+old
 if sha(new)!=e['afterSHA256']:raise ValueError('Manifest output mismatch')
 pending.append((p,new))
for p,new in pending:p.write_bytes(new)
(root/'ATTRIBUTION.md').write_bytes((here/'ATTRIBUTION.md').read_bytes())
print(f'Applied {len(pending)} attribution changes. Existing upstream notices retained; gameplay source unchanged.')