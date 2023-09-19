OToolkit
========

The OToolkit is a "still in progress" project, therefore any locally stored data may be lost or may become invalid.
Check the Discord channel or the new versions of this documentation for updates.

When working with/for OpenTwin the OToolkit application is a handy tool which implements some key features that will decrease the work load.
The OToolkit application is running as a OpenTwin Service, therefore the port number must be passed as a start argument.


.. warning::
   If you are running the OToolkit application in Debug Mode one particular assertion may fail. This assertion (as stated in the assertion message) will tell you that the logging service could not be reached.
   This happens if a Logging Service URL is set in the Environment, but the :doc:`Logger Service</services/logger_service>` is not running.
   
   Since the OToolkit has an internal Logging functionality the :doc:`Logger Service</services/logger_service>` is not required to be running.
   Therefore the assertion may simply be ignored.

========
Features
========

------
Output
------

The output window is used for the OToolkit internal logging.
When developing a custom tool for the OToolkit ensure that you use the provided OToolkit logging macros and NOT the OpenTwin ServiceLogger macros.

-----------------
Log Visualisation
-----------------

The Log Visualisation tool does what the name says,
it displays the log messages that are currently buffered in the :doc:`Logger Service</services/logger_service>`.

A filter can be applied to get a better overview.
If the "Message contains" filter is used, ensure that you confirm it by pressing the return key.
If you remove the filter text from the "Message contains" filter, ensure that you confirm this by pressing the return key aswell.

^^^^^^^^^^^^^^^^^^^^^^^^^
Connect to Logger Service
^^^^^^^^^^^^^^^^^^^^^^^^^

To connect to the :doc:`Logger Service</services/logger_service>` press ``Connect Button`` in the ``Log Visualisation`` menu bar menu.
After pressing the button a Dialog will appear and will ask you to type in the Logger Service URL.

After successfully establishing a connection to the :doc:`Logger Service</services/logger_service>` its current buffer will be returned to the OToolkit application.

---------
OTerminal
---------

The OTerminal can be used to send messages to any OpenTwin Service by using the OpenTwin message API.

^^^^^^^^^^^^^^^^^^^
Request Collections
^^^^^^^^^^^^^^^^^^^

Working on it :-)




==========
Appearance
==========

A custom stylesheet can be set for the OToolkit application.
Simply place a file with the name ``OToolkit.style.qss`` in the same directory where the OToolkit is located at.
This file must contain a `Qt Stylesheet <https://doc.qt.io/qt-5/stylesheet-reference.html>`_.
