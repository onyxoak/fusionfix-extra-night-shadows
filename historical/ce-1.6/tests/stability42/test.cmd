@echo off
call "D:\Microsoft\VC\Auxiliary\Build\vcvars32.bat" >nul
cl /nologo /std:c++20 /EHsc /I"D:\GTA-IV-Collection-Lab\view-priority-candidate26\fusionfix-source\source" outputs\stability42\continuity_tests.cpp /Fo:outputs\stability42\continuity.obj /Fe:outputs\stability42\continuity.exe
if errorlevel 1 exit /b 1
outputs\stability42\continuity.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++20 /EHsc /I"D:\GTA-IV-Collection-Lab\view-priority-candidate26\fusionfix-source\source" outputs\stability42\guard_tests.cpp /Fo:outputs\stability42\guard.obj /Fe:outputs\stability42\guard.exe
if errorlevel 1 exit /b 1
outputs\stability42\guard.exe
exit /b %errorlevel%
