.. _target SetupEnvironment:

SetupEnvironment
================

``Scripts/SetupEnvironment.py``: this single script module is the central environment setter.
In there you can set and change or delete various development related environment variables.
Each development 'Section' is divided by a **python dictionary**.

The sections you will touch most are ``SERVICES``, ``LIBRARIES`` and ``TOOLS``. Each entry there
registers one project: it tells the scripts that the project exists and where its folder is.
The other sections (``RELATIVE``, ``DEV_PATHS``, ``CERTIFICATES``, ``COMPOSITES``, ``PATH_PREPEND``)
hold shared paths and search lists. They rarely need changes.

.. code-block:: python

   SERVICES = {
       # variable name            folder inside Services/
       "OT_MODEL_SERVICE_ROOT": "Model",
   }

   LIBRARIES = {
       # variable name   folder inside Libraries/
       "OT_GUI_ROOT": "OTGui",
   }

   TOOLS = {
       # variable name        folder inside Tools/
       "OT_OTOOLKIT_ROOT": "OToolkit",
   }

Each entry has two parts: the variable name on the left and the folder on the right.

The variable name
-----------------

The left side is the name of the environment variable the scripts create for the project.
For a project it is always ``OT_<NAME>_ROOT``, for example ``OT_GUI_ROOT``.
It has to be unique across all three sections.

``<NAME>`` does not have to match the folder name: ``OT_MODEL_SERVICE_ROOT`` points to the folder ``Model``.
Choose a name that says what the project is, in capital letters with ``_`` between the words.
The scripts make the :ref:`project key<target Project keys>` from it.

The folder
----------

In ``SERVICES``, ``LIBRARIES`` and ``TOOLS`` the right side is only the folder name.
The scripts add the rest of the path themselves: ``"OT_GUI_ROOT": "OTGui"`` gives the variable ``OT_GUI_ROOT``
the value ``<repository>\Libraries\OTGui``. Which section the entry is in decides the parent folder.

In the other sections the right side is a full path or a path below the repository.
Paths can contain other variables, see the note below.

.. important::
   For full paths with reference to other set variables, please use python raw strings (``r"..."``).
   To reference a different variable: ``%VARIABLE%``

   .. code-block:: python

      r"%OPENTWIN_DEV_ROOT%\Deployment",

   Without the ``r``, Python reads ``\n``, ``\t``, ``\b`` and others as special characters,
   so a folder like ``\bin`` or ``\tests`` silently breaks the path.

.. _target Project keys:

The project key
---------------

To name a project, the scripts do not use the variable name but a shorter name: the project key.
It is made from the variable name by removing ``OT_`` at the front and ``_ROOT`` at the end:

.. code-block:: text

   OT_MODEL_SERVICE_ROOT       the variable name from SetupEnvironment.py
   OT_ MODEL_SERVICE _ROOT     remove OT_ and _ROOT
       MODEL_SERVICE           the project key

The key is not written down anywhere else to register it. It exists as soon as the entry is in ``SetupEnvironment.py``.

The project batches pass the key to the Python scripts. This is the line in ``Services/Model/build.bat``:

.. code-block:: bat

   "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\build.py" MODEL_SERVICE %1 %2

``build.py`` looks up ``MODEL_SERVICE``, finds the variable ``OT_MODEL_SERVICE_ROOT`` and builds the folder it points to.

Which of the two names to use:

- **The variable**, when a **path** is needed: ``%OT_MODEL_SERVICE_ROOT%`` in ``DeploymentManifest.py``,
  ``OT_MODEL_SERVICE_ROOT_PATH`` in CMake.
- **The project key**, when you **name a project** for a script: in the project batches,
  in ``BUILD_ORDER`` and in ``BUILD_OVERRIDES``.

A few real examples:

.. list-table::
   :header-rows: 1
   :widths: 35 25 40

   * - Variable name
     - Project key
     - Folder
   * - ``OT_CORE_ROOT``
     - ``CORE``
     - ``Libraries/OTCore``
   * - ``OT_MODEL_SERVICE_ROOT``
     - ``MODEL_SERVICE``
     - ``Services/Model``
   * - ``OT_OTOOLKIT_ROOT``
     - ``OTOOLKIT``
     - ``Tools/OToolkit``

On the command line the key is not case sensitive. In the files, write it in capital letters like the existing ones.
An unknown key stops the script with a list of all known keys, so a typo shows up right away.
