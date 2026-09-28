@echo off
call "D:\Microsoft\VC\Auxiliary\Build\vcvars32.bat" >nul
set "TASK_SRC=D:\GTA-IV-Collection-Lab\view-priority-candidate26\fusionfix-source"
set "TASK_OUT=D:\GTA-IV-Collection-Lab\stability-candidate43"
cl /nologo /std:c++20 /EHsc /I"D:\GTA-IV-Collection-Lab\view-priority-candidate26\fusionfix-source\source" outputs\stability41\continuity_tests.cpp /Fo:"%TASK_OUT%\continuity.obj" /Fe:"%TASK_OUT%\continuity.exe"
if errorlevel 1 exit /b 1
"%TASK_OUT%\continuity.exe"
if errorlevel 1 exit /b 1
cl /nologo /std:c++20 /EHsc /I"D:\GTA-IV-Collection-Lab\view-priority-candidate26" outputs\stability40\view-tests.cpp /Fo:"%TASK_OUT%\view.obj" /Fe:"%TASK_OUT%\view.exe"
if errorlevel 1 exit /b 1
"%TASK_OUT%\view.exe"
if errorlevel 1 exit /b 1
for %%T in (priority-tests38 receivers-tests38 native-lamps-tests35) do (
 cl /nologo /std:c++20 /EHsc outputs\%%T.cpp /Fo:"%TASK_OUT%\%%T.obj" /Fe:"%TASK_OUT%\%%T.exe"
 if errorlevel 1 exit /b 1
 "%TASK_OUT%\%%T.exe"
 if errorlevel 1 exit /b 1
)
for %%T in (budget_tests allocation_pass_tests selector_tests) do (
 cl /nologo /std:c++20 /EHsc "D:\GTA-IV-Collection-Lab\view-priority-candidate26\shadow-test\offline\%%T.cpp" /Fo:"%TASK_OUT%\%%T.obj" /Fe:"%TASK_OUT%\%%T.exe"
 if errorlevel 1 exit /b 1
 "%TASK_OUT%\%%T.exe"
 if errorlevel 1 exit /b 1
)
"D:\Microsoft\MSBuild\Current\Bin\MSBuild.exe" "%TASK_SRC%\build\GTAIV.EFLC.FusionFix.vcxproj" /p:Configuration=Release /p:Platform=Win32 /m /v:minimal > "%TASK_OUT%\ce-build.log" 2>&1
exit /b %errorlevel%



