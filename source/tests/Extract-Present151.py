from pathlib import Path
import sys
source=Path(sys.argv[1]);out=Path(sys.argv[2])
t=(source/'PresentFrameTime151Runtime.inc').read_text()
a=t.index('struct Ops {');b=t.index('HRESULT STDMETHODCALLTYPE Present(',a)
out.write_text(t[a:b])
