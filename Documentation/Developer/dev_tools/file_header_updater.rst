File Header Updater
###################

The **File Header Updater (FHU)** tool is used to update the file headers of all OpenTwin source files.

Running the Tool
****************

The File Header Updater can be executed conveniently using the provided **batch file**:

**Location:** ``Scripts/Other/UpdateFileHeaders.bat``

This batch file serves as a launcher for the tool and handles the mandatory configuration automatically.

How It Works
=============

- The batch file **forwards up to nine command line arguments** directly to the File Header Updater tool.  
  This means all standard arguments (e.g. ``--dry``, ``--preview``) can be passed to the batch file when calling it.

- It automatically specifies the following paths:
  
  - **Configuration file:** ``%OPENTWIN_DEV_ROOT%/Tools/FileHeaderUpdater/OT_FHU_Config.json``  
    - Used to define all file header update settings.
  
  - **Output log file:** ``%OPENTWIN_DEV_ROOT%/Tools/FileHeaderUpdater/FHU_Log.txt``  
    - Contains all tool output and summary information.

Special Option: ``--pause``
===========================

If the **first argument** passed to the batch file is ``--pause``, the script will **pause after the tool finishes**.  
This allows to review the console output before the terminal window closes automatically.

Example Usage
=============

.. code-block:: batch

    :: Run the File Header Updater normally
    Scripts\Other\UpdateFileHeaders.bat

    :: Run with dry mode enabled (no files are changed)
    Scripts\Other\UpdateFileHeaders.bat --dry

    :: Run and preview the modified headers without applying changes
    Scripts\Other\UpdateFileHeaders.bat --preview

    :: Run and keep the console window open to review output of changed files
    Scripts\Other\UpdateFileHeaders.bat --pause --notifychangeonly

Command Line Arguments
**********************

.. note::
    Path variables may contain environment variables using the ``%EnvironmentVariableName%`` syntax.

.. list-table::
    :header-rows: 1

    *   - **Argument**
        - **Description**
    *   - ``--config <file path>``
        - Specifies the path to the configuration file.
    *   - ``--out <file path>``
        - [**Optional**] Specifies the output report file path.  
          If not specified, the output is shown on standard output (e.g., the terminal) only.
    *   - ``--dry``
        - [**Optional**] Performs a dry run without modifying any files.  
    *   - ``--preview``
        - [**Optional**] Shows the modified file data without writing changes to disk.  
          Automatically enables ``--dry``.  
    *   - ``--help``, ``-h``
        - [**Optional**] Displays the help message.  
    *   - ``--showconfig``
        - [**Optional**] Displays the currently imported configuration.  
    *   - ``--notifychangeonly``
        - [**Optional**] The output messages only notifies about changed files.  
          Ignored or empty files are skipped in the output.

Configuration File
******************

.. note::
    Path values may contain environment variables using the ``%EnvironmentVariableName%`` syntax.

The configuration file is a **JSON file** with the following members:

.. list-table::
    :header-rows: 1

    *   - **Key**
        - **Type**
        - **Description**

    *   - ``LicenseFile``
        - *String (Path)*
        - Path to the license file.

    *   - ``Keyword.Start``
        - *String*
        - Start keyword indicating the beginning of the license header.

    *   - ``Keyword.Ignore``
        - *String*
        - Keyword indicating that the file should be ignored.

    *   - ``Keyword.End``
        - *String*
        - End keyword indicating the end of the license header.

    *   - ``RootDirectories``
        - *Array of Strings*
        - List of root directories to scan.

    *   - ``FileExtensions``
        - *Array of Strings*
        - List of file extensions to consider.

Optional Members
****************

The following members are **optional**:

.. list-table::
    :header-rows: 1

    *   - **Key**
        - **Type**
        - **Description**

    *   - ``AddFileName``
        - *Boolean*
        - If ``true``, adds a file name line to the license header.

    *   - ``AddLicenseTitle``
        - *Boolean*
        - If ``true``, adds the ``License`` title line to the license header.

    *   - ``HeaderLinePrefix``
        - *String*
        - Prefix to add to each line of the license (e.g., comment characters).

    *   - ``WarnMissingHeader``
        - *Boolean*
        - If ``true``, warns when a file is missing a header.

    *   - ``WarnInvalidChars``
        - *Boolean*
        - If ``true``, warns when a file contains non-ASCII characters.

    *   - ``BlacklistDirectories``
        - *Array of Strings*
        - List of directories to exclude from scanning.