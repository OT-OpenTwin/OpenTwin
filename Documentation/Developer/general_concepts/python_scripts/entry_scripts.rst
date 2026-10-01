.. _target Entry scripts:

Entry Scripts
=============

Every batch file of the build system runs one Python script from ``Scripts/Python``. These are the entry scripts.
Each one checks its arguments, sets up the environment and then calls the ``ot_dev`` package next to it,
which does the actual work.

The lists below show every entry script, which batch file starts it and what it does.
``[...]`` marks optional arguments. Without them, ``BOTH`` and ``REBUILD`` are used.


Projects
--------

These are started by the four :ref:`project batches<target Project batches>` in every project folder.
``<PROJECT>`` is the :ref:`project key<target Project keys>`.

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - Script
     - What it does
   * - ``build.py <PROJECT> [DEBUG|RELEASE|BOTH] [BUILD|REBUILD]``
     - Configures and builds the project with CMake. Started by ``build.bat``.
   * - ``test.py <PROJECT> [DEBUG|RELEASE|BOTH]``
     - Runs the tests of the project. Started by ``test.bat``.
   * - ``clean.py <PROJECT>``
     - Deletes the build output of the project. Started by ``clean.bat``.
   * - ``edit.py <PROJECT|FOLDER> [EDITOR]``
     - Opens the project in an editor. Instead of a key it also takes a full folder path. Started by ``edit.bat``.


All projects
------------

Started from ``Scripts/BuildAndTest``. The logs and the build summary go to that folder.

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - Script
     - What it does
   * - ``build_all.py [DEBUG|RELEASE|BOTH] [BUILD|REBUILD]``
     - Builds every project in ``BUILD_ORDER`` (see :ref:`BuildOrder<target BuildOrder>`)
       and writes ``buildLog_Summary.txt``. Creates the local certificates first if they are missing.
       ``BuildAll.bat`` starts it with ``BOTH BUILD``, ``RebuildAll.bat`` passes its own arguments on.
       ``--doc-only`` builds only the documentation.
   * - ``test_all.py [DEBUG|RELEASE|BOTH]``
     - Runs the tests of every project that has a ``tests/CMakeLists.txt``. Started by ``TestAll.bat``.
   * - ``clean_all.py``
     - Cleans every project. Started by ``CleanAll.bat``.
   * - ``build_framework.py [DEBUG|RELEASE|BOTH] [BUILD|REBUILD]``
     - Builds only the Rust framework in ``Framework/OpenTwin``. No batch file, run it directly when needed.
   * - ``build_admin_panel.py [DEBUG|RELEASE|BOTH] [BUILD|REBUILD]``
     - Builds only the admin panel in ``Tools/AdminPanel``. No batch file, run it directly when needed.


Deployment and installers
-------------------------

The deployment scripts run the plans in :ref:`DeploymentManifest<target DeploymentManifest>`.

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - Script
     - What it does
   * - ``update_libraries.py``
     - Replaces the OpenTwin binaries in ``Deployment``. Started by ``UpdateDeploymentLibrariesOnly.bat``.
   * - ``create_deployment.py``
     - Builds ``Deployment`` from scratch. Started by ``CreateDeployment.bat``.
   * - ``create_frontend_installer.py``
     - Collects the files of the frontend installer. Started by ``CreateFrontendInstaller.bat``.
   * - ``create_debug_files.py``
     - Builds everything, then collects the debug files. Started by ``CreateDebugFiles.bat``.
   * - ``create_build_information.py``
     - Writes the build date and the git revisions to ``Deployment/BuildInfo.txt``.
       Started by ``Scripts/BuildAndTest/CreateBuildInformation.bat``.
   * - ``installers.py <build|build-information|upgrader>``
     - Builds the NSIS installers (``Scripts/Installer/Build_Installers_noInput.bat``),
       writes their build information (``Scripts/Installer/CreateBuildInformation.bat``)
       or builds the MongoDB upgrader (``Tools/MongoDBUpgrader/Build_Upgrader_Standalone.bat``).


