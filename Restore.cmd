@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Install-ShadowFix.ps1" -Restore
pause
