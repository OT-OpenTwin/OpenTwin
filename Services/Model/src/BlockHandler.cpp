// @otlicense
// File: BlockHandler.cpp
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

// std header
#include <stdafx.h>
#include <queue>

// Service header
#include "Model.h"
#include "Application.h"
#include "BlockHandler.h"

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/FolderNames.h"
#include "OTCore/LogDispatcher.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTCommunication/ActionTypes.h"
#include "EntityBlock.h"
#include "EntityBlockConnection.h"
#include "EntityBlockPython.h"
#include "EntityGraphicsScene.h"
#include "EntityBlockCircuitElement.h"
#include "EntityBlockCircuitConnector.h"
#include "PropertyHelper.h"
#include "OTCore/EntityName.h"

void BlockHandler::processEntity(EntityBase* _entBase) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);

	if (_entBase == nullptr) {
		OT_LOG_E("EntityBase pointer is null");
		return;
	}
	
	EntityGraphicsScene* entGraphicsScene = dynamic_cast<EntityGraphicsScene*>(_entBase);
	if (entGraphicsScene) {
		addEditor(entGraphicsScene);
		return;
	}

	EntityBlock* entBlock = dynamic_cast<EntityBlock*>(_entBase);
	if (entBlock) {
		EntityGraphicsScene* editor = findGraphicsScene(entBlock->getName());
		if (!editor) {
			OT_LOG_E("Failed to determine EntityGraphicsScene for block { \"BlockID\": " + std::to_string(entBlock->getEntityID()) + " }");
			return;
		}
		addBlock(editor->getEntityID(), entBlock);
		return;
	}

	EntityBlockConnection* entBlockConnection = dynamic_cast<EntityBlockConnection*>(_entBase);
	if (entBlockConnection) {
		EntityGraphicsScene* editor = findGraphicsScene(entBlockConnection->getName());
		if (!editor) {
			OT_LOG_E("Failed to determine EntityGraphicsScene for connection { \"ConnectionID\": " + std::to_string(entBlockConnection->getEntityID()) + " }");
			return;
		}
		addConnection(editor->getEntityID(), *entBlockConnection);
		return;
	}
}

const std::map<ot::UID, ot::UIDList>& BlockHandler::getBlocksForEditor(ot::UID editorId) const {
	static const std::map<ot::UID, ot::UIDList> emptyMap{};
	auto it = m_viewBlockConnectionsMap.find(editorId);
	if (it != m_viewBlockConnectionsMap.end()) {
		return it->second;
	}
	return emptyMap;
}

ot::UIDList& BlockHandler::getConnections(ot::UID editorId, ot::UID blockId) {
	static ot::UIDList emptyList{}; 
	auto itEditor = m_viewBlockConnectionsMap.find(editorId);
	if (itEditor != m_viewBlockConnectionsMap.end()) {
		auto itBlock = itEditor->second.find(blockId);
		if (itBlock != itEditor->second.end()) {
			return itBlock->second; 
		}
	}
	return emptyList; 
}

void BlockHandler::addConnection(ot::UID editorId, EntityBlockConnection& _toBeAddedConnection) {
	ot::UID originUid = _toBeAddedConnection.getConnectionCfg().getOriginUid();
	ot::UID destUid = _toBeAddedConnection.getConnectionCfg().getDestinationUid();
	ot::UID connectionId = _toBeAddedConnection.getEntityID();

	auto& blocks = m_viewBlockConnectionsMap[editorId];

	if (originUid != ot::invalidUID) {
		auto& connectionList = blocks[originUid];
		if (std::find(connectionList.begin(), connectionList.end(), connectionId) == connectionList.end()) {
			connectionList.push_back(connectionId);
		}
	}

	if (destUid != ot::invalidUID) {
		auto& connectionList = blocks[destUid];
		if (std::find(connectionList.begin(), connectionList.end(), connectionId) == connectionList.end()) {
			connectionList.push_back(connectionId);
		}
	}
}

void BlockHandler::addBlock(ot::UID editorId, const EntityBlock* _block) {
	auto& blocksMap = m_viewBlockConnectionsMap[editorId];
	blocksMap.emplace(_block->getEntityID(), ot::UIDList{});
}

void BlockHandler::addEditor(const EntityGraphicsScene* _editor) {
	m_viewBlockConnectionsMap.emplace(_editor->getEntityID(), std::map<ot::UID, ot::UIDList>{});
}

void BlockHandler::removeFromMap(EntityBase* _entBase) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);
	
	if (_entBase->getEntityID() == ot::invalidUID) {
		OT_LOG_E("Invalid UID");
		return;
	}

	// Check if editor with id exists - if yes remove the whole entry
	EntityGraphicsScene* entGraphicsScene = dynamic_cast<EntityGraphicsScene*>(_entBase);
	if (entGraphicsScene) {
		auto itEditor = m_viewBlockConnectionsMap.find(_entBase->getEntityID());
		if (itEditor != m_viewBlockConnectionsMap.end()) {
			m_viewBlockConnectionsMap.erase(itEditor);
		}
		return;
	}

	// Check if block with id exists - if yes remove the block with its connections
	EntityBlock* entBlock = dynamic_cast<EntityBlock*>(_entBase);
	if (entBlock) {
		for (auto itEditor = m_viewBlockConnectionsMap.begin(); itEditor != m_viewBlockConnectionsMap.end();) {
			auto& blocks = itEditor->second;
			auto itBlock = blocks.find(_entBase->getEntityID());
			if (itBlock != blocks.end()) {
				blocks.erase(itBlock);
				return;
			}
			else {
				++itEditor;
			}
		}
		return;
	}

	// Check if connection with id exists - if yes remove the connection
	EntityBlockConnection* entBlockConnection = dynamic_cast<EntityBlockConnection*>(_entBase);
	if (entBlockConnection) {
		for (auto& [editorId, blocks] : m_viewBlockConnectionsMap) {
			for (auto& [blockId, connections] : blocks) {
				// Looks for connection id in block
				auto itConn = std::find(connections.begin(), connections.end(), entBlockConnection->getEntityID());
				if (itConn != connections.end()) {
				// Erase the connection from the map (only one connection with same UID can be related to the same block
					connections.erase(itConn);
				}
			}
		}
		return;
	}
}

