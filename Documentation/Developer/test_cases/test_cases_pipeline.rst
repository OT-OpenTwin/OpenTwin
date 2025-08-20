Pipeline Testing Specification
==============================

.. list-table::
   :header-rows: 1

   * - #
     - Setup
     - Execution
     - Expectation

   * - 1
     - * Create a Python block
       * Import the **CalculateNodalZParameter** script using the Import Python Script button in the Model tab.
     - Select the Python script in the Python block property.
     - * Block adapts with an input and an output port
       * The property **"Reference Impedence"** has the value **50**.

   * - 2.1
     - * Import **"CMC_Stysch.s4p"** in the Import Parameterized Data tab using the Import Touchstone button.
       * Select **4** ports in the dialog field. 
       * Create a DataBaseAccess (DBA) block and a File Writer block. 
       * Connect the output from the DBA to the input from the File Writer.
       * **DBA block properties:**
          * Select the project name of the currently open project in the **Projectname** property.
          * Select the QuantitySettings.Name property and set it to S-Parameters.
     - Run the pipeline 
     - * The Output window displays the details about the execution of the DBA and the File Writer block.
       * One entry in the output describes that the query returned **1001** results. 
       * A text file was added to the navigation tree: **Text Files/File Writer**
       * The first entry in the text file should be:
       
       .. code-block:: json

        {
          "S-Parameter": [
            0.00664799160739456, 0.9994438164025367, 0.001587903081894653, 0.001607058983526277,
            0.9999412687092339, 0.005526171247751565, 0.001579206287437996, 0.00157581398427815,
            0.001649662785870558, 0.0016798080739750988, 0.002447593891518981, 0.9990050610098995,
            0.001636659755401767, 0.001608587428865396, 1.001026028272019, 0.0033346144200217056
            ],
          "Frequency": 9000.0
        }

   * - 2.2
     - Complete test **2.1**.
     - * At the **DBA block properties** change the type from Magnitude to Phase in the Quantity settings.
       * At the **File Writer block properties** set the File Name property to "Phase".
       * Run the pipeline.
     - * A text file has been added to the navigation tree: **Text Files/Phase**.
       * The first entry in the text file should be:
       
       .. code-block:: json

          {
            "S-Parameter": [
              -152.0392389462725, 0.5254550519420145, -86.97492714262474, 92.88932342473128,
              0.1719524747673975, -128.16353522192419, 92.23249838369779, -86.45911549500751,
              -90.13597479937043, 92.38877667597682, -145.86418201076899, 0.41514551480424968,
              90.87826723417756, -88.89680605266958, 0.2217375858356425, -142.6517284701131
              ],
              "Frequency":9000.0
          }

   * - 3.1
     - Complete test **2.1**.
     - * At the **DBA block properties** select the comparator **>** (bigger than) and set value 1.
       * Run the pipeline.
     - * A text file has been added to the navigation tree: **Text Files/File Writer_1**.
       * The output window shows that the query returned **4** results.
       * The Phase_1 document contains **4** entries (4 S-parameter entries and 4 frequency entries).

   * - 3.2
     - Complete test **3.1**.
     - * Replace the File Writer block with a Store in Database block.
       * Rename the **Blocks/Data Processing/Store in Database** entity to **Magnitude > 1**.
       * Run the pipeline.
     - A new entity named "**Magitude > 1**" appears in the Dataset folder.

   * - 3.3
     - Complete test **3.2**.
     - * Replace the Store in Database block with a Display block.
       * Remove the comperator (set to the empty comperator) property from the **DBA block properties**
       * Select **Dataset/Magnitude > 1** as the Measurment series at the **DBA block properties**.
       * Run the pipeline.
     - * The Output window shows that the query returned **4** results.
       * Each entry consists of a matrix named S-Parameter and a field named Frequency.
       * To view the complete metadata, please click :ref:`here <result_pipeline_test_displayed_metadata_1>`.
       
   * - 3.4
     - Complete test **3.3**.
     - Select the **Magnitude > 1** entity and click the Info button in the Model tab.
     - Output of metadata as in test 3.3.

   * - 4.1
     - * Create a new developer project and name it **DataSource**.
       * In the Debug tab, click the **Single Curve** button.
       * Create/open a data processing project.
       * Create a DBA block, a Python block, and two Display blocks.
       * Import the **BlockExecution_AddOffset.py** script.
     - * **Python block properties:**
            * Select the **BlockExecution_AddOffset** script in the Scripts property.
            * Set the Offset property to **10**.
       * Connect the DBA block to one of the Display blocks and the input of the Python block.
       * Connect the output of the Python block to the second Display block.
       * **DBA block properties:**
            * select the **“DataSource”** project as the project name and **Magnitude** as the quantity.
            * No comparator should be selected. 
       * Run the pipeline.
     - * The data output from the Display block connected to the Python block should start with a magnitude value of **10**.
       * The data output from the Display block connected to the DBA block should start with a magnitude value of **0**.

   * - 4.2
     - * Complete test **4.1**.
       * Import the Python script **BlockExecution_ChangeParameterFrequencyUnit.py**.
     - * Select the **BlockExecution_ChangeParameterFrequencyUnit** script in the **Python block properties**.
       * Run the pipeline.
     - In the metadata, the unit **Hz** should be specified under **Parameter/Frequency**.

   * - 4.3
     -  Complete test **4.2**.
     - * Replace the DisplayBlock with a **Store in Database** block. 
       * Rename the **Store in Database** into "InHerz", set the property "Create plot" and set the "Plot name" property to "New dataset".
       * Run the pipeline. 
     - * In the folder Datasets, a new entry named "InHerz" appears.
       * In the plot folder a new plot, named "New dataset" appears. The plot has the parameter Frequency[Hz] on the x-axis.

   * - 5
     - The **DataSource** project exists with the dataset **Single Curve** in it. The **BlockExecution_AddOffset.py** script is imported.
     - * Create one database access block (DBA), two store to database (STD) blocks and one python block.
       * Connect one STD block directly with the DBA. Rename this store to database block to "Without Offset"
       * Select the **BlockExecution_AddOffset.py** in the python block.
       * Connect the DBA with the python block and the python block with the second STD block. Change the name of the STD block to "With offset".
       * Set the DBA block project property to **DataSource** and the quantity to Magnitude.
       * Execute the pipeline.
     - * Pipeline executes successfully.
       * In the folder Datasets, one new entry named "With offset" and one new entry named "Without offset" appear.
   
   * - 5.1
     - Complete test **5**.
     - * Select the 
       * Change the "line data["Magnitude"] = int(data["Magnitude"]) + offset" to "line data["Magnitude"] = float(data["Magnitude"]) + offset"
       * Safe the changes of the textfile (strg+s)
       * Execute the pipeline
     - * Pipeline execution fails.
       * Output window says " The data type of: Magnitude is inconsistent with the data type mentioned in its metadata."

   * - 5.2
     - Complete test **5.1**.
     - * Open the **BlockExecution_AddOffset.py** 
       * Change the "line data["Magnitude"] = int(data["Magnitude"]) + offset" to "line data["Magnitude"] = float(data["Magnitude"]) + offset"
       * Safe the changes of the textfile (strg+s)
       * Execute the pipeline
     - * Pipeline execution fails.
       * Output window says " The data type of: Magnitude is inconsistent with the data type mentioned in its metadata."

   * - 6
     - * Import **CMC_Stysch_simple.s4p** (**4** ports).
       * Create a DataBaseAccess block and a Display block.
       * Set up the DBA block (project name, quantity (S-parameter)).
       * Connect the DBA output to Display.Input.
     - * Set the DBA property "Number of queries" to 1
       * In the property group "Query settings" select the name "Frequency"
       * Test various comparators and values for parameter 1 (Results listed at Expectation). 
     - .. list-table:: 
          :header-rows: 1
          :widths: 15 15 70
          
          * - Comparator
            - Value
            - Expected Results
          * - <
            - 9.5
            - 6 Results (9; 9.1; 9.2; 9.3; 9.4; 1)
          * - <=
            - 9.5
            - 7 Results (9; 9.1; 9.2; 9.3; 9.4; 9.5; 1)
          * - =
            - 9.5
            - 1 Result (9.5)
          * - >=
            - 9.5
            - 5 Results (9.5; 9.6; 9.7; 9.8; 9.9)
          * - >
            - 9.5
            - 4 Results (9.6; 9.7; 9.8; 9.9)
          * - Any of
            - 9.5,9.6
            - 2 Results (9.5; 9.6)
          * - Not any of
            - 9.5,9.6
            - 9 Results (9; 9.1; 9.2; 9.3; 9.4; 9.7; 9.8; 9.9; 1)
          * - range
            - [9.5, 9.7]
            - 3 Results (9.5; 9.6; 9.7)
          * - range
            - [9.5, 9.7)
            - 2 Results (9.5; 9.6)
          * - range
            - (9.5, 9.7]
            - 2 Results (9.6; 9.7)
          * - range
            - (9.5, 9.7)
            - 1 Result (9.6)
