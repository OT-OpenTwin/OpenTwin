.. _target Project Setup Guide:

Project Setup Guide
===================

To set up a new project with the new environment system, follow these steps.

1. **SetupEnvironment.py**

   Add the project to SERVICES, LIBRARIES or TOOLS, depending on its folder.

   .. code-block:: text

      E.g. Libraries/OTFoo  ->  LIBRARIES = { ..., "OT_FOO_ROOT": "OTFoo" }

2. **BuildOrder.py** (optional)

   Add the key to BUILD_ORDER to build it with BuildAll/RebuildAll.
   Put it after the projects it depends on.

   .. code-block:: text

      E.g. OT_FOO_ROOT  ->  "FOO"

3. **Project batches**

   Copy build.bat, clean.bat, test.bat and edit.bat from a sibling project.
   In each, replace the project key after the script name:

   .. code-block:: bat

      "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\build.py" FOO %1 %2

4. **CMake**

   Every project needs a CMakeLists.txt and a CMakePresets.json in its folder.

   The CMakeLists.txt includes the shared build system, initializes the target,
   adds its dependencies and finalizes it. E.g. for Libraries/OTFoo:

   .. code-block:: cmake

      cmake_minimum_required(VERSION 3.20)
      project(OTFoo LANGUAGES CXX)

      include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

      ot_initialize_lib(OTFoo OT_FOO_ROOT_PATH)

      ot_add_dependency(OTFoo
          OTSystem
          OTCore
      )

      ot_finalize_lib(OTFoo)
      ot_add_test(OTFoo)

   ``OT_FOO_ROOT_PATH`` comes from the ``OT_FOO_ROOT`` variable of step 1.
   Services build as libraries too, executables use ``ot_initialize_bin`` and ``ot_finalize_bin``.
   See :ref:`Writing a CMakeLists.txt<target Writing a CMakeLists>` and
   :ref:`Dependency Tokens<target CMake Dependency Tokens>`.

   Copy the CMakePresets.json from a sibling project. It only includes the shared
   presets from Scripts/CMake, so it needs no changes. Without it, build.bat cannot configure the project.

   .. code-block:: json

      {
        "version": 7,
        "include": [ "$penv{OT_CMAKE_DIR}/OTPresets.json" ]
      }

5. **Tests** (optional)

   A tests/CMakeLists.txt in the project is enough, TestAll picks it up by itself.

6. **Deployment** (optional)

   If the project ships with OpenTwin, add its binaries to update_libraries
   in DeploymentManifest.py. CreateDeployment runs update_libraries as well.
