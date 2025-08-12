End User Installation Test Specifications
=========================================

.. list-table::
   :header-rows: 1
   :widths: 5 25 25 30

   * - #
     - Setup
     - Execution
     - Expectation

   * - 1
     - Create a fresh VM with Windows installed. Add german keyboard settings if not available yet and copy the end user installer into the VM. Set a checkpoint (recovery point). Reboot is not necessary.
     - Execute the installer. Keep all default settings, except the of the port settings. Use the costumised port option and select different values for all ports (take a screenshot of the chosen ports).
     - Installation succeeds. MongoDB is running and OpenTwin can be started. Projects can be created. Additionally, test a few functions within the project.
       
   * - 1.1
     - 
     - Take the OT_GetSysState from the OpenTwin Repo->Scripts->Other copy it into the VM into the desctop and execute it.
     - A textfile is created in the desctop, containing all the port settings. The certificate folder env OPEN_TWIN_CERTS_PATH should be set to the installation directory.
  
   * - 1.2
     - 
     - In an internet brwoser, enter after another https://127.0.0.1:<port number> for all costumised ports. Only the ui frontend needs http without the 's'
     - For the UI frontend: The webside will show the OpenTwin Logo and a Download button. 
       For The Admin Panel: The webside shows the log-in screen for the Admin Panel.
       For all other ports: The browser shows a short message in which some details of the pinged service are mentioned

   * - 1.3
     - 
     - Log into the admin pannel. Use the default credentials admin admin.
     - Admin dashboard should be shown. Number of projects and users should show the made changes.

   * - 1.4
     - 
     - Reinstall OpenTwin (Accept the removal of existing data). Go through the settings without finishing the installation.
     - The costumised ports should be set as they were defined during the last installation (they should not be the default values) 

   * - 1.5
     - Still in the process of the installation. Still don't conclude it.
     - Change some of the port settings. Change to the prior page of the installer (back button) and proceed again to the port settings page.
     - Adjusted port settings should persist.

   * - 1.6
     - Still during the installation. 
     - In the Server Settings, choose Public IP and enter 127.0.0.1. In the next page enter the custom path to C:\Users\User\Documents\OT_Certs per copy paste. Finish the installation.
     - Try OpenTwin, it should still work. Check the if OPEN_TWIN_CERTS_PATH is now pointing to C:\Users\User\Documents\OT_Certs

   * - 1.7
     - OpenTwin is installed and in the install directory (default is Program Filesx86/OpenTwin) should be a folder called "Documentation".
     - Start a reinstallation. In the "Choose Components" page, unset the "Documentation" option. 
     - In the install options, the option for "Documentation" should have been set by default. After the reinstallation there is no "Documentation" folder in the installation directory.

   * - 1.8
     - After test 1.8. 
     - Start a reinstallation. In the install options, unset the "Documentation" option. 
     -  In the install options, the option for "Documentation" should have been unset by default.

