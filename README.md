# Extra Night Shadows Fix and Better Headlights

Created and maintained by **OnyxOak**, with Codex-assisted development. An unofficial modification of **FusionFix by ThirteenAG and contributors**. Upstream copyright, GPL-3.0 and dependency notices remain applicable.

**[Downloads and support on Nexus](https://www.nexusmods.com/gta4/mods/1459)** · **[GitHub releases](https://github.com/onyxoak/fusionfix-extra-night-shadows/releases)**

## Browse the actual source

This branch contains the **released CE 1.6 source snapshot**, imported from its published matching-source ZIP. Start in **[source/](source)**; you do not need to download a ZIP to read the implementation.

- [CE 1.5 source snapshot](https://github.com/onyxoak/fusionfix-extra-night-shadows/tree/source-ce-1.5/source)
- [CE 1.6 source snapshot](https://github.com/onyxoak/fusionfix-extra-night-shadows/tree/source-ce-1.6/source)
- [Changes from 1.5 to 1.6](https://github.com/onyxoak/fusionfix-extra-night-shadows/compare/source-ce-1.5...source-ce-1.6)
- [Source provenance and file hashes](SOURCE-MANIFEST.json) · [Build guidance](BUILD.md) · [Release notes](RELEASE-NOTES.md)

| Area | Start here |
|---|---|
| Headlight submission and integration | [nightshadows.ixx](source/nightshadows.ixx) |
| Selection and native integration | [ShadowAllocationRuntime.inl](source/ShadowAllocationRuntime.inl) |
| Nearby vehicle receivers on foot | [NearbyVehicleReceivers36.hpp](source/NearbyVehicleReceivers36.hpp) |
| Headlight brightness after exit | [HeadlightEnhancementRuntime.inl](source/HeadlightEnhancementRuntime.inl) |
| Shadow map ownership validation | [ShadowLookupValidation.hpp](source/ShadowLookupValidation.hpp) |
| Reach settings | [ShadowReach.hpp](source/ShadowReach.hpp) |

## What it adds

Expanded headlight-shadow interactions for Niko and nearby vehicles, recently driven vehicle relevance, retained headlight brightness after exiting, separate reach controls, and safeguards for shadow selection and cache ownership. CE 1.6 adds the latest gameplay-camera, light-volume retention, native continuity and cache-dependency protections. See the version-specific source and release notes; not every function or experimental path is active in every configuration.

The fixed shadow budget remains. Some shadows can appear late or be unavailable in crowded scenes. This is not ray tracing, extra shadow slots, or a guarantee of a shadow under every vehicle. Performance varies; no matched performance benchmark is claimed.

## Choose your edition

CE 1.6 targets Complete Edition 1.2.0.59 with FusionFix 5.0.1, its ASI loader and DXVK/Vulkan. Native DirectX is not a supported configuration for this release. Legacy 1.1 targets GTA IV 1.0.8.0 and also needs the Legacy Addon; its separate runtime and source remain in the release downloads. This default source tree is CE, not the legacy adapter.

Follow your package README for ASI/menu installation and required INI/CFG entries. Keep backups. The two editions' ASIs are not interchangeable.

## Credits and screenshots

[OnyxOak attribution and upstream credits](ATTRIBUTION.md) · [GPL-3.0](LICENSE-FusionFix.txt)

Showcase captures may include additional vehicle, reflection and graphics mods that are not included. This mod changes nighttime shadows and headlight behavior. Showcase scenes are not automatically matched before-and-after comparisons.

Please credit “Extra Night Shadows Fix and Better Headlights by OnyxOak, an unofficial modification of FusionFix by ThirteenAG and contributors.” This request adds no restriction to GPL-compliant reuse.
