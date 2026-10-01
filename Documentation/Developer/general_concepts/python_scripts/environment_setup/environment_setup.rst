.. _target Environment Setup Guide:

Environment Setup Guide
=======================

The scripts read their configuration from three files in ``Scripts/``:

- ``SetupEnvironment.py``: the environment variables, above all where each project lives.
- ``BuildOrder.py``: which projects ``BuildAll`` and ``RebuildAll`` build, and in which order.
- ``DeploymentManifest.py``: which files the deployment scripts copy.

All three are plain Python files. You only edit the lists and dictionaries in them,
the scripts in ``Scripts/Python`` read them on every run. An editor that is already open keeps the old
environment, so start it again with ``edit.bat`` after changing ``SetupEnvironment.py``.

The :ref:`Project Setup Guide<target Project Setup Guide>` shows which of these files a new project needs, in which order.

.. toctree::
   :maxdepth: 2

   setup_environment
   build_order
   project_batches
   deployment_manifest
