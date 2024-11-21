//! @file Application.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// Model header
#include "stdafx.h"
#include "Model.h"
#include "Application.h"
#include "ProjectTypeManager.h"
#include "MicroserviceNotifier.h"
#include "UiNotifier.h"
#include "ModelNotifier.h"
#include "base64.h"
#include "zlib.h"
#include "FileHandler.h"
#include "OTCore/ReturnMessage.h"
// OpenTwin header
#include "DataBase.h"
#include "OTCore/Logger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/IpConverter.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/Encryption.h"
#include "CrossCollectionDatabaseWrapper.h"
// std header
#include <thread>

Application* Application::instance(void) {
	static Application* g_instance{ nullptr };
	if (g_instance == nullptr) { g_instance = new Application; }
	return g_instance;
}

// ##################################################################################################################################################################################################################

// Action handler

std::string Application::handleDeleteModel(ot::JsonDocument& _document) {
	if (m_model)
	{
		delete m_model;
		m_model = nullptr;
		OT_LOG_D("Model deleted");
	}
	
	return "";
}

std::string Application::handleProjectSave(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	m_model->projectSave("", false);

	return "";
}

std::string Application::handleSelectionChanged(ot::JsonDocument& _document) {
	this->queueAction(ActionType::SelectionChanged, _document);
	return "";
}

std::string Application::handleItemRenamed(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	ot::UID entityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	std::string newName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Name);
	m_model->modelItemRenamed(entityID, newName);

	return "";
}

std::string Application::handleSetVisualizationModel(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	ot::UID viewerModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	ot::UID viewerViewID = ot::json::getUInt64(_document, OT_ACTION_PARAM_VIEW_ID);

	OT_LOG_D("Visualization model set to (" + std::to_string(viewerModelID) + "). Opening project...");
	
	m_model->projectOpen();
	m_model->setVisualizationModel(viewerModelID);

	return "";
}

std::string Application::handleGetVisualizationModel(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	ot::JsonDocument newDoc;
	newDoc.AddMember(OT_ACTION_PARAM_BASETYPE_UID, m_model->getVisualizationModel(), newDoc.GetAllocator());

	return newDoc.toJson();
}

std::string Application::handleGetIsModified(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	ot::JsonDocument newDoc;
	newDoc.AddMember(OT_ACTION_PARAM_BASETYPE_Bool, m_model->getModified(), newDoc.GetAllocator());

	return newDoc.toJson();
}

std::string Application::handleSetPropertiesFromJSON(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::list<ot::UID> entityIDList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_EntityIDList);

	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);
	ot::PropertyGridCfg cfg;
	cfg.setFromJsonObject(cfgObj);

	bool update = ot::json::getBool(_document, OT_ACTION_PARAM_MODEL_Update);
	bool itemsVisible = ot::json::getBool(_document, OT_ACTION_PARAM_MODEL_ItemsVisible);
	
	m_model->setPropertiesFromJson(entityIDList, cfg, update, itemsVisible);

	return "";
}

std::string Application::handleGenerateEntityIDs(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	unsigned long long count = ot::json::getUInt64(_document, OT_ACTION_PARAM_COUNT);
	ot::UIDList list = m_model->getNewEntityIDs(count);

	ot::JsonDocument newDoc;

	ot::JsonArray listUID;
	for (ot::UID id : list)
	{
		listUID.PushBack(id, newDoc.GetAllocator());
	}
	newDoc.AddMember(OT_ACTION_PARAM_BASETYPE_List, listUID, newDoc.GetAllocator());

	return newDoc.toJson();
}

std::string Application::handleImportTableFile(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::string itemName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	m_model->importTableFile(itemName);

	return "";
}

std::string Application::handleQueueMessages(ot::JsonDocument& _document) {
	this->enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, ot::json::getBool(_document, OT_ACTION_PARAM_QUEUE_FLAG));
	return "";
}

std::string Application::handleGetListOfFolderItems(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::string folder = ot::json::getString(_document, OT_ACTION_PARAM_Folder);
	bool recursive = ot::json::getBool(_document, OT_ACTION_PARAM_Recursive);
	
	std::list<std::string> list = m_model->getListOfFolderItems(folder, recursive);

	ot::JsonDocument newDoc;

	ot::JsonArray listString;
	for (auto id : list)
	{
		listString.PushBack(ot::JsonString(id.c_str(), newDoc.GetAllocator()), newDoc.GetAllocator());
	}

	newDoc.AddMember(OT_ACTION_PARAM_BASETYPE_List, listString, newDoc.GetAllocator());

	return newDoc.toJson();
}

