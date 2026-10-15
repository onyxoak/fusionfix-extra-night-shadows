# Private 2.0 configuration

Edit plugins/OnyxOak.ExtraNightShadows.ini beside the ASI, then restart the game. Menu toggles are not required for these startup switches. This is a private engineering candidate, not an accepted public release.

| Section / option | Effect |
|---|---|
| SHADOWS / Enabled | Master switch: 0 leaves the addon inactive |
| SHADOWS / ExtraNightShadows | Enables the inherited additional night shadows and the requested native keyed-world promotion |
| SHADOWS / HeadlightShadows, VehicleNightShadows | Enable inherited player/traffic headlight handling; both must be enabled for dual headlight splitting |
| SHADOWS / LamppostShadowReach | Player/camera world-light relevance setting, clamped 0–100; default 40 |
| SHADOWS / HeadlightShadowReach | Vehicle-light relevance setting, clamped 0–100; default 10 |
| LIGHTING20 / DualHeadlights | Two actual native front-lamp attachments per eligible car |
| LIGHTING20 / TailLightShadows | Shadows from existing native colored rear illumination, preserving switching/damage/color; combined beams remain combined |
| LIGHTING20 / NativeKeyedWorldShadows | Adds dynamic eligibility to matching native keyed/cache-authorized world lights |
| LIGHTING20 / ModelLightShadows | Adds dynamic shadow identity to authenticated unshadowed native model point/spot emissions from current building/object owners; preserves authored gates, RGB and shape |
| LIGHTING20 / CoronaLightShadows | Reconstructs point illumination for active authored type0 glow-only fixtures with current static owner/model/descriptor identity; native glow remains |
| LIGHTING20 / CoronaApproximation | Allows missing physical radius/intensity to use configured approximations; 0 requires positive authored physical values |
| LIGHTING20 / CoronaRadiusPerAuthoredSize | Approximate radius multiplier for positive authored glow size when physical radius is absent; default 12 |
| LIGHTING20 / CoronaFallbackIntensity | Approximate intensity when authored intensity is absent; default 1 |
| LIGHTING20 / CoronaIntensityScale | Reconstruction intensity multiplier; default 1 |
| LIGHTING20 / CoronaMinRadius, CoronaMaxRadius | Bounds for approximate radius only; default 0.1–128 |
| LIGHTING20 / CoronaMaxIntensity | Bound for approximate intensity only; default 64 |
| LIGHTING20 / WorldGeometryCasters | Includes native static collection and ordinary-building admission mask in immediate dynamic passes |
| LIGHTING20 / TrafficSignalShadows | Controls authenticated traffic signal promotion; older LAB TrafficObserver/TrafficPromote remain fallback settings |
| LIGHTING20 / SignalShadowPriority | Continuous ranking preference for currently authenticated signal lights affecting visible receivers; default 1.5, range 1–4; 1 neutral. Does not add maps or override player-headlight precedence |
| LIGHTING20 / EmergencyLightShadows | Controls implemented nearby emergency-light handling; older LAB PoliceTest/PoliceNearby remain fallback settings |
| LIGHTING20 / ModelEffectCatalog | Sampled source diagnostics only; does not create illumination |
| SUPPORT / Logging | 0 disabled logs, 1 aggregate support evidence, 2 detailed diagnostics; disabling logs does not disable geometry/model shadows |
| SUPPORT / MaxLogKiB | Rotating log size; candidate uses 2048 |
| SUPPORT / DiagnosticCounters | With Logging=2, enables detailed geometry bucket census and hot diagnostic counters; default 0 avoids this optional work while keeping actual caster admission active |

All feature switches use 0/1. Tail/model/traffic/emergency allocation is independent of the front-headlight switches. FusionFix's underlying engine settings still constrain its passes: an INI switch cannot promise to render an engine feature disabled by FusionFix. The seven native active maps remain shared by eligible sources.

The addon no longer rejects a file merely because its hash or PE timestamp differs. It identifies a supported hook layout from the actual process callback and authenticates used instructions, native producer ABIs, mapped executable code and foreign hook ownership. This is capability checking, not a promise of arbitrary executable/FusionFix compatibility. Unknown or conflicting code is refused. Private staging/launch scripts retain exact-path/hash guards to protect installation lanes.

Native-model promotion preserves existing illumination. The separate corona module adds new illumination to authenticated active authored glow-only descriptors citywide; authored positive physical radius/intensity are retained, while missing values use explicit approximation settings. Corona far-clip/draw distance is never used as physical radius. Some native brightness/distance-fade gates can still suppress a light while glow is visible. Shader-emissive surfaces and baked glow with no authenticated authored light/corona descriptor still need additional producers. Full directional point-light receiver coverage, deferred rendering and universal simultaneous coverage remain unresolved. Feature testing with FusionFix and without FusionFix is still required.

Build18 adds compatible static world material local-shadow techniques in separate private shader files. The full FusionFix staging mode installs those reversible shader additions; other modes restore their exact originals. Existing techniques/material cutout behavior remain, and WorldGeometryCasters still controls new dynamic static-world admission. Stage-WorldLocalShaders18.ps1 -Enabled $false restores the prior shader set with the game closed. These shader additions address a missing depth capability; floor-bleed and headlight building occlusion require Austin's playtest, and material-sidedness/unsupported terrain or skinned shaders remain outside this patch.

Build21 scene allocation: [LIGHTING20] ScenePriorityStrength default1.5, range0..4. Zero restores build20 distance/tier ranking. Positive enables continuous receiver/road/brightness/colored-contrast priority instead of the hard nearby-lamp tier jump. ScenePriorityReleaseMs default450, range50..1500, smooths declining importance of still-current same-generation lamp sources. Acquisition rises immediately; this does not postpone owner retirement, keep missing lights/maps alive or fade the actual shadow. Own/native-special precedence and seven slots remain. These are geometric scene-impact estimates, not hardware depth occlusion or ray tracing. Sources outside camera can qualify through their visible influence volume. Actual performance/pop-in needs author test.


Private31: [SUPPORT] WorkingSetValidation=0 (default when absent) uses fresh
committed-region/protection queries for scalar memory checks. Setting1 restores
build30 working-set queries for controlled comparison. Owner/generation/lifetime
and copy-race safeguards remain. Restart required; diagnostic startup control.
