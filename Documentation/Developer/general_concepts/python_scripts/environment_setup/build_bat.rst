build.bat
=========

.. code-block:: text

   build.bat [DEBUG|RELEASE|BOTH] [BUILD|REBUILD] [WSL]

.. code-block:: bat

   @ECHO OFF

   IF "%OPENTWIN_DEV_ROOT%" == "" (
       ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
       PAUSE
       EXIT /B 1
   )

   CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"

   "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\build.py" MODEL_SERVICE %1 %2 %3
   IF ERRORLEVEL 1 PAUSE

All arguments are passed on: ``build.bat RELEASE BUILD`` runs ``build.py MODEL_SERVICE RELEASE BUILD``.

Configures and builds the project with CMake. Without arguments it builds Debug and Release as a full rebuild.
``BUILD`` skips the clean step and only builds what changed, which is a lot faster during development.

The CMake output goes to ``buildLog_Debug.txt`` and ``buildLog_Release.txt``, the console only shows the result.
The logs are written to the folder the batch file was started from, which is the project folder on a double-click.
New output is added at the end, so the latest build is at the bottom. When a build fails, look into these files first.

Building in WSL
---------------

With ``WSL`` as the last argument, the project is built for Linux instead of Windows, in the Windows Subsystem for Linux.
Without it, ``build.bat`` builds for Windows as always.

.. code-block:: powershell

   .\build.bat WSL                 # Debug and Release, full rebuild, in WSL
   .\build.bat DEBUG WSL           # Debug, full rebuild, in WSL
   .\build.bat DEBUG BUILD WSL     # Debug, without cleaning first, in WSL

``WSL`` is always the last word, the arguments before it mean the same as without it.

The batch starts the same ``build.py`` inside WSL. There it runs as on a Linux machine: it reads ``SetupEnvironment.py``
on the Linux side and builds with the Linux presets. The console shows which distro is used (``Building in WSL: <distro>``).

- The output goes to ``build/linux-debug`` and ``build/linux-release`` in the project folder, next to the Windows builds.
- The logs go to ``buildLog_Debug.txt`` and ``buildLog_Release.txt`` as above.

The build runs in the default WSL distro. Set it up once with:

.. code-block:: powershell

   wsl --install FedoraLinux-44
   wsl --set-default FedoraLinux-44

Inside the distro, the compilers and tools have to be installed (``gcc-c++``, ``cmake``, ``ninja-build``).

On a Linux machine, ``WSL`` is not needed: ``build.py`` builds for Linux there anyway.
