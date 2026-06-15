.. _target Writing a CMakeLists:

Writing a CMakeLists.txt
========================

Every project follows the same three steps: initialize the target, declare its
dependencies, then finalize it. Any optional tweaks go between initialize and
finalize.

.. code-block:: text

   ot_initialize_<kind>(<Target> <ROOT_PATH_VAR> [EXPORT_MACRO])
       ... optional modifiers (warnings, definitions, subsystem, resources) ...
   ot_add_dependency(<Target> <TOKEN> <TOKEN> ...)
   ot_finalize_<kind>(<Target>)
   ot_add_test(<Target>)        # optional

``<ROOT_PATH_VAR>`` is the name of the CMake variable that holds the project's
root path. ``OTEnvironment.cmake`` derives it from the ``OT_<NAME>_ROOT``
environment variable, for example ``OT_LOGGER_SERVICE_ROOT`` becomes
``OT_LOGGER_SERVICE_ROOT_PATH``.

Preamble
--------

Every ``CMakeLists.txt`` starts the same way:

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.20)
   project(<Target> LANGUAGES CXX)

   include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

A library (DLL)
---------------

Libraries live in ``Libraries/`` and build as shared libraries. Use
``ot_initialize_lib`` and ``ot_finalize_lib``:

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.20)
   project(OTExample LANGUAGES CXX)

   include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

   ot_initialize_lib(OTExample OT_EXAMPLE_ROOT_PATH)

   ot_add_dependency(OTExample
       OTSystem
       OTCore
       RJSON
   )

   ot_finalize_lib(OTExample)
   ot_add_test(OTExample)

The export macro controls the ``__declspec(dllexport/dllimport)`` switch. Leave it
out and you get ``OPENTWIN<NAME>_EXPORTS`` by default, so ``OTExample`` ends up
with ``OPENTWINEXAMPLE_EXPORTS``. To set your own, pass it as the third argument:

.. code-block:: cmake

   ot_initialize_lib(OToolkitAPI OT_OTOOLKITAPI_ROOT_PATH OTOOLKITAPI_LIB)

A service (DLL)
---------------

A service is a shared library too; it just tends to pull in more dependencies.
The export macro is usually ``SESSIONSERVICE_EXPORTS``:

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.20)
   project(LoggerService LANGUAGES CXX)

   include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

   ot_initialize_lib(LoggerService OT_LOGGER_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
   ot_disable_warnings(LoggerService 4996)

   ot_add_dependency(LoggerService
       OTSystem
       OTCore
       OTCommunication
       OTServiceFoundation
       RJSON
       CURL
       OSLibs
   )

   ot_finalize_lib(LoggerService)
   ot_add_test(LoggerService)

An executable (command line tool)
---------------------------------

Tools live in ``Tools/`` and use ``ot_initialize_bin`` and ``ot_finalize_bin``.

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.20)
   project(KeyGenerator LANGUAGES CXX)

   include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

   ot_initialize_bin(KeyGenerator OT_KEYGENERATOR_ROOT_PATH)
   ot_set_subsystem(KeyGenerator CONSOLE)
   ot_set_runtime_static_release(KeyGenerator)   # ships as a self contained exe

   ot_add_dependency(KeyGenerator
       OTSystem
       OTCore
       OSLibs
   )

   ot_finalize_bin(KeyGenerator)

``ot_finalize_bin`` takes an optional output name if the produced ``.exe`` should
differ from the target name:

.. code-block:: cmake

   ot_finalize_bin(MongoDBUpgradeManager "MongoDBUpgrade")

.. note::
   ``ot_set_runtime_static_release`` is only for stand alone tools that ship as a
   single executable. Do not use it for libraries, services, or anything that
   runs next to the OpenTwin DLLs; those stay on the dynamic CRT.

A Python subprocess
-------------------

A binary that embeds Python uses ``ot_initialize_bin_python``. It keeps the
release runtime in every configuration so it can link the shipped Python
(see :ref:`Runtime library<target CMake Runtime Library>`):

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.20)
   project(PythonExecution LANGUAGES CXX)

   include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

   ot_initialize_bin_python(PythonExecution OT_PYTHON_EXECUTION_ROOT_PATH SESSIONSERVICE_EXPORTS)
   ot_disable_warnings(PythonExecution 4996)
   ot_add_compile_definitions(PythonExecution QT_NO_KEYWORDS)
   ot_set_subsystem(PythonExecution CONSOLE)

   ot_add_dependency(PythonExecution
       OTSystem OTCore OTGui OTCommunication OTServiceFoundation
       OTModelAPI OTBlockEntities
       RJSON CURL MONGO_C MONGO_CXX MONGO_BOOST
       PYTHON
       QtCore QtNetwork QtWidgets
   )

   ot_finalize_bin(PythonExecution)
   ot_add_test(PythonExecution)