void BlockHandler::entityRemoved(EntityBase* _entityToRemove, const std::list<EntityBase*>& _otherEntitiesToRemove) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);

	if (_entityToRemove->getEntityID() == ot::invalidUID) {
		OT_LOG_E("Invalid UID");
		return;
	}

	EntityBlock* blockToRemove = dynamic_cast<EntityBlock*>(_entityToRemove);
	if (blockToRemove) {
		for (auto itEditor = m_viewBlockConnectionsMap.begin(); itEditor != m_viewBlockConnectionsMap.end();) {
			auto& blocks = itEditor->second;
			auto itBlock = blocks.find(blockToRemove->getEntityID());
			if (itBlock != blocks.end()) {
				ot::UIDList connections = getConnections(itEditor->first, itBlock->first);
				for (const ot::UID& connection : connections) {
					EntityBase* connectionEntity = model->getEntityByID(connection);
					if (!connectionEntity) {
						OT_LOG_E("EditorBlockConnections map is not correct sychronized with entity map { \"SceneID\": " + std::to_string(itEditor->first) + ", \"ConnectionID\": " + std::to_string(connection) + " }");
						return;
					}
					if (std::find(_otherEntitiesToRemove.begin(), _otherEntitiesToRemove.end(), model->getEntityByID(connection)) != _otherEntitiesToRemove.end()) {
						continue;
					}
					modifyConnection(connectionEntity->getEntityID(), _entityToRemove);
					continue;
				}
				return;
			}
			else {
				++itEditor;
			}
		}
		return;
	}
}

void BlockHandler::removeConnectionIfUnsnapped(EntityGraphicsScene* _editor, EntityBlockConnection* _connectionEntity, const ot::GraphicsConnectionCfg& _changedConnection) {

	ot::UID origin = _changedConnection.getOriginUid();
	ot::UID destination = _changedConnection.getDestinationUid();

	for (auto itEditor = m_viewBlockConnectionsMap.begin(); itEditor != m_viewBlockConnectionsMap.end(); ++itEditor) {
		if (itEditor->first != _editor->getEntityID()) {
			continue;
		}
		
		if (origin == ot::invalidUID) {
			auto& blocks = itEditor->second;
			for(auto itBlock = blocks.begin(); itBlock != blocks.end(); ++itBlock) {
				if(itBlock->first != _connectionEntity->getConnectionCfg().getOriginUid()) {
					continue;
				}
				auto& connections = itBlock->second;
				auto itConn = std::find(connections.begin(), connections.end(), _connectionEntity->getEntityID());
				if (itConn != connections.end()) {
					connections.erase(itConn);
					return;
				}
			}
		}
		
		if (destination == ot::invalidUID) {
			auto& blocks = itEditor->second;
			for (auto itBlock = blocks.begin(); itBlock != blocks.end(); ++itBlock) {
				if (itBlock->first != _connectionEntity->getConnectionCfg().getDestinationUid()) {
					continue;
				}
				auto& connections = itBlock->second;
				auto itConn = std::find(connections.begin(), connections.end(), _connectionEntity->getEntityID());
				if (itConn != connections.end()) {
					connections.erase(itConn);
					return;
				}
			}
		}
	}
}

void BlockHandler::clearMap() {
	this->m_viewBlockConnectionsMap.clear();
}

bool BlockHandler::blockExists(ot::UID _blockID) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);

	std::map<ot::UID, EntityBase*>& entityMap = model->getAllEntitiesByUID();
	if (entityMap.find(_blockID) != entityMap.end()) {
		return true;
	}
	return false;
}

bool BlockHandler::blockExists(ot::UID _editorID, ot::UID _blockID) {
	auto itEditor = m_viewBlockConnectionsMap.find(_editorID);
	if (itEditor == m_viewBlockConnectionsMap.end()) {
		OT_LOG_E("Editor not found { \"EntityID\": " + std::to_string(_editorID) + " }");
		return false;
	}

	const auto& blocksMap = itEditor->second;
	auto itBlock = blocksMap.find(_blockID);
	if (itBlock == blocksMap.end()) {
		return false;
	}

	return true;
}

EntityGraphicsScene* BlockHandler::findGraphicsScene(const std::string& _graphicsElementName) {
	std::string parentName = ot::EntityName::getParentPath(_graphicsElementName);
	if(parentName.empty()) {
		OT_LOG_E("Could not extract root name from graphics element { \"Name\": \"" + _graphicsElementName + "\" }");
		return nullptr;
	}
	EntityBase* editorBase = Application::instance()->getModel()->findEntityFromName(parentName);
	EntityGraphicsScene* editor = dynamic_cast<EntityGraphicsScene*>(editorBase);
	if(!editor){
		editor = findGraphicsScene(parentName);
	}
	return editor;
}

void BlockHandler::getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const {
	using namespace ot;
	JsonArray viewMapArr;
	for (const auto& it : m_viewBlockConnectionsMap) {
		JsonObject viewObj;
		viewObj.AddMember("ViewID", it.first, _allocator);
		JsonArray blocksArr;
		for (const auto& blockIt : it.second) {
			JsonObject blockObj;
			blockObj.AddMember("BlockID", blockIt.first, _allocator);
			JsonArray connectionsArr;
			for (const auto& connId : blockIt.second) {
				connectionsArr.PushBack(connId, _allocator);
			}
			blockObj.AddMember("Connections", connectionsArr, _allocator);
			blocksArr.PushBack(blockObj, _allocator);
		}
		viewObj.AddMember("Blocks", blocksArr, _allocator);
		viewMapArr.PushBack(viewObj, _allocator);
	}
	_object.AddMember("ViewBlockConnectionsMap", viewMapArr, _allocator);
}

