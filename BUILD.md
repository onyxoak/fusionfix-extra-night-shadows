# Build 19 source and compilation

This repository contains the custom modified-source files and offline tests. The complete corresponding build 19 source tree, populated dependencies, build notes and compiler log are attached to the build19-preview GitHub release as the source ZIP. Use that archive to compile; this small repository is not a standalone upstream checkout.

Read BUILD19-NOTES.txt and BUILD.md in the extracted source archive. The baseline and dependency revisions are recorded in BASELINE.txt and SUBMODULES.txt. Build Release/Win32 with MSVC, disabling post-build deployment. The published binary is the tested artifact; a byte-identical rebuild is not claimed.

Build 19 adds CloseHeadlightRelevance.hpp and changes nightshadows.ixx, ShadowAllocationRuntime.inl and ShadowCasterRuntime.inl. The runtime download is manual installation only; obsolete installer scripts have been removed from this branch. README.md contains installation and rollback instructions.

The candidate19 geometry test in shadow-test/candidate19 uses headers from modified-source and can be compiled as a C++17 console program. 96 new and 39 existing geometry checks passed during development. Geometry checks do not establish rendered correctness.
