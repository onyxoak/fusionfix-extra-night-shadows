# Liberty's Shadows — 2.0.6

Expanded dynamic shadows for GTA IV's existing lights, created and maintained by **OnyxOak** with OpenAI Codex development assistance.

- [Nexus downloads, comparisons and support](https://www.nexusmods.com/gta4/mods/1459)
- [Changes in 2.0.6](release-notes/2.0.6.txt) · [Release history](CHANGELOG.md)

## Credit and link-back request

**If you use, adapt or redistribute code from Liberty’s Shadows, please credit OnyxOak as the original creator of Liberty’s Shadows and link to the [original mod on Nexus](https://www.nexusmods.com/gta4/mods/1459).**

Please distinguish your own additions from the original work, and do not present Liberty’s Shadows as something you created entirely yourself. Retain the applicable FusionFix, FusionShaders and third-party attribution and license notices as well.

Suggested credit:

> Includes work from Liberty’s Shadows by OnyxOak. Original mod: https://www.nexusmods.com/gta4/mods/1459

This credit and link-back request adds no restriction to the GPL or other applicable licenses. Existing license obligations, including preservation of required notices and access to corresponding source, remain unchanged.

## Full mod

2.0.6 adds headlight shadows for supported street and park lamps, restores ground illumination for supported industrial, dock and airport lights, and fixes fixture dimming and the reported severe lamp flicker. It includes dual headlights, taillights, traffic, emergency and world-light shadows, presets, the Graphics-menu option and vehicle shadow-box fallback. Install the matching ASI, INI and shaders together; see [installation](INSTALL.txt) and [tuning](TUNING-GUIDE.txt).

Special thanks to **MrBulkinAleksey** for the original vehicle shadow-box fallback in his FusionFix fork, adapted and integrated by OnyxOak. Attribution and licenses are preserved in `source/vendor/`, `shader-source/` and `licenses/`.

## Fix Only — No Enhancements

The separate **2.0.5 Fix Only** download fixes original night shadows without the enhanced lighting, preset menu or replacement shaders. Its tested source is preserved at [the 2.0.5 revision](https://github.com/onyxoak/fusionfix-extra-night-shadows/tree/574b87942579ed027f20322734335ed507214d84). See [Fix Only installation](INSTALL-FIX-ONLY.txt). It has not been repackaged as 2.0.6. Use one edition at a time; do not install duplicate plugins.

## Building

The current source matches the 2.0.6 corresponding-source archive, including dependencies, tests and shader sources. See [BUILD.md](BUILD.md). Build from an x86 Visual Studio developer prompt using `Build.cmd`. Source files are for developers and should not be copied into the game.

## Validation and remaining work

The packaged source was built from an independent extraction and passed 165 offline checks. The paired shader rebuilt identically. Austin confirmed the lamp and severe-flicker fixes in the tested scenes. These results do not establish complete citywide coverage or compatibility with every setup.

No new FPS gain is claimed for 2.0.6; a small slowdown was reported after the lamp repairs. Dense nighttime performance, indoor shadow competition, other edge-case flicker, some worklight beam shapes and raised-dock light leakage remain under investigation. DLSS/DLAA interruptions and pause-menu-only traffic shadows are not claimed fixed.

## Licensing and history

This is an unofficial mod. Preserve all applicable OnyxOak, FusionFix/FusionShaders, MrBulkinAleksey and dependency notices. See [attribution](ATTRIBUTION.md) and [LICENSE](LICENSE). Earlier releases remain in Git history and tags; `historical/ce-1.6/` preserves the original imported snapshot.
