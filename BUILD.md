# Building the matching release

This repository exposes the released C/C++ source, menu XMLs and focused tests for review. It is not a standalone build checkout: bundled SDK binaries, resource images, generated build files and external dependencies remain in the matching full-source release ZIP.

Download the explicitly named full source ZIP from the corresponding GitHub release. Open `fusionfix-source/build/GTAIV.EFLC.FusionFix.vcxproj` and build Release / Win32 with MSVC and the Windows SDK. Preserve the included dependency and license files. Some archived test scripts contain the original developer's absolute paths; adjust those to your checkout before running them. The tests are code checks, not a substitute for in-game validation.

`SOURCE-MANIFEST.json` records the full archive SHA-256 and exact archive path/hash for each imported source/test/menu file. Source files are copied byte-for-byte, including original line endings. This does not claim a fresh reproducible binary build was performed during the source import.

Use `source-ce-1.5` or `source-ce-1.6` to inspect the corresponding imported snapshot. The older `v1.5` and `v1.6` release tags remain untouched; their attached full-source ZIPs are the packaging authority. No diagnostic-only hang45 changes are included.
