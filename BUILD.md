# Build 18 source snapshot

The `fusionfix-source` folder contains the modified upstream tree and populated submodules. Git metadata and generated build/output folders are omitted. Existing compiled shader resources and required dependency libraries are retained alongside their available source. The source snapshot includes the custom headers and `.inl` files, which a tracked-only Git patch would omit.

Baseline: `619f52d87681ae7dd6f35795208c3b3759f9632c`. Dependency commits are recorded in `SUBMODULES.txt`. `tracked-changes.patch` covers tracked modifications; untracked additions are already present in the full tree. `SOURCE-SHA256SUMS.txt` records every other file in this source archive.

## Windows build

The tested compiler was MSVC 14.51.36231 (Visual Studio 2026 C++ tools), targeting Win32, with the Windows SDK. From a matching Developer Command Prompt:

```bat
cd fusionfix-source
premake5.exe vs2026 --with-version=5.0.1.1
msbuild build\GTAIV.EFLC.FusionFix.vcxproj /p:Configuration=Release /p:Platform=Win32 /p:PostBuildEventUseInBuild=false /m
```

The explicit `PostBuildEventUseInBuild=false` prevents the upstream project's machine-specific deployment command from copying into a game directory. The archive retains the original build files; inspect deployment paths before enabling any post-build event. Output is under `fusionfix-source/bin`. Do not run the release or signing scripts to compile just this ASI.

The original source tree's dependency libraries in `external/modupdater/dist` are included. To rebuild them, use the dependency's own `premake5.bat` and `builddist.bat` in `external/modupdater`, with MSBuild on PATH. The upstream workflow at `.github/workflows/msvc_x86.yml` records its complete packaging pipeline. Other upstream asset build scripts are retained for modifying the corresponding assets.

This archive has no Git checkout metadata, so generated version metadata may omit the short commit ID. Compiler/SDK differences, timestamps and version metadata can change the binary hash. The included install ASI is the exact tested build; a byte-identical rebuild from the extracted archive has not been demonstrated.

## Offline tests

The `shadow-test/offline` directory keeps the original relative layout with `fusionfix-source`. Run its `Run-*.ps1` scripts with `-VisualStudioRoot` set to your Visual Studio installation and, where accepted, `-GameExe` pointing to your own CE executable. The defaults are development-machine paths and should be overridden. No GTA executable is distributed here. The hook-order fixture needs the compiled production objects, so build the ASI first. Offline checks do not establish rendered appearance.
