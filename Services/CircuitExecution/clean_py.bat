@ECHO OFF
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"
"%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\clean.py" CIRCUIT_EXECUTION
IF ERRORLEVEL 1 pause
