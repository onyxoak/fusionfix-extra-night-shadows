from pathlib import Path
import sys,re,ctypes as c,struct,hashlib,json
job=Path(__file__).parent;root=job.parent
sys.path.insert(0,str(job/'tools'))
from inspect_fxc18 import Fxc
from disassemble_fxc18 import listing
original=job/'deferred_lighting-base.fxc'
fx=Fxc(original);meta=fx.read()
assert hashlib.sha256(fx.data).hexdigest()=='2c2c8711e477d0ba2ceb9bdc493fc058f8f0bb3c0d497c8338d0e0e677631cfa'
patch=(job/'../../source/vendor/MrBulkinAleksey/local_light_vehicle_box_shadows.patch').read_text()
section=patch.split('diff --git ')[1]
adds='\n'.join(line[1:] for line in section.splitlines() if line.startswith('+') and not line.startswith('+++'))
routine=adds[adds.index('    label l3'):]+ '\n    ret\n'
assert all('label l'+str(n) in routine for n in (2,3,4))
defs='\n'.join(line for line in adds.splitlines() if line.strip().startswith('def c'))
d=c.WinDLL('d3dcompiler_47.dll');assemble=d.D3DAssemble
assemble.argtypes=[c.c_void_p,c.c_size_t,c.c_char_p,c.c_void_p,c.c_void_p,c.c_uint,c.POINTER(c.c_void_p),c.POINTER(c.c_void_p)];assemble.restype=c.c_long
def blob(p):
    v=c.cast(p,c.POINTER(c.POINTER(c.c_void_p))).contents
    at=c.WINFUNCTYPE(c.c_void_p,c.c_void_p)(v[3])(p);size=c.WINFUNCTYPE(c.c_size_t,c.c_void_p)(v[4])(p)
    data=c.string_at(at,size);c.WINFUNCTYPE(c.c_ulong,c.c_void_p)(v[2])(p);return data
def compile(text):
    data=text.encode('ascii');buffer=c.create_string_buffer(data);out=c.c_void_p();error=c.c_void_p()
    status=assemble(buffer,len(data),b'OnyxOak-MrBulkinAleksey-vehicle-box216',None,None,0,c.byref(out),c.byref(error))
    message=blob(error).decode(errors='replace') if error else ''
    assert status>=0,message
    return blob(out)
