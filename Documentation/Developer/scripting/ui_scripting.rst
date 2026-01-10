.. _frontend_scripting:

Frontend Scripting
##################

OpenTwin supports `JavaScript <https://de.wikipedia.org/wiki/JavaScript>`_ scripting in the user interface.

A script can be provided during application launch via the command line argument ``--script <file path>``.
The provided script will be evaluated just before the LogIn Dialog is shown.

Script usage
************

The script can access different :ref:`Objects <ui_scripting_objects>` to react to signals, trigger slots and change properties.
The :ref:`examples <ui_scripting_examples>` showcase the basic usage.

.. _ui_scripting_objects:

Global Objects
**************

.. list-table::
    :header-rows: 1

    * - Name
      - Class Name
      - Availability
    
    * - AppBase
      - `Frontend/AppBase <../_static/codedochtml/class_app_base.xhtml>`_
      - Global
    
    * - ViewManager
      - `OTWidgets/WidgetViewManager <../_static/codedochtml/classot_1_1_widget_view_manager.xhtml>`_
      - Global
    
    * - ToolBar
      - `Frontend/ToolBar <../_static/codedochtml/class_tool_bar.xhtml>`_
      - After Login
        
        After a successful login the frontend will create the default UI.
        This will trigger the AppBase::toolBarAvailable(ToolBar) signal.

        If it is enusured that the code snipped is executed then the ToolBar object can directly be used.

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

Create Material and Select the Item
===================================

The following example demonstrates how a ToolBar button can be clicked, a timer can be used to check for changes and select a desired navigation item.

.. code-block:: js
    :linenos:

    var TIMER_MATERIAL_SELECT = "Script_MaterialSelect";
    var MAX_SELECTION_ATTEMPTS = 30;
    var currentSelectionAttempt = 0;

    // Auto Login
    AppBase.logInDialogAvailable.connect(function (dialog) {
        dialog.dialogShown.connect(function () {
            dialog.slotLogIn();
        });
    });

    // After login open the latest project
    AppBase.loginSuccessful.connect(function () {
        AppBase.slotOpenProjectFromIndex(0);
    });

    // After project open completed trigger the create material logic
    AppBase.servicesUiSetupComplete.connect(function () {

        // If the material does not exist we create a new one
        if (selectMaterial()===false) {

            // To create a new material we trigger the corresponding button
            var result = ToolBar.triggerToolBarButton("Model:Material:Create Material");

            if (result===true) {
                AppBase.appendInfoMessage("Script: Button triggered\n");

                // Run the timer to refresh the selection
                AppBase.slotRunCustomTimer(TIMER_MATERIAL_SELECT, 100);
            }
            else {
                AppBase.appendInfoMessage("Script: Failed to trigger button\n");
            }
        }
    });

    // Custom timer timeout
    AppBase.customTimerTimeout.connect(function (timerId) {
        AppBase.appendInfoMessage("Script: Handling timer timeout \"" + timerId + "\"\n");

        // Check which timer has a timeout
        if (timerId == TIMER_MATERIAL_SELECT) {

            // Try to select material
            if (selectMaterial()===false) {

                // Material does not exist, check if we reached the attempts limit
                if (currentSelectionAttempt < MAX_SELECTION_ATTEMPTS) {
                    currentSelectionAttempt++;

                    // Run the timer to refresh the selection
                    AppBase.slotRunCustomTimer(TIMER_MATERIAL_SELECT, 100);
                }
                else {
                    // We reached the maximum number of attempts
                    AppBase.appendInfoMessage("Script: Failed to select tree item after " + MAX_SELECTION_ATTEMPTS.toString() + " attempts\n");
                }
            }
            else {
                currentSelectionAttempt = MAX_SELECTION_ATTEMPTS;
            }
        }
    });

    // Helper function to select the material. Returns true on success, false otherwise
    function selectMaterial() {

        // The the ID of the desired navigation tree item
        var treeId = AppBase.findNavigationTreeItemByName("Materials/material1");

        // ID 0 is the invalid ID. In that case we cancel and return false
        if (treeId===0) {
            return false;
        }
        else {
            // Clear the current selection
            AppBase.clearNavigationTreeSelection();

            // Select the desired item by its ID
            AppBase.setSingleNavigationTreeItemSelected(treeId, true);

            return true;
        }
    }
