/*
 *  ModelComponent.cpp
 *
 *  Created on: 08/02/2021
 *	Author: Alexander Kuester, Peter Thoma
 *  Copyright (c) 2021, OpenTwin
 */

// OpenTwin header
#include "OTCommunication/Msg.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/ApplicationBase.h"

#include "EntityBase.h"
#include "EntityResult1DPlot.h"
#include "EntityResult1DCurve.h"
#include "EntityResult1DCurveData.h"
#include "EntityResultText.h"
#include "EntityGeometry.h"

#include <DataBase.h>
#include <ClassFactory.h>

// std header
#include <iostream>
#include <cassert>

ot::components::ModelComponent::ModelComponent(
	const std::string &			_name,
	const std::string &			_type,
	const std::string &			_url,
	serviceID_t					_id,
	ApplicationBase *			_application
)
	:uniqueUIDGenerator(nullptr),
	ServiceBase{ _name, _type, _url, _id },  m_application{ _application }
{ 
	assert(m_application); 

	uniqueUIDGenerator = new DataStorageAPI::UniqueUIDGenerator(_application->getSessionCount(), _application->getServiceIDAsInt());

	EntityBase::setUidGenerator(uniqueUIDGenerator);
}

ot::components::ModelComponent::~ModelComponent()
{
	assert(uniqueUIDGenerator != nullptr);
	delete uniqueUIDGenerator;
	uniqueUIDGenerator = nullptr;

	EntityBase::setUidGenerator(nullptr);
}

EntityResult1DPlot*ot::components::ModelComponent::addResult1DPlotEntity(const std::string &name, const std::string &title, const std::list<std::pair<UID,std::string>> &curves)
{
	EntityResult1DPlot*plot = new EntityResult1DPlot(createEntityUID(), nullptr, nullptr, nullptr, nullptr, "Model");

	plot->setName(name);
	plot->setEditable(true);
	plot->createProperties();

	plot->setTitle(title);
	
	for (auto curve : curves)
	{
		plot->addCurve(curve.first, curve.second);
	}

	// Store the entity in the data base as well.
	plot->StoreToDataBase();

	// Return the entity ID of the newly created entity
	return plot;
}

EntityResult1DCurve *ot::components::ModelComponent::addResult1DCurveEntity(const std::string &name, const std::vector<double> &xdata, 
	const std::vector<double> &ydataRe, const std::vector<double> &ydataIm,
	const std::string &xlabel, const std::string &xunit,
	const std::string &ylabel, const std::string &yunit, int colorID, bool visible)
{
	EntityResult1DCurve *curve = new EntityResult1DCurve(createEntityUID(), nullptr, nullptr, nullptr, nullptr, "Model");

	curve->setName(name);
	curve->setEditable(true);
	curve->createProperties();
	
	curve->setCreateVisualizationItem(visible);

	curve->setXLabel(xlabel);
	curve->setYLabel(ylabel);
	curve->setXUnit(xunit);
	curve->setYUnit(yunit);
	curve->setColorFromID(colorID);

	// Now we store the data in the entity
	curve->setCurveXData(xdata);
	curve->setCurveYData(ydataRe, ydataIm);

	// Release the data from memory (this will write it to the data base)
	curve->releaseCurveData();

	// Store the entity in the data base as well.
	curve->StoreToDataBase();

	// Return the entity ID of the newly created entity
	return curve;
}

EntityResultText *ot::components::ModelComponent::addResultTextEntity(const std::string &name, const std::string &text)
{
	EntityResultText *textItem = new EntityResultText(createEntityUID(), nullptr, nullptr, nullptr, nullptr, "Model");

	textItem->setName(name);
	textItem->setEditable(false);

	// Now we store the data in the entity
	textItem->setText(text);

	// Release the data from memory (this will write it to the data base)
	textItem->releaseTextData();

	// Store the entity in the data base as well.
	textItem->StoreToDataBase();

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

	entityGeom->StoreToDataBase();

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
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_ImportTableFile, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_NAME, JsonString(itemName, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->getServiceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
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

void ot::components::ModelComponent::storeNewEntities(const std::string &description, bool askForBranchCreation)
{
	addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, description, askForBranchCreation);
	clearNewEntityList();
}

const std::string& ot::components::ModelComponent::getModelServiceURL()
{
	return m_serviceURL;
}

const std::string& ot::components::ModelComponent::getThisServiceURL()
{
	return m_application->getServiceURL();
}

void ot::components::ModelComponent::loadMaterialInformation()
{
	materialIDToNameMap.clear();
	materialNameToIDMap.clear();

	ot::UIDList materialIDList = getIDsOfFolderItemsOfType("Materials", "EntityMaterial", true);
	
	std::list<ot::EntityInformation> materialInformation;
	getEntityInformation(materialIDList, materialInformation);

	for (auto matItem : materialInformation)
	{
		materialIDToNameMap[matItem.getID()]   = matItem.getName();
		materialNameToIDMap[matItem.getName()] = matItem.getID();
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
