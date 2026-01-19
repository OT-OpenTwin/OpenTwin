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