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