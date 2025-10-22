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

	bool _rotationChanged = false;
	bool _flipChanged = false;
	bool _positionChanged = false;

	// Here i will update the rotation
	auto propertyBase = blockEnt->getProperties().getProperty("Rotation");
	if (propertyBase != nullptr) {
		auto propertyRotation = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
		if (propertyRotation->getValue() != transform.getRotation()) {
			propertyRotation->setValue(transform.getRotation());
			_rotationChanged = true;
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
		if (propertyFlip->getValue() != stringFlipMap[transform.getFlipStateFlags()]) {
			propertyFlip->setValue(stringFlipMap[transform.getFlipStateFlags()]);
			_flipChanged = true;
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
		_positionChanged = true;
	}




	// Now check the cases and do the update or add 
	if ((_rotationChanged || _flipChanged) && _positionChanged) {
		ot::UIDList topoEntID, topoEntVers, dataEntID, dataEntVers, dataEntParent;
		std::list<bool> forceVis;

		blockEnt->storeToDataBase();

		dataEntID.push_back(coordinateEntity->getEntityID());
		dataEntVers.push_back(coordinateEntity->getEntityStorageVersion());
		dataEntParent.push_back(parent->getEntityID());
		topoEntID.clear();
		topoEntVers.clear();
		forceVis.clear();

		_model->addEntitiesToModel(topoEntID, topoEntVers, forceVis, dataEntID, dataEntVers, dataEntParent, "Update BlockItem position and rotation", false, false, false);

		coordinateEntity->storeToDataBase();
		topoEntID.push_back(blockEnt->getEntityID());
		topoEntVers.push_back(blockEnt->getEntityStorageVersion());
		forceVis.push_back(false);

		_model->updateTopologyEntities(topoEntID, topoEntVers, "Rotation and blockitem position changed", false);

	}
	else if (_rotationChanged || _flipChanged) {
		blockEnt->storeToDataBase();
		const std::string comment = "Rotation and Flip updated";
		std::list<ot::UID>  topologyEntityIDList{ blockEnt->getEntityID() };
		std::list<ot::UID>  topologyEntityVersionList{ blockEnt->getEntityStorageVersion() };

		_model->updateTopologyEntities(topologyEntityIDList, topologyEntityVersionList, comment, false);
	}
	else {
		coordinateEntity->storeToDataBase();
		//blockEnt->storeToDataBase();

		ot::UIDList topoEntID, topoEntVers, dataEntID, dataEntVers, dataEntParent;
		std::list<bool> forceVis;

		dataEntID.push_back(coordinateEntity->getEntityID());
		dataEntVers.push_back(coordinateEntity->getEntityStorageVersion());

		EntityBase* parent = coordinateEntity->getParent();
		if (!parent) {
			OT_LOG_E("Coordinate entity has no parent");
			return ot::ReturnMessage(ot::ReturnMessage::Failed, "Coordinate entity has no parent");
		}
		std::list<ot::UID> dataEntityParentList{ parent->getEntityID() };

		_model->addEntitiesToModel(topoEntID, topoEntVers, forceVis, dataEntID, dataEntVers, dataEntityParentList, "Update BlockItem position", false, false, false);
		topoEntID.push_back(blockEnt->getEntityID());
		topoEntVers.push_back(blockEnt->getEntityStorageVersion());
		_model->updateTopologyEntities(topoEntID, topoEntVers, "BlockPositionUpdated", false);
	}

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
	ot::UIDList topoEntID, topoEntVers;
	connectionEntity->storeToDataBase();
	topoEntID.push_back(connectionEntity->getEntityID());
	topoEntVers.push_back(connectionEntity->getEntityStorageVersion());
	_model->updateTopologyEntities(topoEntID, topoEntVers, "Update connection", false);

	return ot::ReturnMessage::Ok;

}

