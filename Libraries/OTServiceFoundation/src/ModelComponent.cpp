//! @file ModelComponent.cpp
//! @authors Alexander Kuester (alexk95), Peter Thoma
//! @date March 2021
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/ApplicationBase.h"

#include "EntityBase.h"
#include "EntityResultText.h"
#include "EntityGeometry.h"

#include <DataBase.h>

// std header
#include <iostream>
#include <cassert>

ot::components::ModelComponent::ModelComponent(const ot::ServiceBase& _serviceInfo, ApplicationBase* _application)
	:uniqueUIDGenerator(nullptr),
	ServiceBase(_serviceInfo), m_application(_application)
{ 
	assert(m_application); 

	uniqueUIDGenerator = new DataStorageAPI::UniqueUIDGenerator(_application->getSessionCount(), static_cast<unsigned int>(_application->getServiceID()));

	EntityBase::setUidGenerator(uniqueUIDGenerator);
}

ot::components::ModelComponent::~ModelComponent()
{
	assert(uniqueUIDGenerator != nullptr);
	delete uniqueUIDGenerator;
	uniqueUIDGenerator = nullptr;

	EntityBase::setUidGenerator(nullptr);
}

EntityResultText *ot::components::ModelComponent::addResultTextEntity(const std::string &name, const std::string &text)
{
	EntityResultText *textItem = new EntityResultText(createEntityUID(), nullptr, nullptr, nullptr, "Model");

	textItem->setName(name);
	textItem->setEditable(false);

	// Now we store the data in the entity
	textItem->setText(text);

	// Release the data from memory (this will write it to the data base)
	textItem->releaseTextData();

	// Store the entity in the data base as well.
	textItem->storeToDataBase();

	// Return the entity ID of the newly created entity
	return textItem;
}

void ot::components::ModelComponent::facetAndStoreGeometryEntity(EntityGeometry *entityGeom, bool forceVisible)
{
	UID entityID = createEntityUID();
	UID brepID   = createEntityUID();
	UID facetsID = createEntityUID();

	entityGeom->setEntityID(entityID);

	entityGeom->getBrepEntity()->setEntityID(brepID);

	entityGeom->getFacets()->setEntityID(facetsID);
	entityGeom->facetEntity(false);

	entityGeom->storeToDataBase();

	addNewTopologyEntity(entityGeom->getEntityID(), entityGeom->getEntityStorageVersion(), forceVisible);
	addNewDataEntity(entityGeom->getBrepEntity()->getEntityID(), entityGeom->getBrepEntity()->getEntityStorageVersion(), entityGeom->getEntityID());
	addNewDataEntity(entityGeom->getFacets()->getEntityID(), entityGeom->getFacets()->getEntityStorageVersion(), entityGeom->getEntityID());
}

ot::UID ot::components::ModelComponent::createEntityUID(void)
{
	return getUniqueUIDGenerator()->getUID();
}

DataStorageAPI::UniqueUIDGenerator* ot::components::ModelComponent::getUniqueUIDGenerator(void) 
{
	assert(uniqueUIDGenerator != nullptr);

	return uniqueUIDGenerator;
}

void ot::components::ModelComponent::importTableFile(const std::string &itemName)
{
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_RequestImportTableFile, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_NAME, JsonString(itemName, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->getServiceURL(), this->getServiceURL(), ot::EXECUTE, requestDoc.toJson(), response)) {
		std::cout << "ERROR: Failed to get new entity id's: Failed to send HTTP request" << std::endl;
		return;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::cout << "ERROR: Failed to get new entity id's: " << response << std::endl;
		return;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::cout << "ERROR: Failed to get new entity id's: " << response << std::endl;
		return;
	}
}

bool  ot::components::ModelComponent::sendMessage(bool _queue, JsonDocument & _doc, std::string& _response)
{
	return m_application->sendMessage(_queue, OT_INFO_SERVICE_TYPE_MODEL, _doc, _response);
}

void ot::components::ModelComponent::clearNewEntityList(void)
{
	topologyEntityIDList.clear();
	topologyEntityVersionList.clear();
	topologyEntityForceVisible.clear();

	dataEntityIDList.clear();
	dataEntityVersionList.clear();
	dataEntityParentList.clear();
}

void ot::components::ModelComponent::addNewTopologyEntity(UID entityID, UID entityVersion, bool forceVisible)
{
	topologyEntityIDList.push_back(entityID);
	topologyEntityVersionList.push_back(entityVersion);
	topologyEntityForceVisible.push_back(forceVisible);
}

void ot::components::ModelComponent::addNewDataEntity(UID entityID, UID entityVersion, UID entityParentID)
{
	dataEntityIDList.push_back(entityID);
	dataEntityVersionList.push_back(entityVersion);
	dataEntityParentList.push_back(entityParentID);
}

void ot::components::ModelComponent::storeNewEntities(const std::string &description, bool askForBranchCreation, bool saveModel)
{
	ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, description, askForBranchCreation, saveModel);
	clearNewEntityList();
}

void ot::components::ModelComponent::loadMaterialInformation()
{
	materialIDToNameMap.clear();
	materialNameToIDMap.clear();

	ot::UIDList materialIDList = ModelServiceAPI::getIDsOfFolderItemsOfType("Materials", "EntityMaterial", true);
	
	std::list<ot::EntityInformation> materialInformation;
	ModelServiceAPI::getEntityInformation(materialIDList, materialInformation);

	for (auto matItem : materialInformation)
	{
		materialIDToNameMap[matItem.getEntityID()]   = matItem.getEntityName();
		materialNameToIDMap[matItem.getEntityName()] = matItem.getEntityID();
	}
}

std::string ot::components::ModelComponent::getCurrentMaterialName(EntityPropertiesEntityList *material)
{
	// First, we search the material from the entityID
	auto item = materialIDToNameMap.find(material->getValueID());

	if (item != materialIDToNameMap.end())
	{
		// The material id still exists
		return item->second;
	}

	// The material id does not exist, search it by name
	if (materialNameToIDMap.count(material->getValueName()) != 0)
	{
		// The material does exist
		return material->getValueName();
	}

	// The material could not be found
	return "";
}
