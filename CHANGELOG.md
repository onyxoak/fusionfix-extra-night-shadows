# Release history

## 2.0.5

- Reduced repeated validation, vehicle scans and unnecessary shadow-caster processing; added configurable conservative caster culling.
- Uses discovered hooks and required bindings rather than unfamiliar FusionFix version numbers as a rejection criterion. Includes tested FusionFix 5.1.2 / Volumetric Lights 2.00 integration improvements.
- Added vehicle shadow-box fallback, with special thanks to **MrBulkinAleksey** for the original implementation in his FusionFix fork. Adapted and integrated by OnyxOak.
- Improved layered callback handling and night-state status; fixed the captured invalid-negative-index MMA10 shutdown corruption.
- Added the optional **Fix Only — No Enhancements** configuration using the same core, with the preset menu and enhancements disabled and no replacement shaders required.
- Headlight shadows from lamp poles remain unresolved. Further performance, edge-case flicker and shadow transitions remain under refinement.

See [full changes and test limits](release-notes/2.0.5.txt) and [fix-only changes](release-notes/2.0.5-fix-only.txt). The full changelog's roughly 15% result compares development builds in one controlled scene; it is not a direct 2.0.4 performance result or a universal FPS promise.

## 2.0.4

- Corrected version-specific startup adapters for official FusionFix 5.0.1 and 5.1.0, including the matching VolumetricLights paths.
- Added clearer headlight-conflict diagnostics: the first mismatched instruction and callback-owner details where available.
- Added the camera-weighted indoor shadow-priority bubble, extending up to 35 feet around the player.
- Includes the fully commented configuration INI and tuning guide. High remains the default; routine diagnostic tools are off.
- Retains the performance and scenery-shadow improvements from 2.0.3. No additional measured FPS gain is claimed for 2.0.4.


## 2.0.3

- Added compatibility with official VolumetricLights v2.00.
- Reduced repeated CPU work in shadow processing and light validation.
- Added scenery-shadow transitions near the distance limit and brief retention of available static maps.
- Updated lamp-pole handling using existing shadow maps and corrected two low arcade-light placements.
- Added clearer startup errors and compatibility troubleshooting. High remains the default; routine profiling and recording are off.


## 2.0.2

- Added protection against the native model-reference queue overflow investigated during nighttime crashes.
- Reduced redundant shadow-caster CPU work while retaining the same lighting, range, brightness and geometry coverage.
- Kept High as the default, with routine diagnostic tools off.
- This historical version predates the VolumetricLights compatibility adapter; do not use that combination with 2.0.2.


## 2.0.1

- Improved priority for visible scenery lights; High considers scenery shadows out to 250 feet.
- Reduced unnecessary background processing and disabled routine diagnostics.
- Reduced headlight brightness by 15 percent and raised traffic-light spill by 5 percent. Headlight and taillight reach remained unchanged.
- Physical lamp-pole shadows and brightness changes when leaving a car remained known issues.


## 2.0

- Expanded supported shadow interaction across city lights, people, vehicles and scenery.
- Added dual-headlight and taillight enhancements on High, with traffic and emergency-light shadows from Medium.
- Added Low, Medium and High presets and the matching shader package.
- Expanded lighting coverage while keeping further optimization and visual refinement ongoing.


Older versions: see their original GitHub release notes and historical/ce-1.6/.
