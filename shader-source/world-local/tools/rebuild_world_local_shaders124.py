# Packaging path adaptation, 2026-10-12; shader algorithm unchanged.
"""Prepare separate private world local-shadow shader artifacts, never install.

Adds rigid POSITION/TEXCOORD material wd_local variants using authenticated
existing FusionFix local-depth projection and each material's own alpha code.
Existing shader fragments, techniques and render-state pairs remain identical.
Unknown vertex inputs, output shapes or parameter bindings are refused.
"""
from pathlib import Path
import ctypes
import hashlib
import json
import re
import shutil
import struct
from inspect_fxc18 import Fxc
from disassemble_fxc18 import listing

import sys
ROOT = Path(sys.argv[1]).resolve()
GAME = ROOT / 'game/GTAIV'
INPUT = GAME / 'update/common/shaders/win32_30'
OUTPUT = ROOT / 'artifact/world-local-shaders18'
DONOR = INPUT / 'gta_vehicle_basic.fxc'


def digest(raw):
    return hashlib.sha256(raw).hexdigest()


def safe_source(path):
    path = path.absolute()
    if path.resolve() != path or not path.is_relative_to(GAME):
        raise ValueError(f'nonprivate resolved path: {path}')
    for component in [path, *path.parents]:
        status = component.stat(follow_symlinks=False)
        if getattr(status, 'st_file_attributes', 0) & 0x400:
            raise ValueError(f'reparse point forbidden: {component}')
    if path.stat().st_nlink != 1:
        raise ValueError(f'hardlinked input forbidden: {path}')


def blob_bytes(blob):
    table = ctypes.cast(blob, ctypes.POINTER(ctypes.POINTER(ctypes.c_void_p))).contents
    pointer = ctypes.WINFUNCTYPE(ctypes.c_void_p, ctypes.c_void_p)(table[3])(blob)
    size = ctypes.WINFUNCTYPE(ctypes.c_size_t, ctypes.c_void_p)(table[4])(blob)
    result = ctypes.string_at(pointer, size)
    ctypes.WINFUNCTYPE(ctypes.c_ulong, ctypes.c_void_p)(table[2])(blob)
    return result


compiler = ctypes.WinDLL('d3dcompiler_47.dll')
assemble = compiler.D3DAssemble
assemble.argtypes = [ctypes.c_void_p, ctypes.c_size_t, ctypes.c_char_p, ctypes.c_void_p,
                     ctypes.c_void_p, ctypes.c_uint, ctypes.POINTER(ctypes.c_void_p),
                     ctypes.POINTER(ctypes.c_void_p)]
assemble.restype = ctypes.c_long


def compiled(text):
    source = text.encode('ascii')
    buffer = ctypes.create_string_buffer(source)
    shader, errors = ctypes.c_void_p(), ctypes.c_void_p()
    status = assemble(buffer, len(source), b'Libertys-Shadows-world-local18', None,
                      None, 0, ctypes.byref(shader), ctypes.byref(errors))
    error = blob_bytes(errors).decode('ascii', errors='replace') if errors else ''
    if status < 0:
        raise ValueError(f'assembly refused {status & 0xffffffff:08x}: {error}')
    return blob_bytes(shader)


def name(raw):
    value = raw.encode('ascii') + b'\0'
    if len(value) > 255:
        raise ValueError('FXC name exceeds byte count')
    return bytes([len(value)]) + value


def fragment(bindings, raw):
    if len(bindings) > 255 or len(raw) > 65535:
        raise ValueError('FXC fragment bounds')
    result = bytes([len(bindings)])
    for binding in bindings:
        result += struct.pack('<HH', binding['kind'], binding['index']) + name(binding['name'])
    return result + struct.pack('<HH', len(raw), len(raw)) + raw


def technique(title, vs, ps, states):
    if not (0 <= vs < 255 and 1 <= ps < 255 and len(states) < 255):
        raise ValueError('FXC technique bounds')
    return name(title) + bytes([1, vs, ps, len(states)]) + b''.join(
        struct.pack('<II', state['state'], state['value']) for state in states)


def body(text):
    return '\n'.join(line.strip() for line in text.splitlines()
                     if line.strip() and not line.strip().startswith('//')) + '\n'


def bytes_for(fxc, record):
    return fxc.data[record['offset']:record['offset'] + record['size']]


