Create a new service
====================

Enable Service Usage
--------------------

New services must be added to the supported services list of the :ref:`Local Directory Service<lds-head>`.

Service Template
----------------

The easiest way to create a new service for OpenTwin is to use the service template provided with the OpenTwin platform. The template is located at “OpenTwin/Templates/ServiceTemplate”.

    #. Copy the whole folder to a directory of your choice
    #. Rename the folder to your desired project name
    #. Rename the project files to the same name of the project folder

       #. `*.sln`
       
       #. `*.vcxproj`
       
       #. `*.vcxproj.filter`
       
       #. `*.vcxproj.user`

    #. If the project folder contains the folder “.vs” and/or “x64”, delete them with all their contents
    #. Create a new environment variable for the service in the Scripts/SetupEnvironment.bat file
    #. Replace the project path in all batch files so that the new environment variable is used
    #. Now you can open the project by running the edit.bat file.
    #. Create new macros for the service in OpenTwinCommunication/ActionTypes.h.
       In the file “Application.cpp” add the service name and service type to the ot::ApplicationBase constructor call at the top of the file. For the service type you can use one of the OpenTwin service type makros: OT_INFO_SERVICE_TYPE_<Type> or you can type in any other type.
    #. Build the project
    #. In some cases it is necessary to restart the IDE and build the project again to get all syntax highlighting right

Understanding the classes
^^^^^^^^^^^^^^^^^^^^^^^^^

The OpenTwin ServiceTemplate is based on the OpenTwin ServiceFoundation.
The ServiceFoundation library realises all internal functionallity of a OpenTwin service. 

Application
"""""""""""

The central class of the Service is the Application class.
This class receives all important callbacks and can be used to write down the central functionallity of the service.
The virutal functions marked with override must all be implemented since the functions are pure virtual in the parent class.

dllmain
"""""""""""

The dllmain.cpp file contains the startup code for the service.
If modifying the file, ensure that the whole functionallity of the startup is still given.

UiComponent
"""""""""""

The UiComponent is created in the service foundation.
The UiComponent provides all currently implemented features for creating controls in the UI.
An instance of the component will be provided whenever the UI connects or disconnects to the session, so the logic of adding controls can be implemented directly in this call.

Create a tool menu
""""""""""""""""""
To create a menu in the UI toolbar first a menu page must be created. In a menu page multiple groups can be added, also a group can have multiple subgroups.
After creating the page(s), group(s) and subgroup(s) controls can be added to groups or subgroups.
