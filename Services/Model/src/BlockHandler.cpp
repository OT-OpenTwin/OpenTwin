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

void BlockHandler::copyItem(const ot::GraphicsCopyInformation* _copyInformation)
{
	if(_modelComponent == nullptr)
	{
		setModelComponent(Application::instance()->modelComponent());
	}

	const auto itemInfos =	_copyInformation->getItemInformation();
	Model* model = Application::instance()->getModel();
	
	ot::UIDList entityIDsTopo;
	ot::UIDList entityVersionsTopo;
	ot::UIDList entityIDsData;
	ot::UIDList entityVersionsData;
	std::list<bool> forceVisible;

	for (const ot::GraphicsCopyInformation::ItemInformation& itemInfo : itemInfos)
	{
		//First we clone the block entity
		ot::UID itemUID = itemInfo.uid;
		EntityBase* baseEnt = model->getEntityByID(itemUID);
		if (baseEnt == nullptr)
		{
			throw std::exception(("Copied block with ID " + std::to_string(itemUID) + " does not exist in model.").c_str());
		}
		
		EntityBlock* blockEntityOrigin = dynamic_cast<EntityBlock*>(baseEnt);
		if (blockEntityOrigin == nullptr)
		{
			throw std::exception(("Copied block with ID " + std::to_string(itemUID) + " is not a block.").c_str());
		}
		EntityBase *clonedEntity = blockEntityOrigin->clone();
		if (clonedEntity == nullptr)
		{
			throw std::exception("Block does not support copy function yet.");
		}
		
		std::unique_ptr<EntityBlock> newBlock(dynamic_cast<EntityBlock*>(clonedEntity));
		
		//Now we clone the coo data entity
		ot::UID originCooEntityID = blockEntityOrigin->getCoordinateEntityID();

		std::map<ot::UID,EntityBase*>	entMap;
		EntityBase* originCooEntity = model->readEntityFromEntityID(blockEntityOrigin, originCooEntityID, entMap);

		if (originCooEntity== nullptr)
		{
			throw std::exception(("Copied block with ID " + std::to_string(itemUID) + " does not have coordinates stored.").c_str());
		}

		EntityBase* clonedCooEntity = originCooEntity->clone();
		assert(clonedCooEntity != nullptr);
		std::unique_ptr<EntityCoordinates2D> newCoordinateEntity(dynamic_cast<EntityCoordinates2D*>(clonedCooEntity));
		
		//Now we give the cloned coo entity new values
		const ot::Point2DD& newPosition = itemInfo.pos;
		newCoordinateEntity->setCoordinates(newPosition);
		newCoordinateEntity->setEntityID(model->createEntityUID());
		//?Name

		//Now we give the cloned block entity new values
		newBlock->setCoordinateEntityID(newCoordinateEntity->getEntityID());
		newBlock->setEntityID(model->createEntityUID());
		std::string path =	newBlock->getName();
		const std::string name = path.substr(path.find_last_of('/') +1);
		path = path.substr(0,path.find_last_of('/'));
		const std::string newName =	CreateNewUniqueTopologyName(path, name);
		newBlock->setName(newName);
		//Now we store the cloned entities and prepare them for the add to model
		newBlock->StoreToDataBase();
		newCoordinateEntity->StoreToDataBase();
		entityIDsTopo.push_back(newBlock->getEntityID());
		entityVersionsTopo.push_back(newBlock->getEntityStorageVersion());
		entityIDsData.push_back(newCoordinateEntity->getEntityID());
		entityVersionsData.push_back(newCoordinateEntity->getEntityStorageVersion());
		forceVisible.push_back(false);
	}

	model->addEntitiesToModel(entityIDsTopo, entityVersionsTopo, forceVisible, entityIDsData, entityVersionsData, entityIDsTopo, "Copied block entities", true, false);
}

std::string BlockHandler::selectedEntitiesSerialiseAction(ot::JsonDocument& _document)
{
	SelectionHandler& selectionHandler = Application::instance()->getSelectionHandler();
	ot::UIDList selectedEntities = selectionHandler.getSelectedEntityIDs();
	
	Model* model = Application::instance()->getModel();
	std::list<std::string> serialisedEntities;
	for (ot::UID selectedEntityID : selectedEntities)
	{
		EntityBase* entity = model->getEntityByID(selectedEntityID);
		serialisedEntities.push_back(entity->serialiseAsJSON());
	}
	
	ot::JsonDocument document;
	ot::JsonArray serialisedEntitiesJson(serialisedEntities, document.GetAllocator());

	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	std::list < std::pair<ot::UID, ot::UID>> prefetchedDocs;
	model->sendMessageToViewer(document,prefetchedDocs);

	return ot::ReturnMessage(ot::ReturnMessage::Ok).toJson();
}

std::string BlockHandler::pasteEntitiesAction(ot::JsonDocument& _document)
{
	return std::string();
}