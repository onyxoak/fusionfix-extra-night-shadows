"""Generate the test include from actual runtime bodies, without copying a draft."""
from pathlib import Path
import hashlib
import sys

source=Path(sys.argv[1]) if len(sys.argv)>1 else Path(__file__).resolve().parents[1]/'NativeGraphicsRuntime117.cpp'
output=Path(sys.argv[2]) if len(sys.argv)>2 else Path(__file__).with_name('native-graphics-runtime117-extracted.inc')
text=source.read_text()
def span(first,last):
 start=text.index(first);end=text.index(last,start)
 return text[start:end]
preserve=span('struct Preserve {','struct Memory {')
memory=span('struct Memory {','SafetyHookMid fillBoundary;').replace('struct Memory {','struct RuntimeMemory {',1)
helpers=span('std::optional<unsigned> Slot(','uint32_t AppendOwned(')
callbacks=span('uint8_t __cdecl Scroll(','void Require(')
out='// Generated from actual NativeGraphicsRuntime117.cpp. Do not hand-edit.\n'
out+='// Source SHA256 '+hashlib.sha256(source.read_bytes()).hexdigest().upper()+'\n'
out+='#if defined(NATIVE117_EXTRACT_MEMORY)\n'+preserve+memory+'\n#endif\n'
out+='#if defined(NATIVE117_EXTRACT_CALLBACKS)\n'+helpers+callbacks+'\n#endif\n'
if not output.exists() or output.read_text()!=out:output.write_text(out)
print(str(output))
