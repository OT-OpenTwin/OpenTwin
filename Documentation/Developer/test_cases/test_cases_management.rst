Management
##########

Basic functionallity
********************

.. list-table::
    :header-rows: 1

    * - #
      - Name
      - Setup
      - Execution
      - Expectation

    * - 1
      - Sleep test
      - Open a session on a remote installation (e.g. Strato).
      - * Put the local machine, connected to the remove server, to sleep.
        * Wake the local machine
      - A error message is displayed showing that the communication channel is closed and the application needs to be closed.

    * - 2
      - Connect after sleep 1
      - Perform directly after completing test **#1**.
      - Try to open the same project as in test **#1**.
      - A error message is displayed showing that the session is still opened.

    * - 3
      - Connect after sleep 2
      - Perform directly after completing test **#2**.
      - * Wait for 2+ minutes.
        * Try to open the same project as in test **#2**.
      - The project opens successfully.

    * - 4
      - Fesh Install
      - Install OpenTwin on a fresh VM (No previous OpenTwin installation).
      - Create a project and run some tests.
      - Everything should work as expected.

    * - 5
      - New Install
      - Install OpenTwin on a VM which had an older Release of OpenTwin installed
      - Create a project and run some tests.
      - Everything should work as expected.

Project, User and Group management
**********************************

.. list-table::
    :header-rows: 1

    * - #
      - Name
      - Setup
      - Execution
      - Expectation

    * - 1
      - Rename Project 1
      - Create a project with the name **Test 1**.
      - Rename the project to **Test 2**.
      - The project was renamed successfully.

    * - 2
      - Rename Project 2
      - * Complete test **#1**.
        * Create a project with the name **Test 1**.
      - Try to rename the project **Test 1** to **Test 2**.
      - A error message is displayed showing that the name is already taken and the project cannot be renamed.

    * - 3
      - Overwrite Project
      - Complete test **#1**.
      - Try to create a project with the name **Test 2**.
      - A warning message is displayed showing that the project name is already taken and the project would be overwritten.

    * - 4
      - Create User
      - Create a new user if not done so before.
      - Create a new user with the same name as the existing user.
      - A error message is displayed showing that the user name is already taken.

    * - 5
      - Create Group 1
      - * Create two users.
        * Create a project with one of the users.
      - * Create a group (Manage Groups).
        * Assign the other user to the new group.
      - * Both users see that they belong to the group
        * Only the group owner can add/remove users

    * - 6
      - Create Group 2
      - Complete test **#5**.
      - Try to create a new group with the same name as the existing group.
      - A error message is displayed showing that the group name is already taken.

    * - 7
      - Group Access
      - * Complete test **#5**.
        * Create a project.
      - Give the group access to the project.
      - All users in the group can see and open the project (press View All in the ToolBar to see all projects).

    * - 8
      - Project owner
      - Have at least two users.
      - * Create a project with one user.
        * Change the project owner to the other user.
      - * The other user can now see/open the project.
        * The initial user can not see/open the project anymore.

    * - 9
      - Project export
      - Create a project
      - * Make some modifications to the project (e.g. Create a geometry)
        * Export the project
        * Delete the project (in OpenTwin)
        * Import the project
      - The project should be completely intact.
