Python Script Export Test Cases
===============================

Notes
-----

| Scope: User export behavior for Python Script and Environment entities directly into the database.
| If the environment changes (overwrite or create new), the script dependency ID must always be updated to the resulting environment.


User Export to Database Tests
-----------------------------

.. list-table::
   :header-rows: 1
   :widths: 5 30 20 45

   * - #
     - Setup
     - User Decision
     - Expectation

   * - 1
     - | Script: not existing
       | Environment: not existing
     - No prompt expected
     - | Create new script in DB.
       | Create new environment in DB.

   * - 2
     - | Script: not existing
       | Environment: existing with identical content
     - No prompt expected
     - | Create new script in DB with dependency ID pointing to the existing environment.
       | Skip environment creation/update.

   * - 3
     - | Script: not existing
       | Environment: existing with different content
     - Prompt for environment overwrite
     - | Yes: Create new script with dependency ID pointing to the updated environment. Overwrite environment with same ID, increase version, keep old version in DB.
       | No: Create new script with dependency ID pointing to the new environment. Create a new environment with unique name/ID.

   * - 4
     - | Script: existing with identical content
       | Environment: not existing
     - No prompt expected
     - | Environment is created.
       | Script is stored as new version with same logical ID and updated dependency to the new environment.

   * - 5
     - | Script: existing with identical content
       | Environment: existing with identical content
     - No prompt expected
     - | Environment is skipped.
       | Script is stored as new version with same logical ID and updated dependency ID.

   * - 6
     - | Script: existing with identical content
       | Environment: existing with different content
     - Prompt for environment overwrite
     - | Yes: Keep script content, overwrite environment (same ID, version++), keep old environment version in DB.
       | No: Keep script content, create new environment with unique name/ID.
       | In both branches, update script dependency ID to the resulting environment.

   * - 7
     - | Script: existing with different content
       | Environment: not existing
     - Prompt for script overwrite
     - | Yes: Overwrite script (same ID, version++, old version remains), create environment.
       | No: Create script with unique name/ID, create environment.
       | In both branches, resulting script must depend on the created environment.

   * - 8
     - | Script: existing with different content
       | Environment: existing with identical content
     - Prompt for script overwrite
     - | Yes: Overwrite script, skip environment.
       | No: Create script with unique name/ID, skip environment.
       | In both branches, resulting script must depend on the existing environment.

   * - 9
     - | Script: existing with different content
       | Environment: existing with different content
     - Prompt sequence: script first, then environment
     - | Yes/Yes: Overwrite both. Keep IDs, increase versions, old versions remain in DB.
       | Yes/No: Overwrite script. Create new environment with unique name/ID.
       | No/Yes: Create new script with unique name/ID. Overwrite environment.
       | No/No: Create new script and new environment with unique name/ID.
       | In every branch where the environment result differs, update script dependency ID accordingly.


Local Export to Filesystem Tests
--------------------------------

Notes
-----

| Scope: Local export behavior for Python Script and Environment entities to the filesystem (``LibraryData/``).
| The environment (manifest) is always processed **before** the script, because the script's ``DependencyID`` must point to the environment's final ``LibraryElementID``.
| If the environment needs a prompt, the script export is deferred until the environment prompt is answered.
| "Existing" means the file already exists on disk from a previous export.
| Content comparison excludes dynamic parameters (``LibraryElementID``, ``Version``, ``Name``, ``FileName``, ``DependencyID``, ``DependencyCollection``).


.. list-table::
   :header-rows: 1
   :widths: 5 30 20 45

   * - #
     - Setup
     - User Decision
     - Expectation

   * - 1
     - | Script: not existing
       | Environment: not existing
     - No prompt expected
     - | Create environment file with ``LibraryElementID`` and ``Version = 1``.
       | Create script file with ``LibraryElementID``, ``Version = 1``, and ``DependencyID`` pointing to the environment's ``LibraryElementID``.

   * - 2
     - | Script: not existing
       | Environment: existing with identical content
     - No prompt expected
     - | Skip environment (no changes).
       | Create script file with ``DependencyID`` pointing to the existing environment's ``LibraryElementID`` (read from existing meta file).

   * - 3
     - | Script: not existing
       | Environment: existing with different content
     - Prompt for environment overwrite
     - | **Yes:** Overwrite environment file (same ``LibraryElementID``, ``Version++``). Create script file with ``DependencyID`` pointing to the environment's (unchanged) ``LibraryElementID``.
       | **No:** Create new environment file with incremented name (e.g. ``_1``) and a **new** ``LibraryElementID``, ``Version = 1``. Create script file with ``DependencyID`` pointing to the environment's **new** ``LibraryElementID``.

   * - 4
     - | Script: existing with identical content
       | Environment: not existing
     - No prompt expected
     - | Create environment file with ``LibraryElementID`` and ``Version = 1``.
       | Skip script export (content identical).

   * - 5
     - | Script: existing with identical content
       | Environment: existing with identical content
     - No prompt expected
     - | Skip environment (no changes).
       | Skip script (no changes).
       | ``DependencyID`` in existing script meta remains unchanged (already correct from previous export).

   * - 6
     - | Script: existing with identical content
       | Environment: existing with different content
     - Prompt for environment overwrite
     - | **Yes:** Overwrite environment (same ``LibraryElementID``, ``Version++``). Update script meta file's ``DependencyID`` to the environment's (unchanged) ``LibraryElementID``.
       | **No:** Create new environment file with incremented name and **new** ``LibraryElementID``, ``Version = 1``. Update script meta file's ``DependencyID`` to the environment's **new** ``LibraryElementID``.

   * - 7
     - | Script: existing with different content
       | Environment: not existing
     - Prompt for script overwrite
     - | **Yes:** Create environment file. Overwrite script (same ``LibraryElementID``, ``Version++``). Script ``DependencyID`` points to environment's ``LibraryElementID``.
       | **No:** Create environment file. Create new script file with incremented name, **new** ``LibraryElementID``, ``Version = 1``. Script ``DependencyID`` points to environment's ``LibraryElementID``.

   * - 8
     - | Script: existing with different content
       | Environment: existing with identical content
     - Prompt for script overwrite
     - | **Yes:** Skip environment. Overwrite script (same ``LibraryElementID``, ``Version++``). Script ``DependencyID`` points to existing environment's ``LibraryElementID``.
       | **No:** Skip environment. Create new script file with incremented name, **new** ``LibraryElementID``, ``Version = 1``. Script ``DependencyID`` points to existing environment's ``LibraryElementID``.

   * - 9
     - | Script: existing with different content
       | Environment: existing with different content
     - | Prompt sequence:
       | environment first, then script
     - | **Yes/Yes:** Overwrite environment (same ID, ``Version++``). Overwrite script (same ID, ``Version++``). Script ``DependencyID`` points to the environment's (unchanged) ``LibraryElementID``.
       | **Yes/No:** Overwrite environment (same ID, ``Version++``). Create new script with incremented name, **new** ``LibraryElementID``. Script ``DependencyID`` points to the environment's (unchanged) ``LibraryElementID``.
       | **No/Yes:** Create new environment with incremented name, **new** ``LibraryElementID``. Overwrite script (same ID, ``Version++``). Script ``DependencyID`` updated to point to the environment's **new** ``LibraryElementID``.
       | **No/No:** Create new environment with incremented name, **new** ``LibraryElementID``. Create new script with incremented name, **new** ``LibraryElementID``. Script ``DependencyID`` points to the environment's **new** ``LibraryElementID``.

