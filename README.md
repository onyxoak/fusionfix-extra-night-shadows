# Liberty’s Shadows — 2.0.2

Expanded dynamic-shadow interaction for GTA IV’s existing vehicle, city and environmental lights. Created and maintained by **OnyxOak**, with OpenAI Codex development assistance. Original FusionFix/FusionShaders and dependency attribution and license notices remain applicable.

- **[Download 2.0.2 and matching source](https://github.com/onyxoak/fusionfix-extra-night-shadows/releases/tag/v2.0.2)**
- **[Nexus page, comparisons and support](https://www.nexusmods.com/gta4/mods/1459)**
- **[Release history](https://github.com/onyxoak/fusionfix-extra-night-shadows/releases)** · [Changelog](CHANGELOG.md)

## Players

Use the explicitly named player ZIP attached to the selected release. It is a full package; follow its included README and preserve your customINI before updating. Other lighting/vehicle/texture mods shown in the gallery are separate. Complete Edition and Legacy are different packages.

## Source

`source/` is the exact ASI source for 2.0.2 (candidate195). It includes the renderer adapters, allocator, settings, guarded compatibility code and tests. `config/` contains the matching packaged INI. `shaders/` contains the paired transition-shader assembly and rebuild recipe when applicable.

For the **complete corresponding source**, download `Libertys-Shadows-2.0.2-source.zip` from this release. It also contains the full shader-source component and original build materials. GitHub’s automatic source snapshots do not replace that full source package. See [BUILD.md](BUILD.md), [SOURCE-MANIFEST.json](SOURCE-MANIFEST.json) and [RELEASE-CATALOG.json](RELEASE-CATALOG.json).

## Validation and remaining work

Added protection against the native model-reference queue overflow investigated during nighttime crashes. Reduced redundant shadow-caster CPU work without changing lighting, range, brightness or geometry coverage. High remained the default, with routine diagnostics off. VolumetricLights was not compatible with this historical version.

Offline checks do not establish universal mod compatibility, shadow coverage or an FPS guarantee. Heavy nighttime scenes, model-specific pole shadows and further shadow transitions remain active refinement areas. Unfinished research is not represented as released functionality.

## Earlier code

The previous public main-tree CE1.6 snapshot is preserved in `historical/ce-1.6/`. Earlier release assets and existing tags remain intact; new chronological source imports preserve changes through the current release.

## Credits and license

Direction, integration and playtesting: OnyxOak. Development assistance: OpenAI Codex. FusionFix: ThirteenAG and contributors; shader/upstream and third-party credits are retained in `licenses/` and source notices. GPL and dependency terms are included with each version.