def declarations(metadata):
    result = {}
    for variable in metadata['globals'] + metadata['locals']:
        if variable['name'] in result:
            raise ValueError('ambiguous variable declaration')
        result[variable['name']] = (variable['kind'], variable['count'])
    return result


def build(path, donor, donor_meta, donor_vs, donor_ps):
    safe_source(path)
    fxc = Fxc(path)
    metadata = fxc.read()
    techniques = {entry['name']: entry for entry in metadata['techniques']}
    if any(key.startswith('wd_local') for key in techniques):
        raise ValueError('already has local technique')
    selected = [key for key in ('wd_draw', 'wd_masked_draw') if key in techniques]
    if not selected or 'wd_draw' not in selected:
        raise ValueError('missing rigid authored shadow technique')
    dest_declarations = declarations(metadata)
    source_declarations = declarations(donor_meta)
    donor_bindings = donor_vs['bindings'] + donor_ps['bindings']
    for binding in donor_bindings:
        key = binding['name']
        if key not in dest_declarations or dest_declarations[key] != source_declarations[key]:
            raise ValueError(f'local depth global declaration mismatch: {key}')
    new_vs, new_ps, new_techniques = [], [], []
    rows = []
    for key in selected:
        authored = techniques[key]
        if len(authored['passes']) != 1:
            raise ValueError('authored multipass shadow unsupported')
        authored_pass = authored['passes'][0]
        authored_vs = metadata['vertex'][authored_pass['vs']]
        authored_ps = metadata['pixel'][authored_pass['ps'] - 1]
        old_vs = body(listing(bytes_for(fxc, authored_vs)))
        old_ps = body(listing(bytes_for(fxc, authored_ps)))
        # Reject instanced/skinned/displaced inputs; a rigid shadow material uses
        # native gWorld and POSITION0, with TEXCOORD0 explicitly in v1.
        input_lines = re.findall(r'^dcl_\S+ v\d.*$', old_vs, re.M)
        if input_lines != ['dcl_position v0', 'dcl_texcoord v1']:
            raise ValueError(f'unsupported rigid vertex inputs: {input_lines}')
        if not any(item['name'] == 'gWorld' and item['index'] == 0
                   for item in authored_vs['bindings']):
            raise ValueError('missing standard world transform')
        if re.search(r'^dcl_\S+ v1\b', old_ps, re.M):
            raise ValueError('authored alpha already uses additional interpolator')
        if 'dcl_texcoord v0.xyz' not in old_ps or old_ps.count('mov oC0.xyz, v0.x') != 1:
            raise ValueError('unknown authored shadow depth/interpolator')
        if old_ps.count('mov oC0.w, r0.x') != 1:
            raise ValueError('unknown authored alpha output')
        without_outputs = old_ps.replace('mov oC0.xyz, v0.x', '').replace('mov oC0.w, r0.x', '')
        if re.search(r'\bv0\.x(?:[,\s]|$)', without_outputs):
            raise ValueError('authored alpha depends on old depth encoding')
        if re.search(r'\bc56\b', old_ps) or 'def c56,' in old_ps:
            raise ValueError('authored alpha clashes with native shadow parameter')
        vs_text = body(listing(bytes_for(donor, donor_vs)))
        vs_text = vs_text.replace('dcl_position v0\n',
                                  'dcl_position v0\ndcl_texcoord v1\n', 1)
        vs_text = vs_text.replace('dcl_texcoord1 o1\n',
                                  'dcl_texcoord1 o1\ndcl_texcoord o2.xyz\n', 1)
        vs_text += 'mov o2.x, r4.z\nmov o2.yz, v1.xxyw\n'
        # Preserve every existing texture, alpha and stochastic cutout operation.
        # Only replace directional depth output with native local linear depth;
        # local near-plane texkill is the exact existing vehicle condition.
        ps_text = old_ps.replace('dcl_texcoord v0.xyz\n',
                                 'dcl_texcoord v0.xyz\ndcl_texcoord1 v1.zw\n', 1)
        ps_text = ps_text.replace('mov oC0.xyz, v0.x\nmov oC0.w, r0.x',
                                 'abs r24.x, c56.x\n'
                                 'cmp r24.x, -r24.x, c1.y, c1.x\n'
                                 'add r24.y, -v1.z, c0.x\n'
                                 'mul r24, r24.x, r24.y\n'
                                 'texkill r24\nmov oC0, v1.w', 1)
        # Use dedicated constants rather than borrowing material-specific c0/1.
        ps_text = ps_text.replace('ps_3_0\n',
                                 'ps_3_0\ndef c214, 0.01, 1, 0, 0\n', 1)
        ps_text = ps_text.replace('cmp r24.x, -r24.x, c1.y, c1.x',
                                 'cmp r24.x, -r24.x, c214.y, c214.z')
        ps_text = ps_text.replace('add r24.y, -v1.z, c0.x',
                                 'add r24.y, -v1.z, c214.x')
        if re.search(r'\bc214\b', old_ps) or re.search(r'\br24\b', old_ps):
            raise ValueError('authored alpha clashes with added local depth registers')
        new_vertex, new_pixel = compiled(vs_text), compiled(ps_text)
        ps_bindings = [*authored_ps['bindings'], *donor_ps['bindings']]
        vs_id = len(metadata['vertex']) + len(new_vs)
        ps_id = len(metadata['pixel']) + len(new_ps) + 1
        title = key.replace('wd_', 'wd_local_', 1)
        new_vs.append((fragment(donor_vs['bindings'], new_vertex), title + '_vs', new_vertex))
        new_ps.append((fragment(ps_bindings, new_pixel), title + '_ps', new_pixel))
        new_techniques.append(technique(title, vs_id, ps_id, authored_pass['states']))
        rows.append({'technique': title, 'authored_technique': key,
                     'old_vs': authored_pass['vs'], 'old_ps': authored_pass['ps'],
                     'new_vs': vs_id, 'new_ps': ps_id,
                     'states': authored_pass['states'],
                     'new_vs_sha256': digest(new_vertex), 'new_ps_sha256': digest(new_pixel)})
        asm = OUTPUT / 'assembly' / path.stem
        asm.mkdir(parents=True, exist_ok=True)
        (asm / (title + '.vs.asm')).write_text(vs_text)
        (asm / (title + '.ps.asm')).write_text(ps_text)
    new_vs_count = len(metadata['vertex']) + len(new_vs)
    new_ps_count = len(metadata['pixel']) + len(new_ps)
    new_technique_count = len(metadata['techniques']) + len(new_techniques)
    if max(new_vs_count, new_ps_count + 1, new_technique_count) > 255:
        raise ValueError('FXC aggregate count overflow')
    raw = fxc.data[:4] + bytes([new_vs_count])
    raw += fxc.data[5:metadata['pixel_header_offset']]
    raw += b''.join(item[0] for item in new_vs)
    raw += bytes([new_ps_count + 1])
    raw += fxc.data[metadata['pixel_header_offset'] + 1:metadata['variables_offset']]
    raw += b''.join(item[0] for item in new_ps)
    raw += fxc.data[metadata['variables_offset']:metadata['techniques_offset']]
    raw += bytes([new_technique_count])
    raw += fxc.data[metadata['techniques_offset'] + 1:metadata['names_offset']]
    raw += b''.join(new_techniques)
    if metadata['names_offset'] != len(fxc.data):
        raw += b''.join(name(item['name']) for item in metadata['vertex'])
        raw += b''.join(name(item[1]) for item in new_vs)
        raw += b''.join(name(item['name']) for item in metadata['pixel'])
        raw += b''.join(name(item[1]) for item in new_ps)
    else:
        # Naming is optional. Add complete names for all old/new fragments.
        raw += b''.join(name(item.get('name', 'vs' + str(index)))
                        for index, item in enumerate(metadata['vertex']))
        raw += b''.join(name(item[1]) for item in new_vs)
        raw += b''.join(name(item.get('name', 'ps' + str(index)))
                        for index, item in enumerate(metadata['pixel']))
        raw += b''.join(name(item[1]) for item in new_ps)
    relative = path.relative_to(GAME)
    backup, changed = OUTPUT / 'originals' / relative, OUTPUT / 'candidate' / relative
    backup.parent.mkdir(parents=True, exist_ok=True)
    changed.parent.mkdir(parents=True, exist_ok=True)
    backup.write_bytes(fxc.data)
    changed.write_bytes(raw)
    rebuilt = Fxc(changed).read()
    # Identity checks are preparation guards, not game testing: old fragments
    # must remain byte-identical, and native declarations/state pairs preserved.
    for category in ('vertex', 'pixel'):
        old = metadata[category]
        if [item['sha256'] for item in rebuilt[category][:len(old)]] != [item['sha256'] for item in old]:
            raise ValueError('original shader bytecode changed')
    if rebuilt['techniques'][:len(metadata['techniques'])] != metadata['techniques']:
        # Offset annotations shift because new fragments were inserted; compare
        # semantic records after removing byte offsets from every state.
        def semantics(items):
            return [(item['name'], [(p['vs'], p['ps'], [(s['state'], s['value']) for s in p['states']])
                                   for p in item['passes']]) for item in items]
        if semantics(rebuilt['techniques'][:len(metadata['techniques'])]) != semantics(metadata['techniques']):
            raise ValueError('original technique semantics changed')
    if rebuilt['globals'] != metadata['globals'] or rebuilt['locals'] != metadata['locals']:
        raise ValueError('original parameter declarations changed')
    return {'relative': str(relative), 'source': str(path), 'original_sha256': digest(fxc.data),
            'candidate_sha256': digest(raw), 'original_size': len(fxc.data),
            'candidate_size': len(raw), 'original_vertex_fragments': len(metadata['vertex']),
            'original_pixel_fragments': len(metadata['pixel']), 'added': rows,
            'ancestry_reparse_free': True, 'hardlink_count': 1,
            'original_bytecode_alpha_states_preserved': True}


