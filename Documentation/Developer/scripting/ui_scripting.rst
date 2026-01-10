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

.. code-block:: js
    :linenos:

    // Optionally log-in automatically ...

    // After login open the latest project

    AppBase.loginSuccessful.connect(function () {
        AppBase.slotOpenProjectFromIndex(0);
    });

Switch Project 10 times
=======================

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
