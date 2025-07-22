@ECHO OFF
SETLOCAL EnableDelayedExpansion

REM Set PATH="C:\OpenTwin\Deployment\Python";%PATH%
Set base=%CD%
Set PYTHONPATH="%base%\Python;%base%\Python\DLLs;%base%\Python\Lib;%base%\Python\Lib\site-packages"
set targetPath=""

IF "%OT_PYTHON_SITE_PACKAGE_PATH%" == "" (
	echo ATTENTION. The OT_PYTHON_SITE_PACKAGE_PATH environment variable is not defined. All Python modules are stored in the OpenTwin deployment folder which will be overwritten by a new installation/update
	set targetPath=!base!\Python\Lib\site-packages
) else (
	Set PYTHONPATH="!PYTHONPATH!;!OT_PYTHON_SITE_PACKAGE_PATH!"
	set targetPath=!OT_PYTHON_SITE_PACKAGE_PATH!
	echo All modules are stored at the subpackage path that is defined by the OT_PYTHON_SITE_PACKAGE_PATH environment variable: !OT_PYTHON_SITE_PACKAGE_PATH!
)


if "%~1"=="" (
   echo package name is missing.
   exit /b 1
)

set "packageName=%~1"

Start .\Python.exe -i -m pip install --target "%targetPath%" %packageName%
pause