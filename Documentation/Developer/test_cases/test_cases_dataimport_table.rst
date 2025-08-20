Data Import via Table
=====================

Use the files in MeasurementData_TableImport_Test_1. The needed services are started in Data Processing Projects. The data import via table can use selected rows/columns of a table. In a first step the user need to define what row/column ranges or single cells shall be imported.
The definition can be done by selecting a range or single cell in a table and the using one of the four buttons in the tab "Import Parameterized Data" in the group table selection characterisation. A table selection can be characterised 
as campaign metadata, series metadata, quantity or parameter. The created selections are listed in the folder Data characterisation. Quantity and parameter selections are assigned to a series, which allows the assembly of a dataset. 
Characteristics about the dataset itself can be added as Series Metadata. 
All selections are assigned to every existing Series Metadata entity in the Data characterisation folder  (below Campaign Metadata). 
Selecting a Series Metadata entity and executing the button **Lock Data Characterisation** will lock the Series Metadata and new selection characterisations are not assinged to this Series Metadata.
This way the assembly of a Series Metadata can be concluded. If all Series Metadata are locked, or no Series Metadata entity exists yet, a new Series Metadata entity will be created when a new selection characterisations is created.

Single File Imports
-------------------

Import the files Prüfmatrix_S8_Ausschnitt, S8_306_FA, S8_328_FA, S8_329_FA, S8_330_FA, S8_331_FA, S8_332_FA and S8_333_FA from the folder MeasurementData_TableImport_Test_1.





