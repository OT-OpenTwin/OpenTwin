.. _target Project Setup Guide:

Project Setup Guide
===================

This page walks through adding a new library, service or tool to OpenTwin, from the empty folder
to the first successful build. Work through the steps in order, each one builds on the one before.

The example is a new library ``OTFoo`` in ``Libraries/OTFoo``. For a service or a tool the steps are the same,
only the folder and a few names differ. Those differences are mentioned in each step.


Before you start
----------------

The development environment has to be set up as described in :doc:`setup_development_environment`.
The batch files of the project need ``OPENTWIN_DEV_ROOT`` and ``OPENTWIN_THIRDPARTY_ROOT``.

Decide on the names first, because they appear in several files:

.. list-table::
   :header-rows: 1
   :widths: 30 25 45

   * - Name
     - Example
     - Used in
   * - Folder
     - ``OTFoo``
     - ``Libraries/``, ``Services/`` or ``Tools/``
   * - Variable name
     - ``OT_FOO_ROOT``
     - ``SetupEnvironment.py``, ``DeploymentManifest.py``, CMake
   * - Project key
     - ``FOO``
     - ``BuildOrder.py``, the project batches
   * - CMake target
     - ``OTFoo``
     - ``CMakeLists.txt``, and the name of the built DLL

How the project key is made from the variable name: :ref:`The project key<target Project keys>`.

For a new service, ``Templates/ServiceTemplate`` already contains all files of the steps below.
Copy it into ``Services/``, rename it and follow its ``README.md``. Then check every step here.


1. Create the project folder
----------------------------

.. code-block:: text

   Libraries/OTFoo/
     include/            header files, other projects include from here
     src/                source files, every .cpp and .c in here is built
     tests/              optional, see step 7
     CMakeLists.txt      step 5
     CMakePresets.json   step 6
     build.bat  clean.bat  test.bat  edit.bat   step 4

Source files are not listed anywhere. CMake picks up every ``.cpp`` and ``.c`` below ``src/``
and every ``.h`` and ``.hpp`` below ``include/``. A new file is found on the next build.


2. Register the project in SetupEnvironment.py
----------------------------------------------

Add one line to ``Scripts/SetupEnvironment.py``, in the section that matches the parent folder:

.. code-block:: python

   LIBRARIES = {
       # ...
       # OT_FOO_ROOT will hold the full path of Libraries/OTFoo
       "OT_FOO_ROOT": "OTFoo",
   }

A service goes into ``SERVICES``, a tool into ``TOOLS``. The value is only the folder name,
the scripts add the parent folder themselves. More about the key and the value: :ref:`SetupEnvironment<target SetupEnvironment>`.

From now on the scripts know the project as ``FOO``.


3. Add the project to BuildOrder.py
-----------------------------------

This step is optional. Without it, the project builds with its own ``build.bat``, but ``BuildAll``,
``RebuildAll`` and the nightly build skip it.

Add the project key to ``BUILD_ORDER`` in ``Scripts/BuildOrder.py``. It has to come after every project it links against:

.. code-block:: python

   BUILD_ORDER: list[str] = [
       "SYSTEM",
       "CORE",
       # ...
       "FOO",      # after SYSTEM and CORE, because OTFoo links them
   ]

More in :ref:`BuildOrder<target BuildOrder>`.


4. Add the project batches
--------------------------

Copy ``build.bat``, ``clean.bat``, ``test.bat`` and ``edit.bat`` from a sibling project, for example ``Libraries/OTCore``.
In each file, only the project key in the line that starts the Python script changes. In ``Libraries/OTCore`` it is ``CORE``:

.. code-block:: bat

   REM build.bat
   "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\build.py" FOO %1 %2

   REM clean.bat
   "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\clean.py" FOO

   REM test.bat
   "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\test.py" FOO %1

   REM edit.bat
   "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\edit.py" FOO %1

Leave the rest of the files as they are. What each batch does: :ref:`Project batches<target Project batches>`.


5. Write the CMakeLists.txt
---------------------------

