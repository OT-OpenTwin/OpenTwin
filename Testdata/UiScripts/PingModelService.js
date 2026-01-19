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

// After project open completed send ping message
app.servicesUiSetupComplete.connect(function () {
    app.appendInfoMessage("Script: Pinging Model service\n");

    var message = "{ \"action\": \"Ping\" }";
    
    var result = app.slotSendExecuteMessageToService("Model", message);
    
    if (result.success===true) {
        app.appendInfoMessage("Script: Ping send successful. Response: " + result.response + "\n");
    }
    else {
        app.appendInfoMessage("Script: Ping send failed\n");
    }
});