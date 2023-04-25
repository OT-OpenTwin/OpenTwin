Local Directory Service
=======================

The Local Directory Service (internally called "LDS") is responsible for starting and monitoring services.

=============
Configuration
=============

.. warning::
   The Local Directory Service requires a configuration to be able to start

When the Local Directory Service is starting it imports its configuration from the environment variable ``OT_LOCALDIRECTORYSERVICE_CONFIGURATION``.
The value must be a `JSON <https://www.json.org/>`_ object.

-----------------------
Configuration Structure
-----------------------

The JSON object containing the configuration must contain the following name/value pairs:

.. list-table::
    :header-rows: 1

    * - Name
      - Type
      - Default value
      - Description
      
    * - DefaultMaxCrashRestarts
      - Unsigned Integer
      - 8
      - The default number of maximum restarts for a service after it crashes.
        The crash counter is evaluated for every service in every session seperatly.
    
    * - DefaultMaxStartupRestarts
      - Unsigned Integer
      - 64
      - The default number of start attempts for a service.
        The start counter is evaluated for every service in every session seperatly.
        :ref:`Blocked Port Detection<blockedPortDetection>`
        If a service is restarted after a crash, its startup counter is reset.

    * - LauncherPath
      - String
      - Executable in the repository deployment folder ``e.g. C:\OpenTwin\Deployment\open_twin.exe``.
      - The full path to the Open Twin webserver executable which is used to attach the Service Libraries.
    
    * - ServicesLibraryPath
      - String
      - Deployment folder ``e.g. C:\OpenTwin\Deployment``
      - The path to the folder where the service binaries are located at

    * - SupportedServices
      - Array of Strings and Objects
      - List of all Services that are provided by the Open Twin repository
      - The supported services array contains all services that are supported by the Local Directory Service

.. warning::
   Note that the default values are set by the batch file ``OpenTwin_set_up_LDS.bat``.

   If providing a custom configuration, the configuration object must contain all of the members described in the table above.

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Supported Service Configuration Structure
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A supported service can be provided by a string (the String represents name and type, default values will be applied), or by an object with the following structure:

.. list-table::
    :header-rows: 1

    * - Name
      - Type
      - Default value
      - Description
    
    * - Name
      - String
      - None
      - The name of the service. If no service type is provided this value will also be applied as service type

    * - Type
      - String
      - The Service Name
      - The type of the service
    
    * - MaxCrashRestarts
      - Unsigned Integer
      - Configuration default value
      - The default number of maximum restarts for a service after it crashes.
        The crash counter is evaluated for every service in every session seperatly.

    * - MaxStartupRestarts
      - Unsigned Integer
      - Configuration default value
      - The default number of start attempts for a service.
        The start counter is evaluated for every service in every session seperatly.
        If a service is restarted after a crash, its startup counter is reset.

.. _blockedPortDetection:

======================
Blocked Port Detection
======================

When a service was successfully started (Service process started) the Local Directory Service will attempt to send a Ping action to the Service.
If the ping was not delived 3 times (1 second timeout for each ping attempt) the service will restart at another port number and the current port number will be marked as blocked.
