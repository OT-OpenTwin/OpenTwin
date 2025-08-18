Python Testing Specification
============================

Notes
-----

| TTB Stands for TabToolBar.
| Import Scripts via TTB/Model/Import PythonScript.
| Return State != OK will be displayed in the Frontend output.

Test Cases
----------

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

   * - 6
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

   * - 7
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

   * - 8
     - | Import script: **TestScript_Numpy.py**
       | Datapipeline: Python block with script set
       | Connect Python block out with Display block
     - Run Data pipeline
     - | Successful with UI message:
       
       Numpy Matrix is:
       
       .. code-block:: text
       
          [[ 6.  -1.7]
           [30.  44. ]]
       
       Determinant of given 2X2 matrix: **315**

   * - 9
     - | Import script: **TestScript_NoMain.py**
       | Datapipeline: Python block with script set
       | Connect Python block out with Display block
     - Run Data pipeline
     - Message from the DataProcessingService that the execution failed because the main function is missing.