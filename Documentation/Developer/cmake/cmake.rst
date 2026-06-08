.. _target CMake Build System:

CMake Build System
==================

OpenTwin builds with CMake. Every library, service and tool is build by a
``CMakeLists.txt`` that handes almost all of the work to a shared meta build
system in ``Scripts/CMake``.

The idea is that each project file stays short and declarative. You declare what the
target is (a library, an executable, a Python subprocess), which dependencies it
needs as tokens, and the system works out the include paths, link
directories, runtime library, export macro, Qt integration and the debug/release setup.

.. toctree::
   :maxdepth: 2

   build_system_overview
   writing_cmakelists
   dependency_tokens
   api_reference

.. note::
   The pages ``how_to/create_new_library`` and ``how_to/create_new_services``
   still describe the old Visual Studio ``.vcxproj`` workflow (manual
   "Configuration Type", ``DebugTest`` and ``ReleaseTest`` configurations, and so
   on). Those steps no longer apply under CMake. Follow
   :ref:`Writing a CMakeLists.txt<target Writing a CMakeLists>` instead.
