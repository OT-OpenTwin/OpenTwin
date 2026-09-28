@ECHO OFF
SET "OT_PYTHON=%OPENTWIN_THIRDPARTY_ROOT%\Python\Python3_11_9\Interpreter\Release\python.exe"
REM No bytecode caches: they would land in the ThirdParty Python folders and get deployed
SET "PYTHONDONTWRITEBYTECODE=1"

IF NOT EXIST "%OT_PYTHON%" (
	ECHO Python interpreter not found: %OT_PYTHON%
	ECHO Please check the environment variable OPENTWIN_THIRDPARTY_ROOT
)
