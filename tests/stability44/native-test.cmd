@echo off
call "D:\Microsoft\VC\Auxiliary\Build\vcvars32.bat" >nul
cl /nologo /std:c++20 /EHsc /I"D:\GTA-IV-Collection-Lab\view-priority-candidate26\fusionfix-source\source" outputs\stability44\native_cache_tests.cpp /Fo:outputs\stability44\native-cache.obj /Fe:outputs\stability44\native-cache.exe
if not "%errorlevel%"=="0" exit /b %errorlevel%
outputs\stability44\native-cache.exe
exit /b %errorlevel%
