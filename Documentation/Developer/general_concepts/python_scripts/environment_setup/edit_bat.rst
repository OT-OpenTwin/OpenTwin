edit.bat
========

.. code-block:: text

   edit.bat [VS|CODE|NVIM]

.. code-block:: bat

   @ECHO OFF

   IF "%OPENTWIN_DEV_ROOT%" == "" (
       ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
       PAUSE
       EXIT /B 1
   )

   CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"

   "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\edit.py" MODEL_SERVICE %1
   IF ERRORLEVEL 1 PAUSE

The one argument is the editor: ``edit.bat CODE`` runs ``edit.py MODEL_SERVICE CODE``, see :ref:`Editors<target Editors>` below.

Opens the project folder in an editor, with the full development environment set.
Visual Studio opens the folder as a CMake project, so builds and debugging from inside Visual Studio
use the same presets as ``build.bat``.

.. _target Editors:

Editors
-------

``edit.bat`` can start one of these editors:

- VS (Visual Studio)
- CODE (Visual Studio Code)
- NVIM (Neovim)

Without any setting, VS is used. Visual Studio is started from its install folder,
``code`` and ``nvim`` have to be found on ``PATH``.

To use a different editor once, give it as an argument, from a terminal in the project folder:

.. code-block:: powershell

   .\edit.bat CODE

To always start a different editor, set the ``OT_DEFAULT_EDITOR`` environment variable to VS, CODE or NVIM.
An argument given to the edit.bat still wins over the variable.

The editor can also be fixed in the batch file itself, by writing it after the project key instead of ``%1``:

.. code-block:: bat

   "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\edit.py" MODEL_SERVICE CODE

The batch files are shared through git, so this changes the editor for everyone.
For your own preference, use ``OT_DEFAULT_EDITOR`` instead.
