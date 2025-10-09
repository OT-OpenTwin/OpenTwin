// std header
#include <stdafx.h>

// Service header
#include "Model.h"
#include "Application.h"
#include "EntityBlock.h"
#include "BlockHandler.h"

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/FolderNames.h"
#include "OTCore/LogDispatcher.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTCommunication/ActionTypes.h"

BlockHandler::BlockHandler() {
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemChanged, this, &BlockHandler::handleItemChanged, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ConnectionChanged, this, &BlockHandler::handleConnectionChanged, ot::SECURE_MESSAGE_TYPES);
}

void BlockHandler::handleItemChanged(ot::JsonDocument& _doc) {
	std::string editorName = ot::json::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);
	ot::GraphicsItemCfg* itemConfig = ot::GraphicsItemCfgFactory::instance().createFromJSON(ot::json::getObject(_doc, OT_ACTION_PARAM_Config), OT_JSON_MEMBER_GraphicsItemCfgType);
	if (!itemConfig) {
		OT_LOG_E("ItemConfig is null");
		return;
	}

	const ot::UID blockID = itemConfig->getUid();
	const ot::Transform transform = itemConfig->getTransform();
	
	Model* _model = Application::instance()->getModel();
	auto entBase = _model->getEntityByID(blockID);
    EntityBlock* blockEnt = dynamic_cast<EntityBlock*>(entBase);
	if (!blockEnt) {
		OT_LOG_E("BlockEntity is null");
		return;
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
		return;
	}
	if (coordinateEntity->getCoordinates() != itemConfig->getPosition()) {
		coordinateEntity->setCoordinates(itemConfig->getPosition());
		_positionChanged = true;
	}




	// Now check the cases and do the update or add 
	if ((_rotationChanged || _flipChanged) && _positionChanged) {
		ot::UIDList topoEntID, topoEntVers, dataEntID, dataEntVers, dataEntParent;
		std::list<bool> forceVis;

		blockEnt->StoreToDataBase();

		dataEntID.push_back(coordinateEntity->getEntityID());
		dataEntVers.push_back(coordinateEntity->getEntityStorageVersion());
		dataEntParent.push_back(parent->getEntityID());
		topoEntID.clear();
		topoEntVers.clear();
		forceVis.clear();

		_model->addEntitiesToModel(topoEntID, topoEntVers, forceVis, dataEntID, dataEntVers, dataEntParent, "Update BlockItem position and rotation", false, false);

		coordinateEntity->StoreToDataBase();
		topoEntID.push_back(blockEnt->getEntityID());
		topoEntVers.push_back(blockEnt->getEntityStorageVersion());
		forceVis.push_back(false);

		_model->updateTopologyEntities(topoEntID, topoEntVers, "Rotation and blockitem position changed");

	}
	else if (_rotationChanged || _flipChanged) {
		blockEnt->StoreToDataBase();
		const std::string comment = "Rotation and Flip updated";
		std::list<ot::UID>  topologyEntityIDList{ blockEnt->getEntityID() };
		std::list<ot::UID>  topologyEntityVersionList { blockEnt->getEntityStorageVersion() };
	
		_model->updateTopologyEntities(topologyEntityIDList, topologyEntityVersionList, comment);
	}
	else {
		coordinateEntity->StoreToDataBase();
		
		ot::UIDList topoEntID, topoEntVers, dataEntID, dataEntVers, dataEntParent;
		std::list<bool> forceVis;

		dataEntID.push_back(coordinateEntity->getEntityID());
		dataEntVers.push_back(coordinateEntity->getEntityStorageVersion());

		EntityBase* parent = coordinateEntity->getParent();
		if (!parent) {
			OT_LOG_E("Coordinate entity has no parent");
			return;
		}
		std::list<ot::UID> dataEntityParentList{ parent->getEntityID()};
		
		_model->addEntitiesToModel(topoEntID, topoEntVers, forceVis, dataEntID, dataEntVers, dataEntityParentList, "Update BlockItem position", false, false);
		topoEntID.push_back(blockEnt->getEntityID());
		topoEntVers.push_back(blockEnt->getEntityStorageVersion());
		_model->updateTopologyEntities(topoEntID, topoEntVers, "BlockPositionUpdated");
	}
}

void BlockHandler::handleConnectionChanged(ot::JsonDocument& _doc) {
	ot::GraphicsConnectionCfg connection;
	connection.setFromJsonObject(ot::json::getObject(_doc, OT_ACTION_PARAM_Config));
	std::string editorName = ot::json::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);



}
