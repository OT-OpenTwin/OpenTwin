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
		
		const std::string fullName = newEntity->getName();
		const std::string folderName = fullName.substr(0,fullName.find_last_of("/"));

		const std::string name = fullName.substr(fullName.find_last_of("/") + 1,fullName.size());
		std::list<std::string> folderContent = model->getListOfFolderItems(folderName,false);
		folderContent.insert(folderContent.end(), newEntityNames.begin(), newEntityNames.end()); //Names from the other new entities are not known to the model yet
		const std::string newName =	CreateNewUniqueTopologyName(folderContent, folderName, name);
		newEntityNames.push_back(newName);
		newEntity->setName(newName);
		
		//Important! These are set later when the entity is added to the state. If they are set here, it causes issues when the object is destroyed.
		newEntity->setObserver(nullptr);
		newEntity->setParent(nullptr);
	}
}

void BlockHandler::storeEntities(std::map<ot::UID, EntityBase*>& _newEntitiesByName)
{
	ot::UIDList topoEntID, topoEntVers, dataEntID, dataEntVers, dataEntParent;
	std::list<bool> forceVis;

	Model* model = Application::instance()->getModel();
	auto entityNames = model->getListOfEntityNames();
	const uint32_t safetyLimit = 100000;
	for (auto& newEntityByName : _newEntitiesByName)
	{
		EntityBase* newEntity = newEntityByName.second;
		const std::string oldName = newEntity->getName();
		uint32_t counter(0);
		std::string newName = oldName +"_" +std::to_string(counter);
		while (entityNames.find(newName) != entityNames.end() && counter < safetyLimit)
		{
			counter++;
			newName = oldName + "_" + std::to_string(counter);
		}
		entityNames[newName] = true;
		newEntity->setName(newName);
		
		newEntity->StoreToDataBase();

		if (newEntity->getEntityType() == EntityBase::entityType::TOPOLOGY)
		{
			topoEntID.push_back(newEntity->getEntityID());
			topoEntVers.push_back(newEntity->getEntityStorageVersion());
			forceVis.push_back(false);
		}
		else
		{
			dataEntID.push_back(newEntity->getEntityID());
			dataEntVers.push_back(newEntity->getEntityStorageVersion());
			EntityBase* parent = newEntity->getParent();
			assert(parent != nullptr);
			dataEntParent.push_back(parent->getEntityID());
		}
	}
	
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
		const std::string serialisedEntity = entity->serialiseAsJSON();
		if (!serialisedEntity.empty()) 
		{	
			ot::CopyEntityInformation entityInfo;
			entityInfo.setName(entity->getName());
			entityInfo.setUid(entity->getEntityID());
			entityInfo.setRawData(serialisedEntity);
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
	ot::CopyInformation info(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	// Get class factory
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);
	ClassFactoryModel& classFactory = model->getClassFactory();
	std::map<ot::UID, EntityBase*> entityMap;

	// Deserialize entities
	for (const ot::CopyEntityInformation& entityInfo : info.getEntities()) {
		const std::string serialisedEntities = entityInfo.getRawData();
		if (!serialisedEntities.empty())
		{
			ot::JsonDocument document;
			document.fromJson(serialisedEntities);
			std::string entityType = ot::json::getString(document, "SchemaType");
			std::unique_ptr<EntityBase> entity (classFactory.CreateEntity(entityType));
					
			if (entity != nullptr)
			{
				// ########################################################################################################################
				bool serialisedSuccessfully = entity->deserialiseFromJSON(document.GetConstObject(),info, entityMap);
				entity.release();
			}
			else
			{
				OT_LOG_E("Failed to create entity from paste information.");
			}
		}
		else
		{
			OT_LOG_E("Tried to paste an entity without data.");
		}
	}

	storeEntities(entityMap);
	return ot::ReturnMessage().toJson();
}