Documentation and build server
------------------------------

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - Script
     - What it does
   * - ``build_documentation.py [BOTH|SPHINX|DOXYGEN|HTML]``
     - Builds this documentation (``SPHINX``), the code documentation (``DOXYGEN``) or both.
       ``HTML`` is the quick local build of this documentation only.
       Started by ``BuildDocumentation.bat``, ``Documentation/Developer/MakeHtml.bat`` and ``Documentation/Doxygen/Generate.bat``.
   * - ``build_server.py <deploy|deploy-task|continuous|continuous-task>``
     - The jobs of the build server. ``deploy`` is the nightly: update, rebuild, documentation, deployment,
       installers and upload. ``continuous`` updates and builds only when something changed.
       The ``-task`` variants run the same job into a log file and can send a notification at the end. Started by ``BatchBuildAndDeploy*.bat`` and ``ContinuousBuild*.bat``.


Other
-----

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - Script
     - What it does
   * - ``certificates.py``
     - Creates the local certificates in ``Certificates/Generated`` if they are missing.
       Started by ``Certificates/CreateLocalCertificates/CreateLocalCertificates.bat``.
   * - ``shutdown_all.py``
     - Stops all running OpenTwin processes. Started by ``Scripts/Launcher/ShutdownAll_py.bat``.
   * - ``helpers.py <command> [ARGS...]``
     - Small tools, one command each, see below. Mostly started by the batch files in ``Scripts/Other``.

The ``helpers.py`` commands:

.. code-block:: text

   compress-file <file> <archive>     pack a file with 7-Zip                  CompressFile.bat
   decompress-file <archive> <folder> unpack a 7-Zip archive                  DecompressFile.bat
   compress-all                       pack the large ThirdParty files
   decompress-all                     unpack the large ThirdParty files
   update-file-headers [ARGS...]      run the FileHeaderUpdater               UpdateFileHeaders.bat
   run-qtcreator                      Qt Creator with the environment set
   cmake-gui-qt / cmake-gui-qt6       cmake-gui with the Qt folders set
   run-cmake [ARGS...]                cmake-gui with the environment set      RunCMakeWithEnvSet.bat
   run-devenv [ARGS...]               Visual Studio with the environment set  RunDevEnv2022WithEnvSet.bat
   build-solution <sln> [ARGS...]     build a Visual Studio solution
   run-python [ARGS...]               the ThirdParty Python                   RunPython.bat
   check-failed-builds                report failed builds of BuildAll        CheckForFailedBuilds.bat, BuildAll.bat
   register-scheme                    register the opentwin:// URL protocol   RegisterOpenTwinScheme_py.bat

The start scripts in ``Scripts/Launcher`` do not use these entry scripts. They run the ``ot_launcher`` package
with the Python of the Deployment, because they also have to work on machines without the repository.


Writing a new script
--------------------

Before writing a new entry script, check if the job fits as a new ``helpers.py`` command.
Each command is one line in the ``COMMANDS`` table at the top of ``helpers.py``, which is less work for small tools.

A new entry script needs two files: the Python script in ``Scripts/Python`` and a batch file that starts it.
The batch file always looks the same. Keep the ``OPENTWIN_DEV_ROOT`` check and the ``set_python.bat`` call:

.. code-block:: bat

   @ECHO OFF

   IF "%OPENTWIN_DEV_ROOT%" == "" (
   	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
   	PAUSE
   	EXIT /B 1
   )

   CALL "%OPENTWIN_DEV_ROOT%\Scripts\Python\set_python.bat"

   "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\my_script.py" %1 %2
   IF ERRORLEVEL 1 PAUSE

For the Python side, copy an existing entry script of the same kind and change it. ``build.py`` is a good start:

.. code-block:: python

   import sys
   from pathlib import Path
   from typing import Sequence

   # Needed: the ThirdParty Python does not add this folder by itself
   sys.path.insert(0, str(Path(__file__).resolve().parent))

   from ot_dev import build_project, cli


   def main(argv: Sequence[str]) -> int:
       # Check the arguments, SystemExit prints the text and ends with exit code 1
       if not 1 <= len(argv) <= 3:
           raise SystemExit("usage: build.py <PROJECT> [DEBUG|RELEASE|BOTH] [BUILD|REBUILD]")

       # The environment of SetupEnvironment.py and the folder of the project key
       env, target = cli.prepare(argv[0])
       configurations = cli.configurations(cli.argument(argv, 1))
       rebuild = cli.build_type(cli.argument(argv, 2))
       return build_project(env, target, configurations, rebuild)


   if __name__ == "__main__":
       # Ctrl+C ends the script cleanly instead of with a traceback
       sys.exit(cli.run(main, sys.argv[1:]))