std::string Application::handleGetIDsOfFolderItemsByType(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::string folder = ot::json::getString(_document, OT_ACTION_PARAM_Folder);
	std::string className = ot::json::getString(_document, OT_ACTION_PARAM_Type);
	bool recursive = ot::json::getBool(_document, OT_ACTION_PARAM_Recursive);

	ot::UIDList list = m_model->getIDsOfFolderItemsOfType(folder, className, recursive);

	ot::JsonDocument newDoc;

	ot::JsonArray listUID;
	for (ot::UID id : list)
	{
		listUID.PushBack(id, newDoc.GetAllocator());
	}
	newDoc.AddMember(OT_ACTION_PARAM_BASETYPE_List, listUID, newDoc.GetAllocator());

	return newDoc.toJson();
}

std::string Application::handleDeleteCurvesFromPlots(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::list<std::string> curveNames = ot::json::getStringList(_document, OT_ACTION_PARAM_VIEW1D_CurveNames);
	m_model->deleteCurves(curveNames);

	return "";
}

std::string Application::handleUpdateCurvesOfPlot(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::list<std::string> curveNames = ot::json::getStringList(_document, OT_ACTION_PARAM_UI_TREE_SelectedItems);
	ot::UID plotID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);

	m_model->updateCurvesInPlot(curveNames, plotID);

	return "";
}

std::string Application::handleUpdateVisualizationEntity(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	ot::UID visEntityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);
	ot::UID visEntityVersion = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityVersion);
	ot::UID binaryDataItemID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_DataID);
	ot::UID binaryDataItemVersion = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_DataVersion);

	m_model->updateVisualizationEntity(visEntityID, visEntityVersion, binaryDataItemID, binaryDataItemVersion);

	return "";
}

std::string Application::handleUpdateGeometryEntity(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	ot::UID geomEntityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);
	ot::UID brepEntityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID_Brep);
	ot::UID facetsEntityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID_Facets);
	ot::UID brepEntityVersion = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityVersion_Brep);
	ot::UID facetsEntityVersion = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityVersion_Facets);
	bool overrideGeometry = ot::json::getBool(_document, OT_ACTION_PARAM_MODEL_OverrideGeometry);

	ot::PropertyGridCfg cfg;
	bool updateProperties = false;

	if (_document.HasMember(OT_ACTION_PARAM_MODEL_NewProperties)) {
		ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_MODEL_NewProperties);
		cfg.setFromJsonObject(cfgObj);
		updateProperties = true;
	}

	m_model->updateGeometryEntity(geomEntityID, brepEntityID, brepEntityVersion, facetsEntityID, facetsEntityVersion, overrideGeometry, cfg, updateProperties);

	return "";
}

std::string Application::handleModelChangeOperationCompleted(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::string description = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_Description);

	m_model->modelChangeOperationCompleted(description);

	return "";
}

std::string Application::handleRequestUpdateVisualizationEntity(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	ot::UID visEntityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);

	m_model->requestUpdateVisualizationEntity(visEntityID);

	return "";
}

std::string Application::handleCheckParentUpdates(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::list<ot::UID> modifiedEntities = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_EntityIDList);
	
	return m_model->checkParentUpdates(modifiedEntities);
}

std::string Application::handleAddEntities(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::list<ot::UID> topologyEntityIDList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_TopologyEntityIDList);
	std::list<ot::UID> topologyEntityVersionList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_TopologyEntityVersionList);
	std::list<bool> topologyEntityForceVisible = ot::json::getBoolList(_document, OT_ACTION_PARAM_MODEL_TopologyEntityForceShowList);
	std::list<ot::UID> dataEntityIDList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_DataEntityIDList);
	std::list<ot::UID> dataEntityVersionList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_DataEntityVersionList);
	std::list<ot::UID> dataEntityParentList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_DataEntityParentList);
	std::string changeComment = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Description);

	bool askForBranchCreation = true;
	if (ot::json::exists(_document, OT_ACTION_PARAM_MODEL_ITM_AskForBranchCreation))
	{
		askForBranchCreation = ot::json::getBool(_document, OT_ACTION_PARAM_MODEL_ITM_AskForBranchCreation);
	}

	bool saveModel = true;
	if (ot::json::exists(_document, OT_ACTION_PARAM_MODEL_SaveModel))
	{
		saveModel = ot::json::getBool(_document, OT_ACTION_PARAM_MODEL_SaveModel);
	}

	m_model->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, changeComment, saveModel, askForBranchCreation);

	return "";
}

