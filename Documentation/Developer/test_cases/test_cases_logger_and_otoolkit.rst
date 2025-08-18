Logger and OToolkit
###################

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

    * - 1
      - Log Visualization Type Filter
      - * Open any project.
        * Run OToolkit.
        * Open the log visualization tool.
        * Connect to the logger service.
      - Check different message type settings.
      - The displayed log messages should match the filter.

    * - 2
      - Log Visualization Content Filter
      - * Open any project.
        * Run OToolkit.
        * Open the log visualization tool.
        * Connect to the logger service.
      - * Check all message type filters.
        * Enter "health check" in the message contains filter input.
      - Only log messages containing "health check" should be displayed.

    * - 3
      - Log Visualization User Filter
      - * Open any project.
        * Close the project.
        * Open a different project with a different user.
        * Run OToolkit.
        * Open the log visualization tool.
        * Connect to the logger service.
      - Select one of the users in the user filter.
      - * Only messages of the selected user are displayed.
        * Messages of the core services are not displayed.

    * - 4
      - Log Visualization Project Filter
      - * Open/create any project.
        * Close the project.
        * Open/create a different project.
        * Run OToolkit.
        * Open the log visualization tool.
        * Connect to the logger service.
      - Select one of the projects in the project filter.
      - * Only messages of the selected project are displayed.
        * Messages of the core services are not displayed.

    * - 5
      - Terminal send success
      - * Run the core services.
        * Run OToolkit.
        * Open the Terminal tool.
      - Send a ping message to one of the core services (with the endpoing execute):
        
        .. code-block:: json
          
          {
            "action": "Ping"
          }

      - The message is delivered successfully and a "Ping" response is received.

    * - 6
      - Terminal send fail
      - * Run the core services.
        * Run OToolkit.
        * Open the Terminal tool.
      - Send a ping message to an invalid url (e.g. 127.0.0.1:1111):
        
        .. code-block:: json

          {
            "action": "Ping"
          }

      - The message can not be delivered and a corresponding message is displayed in the output window.

    * - 7
      - Terminal save request
      - * Run OToolkit.
        * Open the Terminal tool.
      - * Right click any folder in the terminal requests.
        * Press new request and save it.
        * Change the message, url and endpoint.
        * Save this as a new request.
        * Click on the requests.
      - The requests are restored correctly.

    * - 8
      - Terminal export requests
      - * Run OToolkit.
        * Open the Terminal tool.
      - * Create some filters and requests in the overview.
        * Right click requests.
        * Press export and save the requests.
        * Delete all filters and requests.
        * Import the requests.
      - The requests are restored correctly.
