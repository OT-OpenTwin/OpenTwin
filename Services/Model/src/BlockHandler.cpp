#include <stdafx.h>
#include "BlockHandler.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCore/Logger.h"
#include "OTGui/CopyInformation.h"
#include "Application.h"
#include "Model.h"
#include "EntityBlock.h"
#include "OTCore/ReturnMessage.h"
#include "SelectionHandler.h"

#pragma warning(disable:4996)

void BlockHandler::updateIdentifier(std::list<std::unique_ptr<EntityBase>>& _newEntities) 
{
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);
	std::list<std::string> newEntityNames;
	for (auto& newEntity : _newEntities)
	{
		newEntity->setEntityID(model->createEntityUID());
		
		const std::string name = newEntity->getName();
		const std::string folderName = name.substr(name.find_last_of("/") + 1);
		std::list<std::string> folderContent = model->getListOfFolderItems(folderName,false);
		folderContent.insert(folderContent.end(), newEntityNames.begin(), newEntityNames.end()); //Names from the other new entities are not known to the model yet
		const std::string newName =	CreateNewUniqueTopologyName(folderContent, folderName, name);
		newEntityNames.push_back(newName);
		newEntity->setName(newName);
	}
}

void BlockHandler::storeEntities(std::list<std::unique_ptr<EntityBase>>& _newEntities)
{
	ot::UIDList topoEntID, topoEntVers, dataEntID, dataEntVers, dataEntParent;
	std::list<bool> forceVis;

	for (auto& newEntity : _newEntities)
	{
		newEntity->StoreToDataBase();
		topoEntID.push_back(newEntity->getEntityID());
		topoEntVers.push_back(newEntity->getEntityStorageVersion());
		forceVis.push_back(false);
	}
	
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);
	model->addEntitiesToModel(topoEntID, topoEntVers, forceVis, dataEntID, dataEntVers, dataEntParent, "Copy+Paste Entities", true,false);
}

std::string BlockHandler::selectedEntitiesSerialiseAction(ot::JsonDocument& _document) {
	ot::CopyInformation info = ot::json::getObject(_document, OT_ACTION_PARAM_Config);

	if (!info.getEntities().empty()) {
		OT_LOG_E("Provided copy information already contains entities. This is unexpected here and the existing information will be replaced");
		info.clearEntities();
	}

	SelectionHandler& selectionHandler = Application::instance()->getSelectionHandler();
	ot::UIDList selectedEntities = selectionHandler.getSelectedEntityIDs();
	
	Model* model = Application::instance()->getModel();

	for (ot::UID uid : selectedEntities) {
		// Find entity
		EntityBase* entity = model->getEntityByID(uid);

		if (entity) {
			// Serialize entity

			// #########################################################################################################################################

			// !!!!!    Here the serializeAsJSON would include the coordinate entity ID of a block but we need the coordinate serialized aswell   !!!!!
			ot::CopyEntityInformation entityInfo;
			entityInfo.setName(entity->getName());
			entityInfo.setUid(entity->getEntityID());
			entityInfo.setRawData(entity->serialiseAsJSON());
			info.addEntity(entityInfo);
		}
		else {
			OT_LOG_E("Failed to load entity from uid (" + std::to_string(uid) + ")");
			return ot::ReturnMessage(ot::ReturnMessage::Failed).toJson();
		}
	}
	
	ot::JsonDocument responseDocument;
	info.addToJsonObject(responseDocument, responseDocument.GetAllocator());
	return ot::ReturnMessage(ot::ReturnMessage::Ok, responseDocument.toJson()).toJson();
}

std::string BlockHandler::pasteEntitiesAction(ot::JsonDocument& _document) {
	const ot::CopyInformation info(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	// Get class factory
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);
	ClassFactoryModel& classFactory = model->getClassFactory();
	std::map<ot::UID, EntityBase*> entityMap;

	// Setup info for graphics paste
	/*ot::Point2DD graphicsTopLeft(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	std::list<EntityCoordinates2D*> blockCoordinateEntities;*/

	std::list<std::unique_ptr<EntityBase>> newEntities;
	// Deserialize entities
	for (const ot::CopyEntityInformation& entityInfo : info.getEntities()) {
		OTAssert(entityInfo.getRawData().empty(), "Empty entity data stored");

		// Create bson view from json
		std::string_view serialisedEntityJSONView(entityInfo.getRawData());
		auto serialisedEntityBSON = bsoncxx::from_json(serialisedEntityJSONView);
		auto serialisedEntityBSONView = serialisedEntityBSON.view();

		std::string entityType = serialisedEntityBSON["SchemaType"].get_utf8().value.data();
		OTAssert(entityType.empty(), "Empty entity type");

		std::unique_ptr<EntityBase>entity(classFactory.CreateEntity(entityType));

		// ########################################################################################################################
		entity->restoreFromDataBase(nullptr, model, model->getStateManager(), serialisedEntityBSONView, entityMap);
		newEntities.push_back(std::move(entity));
		//EntityBlock* blockEntity = dynamic_cast<EntityBlock*>(entity.get());
		//
		//if (blockEntity != nullptr) {
		//	// ########################################################################################################################

		//	// !!!!!   note that curently the coordinate entity id of the copy origin is stored   !!!!!
		//	ot::UID coordinateEntityId = blockEntity->getCoordinateEntityID();
		//	EntityCoordinates2D* coordinateEntity = dynamic_cast<EntityCoordinates2D*>(model->getEntityByID(coordinateEntityId));
		//	if (!coordinateEntity) {
		//		OT_LOG_E("Block entity has an invalid coordinate entity");
		//		return ot::ReturnMessage(ot::ReturnMessage::Failed).toJson();;
		//	}

		//	blockCoordinateEntities.push_back(coordinateEntity);
		//	OTAssert(coordinateEntity->getCoordinates().x() != std::numeric_limits<double>::max(), "Invalid x coordinate value stored");
		//	OTAssert(coordinateEntity->getCoordinates().y() != std::numeric_limits<double>::max(), "Invalid y coordinate value stored");
		//	graphicsTopLeft.moveTop(coordinateEntity->getCoordinates().y()).moveLeft(coordinateEntity->getCoordinates().x());
		//}
	}

	updateIdentifier(newEntities);

	//// Check move distance
	//if (!blockCoordinateEntities.empty()) {
	//	// Calculate distance 
	//	ot::Point2DD moveAdder(0., 0.);
	//	if (info.getDestinationScenePosSet()) {
	//		moveAdder = info.getDestinationScenePos() - graphicsTopLeft;
	//	}
	//	else {
	//		// If no scene pos is provided we move all block by 20
	//		moveAdder.set(20., 20.);
	//	}

	//	// Adjust coordinates
	//	for (EntityCoordinates2D* coord : blockCoordinateEntities) {
	//		// Update coordinate
	//		coord->setCoordinates(coord->getCoordinates() + moveAdder);

	//		//coord->StoreToDataBase();
	//	}
	//}

	//model->addEntitiesToModel(entityIDsTopo, entityVersionsTopo, topoForceVisible, entityIDsData, entityVersionsData, entityIDsTopo, "Copied block entities", true, false);

	/*ot::JsonDocument responseDocument;
	info.addToJsonObject(responseDocument, responseDocument.GetAllocator());

	return ot::ReturnMessage(ot::ReturnMessage::Ok, responseDocument.toJson()).toJson();*/
	return ot::ReturnMessage().toJson();
}