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

//std header
#include <set>

// OpenTwin header
#include "OTGuiAPI/GraphicsActionHandler.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "NewModelStateInfo.h"

class EntityBlock;
class EntityGraphicsScene;
class EntityBlockConnection;

class BlockHandler : public BusinessLogicHandler, public ot::GraphicsActionHandler {
	OT_DECL_NOCOPY(BlockHandler)
	OT_DECL_NOMOVE(BlockHandler)
public:
	BlockHandler() = default;
	~BlockHandler() = default;

	//Fill map function
	void processEntity(EntityBase* entBase);

	// Getter
	const std::map<ot::UID, ot::UIDList>& getBlocksForEditor(ot::UID editorId) const;
	ot::UIDList& getConnections(ot::UID editorId, ot::UID blockId);

	// Setter
	void addConnection(ot::UID editorId, EntityBlockConnection& _toBeAddedConnection);
	void addBlock(ot::UID editorId, const EntityBlock* _block);
	void addEditor(const EntityGraphicsScene* _editor);

	// Remover
	void removeFromMap(EntityBase* entBase);
	void entityRemoved(EntityBase* _entity, const std::list<EntityBase*>& _otherEntitiesToRemove);
	void removeConnectionIfUnsnapped(EntityGraphicsScene* _editor, EntityBlockConnection* _connectionEntity, const ot::GraphicsConnectionCfg& _changedConnection);
	void clearMap();

	// Finder
	bool blockExists(ot::UID _blockID);
	bool blockExists(ot::UID _editorID, ot::UID _blockID);
	EntityGraphicsScene* findGraphicsScene(const std::string& _graphicsElementName);

	void getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const;

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
	std::unique_ptr<EntityBlock> createBlockEntity(EntityGraphicsScene* _editor, const ot::Point2DD& _scenePos, const std::string& _itemName,  ot::NewModelStateInfo& _newModelStateInfo);

	//! @brief Creates a new connection entity based on the provided parameters.
	//! @param _newModelStateInfo Information about the new model state.
	//! @param _requestedConnection The connection that is requested.
	//! @param _originBlock The block from which the connection originates.
	//! @param _connectionNaming The naming convention for the requested connection.
	//! @return void 
	void createConnection(EntityGraphicsScene* scene, EntityBlock* _originBlock, ot::GraphicsConnectionCfg& _requestedConnection,  EntityNamingBehavior& _connectionNaming, ot::NewModelStateInfo& _newModelStateInfo);

	//! @brief Modifies the connection information for a given entity.
	//! @param _connectionID Connection ID of the connection to be modified.
	//! @param _connectedBlockEntity A pointer to the block entity that is connected to the entity
	//! @return void
	void modifyConnection(const ot::UID& _connectionID, EntityBase* _connectedBlockEntity);

	//! @brief Handles the snapping of connections in the graphics scene.
	//! @param _scene The graphics scene where the snapping event occurred.
	//! @param _snapEvent The snap event containing details about the snapping action.
	//! @return True if the snapping was handled successfully, false otherwise.
	bool snapConnection(EntityGraphicsScene* _scene, const ot::GraphicsChangeEvent::SnapInfo& _snapInfo, ot::GraphicsConnectionCfg& _connectionCfg , std::set<EntityBlockConnection*>& _processedConnections);

	//! @brief Maps scenes to a map of blocks and their connections.
	std::map<ot::UID, std::map<ot::UID, ot::UIDList>> m_viewBlockConnectionsMap;
	const std::string m_connectionsFolder = "Connections";

};