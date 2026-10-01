.. _target BuildOrder:

BuildOrder
==========

``Scripts/BuildOrder.py``: the planned build order of the ``BuildAll`` and ``RebuildAll`` scripts.

Add or change the build order by changing the ``BUILD_ORDER`` list.
Each entry is a :ref:`project key<target Project keys>`, so ``OT_FOO_ROOT`` is listed as ``"FOO"``.

The projects are built from top to bottom. A project has to come after everything it links against,
which is why ``SYSTEM`` and ``CORE`` are at the top: almost every other project depends on them.
A project that is not in the list is skipped by ``BuildAll`` and ``RebuildAll``, its own ``build.bat`` still works.

.. code-block:: python

   BUILD_ORDER: list[str] = [
       "SYSTEM",
       "CORE",
       # ...
       "FOO",      # after everything FOO links against
   ]

Two batch files in ``Scripts/BuildAndTest`` build this list. ``BuildAll.bat`` builds both configurations
without cleaning first, ``RebuildAll.bat`` passes its own arguments on, so without arguments it cleans and builds everything:

.. code-block:: bat

   REM BuildAll.bat
   "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\build_all.py" BOTH BUILD

   REM RebuildAll.bat, takes the same arguments as a project build.bat
   "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\build_all.py" %1 %2

You can override the configurations of certain builds by inserting the project key into the ``BUILD_OVERRIDES``.
The override is used instead of the arguments ``BuildAll`` or ``RebuildAll`` was started with.

.. code-block:: python

   BUILD_OVERRIDES = {
       # key: (configurations, rebuild)
       "KEYGENERATOR": (["release"], True),   # only Release, always as a full rebuild
   }

.. tip::
   FRAMEWORK, ADMINPANEL and KEYGENERATOR aren't plain CMake builds and rely on different toolchains to be built.
   FRAMEWORK is built with cargo, ADMINPANEL with yarn, and after building KEYGENERATOR the encryption key header is created if it is missing.
   Keep them in ``BUILD_ORDER``, the scripts know how to handle them.
