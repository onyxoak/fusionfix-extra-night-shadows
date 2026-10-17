@echo off
where cl >nul 2>&1
if errorlevel 1 exit /b 1
where cmake >nul 2>&1
if errorlevel 1 exit /b 1
cmake -S "%~dp0source" -B "%~dp0build" -G Ninja -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b 1
cmake --build "%~dp0build" --target OnyxOak.ExtraNightShadows --parallel
exit /b %errorlevel%
