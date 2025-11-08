// @otlicense
// File: GraphicsActionHandler.h
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
#include "OTGui/GuiEvent.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsChangeEvent.h"
#include "OTGui/GraphicsItemDropEvent.h"
#include "OTGui/GraphicsDoubleClickEvent.h"
#include "OTGui/GraphicsConnectionDropEvent.h"
#include "OTCommunication/ActionHandler.h"
#include "OTGuiAPI/OTGuiAPIAPIExport.h"

namespace ot {

	class OT_GUIAPI_API_EXPORT GraphicsActionHandler {
		OT_DECL_NOCOPY(GraphicsActionHandler)
		OT_DECL_NOMOVE(GraphicsActionHandler)
	public:
		// ###########################################################################################################################################################################################################################################################################################################################

		// Static: Document creators

		static JsonDocument createItemRequestedDocument(const GraphicsItemDropEvent& _eventData);

		static JsonDocument createItemDoubleClickedDocument(const GraphicsDoubleClickEvent& _eventData);

		static JsonDocument createConnectionRequestedDocument(const GraphicsConnectionDropEvent& _eventData);

		static JsonDocument createChangeEventDocument(const ot::GraphicsChangeEvent& _changeEvent);

		GraphicsActionHandler(ActionDispatcherBase* _dispatcher = &ot::ActionDispatcher::instance());
		virtual ~GraphicsActionHandler() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Callbacks

	protected:

		//! @brief Is called when the addition of a new graphics item is requested.
		//! @param _eventData Contains all information about the item request.
		virtual ot::ReturnMessage graphicsItemRequested(const GraphicsItemDropEvent& _eventData) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

		//! @brief Is caled when a item was double clicked.
		//! @param _eventData Contains all information about the double click event.
		virtual ot::ReturnMessage graphicsItemDoubleClicked(const GraphicsDoubleClickEvent& _eventData) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

		//! @brief Is called when a new connection between two items is requested.
		//! @param _eventData Contains all information about the connection request.
		virtual ot::ReturnMessage graphicsConnectionRequested(const GraphicsConnectionDropEvent& _eventData) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

		//! @brief Is called when a graphics change event occurs.
		//! @param _changeEvent Contains all information about the change event.
		virtual ot::ReturnMessage graphicsChangeEvent(const ot::GraphicsChangeEvent& _changeEvent) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Handlers

	private:
		ActionHandler m_actionHandler;

		ReturnMessage handleGraphicsItemRequested(JsonDocument& _document);
		ReturnMessage handleGraphicsItemDoubleClicked(JsonDocument& _document);

		ReturnMessage handleGraphicsConnectionRequested(JsonDocument& _document);

		ReturnMessage handleGraphicsChangeEvent(JsonDocument& _document);
	};

}