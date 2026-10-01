.. _target CMake Build System Overview:

Overview and Architecture
=========================

CMake system
---------------------------

All of the shared CMake logic lives in ``Scripts/CMake``:

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - File
     - Purpose
   * - ``OTProject.cmake``
     - The public API. It provides every ``ot_*`` function a project calls, and
       including it also applies the global MSVC compile flags, the common
       definitions and the default runtime library.
   * - ``OTEnvironment.cmake``
     - Reads the OpenTwin environment variables that ``SetupEnvironment.py``
       sets and exposes them as CMake variables and path helpers.
   * - ``OTQt.cmake``
     - Finds Qt 6 and defines the imported ``Qt6::*`` targets used by the
       ``Qt*`` dependency tokens.
       Instead of using ``find_package()`` we directly read set environment for our ``Qt`` packges.
       A Qt outside the ThirdParty folder is rejected, so a system Qt is never picked up by accident.
   * - ``OTPlatform.cmake``
     - Everything that differs between platforms and compilers: the system libraries behind
       ``OSLibs``, the ``WINLIB:`` tokens, warning IDs and the Qt library file names.
   * - ``OTPresets.json``
     - The shared ``CMakePresets`` definition that every project includes, so
       the configurations (such as ``windows-debug`` and ``windows-release``)
       are defined in one place. See :ref:`Configurations and presets<target CMake Presets>`.
   * - ``templates/``
     - Templates for the Visual Studio ``launch.vs.json``, see
       :ref:`Debugging in Visual Studio<target Debugging services>`.

A project finds the meta system through the ``OT_CMAKE_DIR`` environment
variable, which points at ``Scripts/CMake``. ``SetupEnvironment.py`` exports it
together with the per project root variables (``OT_<NAME>_ROOT``) and the third
party paths.

.. note::
   The System depends on the set environment variable ``OT_CMAKE_DIR``, which is defined in the
   ``Scripts/SetupEnvironment.py`` of OpenTwin.
   If unset, CMake wont be able to find all the CMake scripts defined in ``/Scripts/CMake`` as well as the presets.

The two target model: ``_core`` plus the final target
-----------------------------------------------------

Every project builds as two CMake targets.

One is the core object library ``<TARGET>_core``.
It compiles everything under ``src/`` and
``include/`` and carries the compile flags, preprocessor definitions, include
directories, the runtime library and the export macro.

The other is the final target. ``ot_finalize_lib`` builds the DLL from those core
objects, while ``ot_finalize_bin`` builds an executable. Either one picks up the
link directories and link libraries.

.. code-block:: text

   ot_initialize_lib(Foo ...)   ->  creates OBJECT lib  Foo_core
   ot_add_dependency(Foo ...)   ->  records tokens on   Foo_core
   ot_finalize_lib(Foo)         ->  creates SHARED lib  Foo  (from Foo_core objects)
   ot_add_test(Foo)             ->  adds tests/ -> Foo_tests exe (reuses Foo_core objects)

Expected project layout
------------------------

The initialize functions discover sources by convention, so a project needs this
layout:

.. code-block:: text

   <Project>/
     CMakeLists.txt          # the project definition (see next page)
     include/                # *.h / *.hpp
     src/                    # *.cpp / *.c
     tests/                  # optional unit tests
       CMakeLists.txt
       src/                  # *.cpp for the test executable

Sources are found with ``file(GLOB_RECURSE ... CONFIGURE_DEPENDS)``, so adding or
removing a file under ``src/`` or ``include/`` is picked up on the next build.
You do not list files by hand.

OTProject.cmake
---------------------

Including ``OTProject.cmake`` applies the OpenTwin standard MSVC setup to
every target. The main pieces are:

* C++20 with compiler extensions disabled.
* Flags such as ``/permissive-``,
  ``/Zc:__cplusplus``, ``/Zc:preprocessor``, ``/EHsc``, ``/MP`` for parallel
  compilation, and external header warning suppression.
