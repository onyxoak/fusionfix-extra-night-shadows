from pathlib import Path
import re,sys
source=Path(sys.argv[1]);out=Path(sys.argv[2]);out.mkdir(exist_ok=True)
runtime=(source/'standalone-runtime.cpp').read_text();startup=runtime[runtime.index('DWORD StartShadow117('):]
lines=re.findall(r'^\s*FrameBoundary154::requested=.*?;',startup,re.M);assert len(lines)==1
(out/'clock-config162-extracted.inc').write_text('void ConfigureClock162(const std::filesystem::path& ini,unsigned mode){(void)mode;'+lines[0].strip()+'}\n')
world=(source/'WorldGeometryCatalogRuntime20.inc').read_text();prefix=world[:world.index('void Disable() noexcept')];(out/'world-callbacks162-extracted.inc').write_text(prefix+'}\n')
# Include Publish exactly as production, outside preparation/enable hook APIs.
start=world.index('void Publish() {');body=world[start:world.rfind('\n}')];(out/'world-publish162-extracted.inc').write_text(body+'\n')
