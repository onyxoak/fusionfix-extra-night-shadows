# CE stability candidate 43 - 2026-09-27

Installed CE SHA256: `E6A876AA1FC694789B2F8D3FF0EDA407C468B5145F9B669D189702F85255EE29`.
Backup and installation receipt: `D:\GTA-IV-Collection-Lab\stability-candidate43`.
INI/CFG preserved byte for byte on installation. Legacy and official comparison unchanged. Not published.

## Evidence and correction

The rejected candidate42 recording begins at the South Bohan dumpster on Hollowback Street. Reviewed the beginning and sampled later frames. Its detailed trace repeatedly alternates ordinary camera matrices with mirrored and side-facing render views. Resolution/perspective checks had incorrectly treated these other render views as the gameplay camera. The continuity policy consequently released still-visible light volumes.

Candidate43 reads the authoritative scene-camera table used by the native CE selection routine. Executable guards validate that table and the camera object's viewport offset. It retains a valid selected light when the light's bounding volume intersects the gameplay frustum, even if the source itself is outside the frame. Missing camera data does not establish that a light has left view. Native light eligibility, range, identity, cache scheduling, and the seven dynamic slots remain required.

The candidate compiled successfully. Focused executable guard/relocation and light-volume tests passed, followed by the existing continuity, full-frame view, receiver, lamp preservation, allocation, budget and headlight selector regression suites.

## Live run

CE PID7228 launched candidate43. Scene-camera reads and allocation readiness confirmed. No camera fallbacks or comparator adapter rejections in the reviewed log. Gameplay capture saved as `D:\GTA-IV-Collection-Lab\stability-candidate43\live\CE43-smoke-20260927-145836.mp4`; 1661 frames, approximately112seconds, with capture timestamps. Actual capture rate approximately15fps, so this cannot independently exclude every single game-frame artifact. Capture includes a paused map from about40seconds onward.

Initial70seconds of trace: 55lamp removals, 50no longer in native eligible input and5outside configured retention reach; no camera-visibility releases. Extended trace through335.6seconds: 471removals, including416missing native input,44outside reach,5outside view,4without prior claim,2despite a claim. Do not interpret every removal as a visible flicker. There are dropped diagnostic events; counts are observations, not an exhaustive audit. The extended sample is not a matched performance/visual comparison with42.

User's live report: flickering currently gone, shadows hold until walking away, and a "massive stability improvement." This is positive visual validation in the tested scenes, not a guarantee across all scenes or a publication decision.

## Remaining report: acquisition distance

User still sees shadows initially appear only around30-40feet. Confirmed via question that this concerns BOTH streetlamp shadows under vehicles and headlight shadows. Preserve43 as the successful stability baseline; do not weaken its retained-light policy just to replace shadows sooner.

Read-only checks: CFG LamppostShadowReach40 means200feet (five-foot steps), HeadlightShadowReach10 means50feet. On-foot nearby-vehicle receiver discovery reaches80feet. NPC source eligibility also considers native light radius. There is no established common30-40foot cutoff in these checks. The reported late appearance could involve first allocation under a full retained set, native cache eligibility, or caster inclusion; its cause has not yet been isolated. Raising the slider alone is not a verified fix.

No distance changes have been installed. User is still playtesting the stable43 build; leave the running game unchanged.

## Subsequent correction to acceptance

User subsequently reported occasional LAMP ILLUMINATION dropouts in dense scenes. Candidate43 is a substantial shadow-stability improvement, NOT a complete fix or release-ready verdict. User explicitly reminded us that preserving native lamp choices/cache coherence had fixed this earlier; do not replace that invariant with more ranking tweaks.

Final43 diagnostics: 78rejected dynamic-map lookups, all with lookup cache -1,74associated with removal from native eligible input within2frames. This repeats the earlier cache-loss pattern. Native cache picker CE925D40 can choose a cache entry still referenced by the current dynamic selection; strict continuity can make its distance ranking differ from the native cache ranking. Candidate44 protects these dependencies at527E29 before replacement, without moving selected dynamic indices or changing the successful43 camera/visibility rules. In-game visual validation remains necessary.
