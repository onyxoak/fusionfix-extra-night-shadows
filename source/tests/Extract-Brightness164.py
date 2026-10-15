from pathlib import Path
import sys
s=Path(sys.argv[1]);o=Path(sys.argv[2])
r=(s/'core/HeadlightEnhancementRuntime.inl').read_text()
a=r.index('    static uint64_t VehicleToken(');b=r.index('    // No projection-mask replacement')
(o/'headlight164-retain.inc').write_text(r[a:b])
r=(s/'standalone-runtime.cpp').read_text();a=r.index('void Brightness(');b=r.index('\nvoid Warning(',a)
(o/'headlight164-hook.inc').write_text(r[a:b]+'\n')
