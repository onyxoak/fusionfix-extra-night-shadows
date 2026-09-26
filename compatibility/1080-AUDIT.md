# GTA IV 1.0.8.0 compatibility audit

Status: initial binary audit and isolated baseline staged; NOT a working port.
The CE main branch and release remain unchanged. Do not install the CE shadow
ASI into a legacy copy or relax its executable guards.

## Baseline
Owned clean Complete Edition files copied into an independent D-drive folder.
Official Rockstar EFIGS Patch 8 files extracted without executing its installer.
The staged executable reports 1.0.8.0. Official FusionFix 5.0.1 and its Legacy
Addon are installed for a baseline comparison. Liberty's Legacy 2.4.1 trainer
and ScriptHook are included; the CE-specific compatibility hook is omitted.
No game launch, save conversion, or successful trainer operation is claimed yet.
Game files and third-party binaries are private, not committed or redistributed.

## Verified initial differences
CE mapped image size: 0x01BE6400; retail 1.0.8.0: 0x018B2000.
The exact CE headlight submission-wrapper signature occurs once in CE and zero
times in the retail executable. The CE vehicle caster signature likewise differs.
The existing alternate vehicle-caster signature occurs at retail RVA 0x55A41B;
this is only a candidate location, not proof of matching register/stack semantics.

## Required port work
- Verify legacy launch and trainer time/weather/vehicle controls independently.
- Identify headlight submission functions, argument order, keys and frame counter.
- Audit the seven-slot allocation collection/finalization contract and data layout.
- Audit immediate caster context and occupied-car/occupant handling.
- Re-map brightness-retention hooks and verify vehicle pool identities.
- Confirm camera viewport layout and menu integration for legacy rendering.
- Create separate executable fingerprint and byte guards; never reuse CE offsets.
- Run allocation/caster tests and legacy gameplay tests, including phone calls,
  driving transitions, headlight damage, rain and dense traffic.

The portable scoring and selection policies are reusable, but native adapters
must be validated separately. No release date or compatibility promise yet.
