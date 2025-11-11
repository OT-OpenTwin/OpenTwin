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

	std::shared_ptr<EntityBlock> createdBlock = createBlockEntity(modelStateInfo, _eventData.getItemName(),_eventData.getScenePos(), editor);
	if (createdBlock == nullptr) {
		OT_LOG_E("Could not create block entity {\"BlockName\": \"" + _eventData.getItemName() + "\" } ");
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
	EntityNamingBehavior connectionNaming;
	createConnection(modelStateInfo, _scene, connection, _originBlock, connectionNaming);

	
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

	model->addEntitiesToModel(modelStateInfo, "Added Connection", true, true, true);

	return true;
}

bool BlockHandler::createBlockToConnectionConnection(EntityGraphicsScene* _scene, EntityBlock* _originBlock, EntityBlockConnection* _destinationConnection, const ot::GraphicsConnectionDropEvent& _eventData, bool _connectionReversed) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);


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

	// As next step i need to add the intersection item
	ot::NewModelStateInfo _modelStateInfo;
	std::shared_ptr<EntityBlock> connector = createBlockEntity(_modelStateInfo, EntityBlockCircuitConnector::className(), _eventData.getScenePos(), _scene);
	
	// The destination connection is the connection to be deleted
    // Get connection cfg
	ot::GraphicsConnectionCfg connectionCfg = _destinationConnection->getConnectionCfg();

	//First i get the connection which i want to delete by the connection to be added
	if (!_connectionReversed) {
		//Saving connected Element and connector
		connectedElements.push(std::make_pair(requestedConnection.getOriginConnectable(), blockEntities[requestedConnection.getOriginUid()]));
	}
	else {
		//Saving connected Element and connector
		connectedElements.push(std::make_pair(requestedConnection.getDestConnectable(), blockEntities[requestedConnection.getDestinationUid()]));
	}

	// Here I check if the the blocks which are connected to the connection exist
	if (blockEntities.find(connectionCfg.getDestinationUid()) == blockEntities.end() ||
		blockEntities.find(connectionCfg.getOriginUid()) == blockEntities.end()) {
		OT_LOG_E("BlockEntity not found");
		return false;
	}

	//Saving  connected Elements and connectors
	connectedElements.push(std::make_pair(connectionCfg.getDestConnectable(), blockEntities[connectionCfg.getDestinationUid()]));
	connectedElements.push(std::make_pair(connectionCfg.getOriginConnectable(), blockEntities[connectionCfg.getOriginUid()]));

	entitiesToDelete.push_back(_destinationConnection->getName());
	// Now I can delete the connection
	model->deleteEntitiesFromModel(entitiesToDelete, false);

	//Now i create a GraphicsConnectionCfg for all elements
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

	EntityNamingBehavior _connectionNaming;
	_connectionNaming.explicitNaming = true;

	for (auto& connectionCfg : newConnections) {
		createConnection(_modelStateInfo, _scene, connectionCfg, _originBlock, _connectionNaming);
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
		OT_LOG_E("BlockEntity not found");
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
	auto parent = coordinateEntity->getParent();
	if (!coordinateEntity) {
		OT_LOG_E("Coordinate Entity is null");
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

	// Get uid of changed connection
	const ot::UID connectionID = _changedConnection.getUid();
	
	// Get connection entity
	auto entBase = model->getEntityByID(connectionID);
	EntityBlockConnection* connectionEntity = dynamic_cast<EntityBlockConnection*>(entBase);
	if (!connectionEntity) {
		OT_LOG_E("BlockEntity is null");
		return false;
	}

	// Update connection
	connectionEntity->setConnectionCfg(_changedConnection);
	connectionEntity->storeToDataBase();
	model->getStateManager()->modifyEntityVersion(*connectionEntity);

	return true;
}

std::shared_ptr<EntityBlock> BlockHandler::createBlockEntity(ot::NewModelStateInfo& _newModelStateInfo, const std::string& _itemName, const ot::Point2DD& _scenePos , EntityGraphicsScene* _editor) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);
	
	// Create block entity
	EntityBase* entBase = EntityFactory::instance().create(_itemName);
	if (entBase == nullptr) {
		OT_LOG_E("Could not create Entity: " + _itemName);
		return nullptr;
	}

	std::shared_ptr<EntityBlock> blockEnt(dynamic_cast<EntityBlock*>(entBase));
	if (blockEnt == nullptr) {
		OT_LOG_E("Could not cast to EntityBlock: " + entBase->getEntityID());
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
	blockEnt->setEditable(true);
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
			OT_LOG_E("Could not cast to EntityBlockPython: " + blockEnt->getEntityID());
			return nullptr;
		}
		ot::EntityInformation entityInfo;
		EntityBase* entity = model->findEntityFromName(ot::FolderNames::PythonScriptFolder);
		pythonBlock->setScriptFolder(ot::FolderNames::PythonScriptFolder, entity->getEntityID());
	}

	blockEnt->storeToDataBase();
	_newModelStateInfo.addTopologyEntity(*blockEnt);
	return blockEnt;
}

void BlockHandler::createConnection(ot::NewModelStateInfo& _newModelStateInfo, EntityGraphicsScene* _scene, ot::GraphicsConnectionCfg& _connectionCfg, EntityBlock* _originBlock, EntityNamingBehavior& _connectionNaming) {
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);

	
	// Create connection entity
	EntityBlockConnection _connectionEntity(model->createEntityUID(), nullptr, nullptr, nullptr);
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
	return;
}

