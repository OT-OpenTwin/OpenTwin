.. _target Python API:

Python API
==========

The entry scripts in ``Scripts/Python`` are small. The work is done by the ``ot_dev`` package next to them.
Everything on this page can be imported with ``from ot_dev import <name>``, except the ``cli`` helpers
which are used as ``cli.<name>``.

Arguments named ``env`` are an environment dictionary, usually the one returned by :py:func:`ot_dev.build_env`.
Arguments named ``configs`` are a list like ``["debug", "release"]``, use :py:func:`ot_dev.cli.configurations` to get it from a command line argument.
Functions that return an ``int`` return 0 on success, like a batch file exit code.


Writing a new wrapper
---------------------

A wrapper is a batch file that calls an entry script with the ThirdParty Python.
Keep the ``OPENTWIN_DEV_ROOT`` check and the ``set_python.bat`` call:

.. code-block:: bat

   @ECHO OFF

   IF "%OPENTWIN_DEV_ROOT%" == "" (
   	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
   	PAUSE
   	EXIT /B 1
   )

   CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"

   "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\build.py" CORE %1 %2
   IF ERRORLEVEL 1 PAUSE

An entry script checks its arguments, builds the environment and calls into ``ot_dev``.
This is ``build.py``:

.. code-block:: python

   import sys
   from pathlib import Path
   from typing import Sequence

   sys.path.insert(0, str(Path(__file__).resolve().parent))

   from ot_dev import build_project, cli


   def main(argv: Sequence[str]) -> int:
       if not 1 <= len(argv) <= 3:
           raise SystemExit("usage: build.py <PROJECT> [DEBUG|RELEASE|BOTH] [BUILD|REBUILD]")

       env, target = cli.prepare(argv[0])
       configurations = cli.configurations(cli.argument(argv, 1))
       rebuild = cli.build_type(cli.argument(argv, 2))
       return build_project(env, target, configurations, rebuild)


   if __name__ == "__main__":
       sys.exit(cli.run(main, sys.argv[1:]))

The ``sys.path.insert`` line is needed: the ThirdParty Python ignores ``PYTHONPATH`` and does not add the script folder by itself.
``raise SystemExit("...")`` prints the text and exits with 1.


Adding a command instead of a script
------------------------------------

For small tools a new entry script is often not needed. ``helpers.py``, ``installers.py`` and ``build_server.py``
each have a ``COMMANDS`` table that maps the first argument to a function. Add your function there
and it can be called as ``helpers.py <your-command> [ARGS...]``:

.. code-block:: python

   COMMANDS: dict[str, Callable[[Mapping[str, str], Sequence[str]], int]] = {
       "compress-file": lambda env, a: compress_file(env, _argument(a, 0), _argument(a, 1)),
       ...
       "register-scheme": lambda env, a: register_scheme(env),
   }

Each function gets the environment and the remaining arguments. In ``helpers.py`` the environment is
:py:func:`ot_dev.build_env`, except for the commands listed in ``PLAIN``, which get the caller's environment as it is.

Launcher commands work the same way: ``COMMANDS`` in ``Scripts/Launcher/ot_launcher/cli.py``,
called by a batch file with ``python -B -m ot_launcher <command>``. Keep in mind that ``ot_launcher`` ships with the
Deployment and can only use the standard library, not ``ot_dev``.

To add a deployment step, see the ``plan.*`` functions in
:ref:`DeploymentManifest.py<target Environment Setup Guide>`.


Where things are set
--------------------

Most settings live in the three files in ``Scripts`` (see :ref:`Environment Setup Guide<target Environment Setup Guide>`).
Inside ``ot_dev`` there are a few more places worth knowing:

``ot_dev/core/platform.py``
   Everything that differs between Windows and Linux.

   - ``EDITORS``: the editors ``edit.bat`` can start. Each entry is ``NAME: (variable, executable)``.
     With a variable, the executable is taken from the folder in that variable (``VS`` uses ``DEVENV_ROOT_2022``).
     With ``None``, it is looked up on ``PATH`` (``CODE``, ``NVIM``). To add an editor, add a line here.
   - ``DEFAULT_EDITOR``: used when neither an argument nor ``OT_DEFAULT_EDITOR`` is given.
   - ``REQUIRED``: variables that must be set on this platform, on top of ``REQUIRED`` in ``SetupEnvironment.py``.
   - ``cmake_executable`` / ``ctest_executable``: on Windows, CMake and CTest always come from Visual Studio 2022,
     not from ``PATH``.
   - ``ENV_VARS``: set for every build, ``VSLANG=1033`` keeps the compiler output in English.

``ot_dev/core/paths.py``
   Folders and log file names used in more than one place, e.g. ``BUILD_AND_TEST`` (where the logs go),
   ``DEPLOYMENT``, ``build_log(config)`` and ``test_log(config)``. Use these instead of writing the paths again.

