.. _frontend_scripting:

Frontend Scripting
##################

OpenTwin supports `JavaScript <https://de.wikipedia.org/wiki/JavaScript>`_ scripting in the user interface.

A script can be provided during application launch via the command line argument ``--script <file path>``.
The provided script will be evaluated just before the LogIn Dialog is shown.

Script usage
************

The script can access the :ref:`Global Objects <ui_scripting_global_objects>` at any point to react to signals, trigger slots and change properties.
The examples showcase the basic usage.

.. _ui_scripting_global_objects:

Global Objects
**************

.. list-table::
    :header-rows: 1

    * - Name
      - Class Name
    
    * - AppBase
      - `AppBase <../_static/codedochtml/class_app_base.xhtml>`_
    
    * - ViewManager
      - `OTWidgets/WidgetViewManager <../_static/codedochtml/classot_1_1_widget_view_manager.xhtml>`_

.. _ui_scripting_examples:

Examples
********

Automatic Login
===============

The following example demonstrates how to automatically login with the last set user credentials.

.. code-block:: js
    :linenos:

    // React to the "logInDialogAvailable" signal of the global "AppBase" object.
    AppBase.logInDialogAvailable.connect(function (dialog) {

        // React to the "dialogShown" signal of the provided "dialog" object.
        dialog.dialogShown.connect(function () {

            // Call the "slotLogIn" slot of the dialog.
            dialog.slotLogIn();
        });
    });

Open Last Project
=================

The following example demonstrates how to open the latest project.

.. code-block:: js
    :linenos:

    // Optionally log-in automatically ...

    // After login open the latest project

    AppBase.loginSuccessful.connect(function () {
        AppBase.slotOpenProjectFromIndex(0);
    });

Switch Project 10 times
=======================

The following example demonstrates how to switch the project ten times.

The script will wait for the session to be open by connecting to the AppBase::servicesUiSetupComplete slot.

.. code-block:: js
    :linenos:

    // Optionally log-in automatically ...

    // Optionally open the last project after login ...

    // Declare variables
    var PROJECT_INDEX = 10;
    var MAX_SWITCHES = 10;
    var projectSwitchCount = 0;

    // After all services have completed their setup switch to other project.
    AppBase.servicesUiSetupComplete.connect(function () {
        // Check if we reached the project switch limit
        if (projectSwitchCount >= MAX_SWITCHES) {
            return;
        }

        // Refresh the project overview
        // This ensures the last accessed times are updated and the projects are sorted
        AppBase.slotRefreshProjectOverivew();

        // Increase the project switch cout
        projectSwitchCount++;

        // Open the project at the given index
        // Index 0 should not be used since 0 is the last accessed project
        // and therefore the currently open project
        AppBase.slotOpenProjectFromIndex(PROJECT_INDEX); 
    });

Send Message to Service
=======================

The following example demonstrates how to send a message to a service via name and via url.

.. code-block:: js
    :linenos:

    // After project open completed send ping message
    AppBase.servicesUiSetupComplete.connect(function () {
        AppBase.appendInfoMessage("Script: Pinging Model service\n");

        var message = "{ \"action\": \"Ping\" }";
    
        // Send ping message to service by name
        var result = AppBase.slotSendExecuteMessageToService("Model", message);
    
        // Display information in output window.
        if (result.success===true) {
            AppBase.appendInfoMessage("Script: Ping send successful. Receiver: \"Model\". Response: " + result.response + "\n");
        }
        else {
            AppBase.appendInfoMessage("Script: Ping send failed to service \"Model\"\n");
        }

        // Send ping message to service by url
        var result2 = AppBase.slotSendExecuteMessageToUrl("127.0.0.1:8000", message);
    
        // Display information in output window.
        if (result2.success===true) {
            AppBase.appendInfoMessage("Script: Ping send successful. Receiver: 127.0.0.1:8000. Response: " + result2.response + "\n");
        }
        else {
            AppBase.appendInfoMessage("Script: Ping send failed to 127.0.0.1:8000\n");
        }
    });

Trigger Tool Bar Button Click
=============================

The following example demonstrates how to trigger a ToolBar button click.

.. code-block:: js
    :linenos:

    // After project open completed trigger the create material button
    AppBase.servicesUiSetupComplete.connect(function () {
        var result = ToolBar.triggerToolBarButton("Model:Material:Create Material");
        if (result===true) {
            AppBase.appendInfoMessage("Script: Button triggered\n");
        }
        else {
            AppBase.appendInfoMessage("Script: Failed to trigger button\n");
        }
    });
