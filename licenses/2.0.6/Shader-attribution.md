Private shader derivative for Liberty's Shadows / Austin (OnyxOak).

Existing shader projection and fragment material originate in FusionFix/FusionShaders and retain GPL-3.0 attribution. Primary upstream: https://github.com/Parallellines0451/GTAIV.EFLC.FusionShaders.

Modification: independently prepared static world wd_local techniques reuse the installed native local-shadow transform and preserve each material's alpha operations. All original fragments/techniques/parameters remain unchanged. No public release authorized.

## Current release notice — October 12, 2026

The private-derivative and authorization wording above records the historical
development artifact. This release includes the matching 27 world-local shader
files for Liberty's Shadows 2.0 by OnyxOak. Preserve these files with the
matching ASI and INI.

The current package requires official FusionFix and retains the original
FusionFix/FusionShaders authorship, notices and GPL-3.0 license. Its ASI provides
the native Graphics quality menu; the shader files do not implement that menu.
Matching shader sources, assembly, inputs and build recipes are supplied in
`Libertys-Shadows-2.0-source.zip`. Compatibility claims for this package are
limited to the stated tested Windows/CE 1.2.0.59/FusionFix 5.1.1/Vulkan setup.


## Candidate2.0.1 build132 shader notice

All27 shader payloads and their complete corresponding-source component are
byte-identical to the2.0 baseline. Original upstream notices/licenses remain
unchanged. This local candidate is not publicly approved. The matching full
source archive is Libertys-Shadows-2.0.1-test132-source.zip.

Vehicle shadow-box shader: special thanks to MrBulkinAleksey, GPL-3.0. Original fork commit16318f97a79939c53bb0abda1c6aa247dc6078e0. Adapted for Liberty's Shadows by OnyxOak.
https://github.com/MrBulkinAleksey/GTAIV.EFLC.FusionFix