std::string Application::handleUpdateTopologyEntity(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::list<ot::UID> topologyEntityIDList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_TopologyEntityIDList);
	std::list<ot::UID> topologyEntityVersionList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_TopologyEntityVersionList);
	std::string comment = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Description);
	
	m_model->updateTopologyEntities(topologyEntityIDList, topologyEntityVersionList, comment);

	return "";
}

std::string Application::handleAddGeometryOperation(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	ot::UID geometryEntityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);
	ot::UID geometryEntityVersion = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityVersion);
	std::string geomEntityName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_EntityName);
	std::list<ot::UID> dataEntityIDList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_DataEntityIDList);
	std::list<ot::UID> dataEntityVersionList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_DataEntityVersionList);
	std::list<ot::UID> dataEntityParentList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_DataEntityParentList);
	std::list<std::string> childrenList = ot::json::getStringList(_document, OT_ACTION_PARAM_MODEL_EntityChildrenList);
	std::string changeComment = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Description);

	m_model->addGeometryOperation(geometryEntityID, geometryEntityVersion, geomEntityName, dataEntityIDList, dataEntityVersionList, dataEntityParentList, childrenList, changeComment);

	return "";
}

std::string Application::handleDeleteEntity(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::list<std::string> entityNameList = ot::json::getStringList(_document, OT_ACTION_PARAM_MODEL_EntityNameList);
	bool saveModel = ot::json::getBool(_document, OT_ACTION_PARAM_MODEL_SaveModel);

	m_model->deleteEntitiesFromModel(entityNameList, saveModel);

	return "";
}

std::string Application::handleMeshingCompleted(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	ot::UID meshEntityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);

	m_model->setMeshingActive(meshEntityID, false);

	return "";
}

std::string Application::handleGetEntityInformationByID(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::list<ot::UID> entityIDList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_EntityIDList);

	return this->getEntityInformation(entityIDList);
}

std::string Application::handleGetEntityInformationByName(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::list<std::string> entityNameList = ot::json::getStringList(_document, OT_ACTION_PARAM_MODEL_EntityNameList);

	std::list<ot::UID> entityIDList;
	for (auto name : entityNameList)
	{
		EntityBase* entity = m_model->findEntityFromName(name);

		if (entity)
		{
			entityIDList.push_back(entity->getEntityID());
		}
	}

	return this->getEntityInformation(entityIDList);
}

std::string Application::handleGetSelectedEntityInformation(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::string typeFilter("");
	if (ot::json::exists(_document, OT_ACTION_PARAM_SETTINGS_Type))
	{
		typeFilter = ot::json::getString(_document, OT_ACTION_PARAM_SETTINGS_Type);
	}

	std::list<EntityBase*> entityList = m_model->getListOfSelectedEntities(typeFilter);

	std::list<ot::UID> entityIDList;
	for (auto entity : entityList)
	{
		entityIDList.push_back(entity->getEntityID());
	}

	return this->getEntityInformation(entityIDList);
}

std::string Application::handleGetEntityChildInformationByName(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::string entityName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Name);
	bool recursive = ot::json::getBool(_document, OT_ACTION_PARAM_Recursive);

	std::list<ot::UID> entityIDList;
	EntityContainer* entity = dynamic_cast<EntityContainer*>(m_model->findEntityFromName(entityName));

	if (entity != nullptr)
	{
		if (recursive)
		{
			std::list<std::pair<ot::UID, ot::UID>> childrenEntities;
			m_model->getListOfAllChildEntities(entity, childrenEntities);

			for (auto item : childrenEntities)
			{
				entityIDList.push_back(item.first);
			}
		}
		else
		{
			for (auto child : entity->getChildrenList())
			{
				entityIDList.push_back(child->getEntityID());
			}
		}
	}

	return this->getEntityInformation(entityIDList);
}

