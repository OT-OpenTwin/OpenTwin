@ECHO OFF
CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"
"%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\edit.py" CIRCUIT_EXECUTION %1
IF ERRORLEVEL 1 pause
