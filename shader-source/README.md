# Shader sources

world-local contains the corresponding unchanged material-shadow shader sources,
assembly, upstream sources, licenses and rebuilding tools. scenery-fade contains
the scenery transition patch and portable rebuilding recipe. vehicle-fallback
contains the matching deferred-lighting rebuild for 2.0.5.

On Windows with Python3 and d3dcompiler_47.dll, run:
python shader-source/vehicle-fallback/BuildShader.py

Output: shader-source/vehicle-fallback/output/deferred_lighting.fxc.
Special thanks to MrBulkinAleksey; see source/vendor/MrBulkinAleksey.
