from pathlib import Path
import importlib.util, struct, tempfile, sys
sys.dont_write_bytecode=True

path=Path(__file__).parents[1]/'tools/Decode-EffectAttribution199C.py'
spec=importlib.util.spec_from_file_location('decoder',path); module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
header=struct.pack('<8I',0x34335453,1,32,1,44,123,48,0)
def event(stage,nonce,payload,frame=42): return struct.pack('<12I',stage,frame,1000,nonce,*payload)
marker=event(61020,1,[1,1,0x6bd29f,128,8,2048,0,0],0)
packet=[0]*32;packet[17]=0;packet[18]=0x261
def record(stage,nonce,p,epoch=1,vl=0):
    meta=[5,0x12345,0x6bcccb,0x200000,0x6bcc50,0,epoch,vl]
    return event(stage,nonce,meta)+b''.join(event(stage+1+i,nonce,p[i*8:i*8+8]) for i in range(4))
with tempfile.TemporaryDirectory() as td:
    f=Path(td)/'capture.st34'
    def decode(data):
        f.write_bytes(header+data);return module.decode([f])
    a=record(61000,1,packet); b=record(61010,2,packet)
    r=decode(marker+a+b);assert r['match_counts']=={'observed_unique_correlation':1} and r['authority']=='unknown'
    assert r['caller_cohorts'][0]['native_class']=='0x200' and r['caller_cohorts'][0]['distinct_values']['origin']==1
    moved=packet[:];moved[8]=0x3f800000
    r=decode(marker+a+record(61000,3,moved));assert r['caller_cohorts'][0]['distinct_geometry_packets']==2 and r['caller_cohorts'][0]['consecutive_observation_changes']['origin']==1
    keyed=packet[:];keyed[24]=0x10000000
    r=decode(marker+a+record(61010,2,keyed));assert r['matches'][0]['result']=='no_observed_match' and r['matches'][0]['geometry_only_candidates'][0]['changed_dwords']==[24]
    r=decode(marker+a+record(61000,3,packet)+b);assert r['match_counts']=={'ambiguous':1}
    r=decode(marker+b);assert r['match_counts']=={'no_observed_match':1}
    r=decode(a+b);assert not r['records'] and r['incomplete_or_collision_records']==2
    r=decode(marker+a[:-48]+b);assert r['incomplete_or_collision_records']==1
    r=decode(marker+a+a+b);assert r['incomplete_or_collision_records']==1 and r['match_counts']=={'no_observed_match':1}
    r=decode(marker+a+b+b'X');assert r['trailing_fragments']==1
    p=packet[:];p[20]=0xdead;q=p[:];q[18]|=8;q[28]=123;q[29]=456
    r=decode(marker+record(61000,1,p,vl=1)+record(61010,2,q,vl=1));assert r['matches'][0]['candidates'][0]['comparison']=='authenticated_volume_metadata_only'
    r=decode(marker+record(61000,1,p)+record(61010,2,q,vl=1));assert r['match_counts']=={'no_observed_match':1}
    for index in range(30):
        if index in (18,28,29):continue
        changed=q[:];changed[index]^=1
        r=decode(marker+record(61000,1,p,vl=1)+record(61010,2,changed,vl=1))
        assert not r['matches'] or r['matches'][0]['result']=='no_observed_match'
print('199c decoder: complete, partial, collision, absent epoch, duplicate, metadata-only and no-authority cases passed')
