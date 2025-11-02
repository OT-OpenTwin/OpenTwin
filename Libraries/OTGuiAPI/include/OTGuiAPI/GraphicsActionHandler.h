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
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsChangeEvent.h"
#include "OTCommunication/ActionHandler.h"
#include "OTGuiAPI/OTGuiAPIAPIExport.h"

namespace ot {

	class OT_GUIAPI_API_EXPORT GraphicsActionHandler {
		OT_DECL_NOCOPY(GraphicsActionHandler)
		OT_DECL_NOMOVE(GraphicsActionHandler)
	public:
		GraphicsActionHandler(ActionDispatcherBase* _dispatcher = &ot::ActionDispatcher::instance());
		virtual ~GraphicsActionHandler() = default;

	protected:

		//! @brief Is called when the addition of a new graphics item is requested.
		//! @param _document The original document of the request.
		//! @param _viewName The name of the view the item should be added to.
		//! @param _itemName The name of the item to be added.
		//! @param _pos The position where the item should be added.
		virtual ot::ReturnMessage graphicsItemRequested(const std::string& _viewName, const std::string& _itemName, const ot::Point2DD& _pos) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

		//! @brief Is called when a graphics item has changed.
		//! @param _document The original document of the request.
		//! @param _item The configuration of the changed item.
		virtual ot::ReturnMessage graphicsItemChanged(const ot::GraphicsItemCfg* _item) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

		//! @brief Is caled when a item was double clicked.
		//! @param _document The original document of the request.
		//! @param _name The name of the item that was double clicked.
		//! @param _uid The uid of the item that was double clicked.
		virtual ot::ReturnMessage graphicsItemDoubleClicked(const std::string& _name, ot::UID _uid) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

		//! @brief Is called when the removal of items and/or connections is requested.
		//! @param _document The original document of the request.
		//! @param _itemUids The uids of the items to be removed.
		//! @param _connectionUids The uids of the connections to be removed.
		virtual ot::ReturnMessage graphicsRemoveRequested(const ot::UIDList& _itemUids, const ot::UIDList& _connectionUids) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

		//! @brief Is called when a new connection between two items is requested.
		//! @param _document The original document of the request.
		//! @param _connectionData Information about the new connection.
		virtual ot::ReturnMessage graphicsConnectionRequested(const ot::GraphicsConnectionPackage& _connectionData) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

		//! @brief Is called when a connection to an existing connection is requested.
		//! @param _document The original document of the request.
		//! @param _connectionData Information about the new connection.
		//! @param _pos The position where the connection to connection was requested.
		virtual ot::ReturnMessage graphicsConnectionToConnectionRequested(const ot::GraphicsConnectionPackage& _connectionData, const ot::Point2DD& _pos) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

		//! @brief Is called when a graphics connection has changed.
		//! @param _document The original document of the request.
		//! @param _connectionData Information about the changed connection.
		virtual ot::ReturnMessage graphicsConnectionChanged(const ot::GraphicsConnectionCfg& _connectionData) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };
		
		//! @brief Is called when a graphics change event occurs.
		//! @param _changeEvent The change event containing all changes.
		virtual ot::ReturnMessage graphicsChangeEvent(const ot::GraphicsChangeEvent& _changeEvent) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); };

	private:
		ActionHandler m_actionHandler;

		ReturnMessage handleGraphicsItemRequested(JsonDocument& _document);
		ReturnMessage handleGraphicsItemChanged(JsonDocument& _document);
		ReturnMessage handleGraphicsItemDoubleClicked(JsonDocument& _document);

		ReturnMessage handleGraphicsRemoveRequested(JsonDocument& _document);

		ReturnMessage handleGraphicsConnectionRequested(JsonDocument& _document);
		ReturnMessage handleGraphicsConnectionToConnectionRequested(JsonDocument& _document);
		ReturnMessage handleGraphicsConnectionChanged(JsonDocument& _document);

		ReturnMessage handleGraphicsChangeEvent(JsonDocument& _document);
	};

}