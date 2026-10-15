# Build the current ASI

Use Windows with Visual Studio2022 C++ build tools, the Windows SDK, CMake3.24+ and Ninja. Select an **x86/Win32** developer environment. The exact released source, including SafetyHook/Zydis, is under source/.

```text
cmake -S source -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --target OnyxOak.ExtraNightShadows --parallel 8
```

The ASI is build/OnyxOak.ExtraNightShadows.asi. Compilation alone does not validate a live game installation. Test executables additionally need the official CE/FusionFix/VolumetricLights fixtures referenced by CMake. Supply the COMPAT199_GAME, COMPAT199_FUSION and COMPAT199_VOLUME cache variables for your own legitimate files. Some historical test commands retain original D-drive paths, including older FusionFix fixtures; adapt those fixture paths to your environment before running the full suite. No copyrighted game executable or companion binary is distributed in this repository.

Shader builds are separate. Use the matching full-source release ZIP for the complete shader source and original recipes. The current paired scenery-fade assembly/recipe is also readable under shaders/. All original compiler commands, source manifests and validation receipts are retained in the full-source attachment. This source import does not claim a new reproducible runtime build or new gameplay validation.
