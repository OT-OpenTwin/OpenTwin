// @otlicense
// File: ContextMenuHandler.h
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
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/Handler/ActionHandler.h"
#include "OTGuiAPI/OTGuiAPIAPIExport.h"

// std header
#include <functional>
#include <unordered_map>

namespace ot {

	class MenuCfg;
	class ContextRequestData;
	class ContextMenuRequestEvent;

	class ActionHandleConnector;

	//! @brief The CheckBoxHandler class is used to handle check box changed events.
	//! If no handler is registered for a event the corresponding virtual function will be called.
	class OT_GUIAPI_API_EXPORT ContextMenuHandler
	{
		OT_DECL_NOCOPY(ContextMenuHandler)
		OT_DECL_NOMOVE(ContextMenuHandler)
	public:
		explicit ContextMenuHandler(ActionDispatcherBase* _dispatcher = &ot::ActionDispatcher::instance());
		virtual ~ContextMenuHandler() = default;

		//! @brief Create a JSON document for a context menu request event.
		//! @param _eventData Data for the context menu request event. The method takes ownership of the provided pointer and will delete it after use.
		static JsonDocument createContextMenuRequestedDocument(ContextRequestData* _eventData);
		static JsonDocument createContextMenuRequestedDocument(const ContextMenuRequestEvent& _eventData);

	protected:

		OT_DECL_NODISCARD virtual MenuCfg contextMenuRequested(const ContextMenuRequestEvent& _event);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Handler

	private:
		ActionHandler m_actionHandler;

		ReturnMessage handleContextMenuRequested(JsonDocument& _document);

	};
}
