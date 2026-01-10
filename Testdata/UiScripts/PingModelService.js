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

// After project open completed send ping message
AppBase.servicesUiSetupComplete.connect(function () {
    AppBase.appendInfoMessage("Script: Pinging Model service\n");

    var message = "{ \"action\": \"Ping\" }";
    
    var result = AppBase.slotSendExecuteMessageToService("Model", message);
    
    if (result.success===true) {
        AppBase.appendInfoMessage("Script: Ping send successful. Response: " + result.response + "\n");
    }
    else {
        AppBase.appendInfoMessage("Script: Ping send failed\n");
    }
});