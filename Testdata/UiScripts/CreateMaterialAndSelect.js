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