Python Service
==============


The purpose of the Python service is to execute Python code from within OpenTwins C++ environment. Besides simply executing python commands and scripts, it is also possible to access OpenTwin Entities from within the executed Python script or command.

Python Version
--------------
The Python Service uses an interpreter with the version 3.9.13. This version is required from the :ref:`studio suite service<StudioSuiteService>` and thus no embedding of a newer version is intended at this point.


Service Architecture
--------------------
The python functionality founds on two processes. The service, which is acting as any other OpenTwin Microservice is internally PythonExecutionService. Besides this service, there is a subprocess called PythonExecution.
If a session is opened the Subprocess should be listed in the process manager as seen here:

.. image:: ../images/PythonExecutionSubprocess.png

The actual execution of the python interpreter is embedded in the subprocess. This allows the interpreter getting restarted without interrupting the opened session. The PythonExecutionService handles the start, restart and life check of the subprocess. Apart from that the PythonExecutionService simply forwards execution requests that it receives to the subprocess.

Security
^^^^^^^^
Currently, the subprocess is implemented with the QTLocalServer class, which opens a local server with a given name. To guarantee the uniqueness of the server name, the session id is used. However, the QTLocalServer class uses no certificates for communication. Thus, the security concept of this communication still needs some refinement.

Python Service Interface
------------------------
Other services can use the PythonServiceInterface class which is part of the OTServiceFoundation. 
So far it is possible to :
1. Defining a request of executing a python script through its name and a parameter 
2. Adding port data to an execution request
3. Execution of a single string

A script has to be imported into OpenTwin before it can be executed. The python interpreter runs continuously, if the subprocess is not stopped explicitly or the subprocess crashes.
To avoid name conflicts between different script executions, each script is executed in a separat module. By executing a python script a module is created with the name that is assembled with the script UID and Version. The script is then executed within this module. Following an example script is shown: 

.. code-block:: Python
    :linenos:
    
        import sys
        def Test():
            sys.exit()

Any kind of methods can be defined and any modules can be imported. Additional modules have to be located in the OpenTwin\Deployment\Python\site-packages directory. If a custom directory shall be used as location of the site packages it can be specified with the environment variable OT_PYTHON_SITE_PACKAGE_PATH. If this variable is not set, the earlier mentioned default path is used. It is mandatory that the site-package folder contains a version of numpy. OpenTwin uses Numpy Version 1.26.4 per default, older versions may not be supported. 
It is necessary for the interpreter to have a starting point. If the script has only a single method, the PythonService interpretes this method as the entry point for the python execution. A script with multiple methods may look like this:

.. code-block:: Python
    :linenos:
    :caption: CalculateNodalZParameter.py
    
        import numpy as np
        def BuildIdentityMatrix(dimension):
	        return np.identity(dimension)
    
        def __main__(this):
            Sse = np.array([[1, 2], [3, 4]])
            numberOfRows = Sse.shape[0]
            I = BuildIdentityMatrix(numberOfRows)
            Z0 = 50
            Zn = Z0 * np.linalg.inv(I - Sse)*  I+Sse
            return Zn


If more then one method is defined, it is mandatory to have one method called __main__. This main function is then interpreted to be the entry point of the interpreter. The *this* parameter of the main function is internally set to the OpenTwin entity that requested the execution of the python script. One example is the scripted creation of a measurement series from the :ref:`import parameterized data service<ImportParameterizedDataService>`:

.. code-block:: Python
    :linenos:
    :caption: UpdateSelectionWithNextRow.py

    import OpenTwin

    def Test(this):
        topRow = OpenTwin.GetPropertyValue(this,"Top row")
        bottomRow = OpenTwin.GetPropertyValue(this,"Bottom row")
    
        setSuccess = OpenTwin.SetPropertyValue(this,"Top row",topRow+1)
        setSuccess = OpenTwin.SetPropertyValue(this,"Bottom row", bottomRow+1)
    
        flushSuccess = OpenTwin.FlushEntity(this)

This script can be referenced by a *Selection Entity* from the :ref:`import parameterized data service<ImportParameterizedDataService>`. The entity itself is passed to the script as the *this* parameter.
Thus, the script loads the *Selection Entity* und increases the top and bottom row of the selection by one. The functions to interact with OpenTwin are all part of the same named python module.

OpenTwin Module
---------------

The OpenTwin module provides the follwing methods to a python script:

GetPropertyValue
Method Parameter: absolute_entity_name: string, property_name: string|
Get the value of a requested property from a requested entity. |

GetTableCellValue
Method Parameter: row : int, column : int
Get the value of a cell from a requested entity.

SetPropertyValue
Method Parameter:absolute_entity_name: string, property_name: string, value : variant
Set the property value of a requested property from a requested entity.

Flush
Method Parameter: none
Apply all changes on entity properties and store them in the database.

FlushEntity
Method Parameter:absolute_entity_name: string
Apply all changes on requested entity and store it in the database.

GetPythonScript
Method Parameter:absolute_entity_name: string
Gets another python script that is stored within OpenTwin as an executable object.

GetPortData
Method Parameter: portName: string
For Block Items. Gets datachunks from the ingoing ports.

SetPortData
Method Parameter:portName: string, value : variant
For Block Items. Sets datachunks from the outgoing ports.





