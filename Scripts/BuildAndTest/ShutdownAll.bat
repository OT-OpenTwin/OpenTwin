@ECHO OFF

REM The shutdown lives in Scripts\Launcher, this file only forwards to it.
"%~dp0..\Launcher\ShutdownAll_py.bat" %*
