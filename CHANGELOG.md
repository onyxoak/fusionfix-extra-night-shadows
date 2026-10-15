# Release history

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