def main():
    if OUTPUT.exists():
        raise ValueError('artifact already exists; preserve it')
    safe_source(DONOR)
    donor = Fxc(DONOR)
    donor_meta = donor.read()
    # Pin the actual local depth donor before constructing derivative shaders.
    donor_hash = digest(donor.data)
    donor_vs = donor_meta['vertex'][9]
    donor_ps = donor_meta['pixel'][8]
    if donor_vs['sha256'] != 'c1e4e842ffed65cb6a6e62d2196a0da60caf26700759f8b64e51ae30e8ffb7df' or donor_ps['sha256'] != '42cb115092efe1d0a43d58691e8ce32fac9ac2dbfa54675997c7378b15f6affb':
        raise ValueError('local native projection donor fragments changed')
    OUTPUT.mkdir(parents=True)
    rows, refused = [], []
    for path in sorted(INPUT.glob('gta_*.fxc')):
        if not re.match(r'^gta_(default|normal|spec|emissive|reflect|parallax|cutout_fence)', path.stem):
            continue
        try:
            rows.append(build(path, donor, donor_meta, donor_vs, donor_ps))
        except (ValueError, IndexError) as error:
            refused.append({'source': str(path), 'reason': str(error)})
    receipt = {'purpose': 'private static world native local-depth techniques',
               'installed': False, 'runtime_validated': False,
               'donor': str(DONOR), 'donor_sha256': donor_hash,
               'format_reference': 'https://github.com/ImpossibleEchoes/RAGE-Shader-Editor',
               'foreign_parser_source_incorporated': False,
               'shader_attribution': 'Existing private FusionFix shader fragments; GPL-3.0; own alpha/local-depth combination',
               'cull_metadata_modified': False, 'patched': rows, 'refused': refused}
    (OUTPUT / 'receipt.json').write_text(json.dumps(receipt, indent=2))
    license_source = Path(__file__).resolve().parent.parent / 'licenses/FusionShaders-GPL-3.0'
    shutil.copy2(license_source, OUTPUT / 'LICENSE-GPL-3.0')
    (OUTPUT / 'ATTRIBUTION.md').write_text(
        "Private shader derivative for Liberty's Shadows / Austin (OnyxOak).\n\n"
        "Existing shader projection and fragment material originate in FusionFix/FusionShaders "
        "and retain GPL-3.0 attribution. Primary upstream: "
        "https://github.com/Parallellines0451/GTAIV.EFLC.FusionShaders.\n\n"
        "Modification: independently prepared static world wd_local techniques reuse the "
        "installed native local-shadow transform and preserve each material's alpha operations. "
        "All original fragments/techniques/parameters remain unchanged. No public release authorized.\n")
    print(json.dumps({'artifact': str(OUTPUT), 'patched': len(rows), 'refused': len(refused)}))


if __name__ == '__main__':
    main()
