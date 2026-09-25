# FusionFix Extra Night Shadows Fix (Unofficial)

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

- GTA IV Complete Edition **1.2.0.59**, using the audited executable identified below.
- A working **FusionFix 5.0.1 baseline** installation and ASI loader. The source baseline is commit `619f52d87681ae7dd6f35795208c3b3759f9632c`; the custom DLL retains upstream product metadata `5.0.1.1-619f52d`.
- Windows; single-player use. Other game versions and multiplayer have not been validated.

**Install FusionFix first, then install this package over it.** This is a modified replacement for `GTAIV.EFLC.FusionFix.asi`, not another ASI to load alongside it. Compatibility with future FusionFix versions is not assumed.

## Installation (manual package, September 25, 2026)

This repack contains the experimental build 19 ASI. It contains no installer scripts, standalone EXE files, or nested archives. The ASI is still executable mod code; Nexus may require security review. Packaging verification is not a malware certification or a new gameplay test.

1. Use GTA IV Complete Edition 1.2.0.59 with a working FusionFix 5.0.1 baseline and ASI loader. Run that installation once to generate its INI and CFG, then close GTA IV.
2. Verify the game executable matches the audited SHA-256 below. Unlike the previous installer, manual copying cannot enforce this prerequisite. In PowerShell, run the following command with your actual game path, and compare the output before proceeding:

   ```powershell
   Get-FileHash -Algorithm SHA256 -LiteralPath 'D:\YOUR-GAME-FOLDER\GTAIV.exe'
   ```

   Expected game executable SHA-256:
   `08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D`

   Stop if the hash does not match. Other executable builds are not validated.
3. Copy your existing `plugins/GTAIV.EFLC.FusionFix.asi`, `plugins/GTAIV.EFLC.FusionFix.ini`, and `plugins/GTAIV.EFLC.FusionFix.cfg` into a dated backup folder OUTSIDE the game directory. Keep all three for rollback.
4. Extract this ZIP to a temporary folder. Copy only `install/plugins/GTAIV.EFLC.FusionFix.asi` into the game's existing `plugins` folder, replacing the original. Do not leave an extra renamed FusionFix ASI in any game folder.
5. In the existing `[SHADOWS]` section of `plugins/GTAIV.EFLC.FusionFix.ini`, add or update:

   ```ini
   ExperimentalPlayerShadowAllocation = 2
   ExperimentalOwnHeadlightCasterFix = 1
   ExperimentalShadowDiagnostics = 1
   ExperimentalCloseHeadlightRelevance = 1
   ```

6. In the existing `[SHADOWS]` section of `plugins/GTAIV.EFLC.FusionFix.cfg`, set `ExtraNightShadows = 3`. In that CFG's existing `[UPDATE]` section, set `CheckForUpdates = 0` to prevent an official update replacing this custom ASI. Add a section only if it is absent; preserve all unrelated settings. Restore the original update setting when reverting to official FusionFix.
7. Start GTA IV normally. The ASI's runtime guards remain unchanged.

The original Extra Night Shadows warning is hidden when both guarded shadow fixes are active. It remains if the required hooks are unavailable or the fix is disabled.

## Verification and rollback

The installed ASI must have SHA-256:
`EA2C24731233182F551DC7E54C674C061AA45870C0CFD8438516DBF5A24413B8`

Use `Get-FileHash` as above with the ASI's path to verify it.

`plugins/GTAIV-shadow-candidate19.log` should report `allocator_startup enabled`, `allocation_ready=1`, and `caster_enabled=1`. Increasing counters establish activation, not visual correctness. Check night driving and on-foot shadows in your own setup.

To undo this manual installation, close GTA IV and restore all three files from your dated backup. There is no installer or Restore script in this manual package. Keep older installer backups if you already used an earlier package.

## Release status

Experimental build 19 adds close-range on-foot headlight relevance to the earlier player-focused shadow allocation and own-vehicle headlight exclusion. It prioritizes nearby beams that intersect a conservative body volume instead of relying only on a single player-position point. It does not enlarge the rendered light cone or remove the engine shadow budget.

The Release/Win32 build succeeded. 96 new geometry checks and 39 existing light-volume checks passed. Austin reports that this build fixes the near-bumper player-shadow disappearance in his gameplay test. Screenshots document this setup; they are not proof that every car, location, or mod combination is fixed. No new FPS improvement is claimed. The immediate caster path is covered; the deferred caster path remains outside the earlier own-car exclusion fix.

Some distant shadow flicker may remain. Test compatibility in your own setup and report the vehicle, location, other graphics mods, settings, and whether the issue occurs on foot or driving. The original Extra Night Shadows warning is hidden only when the earlier guarded fixes are active; that indicator is not proof of universal correctness.

## Source, permissions, and credits

The exact complete matching source archive, populated dependencies, license notices, build instructions, and offline tests remain available as a separate download on the public release:

https://github.com/onyxoak/fusionfix-extra-night-shadows/releases/tag/build19-preview

Direct corresponding-source download:
https://github.com/onyxoak/fusionfix-extra-night-shadows/releases/download/build19-preview/FusionFix-Extra-Night-Shadows-Unofficial-build19-source.zip

Source archive SHA-256:
`9F4A21ABF0C75B712A996F166288DF1010D5D67FAF31C62F4AD7A128C37A35A9`

This is the complete matching build 19 source, including its dependencies, not the distinct upstream proposal. Keep this source link alongside the runtime download. If redistributing, provide access to the complete matching source and preserve all license notices. The source archive includes required dependency build tools and is hosted separately on GitHub; it is not nested inside this manual runtime ZIP.

FusionFix: **ThirteenAG and the FusionFix contributors**, whose work this modification builds on. [Original project](https://github.com/ThirteenAG/GTAIV.EFLC.FusionFix).

Requirements, reproduction examples, and repeated gameplay testing: **Austin**. Implementation and release preparation were developed with **OpenAI Codex**. This is unofficial and is not endorsed by the FusionFix maintainers.

FusionFix's GPL-3.0 license and attribution are retained. The modifications are provided under those terms; third-party components retain their own included licenses. Follow those licenses when modifying or redistributing. This release does not claim ownership of upstream FusionFix or third-party code.
