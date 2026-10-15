from pathlib import Path
import hashlib,sys
source=Path(sys.argv[1]);out=Path(sys.argv[2]);text=source.read_text()
def between(first,last):
 a=text.index(first);return text[a:text.index(last,a)]
startup=between('DWORD WINAPI Start(void*) {','\n}\nextern "C"')
sources=between('void PrepareLightSources(','void __fastcall Render(')
binding=between(' if(preset117::UseOwnedBackend117(nativeBackend)){',' CShadows::pFrameCounter=')
apply=between('  if(preset117::startup.present){\n   const auto p=preset117::Requests','  PlayerShadowAllocation::additionalFeatures=PlayerShadowAllocation::additionalFeatures||')
result='// Actual117 production source SHA256 '+hashlib.sha256(source.read_bytes()).hexdigest().upper()+'\n'
backend=between('void RefreshApplied119State() noexcept {','void PrepareLightSources(') if 'void RefreshApplied119State() noexcept {' in text else ''
result+=backend+sources+startup+'\nvoid ApplySettings117(){\n'+apply+'}\nvoid BindSettings117(){\n'+binding+'}\n'
out.write_text(result)
