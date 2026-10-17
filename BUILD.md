# Build Liberty's Shadows 2.0.5

Use Windows, Visual Studio C++ build tools with C++23 support, the Windows SDK, CMake 3.24+ and Ninja. Open an **x86 Native Tools** developer command prompt at the repository root, then run:

```bat
Build.cmd
```

Or configure manually:

```bat
cmake -S source -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --target OnyxOak.ExtraNightShadows --parallel
```

The plugin is `build/OnyxOak.ExtraNightShadows.asi`. All ASI build dependencies are included. The release archive and this checkout contain the same ASI and shader source bytes; no local development directory is required to build the ASI.

For Fix Only, install the built ASI as `plugins/OnyxOak.ExtraNightShadows.asi` and copy `config/Fix-Only-No-Enhancements.ini` as `plugins/OnyxOak.ExtraNightShadows.ini`. No shader replacement is required. For Full, use the matching full INI and player-package shaders, following INSTALL.txt.

For the vehicle-fallback shader, use Python 3 on Windows with d3dcompiler_47.dll:

```bat
python shader-source/vehicle-fallback/BuildShader.py
```

See `shader-source/README.md` for the other shader inputs and preserved rebuilding recipes. These shader changes are for the full edition.

Tests include binary-contract fixtures requiring legitimate local GTA IV, FusionFix and Volumetric Lights binaries. Set COMPAT199_GAME, COMPAT199_FUSION, COMPAT199_VOLUME and any other fixture cache paths referenced by the selected tests before running them. Some inherited fixture defaults and historical shader recipes retain original local paths; adjust those inputs to your environment. No copyrighted game or companion binaries are included. Compilation and offline checks do not prove gameplay correctness or universal compatibility.
