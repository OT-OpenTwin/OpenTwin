@ECHO OFF

REM Deployment: python.exe and the ot_launcher package are next to this file.
IF EXIST "%~dp0python.exe" IF EXIST "%~dp0ot_launcher\__main__.py" (
	"%~dp0python.exe" -B -m ot_launcher shutdown
	EXIT /B
)

REM Repository: Scripts\Launcher, the Python scripts are in Scripts\Python.
IF EXIST "%~dp0..\Python\shutdown_all.py" (
	CALL "%~dp0..\Python\set_python.bat"
) ELSE (
	ECHO Neither a Deployment nor the OpenTwin repository: "%~dp0"
	PAUSE
	EXIT /B 1
)

"%OT_PYTHON%" "%~dp0..\Python\shutdown_all.py"
IF ERRORLEVEL 1 PAUSE
