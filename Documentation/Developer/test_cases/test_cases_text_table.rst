Text and Table Test Cases
#########################

Use the Development project type for the following tests.

Test Cases
**********

.. list-table::
    :header-rows: 1

    * - #
      - Name
      - Setup
      - Execution
      - Expectation

    * - 1
      - Open View
      - Import ``Prüfmatrix_S8_Ausschnitt.csv`` from ``Testdata/Tables/Measurement_TableImport_Test_1``
      - Select the entity in the navigation tree
      - * Text view opens
        * Table view opens
        * Entity remains selected

    * - 2
      - Table view correctness
      - Views from ``Test #1`` are open (perform Test #1)
      - * Set the decimal point character to a point (.)
        * Set the column delimiter to a comma (,)
      - * The table has a correctly looking horizontal header (Prüfnr.; BAT-NR; ...)
        * The table has 8 rows with information
        * The table has 18 columns

    * - 3
      - Close Text View
      - * Views from ``Test #1`` are open (perform Test #1)
        * Text view is focused
      - Close Text view
      - * Table view remains open
        * Entity remains selected

    * - 4
      - Close Table View
      - * Views from ``Test #1`` are open (perform Test #1)
        * Table view is focused
      - Close Table view
      - * Text view remains open
        * Entity remains selected

    * - 5
      - Close both Views
      - Views from ``Test #1`` are open (perform Test #1)
      - * Close Table view
        * Close Text view
      - Entity gets deselected

    * - 6
      - Switch to Text View
      - Views from ``Test #1`` are open (perform Test #1)
      - * Pin text view
        * Switch to 3D view
        * Switch back to text view
      - * Table view closes when switching to 3D view
        * Entity gets deselected when switching to 3D view
        * Text view remains open when switching to 3D view
        * Entity gets selected when switching back to text view
        * Table view remains closed when switching back to text view

    * - 7
      - Switch to Table View
      - Views from ``Test #1`` are open (perform Test #1)
      - * Pin table view
        * Switch to 3D view
        * Switch back to table view
      - * Text view closes when switching to 3D view
        * Entity gets deselected when switching to 3D view
        * Table view remains open when switching to 3D view
        * Entity gets selected when switching back to table view
        * Text view remains closed when switching back to table view

    * - 8
      - Re-open Text View
      - Views from ``Test #1`` are open (perform Test #1)
      - * Close text view
        * Deselect entity by left-clicking the navigation tree (not on a entity)
        * Select the entity again
      - * Text view opens again when re-selecting the entity
        * Table view remains open

    * - 9
      - Re-open Table View
      - Views from ``Test #1`` are open (perform Test #1)
      - * Close table view
        * Deselect entity by left-clicking the navigation tree (not on a entity)
        * Select the entity again
      - * Table view opens again when re-selecting the entity
        * Text view remains open

    * - 10
      - View close on entity delete
      - Views from ``Test #1`` are open (perform Test #1)
      - Delete the entity (Press ``DEL`` while the navigation tree has focus or press the delete button in the ToolBar Model page)
      - Both views are closed

    * - 11
      - Update Text from Table
      - Views from ``Test #1`` are open (perform Test #1)
      - * Modify any cell in the Table
        * Apply the changes by either pressing ``CTRL+S`` or pressing the save button in the Tables' modal ToolBar page
      - Text view gets updated with the new changes

    * - 12
      - Update Table from Text
      - Views from ``Test #1`` are open (perform Test #1)
      - * Modify the text in the Table (changes must follow the csv file syntax)
        * Apply the changes by either pressing ``CTRL+S`` or pressing the save button in the Texts' modal ToolBar page
      - Table view gets updated with the new changes

    * - 13
      - Create Data Categorization
      - Views from ``Test #1`` are open (perform Test #1)
      - * Select any column in the table
        * Press the ``Series Metadata`` button in the Import Parameterized Data ToolBar page
      - A range entity is created in the navigations' Data Categorization folder (Data Categorization/Campaign Metadata/Series Metadata_1/<Name of the Column>)

    * - 14
      - Open Table from Range entity
      - Perform ``Test #13``
      - * Close all views
        * Select the range entity (Data Categorization/Campaign Metadata/Series Metadata_1/<Name of the Column>)
      - * Table view opens
        * Range entity remains selected

    * - 15
      - Import Time
      - * No views are open
        * No entity is selected
        * Logging mode is set to Warning and Error logs only
      - * Import ``S9_23_MW.csv`` from ``Testdata/Tables/Measurement_TableImport_Test_2``
        * Select entity
      - * Import takes ~3 seconds
        * Visualization takes ~10 seconds

    * - 16
      - Table view correctness 2
      - Import ``Testfile_Tab_WLB.csv`` from ``Testdata/Tables``
      - * Change the column delimiter to a tabulator by entering ``\t``
        * Change the row delimiter to a carriage return + new line by entering ``\r\n``
      - * Table has a correct horizontal header
        * Table has 3 rows and 3 columns

    * - 17
      - Table view correctness 3
      - Import ``Testfile_Tab_WLB_Masked.csv`` from ``Testdata/Tables``
      - * Change the column delimiter to a semicolon (;)
        * Change the row delimiter to a carriage return by entering ``\r``
      - * Table has a correct horizontal header
        * Table has 3 rows and 3 columns

    * - 18
      - Table view correctness 4
      - Import ``Testfile_Comma_LB.csv`` from ``Testdata/Tables``
      - * Change the column delimiter to a comma (,)
        * Change the row delimiter to a new line by entering ``\n``
      - * Table has a correct horizontal header (First column; Second Column; ThirdColumn)
        * Table has 3 rows and 3 columns
    
    * - 19
      - Table view correctness 5
      - Import ``Testfile_Comma_LB.csv`` from ``Testdata/Tables``
      - * Change the column delimiter to a comma (,)
        * Change the row delimiter to a new line by entering ``\n``
        * Set the header position to "first column"
      - * Table has a horizontal header (1; 2)
        * Table has a row header (First column; 11; 21; 31)
        * Table has 4 rows and 2 columns
    
    * - 20
      - Table view correctness 6
      - Import ``Testfile_Comma_LB.csv`` from ``Testdata/Tables``
      - * Change the column delimiter to a comma (,)
        * Change the row delimiter to a new line by entering ``\n``
        * Set the header position to "none"
      - * Table has a horizontal header (1; 2; 3)
        * Table has a row header (1; 2; 3; 4)
        * Table has 4 rows and 3 columns

Load Test
---------
Setup:
   * Create an empty data pipeline project

Execution:
   * First, import all files from the directory LoadTest_1 (9996 files)
   * Afterwards, import all files from the directory LoadTest_2 (9996 files)

Expectation:
   * Each import shows a progress bar. In the output window, three timingds are shown: Import of x file(s), Storing document(s) and Model update. The timings should be approximately the same for both import operations (LoadTest_1 and LoadTest_2).