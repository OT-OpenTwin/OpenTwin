// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCommunication/ActionHandler.h"
#include "OTGuiAPI/OTGuiAPIAPIExport.h"

namespace ot {

	class OT_GUIAPI_API_EXPORT TextEditorActionHandler {
		OT_DECL_NOCOPY(TextEditorActionHandler)
			OT_DECL_NOMOVE(TextEditorActionHandler)
	public:
		TextEditorActionHandler(ActionDispatcherBase* _dispatcher = &ot::ActionDispatcher::instance());
		virtual ~TextEditorActionHandler() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public: Action document helper

		static ot::JsonDocument createTextEditorSaveRequestDocument(const std::string& _entityName, const std::string& _content);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Callbacks

	protected:

		//! @brief Callback that is triggered when a text editor save is requested.
		//! @param _entityName Name of the text editor entity.
		//! @param _content The content to be saved.
		//! @return ReturnMessage indicating success or failure of the save operation.
		//! A return status of type OK will result in the text editor modified flag being cleared.
		virtual ReturnMessage textEditorSaveRequested(const std::string& _entityName, const std::string& _content) { return ot::ReturnMessage(ot::ReturnMessage::Failed, "Unhandled save request"); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Action handling

	private:
		ActionHandler m_actionHandler;

		ReturnMessage handleTextEditorSaveRequested(JsonDocument& _doc);
	};

}