ot::ReturnMessage BlockHandler::graphicsItemRequested(const ot::GraphicsItemDropEvent& _eventData) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);

	EntityGraphicsScene* editor = dynamic_cast<EntityGraphicsScene*>(model->findEntityFromName(_eventData.getEditorName()));
	if (!editor) {
		OT_LOG_E("Could not find graphics editor entity { \"ViewName\": \"" + _eventData.getEditorName() + "\" }");
		return ot::ReturnMessage::Failed;
	}

	// Check if any service wants to handle this change
	if (!_eventData.isEventFlagSet(ot::GuiEvent::ForceHandle)) {
		std::list<std::string> handlingServices = editor->getServicesForCallback(EntityBase::Callback::DataHandle);
		if (!handlingServices.empty()) {
			ot::GraphicsItemDropEvent fwdEvent(_eventData);
			fwdEvent.setForwarding();
			ot::JsonDocument doc = ot::GraphicsActionHandler::createItemRequestedDocument(fwdEvent);
			Application::instance()->sendMessageAsync(true, handlingServices, doc);
			return ot::ReturnMessage::Ok;
		}
	}

	ot::NewModelStateInfo modelStateInfo;

	std::shared_ptr<EntityBlock> createdBlock = createBlockEntity(editor, _eventData.getScenePos(), _eventData.getItemName(), modelStateInfo);
	if (createdBlock == nullptr) {
		OT_LOG_E("Could not create block entity { \"BlockName\": \"" + _eventData.getItemName() + "\" }");
		return ot::ReturnMessage::Failed;
	}

	model->addEntitiesToModel(modelStateInfo, "Added block", true, true, true);

	// Check if any service wants to be notified about this change
	if (!_eventData.isEventFlagSet(ot::GuiEvent::IgnoreNotify)) {
		std::list<std::string> notifyServices = createdBlock->getServicesForCallback(EntityBase::Callback::DataNotify);
		if (!notifyServices.empty()) {
			ot::GraphicsItemDropEvent fwdEvent(_eventData);
			fwdEvent.setForwarding();
			ot::JsonDocument doc = ot::GraphicsActionHandler::createItemRequestedDocument(fwdEvent);
			Application::instance()->sendMessageAsync(false, notifyServices, doc);
		}
	}

	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage BlockHandler::graphicsItemDoubleClicked(const ot::GraphicsDoubleClickEvent& _eventData) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);

	ot::GraphicsDoubleClickEvent fwdEvent(_eventData);
	fwdEvent.setForwarding();

	const ot::JsonDocument fwdDoc = ot::GraphicsActionHandler::createItemDoubleClickedDocument(fwdEvent);

	EntityBlock* block = dynamic_cast<EntityBlock*>(model->getEntityByID(_eventData.getItemUid()));
	if (!block) {
		OT_LOG_E("Could not find block entity { \"UID\": " + std::to_string(_eventData.getItemUid()) + ", \"Name\": \"" + _eventData.getItemName() + "\" }");
		return ot::ReturnMessage::Failed;
	}

	if (!_eventData.isEventFlagSet(ot::GuiEvent::ForceHandle)) {
		std::list<std::string> handlingServices = block->getServicesForCallback(EntityBase::Callback::DataHandle);
		if (!handlingServices.empty()) {
			Application::instance()->sendMessageAsync(true, handlingServices, fwdDoc);
			return ot::ReturnMessage::Ok;
		}
	}

	if (!_eventData.isEventFlagSet(ot::GuiEvent::IgnoreNotify)) {
		std::list<std::string> notifyServices = block->getServicesForCallback(EntityBase::Callback::DataNotify);
		if (!notifyServices.empty()) {
			Application::instance()->sendMessageAsync(false, notifyServices, fwdDoc);
		}
	}

	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage BlockHandler::graphicsConnectionRequested(const ot::GraphicsConnectionDropEvent& _eventData) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);

	std::string viewName = _eventData.getEditorName();

	EntityGraphicsScene* editor = dynamic_cast<EntityGraphicsScene*>(model->findEntityFromName(viewName));
	if (!editor) {
		OT_LOG_E("Could not find graphics editor entity { \"ViewName\": \"" + viewName + "\" }");
		return ot::ReturnMessage::Failed;
	}

	// Check if any service wants to handle this change
	if (!_eventData.isEventFlagSet(ot::GuiEvent::ForceHandle)) {
		std::list<std::string> handlingServices = editor->getServicesForCallback(EntityBase::Callback::DataHandle);
		if (!handlingServices.empty()) {
			ot::GraphicsConnectionDropEvent fwdEvent(_eventData);
			fwdEvent.setForwarding();
			const ot::JsonDocument doc = ot::GraphicsActionHandler::createConnectionRequestedDocument(fwdEvent);
			Application::instance()->sendMessageAsync(true, handlingServices, doc);
			return ot::ReturnMessage::Ok;
		}
	}

	// Ensure that both origin and destination are set
	if (!_eventData.getConnectionCfg().hasOrigin() || !_eventData.getConnectionCfg().hasDestination()) {
		OT_LOG_D("Ignoring connection request, no origin or destination entity set");
		return ot::ReturnMessage::Ok;
	}

	// Get origin and destination entities
	EntityBase* originEntityBase = model->getEntityByID(_eventData.getConnectionCfg().getOriginUid());
	if (!originEntityBase) {
		OT_LOG_E("Could not find origin entity for connection { \"OriginUID\": " + std::to_string(_eventData.getConnectionCfg().getOriginUid()) + " }");
		return ot::ReturnMessage::Failed;
	}

	EntityBase* destinationEntityBase = model->getEntityByID(_eventData.getConnectionCfg().getDestinationUid());
	if (!destinationEntityBase) {
		OT_LOG_E("Could not find destination entity for connection { \"DestinationUID\": " + std::to_string(_eventData.getConnectionCfg().getDestinationUid()) + " }");
		return ot::ReturnMessage::Failed;
	}

	// Check connection type
	EntityBlockConnection* originConnection = dynamic_cast<EntityBlockConnection*>(originEntityBase);
	if (originConnection)
	{
		EntityBlockConnection* destinationConnection = dynamic_cast<EntityBlockConnection*>(destinationEntityBase);
		if (destinationConnection) {
			// Conenction to connection
			OT_LOG_W("Connection to connection not supported yet");
		}
		else {
			// Block to connection (reversed)
			EntityBlock* destinationBlock = dynamic_cast<EntityBlock*>(destinationEntityBase);
			if (!destinationBlock) {
				OT_LOG_E("Could not cast to EntityBlock { \"EntityUID\": " + std::to_string(destinationEntityBase->getEntityID()) + "\" }");
				return ot::ReturnMessage::Failed;
			}
			if (!createBlockToConnectionConnection(editor, destinationBlock, originConnection, _eventData, true)) {
				OT_LOG_E("Could not handle block to connection (reversed) connection");
				return ot::ReturnMessage::Failed;
			}
		}
	}
	else {
		EntityBlock* originBlock = dynamic_cast<EntityBlock*>(originEntityBase);
		if (!originBlock) {
			OT_LOG_E("Could not cast to EntityBlock { \"EntityUID\": " + std::to_string(originEntityBase->getEntityID()) + "\" }");
			return ot::ReturnMessage::Failed;
		}

		EntityBlockConnection* destinationConnection = dynamic_cast<EntityBlockConnection*>(destinationEntityBase);
		if (destinationConnection) {
			// Block to connection
			if (!createBlockToConnectionConnection(editor, originBlock, destinationConnection, _eventData, false)) {
				OT_LOG_E("Could not handle block to connection connection");
				return ot::ReturnMessage::Failed;
			}
		}
		else {
			// Block to block
			EntityBlock* destinationBlock = dynamic_cast<EntityBlock*>(destinationEntityBase);
			if (!destinationBlock) {
				OT_LOG_E("Could not cast to EntityBlock { \"EntityUID\": " + std::to_string(destinationEntityBase->getEntityID()) + "\" }");
				return ot::ReturnMessage::Failed;
			}
			if (!createBlockToBlockConnection( editor, originBlock, destinationBlock, _eventData)) {
				OT_LOG_E("Could not handle block to block connection");
				return ot::ReturnMessage::Failed;
			}
		}
	}

	// Check if any service wants to be notified about this change
	if (!_eventData.isEventFlagSet(ot::GuiEvent::IgnoreNotify)) {
		std::list<std::string> notifyServices = editor->getServicesForCallback(EntityBase::Callback::DataNotify);
		if (!notifyServices.empty()) {
			ot::GraphicsConnectionDropEvent fwdEvent(_eventData);
			fwdEvent.setForwarding();
			const ot::JsonDocument doc = ot::GraphicsActionHandler::createConnectionRequestedDocument(fwdEvent);
			Application::instance()->sendMessageAsync(false, notifyServices, doc);
		}
	}
	
	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage BlockHandler::graphicsChangeEvent(const ot::GraphicsChangeEvent& _changeEvent) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);


	std::string viewName = _changeEvent.getEditorName();

	EntityGraphicsScene* editor = dynamic_cast<EntityGraphicsScene*>(model->findEntityFromName(viewName));
	if (!editor) {
		OT_LOG_E("Could not find graphics editor entity { \"ViewName\": \"" + viewName + "\" }");
		return ot::ReturnMessage::Failed;
	}

	// Check if any service wants to handle this change
	if (!_changeEvent.isEventFlagSet(ot::GuiEvent::ForceHandle)) {
		std::list<std::string> handlingServices = editor->getServicesForCallback(EntityBase::Callback::DataHandle);
		if (!handlingServices.empty()) {
			ot::GraphicsChangeEvent fwdEvent(_changeEvent);
			fwdEvent.setForwarding();
			ot::JsonDocument doc = ot::GraphicsActionHandler::createChangeEventDocument(fwdEvent);
			Application::instance()->sendMessageAsync(true, handlingServices, doc);
			return ot::ReturnMessage::Ok;
		}
	}


	// Fist handle the block changed requests
	const std::list<ot::GraphicsItemCfg*>& changedItems = _changeEvent.getChangedItems();
	if (!changedItems.empty()) {
		for (const ot::GraphicsItemCfg* item : changedItems) {
			OTAssertNullptr(item);
			if (!updateBlock(item, _changeEvent)) {
				OT_LOG_E("Could not handle block changed event");
				return ot::ReturnMessage::Failed;
			}
		}
	}

	// Now handle the connection changed requests
   const std::list<ot::GraphicsConnectionCfg>& changedConnections = _changeEvent.getChangedConnections();
	if(!changedConnections.empty()) {
		for(const ot::GraphicsConnectionCfg& connectionCfg : changedConnections) {
			if(!updateConnection(connectionCfg, _changeEvent)) {
				OT_LOG_E("Could not handle connection changed event");
				return ot::ReturnMessage::Failed;
			}
		}
	}

	// Now handle the snap requests
	const std::list<ot::GraphicsChangeEvent::SnapInfo>& snapInfos = _changeEvent.getSnapInfos();
	if(!snapInfos.empty()) {
		ot::NewModelStateInfo modelStateInfo;
		std::set<EntityBlockConnection*> processedConnections;
		ot::GraphicsConnectionCfg connectionCfg;
		for (const ot::GraphicsChangeEvent::SnapInfo& snapInfo : snapInfos) {
			if (!snapConnection(editor, snapInfo, connectionCfg, processedConnections)) {
				OT_LOG_E("Could not handle snap event");
				return ot::ReturnMessage::Failed;
			}
		}

		for (EntityBlockConnection* conn : processedConnections) {
			conn->storeToDataBase();
			modelStateInfo.addTopologyEntity(*conn);
		}
		model->addEntitiesToModel(modelStateInfo, "Snapped connections", false, false, true);
	}



	model->setModified();
	model->modelChangeOperationCompleted("Items and connections changed");

	// Check if any service wants to be notified about this change
	if (!_changeEvent.isEventFlagSet(ot::GuiEvent::IgnoreNotify)) {
		std::list<std::string> notifyServices = editor->getServicesForCallback(EntityBase::Callback::DataNotify);
		if (!notifyServices.empty()) {
			ot::GraphicsChangeEvent fwdEvent(_changeEvent);
			fwdEvent.setForwarding();
			ot::JsonDocument doc = ot::GraphicsActionHandler::createChangeEventDocument(fwdEvent);
			Application::instance()->sendMessageAsync(false, notifyServices, doc);
		}
	}

	return ot::ReturnMessage::Ok;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

