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

void BlockHandler::copyItem(const ot::GraphicsCopyInformation* _copyInformation)
{
	if(_modelComponent == nullptr)
	{
		setModelComponent(Application::instance()->modelComponent());
	}

	const auto itemInfos =	_copyInformation->getEntities();
	Model* model = Application::instance()->getModel();
	
	ot::UIDList entityIDsTopo;
	ot::UIDList entityVersionsTopo;
	ot::UIDList entityIDsData;
	ot::UIDList entityVersionsData;
	std::list<bool> forceVisible;

	ot::Point2DD topLeftPos(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	std::list<std::unique_ptr<EntityCoordinates2D>> coordinateEntities;

	for (const ot::CopyEntityInformation& itemInfo : itemInfos)
	{
		// First we clone the block entity
		ot::UID itemUID = itemInfo.getUid();
		EntityBase* baseEnt = model->getEntityByID(itemUID);
		if (baseEnt == nullptr) {
			throw std::exception(("Copied block with ID " + std::to_string(itemUID) + " does not exist in model.").c_str());
		}
		
		// Ensure entity type
		EntityBlock* blockEntityOrigin = dynamic_cast<EntityBlock*>(baseEnt);
		if (blockEntityOrigin == nullptr) {
			throw std::exception(("Copied block with ID " + std::to_string(itemUID) + " is not a block.").c_str());
		}

		// Clone block
		EntityBase* clonedEntity = blockEntityOrigin->clone();
		if (clonedEntity == nullptr) {
			throw std::exception("Block does not support copy function yet.");
		}
		std::unique_ptr<EntityBlock> newBlock(dynamic_cast<EntityBlock*>(clonedEntity));
		if (!newBlock.get()) {
			throw std::exception("Unexpected cloned entity type");
		}

		// Read coordinate entity
		ot::UID originCooEntityID = blockEntityOrigin->getCoordinateEntityID();

		std::map<ot::UID,EntityBase*> entMap;
		EntityBase* originCooEntity = model->readEntityFromEntityID(blockEntityOrigin, originCooEntityID, entMap);

		if (originCooEntity== nullptr) {
			throw std::exception(("Copied block with ID " + std::to_string(itemUID) + " does not have coordinates stored.").c_str());
		}

		// Ensure entity type
		EntityCoordinates2D* actualOriginCooEntity = dynamic_cast<EntityCoordinates2D*>(originCooEntity);
		if (!actualOriginCooEntity) {
			throw std::exception(("Copied block with ID " + std::to_string(itemUID) + " has unexpeceted coordinates stored.").c_str());
		}

		// Update top left coordiante
		topLeftPos.moveLeft(actualOriginCooEntity->getCoordinates().x()).moveTop(actualOriginCooEntity->getCoordinates().y());

		// Clone coordiante entity
		EntityBase* clonedCooEntity = actualOriginCooEntity->clone();
		assert(clonedCooEntity != nullptr);

		// Ensure cloned type
		EntityCoordinates2D* actualClonedCooEntity = dynamic_cast<EntityCoordinates2D*>(clonedCooEntity);
		if (!actualClonedCooEntity) {
			throw std::exception("Coordinate entity clone generated unexpected object type");
		}

		// Store cloned coordinate entity to move after reading all
		coordinateEntities.push_back(std::unique_ptr<EntityCoordinates2D>(actualClonedCooEntity));
		
		// Now we give the cloned coordinate entity new values
		coordinateEntities.back()->setEntityID(model->createEntityUID());

		//?Name

		// Now we give the cloned block entity new values
		newBlock->setCoordinateEntityID(coordinateEntities.back()->getEntityID());
		newBlock->setEntityID(model->createEntityUID());
		std::string path =	newBlock->getName();
		const std::string name = path.substr(path.find_last_of('/') +1);
		path = path.substr(0,path.find_last_of('/'));
		const std::string newName =	CreateNewUniqueTopologyName(path, name);
		newBlock->setName(newName);

		// Now we store the cloned block entity and prepare them for the add to model
		newBlock->StoreToDataBase();
		entityIDsTopo.push_back(newBlock->getEntityID());
		entityVersionsTopo.push_back(newBlock->getEntityStorageVersion());
		forceVisible.push_back(false);
	}

	// Check move distance
	ot::Point2DD moveAdder(0., 0.);
	if (_copyInformation->getScenePosSet()) {
		moveAdder = _copyInformation->getScenePos() - topLeftPos;
	}
	else {
		// If no scene pos is provided we move all block by 20
		moveAdder.set(20., 20.);
	}

	// Adjust coordinates
	for (auto& coord : coordinateEntities) {
		// Update coordinate
		coord->setCoordinates(coord->getCoordinates() + moveAdder);

		// Now we store the cloned entity and prepare it for adding to model
		coord->StoreToDataBase();
		entityIDsData.push_back(coord->getEntityID());
		entityVersionsData.push_back(coord->getEntityStorageVersion());
	}

	model->addEntitiesToModel(entityIDsTopo, entityVersionsTopo, forceVisible, entityIDsData, entityVersionsData, entityIDsTopo, "Copied block entities", true, false);
}

void BlockHandler::updateIdentifier(std::list<std::unique_ptr<EntityBase>>& _newEntities)
{
	//Rename
	//Set obs and parent zu nullptr
	//Set name

}

std::string BlockHandler::selectedEntitiesSerialiseAction(ot::JsonDocument& _document)
{
	ot::CopyInformation* info = ot::CopyInformationFactory::create(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	//SelectionHandler& selectionHandler = Application::instance()->getSelectionHandler();
	//ot::UIDList selectedEntities = selectionHandler.getSelectedEntityIDs();
	
	Model* model = Application::instance()->getModel();

	for (ot::CopyEntityInformation& entityInfo : info->getEntities()) {
		EntityBase* entity = model->getEntityByID(entityInfo.getUid());

		if (entity) {
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

std::string BlockHandler::pasteEntitiesAction(ot::JsonDocument& _document)
{
	Model* model = Application::instance()->getModel();
	ClassFactoryModel& classFactory =	model->getClassFactory();
	std::map<ot::UID, EntityBase*> entityMap;
	
	auto listOfConfigs = _document["ListOfSerialisedEntitiesConfigs"].GetObjectW();

	std::list<std::unique_ptr<EntityBase>> pasteEntities;
	for (auto& config : listOfConfigs)
	{
		std::string serialisedEntityJSON = _document.toJson();
		std::string_view serialisedEntityJSONView(serialisedEntityJSON);
		auto serialisedEntityBSON= bsoncxx::from_json(serialisedEntityJSONView);
		auto serialisedEntityBSONView =serialisedEntityBSON.view();
		std::string entityType = serialisedEntityBSON["SchemaType"].get_utf8().value.data();

		std::unique_ptr<EntityBase>entity(classFactory.CreateEntity(entityType));
		entity->restoreFromDataBase(nullptr, model, model->getStateManager(),serialisedEntityBSONView,entityMap);

		auto blockEntity =	dynamic_cast<EntityBlock*>(entity.get());
		if (blockEntity != nullptr)
		{
			auto coordinates =	config.value["Coordinates"].GetObjectW();
		}

		pasteEntities.push_back(std::move(entity));
	}



	return ot::ReturnMessage().toJson();
}