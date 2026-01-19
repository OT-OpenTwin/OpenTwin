var MAX_SWITCHES = 10;
var projectNameList;
var currentIndex = -1;
var projectSwitchCount = 0;

// Auto Login
app.logInDialogAvailable.connect(function (dialog) {
    dialog.dialogShown.connect(function () {
        dialog.slotLogIn();
    });
});

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
    
    // Ensure we have at least two projects in the list
    if (projectNameList.length < 2) {
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