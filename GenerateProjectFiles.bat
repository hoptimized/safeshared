@echo off

set "_OLD_CD=%CD%"
mkdir "%~dp0\build"
cd /d "%~dp0\build"

call cmake ..

cd /d "%_OLD_CD%"
pause
