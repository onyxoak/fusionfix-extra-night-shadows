# FusionFix Extra Night Shadows 1.3  -  Better Headlights and Extended Shadow Reach

By OnyxOak. An unofficial modification of FusionFix for GTA IV Complete Edition 1.2.0.59, based on FusionFix 5.0.1.

## What I changed

- Headlights on the car I just exited retain the normal player intensity and range boost while its lights remain on. Normal occupied-car and NPC brightness is unchanged. The game still controls damage, high/low beams and light on/off state.

- Fixed NPC headlight shadow selection being disabled when entering a vehicle. Your occupied car can now cast a shadow under a selected NPC headlight. One player beam and one external beam share the existing two-beam budget while driving.
- Kept source vehicles excluded from their own immediate headlight passes while driving as well as on foot.
- Extended self-shadow filtering to selected approaching traffic while on foot. A vehicle is excluded only from its own immediate headlight shadow pass; nearby pedestrians and other vehicles remain eligible casters.
- Added separate Headlight shadow reach and Lamppost shadow reach sliders under Settings > Game, below Extra Night Shadows. Both start at 50 feet, adjust in five-foot steps up to 200 feet, and have an Original position.
- Retained the earlier vehicle-under-lamppost and close-range player-shadow changes.

The sliders extend how nearby lights are prioritized for detailed shadows. They do not extend physical headlight beams, add shadow slots, render every vehicle, or guarantee shadow detail out to the selected distance. Visibility, engine light eligibility and the existing shadow budget still apply. Original restores the prior relevance policy, not official FusionFix.

## Manual installation

1. Close GTA IV. Use Complete Edition 1.2.0.59 with official FusionFix 5.0.1 and its ASI loader installed. Other executables are unsupported. Expected GTAIV.exe SHA-256: `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`.
2. Back up your existing FusionFix ASI, INI and CFG, plus these three menu files, outside the game folder: `update/common/data/frontend_menus.xml`, `update/TLAD/common/data/frontend_menus.xml`, `update/TBoGT/common/data/frontend_menus.xml`.
3. Copy the CONTENTS of this package's `install` folder into your GTAIV game folder, merging folders and replacing the existing ASI and three menu XML files. The menu files are required for the sliders. Do not keep a second renamed FusionFix ASI in any game folder. Mods editing these same menus require a manual merge.
4. In the existing `[SHADOWS]` section of `plugins/GTAIV.EFLC.FusionFix.ini`, add or update:

```ini
ExperimentalPlayerShadowAllocation = 2
ExperimentalOwnHeadlightCasterFix = 1
ExperimentalShadowDiagnostics = 1
ExperimentalCloseHeadlightRelevance = 1
ExperimentalTrafficSelfShadowFix = 1
```

5. In the existing `[SHADOWS]` section of `plugins/GTAIV.EFLC.FusionFix.cfg`, set `ExtraNightShadows = 3`. The new sliders default to `HeadlightShadowReach = 10` and `LamppostShadowReach = 10` (five feet per step). Zero selects Original; 40 means 200 feet. Change these in the game menu. In `[UPDATE]`, set `CheckForUpdates = 0` to prevent official updates replacing the custom ASI. Preserve unrelated settings and avoid duplicate sections.
6. In plugins/GTAIV.EFLC.FusionFix.ini, add a [HEADLIGHTS] section with ConsistentBrightness = 1. Set it to 0 to disable brightness retention. Do not duplicate an existing section.
7. Launch normally. Use lampposts and headlights plus vehicle night shadows. Diagnostics are written to `plugins/GTAIV-shadow-candidate21.log` when enabled.

No installer, scripts, standalone EXEs or nested archives are included in the runtime download. The ASI is executable mod code and hosting scans remain controlled by the host.

## Test status

Release/Win32 compilation and automated selector, shadow-budget, caster, allocation-pass and hook-order checks passed. These include driving transitions, external beams, damaged-headlight identity changes and shuffled traffic selection.

I tested brightness retention and camera movement in my game. A small visible brightness change on exit can remain. Earlier occupied-vehicle shadow fixes remain included. Coverage is limited; this is not a claim that every vehicle or mod combination is verified. The existing shadow budget still limits which nearby lights cast detailed shadows. Combined headlight beams and deferred-caster limitations remain. No performance gain is claimed.

## Rollback

Close the game and restore all six backed-up files (ASI, INI, CFG, and three menu XMLs). Restore your earlier update-check setting if returning to official FusionFix. This package does not include save files.

## Source and credits

Full matching source and offline tests: https://github.com/onyxoak/fusionfix-extra-night-shadows/releases/tag/v1.3

FusionFix is by ThirteenAG and its contributors. I identified the problems, directed changes and playtested them, with coding assistance from OpenAI Codex. GPL-3.0 and dependency notices are retained. This is not an official FusionFix release.
