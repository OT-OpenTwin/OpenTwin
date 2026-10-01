build.bat
=========

.. code-block:: text

   build.bat [DEBUG|RELEASE|BOTH] [BUILD|REBUILD]

.. code-block:: bat

   @ECHO OFF

   IF "%OPENTWIN_DEV_ROOT%" == "" (
       ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
       PAUSE
       EXIT /B 1
   )

   CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"

   "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\build.py" MODEL_SERVICE %1 %2
   IF ERRORLEVEL 1 PAUSE

Both arguments are passed on: ``build.bat RELEASE BUILD`` runs ``build.py MODEL_SERVICE RELEASE BUILD``.

Configures and builds the project with CMake. Without arguments it builds Debug and Release as a full rebuild.
``BUILD`` skips the clean step and only builds what changed, which is a lot faster during development.

The CMake output goes to ``buildlog_Debug.txt`` and ``buildlog_Release.txt``, the console only shows the result.
The logs are written to the folder the batch file was started from, which is the project folder on a double-click.
New output is added at the end, so the latest build is at the bottom. When a build fails, look into these files first.
