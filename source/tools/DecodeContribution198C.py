"""Read existing ShadowTrace34 binary segments; never writes a game path."""
from pathlib import Path
import argparse,collections,json,math,re,struct
EVENT=struct.Struct('<4I4i4f')
def read_trace(path):
 data=Path(path).read_bytes()
 if len(data)<32:raise ValueError('short trace header')
 header=struct.unpack_from('<8I',data)
 if header[:4]!=(0x34335453,1,32,1) or header[6]!=EVENT.size:raise ValueError('unsupported trace format')
 if (len(data)-32)%EVENT.size:raise ValueError('partial trace event; do not silently discard it')
 base=re.sub(r'\.part[0-9]+$','',str(Path(path).resolve()))
 return [dict(zip(('stage','frame','tick','key','a','b','c','d','x','y','z','w'),EVENT.unpack_from(data,p)),
  _trace_identity=(header[4],header[5]),_trace_base=base) for p in range(32,len(data),EVENT.size)]
def summarize(events):
 identities={e['_trace_identity'] for e in events if '_trace_identity' in e}
 bases={e['_trace_base'] for e in events if '_trace_base' in e}
 clips={e['b'] for e in events if e['stage']==240}
 if len(identities)>1 or len(bases)>1 or len(clips)>1:
  raise ValueError('inputs must be one trace capture and its .partN segments, with one PID/mode/clip')
 census=collections.defaultdict(list);rows=collections.defaultdict(lambda:collections.defaultdict(list));starts=collections.defaultdict(list);ends=[]
 unknown_passes=selection_limits=render_limits=0
 for e in events:
  stage=e['stage']
  if stage==240:census[(e['frame'],e['a'])].append(e)
  elif stage in (241,242,243,248):
   serial=e['d'] if stage in (241,242) else e['c'];rows[(e['frame'],serial,e['key'])][stage].append(e)
  elif stage==244:starts[(e['frame'],e['d'],e['key'],e['a'],e['b'])].append(e)
  elif stage==245:ends.append(e)
  elif stage==246:unknown_passes+=1
  elif stage==247:selection_limits+=1
  elif stage==249:render_limits+=1
 classes=collections.Counter();complete={};incomplete=0;slots=collections.defaultdict(set)
 for identity,bundle in rows.items():
  frame,serial,key=identity;heads=census.get((frame,serial),[])
  if len(heads)!=1 or any(len(bundle.get(stage,[]))!=1 for stage in (241,242,243,248)):
   incomplete+=1;continue
  head=heads[0];position=bundle[241][0];meta=bundle[243][0];shape=bundle[248][0];slot=position['a']
  if not 0<=slot<7 or not head['c']&(1<<slot) or meta['d']!=head['key'] or shape['d']!=head['key']:
   incomplete+=1;continue
  complete[identity]=position;classes[str(position['c'])]+=1;slots[(frame,serial)].add(slot)
 expected=sum((h[0]['c']&127).bit_count() for h in census.values() if len(h)==1)
 missing=max(0,expected-sum(len(v) for v in slots.values()))
 joined=[];unjoinable=0;seen=set();joined_rows=set()
 for e in ends:
  identity=(e['frame'],e['d'],e['key']);entry=complete.get(identity);match=(e['frame'],e['d'],e['key'],e['a'],e['b'])
  if not entry or len(starts.get(match,[]))!=1 or match in seen or e['a']!=entry['a']+1 or e['c']!=entry['c'] or not math.isfinite(e['x']) or e['x']<0:
   unjoinable+=1;continue
  start=starts[match][0]
  if start['c']!=e['c']:unjoinable+=1;continue
  seen.add(match);joined_rows.add(identity)
  joined.append({'frame':e['frame'],'selection_serial':e['d'],'key':e['key'],'native_slot':e['a'],'native_kind':e['b'],'class':e['c'],'inclusive_cpu_us':e['x'],'nested':e['y']!=0})
 outside=[e for e in joined if e['class']==2 and not e['nested']]
 return {'trace_mode_pid':list(next(iter(identities))) if identities else None,'capture_tick':next(iter(clips)) if clips else None,
  'selected_censuses':len(census),'complete_selected_rows':len(complete),'rows_by_class':dict(classes),
  'incomplete_or_ambiguous_row_groups':incomplete,'selected_slots_without_complete_record':missing,
  'completed_native_pass_joins':len(joined),'unjoinable_completion_events':unjoinable,
  'unjoined_native_scopes_unknown':unknown_passes,'selected_rows_without_completed_join_unknown':len(complete)-len(joined_rows),
  'selection_budget_markers':selection_limits,'render_budget_markers':render_limits,
  'outside_fresh_main_view_nonnested_passes':len(outside),'outside_fresh_main_view_nonnested_inclusive_cpu_us':sum(e['inclusive_cpu_us'] for e in outside),
  'joined_native_passes':joined,'class_legend':{'0':'unknown view','1':'volume may reach main view','2':'volume outside fresh main view','3':'vehicle excluded from scenery analysis','4':'invalid geometry','5':'ambiguous key'},
  'limits':'Diagnostic numeric correlation and inclusive CPU submission timing only. No GPU/pixel proof, no culling authority, no whole-view-set exclusion. No completed join means unknown, never unused. Deferred identity, source generations after selection, incomplete/dropped trace records, and exhausted budgets can prevent joins. Check recorder status dropped count; diagnostic timing is not clean FPS.'}
def main():
 parser=argparse.ArgumentParser();parser.add_argument('trace',nargs='+');parser.add_argument('--out');args=parser.parse_args()
 events=[]
 for path in args.trace:events.extend(read_trace(path))
 text=json.dumps(summarize(events),indent=2,allow_nan=False)
 if args.out:Path(args.out).write_text(text)
 else:print(text)
if __name__=='__main__':main()
