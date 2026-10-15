# Liberty’s Shadows — 2.0.4

Expanded dynamic-shadow interaction for GTA IV’s existing vehicle, city and environmental lights. Created and maintained by **OnyxOak**, with OpenAI Codex development assistance. Original FusionFix/FusionShaders and dependency attribution and license notices remain applicable.

- **[Download 2.0.4 and matching source](https://github.com/onyxoak/fusionfix-extra-night-shadows/releases/tag/v2.0.4)**
- **[Nexus page, comparisons and support](https://www.nexusmods.com/gta4/mods/1459)**
- **[Release history](https://github.com/onyxoak/fusionfix-extra-night-shadows/releases)** · [Changelog](CHANGELOG.md)

## Players

Use the explicitly named player ZIP attached to the selected release. It is a full package; follow its included README and preserve your customINI before updating. Other lighting/vehicle/texture mods shown in the gallery are separate. Complete Edition and Legacy are different packages.

## Source

`source/` is the exact ASI source for 2.0.4 (candidate213). It includes the renderer adapters, allocator, settings, guarded compatibility code and tests. `config/` contains the matching packaged INI. `shaders/` contains the paired transition-shader assembly and rebuild recipe when applicable.

For the **complete corresponding source**, download `Libertys-Shadows-2.0.4-source.zip` from this release. It also contains the full shader-source component and original build materials. GitHub’s automatic source snapshots do not replace that full source package. See [BUILD.md](BUILD.md), [SOURCE-MANIFEST.json](SOURCE-MANIFEST.json) and [RELEASE-CATALOG.json](RELEASE-CATALOG.json).

## Validation and remaining work

Corrected official FusionFix 5.0.1/5.1.0 adapters that previously used 5.1.1-specific pool/model/backend/copy addresses, including matching VolumetricLights paths. Added canonical headlight-failure evidence and callback-owner details where available. Includes the camera-weighted indoor 35 ft priority bubble and documented configurableINI/tuning guide. High remains the default; routine diagnostics are off.141 offline checks passed; older modpack compatibility and indoor improvements need broader runtime testing. No new FPS gain, universal conflict fix, physical pole/fire fix or RESC 10 fix is claimed.

Offline checks do not establish universal mod compatibility, shadow coverage or an FPS guarantee. Heavy nighttime scenes, model-specific pole shadows and further shadow transitions remain active refinement areas. Unfinished research is not represented as released functionality.

## Earlier code

The previous public main-tree CE1.6 snapshot is preserved in `historical/ce-1.6/`. Earlier release assets and existing tags remain intact; new chronological source imports preserve changes through the current release.

## Credits and license

Direction, integration and playtesting: OnyxOak. Development assistance: OpenAI Codex. FusionFix: ThirteenAG and contributors; shader/upstream and third-party credits are retained in `licenses/` and source notices. GPL and dependency terms are included with each version.
