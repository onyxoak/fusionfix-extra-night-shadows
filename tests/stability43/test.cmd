@echo off
call "D:\Microsoft\VC\Auxiliary\Build\vcvars32.bat" >nul
cl /nologo /std:c++20 /EHsc /I"D:\GTA-IV-Collection-Lab\view-priority-candidate26\fusionfix-source\source" outputs\stability43\visibility_tests.cpp /Fo:outputs\stability43\visibility.obj /Fe:outputs\stability43\visibility.exe
if not "%errorlevel%"=="0" exit /b %errorlevel%
outputs\stability43\visibility.exe
if not "%errorlevel%"=="0" exit /b %errorlevel%
cl /nologo /std:c++20 /EHsc /I"D:\GTA-IV-Collection-Lab\view-priority-candidate26\fusionfix-source\source" outputs\stability43\guard_tests.cpp /Fo:outputs\stability43\guard.obj /Fe:outputs\stability43\guard.exe
if not "%errorlevel%"=="0" exit /b %errorlevel%
outputs\stability43\guard.exe
exit /b %errorlevel%

