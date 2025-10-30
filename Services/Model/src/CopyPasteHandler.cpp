// @otlicense
// File: CopyPasteHandler.cpp
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

#include <stdafx.h>
#include "CopyPasteHandler.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCore/LogDispatcher.h"
#include "OTGui/CopyInformation.h"
#include "Application.h"
#include "Model.h"
#include "EntityBlock.h"
#include "OTCore/ReturnMessage.h"
#include "SelectionHandler.h"

#pragma warning(disable:4996)

CopyPasteHandler::CopyPasteHandler() {
	connectAction(OT_ACTION_CMD_SelectedEntitiesSerialise, this, &CopyPasteHandler::selectedEntitiesSerialiseAction);
	connectAction(OT_ACTION_CMD_PasteEntities, this, &CopyPasteHandler::pasteEntitiesAction);
}

void CopyPasteHandler::storeEntities(std::map<ot::UID, EntityBase*>& _newEntitiesByName)
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
		if (counter == safetyLimit)
		{
			OT_LOG_E("Infinit loop protection. Failed to execute a paste of " + oldName+ " because of entitíes number of entities with the same postfix exceed number of : " + std::to_string(safetyLimit));
		}
		else
		{
			entityNames[newName] = true;
			newEntity->setName(newName);
		
			newEntity->storeToDataBase();

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
	}
	
	model->addEntitiesToModel(topoEntID, topoEntVers, forceVis, dataEntID, dataEntVers, dataEntParent, "Copy+Paste Entities", true,false,true);
}

std::string CopyPasteHandler::selectedEntitiesSerialiseAction(ot::JsonDocument& _document) {
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
			OT_LOG_E("Selected Entity " + entity->getName() + " cannot be copied.");
		}
	}
	
	ot::JsonDocument responseDocument;
	info.addToJsonObject(responseDocument, responseDocument.GetAllocator());
	return ot::ReturnMessage(ot::ReturnMessage::Ok, responseDocument.toJson()).toJson();
}

std::string CopyPasteHandler::pasteEntitiesAction(ot::JsonDocument& _document) {
	ot::CopyInformation info(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	// Get class factory
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);
	EntityFactory& factory = EntityFactory::instance();
	std::map<ot::UID, EntityBase*> entityMap;

	// Deserialize entities
	for (const ot::CopyEntityInformation& entityInfo : info.getEntities()) {
		const std::string serialisedEntities = entityInfo.getRawData();
		if (!serialisedEntities.empty())
		{
			ot::JsonDocument document;
			document.fromJson(serialisedEntities);
			std::string entityType = ot::json::getString(document, "SchemaType");
			std::unique_ptr<EntityBase> entity (factory.create(entityType));
					
			if (entity != nullptr)
			{
				bool serialisedSuccessfully = entity->deserialiseFromJSON(document.getConstObject(),info, entityMap);
				if (serialisedSuccessfully)
				{
					//If successfull, a pointer of the entity is stored in the entitymap during deserialisation.
					entity.release();
				}
				else
				{
					OT_LOG_E("Failed to create entity from paste information.");
				}
			}
			else
			{
				OT_LOG_E("Failed to create entity from paste information.");
			}
		}
		else
		{
			OT_LOG_E("No data for pasting an entity.");
		}
	}

	storeEntities(entityMap);
	return ot::ReturnMessage().toJson();
}