#include <stdafx.h>
#include "BlockHandler.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCore/Logger.h"
#include "OTCore/CopyInformationFactory.h"
#include "Application.h"
#include "Model.h"
#include "EntityBlock.h"
#include "OTCore/ReturnMessage.h"
#include "SelectionHandler.h"

#pragma warning(disable:4996)

void BlockHandler::updateIdentifier(std::list<std::unique_ptr<EntityBase>>& _newEntities) {
	//Rename
	//Set obs and parent zu nullptr
	//Set name

}

std::string BlockHandler::selectedEntitiesSerialiseAction(ot::JsonDocument& _document) {
	ot::CopyInformation* info = ot::CopyInformationFactory::create(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	//SelectionHandler& selectionHandler = Application::instance()->getSelectionHandler();
	//ot::UIDList selectedEntities = selectionHandler.getSelectedEntityIDs();
	
	Model* model = Application::instance()->getModel();

	for (ot::CopyEntityInformation& entityInfo : info->getEntities()) {
		// Find entity
		EntityBase* entity = model->getEntityByID(entityInfo.getUid());

		if (entity) {
			// Serialize entity

			// #########################################################################################################################################

			// !!!!!    Here the serializeAsJSON would include the coordinate entity ID of a block but we need the coordinate serialized aswell   !!!!!
			entityInfo.setRawData(entity->serialiseAsJSON());
		}
		else {
			OT_LOG_E("Failed to load entity (" + std::to_string(entityInfo.getUid()) + ")");
			return ot::ReturnMessage(ot::ReturnMessage::Failed).toJson();
		}
	}
	
	ot::JsonDocument responseDocument;
	info->addToJsonObject(responseDocument, responseDocument.GetAllocator());
	return ot::ReturnMessage(ot::ReturnMessage::Ok, responseDocument.toJson()).toJson();
}

std::string BlockHandler::pasteEntitiesAction(ot::JsonDocument& _document) {
	const ot::CopyInformation* info = ot::CopyInformationFactory::create(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	// Get class factory
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);
	ClassFactoryModel& classFactory = model->getClassFactory();
	std::map<ot::UID, EntityBase*> entityMap;

	// Setup info for graphics paste
	ot::Point2DD graphicsTopLeft(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	std::list<EntityCoordinates2D*> blockCoordinateEntities;

	// Deserialize entities
	for (const ot::CopyEntityInformation& entityInfo : info->getEntities()) {
		OTAssert(entityInfo.getRawData().empty(), "Empty entity data stored");

		// Create bson view from json
		std::string_view serialisedEntityJSONView(entityInfo.getRawData());
		auto serialisedEntityBSON = bsoncxx::from_json(serialisedEntityJSONView);
		auto serialisedEntityBSONView = serialisedEntityBSON.view();

		std::string entityType = serialisedEntityBSON["SchemaType"].get_utf8().value.data();
		OTAssert(entityType.empty(), "Empty entity type");

		std::unique_ptr<EntityBase>entity(classFactory.CreateEntity(entityType));

		// ########################################################################################################################

		// !!!!!   If ithis is a block, the UID of the coordinate entity is pointing to the origin coordinate   !!!!!
		entity->restoreFromDataBase(nullptr, model, model->getStateManager(), serialisedEntityBSONView, entityMap);

		EntityBlock* blockEntity = dynamic_cast<EntityBlock*>(entity.get());
		
		if (blockEntity != nullptr) {
			// ########################################################################################################################

			// !!!!!   note that curently the coordinate entity id of the copy origin is stored   !!!!!
			ot::UID coordinateEntityId = blockEntity->getCoordinateEntityID();
			EntityCoordinates2D* coordinateEntity = dynamic_cast<EntityCoordinates2D*>(model->getEntityByID(coordinateEntityId));
			if (!coordinateEntity) {
				OT_LOG_E("Block entity has an invalid coordinate entity");
				return ot::ReturnMessage(ot::ReturnMessage::Failed).toJson();;
			}

			blockCoordinateEntities.push_back(coordinateEntity);
			OTAssert(coordinateEntity->getCoordinates().x() != std::numeric_limits<double>::max(), "Invalid x coordinate value stored");
			OTAssert(coordinateEntity->getCoordinates().y() != std::numeric_limits<double>::max(), "Invalid y coordinate value stored");
			graphicsTopLeft.moveTop(coordinateEntity->getCoordinates().y()).moveLeft(coordinateEntity->getCoordinates().x());
		}
	}

	// Check move distance
	if (!blockCoordinateEntities.empty()) {
		const ot::GraphicsCopyInformation* graphicsInfo = dynamic_cast<const ot::GraphicsCopyInformation*>(info);
		if (!graphicsInfo) {
			OT_LOG_EA("Block entities found but the paste information is not a graphics copy information");
			return ot::ReturnMessage(ot::ReturnMessage::Failed).toJson();
		}

		// Calculate distance 
		ot::Point2DD moveAdder(0., 0.);
		if (graphicsInfo->getScenePosSet()) {
			moveAdder = graphicsInfo->getScenePos() - graphicsTopLeft;
		}
		else {
			// If no scene pos is provided we move all block by 20
			moveAdder.set(20., 20.);
		}

		// Adjust coordinates
		for (EntityCoordinates2D* coord : blockCoordinateEntities) {
			// Update coordinate
			coord->setCoordinates(coord->getCoordinates() + moveAdder);

			//coord->StoreToDataBase();
		}
	}

	//model->addEntitiesToModel(entityIDsTopo, entityVersionsTopo, topoForceVisible, entityIDsData, entityVersionsData, entityIDsTopo, "Copied block entities", true, false);

	ot::JsonDocument responseDocument;
	info->addToJsonObject(responseDocument, responseDocument.GetAllocator());

	return ot::ReturnMessage(ot::ReturnMessage::Ok, responseDocument.toJson()).toJson();
}