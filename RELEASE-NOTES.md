# 1.4 — Camera-Aware Shadow Priority

Added a bounded camera-view preference to the existing shadow allocator. Lights
whose sampled influence overlaps the view receive higher priority, while the
player-focused reservation groups, seven managed slots and selection hold time
remain unchanged. A light source does not have to be on screen to benefit.

This estimates screen relevance; it does not detect walls, add shadow slots or
guarantee every visible shadow. The upstream headlight admission policy remains
player-centered. Unsuitable camera data uses the previous ranking.

All existing shadows, brighter headlights after parking and separate 200-foot
reach sliders remain. Defaults are still 50 feet.

Install the ASI and menu files together. Add CameraAwareShadowPriority = 1 under
[SHADOWS] in the existing FusionFix INI, alongside the README settings. Set it to
0 and restart to compare the previous allocation. No installer is included.

I playtested this build and accepted its appearance. Runtime logs confirm the
camera weighting activates. Release/Win32 compilation, 728,997 budget checks,
4,960 allocation transaction checks and targeted view-priority checks passed.
No measured performance gain or universal compatibility is claimed.

Internal build: 25. By OnyxOak. Based on FusionFix by ThirteenAG and contributors.
