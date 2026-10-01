clean.bat
=========

.. code-block:: text

   clean.bat

.. code-block:: bat

   @ECHO OFF

   IF "%OPENTWIN_DEV_ROOT%" == "" (
       ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
       PAUSE
       EXIT /B 1
   )

   CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"

   "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\clean.py" MODEL_SERVICE
   IF ERRORLEVEL 1 PAUSE

No arguments, only the project key.

Deletes the build output of the project: the folders ``.vs``, ``build``, ``x64``, ``packages`` and ``test``.
If Visual Studio has the project open, some files are locked and the folder is reported as ``(locked)``.
Close Visual Studio and run it again.
