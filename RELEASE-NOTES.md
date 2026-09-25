# Build 19 — manual installation and close-range headlight shadows

This experimental update addresses the request for an ASI-only manual installation and the reported disappearance of the player's shadow near headlights.

- No installer commands, PowerShell scripts, standalone EXEs, or nested archives in the runtime download.
- Retains the earlier player-focused night-shadow allocation and own-car headlight caster filtering.
- Adds conservative close-range on-foot beam relevance. Austin reports that the near-bumper shadow now appears in his playtest.
- Includes manual installation, configuration, verification, rollback, credits, and a link to the exact matching source.

Requires GTA IV Complete Edition 1.2.0.59 and the FusionFix 5.0.1 baseline. This replaces the existing FusionFix ASI; do not install a second renamed copy. Read README.md and add all four experimental INI entries, including ExperimentalCloseHeadlightRelevance = 1.

Release/Win32 build succeeded; 135 geometry checks passed. Tested by one player on one modded setup. No universal compatibility, complete shadow fix, or FPS improvement is claimed. The engine's dynamic-shadow budget and the earlier deferred-caster limitation remain. Nexus scanning is independent of this GitHub release; approval is not guaranteed.

FusionFix remains the work of ThirteenAG and its contributors. Austin/onyxoak supplied requirements, reproductions, and gameplay testing. Custom code and release preparation used OpenAI Codex. GPL-3.0 and dependency notices are retained. Full corresponding source is attached separately.
