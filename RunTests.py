"""Run built regression checks with fresh, independent working directories."""
from pathlib import Path
import argparse,datetime,json,os,re,subprocess,time
parser=argparse.ArgumentParser()
parser.add_argument('--build',type=Path,default=Path(__file__).resolve().parent/'build')
parser.add_argument('--ctest',default='ctest')
parser.add_argument('--filter',default='.*',help='Regular expression matching test names')
args=parser.parse_args();build=args.build.resolve()
tests=json.loads(subprocess.run([args.ctest,'--test-dir',str(build),'--show-only=json-v1'],capture_output=True,text=True,check=True).stdout)['tests']
tests=[t for t in tests if re.search(args.filter,t['name'])]
if not tests:raise SystemExit('No matching tests.')
runs=build/'test-results'/datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%S%fZ');runs.mkdir(parents=True)
results=[]
for t in tests:
 cwd=runs/t['name'];cwd.mkdir();start=time.perf_counter()
 p=subprocess.run(t['command'],cwd=cwd,text=True,capture_output=True,timeout=90,env={**os.environ,'PYTHONDONTWRITEBYTECODE':'1'})
 (cwd/'console.txt').write_text(p.stdout+p.stderr,encoding='utf-8')
 results.append(dict(name=t['name'],returncode=p.returncode,seconds=time.perf_counter()-start))
 print(('PASS ' if p.returncode==0 else 'FAIL ')+t['name'],flush=True)
(runs/'results.json').write_text(json.dumps(results,indent=2))
failed=sum(t['returncode']!=0 for t in results)
print(str(len(tests)-failed)+'/'+str(len(tests))+' passed. Results: '+str(runs))
raise SystemExit(bool(failed))