.. list-table::
   :header-rows: 1
   :widths: 5 15 25 30


   * - #
     - Setup
     - Execution
     - Expectation

   * - 1
     - All mentioned csv files are imported
     - For S8_306_FA and Prüfmatrix_S8_Ausschnitt change the column delimiter to ','
     - | The tables of each file is correctly displayed. The Umlaut 'ü' in Prüfmatrix_S8_Ausschnitt is correctly displayed in the table tab. 
       | Same goes for the column header Prüfnr. in Prüfmatrix_S8_Ausschnitt.

   * - 1.1
     - 
     - Select the Prüfnr. 306 cell in the Prüfmatrix_S8_Ausschnitt and select the Series Metadata button
     - | A new entry below Campaign Metadata appears with the name Series Metadata_1 and one selection entity below it. 
       | The name of the Entity is "Prüfnr.", its "Consider for data import" property is selected and the datatype is int32. The table cell is coloured in the colour of Series Metadata.
          
   * - 1.2
     - 
     - Select the cell BAT-NR and use the button Series Metadata. Do the same for Prüfdatum and Musternr. Select the remaining cells in the row all at once and select the Series Metadata button. 
     - | BAT-NR Prüfdatum and Musternr. have their own selection entities, named accordingly. The multiselection should have a single selection entity with the name "Ziel der Messung (inkl. Modifikation), ...". 
       | The range properties should show correct values (1-based) for the table coordinates. A warning should have been displayed in the output window that "[...] issues were detected [...]" and that a log file was created. 
       | The log file shows that an Empty fields were detected and cells are containing string values. Double check if the row/column values in the log files are accurate.

   * - 1.3
     - | Select the entity Campaign Metadata/Series Metadata_1
       | and the table entity S8_306_FA.
     - Select the header of the first column and use the "Parameter" button
     - | Another selection entity should have appeared below Series Metadata_1/Parameterset with the name Timestamp [us]. 
       | Datatype int32 and in the property group "Update Strategy" the property "Select entire column" should be selected

   * - 1.4
     - 
     - Select the header of the Frequency column and use the Button "Parameter". Do the same for the Amplitude column
     - | Two more selections appeared in Series Metadata_1/Parameterset with names according to the table header. 
       | Frequency has the data type float and Amplitude has the property string (change the setting to double). A warning should be shown in the output window and the logfile about finding strings in the amplitude column.

   * - 1.5
     - 
     - Select the header of the "Delta_acc_1 [BITS]" column and keep the left mouse button pressed. Pull the selection to the right to select all leftover columns at once. Use the Button "Quantity".
     - One more selection should have appeared in Series Metadata_1/Quantityset. The datatype should be int32. 

   * - 1.6
     - 
     - | Make sure that the table entity has the property "Decimal point character" set to **'.'**. 
       | Change the name of Series Metadata_1 into S8_306. Select the button "Create Data Collection".
     - The output window describes that the tables Prüfmatrix_S8_Ausschnitt and S8_306_FA are loaded. 8 selections are considered. A new entity appears in the folder Datasets. The name of the entity is S8_306_FA.

   * - 1.7
     - 
     - Select the Datasets/S8_306_FA entity and select the info button in the model Tab.
     - Three parameter are listed. Timestamps has 378 entries, Frequencies has 362 entries and Amplitude has six. 33 Quantities are listed 

   * - 1.8
     - 
     - | Look with MongoDB Compass into the associated .result database. For that go to the projects database and select the Catalog collection. In there find an entry with the correct project_name. 
       | Take the collection_name of the document and search it in the Projects database. Open the collection with the collection_name and the .result ending.
     - The collection has 47817 documents

   * - 1.9
     - 
     - In OpenTwin, select the "Create Data Collection" button again.
     - Output mentions that there are no selections that are considered.

   * - 2
     - Lock the S8_306 data characterisation entity by selecting it and using the "Lock Data Characterisation"
     - | Perform a data import analog to Test 1.x for S8_328_FA. The selections in the Prüfmatrix_S8_Ausschnitt table shall be analog to the Test 1.x, only with a different row (the row with Prüfnr. 328). 
       | Make sure that the property "Decimal point character" of the S8_328_FA table is set to **','**
     - * The tables of each file is correctly displayed. 
       * The Umlaut 'ü' in Prüfmatrix_S8_Ausschnitt is correctly displayed in the table tab. Same goes for the column header Prüfnr. in Prüfmatrix_S8_Ausschnitt.
       * The table selections of series metadata S8_306 have not changed.
   
   * - 2.1
     - 
     - Select the Datasets/S8_328_FA entity and select the info button in the model Tab.
     - Three parameter are listed. Timestamps has 1276 entries, Frequencies has 452 entries and Amplitude has 413. 33 Quantities are listed 
  
   * - 2.2
     - 
     - | Look with MongoDB Compass into the associated .result database. For that go to the projects database and select the Catalog collection. In there find an entry with the correct project_name. 
       | Take the collection_name of the document and search it in the Projects database. Open the collection with the collection_name and the .result ending.
     - The collection has 140745 documents

   * - 3
     - Create a new dataprocessing project.
     - | Perform a data import analog to Test 1.x for S8_329_FA. The selections in the Prüfmatrix_S8_Ausschnitt table shall be analog to the Test 1.x, only with a different row (the row with Prüfnr. 329)
       | Make sure that the property "Decimal point character" of the S8_329_FA table is set to **','**
     - The tables of each file is correctly displayed. The Umlaut 'ü' in Prüfmatrix_S8_Ausschnitt is correctly displayed in the table tab. Same goes for the column header Prüfnr. in Prüfmatrix_S8_Ausschnitt.
   
   * - 3.1
     - 
     - Select the Datasets/S8_329_FA entity and select the info button in the model Tab.
     - Three parameter are listed. Timestamps has 726 entries, Frequencies has 362 entries and Amplitude has 318. 33 Quantities are listed 
  
   * - 3.2
     - 
     - | Look with MongoDB Compass into the associated .result database. For that go to the projects database and select the Catalog collection. In there find an entry with the correct project_name. 
       | Take the collection_name of the document and search it in the Projects database. Open the collection with the collection_name and the .result ending.
     - The collection has 95535 documents

   * - 4
     - Create a new dataprocessing project.
     - | Perform a data import analog to Test 1.x for S8_330_FA. The selections in the Prüfmatrix_S8_Ausschnitt table shall be analog to the Test 1.x, only with a different row (the row with Prüfnr. 330). 
       | Make sure that the property "Decimal point character" of the S8_330_FA table is set to **','**
     - The tables of each file is correctly displayed. The Umlaut 'ü' in Prüfmatrix_S8_Ausschnitt is correctly displayed in the table tab. Same goes for the column header Prüfnr. in Prüfmatrix_S8_Ausschnitt.
   
   * - 4.1
     - 
     - Select the Datasets/S8_330_FA entity and select the info button in the model Tab.
     - Three parameter are listed. Timestamps has 522 entries, Frequencies has 114 entries and Amplitude has 204. 33 Quantities are listed 
  
   * - 4.2
     - 
     - | Look with MongoDB Compass into the associated .result database. For that go to the projects database and select the Catalog collection. In there find an entry with the correct project_name. 
       | Take the collection_name of the document and search it in the Projects database. Open the collection with the collection_name and the .result ending.
     - The collection has 52173 documents

   * - 5
     - Create a new dataprocessing project.
     - | Perform a data import analog to Test 1.x for S8_331_FA. The selections in the Prüfmatrix_S8_Ausschnitt table shall be analog to the Test 1.x, only with a different row (the row with Prüfnr. 331). 
       | Make sure that the property "Decimal point character" of the S8_331_FA table is set to **','**
     - The tables of each file is correctly displayed. The Umlaut 'ü' in Prüfmatrix_S8_Ausschnitt is correctly displayed in the table tab. Same goes for the column header Prüfnr. in Prüfmatrix_S8_Ausschnitt.
   
   * - 5.1
     - 
     - Select the Datasets/S8_331_FA entity and select the info button in the model Tab.
     - Three parameter are listed. Timestamps has 2085 entries, Frequencies has 452 entries and Amplitude has 682. 33 Quantities are listed 
  
   * - 5.2
     - 
     - | Look with MongoDB Compass into the associated .result database. For that go to the projects database and select the Catalog collection. In there find an entry with the correct project_name. 
       | Take the collection_name of the document and search it in the Projects database. Open the collection with the collection_name and the .result ending.
     - The collection has 191301 documents

   * - 6
     - Create a new dataprocessing project.
     - | Perform a data import analog to Test 1.x for S8_332_FA. The selections in the Prüfmatrix_S8_Ausschnitt table shall be analog to the Test 1.x, only with a different row (the row with Prüfnr. 332). 
       | Make sure that the property "Decimal point character" of the S8_332_FA table is set to **','**
     - The tables of each file is correctly displayed. The Umlaut 'ü' in Prüfmatrix_S8_Ausschnitt is correctly displayed in the table tab. Same goes for the column header Prüfnr. in Prüfmatrix_S8_Ausschnitt.
   
   * - 6.1
     - 
     - Select the Datasets/S8_332_FA entity and select the info button in the model Tab.
     - Three parameter are listed. Timestamps has 416 entries, Frequencies has 452 entries and Amplitude has 405. 33 Quantities are listed 
  
   * - 6.2
     - 
     - | Look with MongoDB Compass into the associated .result database. For that go to the projects database and select the Catalog collection. In there find an entry with the correct project_name. 
       | Take the collection_name of the document and search it in the Projects database. Open the collection with the collection_name and the .result ending.
     - The collection has 59829 documents

   * - 7
     - Create a new dataprocessing project.
     - | Perform a data import analog to Test 1.x for S8_333_FA. The selections in the Prüfmatrix_S8_Ausschnitt table shall be analog to the Test 1.x, only with a different row (the row with Prüfnr. 333). 
       | Make sure that the property "Decimal point character" of the S8_333_FA table is set to **','**
     - The tables of each file is correctly displayed. The Umlaut 'ü' in Prüfmatrix_S8_Ausschnitt is correctly displayed in the table tab. Same goes for the column header Prüfnr. in Prüfmatrix_S8_Ausschnitt.
   
   * - 7.1
     - 
     - Select the Datasets/S8_333_FA entity and select the info button in the model Tab.
     - Three parameter are listed. Timestamps has 364 entries, Frequencies has 362 entries and Amplitude has 327. 33 Quantities are listed 
  
   * - 7.2
     - 
     - 
     - | All quentities have the unit BITS and the quantity names should be:  Delta_acc_1, Delta_acc_2, Delta_acc_3, Delta_CLK_freq, MONIT_Error_1, MONIT_Error_2, SID_Error_1, SID_Error_2, SID_Error_3, 
       | ECU-Stat_Error_1, Delta_ACC_2_MA10, Delta_ACC_3_MA10, Delta_STDD_ACC_1, Delta_STDD_ACC_2, Delta_STDD_ACC_3, Delta_ACC_51HZ_1, Delta_ACC_51HZ_2, Delta_ACC_51HZ_3, Delta_STDD_AC_51_1, Delta_STDD_AC_51_2, 
       | Delta_STDD_AC_51_3, Delta_OFFS_ACC_1, Delta_OFFS_ACC_2, Delta_OFFS_ACC_3, Delta_MA100_51AC_1, Delta_MA100_51AC_2, Delta_MA100_51AC_3, Delta_MA1200_51_1, Delta_MA1200_51_2, Delta_MA1200_51_3, 
       | Delta_ACC_3_MA100, Delta_ACC_3_MA1200, Delta_ACC_1_MA10

   * - 7.3
     - 
     - 
     - The parameters have the following units: Frequency = MHz, Timestamp = uS, Amplitude = mA,V/m
  
   * - 7.4
     - 
     - | Look with MongoDB Compass into the associated .result database. For that go to the projects database and select the Catalog collection. In there find an entry with the correct project_name. 
       | Take the collection_name of the document and search it in the Projects database. Open the collection with the collection_name and the .result ending.
     - The collection has 47883 documents


