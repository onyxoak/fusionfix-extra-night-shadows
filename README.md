# Liberty's Shadows — 2.0.5

Expanded dynamic shadows for GTA IV's existing lights, created and maintained by **OnyxOak** with OpenAI Codex development assistance. FusionFix and third-party attribution and licenses remain applicable.

- [Nexus downloads, comparisons and support](https://www.nexusmods.com/gta4/mods/1459)
- [Current 2.0.5 source](https://github.com/onyxoak/fusionfix-extra-night-shadows/tree/main/source) · [2.0.5 changelog](release-notes/2.0.5.txt)
- [Release history](https://github.com/onyxoak/fusionfix-extra-night-shadows/releases) · [Changelog](CHANGELOG.md)

## Credit and link-back request

**If you use, adapt or redistribute code from Liberty’s Shadows, please credit OnyxOak as the original creator of Liberty’s Shadows and link to the [original mod on Nexus](https://www.nexusmods.com/gta4/mods/1459).**

Please distinguish your own additions from the original work, and do not present Liberty’s Shadows as something you created entirely yourself. Retain the applicable FusionFix, FusionShaders and third-party attribution and license notices as well.

Suggested credit:

> Includes work from Liberty’s Shadows by OnyxOak. Original mod: https://www.nexusmods.com/gta4/mods/1459

This credit and link-back request adds no restriction to the GPL or other applicable licenses. Existing license obligations, including preservation of required notices and access to corresponding source, remain unchanged.

## Editions

**Full 2.0.5** includes the extra vehicle, traffic, emergency and world-light shadows through its presets, matching shader package, Graphics-menu option and vehicle shadow-box fallback. See [installation](INSTALL.txt) and [full changelog](release-notes/2.0.5.txt).

**FusionFix Night Shadows Fix — No Enhancements (2.0.5)** uses the same shared ASI core with the original-fix configuration. Additional lighting features, vehicle fallback and the Liberty's Shadows menu are disabled. No Liberty's Shadows shader replacement or map assets are required. See [installation](INSTALL-FIX-ONLY.txt) and [fix-only changelog](release-notes/2.0.5-fix-only.txt).

Use one edition at a time. They use the same ASI/INI names; do not install duplicate plugins. Replacing the full edition's INI does not undo its previously installed shaders or map files: restore those from your own backup if needed. Both editions require FusionFix and its normal prerequisites.

## Source and building

`source/`, `shader-source/` and `Build.cmd` are the exact 735-file corresponding source from the public 2.0.5 source ZIP. They include build dependencies, tests, the vehicle-fallback shader and required upstream notices. `config/` contains each edition's settings. See [BUILD.md](BUILD.md), [source manifest](SOURCE-MANIFEST.json) and [release catalog](RELEASE-CATALOG.json).

The fix-only edition uses this same codebase; its disabled enhancements are configuration choices, not a separate fork. Compile once and use the appropriate edition configuration. Source files are for developers and should not be copied into the game.

## Validation and remaining work

The full release passed 162 offline checks and the source was rebuilt from an extracted package. The fix-only configuration was parser-checked and launched into nighttime gameplay with official FusionFix shaders and enhanced producers disabled. These checks do not establish universal compatibility or complete citywide shadow coverage.

2.0.5 reduces repeated validation/caster work, improves integration through active hooks, adds vehicle shadow-box fallback and repairs the captured negative-index MMA10 shutdown corruption. Special thanks to **MrBulkinAleksey** for the original fallback implementation in his FusionFix fork; OnyxOak adapted and integrated it. The fallback is disabled in Fix Only.

Heavy scenes remain demanding. Missing lamppost shadows from headlights, edge-case flicker and some transitions remain unresolved. The approximately 15% FPS improvement in the full changelog is a particular development-build comparison, not a measured 2.0.4-to-2.0.5 improvement or a 60 FPS guarantee. Private lamp diagnostics and unreleased experiments are not part of this source release.

## History and licensing

Earlier source revisions, tags and release assets remain in Git history. `historical/ce-1.6/` preserves the original imported snapshot. This is an unofficial mod, not an official FusionFix release or endorsement. Retain OnyxOak, FusionFix/FusionShaders, MrBulkinAleksey and dependency notices. See [attribution](ATTRIBUTION.md), [LICENSE](LICENSE), `licenses/` and source/vendor/.
