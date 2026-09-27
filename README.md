# Extra Night Shadows Fix and Better Headlights

## Created and maintained by OnyxOak

An unofficial GTA IV lighting and shadow modification of **FusionFix by ThirteenAG and its contributors**. OnyxOak leads the project's design, integration, visual testing, refinement and releases, with OpenAI Codex coding assistance. This modification is not an official FusionFix release.

**[Official downloads, installation instructions and support on Nexus Mods](https://www.nexusmods.com/gta4/mods/1459)**

**[Authorship, upstream credits and information for articles/mirrors](ATTRIBUTION.md)**

### Choose your edition

| Download | Game | Prerequisites |
|---|---|---|
| CE 1.5 | Complete Edition 1.2.0.59 | FusionFix 5.0.1 and its ASI loader |
| Legacy 1.1 | GTA IV 1.0.8.0 | FusionFix 5.0.1, ASI loader and Legacy Addon |

Use only the package matching your executable. The two ASIs are not interchangeable. Follow the README included in your chosen download; installation requires both the ASI/menu files and the documented configuration changes. The current public downloads are manual packages.

### What this modification adds

- More complete interactions between vehicle, pedestrian, streetlamp and headlight shadows within the engine's existing shadow budget.
- Protected native lamp choices and cache relationships, replacing the earlier approach that displaced lamp selections.
- Nearby visible vehicles can qualify for headlight shadows while Niko is on foot, without requiring him to stand directly in the beam.
- Player-car relevance and headlight identity/cache safeguards.
- Headlight brightness retention after leaving your car; normal NPC headlight brightness is unchanged.
- Separate headlight and lamppost reach controls, up to 200 feet plus Original. These influence selection, not physical beam length or guaranteed shadow distance.

CE enables an additional bounded camera-based lamp preference before native cache decisions. The recommended legacy configuration leaves that preference off, preserving native lamp selection. Some pop-in and scene-dependent switching can remain; this does not add shadow slots, ray tracing or a true per-shadow fade.

### Credits and source

**Mod project: OnyxOak. Upstream FusionFix: ThirteenAG and contributors.** Existing upstream copyright and dependency notices remain applicable. GPL-3.0 licensing is retained; see [ATTRIBUTION.md](ATTRIBUTION.md).

Matching source is provided with the project's [source releases](https://github.com/onyxoak/fusionfix-extra-night-shadows/releases). In particular, [CE 1.5 source](https://github.com/onyxoak/fusionfix-extra-night-shadows/releases/tag/v1.5) corresponds to that gameplay release. Do not assume the default branch's historical code is identical to every edition or downloadable build; use the matching release source and build identity.

For coverage, please credit **“Extra Night Shadows Fix and Better Headlights by OnyxOak, an unofficial modification of FusionFix by ThirteenAG and contributors.”** Linking readers to the official Nexus page helps avoid stale third-party downloads. This is a request, not an additional restriction on GPL-compliant redistribution.