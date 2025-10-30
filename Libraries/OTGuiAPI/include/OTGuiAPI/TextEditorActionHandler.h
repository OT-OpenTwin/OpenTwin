// @otlicense
// File: TextEditorActionHandler.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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