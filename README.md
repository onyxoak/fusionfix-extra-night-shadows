# FusionFix Extra Night Shadows 1.1 — Traffic Shadows and Distance Controls

By OnyxOak. An unofficial modification of FusionFix for GTA IV Complete Edition 1.2.0.59, based on FusionFix 5.0.1.

## What I changed

- Extended self-shadow filtering to selected approaching traffic while on foot. A vehicle is excluded only from its own immediate headlight shadow pass; nearby pedestrians and other vehicles remain eligible casters.
- Added separate Headlight shadow reach and Lamppost shadow reach sliders under Settings > Game, below Extra Night Shadows. Both start at 50 feet, adjust in five-foot steps up to 75 feet, and have an Original position.
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

5. In the existing `[SHADOWS]` section of `plugins/GTAIV.EFLC.FusionFix.cfg`, set `ExtraNightShadows = 3`. The new sliders default to `HeadlightShadowReach = 10` and `LamppostShadowReach = 10` (five feet per step). Zero selects Original; 15 means 75 feet. Change these in the game menu. In `[UPDATE]`, set `CheckForUpdates = 0` to prevent official updates replacing the custom ASI. Preserve unrelated settings and avoid duplicate sections.
6. Launch normally. Use lampposts and headlights plus vehicle night shadows. Diagnostics are written to `plugins/GTAIV-shadow-candidate21.log` when enabled.

No installer, scripts, standalone EXEs or nested archives are included in the runtime download. The ASI is executable mod code and hosting scans remain controlled by the host.

## Test status

Release/Win32 compilation passed. New reach tests passed 62 checks, the existing seven-slot budget suite passed 754,197 checks, and selector tests passed 3,421,980 checks. The earlier traffic candidate passed 141 new caster checks and the existing caster/guard suite. These are offline correctness checks, not proof of gameplay stability.

I reproduced flicker on the published build with a Rancher and Crown Vic-style police car and taxi in the minimal setup. I subsequently reported no longer seeing that issue while testing the newer build. Coverage is limited; this is not proof that every affected vehicle or situation is fixed. The slider settings were saved and the test game launched successfully; the amount of visible pop-in reduction and performance cost have not been measured. The combined headlight beam and deferred-caster limitations remain.

## Rollback

Close the game and restore all six backed-up files (ASI, INI, CFG, and three menu XMLs). Restore your earlier update-check setting if returning to official FusionFix. This package does not include save files.

## Source and credits

Full matching source and offline tests: https://github.com/onyxoak/fusionfix-extra-night-shadows/releases/tag/v1.1

FusionFix is by ThirteenAG and its contributors. I identified the problems, directed changes and playtested them, with coding assistance from OpenAI Codex. GPL-3.0 and dependency notices are retained. This is not an official FusionFix release.
