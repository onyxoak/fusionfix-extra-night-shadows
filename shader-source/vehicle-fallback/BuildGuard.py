from pathlib import Path
import ctypes as c, hashlib, json, re, struct, sys
J=Path(__file__).parent;R=J.parent;sys.path.insert(0,str(J/'tools'))
from inspect_fxc18 import Fxc
from disassemble_fxc18 import listing
base=J/'output/deferred_lighting.fxc'
assert hashlib.sha256(base.read_bytes()).hexdigest()=='6f17f3f64a20d534d646c8c531198fbd3e019166b41aa6ce4729b4318e429ebb'
fx=Fxc(base);meta=fx.read()
d=c.WinDLL('d3dcompiler_47.dll');assemble=d.D3DAssemble
assemble.argtypes=[c.c_void_p,c.c_size_t,c.c_char_p,c.c_void_p,c.c_void_p,c.c_uint,c.POINTER(c.c_void_p),c.POINTER(c.c_void_p)];assemble.restype=c.c_long
def blob(p):
 v=c.cast(p,c.POINTER(c.POINTER(c.c_void_p))).contents
 ptr=c.WINFUNCTYPE(c.c_void_p,c.c_void_p)(v[3])(p);size=c.WINFUNCTYPE(c.c_size_t,c.c_void_p)(v[4])(p)
 data=c.string_at(ptr,size);c.WINFUNCTYPE(c.c_ulong,c.c_void_p)(v[2])(p);return data
def compile(text):
 data=text.encode('ascii');buf=c.create_string_buffer(data);out=c.c_void_p();error=c.c_void_p()
 status=assemble(buf,len(data),b'OnyxOak-MrBulkinAleksey-fallback227-zero-light-guard',None,None,0,c.byref(out),c.byref(error))
 message=blob(error).decode(errors='replace') if error else '';assert status>=0,message
 return blob(out)
selected=sorted({p['ps']-1 for t in meta['techniques'] if t['name'] in ('lightVolumePoint','fillerVolumePoint','lightVolumeShadowPoint','fillerVolumeShadowPoint','lightVolumeTexPoint','lightVolumeShadowTexPoint') for p in t['passes']})
assert len(selected)==11
replacements={};details=[];outAsm=J/'guard-assembly';outAsm.mkdir(exist_ok=True)
for index in selected:
 f=meta['pixel'][index];raw=fx.data[f['offset']:f['offset']+f['size']];text=listing(raw)
 # Restore the original author's guard, adapting his radiance register r26
 # to this port's preserved final native radiance r28. No box/ray math changes.
 pattern=r'(?m)^([ ]*)mov r12, c149\r?\n[ ]*if_lt c138.x, r12.w'
 def guard(m):
  pad=m[1]
  return '\n'.join(pad+line for line in ['mov r12, c149','max r13.x, r28_abs.x, r28_abs.y','max r13.x, r13.x, r28_abs.z','cmp r13.x, -r13.x, c138.x, r12.w','cmp r13.x, -r29.w, r12.w, r13.x','if_lt c138.x, r13.x'])
 optimized,n=re.subn(pattern,guard,text);assert n==1,(index,n)
 (outAsm/f'pixel{index}.before.asm').write_text(text);(outAsm/f'pixel{index}.after.asm').write_text(optimized)
 compiled=compile(optimized);assert len(compiled)<65536
 replacements[index]=compiled;details.append(dict(pixel=index,before_bytes=len(raw),after_bytes=len(compiled),native_output_zero_guard=True,ray_box_algorithm_unchanged=True,sha256=hashlib.sha256(compiled).hexdigest()))
parts=[];position=0
for index,new in sorted(replacements.items()):
 f=meta['pixel'][index]
 parts.extend((fx.data[position:f['offset']-4],struct.pack('<HH',len(new),len(new)),new));position=f['offset']+f['size']
parts.append(fx.data[position:]);out=J/'output/deferred_lighting.fxc';out.parent.mkdir(exist_ok=True);out.write_bytes(b''.join(parts))
changed=Fxc(out);after=changed.read()
assert after['globals']==meta['globals'] and after['locals']==meta['locals']
def techniques(m):
 return [(t['name'],[(p['vs'],p['ps'],[(s['state'],s['value']) for s in p['states']]) for p in t['passes']]) for t in m['techniques']]
assert techniques(after)==techniques(meta)
for kind in ['vertex','pixel']:
 for i,(a,b) in enumerate(zip(meta[kind],after[kind])):
  assert a['bindings']==b['bindings'] and b['size']==b['secondary']
  if kind=='vertex' or i not in replacements:
   assert fx.data[a['offset']:a['offset']+a['size']]==changed.data[b['offset']:b['offset']+b['size']]
sha=hashlib.sha256(out.read_bytes()).hexdigest()
(J/'output/guard-verification.json').write_text(json.dumps(dict(path=str(out),sha256=sha,base_sha256=hashlib.sha256(base.read_bytes()).hexdigest(),fragments=details,techniques_and_bindings_unchanged=True,both_resource_lengths_verified=True,unmodified_fragments_identical=True,game_pixels_verified=False,performance_verified=False,credit='MrBulkinAleksey original zero-contribution guard and rounded-box method'),indent=2))
assert sha=='86db809154f3b1249bb4bb6ea3adaa388ff40ae5c91bb232cedc215eb09e1ba4', 'Paired shader digest differs'
print(json.dumps(dict(path=str(out),sha256=sha,changed_pixel_shaders=len(details),metadata_verified=True),indent=2))
