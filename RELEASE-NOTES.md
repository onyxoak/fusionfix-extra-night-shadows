# 1.2 — Occupied Vehicle Headlight Shadows

I fixed NPC headlight shadows disappearing when entering a vehicle. The selection now allows a nearby NPC beam while driving, so the occupied car can cast a shadow under it. Cars remain excluded from their own immediate headlight shadow pass.

The existing two-headlight and seven-light shadow budgets are unchanged. Distance controls and the earlier fixes remain included. I confirmed the reported behavior works in my playtest; wider vehicle and mod compatibility still needs feedback.

Manual installation: replace the ASI and three menu XML files from the install folder, and retain the five INI settings described in the README. No installer is included. If upgrading from 1.1, your settings can stay as they are.

Release/Win32 compilation and offline selector, budget, caster, allocation-pass and hook-order checks passed. Internal build: 22. Full matching source is attached. By OnyxOak, based on FusionFix by ThirteenAG and contributors, with coding assistance from OpenAI Codex.
