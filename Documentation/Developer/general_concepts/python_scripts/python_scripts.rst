.. _target Build and Deployment Scripts:

Build and Deployment Scripts
============================

Everything around building, testing and deploying OpenTwin runs through the scripts in ``Scripts/``.
The batch files you start (``build.bat``, ``RebuildAll.bat``, ``CreateDeployment.bat``, ...) are small wrappers.
The actual work is done by Python scripts in ``Scripts/Python``, and what they do is configured in three files:
``SetupEnvironment.py``, ``BuildOrder.py`` and ``DeploymentManifest.py``.

These pages explain how the scripts are organized and how to change their configuration.
If you want to add a new library, service or tool, follow the
:ref:`Project Setup Guide<target Project Setup Guide>` in Get Started, it links back to the details here.

.. toctree::
   :maxdepth: 3

   directory_guide
   environment_setup/environment_setup
   entry_scripts