std::string Application::handleGetEntityChildInformationByID(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	ot::UID entityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ITM_ID);
	bool recursive = ot::json::getBool(_document, OT_ACTION_PARAM_Recursive);

	std::list<ot::UID> entityIDList;

	EntityContainer* entity = dynamic_cast<EntityContainer*>(m_model->getEntityByID(entityID));

	if (entity != nullptr)
	{
		if (recursive)
		{
			std::list<std::pair<ot::UID, ot::UID>> childrenEntities;
			m_model->getListOfAllChildEntities(entity, childrenEntities);

			for (auto item : childrenEntities)
			{
				entityIDList.push_back(item.first);
			}
		}
		else
		{
			for (auto child : entity->getChildrenList())
			{
				entityIDList.push_back(child->getEntityID());
			}
		}
	}

	return this->getEntityInformation(entityIDList);
}

std::string Application::handleGetAllGeometryEntitiesForMeshing(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	ot::JsonDocument newDoc;
	newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(m_model->getAllGeometryEntitiesForMeshing(), newDoc.GetAllocator()), newDoc.GetAllocator());

	return newDoc.toJson();
}

std::string Application::handleGetEntityProperties(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	ot::UID entityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);
	bool recursive = ot::json::getBool(_document, OT_ACTION_PARAM_Recursive);
	std::string propertyGroupFilter = ot::json::getString(_document, OT_ACTION_PARAM_Filter);

	std::map<ot::UID, ot::PropertyGridCfg> entityProperties;

	m_model->getEntityProperties(entityID, recursive, propertyGroupFilter, entityProperties);

	// Now we convert the results into JSON data
	ot::JsonDocument newDoc;
	std::list<ot::UID> entityIDList;
	ot::JsonArray propertyList;
	for (auto item : entityProperties)
	{
		entityIDList.push_back(item.first);
		ot::JsonObject cfgObj;
		item.second.addToJsonObject(cfgObj, newDoc.GetAllocator());
		propertyList.PushBack(cfgObj, newDoc.GetAllocator());
	}

	newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityIDList, newDoc.GetAllocator()), newDoc.GetAllocator());
	newDoc.AddMember(OT_ACTION_PARAM_MODEL_PropertyList, propertyList, newDoc.GetAllocator());

	return newDoc.toJson();
}

std::string Application::handleGetEntityPropertiesByName(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::string entityName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_EntityName);
	bool recursive = ot::json::getBool(_document, OT_ACTION_PARAM_Recursive);
	std::string propertyGroupFilter = ot::json::getString(_document, OT_ACTION_PARAM_Filter);

	EntityBase* entity = m_model->findEntityFromName(entityName);

	std::map<ot::UID, ot::PropertyGridCfg> entityProperties;

	if (entity)
	{
		m_model->getEntityProperties(entity->getEntityID(), recursive, propertyGroupFilter, entityProperties);
	}

	// Now we convert the results into JSON data
	ot::JsonDocument newDoc;
	std::list<ot::UID> entityIDList;
	ot::JsonArray propertyList;

	for (auto item : entityProperties)
	{
		entityIDList.push_back(item.first);
		ot::JsonObject cfgObj;
		item.second.addToJsonObject(cfgObj, newDoc.GetAllocator());
		propertyList.PushBack(cfgObj, newDoc.GetAllocator());
	}

	newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityIDList, newDoc.GetAllocator()), newDoc.GetAllocator());
	newDoc.AddMember(OT_ACTION_PARAM_MODEL_PropertyList, propertyList, newDoc.GetAllocator());

	return newDoc.toJson();
}

std::string Application::handleUpdatePropertyGrid(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	m_model->updatePropertyGrid();

	return "";
}

std::string Application::handleGetCurrentVisualizationModelID(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	ot::JsonDocument newDoc;
	newDoc.AddMember(OT_ACTION_PARAM_BASETYPE_UID, m_model->getVisualizationModel(), newDoc.GetAllocator());

	return newDoc.toJson();
}

std::string Application::handleAddPropertiesToEntities(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::list<ot::UID> entityIDList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_EntityIDList);

	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);
	ot::PropertyGridCfg cfg;
	cfg.setFromJsonObject(cfgObj);

	m_model->addPropertiesToEntities(entityIDList, cfg);

	return "";
}

