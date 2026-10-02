.. _target Writing a CMakeLists:

Writing a CMakeLists.txt
========================

Steps:
Initialize the target, declare its dependencies, then finalize it. 

Any optional tweaks go between initialize and finalize.

.. code-block:: text

   ot_initialize_<kind>(<Target> <ROOT_PATH_VAR> [EXPORT_MACRO])
       ... optional modifiers (warnings, definitions, subsystem, resources) ...
   ot_add_dependency(<Target> <TOKEN> <TOKEN> ...)
   ot_finalize_<kind>(<Target>)
   ot_add_test(<Target>)        # optional

``<ROOT_PATH_VAR>`` is the name of the CMake variable that holds the project's folder.
It is the project's variable name from ``SetupEnvironment.py`` with ``_PATH`` added:

.. code-block:: text

   OT_LOGGER_SERVICE_ROOT         in SetupEnvironment.py
   OT_LOGGER_SERVICE_ROOT_PATH    in ot_initialize_lib(LoggerService OT_LOGGER_SERVICE_ROOT_PATH ...)

The build system fills in the value by itself, nothing else has to be set.
If the two names do not match, configuring stops with "root path var 'OT_FOO_ROOT_PATH' is not set".
See the :ref:`Project Setup Guide<target Project Setup Guide>`, step 2 and 5.

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

A service is a shared library too.

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

.. note::
   Services can also be binaries/executables. In that case use the ``ot_initialize_bin`` and ``ot_finalize_bin``.

An executable
---------------------------------

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

   ot_finalize_bin(uiService uiFrontend)

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

   # F5 debug against the RELEASE OT runtime + release python.
   ot_bin_debug_launch(PythonExecution
       PATH [[${env.OT_ALL_DLLR};${env.OT_PYTHON_BIN}\Release;${env.PATH}]])

   ot_add_test(PythonExecution)

Because the subprocess is ``/MD`` while the rest of a Debug session is ``/MDd``,
F5-debugging it must point Visual Studio at the **release** OT runtime
(``${env.OT_ALL_DLLR}``); otherwise it loads the ``/MDd`` debug OT DLLs and crashes
on a CRT / ``_ITERATOR_DEBUG_LEVEL`` mismatch. ``ot_bin_debug_launch`` writes that
per-project ``launch.vs.json``; the executable is launched directly (no service
loader) and, with no ``ARGS``, the subprocess falls back to its default server name.
See :ref:`API reference<target CMake API Reference>`.

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

Debugging in Visual Studio
--------------------------

To start a project with **F5**, Visual Studio needs a launch configuration: which program to start,
with which arguments and with which ``PATH``. The build system writes it to ``.vs/launch.vs.json``
in the project folder while CMake configures, with one entry for Debug and one for Release.

The file is generated, so do not edit it by hand. Change the ``CMakeLists.txt`` and configure again.
``clean.bat`` deletes the ``.vs`` folder, the next configure writes the file again.

There is one function per kind of project. Each one writes the whole file, so a project uses only one of them:

.. list-table::
   :header-rows: 1
   :widths: 32 38 30

   * - Function
     - For
     - Visual Studio starts
   * - ``ot_service_debug_launch``
     - services in ``Services/`` that need startup arguments
     - ``open_twin.exe`` with the service DLL
   * - ``ot_bin_debug_launch``
     - executables that need a special ``PATH``
     - the executable
   * - ``ot_tool_debug_launch``
     - DLLs outside ``Services/`` that run in ``open_twin.exe``
     - ``open_twin.exe`` with the DLL
   * - ``ot_tool_bin_debug_launch``
     - executables in ``Tools/``
     - the executable

From CMakeLists.txt to F5
~~~~~~~~~~~~~~~~~~~~~~~~~

The Local Session Service shows every step. In its ``CMakeLists.txt``:

.. code-block:: cmake

   ot_service_debug_launch(LocalSessionService
       ARGS "@OPEN_TWIN_LOGGING_URL@"
            "@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_LSS_PORT@"
            "@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_GSS_PORT@"
            "@OPEN_TWIN_AUTH_PORT@")

Every ``@NAME@`` is written to ``launch.vs.json`` as ``${env.NAME}``. The value is not filled in yet:

