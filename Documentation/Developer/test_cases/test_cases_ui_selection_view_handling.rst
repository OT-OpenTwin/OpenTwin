Frontend Selection and View Handling
####################################

Use the Development project type for the following tests.

.. note::
  Use any python script from the ``TestData/PythonScripts`` folder when a Python script is required for the test case.

  Use any csv file from the ``TestData/Tables`` folder when a csv file is required for the test case.
  Do not use csv files from the subdirectories since some of them are pretty large and might take a long time to visualize.

View handling tests
*******************

.. list-table::
    :header-rows: 1

    * - #
      - Name
      - Setup
      - Execution
      - Expectation
    
    * - 1
      - New View from Selection
      - No entity selection
      - Select a Python script entity
      - * Text view opens
        * Entity remains selected
    
    * - 2
      - Auto close views
      - * Python script entity is selected
        * Corresponding text view is open
      - Switch to another central view (e.g. 3D view)
      - Python script text view closes
    
    * - 3
      - Auto close pinned views 1
      - * Python script entity is selected
        * Corresponding text view is open and pinned
      - Switch to another central view (e.g. 3D view)
      - Python script text view remains open
    
    * - 4
      - Auto close pinned views 2
      - * CSV file entity is selected
        * Corresponding text and table views are open
      - * Pin one of the views (text or table)
        * Switch to another central view (e.g. 3D view)
      - * The pinned view remains open
        * The unpinned view closes
        * The unpinned view remains closed when switching back to the pinned view
    
    * - 5
      - New view position 1
      - * Python script entity is selected
        * Corresponding text view is open and pinned
      - * Dock the python script text view to any other location
        * Select another python script entity
      - * The corresponding text view opens for the newly selected python script entity
        * The newly opened view is tabbed at the same container the previous python script text view
        * The previous python script text view remains open
    
    * - 6
      - New view position 2
      - * Python script entity is selected
        * Corresponding text view is open and pinned
      - * Dock the python script text view to any other location
        * Select another central view (e.g. 3D view)
        * Select another python script entity
      - * The python script text view remains open when switching to the 3D view
        * The corresponding text view opens for the newly selected python script entity
        * The newly opened view is tabbed at the same container as the last selected central view (e.g. 3D view)
        * The previous python script text view remains open

Selection handling tests
************************

.. list-table::
    :header-rows: 1

    * - #
      - Name
      - Setup
      - Execution
      - Expectation
    
    * - 1
      - Remove Selection from View Close
      - * Python script entity is selected
        * Corresponding text view is open
      - Close the text view
      - Entity gets deselected

    * - 2
      - Selection restore single view
      - * Python script entity is selected
        * Corresponding text view is open and pinned
      - Change to another central view (e.g. 3D view) and then back to the Python script text view
      - * Entity gets deselected when switching to the 3D view
        * Python script text view remains open when switching to the 3D view
        * Entity gets selected when switching to the Python script view

    * - 3
      - View change same entity
      - * CSV file entity is selected
        * Corresponding text and table view are open but not pinned
      - Switch from the current view to the other view (by pressing the tab of the view) of the entity (from text to table view and vice versa)
      - * Both views remain open while switching
        * Entity selection remains while switching

    * - 4
      - Selection restore two views
      - * Two csv files are imported
        * One of the two views (text or table) is closed and the other is open and pinned
      - * Select the other csv file entity
        * Close one of the two views (text or table) of the selected entity
        * Pin the remaining view of the selected entity
        * Switch between the views of the first and second csv file entity
      - * When selecting the second csv file entity the pinned view remains open
        * When switching between the views the corresponding entity gets selected
        * When the entity selection changes the closed views do not open

    * - 5
      - Range selection triggers view
      - Import a csv file and make a range selection in the table (by selecting a column in the table view and pressing ``Series Metadata`` in the Import Parameterized Data ToolBar tab)
      - * Close all the views of the entity
        * Select the range entity (Data Categorization/Campaign Metadata/Series Metadata_1/<Name of the Column>)
      - * When selecting the entity only the table view opens
        * Only the range entity is stored as visualizing entity (check by pinning the view and switching back and forth to another central view (e.g. 3D view))
