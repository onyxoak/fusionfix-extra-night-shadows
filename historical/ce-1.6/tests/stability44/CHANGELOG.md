# CE 1.6 - Shadow stability update

By OnyxOak. Complete Edition 1.2.0.59 only; based on FusionFix 5.0.1.

- Improved retention of relevant, selected shadows across the gameplay camera view, reducing repeated switching beneath vehicles as you approach traffic or move the camera.
- Corrected camera selection so reflection and auxiliary rendering views do not incorrectly decide which gameplay shadows stay active.
- Kept lights relevant when their illumination volume reaches the visible scene, even if the light source itself is outside the camera view.
- Protected the native lamp choices and their shadow-cache dependencies. A cache used by a selected shadow is no longer replaced by a competing light through the guarded replacement path; another valid cache is used, or the replacement is deferred. This addresses the lamp illumination dropouts reproduced during development.
- Preserved nearby vehicle headlight receivers, player-car priority, retained headlight brightness after exiting, and the existing reach controls.

The author reports a major stability improvement and no further lamp dropouts in the latest playtest, and has approved this build for release. Automated cache, camera, selection and regression checks passed. Results can vary by scene, hardware and mod setup.

Some shadows can still appear late as you approach traffic (reported around 30-40 feet in some scenes). This release does not increase shadow draw distance or the engine's finite shadow budget, and does not guarantee a shadow from every light at once. Further refinement is planned.

DXVK / Vulkan is required for the supported configuration, alongside FusionFix 5.0.1 and its ASI loader. Native DirectX is not a supported configuration for this release. Diagnostic logging is disabled in the release instructions.

This is a CE-only update. The separate GTA IV 1.0.8.0 download is unchanged.
