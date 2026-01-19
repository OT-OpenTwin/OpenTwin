var PROJECT_INDEX = 10;
var MAX_SWITCHES = 10;

// Auto Login
app.logInDialogAvailable.connect(function (dialog) {
    dialog.dialogShown.connect(function () {
        dialog.slotLogIn();
    });
});

// After login open the first project
app.loginSuccessful.connect(function () {
    app.slotOpenProjectFromIndex(PROJECT_INDEX);
});

// After project open completed switch to other project
var projectSwitchCount = 0;

app.servicesUiSetupComplete.connect(function () {
    if (projectSwitchCount >= MAX_SWITCHES) {
        return;
    }

    app.slotRefreshProjectOverivew();

    projectSwitchCount++;
    app.slotOpenProjectFromIndex(PROJECT_INDEX); 
});