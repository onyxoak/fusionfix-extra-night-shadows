# Building Liberty's Shadows

Use a Visual Studio x86 Native Tools command prompt with MSVC C++23 support,
the Windows SDK, CMake, Ninja and Python 3 available.

Run Build.cmd from the extracted source package. The ASI is written to
`build/OnyxOak.ExtraNightShadows.asi`. No game installation is required to
compile the ASI.

To configure manually:

```bat
cmake -S source -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --target OnyxOak.ExtraNightShadows --parallel
```

To build tests, build all targets with `cmake --build build --parallel`.
Run `python RunTests.py --build build` from the extracted package root. The
runner gives every check a fresh working folder so test settings cannot collide.
Binary-contract tests require your own GTA IV, FusionFix and Volumetric Lights
fixtures. Supply their paths through the COMPAT CMake cache variables, including
COMPAT199_GAME, COMPAT199_VOLUME, COMPAT199_FUSION, COMPAT218_FUSION,
COMPAT213_FUSION510 and COMPAT213_FUSION501. Third-party/game binaries are not
bundled. Python can be selected with `-DPython3_EXECUTABLE=...`.

Install the compiled ASI with the matching shaders and INI after closing GTA IV.
