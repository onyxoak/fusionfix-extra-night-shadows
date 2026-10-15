from pathlib import Path
import sys,ctypes,struct,hashlib,json,re
job=Path(__file__).parent;root=job.parent.parent
sys.path.insert(0,str(root/'tools'))
from inspect_fxc18 import Fxc
from disassemble_fxc18 import listing
def digest(b):return hashlib.sha256(b).hexdigest()
def blob_bytes(blob):
 table=ctypes.cast(blob,ctypes.POINTER(ctypes.POINTER(ctypes.c_void_p))).contents
 pointer=ctypes.WINFUNCTYPE(ctypes.c_void_p,ctypes.c_void_p)(table[3])(blob)
 size=ctypes.WINFUNCTYPE(ctypes.c_size_t,ctypes.c_void_p)(table[4])(blob)
 result=ctypes.string_at(pointer,size);ctypes.WINFUNCTYPE(ctypes.c_ulong,ctypes.c_void_p)(table[2])(blob);return result
compiler=ctypes.WinDLL('d3dcompiler_47.dll');assemble=compiler.D3DAssemble
assemble.argtypes=[ctypes.c_void_p,ctypes.c_size_t,ctypes.c_char_p,ctypes.c_void_p,ctypes.c_void_p,ctypes.c_uint,ctypes.POINTER(ctypes.c_void_p),ctypes.POINTER(ctypes.c_void_p)]
assemble.restype=ctypes.c_long
def compiled(text):
 raw=text.encode('ascii');buffer=ctypes.create_string_buffer(raw);shader,errors=ctypes.c_void_p(),ctypes.c_void_p()
 status=assemble(buffer,len(raw),b'Libertys-Shadows-fade198B',None,None,0,ctypes.byref(shader),ctypes.byref(errors))
 error=blob_bytes(errors).decode('ascii',errors='replace') if errors else ''
 if status<0:raise RuntimeError(error)
 return blob_bytes(shader)
source=Path(r'D:\SteamLibrary\steamapps\common\Grand Theft Auto IV\GTAIV\update\common\shaders\win32_30\deferred_lighting.fxc')
fxc=Fxc(source);meta=fxc.read();assert digest(fxc.data)=='534e1f863ef9555b591afd205556b2fc40506c14ec86634a7b7df14b1ce7277b'
out=job/'shader';out.mkdir(exist_ok=True);(out/'deferred_lighting-baseline195.fxc').write_bytes(fxc.data)
changes=[];patches=[]
for index,visibility,anchor in [(13,'r0.x','    dp3 r0.y, r2, r2'),(14,'r0.w','    add r2.xyz, r1, c0.w'),(16,'r1.x','    mul r0.xyz, r0, r1.x')]:
 item=meta['pixel'][index];text=listing(fxc.data[item['offset']:item['offset']+item['size']])
 assert not re.search(r'\bc114\b|\br30\b|\br29\b',text)
 anchor='    endif\n'+anchor
 assert text.count(anchor)==1
 # Only a paired native binder marks this source with x=-198-strength.
 # Every ordinary native x (including negative values outside[-199,-198])
 # resolves to strength1. The original visibility1 remains1 at every strength.
 code='\n'.join(['    mov r30.w, c76.x','    add r30.x, c114.x, -r30.w','    add r30.y, r30.w, -c114.y','    cmp r30.z, r30.x, c114.z, c114.w','    cmp r30.y, r30.y, c114.z, c114.w','    mul r30.y, r30.y, r30.z','    lrp r29.x, r30.y, r30.x, c114.z',f'    lrp {visibility}, r29.x, {visibility}, c114.z'])+'\n'
 text=text.replace('    ps_3_0','    ps_3_0\n    def c114, -198, -199, 1, 0').replace(anchor,'    endif\n'+code+anchor.split('\n',1)[1])
 raw=compiled(text);assert len(raw)<65536
 def instructions(value):return [line.strip() for line in value.splitlines() if line.strip() and not line.strip().startswith('//')]
 old_lines=instructions(listing(fxc.data[item['offset']:item['offset']+item['size']]))
 new_lines=[line for line in instructions(listing(raw)) if not re.search(r'\bc114\b|\br29\b|\br30\b',line)]
 assert old_lines==new_lines # Original instruction order and operands exact.
 (out/f'deferred-pixel{index}-fade198B.asm').write_text(text)
 (out/f'deferred-pixel{index}-compiled198B.asm').write_text(listing(raw))
 # The same bindings/techniques/materials survive; replace only size+bytecode.
 patches.append((item['offset']-4,item['offset']+item['size'],struct.pack('<HH',len(raw),len(raw))+raw))
 changes.append({'pixel':index,'baseline_sha256':item['sha256'],'candidate_sha256':digest(raw),'bytes':len(raw),'extra_arithmetic_instructions':8})
result=fxc.data
for start,end,data in reversed(patches):result=result[:start]+data+result[end:]
destination=out/'deferred_lighting.fxc';destination.write_bytes(result);candidate=Fxc(destination).read()
assert result[candidate['variables_offset']:]==fxc.data[meta['variables_offset']:]
for category in ['vertex','pixel']:
 assert len(candidate[category])==len(meta[category])
 for i,(a,b) in enumerate(zip(meta[category],candidate[category])):
  assert a['bindings']==b['bindings']
  if category=='vertex' or i not in [13,14,16]:assert a['sha256']==b['sha256']
# Shader equation checks include exactly unchanged unoccluded lighting and
# baseline behavior for every ordinary/unmarked source, including vehicle draws.
def apply(visibility,x):
 strength=(-198-x) if -199<=x<=-198 else 1
 return strength*visibility+(1-strength)
for x in [0,.001,-.1,1,198,-200,-197.999]:
 for visibility in [0,.25,.5,.75,1]:assert apply(visibility,x)==visibility
for strength in [0,.25,.5,.75,1]:
 assert apply(1,-198-strength)==1
 assert apply(0,-198-strength)==1-strength
record={'input':str(source),'input_sha256':digest(fxc.data),'output':str(destination),'output_sha256':digest(result),'changed_pixels':changes,'other_fragments_and_techniques_identical':True,'equation_tests':True,'rendered_pixels_verified':False,'installed':False}
(out/'identity.json').write_text(json.dumps(record,indent=2));print(json.dumps(record,indent=2))