bool BlockHandler::createBlockToBlockConnection(EntityGraphicsScene* _scene, EntityBlock* _originBlock, EntityBlock* _destinationBlock, const ot::GraphicsConnectionDropEvent& _eventData) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);

	ot::NewModelStateInfo modelStateInfo;
	std::list<bool> forceVis;

	// Get connectionCfg
	ot::GraphicsConnectionCfg connection = _eventData.getConnectionCfg();
	
	if (_originBlock->getClassName() == EntityBlockCircuitConnector::className() ||
		_destinationBlock->getClassName() == EntityBlockCircuitConnector::className()) {

		// Find connectors
		auto originConnectorIt = _originBlock->getAllConnectorsByName().find(connection.getOriginConnectable());
		auto destinationConnectorIt = _destinationBlock->getAllConnectorsByName().find(connection.getDestConnectable());

		if (originConnectorIt == _originBlock->getAllConnectorsByName().end() || destinationConnectorIt == _destinationBlock->getAllConnectorsByName().end()) {
			OT_LOG_E("Could not find origin or destination connector for connection");
			return false;
		}

		// Check connector types
		auto originConnectorType = originConnectorIt->second.getConnectorType();
		auto destinationConnectorType = destinationConnectorIt->second.getConnectorType();

		if ((originConnectorType == ot::ConnectorType::In || originConnectorType == ot::ConnectorType::InOptional) &&
			(destinationConnectorType == ot::ConnectorType::In || destinationConnectorType == ot::ConnectorType::InOptional)) {
			Application::instance()->getUiComponent()->displayMessage("Cannot create connection. One port needs to be an ingoing port while the other is an outgoing port.\n");
			return true;
		}
		else if (originConnectorType == ot::ConnectorType::Out &&
			destinationConnectorType == ot::ConnectorType::Out) {
			Application::instance()->getUiComponent()->displayMessage("Cannot create connection. One port needs to be an ingoing port while the other is an outgoing port.\n");
			return true;
		}
	}
	
	EntityNamingBehavior connectionNaming;
	connection.setUid(model->createEntityUID());
	createConnection(_scene, _originBlock, connection, connectionNaming, modelStateInfo);
	model->addEntitiesToModel(modelStateInfo, "Added Connection", true, true, true);

	return true;
}

