Text Editor
===========

Create Editor
^^^^^^^^^^^^^

A text editor can be created by sending the ``OT_ACTION_CMD_UI_TEXTEDITOR_SetText`` action to the UI.
Note that calling SetText for an existing editor will set the editors contents changed flag to false.

.. code-block:: c++

    void Application::foo(void) {
        // We assume that Application inherits from ot::ApplicationBase (OTServiceFoundation/ApplicationBase.h)

        // Get the editor name and the current text
        std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Name);
        std:string editorText = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Text);

        // ...

        // Create response document
        ot::JsonDocument doc;
        this->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

        // Add the "SetSaved" action to notify the editor that the changes have been saved
        doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_TEXTEDITOR_SetSaved, doc.GetAllocator()), doc.GetAllocator());
        doc.AddMember(OT_ACTION_PARAM_TEXTEDITOR_Name, ot::JsonString(editorName, doc.GetAllocator()), doc.GetAllocator());

        // Now we can send the request
    }

Save Requested Callback
^^^^^^^^^^^^^^^^^^^^^^^

Since a callback on every text change would cause too many messages beeing sent, the editor will send a save request whenever the user activates the save "Ctrl+S" shortcut.
To receive the callbacks simply add a handler to the application class.

.. code-block:: c++
    
    class Application : public ot::ApplicationBase {
    
        ...

    public:
        OT_HANDLER(handleEditorSaveRequested, Application, OT_ACTION_CMD_UI_TEXTEDITOR_SaveRequest, ot::SECURE_MESSAGE_TYPES);

        ...
    };

When the handler is called, the provided document holds the editor name and text.

.. code-block:: c++

    std::string Application::handleEditorSaveRequested(ot::JsonDocument& _document) {
        // We assume that Application inherits from ot::ApplicationBase (OTServiceFoundation/ApplicationBase.h)

        // Get the editor name and the current text
	    std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Name);
	    std::string editorText = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Text);

	    // ...

	    // Create response document
	    ot::JsonDocument doc;
	    this->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

	    // Add the "SetSaved" action to notify the editor that the changes have been saved
	    doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_TEXTEDITOR_SetSaved, doc.GetAllocator()), doc.GetAllocator());

	    // Send the request
	    std::string response;
        std::string req = doc.toJson();
        if (!ot::msg::send("", m_uiComponent->serviceURL(), ot::QUEUE, req, response)) {
            return OT_ACTION_RETURN_VALUE_FAILED;
        }

	    return OT_ACTION_RETURN_VALUE_OK;
    }