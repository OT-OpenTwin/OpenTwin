// @otlicense
// File: BlockHandler.h
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
#include "OTGuiAPI/GraphicsActionHandler.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"

class EntityBlock;
class EntityGraphicsScene;
class EntityBlockConnection;

class BlockHandler : public BusinessLogicHandler, public ot::GraphicsActionHandler {
	OT_DECL_NOCOPY(BlockHandler)
	OT_DECL_NOMOVE(BlockHandler)
public:
	BlockHandler() = default;
	~BlockHandler() = default;

protected:
	virtual ot::ReturnMessage graphicsItemRequested(const ot::GraphicsItemDropEvent& _eventData) override;
	virtual ot::ReturnMessage graphicsItemDoubleClicked(const ot::GraphicsDoubleClickEvent& _eventData) override;

	virtual ot::ReturnMessage graphicsConnectionRequested(const ot::GraphicsConnectionDropEvent& _eventData) override;

	virtual ot::ReturnMessage graphicsChangeEvent(const ot::GraphicsChangeEvent& _changeEvent) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

private:
	//! @brief Handles the connection of one block to another block.
	//! @param _scene The graphics scene in which the connection is being made.
	//! @param _originBlock The block from which the connection originates.
	//! @param _destinationBlock The block to which the connection is made.
	//! @param _eventData The event data associated with the connection drop event.
	//! @return True if the connection was handled successfully, false otherwise.
	bool createBlockToBlockConnection(EntityGraphicsScene* _scene, EntityBlock* _originBlock, EntityBlock* _destinationBlock, const ot::GraphicsConnectionDropEvent& _eventData);

	//! @brief Handles the connection of a block to another connection.
	//! @param _scene The graphics scene in which the connection is being made.
	//! @param _originBlock The block from which the connection originates.
	//! @param _destinationConnection The connection to which the block is being connected.
	//! @param _eventData The event data associated with the connection drop event.
	//! @param _connectionReversed If true, the origin of the event data is considered the destination and vice versa.
	//! @return True if the connection was handled successfully, false otherwise.
	bool createBlockToConnectionConnection(EntityGraphicsScene* _scene, EntityBlock* _originBlock, EntityBlockConnection* _destinationConnection, const ot::GraphicsConnectionDropEvent& _eventData, bool _connectionReversed);

	//! @brief Handles the change of a block.
	//! @param _changedBlock The block that changed.
	//! @param _changeEvent The event data associated with the changed event.
	//! @return True if theblock change was handled successfully, false otherwise.
	bool updateBlock(const ot::GraphicsItemCfg* _changedBlock, const ot::GraphicsChangeEvent& _changeEvent);

	//! @brief Handles the change of a connection.
	//! @param _changedConnection The connection that changed.
	//! param _changeEvent The event data associated with the changed event.
	//! @return True if the connection change was handled successfully, false otherwise.
	bool updateConnection(const ot::GraphicsConnectionCfg& _changedConnection, const ot::GraphicsChangeEvent& _changeEvent);

	//! @brief Creates a new block entity based on the provided parameters.
	//! @param _newModelStateInfo Information about the new model state.
	//! @param _itemName The name of the item.
	//! @param _scenePos The position in the scene where the block should be created.
	//! @param _editor The graphics scene editor.
	//! @return A unique pointer to the created block entity.
	std::shared_ptr<EntityBlock> createBlockEntity(ot::NewModelStateInfo& _newModelStateInfo, const std::string& _itemName, const ot::Point2DD& _scenePos, EntityGraphicsScene* _editor);

	//! @brief Creates a new connection entity based on the provided parameters.
	//! @param _newModelStateInfo Information about the new model state.
	//! @param _requestedConnection The connection that is requested.
	//! @param _originBlock The block from which the connection originates.
	//! @param _connectionNaming The naming convention for the requested connection.
	//! @return void 
	void createConnection(ot::NewModelStateInfo& _newModelStateInfo, EntityGraphicsScene* scene, ot::GraphicsConnectionCfg& _requestedConnection, EntityBlock* _originBlock, EntityNamingBehavior& _connectionNaming);

	std::map<ot::UID, std::map<ot::UID, ot::UIDList>> m_viewBlockConnectionsMap;
	const std::string m_connectionsFolder = "Connections";

};