bool BlockHandler::createBlockToConnectionConnection(EntityGraphicsScene* _scene, EntityBlock* _originBlock, EntityBlockConnection* _destinationConnection, const ot::GraphicsConnectionDropEvent& _eventData, bool _connectionReversed) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);

	// Ensure connection on connection is allowed
	if (!_scene->getSceneFlags().has(EntityGraphicsScene::AllowConnectionsOnConnections)) {
		return true; // Ingore processing but return success
	}

	// Here I want to get all block/connection entities
	std::map<ot::UID, EntityBlock*> blockEntities;
	std::map<ot::UID, EntityBlockConnection*> connectionEntities;
	std::list<std::string> entitiesToDelete;
	std::list<ot::GraphicsConnectionCfg> newConnections;

	std::queue<std::pair<std::string, EntityBlock*>> connectedElements;
	auto blocks = model->getListOfFolderItems(_scene->getName(), true);
	auto connections = model->getListOfFolderItems(_scene->getName() + "/" + m_connectionsFolder, true);

	for (auto& block : blocks) {
		EntityBase* baseEntity = model->findEntityFromName(block);
		if (!baseEntity) {
			OT_LOG_E("Could not find block entity { \"Name\": \"" + block + "\" }");
			return false;
		}
		EntityBlock* blockEntity = dynamic_cast<EntityBlock*>(baseEntity);
		if (!blockEntity) {
			//OT_LOG_E("Could not cast to EntityBlock { \"Name\": \"" + block + "\" }");
			continue;
		}
		blockEntities.insert_or_assign(blockEntity->getEntityID(), blockEntity);
	}

	for(auto & connection : connections) {
		EntityBase* baseEntity = model->findEntityFromName(connection);
		if (!baseEntity) {
			OT_LOG_E("Could not find connection entity { \"Name\": \"" + connection + "\" }");
			return false;
		}
		EntityBlockConnection* connectionEntity = dynamic_cast<EntityBlockConnection*>(baseEntity);
		if (!connectionEntity) {
			//OT_LOG_E("Could not cast to EntityBlockConnection { \"Name\": \"" + connection + "\" }");
			continue;
		}
		connectionEntities.insert_or_assign(connectionEntity->getEntityID(), connectionEntity);
	}

	// The connection to be added
	ot::GraphicsConnectionCfg requestedConnection = _eventData.getConnectionCfg();

	// Add the intersection item
	ot::NewModelStateInfo _modelStateInfo;
	
	// The destination connection is the connection to be deleted
    // Get connection cfg
	ot::GraphicsConnectionCfg connectionCfg = _destinationConnection->getConnectionCfg();

	// First get the connection which needs to be deleted by the new connection
	std::unique_ptr<EntityBlock> connector = nullptr;
	if (!_connectionReversed) {
		//Saving connected Element and connector
		connectedElements.push(std::make_pair(requestedConnection.getOriginConnectable(), blockEntities[requestedConnection.getOriginUid()]));
		connector = createBlockEntity(_scene, requestedConnection.getDestPos(), EntityBlockCircuitConnector::className(), _modelStateInfo);
	}
	else {
		// Saving connected Element and connector
		connectedElements.push(std::make_pair(requestedConnection.getDestConnectable(), blockEntities[requestedConnection.getDestinationUid()]));
		connector = createBlockEntity(_scene, requestedConnection.getOriginPos(), EntityBlockCircuitConnector::className(), _modelStateInfo);
	}

	if (!connector) {
		OT_LOG_E("Failed to create BlockEntity");
		return false;
	}

	// Check if the the blocks which are connected to the connection exist
	if (blockEntities.find(connectionCfg.getDestinationUid()) == blockEntities.end() ||
		blockEntities.find(connectionCfg.getOriginUid()) == blockEntities.end()) {
		OT_LOG_E("BlockEntity not found { \"OriginID\": " + std::to_string(connectionCfg.getOriginUid()) + ", \"DestinationID\": " + std::to_string(connectionCfg.getDestinationUid()) + " }");
		return false;
	}

	// Saving connected Elements and connectors
	connectedElements.push(std::make_pair(connectionCfg.getDestConnectable(), blockEntities[connectionCfg.getDestinationUid()]));
	connectedElements.push(std::make_pair(connectionCfg.getOriginConnectable(), blockEntities[connectionCfg.getOriginUid()]));

	entitiesToDelete.push_back(_destinationConnection->getName());
	// Delete the connection
	model->deleteEntitiesFromModel(entitiesToDelete, false);

	// Create a GraphicsConnectionCfg for all elements
	while (!connectedElements.empty()) {
		ot::GraphicsConnectionCfg temp(_eventData.getConnectionCfg());
		temp.setDestUid(connector->getEntityID());
		temp.setDestConnectable(connector->getName());
		temp.setOriginUid(connectedElements.front().second->getEntityID());
		temp.setOriginConnectable(connectedElements.front().first);
		temp.setLineShape(_originBlock->getDefaultConnectionShape());
		newConnections.push_back(temp);
		connectedElements.pop();
	}

	EntityNamingBehavior connectionNaming;
	connectionNaming.explicitNaming = true;

	for (auto& connectionCfg : newConnections) {
		connectionCfg.setUid(model->createEntityUID());
		createConnection(_scene, _originBlock, connectionCfg, connectionNaming, _modelStateInfo);
	}

	model->addEntitiesToModel(_modelStateInfo, "Added Connection to Connection", true, true, true);
	return true;
}

