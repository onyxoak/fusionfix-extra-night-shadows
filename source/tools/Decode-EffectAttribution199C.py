"""Decode bounded diagnostic observations, never assign native owner authority."""
from pathlib import Path
from collections import defaultdict, Counter
import argparse, json, struct

SOURCE, BANK, STATUS = 61000, 61010, 61020
GEOMETRY = (0,1,2,4,5,6,8,9,10,21,22,23)

def geometry(p):
    return tuple(p[i] for i in GEOMETRY)

def cohort_report(records):
    """One caller can emit many lights; distinct values do not prove owner motion."""
    cohorts = defaultdict(list)
    for r in records:
        if r['kind'] == 'source':
            p = r['packet'];m = r['metadata']
            cohorts[(r['epoch'],m[1],m[2],p[17],p[18]&0x300)].append(r)
    output=[]
    for (epoch,producer,parent,kind,domain),rows in sorted(cohorts.items()):
        rows.sort(key=lambda r:r['nonce']); packets=[r['packet'] for r in rows]
        groups={'direction':(0,1,2),'tangent':(4,5,6),'origin':(8,9,10),'radius':(21,),
                'cone':(22,23),'flags':(18,),'key':(24,),'color':(12,13,14),'intensity':(16,)}
        output.append(dict(epoch=epoch,producer_return_rva=hex(producer),parent_return_rva=hex(parent),
            native_type=kind,native_class=hex(domain),observations=len(rows),
            flags=dict(Counter(hex(p[18]) for p in packets)),
            distinct_geometry_packets=len({geometry(p) for p in packets}),
            distinct_values={name:len({tuple(p[i] for i in fields) for p in packets}) for name,fields in groups.items()},
            consecutive_observation_changes={name:sum(any(a[i]!=b[i] for i in fields) for a,b in zip(packets,packets[1:])) for name,fields in groups.items()},
            nonzero_keys=len({p[24] for p in packets if p[24]}),
            interpretation='Observed caller cohort only; multiple emitters may share this site. No owner motion, token issuance, key waste, or leak is established.'))
    return output

def normalized(p):
    if p[17] not in (0, 2) or p[20] != 0xDEAD:
        return None
    q = list(p); q[18] &= ~8; q[28] = q[29] = 0
    return tuple(q)

def decode(paths):
    groups = defaultdict(dict); bad = set(); epochs = set(); fragments = 0
    pid = None
    for path in paths:
        raw = Path(path).read_bytes()
        if len(raw) < 32:
            raise ValueError('truncated ST34 header')
        h = struct.unpack_from('<8I', raw)
        if h[:5] != (0x34335453, 1, 32, 1, 44) or h[6:] != (48, 0):
            raise ValueError('unsupported capture header')
        if pid is not None and pid != h[5]:
            raise ValueError('different process captures cannot be combined')
        pid = h[5]
        fragments += (len(raw) - 32) % 48 != 0
        for offset in range(32, len(raw) - 47, 48):
            e = struct.unpack_from('<12I', raw, offset)
            stage, frame, tick, nonce = e[:4]
            if stage == STATUS:
                if e[4:10] == (nonce, 1, 0x6BD29F, 128, 8, 2048):
                    epochs.add(nonce)
                continue
            base = SOURCE if SOURCE <= stage <= SOURCE + 4 else BANK if BANK <= stage <= BANK + 4 else 0
            if not base:
                continue
            key = (base, nonce)
            if stage in groups[key]:
                bad.add(key)
            groups[key][stage] = e
    records = []
    for key, parts in groups.items():
        base, nonce = key
        if key in bad or len(parts) != 5:
            bad.add(key); continue
        ordered = [parts[base + i] for i in range(5)]
        if any(e[1:4] != ordered[0][1:4] for e in ordered):
            bad.add(key); continue
        meta = ordered[0][4:]; packet = sum((e[4:] for e in ordered[1:]), ())
        if meta[6] not in epochs or packet[30:] != (0, 0) or not nonce:
            bad.add(key); continue
        if packet[17] not in (0, 2) or packet[18] & 0x300 != 0x200:
            bad.add(key); continue
        records.append(dict(kind='source' if base == SOURCE else 'bank', nonce=nonce,
                            frame=ordered[0][1], tick=ordered[0][2], epoch=meta[6],
                            metadata=list(meta), packet=list(packet[:30]),native_type=packet[17],
                            native_class=hex(packet[18]&0x300),flags=hex(packet[18]),key=hex(packet[24])))
    exact = defaultdict(list); volume = defaultdict(list); geometries = defaultdict(list)
    for r in records:
        if r['kind'] != 'source':
            continue
        exact[(r['epoch'], r['frame'], tuple(r['packet']))].append(r)
        geometries[(r['epoch'],r['frame'],r['packet'][17],r['packet'][18]&0x300,geometry(r['packet']))].append(r)
        n = normalized(r['packet'])
        if r['metadata'][7] == 1 and n:
            volume[(r['epoch'], r['frame'], n)].append(r)
    matches = []
    for r in records:
        if r['kind'] != 'bank':
            continue
        found = {s['nonce']: (s, 'exact_120_bytes') for s in exact[(r['epoch'], r['frame'], tuple(r['packet']))]}
        n = normalized(r['packet'])
        if r['metadata'][7] == 1 and n:
            for s in volume[(r['epoch'], r['frame'], n)]:
                found.setdefault(s['nonce'], (s, 'authenticated_volume_metadata_only'))
        geometry_candidates=[]
        if not found:
            for s in geometries[(r['epoch'],r['frame'],r['packet'][17],r['packet'][18]&0x300,geometry(r['packet']))]:
                changed=[i for i,(a,b) in enumerate(zip(s['packet'],r['packet'])) if a!=b]
                geometry_candidates.append(dict(source_nonce=s['nonce'],changed_dwords=changed,
                    source_flags=hex(s['packet'][18]),bank_flags=hex(r['packet'][18]),
                    source_key=hex(s['packet'][24]),bank_key=hex(r['packet'][24]),
                    interpretation='Geometry equality only; not a copy witness or proof of a key overwrite.'))
        matches.append(dict(bank_nonce=r['nonce'], epoch=r['epoch'], frame=r['frame'],
            result='observed_unique_correlation' if len(found) == 1 else 'ambiguous' if found else 'no_observed_match',
            producer_authority='unknown', candidates=[dict(source_nonce=s['nonce'], producer_return_rva=hex(s['metadata'][1]),
                parent_return_rva=hex(s['metadata'][2]), source_thread=s['metadata'][0], bank_thread=r['metadata'][0],
                comparison=kind) for s, kind in found.values()],geometry_only_candidates=geometry_candidates))
    return dict(process_id=pid, epochs=sorted(epochs), records=records, matches=matches,
        match_counts=dict(Counter(r['result'] for r in matches)), incomplete_or_collision_records=len(bad),
        trailing_fragments=fragments, authority='unknown',caller_cohorts=cohort_report(records),
        collection_scope='Native point/spot class 0x200 only; class 0 and other classes are not collected by this diagnostic.',
        limitations='Sampling is bounded and may omit identical producers. Unique means observed correlation only. '
        'Missing, ambiguous, overflow, incomplete and unmatched records cannot establish absence or owner identity. '
        'Use the capture status/log for dropped and budget-limited counts. No fire semantics are inferred.')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('captures', nargs='+', type=Path)
    parser.add_argument('--output', required=True, type=Path)
    args = parser.parse_args()
    args.output.write_text(json.dumps(decode(args.captures), indent=2))
