"""Read-only GTA IV PC FXC metadata inspection; never rewrites game files.

Independently implemented format reader. Format research reference:
https://github.com/ImpossibleEchoes/RAGE-Shader-Editor (Structures.cs).
No foreign source is included. Names are length-prefixed ASCII including NUL;
fragments, parameters and technique/pass state pairs are bounded by file size.
"""
from pathlib import Path
import hashlib
import json
import struct
import sys


class Fxc:
    def __init__(self, path):
        self.path = Path(path)
        self.data = self.path.read_bytes()
        self.offset = 0

    def take(self, size):
        if size < 0 or self.offset + size > len(self.data):
            raise ValueError(f"truncated FXC at {self.offset:x}, size {size}")
        start = self.offset
        self.offset += size
        return self.data[start:self.offset]

    def number(self, fmt):
        return struct.unpack('<' + fmt, self.take(struct.calcsize(fmt)))[0]

    def byte(self):
        return self.number('B')

    def name(self):
        length = self.byte()
        if not length:
            raise ValueError(f"zero name length at {self.offset:x}")
        raw = self.take(length)
        if raw[-1] != 0:
            raise ValueError(f"unterminated name at {self.offset:x}")
        return raw[:-1].decode('ascii')

    def fragments(self, count):
        result = []
        for _ in range(count):
            record_start = self.offset
            bindings = []
            for _ in range(self.byte()):
                kind, index = self.number('H'), self.number('H')
                bindings.append({'kind': kind, 'index': index, 'name': self.name()})
            size, secondary = self.number('H'), self.number('H')
            start = self.offset
            blob = self.take(size)
            result.append({'offset': start, 'record_start': record_start, 'record_end': self.offset,
                           'size': size, 'secondary': secondary,
                           'sha256': hashlib.sha256(blob).hexdigest(), 'bindings': bindings})
        return result

    def variables(self):
        result = []
        for _ in range(self.byte()):
            kind, count = self.byte(), self.byte()
            name, semantic = self.name(), self.name()
            annotations = []
            for _ in range(self.byte()):
                key, mode = self.name(), self.byte()
                if mode == 2:
                    value = self.name()
                elif mode in (0, 1):
                    value = self.number('i' if mode == 0 else 'f')
                else:
                    raise ValueError(f"unknown annotation type {mode}")
                annotations.append({'name': key, 'type': mode, 'value': value})
            values = self.byte()
            self.take(values * 4)
            result.append({'kind': kind, 'count': count, 'name': name,
                           'semantic': semantic, 'annotations': annotations})
        return result

    def read(self):
        magic = self.number('I')
        if magic != 0x61786772:
            raise ValueError(f"unexpected PC FXC signature {magic:x}")
        vertex = self.fragments(self.byte())
        pixel_header_offset = self.offset
        pixels = self.byte()
        if not pixels:
            raise ValueError('missing pixel fragment count')
        reserved_byte, reserved_word = self.byte(), self.number('I')
        pixel = self.fragments(pixels - 1)
        variables_offset = self.offset
        global_variables, local_variables = self.variables(), self.variables()
        techniques_offset = self.offset
        techniques = []
        for _ in range(self.byte()):
            name, passes = self.name(), []
            for _ in range(self.byte()):
                vs, ps = self.byte(), self.byte()
                states = []
                for _ in range(self.byte()):
                    offset = self.offset
                    state, value = self.number('I'), self.number('I')
                    states.append({'state': state, 'value': value, 'offset': offset})
                passes.append({'vs': vs, 'ps': ps, 'states': states})
            techniques.append({'name': name, 'passes': passes})
        names_offset = self.offset
        if self.offset < len(self.data):
            for fragment in vertex + pixel:
                fragment['name'] = self.name()
        if self.offset != len(self.data):
            raise ValueError(f"unexpected trailing bytes at {self.offset:x}")
        return {'path': str(self.path), 'sha256': hashlib.sha256(self.data).hexdigest(),
                'size': len(self.data), 'magic': hex(magic), 'reserved_byte': reserved_byte,
                'reserved_word': reserved_word, 'vertex': vertex, 'pixel': pixel,
                'globals': global_variables, 'locals': local_variables,
                'techniques': techniques, 'names_offset': names_offset,
                'pixel_header_offset': pixel_header_offset, 'variables_offset': variables_offset,
                'techniques_offset': techniques_offset}


if __name__ == '__main__':
    rows = []
    for argument in sys.argv[1:]:
        path = Path(argument)
        rows.append(Fxc(path).read())
    print(json.dumps(rows, indent=2))
