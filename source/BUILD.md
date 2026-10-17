# Building Liberty's Shadows

Use a Visual Studio x86 Native Tools command prompt with MSVC C++23 support,
the Windows SDK, CMake and Ninja available.

Run Build.cmd from the extracted source package. The ASI is written to
`build/OnyxOak.ExtraNightShadows.asi`.

To configure manually:

```bat
cmake -S source -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --target OnyxOak.ExtraNightShadows --parallel
```

The test targets include binary-contract tests that require local GTA IV and
FusionFix fixtures. Supply the corresponding COMPAT CMake cache paths when
running those tests. Game and third-party mod binaries are not part of this
source package.

Install the ASI with the matching package shaders and INI after closing GTA IV.
