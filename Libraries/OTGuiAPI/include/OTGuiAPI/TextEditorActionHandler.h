//! @file TextEditorActionHandler.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

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

	protected:
		virtual void textEditorSaveRequested(const std::string& _entityName, const std::string& _content) {};

	private:
		ActionHandler m_actionHandler;

		void handleTextEditorSaveRequested(JsonDocument& _doc);
	};

}