std::string Application::handleUpdatePropertiesOfEntities(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::list<ot::UID> entityIDList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_EntityIDList);

	std::string json = ot::json::getString(_document, OT_ACTION_PARAM_JSON);

	m_model->updatePropertiesOfEntities(entityIDList, json);

	return "";
}

std::string Application::handleDeleteProperty(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::list<ot::UID> entityIDList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_EntityIDList);
	std::string propertyName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_EntityName);
	std::string groupName = "";
	if (ot::json::exists(_document, OT_PARAM_GROUP))
	{
		groupName = ot::json::getString(_document, OT_PARAM_GROUP);
	}
	m_model->deleteProperty(entityIDList, propertyName,groupName);

	return "";
}

std::string Application::handleExecuteAction(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	bool alreadyHandled = m_baseHandler.tryToHandleAction(action, _document);
	if (!alreadyHandled)
	{
		m_model->executeAction(action, _document);
	}

	return "";
}

std::string Application::handleExecuteFunction(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::string function = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_FunctionName);
	bool alreadyHandled = 	m_baseHandler.tryToHandleAction(function, _document);
	if (!alreadyHandled)
	{
		std::string mode = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Mode);

		if (mode == OT_ACTION_VALUE_FILE_Mode_Name)
		{
			std::string fileName = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Name);
			m_model->executeFunction(function, fileName, false);
		}
		else if (mode == OT_ACTION_VALUE_FILE_Mode_Content)
		{
			std::string content = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Content);
			unsigned long long uncompressedDataLength = ot::json::getUInt64(_document, OT_ACTION_PARAM_FILE_Content_UncompressedDataLength);

			// Create a tmp file from uncompressing the data
			// Decode the encoded string into binary data
			int decoded_compressed_data_length = Base64decode_len(content.c_str());
			char* decodedCompressedString = new char[decoded_compressed_data_length];

			Base64decode(decodedCompressedString, content.c_str());

			// Decompress the data
			char* decodedString = new char[uncompressedDataLength];
			uLongf destLen = (uLongf)uncompressedDataLength;
			uLong  sourceLen = decoded_compressed_data_length;
			uncompress((Bytef*)decodedString, &destLen, (Bytef*)decodedCompressedString, sourceLen);

			delete[] decodedCompressedString;
			decodedCompressedString = nullptr;

			// Store the data in a temporary file
			std::string tmpFileName = DataBase::GetDataBase()->getTmpFileName();

			std::ofstream file(tmpFileName, std::ios::binary);
			file.write(decodedString, uncompressedDataLength);
			file.close();

			delete[] decodedString;
			decodedString = nullptr;

			// Process the file content
			m_model->executeFunction(function, tmpFileName, true);
		}
		else
		{
			OT_LOG_E("Unknown mode");
		}
	}

	return "";
}

std::string Application::handleEntitiesSelected(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::string selectionAction = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_SelectionAction);
	std::string selectionInfo = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_SelectionInfo);
	std::list<std::string> optionNames = ot::json::getStringList(_document, OT_ACTION_PARAM_MODEL_ITM_Selection_OptNames);
	std::list<std::string> optionValues = ot::json::getStringList(_document, OT_ACTION_PARAM_MODEL_ITM_Selection_OptValues);

	// Build a map from the option name and values lists
	std::map<std::string, std::string> options;
	auto optValue = optionValues.begin();
	for (auto optName : optionNames)
	{
		options[optName] = *optValue;
		optValue++;
	}

	m_model->entitiesSelected(selectionAction, selectionInfo, options);

	return "";
}

std::string Application::handlePromptResponse(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::string response = ot::json::getString(_document, OT_ACTION_PARAM_RESPONSE);
	std::string answer = ot::json::getString(_document, OT_ACTION_PARAM_ANSWER);
	std::string parameter1 = ot::json::getString(_document, OT_ACTION_PARAM_PARAMETER1);

	m_model->promptResponse(response, answer, parameter1);

	return "";
}

