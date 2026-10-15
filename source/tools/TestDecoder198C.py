from DecodeContribution198C import summarize,EVENT,read_trace
from pathlib import Path
import struct
def event(stage,key=100,a=0,b=0,c=0,d=0,x=0.,y=0.):return dict(zip(('stage','frame','tick','key','a','b','c','d','x','y','z','w'),(stage,40,50,key,a,b,c,d,x,y,0.,0.)))
events=[event(240,key=9,a=1,b=10,c=1,d=1),event(241,c=2,d=1),event(242,d=1),event(243,c=1,d=9),event(248,c=1,d=9),event(244,a=1,b=3,c=2,d=1),event(245,a=1,b=3,c=2,d=1,x=12.5)]
r=summarize(events);assert r['completed_native_pass_joins']==1 and r['outside_fresh_main_view_nonnested_inclusive_cpu_us']==12.5
for missing in (0,1,2,3,4,5):assert summarize(events[:missing]+events[missing+1:])['completed_native_pass_joins']==0
r=summarize(events+[event(246),event(247),event(249)]);assert r['unjoined_native_scopes_unknown']==1 and r['selection_budget_markers']==r['render_budget_markers']==1
assert summarize(events+[events[1]])['completed_native_pass_joins']==0
rows=Path.cwd()/'decoder-fixture198C.bin';rows.write_bytes(struct.pack('<8I',0x34335453,1,32,1,1,9,48,0)+b''.join(EVENT.pack(*e.values()) for e in events))
assert summarize(read_trace(rows))['completed_native_pass_joins']==1
for field,value in (('_trace_identity',(1,10)),('_trace_base','different-capture')):
 annotated=read_trace(rows);other=dict(annotated[-1]);other[field]=value
 try:summarize(annotated+[other]);assert False,'mixed trace accepted'
 except ValueError:pass
try:summarize(events+[event(240,key=9,a=1,b=11,c=1,d=1)]);assert False,'mixed clip accepted'
except ValueError:pass
print('decoder198C: complete native join, missing components, duplicate ambiguity, unknown scopes, exhausted budgets, actual trace binary parsing passed; synthetic only')
