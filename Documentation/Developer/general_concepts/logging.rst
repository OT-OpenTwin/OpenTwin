Logging
=======

OpenTwin provides a :doc:`Logger Service</services/logger_service>` that can receive log messages from multiple services.
The Logger Service stores the received log messages and forwards them to any registered Log Receiver.

.. image:: images/logging_overview.png
    :scale: 50%

========================
Use Logging in a Service
========================

To use the logging functionallity use the ``openTwin/ServiceLogger`` header file which is part of the ``OpenTwinCommunication`` library.
This header file contains the ``LogMessage`` and ``ServiceLogger`` classes.

------------
Setup Logger
------------

When a service is started the logger should be setup by providing at least the current services information.

.. code-block:: c++

    #include "OpenTwinCore/Logger.h"

    char * init(const char * _serviceURL, const char * _localDirectoryServiceURL, const char * _unused1, const char * _unused2) {
    #ifdef _DEBUG
        ot::LogDispatcher::initialize("MyServiceName", true);
    #else 
        ot::LogDispatcher::initialize("MyServiceName", false);
    #endif
        ...
    }

If you want to be able to use the central logging system you should setup the ServiceLogNotifier.
This notifier will register itself at the LogDispatcher when calling the initialize function.
Also the initialize for the LogDispatcher will be called when the LogNotifier initialization is performed.

.. code-block:: c++

    #include "OpenTwinCommunication/ServiceLogNotifier.h"

    char * init(const char * _serviceURL, const char * _localDirectoryServiceURL, const char * _unused1, const char * _unused2) {
    #ifdef _DEBUG
        ot::ServiceLogNotifier::initialize("MyServiceName", "LoggingServiceURL", true);
    #else 
        ot::ServiceLogNotifier::initialize("MyServiceName", "LoggingServiceURL", false);
    #endif
        ...
    }

.. note::

   If the service was created by using the ``ServiceTemplate`` the logger setup is performed automatically.

When creating the Service Logger instance, it sets the logging options from the following environment variables.

.. list-table::
    :header-rows: 1

    * - Name
      - Description
    
    * - OPEN_TWIN_LOGGING_URL
      - The URL where the :doc:`Logger Service</services/logger_service>` is running at.

        If the URL is either empty, or starts with ``//`` then the log messages won't be send to the :doc:`Logger Service</services/logger_service>`

    * - OPEN_TWIN_LOGGING_MODE
      - Sets the active log flags that determine which log messages will be generated.

        * All: *Any log*
        
        * Default: *Only warning and error logs*
        
        * IO: *Only inbound and outbound messages will be logged*
        
        * None: *No log*

.. note::
   The environment variables are optional. Also the variables will be set when running OpenTwin by using the batch files provided with OpenTwin.



---------------------
Generate Log Messages
---------------------

To generate log messages at any point in your code use the :ref:`macros<logMacros>` defined in the ``openTwin/ServiceLogger`` header file.
Log messages will be send to the :doc:`Logger Service</services/logger_service>` if the provided log flags are allowed. 
When the service is running in Debug Mode, the Log Messages will also be written to std::cout.

.. code-block:: c++

    #include <openTwin/ServiceLogger.h>

    namespace test {
        void foo(void) {
            OT_LOG_I("Hello World!");
        }
    }

The example above will generate a log message with:

* Service name: The service that was set in the initialize function
* Function name: test::foo
* Type: Information 
* Text: "Hello World!"
* LocalSystemTime: The time where the Log message object was generated (UTC)
* GlobalSystemTime: The time when the Log message was received by the LoggerService (UTC)

.. _logMacros:

^^^^^^^^^^
Log Macros
^^^^^^^^^^

* ``OT_LOG_I`` Information log: General information
* ``OT_LOG_D`` Detailed log: Detailed information
* ``OT_LOG_W`` Warning log
* ``OT_LOG_E`` Error log

If any other log type(s) should be set for the log message, use the ``OT_LOG`` macro.

============
View the Log
============

There are two approaches to view the log messages.

The first one is to register a receiver at the logger service,
this receiver will get all the currently buffered log messages when registrating at the :doc:`Logger Service</services/logger_service>`. Afther the registration the receiver will receive a copy of every new log message.

The second approach is to request the currently buffered log messages.