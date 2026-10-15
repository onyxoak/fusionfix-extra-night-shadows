@echo off
call "D:\Microsoft\VC\Auxiliary\Build\vcvars32.bat" >nul
cl /nologo /std:c++20 /EHsc /I"D:\GTA-IV-Collection-Lab\view-priority-candidate26\fusionfix-source\source" outputs\stability40\stability_tests.cpp /Fo:outputs\stability40\stability.obj /Fe:outputs\stability40\stability.exe
if errorlevel 1 exit /b 1
outputs\stability40\stability.exe