bool BlockHandler::updateBlock(const ot::GraphicsItemCfg* _changedBlock, const ot::GraphicsChangeEvent& _changeEvent) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);

	// Get uid and transform
	const ot::UID blockID = _changedBlock->getUid();
	const ot::Transform transform = _changedBlock->getTransform();

	// Get EntityBlock
	auto entBase = model->getEntityByID(blockID);
	EntityBlock* blockEnt = dynamic_cast<EntityBlock*>(entBase);
	if (!blockEnt) {
		OT_LOG_E("BlockEntity not found { \"EntityID\": " + std::to_string(blockID) + " }");
		return false;
	}

	bool storeTopologyEntity = false;

	// Here i will update the rotation
	if (PropertyHelper::hasProperty(blockEnt, "Rotation")) {
		EntityPropertiesDouble* propertyRotation = PropertyHelper::getDoubleProperty(blockEnt, "Rotation");
		if (propertyRotation->getValue() != transform.getRotation()) {
			propertyRotation->setValue(transform.getRotation());
			storeTopologyEntity = true;
		}
	}

	// Here i update the flip
	std::map<ot::Transform::FlipState, std::string > stringFlipMap;
	stringFlipMap.insert_or_assign(ot::Transform::NoFlip, "NoFlip");
	stringFlipMap.insert_or_assign(ot::Transform::FlipVertically, "FlipVertically");
	stringFlipMap.insert_or_assign(ot::Transform::FlipHorizontally, "FlipHorizontally");

	if(PropertyHelper::hasProperty(blockEnt, "Flip")) {
		EntityPropertiesSelection* propertyFlip = PropertyHelper::getSelectionProperty(blockEnt, "Flip");
		auto propertyBaseFlip = blockEnt->getProperties().getProperty("Flip");
		if (transform.getFlipStateFlags() & ot::Transform::FlipHorizontally) {
			if (propertyFlip->getValue() != stringFlipMap[static_cast<ot::Transform::FlipState>(transform.getFlipStateFlags() & ot::Transform::FlipHorizontally)]) {
				propertyFlip->setValue(stringFlipMap[ot::Transform::FlipHorizontally]);
				storeTopologyEntity = true;
			}
		}

		if (transform.getFlipStateFlags() & ot::Transform::FlipVertically) {
			if (propertyFlip->getValue() != stringFlipMap[static_cast<ot::Transform::FlipState>(transform.getFlipStateFlags() & ot::Transform::FlipVertically)]) {
				propertyFlip->setValue(stringFlipMap[ot::Transform::FlipVertically]);
				storeTopologyEntity = true;
			}
		}
	}

	// Here update block positition
	ot::UID positionID = blockEnt->getCoordinateEntityID();
	std::map<ot::UID, EntityBase*> map;
	auto coordinateEntBase = model->readEntityFromEntityID(blockEnt, positionID, map);

	EntityCoordinates2D* coordinateEntity = dynamic_cast<EntityCoordinates2D*>(coordinateEntBase);
	if (!coordinateEntity) {
		OT_LOG_E("Coordinate Entity not found { \"EntityID\": " + std::to_string(positionID) + " }");
		return false;
	}
	
	if (coordinateEntity->getCoordinates() != _changedBlock->getPosition()) {
		coordinateEntity->setCoordinates(_changedBlock->getPosition());
		coordinateEntity->storeToDataBase();
		blockEnt->setCoordinateEntity(*coordinateEntity);
		model->getStateManager()->modifyEntityVersion(*coordinateEntity);
		storeTopologyEntity = true;
	}
	
	if (storeTopologyEntity) {
		blockEnt->storeToDataBase();
		model->getStateManager()->modifyEntityVersion(*blockEnt);
	}
	return true;
}

bool BlockHandler::updateConnection(const ot::GraphicsConnectionCfg& _changedConnection, const ot::GraphicsChangeEvent& _changeEvent) {
    Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);

	EntityBase* baseEditor = model->findEntityFromName(_changeEvent.getEditorName());
	if(!baseEditor) {
		OT_LOG_E("Could not find graphics editor entity { \"ViewName\": \"" + _changeEvent.getEditorName() + "\" }");
		return false;
	}
	EntityGraphicsScene* editor = dynamic_cast<EntityGraphicsScene*>(baseEditor);
	if(!editor) {
		OT_LOG_E("Could not cast to EntityGraphicsScene { \"ViewName\": \"" + _changeEvent.getEditorName() + "\" }");
		return false;
	}

	// Get uid of changed connection
	const ot::UID connectionID = _changedConnection.getUid();

	// Get connection entity
	auto entBase = model->getEntityByID(connectionID);
	EntityBlockConnection* connectionEntity = dynamic_cast<EntityBlockConnection*>(entBase);
	if (!connectionEntity) {
		OT_LOG_E("BlockEntity not found { \"EntityID\": " + std::to_string(connectionID) + " }");
		return false;
	}

	// Ui does not update the connectionCfg anymore means that originUid/destinationUid could exist even if the block with corresponding uid does not exits
	// Only update the position of the connections means no need of checking if originUid or destinationUid exists
	ot::GraphicsConnectionCfg connectionCfg = connectionEntity->getConnectionCfg();

	if(_changedConnection.getOriginUid() == ot::invalidUID || blockExists(_changedConnection.getOriginUid())) {
		connectionCfg.setOriginUid(_changedConnection.getOriginUid());
		connectionCfg.setOriginConnectable(_changedConnection.getOriginConnectable());
	}

	if(_changedConnection.getDestinationUid() == ot::invalidUID || blockExists(_changedConnection.getDestinationUid())) {
		connectionCfg.setDestUid(_changedConnection.getDestinationUid());
		connectionCfg.setDestConnectable(_changedConnection.getDestConnectable());
	}

	removeConnectionIfUnsnapped(editor, connectionEntity, _changedConnection);

	connectionCfg.setOriginPos(_changedConnection.getOriginPos());
	connectionCfg.setDestPos(_changedConnection.getDestPos());
	connectionEntity->setConnectionCfg(connectionCfg);
	connectionEntity->storeToDataBase();
	model->getStateManager()->modifyEntityVersion(*connectionEntity);

	return true;
}