``ot_dev/core/config.py``
   Loads ``SetupEnvironment.py``, ``DeploymentManifest.py`` and ``BuildOrder.py`` as ``definitions``, ``manifest``
   and ``order``. Read the settings through these, e.g. ``order.BUILD_ORDER``.


Optional variables
------------------

This does not need to be set:

.. code-block:: text

   OT_DEFAULT_EDITOR                    editor for edit.bat: VS, CODE or NVIM (default VS)


Command line helpers
--------------------

.. py:currentmodule:: ot_dev.cli

.. py:function:: environment() -> dict

   Same as :py:func:`ot_dev.build_env`.

.. py:function:: prepare(project) -> tuple

   Returns ``(env, root)``: the environment and the folder of the project key, e.g. ``prepare("CORE")``.

.. py:function:: configurations(value) -> list

   ``DEBUG`` → ``["debug"]``, ``RELEASE`` → ``["release"]``, ``BOTH`` → both. ``None`` means ``BOTH``.
   Not case sensitive, anything else exits with the known values.

.. py:function:: build_type(value) -> bool

   ``BUILD`` → ``False``, ``REBUILD`` → ``True``. ``None`` means ``REBUILD``.

.. py:function:: argument(argv, index)

   ``argv[index]``, or ``None`` when there are fewer arguments.

.. py:function:: run(entry, argv) -> int

   Calls ``entry(argv)`` and returns its result. Ctrl+C prints ``Aborted.`` and returns 130.


Environment and projects
------------------------

.. py:currentmodule:: ot_dev

.. py:function:: build_env(base=None) -> dict

   Returns a copy of the current environment with everything the scripts need: the ThirdParty variables,
   the project roots and paths from ``SetupEnvironment.py``, certificates, service arguments and ``PATH``.
   The real environment of the process is not changed.
   ``base`` replaces ``OPENTWIN_DEV_ROOT`` as the repository folder.

.. py:function:: check_required() -> None

   Exits with a message when ``OPENTWIN_DEV_ROOT``, ``OPENTWIN_THIRDPARTY_ROOT`` or ``DEVENV_ROOT_2022`` is not set.
   :py:func:`build_env` already does this.

.. py:function:: apply_toolchain(env) -> dict

   Runs ``vcvars64.bat`` of Visual Studio 2022 and adds its variables (``INCLUDE``, ``LIB``, ``PATH``, ...) to ``env``.
   Changes ``env`` and returns it. Does nothing when ``OT_TOOLCHAIN_READY`` is already set.
   :py:func:`build_project` and :py:func:`build_all` call it themselves.

.. py:function:: project_roots() -> dict

   All project keys from ``SetupEnvironment.py`` and their variable, e.g. ``{"CORE": "OT_CORE_ROOT", ...}``.

.. py:function:: resolve_root(env, key) -> str

   The folder of a project key, e.g. ``resolve_root(env, "CORE")``. Not case sensitive.
   An unknown key exits with the list of known keys.


Single projects
---------------

.. py:function:: build_project(env, target, configs, rebuild, logs=None) -> int

   Builds the CMake project in the folder ``target`` with its presets, for each configuration:
   ``cmake --preset windows-<config>``, then ``cmake --build --preset build-windows-<config>``.
   With ``rebuild`` the build target ``clean`` runs first.
   Adds ``--parallel`` when ``OPENTWIN_DEV_PARALLEL_BUILDS`` is set.
   The output is appended to ``buildlog_Debug.txt`` / ``buildlog_Release.txt`` in ``logs`` (default: the current folder).

.. py:function:: test_project(env, target, configs, logs=None) -> int

   Runs ``ctest`` on ``build/windows-<config>/tests`` of the project.
   ``PATH`` gets the ThirdParty DLL folders and the Deployment folder, so the tests find their DLLs.
   The output is appended to ``testlog_Debug.txt`` / ``testlog_Release.txt`` in ``logs`` (default: the current folder).
   A project that was not built is reported as ``SKIPPED`` and returns 0.

.. py:function:: clean_project(target) -> int

   Deletes the folders ``.vs``, ``build``, ``x64``, ``packages`` and ``test`` in ``target``.
   Returns 1 when one of them could not be deleted (e.g. open in Visual Studio).


All projects
------------

.. py:function:: build_all(env, order, overrides, special, configs, rebuild, logs, summary) -> int

   Builds the project keys in ``order`` one after another with :py:func:`build_project`.
   ``overrides`` maps a key to its own ``(configs, rebuild)``, like ``BUILD_OVERRIDES`` in ``BuildOrder.py``.
   ``special`` maps a key to ``(name, step)`` for builds that are not CMake, ``step`` is called as ``step(env, configs, rebuild, logs)``.
   Deletes the old build logs in ``logs`` first and writes the file ``summary`` there at the end.
   Ctrl+C asks whether to stop the run. Returns 1 when any step failed.
   ``RebuildAll`` uses it through ``ot_dev.builds.rebuild.rebuild_all(env, configs, rebuild)``, which reads ``BuildOrder.py``.