.. code-block:: json

   "exe": "${env.OPENTWIN_DEV_ROOT}\\Framework\\OpenTwin\\target\\debug\\open_twin.exe",
   "args": [ "C:\\...\\LocalSessionService\\build\\windows-debug\\Debug\\LocalSessionService.dll",
             "${env.OPEN_TWIN_LOGGING_URL}",
             "${env.OPEN_TWIN_SERVICES_ADDRESS}:${env.OPEN_TWIN_LSS_PORT}",
             "${env.OPEN_TWIN_SERVICES_ADDRESS}:${env.OPEN_TWIN_GSS_PORT}",
             "${env.OPEN_TWIN_AUTH_PORT}" ],

When you press F5, Visual Studio fills in the values from the environment it was started with by ``edit.bat``.
With the default values the service is started as:

.. code-block:: text

   open_twin.exe ...\LocalSessionService.dll 127.0.0.1:8090 127.0.0.1:8093 127.0.0.1:8091 8092

The defaults come from ``Scripts/Launcher/ot_launcher/service_args.py``. A variable you set yourself,
for example ``OPEN_TWIN_SERVICES_ADDRESS``, wins over the default. Restart Visual Studio with
``edit.bat`` after changing one, an open Visual Studio keeps the old values.

Because the values are only filled in at F5, changing a port needs no new configure.
Changing the ``ARGS`` list in the ``CMakeLists.txt`` does.

Services
~~~~~~~~

``ot_finalize_lib`` writes the launch configuration for every target in ``Services/`` by itself.
It always starts ``open_twin.exe`` and puts the path of the built service DLL in front of the ``ARGS``.
The ``PATH`` is ``OT_ALL_DLLD`` (or ``OT_ALL_DLLR``) plus the normal ``PATH``.

Most services need no ``ARGS`` at all: in a Debug build they read their configuration from a ``.cfg``
written by the Local Directory Service. Only the **backbone services** that run stand-alone
(logger, authorisation, session and directory services) declare ``ARGS``.

``ot_service_debug_launch`` only stores the ``ARGS``, ``ot_finalize_lib`` writes them into the file.
So it has to come **before** ``ot_finalize_lib``, otherwise the arguments are missing.

The arguments are passed by position, in the order the service expects them. Use the same values and
order as the start scripts in ``Scripts/Launcher``. A position cannot be left out, the next value would move
into its place. A service that does not read a position gets the placeholder ``"unused"`` there,
as in ``LoggerService``, which only reads its own address:

.. code-block:: cmake

   ot_service_debug_launch(LoggerService
       ARGS "unused"                                             # 1: not read
            "@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_LOG_PORT@"  # 2: its own address
            "unused"                                             # 3: not read
            "unused"                                             # 4: not read
   )

Writing the arguments
~~~~~~~~~~~~~~~~~~~~~

These rules apply to the ``ARGS``, ``ARGSD``, ``ARGSR``, ``PATHD`` and ``PATHR`` of all launch functions:

- Write every argument as its own string in double quotes: ``"--config"``, ``"127.0.0.1:8080"``.
  Text outside of ``@...@`` is passed as it is.
- ``@NAME@`` stands for the environment variable ``NAME``. It can be mixed with text,
  ``"@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_LSS_PORT@"`` becomes ``127.0.0.1:8093``.
- A backslash is written as ``\\``, because CMake reads a single ``\`` as an escape character:
  ``"@OPENTWIN_DEV_ROOT@\\Tools\\FileHeaderUpdater"``. The JSON escaping is done by the build system.
- An argument must not contain ``;``, CMake would split it into two arguments.
  In ``PATHD`` and ``PATHR`` the ``;`` separates the folders as usual.
- If a variable used in an argument is not set when CMake configures, the whole argument is passed empty.
  Check the variable first if a service complains about an empty argument.

Executables
~~~~~~~~~~~

An executable is started directly by Visual Studio, its arguments go to its ``main()`` as usual.
``ot_bin_debug_launch`` is for an executable that needs a special ``PATH``. Call it **after** ``ot_finalize_bin``.

``PythonExecution`` uses it, because it runs the release runtime in Debug as well
(see :ref:`Runtime library<target CMake Runtime Library>`). It must load the release OpenTwin DLLs,
the debug ones would crash it:

.. code-block:: cmake

   ot_finalize_bin(PythonExecution)

   ot_bin_debug_launch(PythonExecution
       # release OpenTwin DLLs ; release Python ; the normal PATH
       PATH [[${env.OT_ALL_DLLR};${env.OT_PYTHON_BIN}\Release;${env.PATH}]])

Unlike the other functions, ``PATH`` here is written in the Visual Studio syntax ``${env.NAME}`` instead of ``@NAME@``.
The ``[[...]]`` brackets keep CMake from changing the ``$`` and the ``\``, so a single ``\`` is enough inside them.
``ARGS`` applies to Debug and Release alike.

Tools
~~~~~

The two tool functions take separate values per configuration: ``ARGSD`` and ``PATHD`` for Debug,
``ARGSR`` and ``PATHR`` for Release. Arguments that are left out are empty, a ``PATH`` that is left out
gets the default. Call them **after** ``ot_finalize_bin`` or ``ot_finalize_lib``.

``ot_tool_bin_debug_launch`` is for a tool executable. ``FileHeaderUpdater`` gets arguments for a safe test run in Debug:

.. code-block:: cmake

   ot_finalize_bin(FileHeaderUpdater)

   ot_tool_bin_debug_launch(FileHeaderUpdater
       ARGSD "--dry"         # dry run: report the changes, modify no file
             "--config"      # the next argument is the configuration file
             "@OPENTWIN_DEV_ROOT@\\Tools\\FileHeaderUpdater\\OT_FHU_Config.json"
   )

In Debug, F5 runs ``FileHeaderUpdater.exe --dry --config C:\...\OT_FHU_Config.json``.
There is no ``ARGSR``, so Release starts it without arguments.

``EndpointDocParser`` needs no arguments, only the debug DLLs of ``OTSystem`` and ``OTCore``:

.. code-block:: cmake

   ot_tool_bin_debug_launch(EndpointDocParser
       # OTSystem debug DLL ; OTCore debug DLL ; zlib debug DLL ; the normal PATH
       PATHD "@OT_SYSTEM_ROOT@\\@OT_CDLLD@;@OT_CORE_ROOT@\\@OT_CDLLD@;@ZLIB_DLLPATHD@;@PATH@"
   )

Without ``PATHD``, ``ot_tool_bin_debug_launch`` uses the ``PATH`` Visual Studio was started with.
End your own ``PATH`` with ``@PATH@``, otherwise the normal ``PATH`` is lost.

``ot_tool_debug_launch`` is for a DLL that runs in ``open_twin.exe`` like a service but lives outside ``Services/``,
such as ``OToolkit``. Its arguments are passed by position like those of a service. ``OToolkit`` reads position 2 as its own address
and position 4 for its own options, 1 and 3 are not read:

.. code-block:: cmake

   ot_finalize_lib(OToolkit)

   ot_tool_debug_launch(OToolkit
       ARGSD "unused" "127.0.0.1:8080" "unused" "unused"
       ARGSR "unused" "127.0.0.1:8094" "127.0.0.1:8095" "unused"
       PATHD "@OT_DEFAULT_SERVICE_DLLD@;@OT_GUI_ROOT@\\@OT_CDLLD@;@OT_WIDGETS_ROOT@\\@OT_CDLLD@;@OT_OTOOLKITAPI_ROOT@\\@OT_CDLLD@;@QT_ADS_ROOT@\\lib;@QT_DLLD@;@PATH@"
       PATHR "@QT_DLLR@;@OT_DEFAULT_SERVICE_DLLR@;@OT_WIDGETS_ROOT@\\@OT_CDLLR@;@OT_OTOOLKITAPI_ROOT@\\@OT_CDLLR@;@QT_ADS_ROOT@\\lib;@PATH@"
   )

Its default ``PATH`` is ``OT_ALL_DLLD`` or ``OT_ALL_DLLR`` plus the normal ``PATH``. ``OToolkit`` sets its own,
because it also needs Qt and the Qt Advanced Docking System.

.. note::
   Third-party runtime DLLs (Python, ngspice, ...) live in ``Deployment``, which is on
   ``PATH``, so services that link them load without extra configuration.
   ``CircuitExecution`` is a plain executable that Visual Studio starts directly, it needs no launch declaration.
   ``PythonExecution`` is the exception, see Executables above.