The ``CMakeLists.txt`` describes what the project is and what it links against.
The build system in ``Scripts/CMake`` does the rest. For ``OTFoo``:

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.20)
   project(OTFoo LANGUAGES CXX)

   # The shared build system, OT_CMAKE_DIR comes from SetupEnvironment.py
   include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

   # OTFoo is a library (DLL)
   ot_initialize_lib(OTFoo OT_FOO_ROOT_PATH)

   # What OTFoo links against, as dependency tokens
   ot_add_dependency(OTFoo
       OTSystem
       OTCore
   )

   ot_finalize_lib(OTFoo)

   # Builds the tests/ folder if there is one (step 7)
   ot_add_test(OTFoo)

``OT_FOO_ROOT_PATH`` is the project folder, taken from ``OT_FOO_ROOT`` of step 2.
If CMake stops with "root path var 'OT_FOO_ROOT_PATH' is not set", step 2 is missing or the name differs.

A service is a library as well and uses the same calls. An executable uses ``ot_initialize_bin`` and ``ot_finalize_bin``.
How each kind looks, and what the dependency tokens are:
:ref:`Writing a CMakeLists.txt<target Writing a CMakeLists>` and :ref:`Dependency Tokens<target CMake Dependency Tokens>`.

Other projects can link ``OTFoo`` with the token ``OTFoo``. The token is found through ``OT_FOO_ROOT``,
so step 2 is all that is needed for that.


6. Add the CMakePresets.json
----------------------------

Copy the ``CMakePresets.json`` from a sibling project, it is the same file in every project:

.. code-block:: json

   {
     "version": 7,
     "include": [ "$penv{OT_CMAKE_DIR}/OTPresets.json" ]
   }

It only includes the shared presets from ``Scripts/CMake/OTPresets.json``, so it needs no changes.
The presets decide the compiler, the build folder and the Debug and Release settings.
``build.bat`` and Visual Studio both use them, without this file the project cannot be configured.
What the presets contain: :ref:`Configurations and presets<target CMake Presets>`.


7. Add tests
------------

This step is optional. Create a ``tests/`` folder with a ``CMakeLists.txt`` and the test sources in ``tests/src/``:

.. code-block:: cmake

   # Libraries/OTFoo/tests/CMakeLists.txt
   cmake_minimum_required(VERSION 3.20)
   project(OTFoo_tests LANGUAGES CXX)

   include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

   # The test executable, and the target it tests
   ot_initialize_test(OTFoo_tests OTFoo)

``test.bat`` runs them, and ``TestAll`` picks the project up by itself as soon as ``tests/CMakeLists.txt`` exists.
More in :ref:`Writing a CMakeLists.txt<target Writing a CMakeLists>`, section "Unit tests".


8. Add the project to the deployment
------------------------------------

This step is only needed if the project ships with OpenTwin. Add its DLL to ``update_libraries``
in ``Scripts/DeploymentManifest.py``, once to remove the old file and once to copy the new one:

.. code-block:: python

   def update_libraries(plan) -> None:
       # ...
       # Previous build
       plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTFoo.dll")
       # ...
       plan.copy(r"%OT_FOO_ROOT%\%OT_CDLLR%\OTFoo.dll", "%OPENTWIN_DEPLOYMENT_DIR%")

Put the lines next to the ones of similar projects. ``CreateDeployment`` runs ``update_libraries`` as well,
so nothing else is needed. More in :ref:`DeploymentManifest<target DeploymentManifest>`.


9. Build and check
------------------

Run ``build.bat`` in the project folder with a double-click. At the end the console shows the result.

- **Success:** the DLL is in ``build/windows-debug/Debug`` and ``build/windows-release/Release``.
- **Failed:** open ``buildlog_Debug.txt`` or ``buildlog_Release.txt`` in the project folder.
  The first error from the top is usually the real one.

Then check the rest:

- ``edit.bat`` opens the project in Visual Studio.
- ``test.bat`` runs the tests, if the project has any.
- ``RebuildAll`` builds the project at its place in ``BUILD_ORDER``, if you added it in step 3.

When everything works, commit the project folder and the changed files in ``Scripts/``.
Leave out ``build/``, ``.vs/`` and the ``buildlog_*.txt`` and ``testlog_*.txt`` files.
