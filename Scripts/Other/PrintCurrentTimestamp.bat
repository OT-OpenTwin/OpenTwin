@echo off
setlocal enabledelayedexpansion

REM Get current date and time
for /f "delims=" %%a in ('powershell -NoProfile -Command "(Get-Date).ToString(\"yyyyMMddHHmmss\")"') do set datetime=%%a

SET OT_TIMESTAMP=!datetime:~0,4!-!datetime:~4,2!-!datetime:~6,2! !datetime:~8,2!:!datetime:~10,2!:!datetime:~12,2!

REM Remove quotes from parameters
set OT_LCL_Prefix=%~1
set OT_LCL_Suffix=%~2

REM Print the formatted string
echo !OT_LCL_Prefix!%OT_TIMESTAMP%!OT_LCL_Suffix!