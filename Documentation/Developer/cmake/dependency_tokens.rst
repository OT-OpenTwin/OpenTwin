.. _target CMake Dependency Tokens:

Dependency Tokens
=================

``ot_add_dependency(<Target> <TOKEN> ...)`` is where you list the project
dependencies to build. Each token is a short name that the build system turns into the
right include directories (on the ``_core`` object library) and the right link
directories and link libraries (on the final target).

.. code-block:: cmake

   ot_add_dependency(MyTarget
       OTCore OTCommunication      # OpenTwin libraries
       QtWidgets                   # Qt
       RJSON CURL MONGO_C          # third party
       OSLibs                      # OS system libraries
   )

Order does not matter.
A token that is not recognised produces a warning at
configure time, so typos are easy to spot.

OpenTwin library tokens
-----------------------

Any token that names an OpenTwin library (``OTCore``, ``OTSystem``,
``OTCommunication``, ``OTGui``, ``OTGuiAPI``, ``OTWidgets``,
``OTServiceFoundation``, ``OTModelAPI``, ``OTModelEntities``,
``OTDataStorage``, ``OTBlockEntities``, ``OToolkitAPI`` and so on) is resolved
through its ``OT_<NAME>_ROOT`` environment variable. The build system adds the
library's include directory to your core target and links its import library to
the final target, picking the correct paths per configuration.

Most names follow a simple rule: ``OTFooBar`` maps to ``OT_FOO_BAR_ROOT``. A few
older names break that rule and are mapped by hand:

.. list-table::
   :header-rows: 1
   :widths: 40 60

   * - Token
     - Environment variable
   * - ``OTServiceFoundation``
     - ``OT_FOUNDATION_ROOT``
   * - ``OTDataStorage``
     - ``OT_DATASTORAGE_ROOT``
   * - ``OTModelEntities``
     - ``OT_MODELENTITIES_ROOT``
   * - ``OTCADEntities``
     - ``OT_CADMODELENTITIES_ROOT``
   * - ``OTRubberband``
     - ``OT_RUBBERBANDAPI_ROOT``
   * - ``OToolkitAPI``
     - ``OT_OTOOLKITAPI_ROOT``
   * - ``OToolkit``
     - ``OT_OTOOLKIT_ROOT``
   * - ``UICore``
     - ``OT_UICORE_ROOT``
   * - ``OTFMC``
     - ``OT_FILE_MANAGER_CONNECTOR_ROOT`` (with fallbacks)

If you add a new OpenTwin library whose root variable does not match the
``OT_FOO_BAR_ROOT`` pattern, add a mapping in the ``_ot_get_ot_root_envvar``
function in ``OTProject.cmake``.

.. note::
This will be revised in the upcoming Environment setup changes via Python in the future.

Qt tokens
---------

Qt tokens create the imported ``Qt6::*`` targets on demand and link them. As soon
as one ``Qt*`` token is present, ``AUTOMOC`` is turned on for you at finalize.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Token
     - Links
   * - ``QtCore``
     - ``Qt6::Core``
   * - ``QtGui``
     - ``Qt6::Gui``
   * - ``QtWidgets``
     - ``Qt6::Widgets`` (Core, Gui, Widgets)
   * - ``QtNetwork``
     - ``Qt6::Network``
   * - ``QtWebSockets``
     - ``Qt6::WebSockets``
   * - ``QtOpenGLWidgets``
     - ``Qt6::OpenGLWidgets``
   * - ``QtEntryPoint``
     - ``Qt6::EntryPoint``, the ``WinMain`` shim. Only needed for a Qt GUI
       executable built on the ``WINDOWS`` subsystem.
   * - ``QtFull``
     - Core, Gui, Widgets, Network, Svg, Xml, OpenGLWidgets, SvgWidgets, Qml,
       WebSockets

Qt resource (``.qrc``) files are handled by
``ot_add_qt_resources(T)``, which turns on ``AUTORCC``.

Third party tokens
------------------

These map to the vendored third party headers and libraries through their own
environment variables. Header only tokens add an include directory; the rest add
both include and link information.

.. list-table::
   :header-rows: 1
   :widths: 25 55 20

   * - Token
     - Library
     - Kind
   * - ``RJSON``
     - RapidJSON
     - header only
   * - ``BASE64``
     - base64, compiled into the target
     - source
   * - ``TINYXML2``
     - TinyXML-2, compiled into the target
     - source
   * - ``EXPREVAL``
     - tinyexpr, compiled into the target (C)
     - source
   * - ``EARCUT``
     - earcut polygon triangulation
     - header only
   * - ``CURL``
     - libcurl
     - lib
   * - ``MONGO_C``
     - MongoDB C driver
     - lib
   * - ``MONGO_CXX``
     - MongoDB C++ driver
     - lib
   * - ``MONGO_BOOST``
     - Boost headers used by the Mongo C++ driver
     - header only
   * - ``BOOST``
     - Boost
     - lib
   * - ``BOOST_FILESYSTEM``
     - Boost.Filesystem
     - lib
   * - ``ADS``
     - Qt Advanced Docking System
     - lib
   * - ``QWT``
     - Qwt plotting
     - lib
   * - ``OSG``
     - OpenSceneGraph
     - lib
   * - ``OCCT``
     - OpenCASCADE
     - lib
   * - ``VTK``
     - VTK
     - lib
   * - ``EMBREE``
     - Embree
     - lib
   * - ``GMP``
     - GMP
     - lib
   * - ``GMSH``
     - Gmsh
     - lib
   * - ``NGSPICE``
     - ngspice
     - lib
   * - ``PYTHON``
     - CPython 3.11
     - lib
   * - ``PYTHON_LEGACY``
     - Legacy CPython include and numpy paths
     - include

Use the ``PYTHON`` token together with ``ot_initialize_bin_python`` (see
:ref:`Runtime library<target CMake Runtime Library>`).

System and special tokens
-------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Token
     - Effect
   * - ``OSLibs``
     - Links the standard Windows system libraries: ``userenv``, ``ws2_32``,
       ``advapi32``, ``shell32``, ``bcrypt``, ``secur32``, ``pdh``, ``odbc32``.
   * - ``WINLIB:<name>``
     - Links a single Windows system library, for example
       ``WINLIB:Crypt32``. The ``.lib`` suffix is optional; the linker adds it.
   * - ``OTEncryptionKey``
     - Adds the encryption key include directory only.

For example, ``OTSystem`` needs the Windows crypto library for single sign on:

.. code-block:: cmake

   ot_add_dependency(OTSystem
       OSLibs
       WINLIB:Crypt32
   )

Fallbacks
----------------

If a token is none of the above, the build system looks for a matching set of
environment variables and wires it up generically:

* include: ``<TOKEN>_INC`` (or per config ``<TOKEN>_INCD`` / ``<TOKEN>_INCR``)
* link dirs: ``<TOKEN>_LIBPATHD`` / ``<TOKEN>_LIBPATHR``
* link libs: ``<TOKEN>_LIBD`` / ``<TOKEN>_LIBR`` (or a single ``<TOKEN>_LIB``)

So a simple new third party dependency can often be added just by defining those
variables in ``SetupEnvironment.bat`` and using the token, with no change to
``OTProject.cmake``. For anything less regular, add a branch in
``_ot_apply_dep_to_core`` and ``_ot_apply_dep_to_final``.
