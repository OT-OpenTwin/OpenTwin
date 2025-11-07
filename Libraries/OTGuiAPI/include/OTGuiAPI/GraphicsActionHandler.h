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
#include "OTCommunication/ActionHandler.h"
#include "OTGuiAPI/OTGuiAPIAPIExport.h"

namespace ot {

	class OT_GUIAPI_API_EXPORT GraphicsActionHandler {
		OT_DECL_NOCOPY(GraphicsActionHandler)
		OT_DECL_NOMOVE(GraphicsActionHandler)
	public:
		// ###########################################################################################################################################################################################################################################################################################################################

		// Static: Document creators

		static JsonDocument createItemRequestedDocument(const std::string& _viewName, const std::string& _itemName, const ot::Point2DD& _pos, const GuiEvent& _eventData);

		static JsonDocument createItemChangedDocument(const ot::GraphicsItemCfg* _item, const GuiEvent& _eventData);

		static JsonDocument createItemDoubleClickedDocument(const std::string& _name, ot::UID _uid, const GuiEvent& _eventData);

		static JsonDocument createConnectionRequestedDocument(const ot::GraphicsConnectionPackage& _connectionData, const GuiEvent& _eventData);

		static JsonDocument createConnectionToConnectionRequestedDocument(const ot::GraphicsConnectionPackage& _connectionData, const ot::Point2DD& _pos, const GuiEvent& _eventData);

		static JsonDocument createConnectionChangedDocument(const ot::GraphicsConnectionCfg& _connectionData, const GuiEvent& _eventData);

		static JsonDocument createChangeEventDocument(const ot::GraphicsChangeEvent& _changeEvent);

		GraphicsActionHandler(ActionDispatcherBase* _dispatcher = &ot::ActionDispatcher::instance());
		virtual ~GraphicsActionHandler() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Callbacks

	protected:

		//! @brief Is called when the addition of a new graphics item is requested.
		//! @param _document The original document of the request.
		//! @param _viewName The name of the view the item should be added to.
		//! @param _itemName The name of the item to be added.
		//! @param _pos The position where the item should be added.
		//! @param _forceHandle If true, the handler must process the event even if the corresponding entity DataHandle callback is set.
		virtual ot::ReturnMessage graphicsItemRequested(const std::string& _viewName, const std::string& _itemName, const ot::Point2DD& _pos, const ot::GuiEvent& _eventData) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

		//! @brief Is called when a graphics item has changed.
		//! @param _document The original document of the request.
		//! @param _item The configuration of the changed item.
		//! @param _forceHandle If true, the handler must process the event even if the corresponding entity DataHandle callback is set.
		virtual ot::ReturnMessage graphicsItemChanged(const ot::GraphicsItemCfg* _item, const ot::GuiEvent& _eventData) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

		//! @brief Is caled when a item was double clicked.
		//! @param _document The original document of the request.
		//! @param _name The name of the item that was double clicked.
		//! @param _uid The uid of the item that was double clicked.
		//! @param _forceHandle If true, the handler must process the event even if the corresponding entity DataHandle callback is set.
		virtual ot::ReturnMessage graphicsItemDoubleClicked(const std::string& _name, ot::UID _uid, const ot::GuiEvent& _eventData) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

		//! @brief Is called when a new connection between two items is requested.
		//! @param _document The original document of the request.
		//! @param _connectionData Information about the new connection.
		//! @param _forceHandle If true, the handler must process the event even if the corresponding entity DataHandle callback is set.
		virtual ot::ReturnMessage graphicsConnectionRequested(const ot::GraphicsConnectionPackage& _connectionData, const ot::GuiEvent& _eventData) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

		//! @brief Is called when a connection to an existing connection is requested.
		//! @param _document The original document of the request.
		//! @param _connectionData Information about the new connection.
		//! @param _pos The position where the connection to connection was requested.
		//! @param _forceHandle If true, the handler must process the event even if the corresponding entity DataHandle callback is set.
		virtual ot::ReturnMessage graphicsConnectionToConnectionRequested(const ot::GraphicsConnectionPackage& _connectionData, const ot::Point2DD& _pos, const ot::GuiEvent& _eventData) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

		//! @brief Is called when a graphics connection has changed.
		//! @param _document The original document of the request.
		//! @param _connectionData Information about the changed connection.
		//! @param _forceHandle If true, the handler must process the event even if the corresponding entity DataHandle callback is set.
		virtual ot::ReturnMessage graphicsConnectionChanged(const ot::GraphicsConnectionCfg& _connectionData, const ot::GuiEvent& _eventData) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };
		
		//! @brief Is called when a graphics change event occurs.
		//! @param _changeEvent The change event containing all changes.
		//! @param _forceHandle If true, the handler must process the event even if the corresponding entity DataHandle callback is set.
		virtual ot::ReturnMessage graphicsChangeEvent(const ot::GraphicsChangeEvent& _changeEvent) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Handlers

	private:
		ActionHandler m_actionHandler;

		ReturnMessage handleGraphicsItemRequested(JsonDocument& _document);
		ReturnMessage handleGraphicsItemChanged(JsonDocument& _document);
		ReturnMessage handleGraphicsItemDoubleClicked(JsonDocument& _document);

		ReturnMessage handleGraphicsConnectionRequested(JsonDocument& _document);
		ReturnMessage handleGraphicsConnectionToConnectionRequested(JsonDocument& _document);
		ReturnMessage handleGraphicsConnectionChanged(JsonDocument& _document);

		ReturnMessage handleGraphicsChangeEvent(JsonDocument& _document);
	};

}