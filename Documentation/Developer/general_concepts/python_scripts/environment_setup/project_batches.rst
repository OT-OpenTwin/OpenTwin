.. _target Project batches:

Project batches
===============

We use the build/clean/test/edit.bat batch files that wrap our python system entry points of the same name: build/clean/test/edit.py.
Every project folder has all four. They are identical in every project except for the :ref:`project key<target Project keys>`.

The batch files can be started by double-click or from a terminal. Arguments only work from a terminal.

.. toctree::
   :maxdepth: 2

   build_bat
   test_bat
   edit_bat
   clean_bat

The parts of a batch
--------------------

All four batches are built the same way. This is ``build.bat`` of ``Services/Model``, with the four parts numbered.
The ``REM`` lines are only added here for explanation:

.. code-block:: bat

   @ECHO OFF

   REM [1] Check the development environment
   IF "%OPENTWIN_DEV_ROOT%" == "" (
       ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
       PAUSE
       EXIT /B 1
   )

   REM [2] Set OT_PYTHON
   CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"

   REM [3] Start the Python script with the project key and the arguments
   "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\build.py" MODEL_SERVICE %1 %2

   REM [4] Keep the window open if something failed
   IF ERRORLEVEL 1 PAUSE

Parts 1, 2 and 4 are mandatory and the same in every batch. Copy them as they are.

**[1] Check the development environment.**
Every path in the batch starts with ``%OPENTWIN_DEV_ROOT%``. Without the variable, the batch would look for
``\Scripts\Python\...`` and fail with a "cannot find the path" error that does not say what is wrong.
The check stops with a clear message instead. ``PAUSE`` keeps the window open after a double-click,
so the message can be read, and ``EXIT /B 1`` ends the batch with an error code.

**[2] Set OT_PYTHON.**
``set_python.bat`` sets ``OT_PYTHON`` to the Python that comes with ThirdParty. The scripts are written for this Python,
and every developer gets the same one, no matter which Python is installed on the machine, or none at all.
It also switches off the Python cache files, which would otherwise end up in the ThirdParty Python folders and get deployed.

The ``CALL`` is needed. Without it, the batch jumps into ``set_python.bat`` and never comes back, so the Python line never runs.

.. warning::
   To correctly use the python environment system, ALWAYS(!) keep the set_python.bat call, before using the OT_PYTHON variable.

**[3] Start the Python script.**
The only line that differs: between the four batches the script name, between projects the project key.
``%1`` and ``%2`` are the first and second argument given to the batch file, they are passed on unchanged.
The quotes keep paths with spaces working.

**[4] Keep the window open if something failed.**
On a double-click, the window closes as soon as the batch ends. If the script failed, ``ERRORLEVEL`` is 1 or higher
and ``PAUSE`` keeps the window open, so the error can be read. A successful run closes the window.

``@ECHO OFF`` is not needed for the batch to work. It hides the commands themselves, so only the output is shown.
