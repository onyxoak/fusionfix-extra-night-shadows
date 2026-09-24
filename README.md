# FusionFix Extra Night Shadows Fix (Unofficial)

[Download the preview release](https://github.com/onyxoak/fusionfix-extra-night-shadows/releases/tag/build18-preview) — installer, complete source, and checksums.

**Want vehicle shadows under streetlamps and headlight-cast shadows together?**

This mod grew out of a frustrating nighttime tradeoff: getting the car's shadow under lamps, keeping headlight shadows while driving, and seeing Niko cast a shadow when walking in front of a car. In our test setup, attempts to combine those effects produced flickering and headlights that cut off or broke up depending on the car's direction.

This unofficial FusionFix modification targets those problems. It gives nearby lights around the player priority and prevents the occupied car from obstructing its own headlight shadow pass, while retaining the car's shadows from streetlamps.

## What it targets

- Vehicle shadows cast by nearby streetlamps, including while you are driving.
- Headlights casting shadows on nearby objects without the driving-only cutoff seen in our tests.
- Niko casting a shadow when walking through headlights, including nearby traffic headlights.
- More stable selection of nearby shadow-casting lights when traffic passes.

It keeps FusionFix as the foundation. It does not add unlimited shadow slots or guarantee every nearby light can cast a shadow simultaneously. This is not a blanket self-shadowing feature: the car is deliberately excluded from blocking its own headlights.

## Requirements

- GTA IV Complete Edition **1.2.0.59**, using the audited executable supported by the installer.
- A working **FusionFix 5.0.1 baseline** installation and ASI loader. The source baseline is commit `619f52d87681ae7dd6f35795208c3b3759f9632c`; the custom DLL retains upstream product metadata `5.0.1.1-619f52d`.
- Windows; single-player use. Other game versions and multiplayer have not been validated.

**Install FusionFix first, then install this package over it.** This is a modified replacement for `GTAIV.EFLC.FusionFix.asi`, not another ASI to load alongside it. Compatibility with future FusionFix versions is not assumed.

## Installation

1. Install the compatible FusionFix baseline and run the game once so its configuration files exist.
2. Close GTA IV. Extract the runtime ZIP into a temporary folder.
3. Run `Install.cmd` and select your game's `GTAIV.exe`.
4. Start GTA IV normally.

The installer verifies the supported executable and release DLL, backs up the existing FusionFix ASI/INI/CFG, installs the replacement, and sets the required shadow options. It preserves other graphics settings. Official FusionFix update checks are disabled to prevent an update replacing the custom ASI; the original setting is included in the rollback backup. Protected game folders may require running the installer as administrator.

The original Extra Night Shadows warning is hidden when both guarded shadow fixes are active. It remains if the required hooks are unavailable or the fix is disabled.

### Manual installation

Back up the three existing FusionFix files, then replace the ASI with `install/plugins/GTAIV.EFLC.FusionFix.asi`. In the existing `[SHADOWS]` section of the INI, set:

```ini
ExperimentalPlayerShadowAllocation = 2
ExperimentalOwnHeadlightCasterFix = 1
ExperimentalShadowDiagnostics = 1
```

In the CFG, set `[SHADOWS] ExtraNightShadows = 3` and `[UPDATE] CheckForUpdates = 0`. Keep other settings. Do not create duplicate sections or keep a second renamed FusionFix ASI in the game folders.

## Verification and rollback

`plugins/GTAIV-shadow-candidate18.log` should report `allocator_startup enabled`, `allocation_ready=1`, and `caster_enabled=1`. Counters increasing during gameplay establish that the code is active; they do not by themselves prove the image is correct.

To undo the installation, close GTA IV, run `Restore.cmd`, and select the same game executable. This restores the most recent installer backup. Backups are kept in `ExtraNightShadows-backups` under the game directory. For a manual install, restore your own three-file backup.

## Release status

**Experimental build 18.** Build 17 passed offline checks and live activation checks, and the tester reported that the driving flicker appeared fixed. Build 18 keeps that shadow-rendering logic and adds the conditional menu-warning change; it has compiled successfully, but its in-game menu behavior has not yet been visually verified. The installer is tested separately in an isolated copy.

Testing so far is limited to one PC and mod setup. No FPS improvement is claimed. The own-car exclusion covers the immediate rendering path; the deferred path is not covered and was not observed in the verified session. Please report the game version, other graphics mods, vehicle, location, direction, and whether the problem occurs in or out of the car if you encounter an issue.

## Source, permissions, and credits

Full modified source, populated dependencies, license notices, build instructions, and offline test sources are supplied in the source download. The source snapshot and the runtime download are separate files; obtain both if redistributing this build. No game executable or saves are supplied.

FusionFix: **ThirteenAG and the FusionFix contributors**, whose work this modification builds on. [Original project](https://github.com/ThirteenAG/GTAIV.EFLC.FusionFix).

Requirements, reproduction examples, and repeated gameplay testing: **Austin**. Implementation and release preparation were developed with **OpenAI Codex**. This is unofficial and is not endorsed by the FusionFix maintainers.

FusionFix's GPL-3.0 license and attribution are retained. The modifications are provided under those terms; third-party components retain their own included licenses. Follow those licenses when modifying or redistributing. This release does not claim ownership of upstream FusionFix or third-party code.

## Repository layout

The Releases section supplies the installable runtime ZIP and the complete corresponding source ZIP, including populated dependencies and shader resources. Download both for redistribution. Extract the runtime archive before running its installer; the repository does not contain the installable ASI.

`modified-source` contains the changed/custom shadow code for review. `tracked-changes.patch` covers changes to tracked upstream files at the pinned commit; new custom files are provided separately here and in the full source archive. Use the full source download for building and running the offline tests in their original folder layout. The installer scripts in this repository match the runtime package.
