Shader modifications for Liberty's Shadows by OnyxOak.

Existing shader projection and fragment material originate in FusionFix/FusionShaders and retain GPL-3.0 attribution. Primary upstream: https://github.com/Parallellines0451/GTAIV.EFLC.FusionShaders.

Modification: independently prepared static world wd_local techniques reuse the installed native local-shadow transform and preserve each material's alpha operations. All original fragments/techniques/parameters remain unchanged.
