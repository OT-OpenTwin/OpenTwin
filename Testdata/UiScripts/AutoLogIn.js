// Auto Login
AppBase.logInDialogAvailable.connect(function (dialog) {
    dialog.dialogShown.connect(function () {
        dialog.slotLogIn();
    });
});