std::unique_ptr<EntityBlock> BlockHandler::createBlockEntity(EntityGraphicsScene* _editor, const ot::Point2DD& _scenePos, const std::string& _itemName, ot::NewModelStateInfo& _newModelStateInfo) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);
	
	// Create block entity
	EntityBase* entBase = EntityFactory::instance().create(_itemName);
	if (entBase == nullptr) {
		OT_LOG_E("Failed to create Entity { \"ClassName\": \"" + _itemName + "\" }");
		return nullptr;
	}

	std::unique_ptr<EntityBlock> blockEnt(dynamic_cast<EntityBlock*>(entBase));
	if (blockEnt == nullptr) {
		OT_LOG_E("Could not cast to EntityBlock { \"EntityID\": " + std::to_string(entBase->getEntityID()) + " }");
		return nullptr;
	}

	// Determine unique name
	std::string blockFolderName;
	if (!blockEnt->getBlockFolderName().empty()) {
		blockFolderName = "/" + blockEnt->getBlockFolderName();
	}
	else {
		blockFolderName = blockEnt->getBlockFolderName();
	}

	std::string folderName = _editor->getName() + blockFolderName;

	std::list<std::string> blocks = model->getListOfFolderItems(folderName, true);
	std::string entName = CreateNewUniqueTopologyName(blockEnt->getNamingBehavior(), blocks, folderName, blockEnt->getBlockTitle());

	// Setup block entity
	blockEnt->setName(entName);
	blockEnt->setTreeItemEditable(true);
	blockEnt->setGraphicsPickerKey(_editor->getGraphicsPickerKey());
	blockEnt->setCallbackData(_editor->getCallbackData());
	blockEnt->setEntityID(model->createEntityUID());
	blockEnt->setGraphicsScenePackageChildName(blockEnt->getBlockFolderName());

	// Create coordinate entity
	EntityCoordinates2D* blockCoordinates = new EntityCoordinates2D(model->createEntityUID(), nullptr, nullptr, nullptr);
	blockCoordinates->setCoordinates(_scenePos);
	blockCoordinates->storeToDataBase();
	_newModelStateInfo.addDataEntity(*blockEnt, *blockCoordinates);

	// Link block and coordinate entity
	blockEnt->setCoordinateEntityID(blockCoordinates->getEntityID());
	blockEnt->createProperties();

	// Special handling for python blocks
	if (blockEnt->getClassName() == "EntityBlockPython") {
		EntityBlockPython* pythonBlock = dynamic_cast<EntityBlockPython*>(blockEnt.get());
		if (pythonBlock == nullptr) {
			OT_LOG_E("Could not cast to EntityBlockPython { \"EntityID\": " + std::to_string(blockEnt->getEntityID()) + " }");
			return nullptr;
		}
		ot::EntityInformation entityInfo;
		EntityBase* scriptFolder = model->findEntityFromName(ot::FolderNames::PythonScriptFolder);
		EntityBase* manifestFolder = model->findEntityFromName(ot::FolderNames::PythonManifestFolder);
		if (manifestFolder == nullptr || scriptFolder == nullptr)
		{
			OT_LOG_E(std::string("Python folders not found { \"ScriptFolderFound\": ") + (scriptFolder ? "true" : "false") + ", \"ManifestFolderFound\": " + (manifestFolder ? "true" : "false") + " }");
			return nullptr;
		}
		pythonBlock->setScriptFolder(scriptFolder->getEntityID());
		pythonBlock->setManifestFolder(manifestFolder->getEntityID());
	}

	blockEnt->storeToDataBase();
	_newModelStateInfo.addTopologyEntity(*blockEnt);

	// Add Block to Map
	addBlock(_editor->getEntityID(), blockEnt.get());
	return blockEnt;
}

void BlockHandler::createConnection( EntityGraphicsScene* _scene, EntityBlock* _originBlock, ot::GraphicsConnectionCfg& _connectionCfg, EntityNamingBehavior& _connectionNaming, ot::NewModelStateInfo& _newModelStateInfo) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);

	// Create connection entity
	EntityBlockConnection _connectionEntity(_connectionCfg.getUid(), nullptr, nullptr, nullptr);
	_connectionEntity.createProperties();
	_connectionEntity.setCallbackData(_scene->getCallbackData());
	_connectionEntity.setGraphicsPickerKey(_scene->getGraphicsPickerKey());
	_connectionEntity.setGraphicsScenePackageChildName(m_connectionsFolder);
	_connectionCfg.setLineShape(_originBlock->getDefaultConnectionShape());
	_connectionEntity.setConnectionCfg(_connectionCfg);

	if (_connectionNaming.explicitNaming) {
		std::list<std::string> connectionItems = model->getListOfFolderItems(_scene->getName() + "/" + m_connectionsFolder, true);

		// Use a manual counter because all connections are added to the model at the end.
		// CreateNewUniqueEntityName only considers connections already in the model,
		// which could otherwise result in duplicate names.
		std::string connectionName;
		do {
			connectionName = _scene->getName() + "/" + m_connectionsFolder + "/Connection" + std::to_string(_connectionNaming.namingCounter);
			_connectionNaming.namingCounter++;
		} while (std::find(connectionItems.begin(), connectionItems.end(), connectionName) != connectionItems.end());

		_connectionEntity.setName(connectionName);
	}
	else {
		// Determine unique name
		auto connectionsFolder = model->getListOfFolderItems(_scene->getName() + "/" + m_connectionsFolder, true);
		const std::string connectionName = CreateNewUniqueTopologyName(connectionsFolder, _scene->getName() + "/" + m_connectionsFolder, "Connection");
		_connectionEntity.setName(connectionName);
	}

	_connectionEntity.storeToDataBase();
	_newModelStateInfo.addTopologyEntity(_connectionEntity);

	// Add connection to map
	addConnection(_scene->getEntityID(), _connectionEntity);

	return;
}

void BlockHandler::modifyConnection(const ot::UID& _connectionID, EntityBase* _connectedBlockEntity){
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);
	OTAssertNullptr(_connectedBlockEntity);

	auto entBase = model->getEntityByID(_connectionID);
	if (!entBase) {
		OT_LOG_E("Entity not found { \"ConnectionID\": " + std::to_string(_connectionID) + " }");
		return;
	}
	EntityBlockConnection* connectionEntity = dynamic_cast<EntityBlockConnection*>(entBase);
	if (!connectionEntity) {
		OT_LOG_E("EntityBlockConnection cast failed { \"ConnectionID\": " + std::to_string(_connectionID) + " }");
		return;
	}

	EntityBlock* blockEnt = dynamic_cast<EntityBlock*>(_connectedBlockEntity);
	if (!blockEnt) {
		OT_LOG_E("EntityBlock cast failed { \"EntityID\": " + std::to_string(_connectedBlockEntity->getEntityID()) + " }");
		return;
	}

	ot::GraphicsConnectionCfg oldConnectionCfg = connectionEntity->getConnectionCfg();
	if (oldConnectionCfg.getOriginUid() == _connectedBlockEntity->getEntityID()) {
		oldConnectionCfg.setOriginUid(ot::invalidUID);
		oldConnectionCfg.setOriginConnectable("");
	}
	else if (oldConnectionCfg.getDestinationUid() == _connectedBlockEntity->getEntityID()) {
		oldConnectionCfg.setDestUid(ot::invalidUID);
		oldConnectionCfg.setDestConnectable("");
	}
	else {
		OT_LOG_E("Origin and destination differ for existing connection, can not modify "
			"{ \"ConnectedBlockID\": " + std::to_string(_connectedBlockEntity->getEntityID()) +
			", \"ConnectionID\": " + std::to_string(_connectionID) +
			", \"ConnectionOriginID\": " + std::to_string(oldConnectionCfg.getOriginUid()) +
			", \"ConnectionDestinationID\": " + std::to_string(oldConnectionCfg.getDestinationUid()) +
			" }"
		);
		return;
	}
	
	connectionEntity->setConnectionCfg(oldConnectionCfg);
	connectionEntity->storeToDataBase();
	model->getStateManager()->modifyEntityVersion(*connectionEntity);
}

