@echo off
call "D:\Microsoft\VC\Auxiliary\Build\vcvars32.bat" >nul
cl /nologo /std:c++20 /EHsc /I"D:\GTA-IV-Collection-Lab\view-priority-candidate26\fusionfix-source\source" outputs\stability44\cache_tests.cpp /Fo:outputs\stability44\cache.obj /Fe:outputs\stability44\cache.exe
if not "%errorlevel%"=="0" exit /b %errorlevel%
outputs\stability44\cache.exe
if not "%errorlevel%"=="0" exit /b %errorlevel%
cl /nologo /std:c++20 /EHsc /I"D:\GTA-IV-Collection-Lab\view-priority-candidate26\fusionfix-source\source" outputs\stability44\guard_tests.cpp /Fo:outputs\stability44\guard.obj /Fe:outputs\stability44\guard.exe
if not "%errorlevel%"=="0" exit /b %errorlevel%
outputs\stability44\guard.exe
exit /b %errorlevel%