std::string Application::handleGetEntityIdentifier(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}
	if (!this->isUiConnected()) {
		OT_LOG_E("Frontend is not connected");
		return OT_ACTION_RETURN_INDICATOR_Error "Frontend is not connected";
	}
	
	const int numberOfIdentifier = ot::json::getInt(_document, OT_ACTION_PARAM_MODEL_ENTITY_IDENTIFIER_AMOUNT);
	ot::UIDList requestedUIDs, requestedVersions;
	for (int i = 0; i < numberOfIdentifier; i++)
	{
		requestedUIDs.push_back(m_model->createEntityUID());
		requestedVersions.push_back(m_model->createEntityUID());
	}
	const std::string& requestingService = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);
	const std::string& subsequentFunction = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_FunctionName);

	ot::JsonDocument replyDoc;
	replyDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteFunction, replyDoc.GetAllocator()), replyDoc.GetAllocator());
	replyDoc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(subsequentFunction, replyDoc.GetAllocator()), replyDoc.GetAllocator());
	replyDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(requestedUIDs, replyDoc.GetAllocator()), replyDoc.GetAllocator());
	replyDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersionList, ot::JsonArray(requestedVersions, replyDoc.GetAllocator()), replyDoc.GetAllocator());

	std::string response;
	//Currently only the ui service uses this function but the ui url is currently not available when this method is being invoked.
	this->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, replyDoc, response);

	return response;
}

std::string Application::handleGetEntitiesFromAnotherCollection(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::string	collectionName = ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME);
	std::string folder = ot::json::getString(_document, OT_ACTION_PARAM_Folder);
	std::string className = ot::json::getString(_document, OT_ACTION_PARAM_Type);
	bool recursive = ot::json::getBool(_document, OT_ACTION_PARAM_Recursive);

	std::string actualOpenedProject = DataBase::GetDataBase()->getProjectName();
	CrossCollectionDatabaseWrapper wrapper(collectionName);
		
	ModelState secondary(m_model->getSessionCount(), m_model->getServiceIDAsInt());
	secondary.openProject();

	std::list<ot::UID> prefetchIds;
	secondary.getListOfTopologyEntites(prefetchIds);
	std::list<std::pair<ot::UID, ot::UID>> prefetchIdandVersion;
	for (auto id : prefetchIds)
	{
		ot::UID entityID = id;
		ot::UID entityVersion = secondary.getCurrentEntityVersion(entityID);
		prefetchIdandVersion.push_back(std::pair<ot::UID, ot::UID>(entityID, entityVersion));
	}
	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIdandVersion);
	ot::UIDList entityIDList, entityVersionList;
	for (auto& identifier : prefetchIdandVersion)
	{
		auto doc = bsoncxx::builder::basic::document{};
		if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(identifier.first, identifier.second, doc))
		{
			return "";
		}
		auto doc_view = doc.view()["Found"].get_document().view();
		std::string entityType = doc_view["SchemaType"].get_utf8().value.data();
		if (entityType == className)
		{
			entityIDList.push_back(identifier.first);
			entityVersionList.push_back(identifier.second);
		}
	}

	ot::JsonDocument newDoc;
	newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityIDList, newDoc.GetAllocator()), newDoc.GetAllocator());
	newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersionList, ot::JsonArray(entityVersionList, newDoc.GetAllocator()), newDoc.GetAllocator());

	return newDoc.toJson();
}

std::string Application::handleViewsFromProjectType(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	ProjectTypeManager typeManager(m_model->getProjectType());
	return typeManager.getViews();
}



// Versions

std::string Application::handleGetCurrentVersion(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	return m_model->getCurrentModelVersion();
}

std::string Application::handleActivateVersion(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::string version = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_Version);

	m_model->activateVersion(version);

	return "";
}

std::string Application::handleVersionSelected(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::string version = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_Version);

	m_model->versionSelected(version);

	return "";
}

std::string Application::handleVersionDeselected(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	m_model->versionDeselected();

	return "";
}

std::string Application::handleSetVersionLabel(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return OT_ACTION_RETURN_INDICATOR_Error "No model created yet";
	}

	std::string version = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_Version);
	std::string label = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_VersionLabel);

	m_model->setVersionLabel(version, label);

	return "";
}

std::string Application::handleVisualisationDataRequest(ot::JsonDocument& _document)
{
	ot::UID entityID =  ot::json::getUInt64(_document,OT_ACTION_PARAM_MODEL_EntityID);
	const std::string visualisationType = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_FunctionName);
	m_visualisationHandler.handleVisualisationRequest(entityID,visualisationType);
	return "";
}
// ##################################################################################################################################################################################################################

// Setter / Getter

