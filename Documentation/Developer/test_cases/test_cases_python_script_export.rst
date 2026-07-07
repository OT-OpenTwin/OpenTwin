Python Script Export Test Cases
===============================

Notes
-----

| Scope: Export behavior for Python Script and Environment entities.
| If the environment changes (overwrite or create new), the script dependency ID must always be updated to the resulting environment.


Export Combination Tests
------------------------

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
