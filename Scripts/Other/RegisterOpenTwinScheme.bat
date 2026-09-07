@echo off
setlocal

echo Registering OpenTwin URL protocol...

REM Check environment variable
if "%OPENTWIN_DEV_ROOT%"=="" (
    echo ERROR: OPENTWIN_DEV_ROOT is not set.
    exit /b 1
)

REM Define executable path
set "OPENTWIN_EXE=%OPENTWIN_DEV_ROOT%\Deployment\uiFrontend.exe"

REM Check executable
if not exist "%OPENTWIN_EXE%" (
    echo ERROR: OpenTwin executable not found:
    echo        "%OPENTWIN_EXE%"
    exit /b 1
)

echo Executable:
echo   "%OPENTWIN_EXE%"
echo.

REM Register opentwin:// protocol
reg add "HKCU\Software\Classes\OpenTwin" ^
    /ve /t REG_SZ /d "URL:OpenTwin Protocol" /f

reg add "HKCU\Software\Classes\OpenTwin" ^
    /v "URL Protocol" /t REG_SZ /d "" /f

REM Register application icon
reg add "HKCU\Software\Classes\OpenTwin\DefaultIcon" ^
    /ve /t REG_SZ /d "\"%OPENTWIN_EXE%\",0" /f

REM Register command executed for opentwin:// URLs
reg add "HKCU\Software\Classes\OpenTwin\shell\open\command" ^
    /ve /t REG_SZ /d "\"%OPENTWIN_EXE%\" \"%%1\"" /f

if errorlevel 1 (
    echo.
    echo ERROR: Failed to register OpenTwin URL protocol.
    exit /b 1
)

echo.
echo Successfully registered:
echo   opentwin://
echo.
echo Command:
echo   "%OPENTWIN_EXE%" "%%1"
echo.

REM Test registration
echo You can test it with:
echo   start "" "opentwin://open?project=test"

endlocal
exit /b 0