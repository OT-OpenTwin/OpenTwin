test.bat
========

.. code-block:: text

   test.bat [DEBUG|RELEASE|BOTH]

.. code-block:: bat

   @ECHO OFF

   IF "%OPENTWIN_DEV_ROOT%" == "" (
       ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
       PAUSE
       EXIT /B 1
   )

   CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"

   "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\test.py" MODEL_SERVICE %1
   IF ERRORLEVEL 1 PAUSE

Only one argument is passed on, the configuration: ``test.bat DEBUG`` runs ``test.py MODEL_SERVICE DEBUG``.

Runs the tests of the project, the ones defined in its ``tests/`` folder. Build the project first:
a configuration that was not built is skipped, and if none was built the result is ``SKIPPED``.
The output goes to ``testlog_Debug.txt`` and ``testlog_Release.txt``, in the same folder as the build logs.