File Batch Import
-----------------

Import the files Prüfmatrix_S8_Ausschnitt, S8_306_FA, S8_328_FA, S8_329_FA, S8_330_FA, S8_331_FA, S8_332_FA and S8_333_FA from the folder MeasurementData_TableImport_Test_1.


.. list-table::
   :header-rows: 1
   :widths: 5 25 25 30

   * - #
     - Setup
     - Execution
     - Expectation
   

   * - 1
     - * Create a new data processing project. 
       * All mentioned csv files are imported.
       * Import the python scripts UpdateScriptNextRow.py and UpdateScriptNextTable.py
       * Check that all tables have the correct column delimiters and decimal delimiter set.
     - * Create selections as described in the tests 1.x. But use the 328 table instead of the 306 table. 
       * Perform a multiselection of all Quantity and Parameter selections in the S8_328_FA table.
     - The shared properties of the selection entities are shown.

   * - 1.1
     - 
     - Follow these steps:
         *   Select the property "Consider for batching". Select the UpdateScriptNextTable.py script as Update Script and select "Pass on script".
         *   Select all Measurement Series selections and set the properties "Consider for batching" and "Pass on script". Select the UpdateScriptNextRow.py as Update script
         *   In the Prüfnr. selection set the property "Execution priority" to a value > 0
         *   In the "Import Parameterized Data" tab, select the "Add Batch Importer" button. Set the "Repetitions" property of the Batch Importer entity (in the Data characterisation folder) to 5.
         *   Set the "Name base" property to "Series Metadata"
         *   Create a copy of the OpenTwin project and rename it to "BatchImport_Root"
         *   Select the Importer entity and execute the "Auto Create Series Metadata" button.
     - The data of the tables are imported after another. A report file is added below the "Batch Importer" entity. In the Dataset folder, six entities were added with the name base "Batch_Import" and a number behind.

   * - 1.2
     - 
     - Check the infos of the created  "Batch_Import" entities in the Dataset folder.
     - The property characteristics should be as they were described in the tests 1 - 7 described. The result collection should hold 587466 documents in total.

   * - 1.3
     - 
     - Select the importer and execute the "Auto Create Series Metadata" button.
     - The importer tries to run 5 times again, starts the first run but interupts then with the output window saying\: "Aborted batch import due to error: No selection is considered for batching"


Touchstone File Import
----------------------

.. list-table::
   :header-rows: 1
   :widths: 5 25 25 30

   * - #
     - Setup
     - Execution
     - Expectation
   

   * - 1
     - Create a new dataprocessing project.
     - Import the CMC_Stysch.s4p file via the "Import Touchstone" button.
     - After selecting the button, a window asks if the file realy has 4 ports. After selecting ok, a entity appears in the Dataset folder, called CMC_Stysch.


   * - 1.1
     - 
     - Select the Dataset/CMC_Stysch entity and select the info button in the model tab.
     - | One quantity "S-Parameter" is listed. The quantitty has a dimension 4,4. It has two value descriptions: "Magnitude" which has no unit and "Phase" which has the unit "Deg". 
       | Both value descriptions are of type double. A single parameter "Frequency" is listed with 1001 values and the unit "Hz". The associated .result collection has 2002 documents.