std::string Application::getEntityInformation(std::list<ot::UID>& _entityIDList) const {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return "";
	}

	std::list<ot::UID> entityVersions;
	std::list<std::string> entityNames;
	std::list<std::string> entityTypes;

	m_model->getEntityVersions(_entityIDList, entityVersions);
	m_model->getEntityNames(_entityIDList, entityNames);
	m_model->getEntityTypes(_entityIDList, entityTypes);

	ot::JsonDocument newDoc;
	newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(_entityIDList, newDoc.GetAllocator()), newDoc.GetAllocator());
	newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersionList, ot::JsonArray(entityVersions, newDoc.GetAllocator()), newDoc.GetAllocator());
	newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityNameList, ot::JsonArray(entityNames, newDoc.GetAllocator()), newDoc.GetAllocator());
	newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityTypeList, ot::JsonArray(entityTypes, newDoc.GetAllocator()), newDoc.GetAllocator());

	return newDoc.toJson();
}

// ##################################################################################################################################################################################################################

// Communication

bool Application::queuedRequestToFrontend(const ot::JsonDocument& _request) {
	if (!this->isUiConnected()) return false;
	std::string tmp;
	return this->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, _request, tmp);
}

bool Application::queuedRequestToFrontend(const ot::JsonDocument& _request, const std::list<std::pair<ot::UID, ot::UID>>& _prefetchIDs) {
	if (!this->isUiConnected()) return false;
	std::string tmp;
	return this->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, _request, tmp);
}

void Application::flushRequestsToFrontEnd()
{
	this->flushQueuedHttpRequests(OT_INFO_SERVICE_TYPE_UI);
}

// ##################################################################################################################################################################################################################

// Required functions

void Application::run(void) {
	if (m_model) {
		OT_LOG_E("Model already created!");
		return;
	}

	size_t index = this->sessionID().find(':');
	if (index == std::string::npos) {
		OT_LOG_E("Invalid session id format");
		return;
	}

	std::string projectName(this->sessionID().substr(0, index));
	std::string collectionName(this->sessionID().substr(index + 1));

	this->EnsureDataBaseConnection();

	m_model = new Model(projectName, this->projectType(), collectionName);
}

std::string Application::processAction(const std::string& _action, ot::JsonDocument& _doc) {
	
	return "";
}

std::string Application::processMessage(ServiceBase* _sender, const std::string& _message, ot::JsonDocument& _doc) {

	return "";
}

void Application::uiConnected(ot::components::UiComponent* _ui) {
	ot::JsonDocument registerDoc;
	registerDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_RegisterForModelEvents, registerDoc.GetAllocator()), registerDoc.GetAllocator());
	registerDoc.AddMember(OT_ACTION_PARAM_PORT, ot::JsonString(ot::IpConverter::portFromIp(this->getServiceURL()), registerDoc.GetAllocator()), registerDoc.GetAllocator());
	registerDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, this->getServiceID(), registerDoc.GetAllocator());
	registerDoc.AddMember(OT_ACTION_PARAM_RegisterForModelEvents, true, registerDoc.GetAllocator());

	std::string response;
	if (!this->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, registerDoc, response)) {
		OT_LOG_E("Failed to send request");
		return;
	}

	ot::JsonDocument commandDoc;
	commandDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_MODEL_Create, commandDoc.GetAllocator()), commandDoc.GetAllocator());
	commandDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, this->getServiceID(), commandDoc.GetAllocator());

	response.clear();
	if (!this->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, commandDoc, response)) {
		OT_LOG_E("Failed to send request");
		return;
	}

	if (!m_model) {
		OT_LOG_E("No model created yet");
		return;
	}
	
	m_model->uiIsAvailable();
}

void Application::uiDisconnected(const ot::components::UiComponent* _ui) {

}

void Application::uiPluginConnected(ot::components::UiPluginComponent* _uiPlugin) {

}

void Application::modelConnected(ot::components::ModelComponent* _model) {

}

void Application::modelDisconnected(const ot::components::ModelComponent* _model) {

}

void Application::serviceConnected(ot::ServiceBase* _service) {

}

void Application::serviceDisconnected(const ot::ServiceBase* _service) {

}

void Application::preShutdown(void) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return;
	}

	m_model->projectSave("", false);

	// Here we disconnect all viewers, but leave them alone
	m_model->detachAllViewer();

	// Disable the UI clear since the UI will clear all controls anyway
	m_model->setClearUiOnDelete(false);

	delete m_model;
	m_model = nullptr;
}

