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

Objects
*******

.. list-table::
    :header-rows: 1

    * - Object Name
      - Class Name
      - Availability
    
    * - app
      - `Frontend/AppBase <../_static/codedochtml/class_app_base.xhtml>`_
      - Global
    
    * - views
      - `OTWidgets/WidgetViewManager <../_static/codedochtml/classot_1_1_widget_view_manager.xhtml>`_
      - Global
    
    * - ttb
      - `Frontend/ToolBar <../_static/codedochtml/class_tool_bar.xhtml>`_
      - After Login
        
        After a successful login the frontend will create the default UI.
        This will trigger the AppBase::toolBarAvailable(ToolBar) signal.

        If it is enusured that the code snipped is executed after the login, then the ToolBar object can directly be used.

.. _ui_scripting_examples:

Examples
********

Automatic Login
===============

The following example demonstrates how to automatically login with the last set user credentials.

.. code-block:: js
    :linenos:
    :emphasize-lines: 1, 4, 7

    // React to the "logInDialogAvailable" signal of the global "AppBase" object.
    app.logInDialogAvailable.connect(function (dialog) {

        // React to the "dialogShown" signal of the provided dialog object.
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
    :emphasize-lines: 3

    // Optionally log-in automatically ...

    // After login open the latest project
    app.loginSuccessful.connect(function () {
        app.slotOpenProjectFromIndex(0);
    });

Switch Project 10 times
=======================

The following example demonstrates how to switch the project 10 times.

After the login the script will get the available project names and open the first one.

After project open completed the script will open the next project in the list.

.. code-block:: js
    :linenos:
    :emphasize-lines: 3, 9, 21, 34

    // Optionally log-in automatically ...

    // Declare variables
    var MAX_SWITCHES = 10;
    var projectNameList;
    var currentIndex = -1;
    var projectSwitchCount = 0;

    // After login
    app.loginSuccessful.connect(function () {
        // Refresh the project information
        app.slotRefreshProjectOverivew();

        // Get the available project names
        projectNameList = app.getAvailableProjectNames();

        // Open the first project
        openNextProject();
    });

    // After project open completed
    app.servicesUiSetupComplete.connect(function () {

        // Check for project switch limit
        if (projectSwitchCount >= MAX_SWITCHES) {
            return;
        }

        // Increase switch counter and open next project
        projectSwitchCount++;
        openNextProject();
    });

    // Helper function to open the next project
    function openNextProject() {
    
        // Ensure we have at least one project in the list
        if (projectNameList.length==0) {
            return false;
        }

        // Increase index and check for wrap around
        currentIndex++;
        if (currentIndex >= projectNameList.length) {
            currentIndex = 0;
        }

        // Open the next project (name, version)
        app.slotOpenSpecificProject(projectNameList[currentIndex], "");
    }

Send Message to Service
=======================

The following example demonstrates how to send a message to a service via name and via url.

.. code-block:: js
    :linenos:
    :emphasize-lines: 1, 7, 18

    // After project open completed send ping message
    app.servicesUiSetupComplete.connect(function () {
        app.appendInfoMessage("Script: Pinging Model service\n");

        var message = "{ \"action\": \"Ping\" }";
    
        // Send ping message to service by name
        var result = app.slotSendExecuteMessageToService("Model", message);
    
        // Display information in output window.
        if (result.success===true) {
            app.appendInfoMessage("Script: Ping send successful. Receiver: \"Model\". Response: " + result.response + "\n");
        }
        else {
            app.appendInfoMessage("Script: Ping send failed to service \"Model\"\n");
        }

        // Send ping message to service by url
        var result2 = app.slotSendExecuteMessageToUrl("127.0.0.1:8000", message);
    
        // Display information in output window.
        if (result2.success===true) {
            app.appendInfoMessage("Script: Ping send successful. Receiver: 127.0.0.1:8000. Response: " + result2.response + "\n");
        }
        else {
            app.appendInfoMessage("Script: Ping send failed to 127.0.0.1:8000\n");
        }
    });

Trigger Tool Bar Button Click
=============================

The following example demonstrates how to trigger a ToolBar button click.

.. code-block:: js
    :linenos:
    :emphasize-lines: 1

    // After project open completed trigger the create material button
    app.servicesUiSetupComplete.connect(function () {
        var result = ttb.triggerToolBarButton("Model:Material:Create Material");
        if (result===true) {
            app.appendInfoMessage("Script: Button triggered\n");
        }
        else {
            app.appendInfoMessage("Script: Failed to trigger button\n");
        }
    });

Create Material and Select the Item
===================================

The following example demonstrates how a ToolBar button can be clicked, a timer can be used to check for changes and select a desired navigation item.

.. code-block:: js
    :linenos:
    :emphasize-lines: 5, 10, 13, 16, 19, 23, 32, 36, 39, 42, 46, 50, 60, 63, 66, 71, 74

    // Optionally log-in automatically ...

    // Optionally automatically open a project ...

    // Declare variables
    var TIMER_MATERIAL_SELECT = "Script_MaterialSelect";
    var MAX_SELECTION_ATTEMPTS = 30;
    var currentSelectionAttempt = 0;

    // After project open completed trigger the create material logic
    app.servicesUiSetupComplete.connect(function () {

        // If the material does not exist we create a new one
        if (selectMaterial()===false) {

            // To create a new material we trigger the corresponding button
            var result = ttb.triggerToolBarButton("Model:Material:Create Material");

            // Check the result of the trigger command
            if (result===true) {
                app.appendInfoMessage("Script: Button triggered\n");

                // Run the timer to refresh the selection
                app.slotRunCustomTimer(TIMER_MATERIAL_SELECT, 100);
            }
            else {
                app.appendInfoMessage("Script: Failed to trigger button\n");
            }
        }
    });

    // Custom timer timeout
    app.customTimerTimeout.connect(function (timerId) {
        app.appendInfoMessage("Script: Handling timer timeout \"" + timerId + "\"\n");

        // Check which timer has a timeout
        if (timerId == TIMER_MATERIAL_SELECT) {

            // Try to select material
            if (selectMaterial()===false) {

                // Material does not exist, check if we reached the attempts limit
                if (currentSelectionAttempt < MAX_SELECTION_ATTEMPTS) {
                    currentSelectionAttempt++;

                    // Run the timer to refresh the selection
                    app.slotRunCustomTimer(TIMER_MATERIAL_SELECT, 100);
                }
                else {
                    // We reached the maximum number of attempts
                    app.appendInfoMessage("Script: Failed to select tree item after " + MAX_SELECTION_ATTEMPTS.toString() + " attempts\n");
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
        var treeId = app.findNavigationTreeItemByName("Materials/material1");

        // ID 0 is the invalid ID. In that case we cancel and return false
        if (treeId===0) {
            return false;
        }
        else {
            // Clear the current selection
            app.clearNavigationTreeSelection();

            // Select the desired item by its ID
            app.setSingleNavigationTreeItemSelected(treeId, true);

            return true;
        }
    }