* The dynamic CRT by default: ``/MD`` in Release, ``/MDd`` in Debug. A target
  overrides this only when it has to.
* Common definitions everywhere (``WIN32``, ``_WIN32``, ``_WINDOWS``,
  ``UNICODE``, ``_UNICODE``), plus a few Debug only ones.
* ``/OPT:REF`` and ``/OPT:ICF`` for Release linking.
* ``_DEBUG`` in Debug and ``NDEBUG`` in Release on the core (Python targets
  differ, see below).

.. note::
   The overal compiler settings will be adjusted in the future.
   E.g. ``/W3`` (Warning level 3) will be added globally.

.. _target CMake Runtime Library:

Runtime library and the Debug/Release mapping
---------------------------------------------

There are three runtime profiles. You pick one by choosing the initialize
function, plus an optional helper:

.. list-table::
   :header-rows: 1
   :widths: 44 28 28

   * - Profile
     - Debug
     - Release
   * - Default (libraries and most binaries)
     - ``/MDd`` + ``_DEBUG``
     - ``/MD`` + ``NDEBUG``
   * - ``ot_set_runtime_static_release`` (tools)
     - ``/MDd``
     - ``/MT`` (static)
   * - ``ot_initialize_bin_python`` (subprocess)
     - ``/MD`` + ``_RELEASEDEBUG``
     - ``/MD`` + ``NDEBUG``

The default profile uses the dynamic CRT in both configurations. All libraries
and services use it, and so do binaries that run next to the OpenTwin DLLs.

Command line tools that ship as a single executable use
``ot_set_runtime_static_release``, so Release links the static CRT while Debug
stays on ``/MDd`` for normal debugging.

Python only ships a release build with debug information, so a binary that embeds Python always uses
the release runtime, even in Debug. ``ot_initialize_bin_python`` handles that distinction:
it keeps ``/MD`` in every configuration and, in Debug, defines ``_RELEASEDEBUG``
instead of ``_DEBUG`` so the service's debug code paths are still active.

.. _target CMake Presets:

Configurations and presets
--------------------------

All presets are defined once, in ``Scripts/CMake/OTPresets.json``. Every project has its own
``CMakePresets.json`` next to its ``CMakeLists.txt``, but that file only includes the shared one:

.. code-block:: json

   {
     "version": 7,
     "include": [ "$penv{OT_CMAKE_DIR}/OTPresets.json" ]
   }

So to change a compiler setting or a build folder for all projects, edit ``OTPresets.json``.
The project files never need changes, a new project simply copies one from a sibling.

``OTPresets.json`` defines two configure presets:

.. list-table::
   :header-rows: 1
   :widths: 25 35 40

   * - Configure preset
     - Build folder
     - Build preset
   * - ``windows-debug``
     - ``build/windows-debug``
     - ``build-windows-debug`` (builds ``Debug``)
   * - ``windows-release``
     - ``build/windows-release``
     - ``build-windows-release`` (builds ``Release``)

Both use the ``Ninja Multi-Config`` generator with the Ninja and the MSVC compiler (``cl.exe``)
that come with Visual Studio 2022. They also write a ``compile_commands.json``, which editors such as
VS Code use for code completion.

``build.bat`` uses exactly these presets: it runs ``cmake --preset windows-debug`` and then
``cmake --build --preset build-windows-debug``, the same for Release. Visual Studio reads the same
file when it opens the project folder and shows both presets in its configuration list.

The finished binaries end up in ``build/windows-debug/Debug`` and ``build/windows-release/Release``.
These folders match ``OT_CDLLD`` and ``OT_CDLLR`` in ``Scripts/SetupEnvironment.py``, which is how other
projects, the tests and the deployment find them. If the build folders in ``OTPresets.json`` change,
those two values have to change with them.

The presets also put the OpenTwin and ThirdParty DLL folders and ``Deployment`` on ``PATH``
(the debug or the release ones), so programs started from Visual Studio find their DLLs.
The ``test-windows-debug`` and ``test-windows-release`` presets do the same for ``ctest``.
