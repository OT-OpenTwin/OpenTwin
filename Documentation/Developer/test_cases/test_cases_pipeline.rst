Pipeline Testing Specification
==============================

.. list-table::
   :header-rows: 1
   :widths: 5 25 25 30

   * - #
     - Setup
     - Execution
     - Expectation

   * - 1
     - | Create a Python block, import the **CalculateNodalZParameter** script
       | using the Import Python Script button in the Model tab.
     - Select the Python script in the Python block property.
     - | Block adapts. It has an input and an output port, as well as the property 
       | **"Reference Impedence"** with the value **50**.

   * - 2
     - | Import **"CMC_Stysch_simple.s4p"** in the Import Parameterized Data 
       | tab using the Import Touchstone button.
       | Select **4** ports in the dialog field. 
       | Create a DataBaseAccess (DBA) block and a File Writer block. 
       | Connect the output from the DBA to the input from the File Writer.
     - **In the DBA:** 

         - select the project name of the currently open project in the "project name" property.
         - Select the QuantitySettings.Name property and set it to S-Parameters.
     - | The Output window displays the details about the execution of the DBA 
       | and the File Writer block.
       | One entry describes that the query returned **1001** results. 
       | A text file was added to the navigation tree: **Text Files -> File Writer**

   * - 2.1
     - 
     - 
     - The first entry should look like:
       
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
     -
     - **In the DBA properties:**
     
       - change the type from Magnitude to Phase in the Quantity settings.
       
       **In the File Writer Block:**
       
       - set the File Name property to "Phase".
       
       Run the pipeline.
     - | A text file has been added to the navigation tree: **Text Files -> Phase**.
       | The first entry should look like:
       
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

   * - 3
     - Repeat the setup and the complete execution of Test 2.
     - **In the DBA:**
     
       - Select the comparator **>** and set value 1.

       Run the pipeline.
     - | A text file has been added to the navigation tree: **Text Files -> Phase_1**.
       | The output window shows that the query returned **4** results.
       | The Phase_1 document contains **4** entries (4 S-parameter entries and 4 frequency entries).

   * - 3.1
     -
     - | Replace the File Writer with a Store in Database block.
       | Rename the entity to **“Magnitude > 1”**.
       | Run the pipeline.
     - A new entity named **“Magitude > 1”** appears in the Dataset folder.

   * - 3.2
     -
     - Replace the Store in Database block with a Display block.
     
       **In the DBA:**

         - set the Comparator to “”.

       **In Measurement Series:**
       
         - select Dataset/Magnitude > 1.
       
       Run the pipeline.
     - | The Output window shows that the query returned **4** results.
       | Each entry consists of a matrix named S-Parameter and a field named Frequency.
       | To view the complete metadata, please click :ref:`here <displayed-metadata>`.
       
   * - 3.3
     -
     - Select the **“Magnitude > 1”** entity and click the Info button in the Model tab.
     - Output of metadata as in test 3.2.

   * - 4
     - | Create a new developer project and name it **DataSource**.
       | In the Debug tab, click the **“Single Curve”** button.
       | Switch to the data processing project.
       | Create a DBA block, a Python block, and two Display blocks.
       | Import the **BlockExecution_AddOffset.py** script.
     - **In the Python block:**
         - select the **BlockExecution_AddOffset** script in the Scripts property.
         - Set the Offset property to **10**.
      
       | Connect the DBA block to one of the Display blocks and the input of the Python block.
       | Connect the output of the Python block to the second Display block.
       
       **In the DBA:**
         - select the **“DataSource”** project as the project name and **Magnitude** as the quantity.
         -  No comparator should be selected. 
       
       Run the pipeline.
     - | The data output from the Display block connected to the Python  block
       | should start with a magnitude value of **10**.
       | The data output from the Display block connected to the DBA
       | should start with a magnitude value of **0**.

   * - 4.1
     - | Import the Python script
       | **BlockExecution_ChangeParameterFrequencyUnit.py**.
     - **In the Python block:**
        - select the **BlockExecution_ChangeParameterFrequencyUnit** script.
       
       Run the pipeline.
     - In the metadata, the unit **Hz** should be specified under Parameter->Frequency.

   * - 5
     - | Import **“CMC_Stysch_simple.s4p”** (**4** ports).
       | Create a DataBaseAccess block and a Display block.
       | Set up the DBA block (project name, quantity (S-parameter)).
       | Connect the DBA output to Display.Input.
     - Various comparators and values for parameter 1.
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
          * - is any
            - 9.5,9.6
            - 2 Results (9.5; 9.6)
          * - is not any
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

.. _displayed-metadata:
.. code-block:: json
          :caption: Displayed metadata in test 3.2
          
             {
                 "Selected Campaign": {
                     "Name": "Dataset/Campaign Metadata",
                     "series": [
                         {
                             "Label": "",
                             "Name": "Dataset/CMC_Stysc",
                             "quantities": [
                                 {
                                     "Label": "S-Parameter",
                                     "Name": "S-Parameter",
                                     "Dimensions": [4, 4],
                                     "ValueDescriptions": [
                                         {
                                             "Label": "Magnitude",
                                             "Name": "Magnitude", 
                                             "Type": "double",
                                             "Unit": ""
                                         },
                                         {
                                             "Label": "Phase",
                                             "Name": "Phase",
                                             "Type": "double", 
                                             "Unit": "Deg"
                                         }
                                     ],
                                     "DependingParametersLabels": ["Frequency"]
                                 }
                             ],
                             "parameter": [
                                 {
                                     "Label": "Frequency",
                                     "Name": "Frequency",
                                     "Type": "double",
                                     "Unit": "Hz",
                                     "Values": [9000.0, "[...]", 1000000000.0]
                                 }
                             ]
                         },
                         {
                             "Label": "",
                             "Name": "Dataset/Magnitude > 1",
                             "quantities": [
                                 {
                                     "Label": "S-Parameter",
                                     "Name": "S-Parameter",
                                     "Dimensions": [4, 4],
                                     "ValueDescriptions": [
                                         {
                                             "Label": "Magnitude",
                                             "Name": "Magnitude",
                                             "Type": "double",
                                             "Unit": ""
                                         },
                                         {
                                             "Label": "Phase", 
                                             "Name": "Phase",
                                             "Type": "double",
                                             "Unit": "Deg"
                                         }
                                     ],
                                     "DependingParametersLabels": ["Frequency_2"]
                                 }
                             ],
                             "parameter": [
                                 {
                                     "Label": "Frequency",
                                     "Name": "Frequency", 
                                     "Type": "double",
                                     "Unit": "Hz",
                                     "Values": [
                                         9000.0,
                                         9105.174363669434,
                                         9211.57779920257,
                                         9319.224669582862
                                     ]
                                 }
                             ]
                         }
                     ]
                 }
             }
