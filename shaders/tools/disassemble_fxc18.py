"""Extract and disassemble existing PC FXC fragments for file-only RE."""
from pathlib import Path
import ctypes
import sys
from inspect_fxc18 import Fxc

compiler = ctypes.WinDLL('d3dcompiler_47.dll')
disassemble = compiler.D3DDisassemble
disassemble.argtypes = [ctypes.c_void_p, ctypes.c_size_t, ctypes.c_uint,
                       ctypes.c_char_p, ctypes.POINTER(ctypes.c_void_p)]
disassemble.restype = ctypes.c_long


def listing(raw):
    buffer = ctypes.create_string_buffer(raw)
    blob = ctypes.c_void_p()
    status = disassemble(buffer, len(raw), 0, None, ctypes.byref(blob))
    if status < 0:
        raise ValueError(f'D3DDisassemble failed {status & 0xffffffff:08x}')
    table = ctypes.cast(blob, ctypes.POINTER(ctypes.POINTER(ctypes.c_void_p))).contents
    address = ctypes.WINFUNCTYPE(ctypes.c_void_p, ctypes.c_void_p)(table[3])(blob)
    size = ctypes.WINFUNCTYPE(ctypes.c_size_t, ctypes.c_void_p)(table[4])(blob)
    result = ctypes.string_at(address, size).rstrip(b'\0').decode('ascii')
    ctypes.WINFUNCTYPE(ctypes.c_ulong, ctypes.c_void_p)(table[2])(blob)
    return result


if __name__ == '__main__':
    fxc = Fxc(sys.argv[1])
    metadata = fxc.read()
    destination = Path(sys.argv[2])
    destination.mkdir(parents=True, exist_ok=True)
    for category in ('vertex', 'pixel'):
        for index, fragment in enumerate(metadata[category]):
            start, size = fragment['offset'], fragment['size']
            text = listing(fxc.data[start:start+size])
            (destination / f'{fxc.path.stem}-{category}{index}.asm').write_text(text)
    print(f'{destination}: {len(metadata["vertex"])} vertex and {len(metadata["pixel"])} pixel fragments')
