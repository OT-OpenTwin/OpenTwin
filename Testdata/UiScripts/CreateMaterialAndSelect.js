var TIMER_MATERIAL_SELECT = "Script_MaterialSelect";
var MAX_SELECTION_ATTEMPTS = 30;
var currentSelectionAttempt = 0;

// Auto Login
app.logInDialogAvailable.connect(function (dialog) {
    dialog.dialogShown.connect(function () {
        dialog.slotLogIn();
    });
});

// After login open the latest project
app.loginSuccessful.connect(function () {
    app.slotOpenProjectFromIndex(0);
});

// After project open completed trigger the create material logic
app.servicesUiSetupComplete.connect(function () {
    
    // If the material does not exist we create a new one
    if (selectMaterial()===false) {

        // To create a new material we trigger the corresponding button
        var result = ttb.triggerToolBarButton("Model:Material:Create Material");

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