.. py:function:: test_all(env, projects, configs, logs) -> int

   Runs :py:func:`test_project` for each project key. Deletes the old test logs in ``logs`` first.

.. py:function:: clean_all(env, projects) -> int

   Runs :py:func:`clean_project` for each project key.

.. py:function:: testable_projects(env) -> list

   The project keys that have a ``tests/CMakeLists.txt``.

.. py:function:: build_framework(env, configs, rebuild, logs) -> int

   Builds ``Framework/OpenTwin`` with cargo.

.. py:function:: build_admin_panel(env, configs, rebuild, logs) -> int

   Runs ``yarn install`` and ``yarn build`` in ``Tools/AdminPanel``. ``configs`` and ``rebuild`` are not used,
   they are there so it fits as a ``special`` step of :py:func:`build_all`.


Deployment
----------

These run the plans of ``DeploymentManifest.py``. Without ``env`` they build it themselves.
They print the missing sources and failed files at the end and return 1 if there were any.

.. py:function:: update_libraries(env=None) -> int

.. py:function:: create_deployment(env=None) -> int

.. py:function:: create_frontend_installer(env=None) -> int

.. py:function:: create_debug_files(env=None) -> int

.. py:function:: create_build_information(target=None) -> int

   Writes the build date and the git revisions of OpenTwin and ThirdParty to ``target``
   (default: ``Deployment/BuildInfo.txt``).


Documentation
-------------

.. py:function:: build_documentation(env=None, mode="BOTH") -> int

   ``BOTH``, ``SPHINX`` or ``DOXYGEN`` build the documentation, the logs go to ``Scripts/BuildAndTest``.
   ``HTML`` only runs ``sphinx-build -M html``, like ``make html``.
   ``sphinx-build`` is taken from ``SPHINXBUILD`` or ``PATH``.


Tools
-----

.. py:function:: launch_editor(env, target, editor=None) -> int

   Opens ``target`` (a folder or file) in ``VS``, ``CODE`` or ``NVIM``.
   Without ``editor``, ``OT_DEFAULT_EDITOR`` is used, and without that ``VS``.
   Visual Studio is started from ``DEVENV_ROOT_2022`` and the call returns at once. ``code`` and ``nvim`` are taken from ``PATH``.

.. py:function:: run_program(env, command, toolchain=False, detach=False) -> int

   Runs ``command`` (a list) and returns its exit code.
   Leading ``NAME=VALUE`` entries are set as variables first, ``%VAR%`` anywhere in the command is expanded.
   ``toolchain=True`` runs :py:func:`apply_toolchain` first. ``detach=True`` starts it and returns 0 at once.

   .. code-block:: python

      run_program(env, [str(seven_zip), "x", f"-o{folder}", archive])

.. py:function:: shutdown_all() -> int

   Stops ``open_twin.exe``, ``PythonExecution.exe``, ``uiFrontend.exe`` and ``httpd.exe`` and waits until ``httpd.exe`` is gone.
   Windows only. It comes from ``ot_launcher``, the launcher package in ``Scripts/Launcher``.


Entry scripts
-------------

The existing entry scripts and their arguments. ``[...]`` is optional, without it ``BOTH`` and ``REBUILD`` are used.

.. code-block:: text

   build.py <PROJECT> [DEBUG|RELEASE|BOTH] [BUILD|REBUILD]
   test.py <PROJECT> [DEBUG|RELEASE|BOTH]
   clean.py <PROJECT>
   edit.py <PROJECT|FOLDER> [EDITOR]

   build_all.py [DEBUG|RELEASE|BOTH] [BUILD|REBUILD] | --doc-only
   test_all.py [DEBUG|RELEASE|BOTH]
   clean_all.py
   build_framework.py [DEBUG|RELEASE|BOTH] [BUILD|REBUILD]
   build_admin_panel.py [DEBUG|RELEASE|BOTH] [BUILD|REBUILD]
   build_documentation.py [BOTH|SPHINX|DOXYGEN|HTML]
   build_server.py <deploy|deploy-task|continuous|continuous-task>

   update_libraries.py
   create_deployment.py
   create_frontend_installer.py
   create_debug_files.py
   create_build_information.py
   installers.py <build|build-information|upgrader>
   certificates.py
   shutdown_all.py

   helpers.py <compress-file|decompress-file|compress-all|decompress-all|update-file-headers|
               run-qtcreator|cmake-gui-qt|cmake-gui-qt6|run-cmake|run-devenv|build-solution|
               run-python|check-failed-builds|register-scheme> [ARGS...]

The launcher scripts in ``Scripts/Launcher`` use ``ot_launcher`` instead, with the ``python.exe`` of the Deployment:

.. code-block:: text

   python -B -m ot_launcher <local|local-ui|logger|session|all-services|admin|toolkit|server-certificate|shutdown>
   python -B -m ot_launcher setup <service-args|certificates|lds> [dev]

``ot_launcher`` only uses the Python standard library and never imports ``ot_dev``, because it ships with the Deployment.
