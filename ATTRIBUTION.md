# Extra Night Shadows Fix and Better Headlights — by OnyxOak

## Credit and link-back request

**If you use, adapt or redistribute code from Liberty’s Shadows, please credit OnyxOak as the original creator of Liberty’s Shadows and link to the [original mod on Nexus](https://www.nexusmods.com/gta4/mods/1459).**

Please distinguish your own additions from the original work, and do not present Liberty’s Shadows as something you created entirely yourself. Retain the applicable FusionFix, FusionShaders and third-party attribution and license notices as well.

Suggested credit:

> Includes work from Liberty’s Shadows by OnyxOak. Original mod: https://www.nexusmods.com/gta4/mods/1459

This credit and link-back request adds no restriction to the GPL or other applicable licenses. Existing license obligations, including preservation of required notices and access to corresponding source, remain unchanged.

**OnyxOak created and maintains this unofficial GTA IV shadow modification:** project concept and design direction, integration decisions, visual testing, refinement, and releases. Development used OpenAI Codex coding assistance.

**FusionFix is the upstream project, by ThirteenAG and its contributors.** Their original work, copyright notices, GPL-3.0 license and dependency credits remain applicable. This modification is not an official FusionFix release or an endorsement by its contributors.

## Identify this modification accurately

Suggested credit: **“Extra Night Shadows Fix and Better Headlights by OnyxOak, an unofficial modification of FusionFix by ThirteenAG and contributors.”**

Official downloads, current requirements and support:
https://www.nexusmods.com/gta4/mods/1459

Project history and source releases:
https://github.com/onyxoak/fusionfix-extra-night-shadows

Modification notices added 2026-09-27. Attribution packaging revision A1 retains the CE 1.5 / legacy 1.1 lighting behavior. Mod versions are separate from the upstream FusionFix version.

## For articles and download mirrors

Please identify OnyxOak as the author of this modification near its first mention, and distinguish mod authorship from upstream and photograph credits. Link to the official page so readers receive the current version and edition-specific instructions. Screenshots featuring other graphics or vehicle mods do not imply those mods are included here.

Linking to the official download instead of maintaining a mirror is a request, not an additional license restriction. Redistribution remains governed by GPL-3.0 and applicable dependency licenses, including their notice and corresponding-source requirements. This document does not claim ownership of upstream work or add a restriction to its license.

Standalone adaptation, 2026-10-01: separate guarded CE adapter for official FusionFix 5.0.1, 5.1.0 and 5.1.1. This CE 1.7 proposal preserves CE44 shadow behavior; Legacy is unchanged. The historical packaging-version note above describes prior releases.


CE 1.8 release, 2026-10-04: Liberty's Shadows - Extra Night Shadows Fix and Enhancements. OnyxOak's accepted standalone V10 implementation adds supported traffic/emergency-light enhancements. Historical version notes and original code notices above are retained. No experimental menu code is included; Legacy is unchanged.


## Current release notice — Liberty's Shadows 2.0, October 12, 2026

The version-specific adaptation and release notes above describe historical
releases. This package is Liberty's Shadows by OnyxOak and includes its guarded
native Graphics menu with Off, Low, Medium and High, plus INI-only Custom
settings. Master and feature-family changes require a game restart.

FusionFix remains the sole required companion mod, including its normal loading
support. This package is tested with Windows, GTA IV Complete Edition 1.2.0.59,
official FusionFix 5.1.1 and Vulkan/DXVK. Historical references to other versions,
standalone adaptations or releases without menu code do not establish this
package's compatibility or remove its FusionFix dependency. The developer
launcher, trainer, ScriptHook and recorder are not player prerequisites.

Original upstream authorship, copyright notices and license terms are preserved.
Matching ASI and shader source/build recipes are provided separately in
`Libertys-Shadows-2.0-source.zip`. This is an unofficial release by OnyxOak,
not an official FusionFix release or endorsement.


## Candidate2.0.1 build132 - October13,2026 (local playtest only)

This candidate retains the original authorship and license notices. It adds
a guarded official FusionFix5.1.1 shared-backend handoff for enabled owned
presets while preserving the genuine saved preference and restoring it for
Off/failure, successful-issued dual-key ownership, native Night Shadows
prerequisite status and bounded startup-conflict reporting. The source and
98-check receipt match build132. Actual rendered/author acceptance and public
approval remain pending. Matching ASI and complete unchanged shader source
are in Libertys-Shadows-2.0.1-test132-source.zip. No new dependency, geometry
walker, native shadow-map capacity or universal FPS/lighting claim is added.


## Liberty's Shadows 2.0.5 — vehicle-shadow fallback

Special thanks to **MrBulkinAleksey** for the original vehicle shadow-box fallback implementation in his FusionFix fork. Adapted and integrated into Liberty's Shadows by OnyxOak. See `source/vendor/MrBulkinAleksey/` and `licenses/2.0.5/` for the preserved source and GPL notices. This enhancement is enabled in the Full package and disabled in Fix Only.
