# Release history

## 2.0.4

Corrected official FusionFix 5.0.1/5.1.0 adapters that previously used 5.1.1-specific pool/model/backend/copy addresses, including matching VolumetricLights paths. Added canonical headlight-failure evidence and callback-owner details where available. Includes the camera-weighted indoor 35 ft priority bubble and documented configurableINI/tuning guide. High remains the default; routine diagnostics are off.141 offline checks passed; older modpack compatibility and indoor improvements need broader runtime testing. No new FPS gain, universal conflict fix, physical pole/fire fix or RESC 10 fix is claimed.

## 2.0.3

Added compatibility with official VolumetricLights 2.00. Reduced repeated CPU work in shadow processing and validation. Added scenery-shadow distance transitions and brief retention of available static maps. Updated pole handling using existing maps and corrected two low arcade-light placements. Improved startup-conflict guidance. Heavy nighttime scenes and missing model-specific pole shadows remained open.

## 2.0.2

Added protection against the native model-reference queue overflow investigated during nighttime crashes. Reduced redundant shadow-caster CPU work without changing lighting, range, brightness or geometry coverage. High remained the default, with routine diagnostics off. VolumetricLights was not compatible with this historical version.

## 2.0.1

Improved scenery priority and reduced unnecessary background processing. High considers scenery shadows out to 250 ft; headlight/taillight reach remains unchanged. Headlights were reduced 15 percent and traffic-light spill raised 5 percent. Physical lamp-pole shadows and the brightness change when leaving a car remained unresolved.

## 2.0

Expanded supported city and vehicle lighting, including dual headlights and taillights on High and traffic/emergency-light shadows from Medium. Added Low/Medium/High choices and matching shaders. Further optimization and model-specific shadow refinements remained ongoing.

Older release notes remain on their GitHub releases and in historical/ce-1.6/RELEASE-NOTES.md.
