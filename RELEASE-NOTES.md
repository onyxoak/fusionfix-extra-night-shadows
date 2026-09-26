# 1.3 — Better Headlights and Extended Shadow Reach

Headlights now retain the normal player intensity and range boost after I exit
my car with its lights left on. Driven cars and NPC traffic keep their original
brightness. Headlight and lamppost shadow reach now adjust up to 200 feet;
the default remains 50 feet and the engine's shadow budget is unchanged.

All existing night-shadow improvements are retained. I playtested brightness
retention and camera movement. A small visible brightness difference can remain
on exit; this is not a promise of pixel-identical output or universal compatibility.

Install the ASI and three menu files, then set ConsistentBrightness = 1 in the
[HEADLIGHTS] section of the existing FusionFix INI. Keep the shadow settings in
the README. No installer or replacement personal settings file is included.

Release/Win32 build and slider boundary checks passed. Internal build: 24.
By OnyxOak. Based on FusionFix by ThirteenAG and contributors.