.. warning::
   A service that just talks to the Python subprocess over the service interface 
   is an ordinary library and uses ``ot_initialize_lib``.

Unit tests
----------

If ``ot_add_test(<Target>)`` is present, the build system descends into the
project's ``tests/`` directory when it exists and ``BUILD_TESTING`` is on. The
``tests/CMakeLists.txt`` reuses the already compiled core objects:

.. code-block:: cmake

   # <Project>/tests/CMakeLists.txt
   cmake_minimum_required(VERSION 3.20)
   project(OTExample_tests LANGUAGES CXX)

   include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

   ot_initialize_test(OTExample_tests OTExample)

``ot_initialize_test`` compiles ``tests/src/*.cpp``, links gtest, matches the
runtime library of the target under test, and registers the test with CTest. The
first argument is the test executable name; the second is the main target whose
core objects and dependencies it reuses.

.. warning::
   If ``ot_add_test()`` is defined in the core target CMakeLists, and no tests subdir is present,
   the configuration step will throw a ``BUILD_TESTING`` not configured warning.

Modifiers (between initialize and finalize)
-------------------------------------------

These act on the core target. Call them after ``ot_initialize_*`` and before
``ot_finalize_*``:

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - Call
     - Effect
   * - ``ot_add_compile_definitions(T defs…)``
     - Adds private preprocessor definitions.
   * - ``ot_add_compile_options(T opts…)``
     - Adds raw compile flags (a bare number becomes ``/wd<number>``).
   * - ``ot_disable_warnings(T ids…)``
     - Suppresses warnings by numeric ID (``/wd<id>``).
   * - ``ot_set_subsystem(T CONSOLE|WINDOWS)``
     - Sets the linker subsystem of the final binary.
   * - ``ot_set_subsystem_for_config(...)``
     - Same, but only for one configuration.
   * - ``ot_set_runtime_static_release(T)``
     - Static CRT (``/MT``) in Release, for tools only.
   * - ``ot_add_qt_resources(T)``
     - Globs ``*.qrc`` and enables ``AUTORCC``.
   * - ``ot_add_resources(T)``
     - Globs ``*.rc`` and ``*.ico`` into the target.
   * - ``ot_deploy_app_configuration(T)``
     - Copies ``qt.conf`` next to the binary after build.
   * - ``ot_service_debug_launch(T ...)``
     - Declares the Visual Studio F5 launch contract for a service (see below).

See the :ref:`API reference<target CMake API Reference>` for full signatures and
:ref:`Dependency tokens<target CMake Dependency Tokens>` for everything you can
pass to ``ot_add_dependency``.

.. _target Debugging services:

Debugging services in Visual Studio
-----------------------------------

``ot_finalize_lib`` generates a ``.vs/launch.vs.json`` for every ``Services/`` target,
so **F5** launches the service under the loader. Most services need nothing more: in a
Debug build they read their configuration from a ``.cfg`` written by the Local
Directory Service, so the launch arguments are ignored.

Only the **backbone services** that run stand-alone and parse their own arguments
(logger, authorisation, session and directory services) call ``ot_service_debug_launch``
(between initialize and finalize) to declare them with ``ARGS``, using ``@NAME@`` tokens
for environment values:

.. code-block:: cmake

   ot_service_debug_launch(LocalSessionService
       ARGS "@OPEN_TWIN_LOGGING_URL@"
            "@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_LSS_PORT@"
            "@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_GSS_PORT@"
            "@OPEN_TWIN_AUTH_PORT@")

.. note::
   Third-party runtime DLLs (Python, ngspice, ...) live in ``Deployment``, which is on
   ``PATH``, so services that link them load without extra configuration. The execution
   subprocesses (``PythonExecution``, ``CircuitExecution``) are plain executables that
   Visual Studio launches directly and resolve their DLLs the same way, so they need no
   launch declaration either.
