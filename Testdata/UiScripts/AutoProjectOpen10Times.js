var PROJECT_INDEX = 10;
var MAX_SWITCHES = 10;

// Auto Login
AppBase.logInDialogAvailable.connect(function (dialog) {
    dialog.dialogShown.connect(function () {
        dialog.slotLogIn();
    });
});

// After login open the first project
AppBase.loginSuccessful.connect(function () {
    AppBase.slotOpenProjectFromIndex(PROJECT_INDEX);
});

// After project open completed switch to other project
var projectSwitchCount = 0;

AppBase.servicesUiSetupComplete.connect(function () {
    if (projectSwitchCount >= MAX_SWITCHES) {
        return;
    }

    AppBase.slotRefreshProjectOverivew();

    projectSwitchCount++;
    AppBase.slotOpenProjectFromIndex(PROJECT_INDEX); 
});