void Application::shuttingDown(void) {
	m_asyncActionMutex.lock();
	m_continueAsyncActionWorker = false;
	m_asyncActionMutex.unlock();
}

bool Application::startAsRelayService(void) const {
	return false;
}

ot::PropertyGridCfg Application::createSettings(void) const {
	return ot::PropertyGridCfg();
}

void Application::settingsSynchronized(const ot::PropertyGridCfg& _dataset) {

}

bool Application::settingChanged(const ot::Property* _item) {
	return false;
}

// ##################################################################################################################################################################################################################

// Private functions

void Application::addButtons()
{
	m_fileHandler.addButtons(uiComponent(), "Model");
}

void Application::queueAction(ActionType _type, const ot::JsonDocument& _document) {
	ActionData newData;
	newData.type = _type;
	newData.document = _document.toJson();

	m_asyncActionMutex.lock();
	m_queuedActions.push_back(newData);
	m_asyncActionMutex.unlock();
}

void Application::asyncActionWorker(void) {
	while (this->getContinueAsyncActionWorker()) {
		// Check if a action is present
		m_asyncActionMutex.lock();
		if (m_queuedActions.empty()) {
			m_asyncActionMutex.unlock();

			// No action, sleep for 10ms
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(10ms);
			continue;
		}

		// Get next action
		ActionData nextAction = m_queuedActions.front();
		m_queuedActions.pop_front();
		m_asyncActionMutex.unlock();

		ot::JsonDocument actionDocument;
		actionDocument.fromJson(nextAction.document);

		// Dispatch
		switch (nextAction.type)
		{
		case ActionType::SelectionChanged:
			this->handleAsyncSelectionChanged(actionDocument);
			break;
		default:
			OT_LOG_E("Unknown action type (" + std::to_string((int)nextAction.type) + ")");
			break;
		}
	}
}

void Application::handleAsyncSelectionChanged(const ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_EA("No model created yet");
	}

	std::list<ot::UID> selectedEntityIDs = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_SelectedEntityIDs);
	std::list<ot::UID> selectedVisibleEntityIDs = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_SelectedVisibleEntityIDs);

	if (m_model->getVisualizationModel() == 0)
	{
		return;
	}

	// Since we are performing notifications in a parallel thread, we need to make sure that all notifications are done before
	// we send the next round of notifications
	using namespace std::chrono_literals;
	while (m_selectionHandler.notificationInProcess())
	{
		std::this_thread::sleep_for(1ms);
	}

	// It might happen that the UI still has some reference to entitires which have already been deleted.
	// Therefore we filter the items by checing whether they still exist.

	std::list<ot::UID> selectedEntityIDsVerified, selectedVisibleEntityIDsVerified;
	for (auto entityID : selectedEntityIDs)
	{
		if (m_model->getEntityByID(entityID) != nullptr)
		{
			selectedEntityIDsVerified.push_back(entityID);
		}
		else
		{
			const std::string message = "Notification about selected entity that does not exist in this model state; id: " + std::to_string(entityID);
			OT_LOG_W(message);
		}
	}

	for (auto entityID : selectedVisibleEntityIDs)
	{
		if (m_model->getEntityByID(entityID) != nullptr)
		{
			selectedVisibleEntityIDsVerified.push_back(entityID);
		}
		else
		{
			const std::string message = "Notification about selected visible entity that does not exist in this model state; id: " + std::to_string(entityID);
			OT_LOG_W(message);
		}
	}

	m_selectionHandler.processSelectionChanged(selectedEntityIDsVerified, selectedVisibleEntityIDsVerified);
}

bool Application::getContinueAsyncActionWorker(void) {
	bool ret = false;
	m_asyncActionMutex.lock();
	ret = m_continueAsyncActionWorker;
	m_asyncActionMutex.unlock();
	return ret;
}

Application::Application() 
	: ot::ApplicationBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL, new UiNotifier, new ModelNotifier),
	m_model(nullptr), m_continueAsyncActionWorker(true)
{
	m_notifier = new MicroserviceNotifier;

	std::thread asyncActionThread(&Application::asyncActionWorker, this);
	asyncActionThread.detach();
	m_baseHandler.setNextHandler(&m_fileHandler);
}

Application::~Application() {

}
