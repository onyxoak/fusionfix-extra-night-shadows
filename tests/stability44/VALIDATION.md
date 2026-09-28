# CE44 - selected-shadow cache dependencies

Candidate43 provides the user's reported large shadow-stability improvement, but still loses some lamp illumination. This update targets that remaining dropout; it does not change draw distance, brightness, headlight admission, camera source or continuity rules. Legacy and the official comparison are untouched. Nothing published.

The CE43 trace has78rejected lamp-map lookups with cache -1;74coincide with native input removal within2frames. In the native selection routine, dynamic choices are complete before the static cache replacement routine runs. With retained shadows, the distance-only cache picker can select a cache entry still used by the dynamic output.

The new guarded hook at CE527E29 protects cache entries referenced by the actual current seven-slot native selection. If the native proposed victim is protected, it chooses a permissible unprotected entry using the same native unused-age/distance rules. If none qualifies, it returns -1 and native code continues its ordinary cache refresh branch. No light geometry/intensity, final dynamic indices, cache contents, cache identities or stale maps are fabricated. Native entries can still be recycled when no selected shadow depends on them.

Guards validate the original selection function and the entire native cache picker, including relocations. At runtime the hook validates the frame, stack, indices, keys and copied cache decision against the actual native result before applying protection.

Offline verification passed:
- Reproduced the selected-lamp/static-cache eviction conflict and covered no-space, freed entries, unchanged native choices, age and tie cases.
- 1,280,000 dependency-mask cases.
- 100,000 snapshots comparing the policy directly with the audited native machine instructions executing only against test-owned cache data in a separate process.
- Original/relocated executable guards and incompatible-code rejection.
- Existing receiver, lamp conservation, camera, continuity, allocation and headlight suites.
- CE Release Win32 compiled successfully.

Installed SHA256: `979E26B12448582390E9EC69244A408A09C28C2D9A3BB06612B5CFECD8C66757`.
Rollback: `D:\GTA-IV-Collection-Lab\stability-candidate44\rollback-20260927-151615` (the user-tested CE43 binary, INI and CFG).
Settings and other editions hash-verified unchanged. Live hook integration and visual outcome pending at initial installation.

Live startup: CE PID36720 loaded the save with allocator_startup enabled, allocation_ready1 and candidate44 identity. Initial interior run executed8cache-dependency checks with0dependency adapter rejections,0continuity adapter rejections,0camera fallbacks and0rejected shadow-map lookups. The crowded-lamp replacement path and its visible outcome are not established by this startup check. Leave the game available for the user's visual test; do not present this as a verified end to all flicker.
