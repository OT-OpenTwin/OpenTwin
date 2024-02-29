Properties
===========

Concept
-------

The properties may be used in various use cases.
The property classes in the ``OTGui`` library should be used to create the property structure which then may be send to the UI.
The property classes in the ``OTWidgets`` library are used by the UI to display the corresponding inputs (e.g. ``PropertyInputInt``) that may be displayed in dialogs or other widgets (e.g. :ref:`PropertyDialog`, :ref:`OnePropertyDialog`).

.. _OnePropertyDialog:

OnePropertyDialog
-----------------

The ``OnePropertyDialog`` can be requested from any service by sending the ``OT_ACTION_CMD_UI_OnePropertyDialog`` action to the UI.
The key difference to the :ref:`PropertyDialog` is that only one property will be edited by the user.
The dialog will have the ``Property::propertyTitle()`` in a lable that is followed by the corresponding input depending on the property (e.g. ``LineEdit`` for a ``PropertyString``).

Request
^^^^^^^

Provide a ``OTGui/OnePropertyDialogCfg`` which contains one ``OTGui/Property``.

.. code-block:: c++

    // Application.cpp

    #include "Application.h"

    void Application::foo(void) {
	    // Create json document
	    ot::JsonDocument doc;
	    doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_OnePropertyDialog, doc.GetAllocator()), doc.GetAllocator());
         
	    // Create property to edit
	    ot::PropertyInt* prop = new ot::PropertyInt;
	    prop->setPropertyName("<name>");                 // Is not relevant for the OnePropertyDialog
	    prop->setPropertyTitle("<title>");               // Title that will be displayed to the user
	    prop->setRange(1, 99);                           // Value range for the user input
	    prop->setValue(2);                               // Inital value
         
	    // Create dialog configuration
	    ot::OnePropertyDialogCfg cfg(prop);
	    cfg.setName("MyIntegerDialog");                  // Dialog name that may be used to identify the dialog (e.g. when the service wants to request different dialogs)
	    cfg.setTitle("MyIntegerDialogTitle");            // Title that will be displayed to the user
	    cfg.setFlags(ot::DialogCfg::CancelOnEqualValue); // If set and the input remained unchaned the dialog will behave as if the user pressed cancel when confirm is pressed
         
	    // Serialize
	    ot::JsonObject cfgObj;
	    cfg.addToJsonObject(cfgObj, doc.GetAllocator());
	    doc.AddMember(OT_ACTION_PARAM_Config, cfgObj, doc.GetAllocator());
         
	    // Add service information
	    this->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());
	    // or use "Application::instance()->" instead of "this" when using outside of class Application
         
	    // Send
	    m_uiComponent->sendMessage(true, doc);
         
	    //--- OR ---
	    std::string response; // Response will be empty when calling queue
	    if (!ot::msg::send("<sender url>", "<UI url>", ot::QUEUE, doc.toJson(), response)) {
		    // Error handling
	    }
    }

Response
^^^^^^^^

The UI will respond by sending the ``OT_ACTION_CMD_UI_OnePropertyDialogValue`` action.
The action only will be send if the user confirmes the dialog (and the data has changed if the 'DialogCfg::CancelOnEqualValue' flag is set)

First a ``OT_HANDLER`` to the ``Application`` class.

.. code-block:: c++
    
    // Application.h

    #include "OTServiceFoundation/ApplicationBase.h"

    class Application : ot::ApplicationBase {
        
        OT_HANDLER(handleOnePropertyDialogValue, Application, OT_ACTION_CMD_UI_OnePropertyDialogValue, ot::SECURE_MESSAGE_TYPES);
        
    };

Then process the response from the UI.

.. code-block:: c++

    // Application.cpp

    #include "Application.h"

    std::string Application::handleOnePropertyDialogValue(ot::JsonDocument& _document) {
	    // Get the dialog name that was set when requesting the dialog (DialogCfg::setName())
	    std::string dialogName = ot::json::getString(_document, OT_ACTION_PARAM_ObjectName);

	    // Check the dialog name
	    if (dialogName == "MyIntegerDialog") {

		    // Get the value, in this example the value type is integer
		    int value = ot::json::getInt(_document, OT_ACTION_PARAM_Value);

		    m_uiComponent->displayMessage("\"MyIntegerDialog\" resulted with value: " + std::to_string(value));
	    }
	    return std::string();
    }

.. _PropertyDialog:

PropertyDialog
^^^^^^^^^^^^^^

x