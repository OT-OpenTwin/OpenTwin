.. _scheduled_task_startup:

Starting OpenTwin via Windows Scheduled Task
============================================

Windows provides two main mechanisms for executing software or scripts at startup: **Windows Services** and **Scheduled Tasks**. Either method may be appropriate for launching OpenTwin automatically, such as after a system reboot.

This section details how to configure OpenTwin to start automatically using a **Scheduled Task**.

Creating a Scheduled Task
-------------------------

To configure OpenTwin as a scheduled task, follow the steps below:

1. Open the **Task Scheduler** and select **Create Task**.
2. In the **General** tab, assign a name to the task (e.g., ``OpenTwin``).
3. Navigate to the **Actions** tab and click **New...**.
   - Specify the path to the script ``OpenTwin_task_all_services.bat``, which is located in the OpenTwin installation directory.
4. In the **Triggers** tab:
   - Choose **Begin the task**: ``At startup``.
5. In the **General** tab under **Security Options**:
   - Select **Change User or Group...** to specify the user context under which the task will run.

Security Context and User Account Selection
-------------------------------------------

Windows offers several built-in security principals that can be used to execute scheduled tasks. These are also commonly used to run services:

- **SYSTEM (LocalSystem)**:  
  This is the most privileged built-in account. It has unrestricted access to the local system and uses the computer’s identity when accessing network resources. This account is recommended for guaranteed execution, particularly when elevated permissions are required.

- **NETWORK SERVICE**:  
  This account has the same local permissions as a regular user but accesses network resources using the computer's credentials. It is suitable for scenarios where authenticated network access is necessary.

- **LOCAL SERVICE**:  
  This account also runs with user-level privileges locally but accesses network resources as an anonymous user. It is appropriate for limited-permission contexts without authenticated network access.

| We recommend to use the **SYSTEM** account to ensure robust and unrestricted operation. 

| If the deployment requires execution with restricted privileges, either **LOCAL SERVICE** or **NETWORK SERVICE** should be used. However, note that the OpenTwin admin panel cannot be launched under these accounts. To support the admin panel, a secondary scheduled task can be created using the ``OpenTwin_task_admin_panel.bat`` script, configured to run under the **SYSTEM** account

Further Information
-------------------

Refer to the Microsoft documentation for more details on Windows security identifiers and account privileges:  
`Windows Access Management <https://learn.microsoft.com/en-us/windows-server/identity/ad-ds/manage/understand-security-identifiers>`_
