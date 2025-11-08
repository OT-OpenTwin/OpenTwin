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
#include "PropertyHelper.h"

bool BlockHandler::addViewBlockRelation(std::string _viewName, ot::UID _blockId, ot::UID _connectionId) {
	auto& blocks = m_viewBlockConnectionMap[_viewName];
	if (blocks.find(_blockId) != blocks.end()) {
		return false;
	}

	blocks.emplace(_blockId, _connectionId);
	return true;
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

	// Create block entity
	EntityBase* entBase = EntityFactory::instance().create(_eventData.getItemName());
	if (entBase == nullptr) {
		OT_LOG_E("Could not create Entity: " + _eventData.getItemName());
		return ot::ReturnMessage::Failed;
	}

	std::unique_ptr<EntityBlock> blockEnt(dynamic_cast<EntityBlock*>(entBase));
	if (blockEnt == nullptr) {
		OT_LOG_E("Could not cast to EntityBlock: " + entBase->getEntityID());
		return ot::ReturnMessage::Failed;
	}

	// Determine unique name
	std::string blockFolderName;
	if (!blockEnt->getBlockFolderName().empty()) {
		blockFolderName = "/" + blockEnt->getBlockFolderName();
	}
	else {
		blockFolderName = blockEnt->getBlockFolderName();
	}

	std::string folderName = _eventData.getEditorName() + blockFolderName;

	std::list<std::string> blocks = model->getListOfFolderItems(folderName, true);
	std::string entName = CreateNewUniqueTopologyName(blockEnt->getNamingBehavior(), blocks, folderName, blockEnt->getBlockTitle());

	// Setup block entity
	blockEnt->setName(entName);
	blockEnt->setEditable(true);
	blockEnt->setGraphicsPickerKey(editor->getGraphicsPickerKey());
	blockEnt->setCallbackData(editor->getCallbackData());
	blockEnt->setEntityID(model->createEntityUID());
	blockEnt->setGraphicsScenePackageChildName(blockEnt->getBlockFolderName());

	// Create coordinate entity
	EntityCoordinates2D* blockCoordinates = new EntityCoordinates2D(model->createEntityUID(), nullptr, nullptr, nullptr);
	blockCoordinates->setCoordinates(_eventData.getScenePos());
	blockCoordinates->storeToDataBase();
	modelStateInfo.addDataEntity(*blockEnt, *blockCoordinates);

	// Link block and coordinate entity
	blockEnt->setCoordinateEntityID(blockCoordinates->getEntityID());
	blockEnt->createProperties();

	// Special handling for python blocks
	if (blockEnt->getClassName() == "EntityBlockPython") {
		EntityBlockPython* pythonBlock = dynamic_cast<EntityBlockPython*>(blockEnt.get());
		if (pythonBlock == nullptr) {
			OT_LOG_E("Could not cast to EntityBlockPython: " + blockEnt->getEntityID());
			return ot::ReturnMessage::Failed;
		}
		ot::EntityInformation entityInfo;
		EntityBase* entity = model->findEntityFromName(ot::FolderNames::PythonScriptFolder);
		pythonBlock->setScriptFolder(ot::FolderNames::PythonScriptFolder, entity->getEntityID());
	}

	blockEnt->storeToDataBase();
	modelStateInfo.addTopologyEntity(*blockEnt);

	model->addEntitiesToModel(modelStateInfo, "Added block", true, true, true);

	// Check if any service wants to be notified about this change
	if (!_eventData.isEventFlagSet(ot::GuiEvent::IgnoreNotify)) {
		std::list<std::string> notifyServices = blockEnt->getServicesForCallback(EntityBase::Callback::DataNotify);
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

			if (!createBlockToBlockConnection(editor, originBlock, destinationBlock, _eventData)) {
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

	ot::GraphicsConnectionCfg connection = _eventData.getConnectionCfg();

	// Create connection entity
	EntityBlockConnection connectionEntity(model->createEntityUID(), nullptr, nullptr, nullptr);
	connectionEntity.createProperties();
	connectionEntity.setCallbackData(_scene->getCallbackData());
	connectionEntity.setGraphicsPickerKey(_scene->getGraphicsPickerKey());
	connectionEntity.setGraphicsScenePackageChildName(m_connectionsFolder);

	// Determine unique name
	auto connectionsFolder = model->getListOfFolderItems(_scene->getName() + "/" + m_connectionsFolder, true);
	const std::string connectionName = CreateNewUniqueTopologyName(connectionsFolder, _scene->getName() + "/" + m_connectionsFolder, "Connection");
	connectionEntity.setName(connectionName);

	// Get connectors
	connection.setLineShape(_originBlock->getDefaultConnectionShape());
	connectionEntity.setConnectionCfg(connection);

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

	connectionEntity.storeToDataBase();
	modelStateInfo.addTopologyEntity(connectionEntity);

	model->addEntitiesToModel(modelStateInfo, "Added connections", true, true, true);

	return true;
}

bool BlockHandler::createBlockToConnectionConnection(EntityGraphicsScene* _scene, EntityBlock* _originBlock, EntityBlockConnection* _destinationConnection, const ot::GraphicsConnectionDropEvent& _eventData, bool _connectionReversed) {
	Model* model = Application::instance()->getModel();

	return false;
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

