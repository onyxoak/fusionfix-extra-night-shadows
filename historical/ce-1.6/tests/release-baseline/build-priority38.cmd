@echo off
call "D:\Microsoft\VC\Auxiliary\Build\vcvars32.bat" >nul
set "TASK_SRC=D:\GTA-IV-Collection-Lab\view-priority-candidate26\fusionfix-source"
set "TASK_OUT=D:\GTA-IV-Collection-Lab\coherent-priority-candidate38"
cl /nologo /std:c++20 /EHsc outputs\priority-tests38.cpp /Fo:"%TASK_OUT%\priority.obj" /Fe:"%TASK_OUT%\priority.exe"
if not "%errorlevel%"=="0" exit /b 1
"%TASK_OUT%\priority.exe"
if not "%errorlevel%"=="0" exit /b 1
cl /nologo /std:c++20 /EHsc outputs\receivers-tests38.cpp /Fo:"%TASK_OUT%\receivers.obj" /Fe:"%TASK_OUT%\receivers.exe"
if not "%errorlevel%"=="0" exit /b 1
"%TASK_OUT%\receivers.exe"
if not "%errorlevel%"=="0" exit /b 1
cl /nologo /std:c++20 /EHsc outputs\native-lamps-tests35.cpp /Fo:"%TASK_OUT%\lamps.obj" /Fe:"%TASK_OUT%\lamps.exe"
if not "%errorlevel%"=="0" exit /b 1
"%TASK_OUT%\lamps.exe"
if not "%errorlevel%"=="0" exit /b 1
call "D:\GTA-IV-Collection-Lab\view-priority-candidate26\test.cmd" > "%TASK_OUT%\regression-tests.log" 2>&1
if not "%errorlevel%"=="0" exit /b 1
"D:\Microsoft\MSBuild\Current\Bin\MSBuild.exe" "%TASK_SRC%\build\GTAIV.EFLC.FusionFix.vcxproj" /p:Configuration=Release /p:Platform=Win32 /m /v:minimal > "%TASK_OUT%\ce-build.log" 2>&1
if not "%errorlevel%"=="0" exit /b 1
exit /b 0