selected=sorted({p['ps']-1 for t in meta['techniques'] if t['name'] in ('lightVolumePoint','fillerVolumePoint','lightVolumeShadowPoint','fillerVolumeShadowPoint','lightVolumeTexPoint','lightVolumeShadowTexPoint') for p in t['passes']})
replacements={};details=[];asm=job/'assembly';asm.mkdir(exist_ok=True)
for index in selected:
    frag=meta['pixel'][index];raw=fx.data[frag['offset']:frag['offset']+frag['size']];text=listing(raw)
    for reg in (138,185,186,187,188,189,143,144,145,146,147,148,149,150):assert not re.search(r'\bc'+str(reg)+r'\b',text),(index,reg)
    assert not re.search(r'\br28\b|\blabel\b|\bcall\b',text),index
    bindings={b['name']:b['index'] for b in frag['bindings']}
    sampler=bindings['GBufferTextureSampler3'];screen=bindings['gooDeferredLightScreenSize'];light=bindings['gDeferredLightPosition']
    prefix=f'''    mov r17.w, c138.y
    mov r29, c150
    if_eq r29.x, c190.x
    if_eq r29.y, c190.y
      mov r12, c149
      if_lt c138.x, r12.w
        add r13.xy, vPos, c138.z
        mul r13.xy, r13, c{screen}.zw
        mov r13.zw, c138.x
        texldl r14, r13, s{sampler}
        pow r19.x, c209_abs.z, r14.x
        mul r14.x, r19.x, c209.w
        rcp r14.y, v0.w
        mul r14.y, r14.y, r14.x
        mad r15.xyz, v0, -r14.y, c15
        add r16.xyz, c{light}, -r15
        mul r16.xy, r16, r12.z
        mul r13.w, r12.y, c138.z
        frc r12.y, r13.w
        add r13.w, r13.w, -r12.y
        add r12.y, r12.y, r12.y
        mul r13.w, r13.w, c185.w
        mov r17.x, c138.y
'''
    # The original fork uses a separately bound material-ID texture. The CE
    # standalone adaptation instead exempts receivers inside the car above its
    # foot; the original foot/rounded-cabin test remains, with no new sampler.
    for a,b in ((143,144),(145,146),(147,148)):
        prefix+=f'        mov r18, c{a}\n        if_lt c189.y, r18.w\n          mov r19, c{b}\n          call l3\n        endif\n'
    prefix+='''        add r17.w, r17.w, -c138.y
        mad r17.w, r17.w, r12.w, c138.y
      endif
    endif
    endif
    mul oC0.xyz, r28, r17.w
    mov oC0.w, r28.w
    ret
'''
    # Existing outputs and depth execute first; only the final RGB is attenuated.
    body=text.split('// approximately')[0]
    body=re.sub(r'\boC0\b','r28',body)
    body=body.replace('    ps_3_0','    ps_3_0\n'+defs+'\n    def c190, 216, -216, 0, 0',1)
    assert not re.search(r'\bc190\b',text)
    body+='\n'+prefix+routine
    (asm/f'pixel{index}.asm').write_text(body)
    compiled=compile(body);assert len(compiled)<65536
    replacements[index]=compiled;details.append(dict(index=index,before_bytes=len(raw),after_bytes=len(compiled),depth_sampler=sampler,screen_register=screen,light_register=light,sha256=hashlib.sha256(compiled).hexdigest()))
parts=[];position=0
for index,new in sorted(replacements.items()):
    f=meta['pixel'][index];parts.extend((fx.data[position:f['offset']-4],struct.pack('<HH',len(new),len(new)),new));position=f['offset']+f['size']
parts.append(fx.data[position:]);out=job/'output/deferred_lighting.fxc';out.parent.mkdir(exist_ok=True);out.write_bytes(b''.join(parts))
changed=Fxc(out);m2=changed.read()
assert m2['globals']==meta['globals'] and m2['locals']==meta['locals']
assert [(t['name'],[(p['vs'],p['ps'],[(s['state'],s['value']) for s in p['states']]) for p in t['passes']]) for t in m2['techniques']]==[(t['name'],[(p['vs'],p['ps'],[(s['state'],s['value']) for s in p['states']]) for p in t['passes']]) for t in meta['techniques']]
for kind in ('vertex','pixel'):
    for i,(a,b) in enumerate(zip(meta[kind],m2[kind])):
        assert a['bindings']==b['bindings']
        assert b['size']==b['secondary'],(kind,i,b['size'],b['secondary'])
        if kind=='vertex' or i not in replacements:assert fx.data[a['offset']:a['offset']+a['size']]==changed.data[b['offset']:b['offset']+b['size']]
digest=hashlib.sha256(out.read_bytes()).hexdigest()
(job/'output/verification.json').write_text(json.dumps(dict(path=str(out),sha256=digest,source_sha256=meta['sha256'],patched=details,all_other_fragments_and_metadata_unchanged=True,origin_commit='16318f97a79939c53bb0abda1c6aa247dc6078e0',limits='Assembled SM3 and structural checks, not GPU pixel or performance validation. Native-light constants gate the additional code; receiver material filter adapted to box/foot position.'),indent=2))
print(json.dumps(dict(shader=str(out),sha256=digest,fragments=len(details),bytes=out.stat().st_size),indent=2))
assert digest=='6f17f3f64a20d534d646c8c531198fbd3e019166b41aa6ce4729b4318e429ebb', 'Base fallback shader digest differs'
import subprocess
subprocess.run([sys.executable,str(job/'BuildGuard.py')],check=True)
