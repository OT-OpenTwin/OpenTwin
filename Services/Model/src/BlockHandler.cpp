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

bool BlockHandler::addViewBlockRelation(std::string _viewName, ot::UID _blockId, ot::UID _connectionId) {
	auto& blocks = m_viewBlockConnectionMap[_viewName];
	if (blocks.find(_blockId) != blocks.end()) {
		return false;
	}

	blocks.emplace(_blockId, _connectionId);
	return true;
}

ot::ReturnMessage BlockHandler::graphicsItemChanged(const ot::GraphicsItemCfg* _item) {
	const ot::UID blockID = _item->getUid();
	const ot::Transform transform = _item->getTransform();

	Model* _model = Application::instance()->getModel();
	auto entBase = _model->getEntityByID(blockID);
	EntityBlock* blockEnt = dynamic_cast<EntityBlock*>(entBase);
	if (!blockEnt) {
		OT_LOG_E("BlockEntity not fond");
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Entity not found");
	}

	bool storeTopologyEntity = false;

	// Here i will update the rotation
	auto propertyBase = blockEnt->getProperties().getProperty("Rotation");
	if (propertyBase != nullptr) {
		auto propertyRotation = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
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

	auto propertyBaseFlip = blockEnt->getProperties().getProperty("Flip");
	if (propertyBaseFlip != nullptr) {
		auto propertyFlip = dynamic_cast<EntityPropertiesSelection*>(propertyBaseFlip);
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
	auto coordinateEntBase = _model->readEntityFromEntityID(blockEnt, positionID, map);

	EntityCoordinates2D* coordinateEntity = dynamic_cast<EntityCoordinates2D*>(coordinateEntBase);
	auto parent = coordinateEntity->getParent();
	if (!coordinateEntity) {
		OT_LOG_E("Coordinate Entity is null");
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Coordinate entity not found");
	}

	
	if (coordinateEntity->getCoordinates() != _item->getPosition()) {
		coordinateEntity->setCoordinates(_item->getPosition());
		coordinateEntity->storeToDataBase();
		blockEnt->setCoordinateEntity(*coordinateEntity);
		_model->getStateManager()->modifyEntityVersion(*coordinateEntity);
		storeTopologyEntity = true;
	}

	if (storeTopologyEntity) {
		blockEnt->storeToDataBase();
		_model->getStateManager()->modifyEntityVersion(*blockEnt);
	}

	_model->setModified();
	_model->modelChangeOperationCompleted("Item changed");
	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage BlockHandler::graphicsConnectionChanged(const ot::GraphicsConnectionCfg& _connectionData) {
	

	const ot::UID connectionID =  _connectionData.getUid();
	Model* _model = Application::instance()->getModel();
	auto entBase = _model->getEntityByID(connectionID);
	EntityBlockConnection* connectionEntity = dynamic_cast<EntityBlockConnection*>(entBase);
	if (!connectionEntity) {
		OT_LOG_E("BlockEntity is null");
		return ot::ReturnMessage::Failed;
	}

	connectionEntity->setConnectionCfg( _connectionData);
	connectionEntity->storeToDataBase();
	_model->getStateManager()->modifyEntityVersion(*connectionEntity);


	_model->setModified();
	_model->modelChangeOperationCompleted("Connection Changed");
	return ot::ReturnMessage::Ok;

}

ot::ReturnMessage BlockHandler::graphicsItemRequested(const std::string& _viewName, const std::string& _itemName, const ot::Point2DD& _pos) {
	Model* model = Application::instance()->getModel();

	ot::UIDList topoEntID, topoEntVers, dataEntID, dataEntVers, dataEntParent;
	std::list<bool> forceVis;
	
	EntityBase* entBase = EntityFactory::instance().create(_itemName);
	if (entBase == nullptr) {
		OT_LOG_E("Could not create Entity: " + _itemName);
		return ot::ReturnMessage::Failed;
	}

	EntityBlock* blockEnt = dynamic_cast<EntityBlock*>(entBase);
	if (blockEnt == nullptr) {
		OT_LOG_E("Could not cast to EntityBlock: " + entBase->getEntityID());
		return ot::ReturnMessage::Failed;
	}

	EntityBase* editorBase = model->findEntityFromName(_viewName);
	EntityGraphicsScene* editor = dynamic_cast<EntityGraphicsScene*>(editorBase);

	std::string blockFolderName;
	if (!blockEnt->getBlockFolderName().empty()) {
		blockFolderName = "/" + blockEnt->getBlockFolderName();
	}
	else {
		blockFolderName = blockEnt->getBlockFolderName();
	}

	std::string folderName = _viewName + blockFolderName;

	std::list<std::string> blocks = model->getListOfFolderItems(folderName, true);
	std::string entName = CreateNewUniqueTopologyName(blockEnt->getNamingBehavior(), blocks, folderName, blockEnt->getBlockTitle());
	blockEnt->setName(entName);
	blockEnt->setEditable(true);
	blockEnt->setGraphicsPickerKey(editor->getGraphicsPickerKey());
	blockEnt->setOwningService(editor->getOwningService());
	blockEnt->setEntityID(model->createEntityUID());

	
	blockEnt->setGraphicsScenePackageChildName(blockEnt->getBlockFolderName());

	EntityCoordinates2D* blockCoordinates = new EntityCoordinates2D(model->createEntityUID(), nullptr, nullptr, nullptr, "");
	blockCoordinates->setCoordinates(_pos);
	blockCoordinates->storeToDataBase();

	blockEnt->setCoordinateEntityID(blockCoordinates->getEntityID());
	blockEnt->createProperties();
	if (blockEnt->getClassName() == "EntityBlockPython") {
		EntityBlockPython* pythonBlock = dynamic_cast<EntityBlockPython*>(blockEnt);
		if (pythonBlock == nullptr) {
			OT_LOG_E("Could not cast to EntityBlockPython: " + blockEnt->getEntityID());
			return ot::ReturnMessage::Failed;
		}
		ot::EntityInformation entityInfo;
		EntityBase* entity = model->findEntityFromName(ot::FolderNames::PythonScriptFolder);
		pythonBlock->setScriptFolder(ot::FolderNames::PythonScriptFolder, entity->getEntityID());
	}

	blockEnt->storeToDataBase();

	topoEntID.push_back(blockEnt->getEntityID());
	topoEntVers.push_back(blockEnt->getEntityStorageVersion());
	forceVis.push_back(false);
	dataEntID.push_back(blockCoordinates->getEntityID());
	dataEntVers.push_back(blockCoordinates->getEntityStorageVersion());
	dataEntParent.push_back(blockEnt->getEntityID());
	model->addEntitiesToModel(topoEntID, topoEntVers, forceVis, dataEntID, dataEntVers, dataEntParent, "Added block", true, true, true);

	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage BlockHandler::graphicsConnectionRequested(const ot::GraphicsConnectionPackage& _connectionData) {
	Model* model = Application::instance()->getModel();
	
	auto connections = _connectionData.getConnections();
	std::string viewName = _connectionData.getName();
	EntityBase* editorBase = model->findEntityFromName(viewName);
	EntityGraphicsScene* editor = dynamic_cast<EntityGraphicsScene*>(editorBase);
	std::list<EntityBlockConnection> entitiesForUpdate;
	ot::UIDList topoEntID, topoEntVers, dataEntID, dataEntVers, dataEntParent;
	std::list<bool> forceVis;


	for (auto& _connection : connections) {
		EntityBlockConnection connectionEntity(model->createEntityUID(), nullptr, nullptr, nullptr, editor->getOwningService());
		connectionEntity.createProperties();
		
		auto connectionsFolder = model->getListOfFolderItems(_connectionData.getName() + "/" + m_connectionsFolder, true);
		const std::string connectionName = CreateNewUniqueTopologyName(connectionsFolder, _connectionData.getName() + "/" + m_connectionsFolder, "Connection");
		connectionEntity.setName(connectionName);
		connectionEntity.setGraphicsPickerKey(_connectionData.getPickerKey());
		connectionEntity.setGraphicsScenePackageChildName(m_connectionsFolder);

		EntityBase* originEntityBase = model->getEntityByID(_connection.getOriginUid());
		EntityBlock* originBlock = dynamic_cast<EntityBlock*>(originEntityBase);

		EntityBase* destinationEntityBase = model->getEntityByID(_connection.getDestinationUid());
		EntityBlock* destinationBlock = dynamic_cast<EntityBlock*>(destinationEntityBase);
		
	
		_connection.setLineShape(originBlock->getDefaultConnectionShape());
		
		connectionEntity.setConnectionCfg(_connection);

		if (originBlock == nullptr || destinationBlock == nullptr) {
			OT_LOG_E("Could not find origin or destination block for connection");
			return ot::ReturnMessage::Failed;
		}

		auto originConnectorIt = originBlock->getAllConnectorsByName().find(_connection.getOriginConnectable());
		auto destinationConnectorIt = destinationBlock->getAllConnectorsByName().find(_connection.getDestConnectable());

		if (originConnectorIt == originBlock->getAllConnectorsByName().end() || destinationConnectorIt == destinationBlock->getAllConnectorsByName().end()) {
			OT_LOG_E("Could not find origin or destination connector for connection");
			return ot::ReturnMessage::Failed;
		}

		auto originConnectorType = originConnectorIt->second.getConnectorType();
		auto destinationConnectorType = destinationConnectorIt->second.getConnectorType();

		if ((originConnectorType == ot::ConnectorType::In || originConnectorType == ot::ConnectorType::InOptional) &&
			(destinationConnectorType == ot::ConnectorType::In || destinationConnectorType == ot::ConnectorType::InOptional)) {
			Application::instance()->getUiComponent()->displayMessage("Cannot create connection. One port needs to be an ingoing port while the other is an outgoing port.\n");
			return ot::ReturnMessage::Ok;
		}
		else if (originConnectorType == ot::ConnectorType::Out &&
			destinationConnectorType == ot::ConnectorType::Out) {
			Application::instance()->getUiComponent()->displayMessage("Cannot create connection. One port needs to be an ingoing port while the other is an outgoing port.\n");
			return ot::ReturnMessage::Ok;
		}
		
		
		entitiesForUpdate.push_back(connectionEntity);
	}

	for (auto& _entityForUpdate : entitiesForUpdate) {
		_entityForUpdate.storeToDataBase();
		topoEntID.push_back(_entityForUpdate.getEntityID());
		topoEntVers.push_back(_entityForUpdate.getEntityStorageVersion());
		forceVis.push_back(false);
	}

	model->addEntitiesToModel(topoEntID, topoEntVers, forceVis, dataEntID, dataEntVers, dataEntParent, "Added connections", true, true, true);
	return ot::ReturnMessage::Ok;
}

