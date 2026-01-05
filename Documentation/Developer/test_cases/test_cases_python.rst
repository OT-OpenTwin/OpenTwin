Python Testing Specification
============================

Notes
-----

| TTB Stands for TabToolBar.
| Import Scripts via TTB/Model/Import PythonScript.
| Return State != OK will be displayed in the Frontend output.
| To run a pipeline, first create one by selecting the **Create Pipeline** button in the Data Processing tab.
| Then create a solver by selecting the corresping button in the Data Processing tab. 
| Select the pipeline that shall be executed in the solver's properties. 
| Select a solver and start it by selecting the **Run** button in the Data Processing tab.

Error Handling Tests
--------------------
For the following tests, no environment shall be selected.

.. list-table::
   :header-rows: 1
   :widths: 5 25 25 30

   * - #
     - Setup
     - Execution
     - Expectation

   * - 1
     - | Import script: **TestScript_Abort.py**
       | Datapipeline: Python block with script set
       | Connect Python block out with Display block
     - Run Data pipeline
     - | Message from the DataProcessingService that the execution failed because abort was called.
       | Subprocess is still alive.

   * - 2
     - | Import script: **TestScript_Exit.py**
       | Datapipeline: Python block with script set
       | Connect Python block out with Display block
     - Run Data pipeline
     - | Message from the DataProcessingService that the execution failed because a SystemExit occurred.
       | Subprocess is still alive.

   * - 3
     - | Import script: **TestScript_Throw.py**
       | Datapipeline: Python block with script set
       | Connect Python block out with Display block
     - Run Data pipeline
     - | Message from the DataProcessingService that the execution failed because an exception was thrown.
       | Exception content: **(‘spam’, ‘eggs’)**

   * - 4
     - | Import script: **TestScript_ExtensionException.py**
       | Datapipeline: Python block with script set
       | Connect Python block out with Display block
     - Run Data pipeline
     - Message from the DataProcessingService that the execution failed because the property does not exist.

   * - 5
     - | Import script: **TestScript_ModuleNotFound.py**
       | Datapipeline: Python block with script set
       | Connect Python block out with Display block
     - Run Data pipeline
     - Message from the DataProcessingService that the execution failed because the module could not be found.

   * - 9
     - | Import script: **TestScript_NoMain.py**
       | Datapipeline: Python block with script set
       | Connect Python block out with Display block
     - Run Data pipeline
     - Message from the DataProcessingService that the execution failed because the main function is missing.

Misc Tests
----------

For the following tests, no environment shall be selected.

.. list-table::
   :header-rows: 1
   :widths: 5 15 25 25 30

   * - #
     - Target
     - Setup
     - Execution
     - Expectation

   * - 1
     - | Print redirection
     - | Import script: **TestScript_HelloWorld.py**
       | Datapipeline: Python block with script set
       | Connect Python block out with Display block
     - | Run Data pipeline
     - | In the output box is a "Hello World" written


   * - 2
     - | Accessing entity properties
     - | Import **Prüfmatrix_S8_Ausschnitt.csv**, change Column delimiter to **','**
       | Import script: **TestScript_TableCell.py**
       | Datapipeline: Python block with script set
       | Connect Python block out with Display block
     - Run Data pipeline
     - | Successful with UI message: 
       
       Data: 329
       
       Metadata:
       
       .. code-block:: json
       
          {
              "Selected Campaign": null,
              "Selected Series label": null
          }

   * - 3
     - | Executing another script from within a script
     - | Import **Prüfmatrix_S8_Ausschnitt.csv**, change Column delimiter to **','**
       | Import script 1: **TestScript_TableCell.py**
       | Import script 2: **TestScript_ExecuteOtherScript.py**
       | Datapipeline: Python block with script **2** set
       | Connect Python block out with Display block
     - Run Data pipeline
     - | Successful with UI message: 
       
       Data: **329**
       
       Metadata:
       
       .. code-block:: json
       
          {
              "Selected Campaign": null,
              "Selected Series label": null
          }


Environment Tests
-----------------

Restart OpenTwin before proceeding with the following tests.

.. list-table::
   :header-rows: 1
   :widths: 5 15 25 25 30

   * - #
     - Target
     - Setup
     - Execution
     - Expectation
   * - 1
     - | Protection of default environment
     - | Import script: **TestScript_Numpy.py**
       | Datapipeline: Python block with script set
       | Connect Python block out with Display block
     - Run Data pipeline
     - | Error message in output window, stating that packages cannot be added to the default environment.

   * - 2
     - | Creation of new environment by script analysis
     - | Import script: **TestScript_Numpy.py**
       | Datapipeline: Python block with script set
       | Connect Python block out with Display block
       | Create a new manifest and select it in the python block properties.
     - Run Data pipeline
     - | Successful with UI message:
       
       Installing package: numpy
       Installing completed
       Numpy Matrix is:
       
       .. code-block:: text
       
          [[ 6.  -1.7]
           [30.  44. ]]
       
       Determinant of given 2X2 matrix: **315**

       | In the navigation tree in the Python folder, an Installation log entity appeared.  It contains a timestamp and details about the installed packages.
       | The Environment entity in the Python/Manifest folder has an entry for the installed numpy version, e.g.: "numpy==2.4.0"

   * - 3
     - | Change of environment package version
     - | After finishing test 2, change the numpy version noted in the environment entity. 
       | E.g.: "numpy==2.4.0" -> "numpy==2.2.0"
     - Run Data pipeline
     - | Successful with UI message:
       
       Installing package: numpy==2.2.0
       Installing completed
       Numpy Matrix is:
       
       .. code-block:: text
       
          [[ 6.  -1.7]
           [30.  44. ]]
       
       Determinant of given 2X2 matrix: **315**

       | In the navigation tree in the Python folder, an Installation log entity appeared.  It contains a timestamp and details about the installed packages.
       | The Environment entity in the Python/Manifest folder has an entry for the installed numpy version, e.g.: "numpy==2.4.0"

   * - 4
     - | Change of environment package version
     - | After finishing test 3, change the numpy version noted in the environment entity. E.g.: "numpy==2.4.0" -> "numpy==2.2.0"
     - Run Data pipeline
     - | Successful with UI message:
       
       Installing package: numpy==2.2.0
       Installing completed
       Numpy Matrix is:
       
       .. code-block:: text
       
          [[ 6.  -1.7]
           [30.  44. ]]
       
       Determinant of given 2X2 matrix: **315**

       | In the navigation tree in the Python folder, an Installation log entity appeared.  It contains a timestamp and details about the installed packages.
       | The Environment entity in the Python/Manifest folder has an entry for the installed numpy version, e.g.: "numpy==2.4.0"

   * - 5
     - | Invalid manifest
     - | After finishing test 4, change the entry into an invalid form. 
       | E.g.: "numpy==2.4.0" -> "numpy 2.2.0"
     - Run Data pipeline
     - | Fail with UI message:
       | Failed to initialize environment: Invalid manifest

   * - 6
     - | Environment garbage collector
     - | Repeatedly change the version of numpy until there are seven folders named with numbers in the Deployment/PythonEnvironments folder. 
       | Alter the last-access-date of three folders to four days before. This can be done for the folder 1234 for instance in a powershell with:
       | $file=Get-Item "1234"
       | $file.LastAccessTime = "2026-01-01 12:00:00"

     - Select a new environment variable for the python block and run the Data pipeline. Keep an eye on the Deployment/PythonEnvironments folder.
     - | First, the three folders are renamed by appending "_remove" to each name. Afterwards they are deleted one by one. Finally, only three folders with numbers as name should remain.