bool BlockHandler::snapConnection(EntityGraphicsScene* _scene, const ot::GraphicsChangeEvent::SnapInfo& _snapInfo, ot::GraphicsConnectionCfg& _connectionCfg, std::set<EntityBlockConnection*>& _processedConnections) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);

	//First get the connection entity from the snapinfo
	EntityBase* baseEnt = model->getEntityByID(_snapInfo.connectionCfg.getUid());
	if (baseEnt == nullptr) {
		OT_LOG_E("Could not find EntityBase { \"EntityID\": " + std::to_string(_snapInfo.connectionCfg.getUid()) + " }");
		return false;
	}

	//Cast to EntityBlockConnection
	EntityBlockConnection* connectionEnt = dynamic_cast<EntityBlockConnection*>(baseEnt);
	if (!connectionEnt) {
		OT_LOG_E("Could not cast to EntityBlockConnection { \"EntityID\": " + std::to_string(_snapInfo.connectionCfg.getUid()) + " }");
		return false;
	}

	// Now the the connection cfg from snap info
	ot::GraphicsConnectionCfg connectionCfg = _snapInfo.connectionCfg;
	ot::GraphicsConnectionCfg newConnectionCfg;
	auto itConnection = _processedConnections.find(connectionEnt);

	// If the connection has already been processed, use the updated cfg
	if (itConnection != _processedConnections.end()) {
		newConnectionCfg = (*itConnection)->getConnectionCfg();
	}
	else {
		newConnectionCfg = connectionEnt->getConnectionCfg();
	}

	const bool isOrigin = _snapInfo.isOrigin;

	// Determine UIDs and connector names based on isOrigin once
	const ot::UID snapUid = isOrigin ? connectionCfg.getOriginUid() : connectionCfg.getDestinationUid();
	const ot::UID otherUid = isOrigin ? connectionCfg.getDestinationUid() : connectionCfg.getOriginUid();
	const std::string& snapConnectable = isOrigin ? connectionCfg.getOriginConnectable() : connectionCfg.getDestConnectable();
	const std::string& otherConnectable = isOrigin ? connectionCfg.getDestConnectable() : connectionCfg.getOriginConnectable();

	// Get the snapped entity
	EntityBase* snapEntity = model->getEntityByID(snapUid);
	if (!snapEntity) {
		OT_LOG_E((isOrigin ? "Origin" : "Destination") + std::string(" entity not found { \"EntityID\": ") + std::to_string(snapUid) + " }");
		return false;
	}

	EntityBlock* snapBlock = dynamic_cast<EntityBlock*>(snapEntity);
	if (!snapBlock) {
		OT_LOG_E("BlockEntity cast failed { \"EntityID\": " + std::to_string(snapEntity->getEntityID()) + " }");
		return false;
	}

	// Check if the other entity exists
	bool bothItemsExist = true;
	EntityBlock* otherBlock = nullptr;
	if (!blockExists(_scene->getEntityID(), otherUid)) {
		if (isOrigin) {
			connectionCfg.setDestUid(ot::invalidUID);
			connectionCfg.setDestConnectable("");
		}
		else {
			connectionCfg.setOriginUid(ot::invalidUID);
			connectionCfg.setOriginConnectable("");
		}
		bothItemsExist = false;
	}
	else {
		EntityBase* otherEntity = model->getEntityByID(otherUid);
		if (!otherEntity) {
			OT_LOG_E((isOrigin ? "Destination" : "Origin") + std::string(" entity not found { \"EntityID\": ") + std::to_string(otherUid) + " }");
			return false;
		}
		otherBlock = dynamic_cast<EntityBlock*>(otherEntity);
		if (!otherBlock) {
			OT_LOG_E("BlockEntity cast failed { \"EntityID\": " + std::to_string(otherEntity->getEntityID()) + " }");
			return false;
		}
	}

	// Validate connector types if both items exist and neither is a circuit connector
	if (bothItemsExist && snapBlock->getClassName() != "EntityBlockCircuitConnector" && otherBlock->getClassName() != "EntityBlockCircuitConnector") {
		// Determine which block is origin and which is destination
		EntityBlock* originBlock = isOrigin ? snapBlock : otherBlock;
		EntityBlock* destBlock = isOrigin ? otherBlock : snapBlock;
		const std::string& originConnectorName = isOrigin ? snapConnectable : otherConnectable;
		const std::string& destConnectorName = isOrigin ? otherConnectable : snapConnectable;

		auto originConnectorIt = originBlock->getAllConnectorsByName().find(originConnectorName);
		auto destConnectorIt = destBlock->getAllConnectorsByName().find(destConnectorName);

		if (originConnectorIt != originBlock->getAllConnectorsByName().end() &&
			destConnectorIt != destBlock->getAllConnectorsByName().end()) {
			ot::ConnectorType originConnectorType = originConnectorIt->second.getConnectorType();
			ot::ConnectorType destConnectorType = destConnectorIt->second.getConnectorType();

			if (originConnectorType == ot::ConnectorType::UNKNOWN || destConnectorType == ot::ConnectorType::UNKNOWN) {
				OT_LOG_E("Could not determine connector types for connection");
				return false;
			}

			if ((originConnectorType == ot::ConnectorType::In || originConnectorType == ot::ConnectorType::InOptional) &&
				(destConnectorType == ot::ConnectorType::In || destConnectorType == ot::ConnectorType::InOptional)) {
				Application::instance()->getUiComponent()->displayMessage("Cannot create connection. One port needs to be an ingoing port while the other is an outgoing port.\n");
				return true;
			}
			else if (originConnectorType == ot::ConnectorType::Out && destConnectorType == ot::ConnectorType::Out) {
				Application::instance()->getUiComponent()->displayMessage("Cannot create connection. One port needs to be an ingoing port while the other is an outgoing port.\n");
				return true;
			}
		}
	}

	// Update connection configuration
	if (isOrigin) {
		newConnectionCfg.setOriginConnectable(connectionCfg.getOriginConnectable());
		newConnectionCfg.setOriginUid(connectionCfg.getOriginUid());
	}
	else {
		newConnectionCfg.setDestConnectable(connectionCfg.getDestConnectable());
		newConnectionCfg.setDestUid(connectionCfg.getDestinationUid());
	}

	connectionEnt->setConnectionCfg(newConnectionCfg);
	addConnection(_scene->getEntityID(), *connectionEnt);
	_processedConnections.insert(connectionEnt);
	return true;
}



