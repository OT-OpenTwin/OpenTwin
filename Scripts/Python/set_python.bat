@ECHO OFF
SET "OT_PYTHON=%OPENTWIN_THIRDPARTY_ROOT%\Python\Python3_11_9\Interpreter\Release\python.exe"

IF NOT EXIST "%OT_PYTHON%" (
	ECHO Python interpreter not found: %OT_PYTHON%
	ECHO Please check the environment variable OPENTWIN_THIRDPARTY_ROOT
)
