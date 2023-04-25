@ECHO OFF

REM This batch script will setup the default environment variables for the given service
REM
REM Arguments
REM    1. Name of the Service in all capital letters and an underscore as a delimiter [required]
REM    2. Name of the Service in the same way the library/exe will be named           [required]
REM    3. Path to root folder (without the backslash suffix)                          [required]

IF "%1" == "" (
    GOTO INVALID_ARGS
)
IF "%2" == "" (
    GOTO INVALID_ARGS
)
IF "%3" == "" (
    GOTO INVALID_ARGS
)

REM The folder structure should be as following:
REM    root                 (Project root folder)             (NAME_ROOT)
REM     |
REM     +--> \include       (Include files root)              (NAME_INCD; NAME_INCR)
REM     |
REM     +--> \src           (Source files root)               (NAME_SRCD; NAME_SRCR)
REM     |
REM     +--> \test          (Test project root)               (NAME_TESTD; NAME_TESTR)
REM     |
REM     +--> \x64
REM           |
REM           +--> \Debug   (contains debug: lib and dll)     (NAME_LIBPATHD; NAME_DLLD)
REM           |
REM           +--> \Release (contains release: lib and dll)   (NAME_LIBPATHR; NAME_DLLR)
REM
REM Additionally the following variables will be set:
REM 
REM    LibraryNamed.lib  -> NAME_LIBD
REM    LibraryName.lib   -> NAME_LIBR

REM Root
SET %1_ROOT=%3

REM Include
SET %1_INCD=%3\include
SET %1_INCR=%3\include

REM Source
SET %1_SRCD=%3\src
SET %1_SRCR=%3\src

REM Test
SET %1_TESTD=%3\test
SET %1_TESTR=%3\test

REM Library path
SET %1_LIBPATHD=%3\x64\Debug
SET %1_LIBPATHR=%3\x64\Release

REM Library dependency / name
REM SET %1_LIBD=%2d.lib
SET %1_LIBD=%2.lib

SET %1_LIBR=%2.lib

REM Binary path
SET %1_DLLD=%3\x64\Debug
SET %1_DLLR=%3\x64\Release

GOTO END

:INVALID_ARGS
ECHO ERROR While running (name = "SetLibraryEnv.bat"; arg1 = "%1"; arg2 = "%2"; arg3 = "%3") script. Invalid arguments:
ECHO ..... Please provide the following arguments:
ECHO ..... ..... 1. Name of the Service in all capital letters and an underscore as a delimiter [required]
ECHO ..... ..... 2. Name of the Service in the same way the library/exe will be named           [required]
ECHO ..... ..... 3. Path to root folder (without the backslash suffix)                          [required]
GOTO PAUSE_END

:PAUSE_END
PAUSE
GOTO END

:END
