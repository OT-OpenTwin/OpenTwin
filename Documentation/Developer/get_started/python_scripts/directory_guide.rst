.. _target Scripts Directory guide:

Directory guide
===============

.. code-block:: text

   Scripts/
     SetupEnvironment.py     all development environment variables
     BuildOrder.py           build order of BuildAll/RebuildAll
     DeploymentManifest.py   what the deployments copy
     Python/                 the python system: entry scripts (*.py) and the ot_dev package
       set_python.bat        sets OT_PYTHON, call it before any entry script
     BuildAndTest/           RebuildAll, TestAll, CleanAll, deployments, build server jobs, build logs
     CMake/                  shared CMake modules and presets (OT_CMAKE_DIR)
     Installer/              NSIS installers
     Launcher/               start scripts that ship with the deployment
     Other/                  helpers: compress/decompress, file headers, URL scheme, cmake-gui/VS with env

Every project folder has its own build.bat, clean.bat, test.bat and edit.bat (see :ref:`Project batches<target Project batches>`).
