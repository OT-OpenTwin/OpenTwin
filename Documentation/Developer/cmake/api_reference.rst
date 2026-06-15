.. _target CMake API Reference:

API Reference
=============

The public functions provided by ``OTProject.cmake``. Square brackets denote
optional arguments. ``<Target>`` is the project/target name passed to
``project()``.

Initialize
----------

``ot_initialize_lib(<Target> <ROOT_PATH_VAR> [EXPORT_MACRO])``
    Creates the object library ``<Target>_core`` from ``src/`` and ``include/``,
    applies the C++20 standard, the default runtime, the Debug/Release
    definitions and ``_USRDLL``, and sets the dll-export macro
    (default ``OPENTWIN<NAME>_EXPORTS``). Use for libraries and services.

``ot_initialize_bin(<Target> <ROOT_PATH_VAR> [EXPORT_MACRO])``
    Like ``ot_initialize_lib`` but marks the target as an application. The
    export macro is optional. Use for executables.

``ot_initialize_bin_python(<Target> <ROOT_PATH_VAR> [EXPORT_MACRO])``
    Like ``ot_initialize_bin`` but forces the ``/MD`` runtime in every
    configuration and defines ``_RELEASEDEBUG`` (Debug) / ``NDEBUG`` (Release)
    instead of ``_DEBUG``, so the target links the release ``python311.lib``.
    Use only for binaries that embed CPython.

Dependencies
------------

``ot_add_dependency(<Target> <TOKEN> ...)``
    Records dependency tokens on ``<Target>_core``. Resolved at finalize. See
    :ref:`Dependency tokens<target CMake Dependency Tokens>`. May be called
    multiple times; tokens accumulate.

Finalize
--------

``ot_finalize_lib(<Target>)``
    Creates the shared library ``<Target>`` from the core objects, exposes
    ``include/`` as a ``PUBLIC`` include directory, applies all recorded
    dependencies and the requested subsystem, and enables ``AUTOMOC`` if any
    ``Qt*`` token is present.

``ot_finalize_bin(<Target> [OUTPUT_NAME])``
    Creates the executable ``<Target>`` from the core objects. ``WIN32_EXECUTABLE``
    is set according to the subsystem (Release GUI vs console). Pass
    ``OUTPUT_NAME`` to give the produced ``.exe`` a different file name. Applies
    dependencies, subsystem and ``AUTOMOC`` as for ``ot_finalize_lib``.

Tests
-----

``ot_add_test(<Target>)``
    If ``BUILD_TESTING`` is on and ``tests/CMakeLists.txt`` exists, adds the
    ``tests`` subdirectory. Safe to call when no tests exist (it just logs a
    status message).

``ot_initialize_test(<TestTarget> <MainTarget>)``
    Called inside ``tests/CMakeLists.txt``. Builds ``<TestTarget>`` from
    ``tests/src/*.cpp`` plus the compiled ``<MainTarget>_core`` objects, inherits
    the main target's export macro, dependencies and runtime library, links the
    gtest library and registers the test with CTest.

Compile options and definitions
-------------------------------

``ot_add_compile_definitions(<Target> <def> ...)``
    Adds private preprocessor definitions to the core target.

``ot_add_compile_options(<Target> <opt> ...)``
    Adds raw compile options to the core target. A bare numeric argument is
    treated as a warning to disable and becomes ``/wd<number>``.

``ot_disable_warnings(<Target> <id> ...)``
    Convenience wrapper that suppresses warnings by numeric ID. Non-numeric
    arguments produce an author warning (use ``ot_add_compile_options`` for
    flags).

Subsystem
---------

``ot_set_subsystem(<Target> CONSOLE|WINDOWS)``
    Sets the linker subsystem for the final binary (applied on finalize).
    ``CONSOLE`` keeps a ``main()`` entry point and console window; ``WINDOWS``
    produces a windowed application.

``ot_set_subsystem_for_config(<Target> <Config> CONSOLE|WINDOWS)``
    Overrides the subsystem for a single configuration (``Debug``, ``Release``,
    ``RelWithDebInfo`` or ``MinSizeRel``). Configuration-specific overrides take
    precedence over ``ot_set_subsystem``.

Runtime library
---------------

``ot_set_runtime_static_release(<Target>)``
    Links the static CRT (``/MT``) in Release while keeping ``/MDd`` in Debug.
    For stand-alone command line tools that ship as a single self-contained
    executable. Never use it for libraries, services, or Python targets.

Resources and deployment
------------------------

``ot_add_qt_resources(<Target>)``
    Globs ``*.qrc`` under the project and enables ``AUTORCC``.

``ot_add_resources(<Target>)``
    Globs ``*.rc`` and ``*.ico`` under the project into the target.

``ot_deploy_app_configuration(<Target>)``
    Adds a post-build step copying ``qt.conf`` (if present) next to the binary.

Debug launch (Visual Studio F5)
-------------------------------

``ot_service_debug_launch(<Target> ARGS <arg> ...)``
    Declares the arguments Visual Studio passes to the service loader (after the
    service DLL path) when the service is launched with F5. ``@NAME@`` tokens are
    emitted as ``${env.NAME}`` and resolved by Visual Studio at debug time. Most
    services do not need this; see
    :ref:`Debugging services in Visual Studio<target Debugging services>`.

Internal helpers
----------------

Functions prefixed with ``_ot_`` (for example ``_ot_initialize_target``,
``_ot_apply_dep_to_core``, ``_ot_apply_dep_to_final``, ``_ot_get_ot_root_envvar``)
are implementation details of the meta system. Do not call them from a project
``CMakeLists.txt``. Extend them only when adding a new dependency token or
environment-variable mapping that the public API cannot express.
