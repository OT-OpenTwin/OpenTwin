OToolkit
########

The OToolkit is a work-in-progress project. Data stored locally may be lost or become invalid during development.

OToolkit is a handy debugging tool that implements key features for OpenTwin development. It runs as an OpenTwin service and requires a port number as a startup argument.

.. warning::
   When running OToolkit in Debug Mode, an assertion may fail indicating the logging service is unreachable. This occurs when a Logging Service URL is set in the environment but the :doc:`Logger Service</services/logger_service>` isn't running.
   
   Since OToolkit includes internal logging functionality, the :doc:`Logger Service</services/logger_service>` is optional. You can safely ignore this assertion.

Configuration
*************

Start Arguments
===============

OToolkit accepts startup arguments passed as the fourth parameter. Alternatively, use the ``OpenTwin_toolkit`` script, which properly formats and passes arguments to OToolkit.

.. note::
    Each argument must start with a hyphen ``-`` and be separated by at least one space.

.. list-table:: 
    :header-rows: 1

    *   - Argument
        - Description
    *   - ``logexport``
        - Automatically starts the :ref:`Log Visualization tool<otoolkit_log_visualization>`, connects to the logger service, and exports logs after fetching messages.
    *   - ``noauto``
        - Suppresses automatic tool startup when launching OToolkit. Useful if a tool causes issues during startup.

Features
********

Output
======

The output window displays OToolkit's internal logs. When developing custom tools, use OToolkit's logging macros instead of OpenTwin ServiceLogger macros.

.. _otoolkit_log_visualization:

Log Visualization
=================

The Log Visualization tool displays messages currently buffered in the :doc:`Logger Service</services/logger_service>`.

Apply filters to improve log message overview:

- **Message contains**: Enter text and press Return to apply the filter
- **Clear filter**: Remove text and press Return to show all messages

Connect to Logger Service
-------------------------

To connect to the :doc:`Logger Service</services/logger_service>`:

1. Click the **Connect** button in the Log Visualization menu bar
2. Enter the Logger Service URL in the dialog that appears

After successfully connecting, OToolkit retrieves the current log buffer from the Logger Service.

OTerminal
=========

Use OTerminal to send messages to any OpenTwin service using the OpenTwin message API.