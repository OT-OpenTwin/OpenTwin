Frontend Selection and View Handling
####################################



Logging test cases
******************

.. list-table::
    :header-rows: 1

    * - #
      - Name
      - Setup
      - Execution
      - Expectation

    * - 1
      - General logging
      - * All open twin instances and services are shut down (Shutdown all).
        * :ref:`Set the logger<logging_environment_variables>` to all log flags.
      - * Run OpenTwin.
        * Create/open a project.
        * Run the OToolkit.
        * Run the Log Visualization tool in the OToolkit.
        * Connect to the logger.
      - * The log visualization should display all current log messages.
        * New log messages should be appended.
        * The message view (double click a row) should work.

    * - 2
      - Log flags 1
      - :ref:`Set the logger<logging_environment_variables>` to inbound and outbound message logs only.
      - * Run OpenTwin.
        * Create/open a project.
        * Run the OToolkit.
        * Run the Log Visualization tool in the OToolkit.
        * Connect to the logger.
      - * Only inbound and outbound message logs should be displayed.
        * A green "light" should be displayed at the log level status in the frontend.
        * When hovering over the log level status the correct log flags are displayed.

    * - 3
      - Log flags 2
      - Complete test **#2**.
      - * Clear the logs in the OToolkit log visualization window.
        * Change the log flags to Information and Detailed via the **Log Switch** in the OToolkit.
        * Click around a bit in the Frontend (e.g. Select some entities and/or change some properties).
      - * When hovering over the log level status the correct log flags are displayed.
        * Only Information and Detailed log messages are displayed in the log visualization window of the OToolkit.

    * - 4
      - File logging
      - * All OpenTwin instances and services are shut down (Shutdown all).
        * :ref:`File logging<use_file_logging>` is enabled
        * :ref:`Set the logger<logging_environment_variables>` to all log flags.
      - * Run OpenTwin
        * Create/open a project
        * Perform a circuit simulation
        * Perform the execution of a Python script
      - * All core services created a log file.
        * The frontend created a log file.
        * All servives running in the session created log files.
        * The subservices (CircuitExecution and PythonExecution) created log files.

    * - 5
      - Core logging
      - * All open twin instances and services are shut down (Shutdown all).
        * :ref:`Set the logger<logging_environment_variables>` to all log flags.
      - * Run the core services (Logger, Authorization, GSS, LSS, GDS, LDS).
        * Run the OToolkit.
        * Run the Log Visualization tool in the OToolkit.
        * Connect to the logger.
      - * The log visualization should display all current log messages.
        * New log messages should be appended.
    
OToolkit test cases
*******************

.. list-table::
    :header-rows: 1

    * - #
      - Name
      - Setup
      - Execution
      - Expectation

    * - 
      - 
      - 
      - 
      - 

    * - 
      - 
      - 
      - 
      - 

    * - 
      - 
      - 
      - 
      - 
    
