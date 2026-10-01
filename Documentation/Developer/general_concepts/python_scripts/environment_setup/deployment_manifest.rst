.. _target DeploymentManifest:

DeploymentManifest
==================

``Scripts/DeploymentManifest.py``: the central DeploymentManifest to plan and set various deployment files.

This module contains the ``UpdateDeployment`` setup, the ``CreateDeployment`` setup, the ``CreateFrontendInstaller`` and the ``CreateDebugFiles`` setup.
Each one is a function, and each function is run by one batch file in ``Scripts/BuildAndTest``:

.. list-table::
   :header-rows: 1
   :widths: 35 35 30

   * - Batch file
     - Function
     - What it does
   * - ``UpdateDeploymentLibrariesOnly.bat``
     - ``update_libraries(plan)``
     - Replaces the OpenTwin binaries in ``Deployment``.
   * - ``CreateDeployment.bat``
     - ``create_deployment(plan)``
     - Deletes ``Deployment`` and builds it from scratch. Runs ``update_libraries`` as part of it.
   * - ``CreateFrontendInstaller.bat``
     - ``create_frontend_installer(plan)``
     - Collects the files for the frontend installer.
   * - ``CreateDebugFiles.bat``
     - ``create_debug_files(plan)``
     - Collects the debug symbols.

Please check to write your changes to the correct functions accordingly.
A new library or service usually only needs lines in ``update_libraries``,
because ``create_deployment`` runs it as well.

To add content to the deployment manifest edit the file with these new functions:

.. code-block:: text

   plan.copy(source, target)          COPY, wildcards allowed
   plan.tree(source, target)          XCOPY /S, add empty=True for /E
   plan.mkdir(target)                 MKDIR
   plan.rmtree(target)                RMDIR /S /Q
   plan.remove(target)                DEL, wildcards allowed
   plan.remove_glob(folder, pattern)  recursive DEL
   plan.write(target, lines)          ECHO > file
   plan.rename(target, name)          REN
   plan.move(source, target)          MOVE
   plan.run(label, command, cwd)      runs a program (e.g. makensis)
   plan.action(name)                  a step ot_dev runs: "shutdown", "build information" or "build all"

This module resembles the batch functions of the old batch system.

The steps run from top to bottom. ``update_libraries`` first removes the files of the previous build,
then copies the new ones. A library therefore appears twice:

.. code-block:: python

   def update_libraries(plan) -> None:
       # ...
       # Previous build: delete the old file from the Deployment
       plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTCore.dll")
       # ...
       # Copy the Release build of OTCore into the Deployment:
       # %OT_CORE_ROOT%  the project folder (from SetupEnvironment.py)
       # %OT_CDLLR%      its Release output folder
       plan.copy(r"%OT_CORE_ROOT%\%OT_CDLLR%\OTCore.dll", "%OPENTWIN_DEPLOYMENT_DIR%")

The :ref:`raw string rule<target SetupEnvironment>` from ``SetupEnvironment.py`` applies here as well.
A source that does not exist does not stop the run. It is listed under "Missing sources" at the end,
and the run finishes with ``FINISHED WITH PROBLEMS`` instead of ``SUCCESS``.
