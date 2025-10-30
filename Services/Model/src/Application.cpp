// @otlicense

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
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/IpConverter.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/Encryption.h"
#include "CrossCollectionDatabaseWrapper.h"
#include "OTGui/VisualisationCfg.h"
// std header
#include <thread>

Application* Application::instance() {
	static Application* g_instance{ nullptr };
	if (g_instance == nullptr) { g_instance = new Application; }
	return g_instance;
}

// ##################################################################################################################################################################################################################

// Action handler

void Application::handleProjectSave() {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	m_model->projectSave("", false);
}

ot::ReturnMessage Application::handleCheckProjectOpen() {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return ot::ReturnMessage::False;
	}
	else {
		return (m_model->isProjectOpen() ? ot::ReturnMessage::True : ot::ReturnMessage::False);
	}
}

void Application::handleSelectionChanged(ot::JsonDocument& _document) {
	this->queueAction(ActionType::SelectionChanged, _document);
}

void Application::handleItemRenamed(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	ot::UID entityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	std::string newName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Name);
	m_model->modelItemRenamed(entityID, newName);
}

void Application::handleSetVisualizationModel(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	ot::UID viewerModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	ot::UID viewerViewID = ot::json::getUInt64(_document, OT_ACTION_PARAM_VIEW_ID);

	std::string customVersion;
	if (_document.HasMember(OT_ACTION_PARAM_PROJECT_VERSION)) {
		customVersion = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_VERSION);
	}

	OT_LOG_D("Visualization model set to (" + std::to_string(viewerModelID) + "). Opening project...");
	
	m_model->projectOpen(customVersion);
	m_model->setVisualizationModel(viewerModelID);
}

std::string Application::handleGetVisualizationModel(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "No model created yet");
	}

	ot::JsonDocument newDoc;
	newDoc.AddMember(OT_ACTION_PARAM_BASETYPE_UID, m_model->getVisualizationModel(), newDoc.GetAllocator());

	return newDoc.toJson();
}

std::string Application::handleGetIsModified(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "No model created yet");
	}

	ot::JsonDocument newDoc;
	newDoc.AddMember(OT_ACTION_PARAM_BASETYPE_Bool, m_model->getModified(), newDoc.GetAllocator());

	return newDoc.toJson();
}

std::string Application::handleGenerateEntityIDs(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
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

void Application::handleRequestImportTableFile(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	std::string itemName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	m_model->requestImportTableFile(itemName);
}

void Application::handleQueueMessages(ot::JsonDocument& _document) {
	this->enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, ot::json::getBool(_document, OT_ACTION_PARAM_QUEUE_FLAG));
}

std::string Application::handleGetListOfFolderItems(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
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
		throw ot::Exception::ObjectNotFound("No model created yet");
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

void Application::handleUpdateVisualizationEntity(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	ot::UID visEntityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);
	ot::UID visEntityVersion = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityVersion);
	ot::UID binaryDataItemID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_DataID);
	ot::UID binaryDataItemVersion = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_DataVersion);

	m_model->updateVisualizationEntity(visEntityID, visEntityVersion, binaryDataItemID, binaryDataItemVersion);
}

void Application::handleUpdateGeometryEntity(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
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
}

void Application::handleModelChangeOperationCompleted(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	std::string description = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_Description);

	m_model->modelChangeOperationCompleted(description);
}

void Application::handleRequestUpdateVisualizationEntity(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	ot::UID visEntityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);

	m_model->requestUpdateVisualizationEntity(visEntityID);
}

std::string Application::handleCheckParentUpdates(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	std::list<ot::UID> modifiedEntities = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_EntityIDList);
	
	return m_model->checkParentUpdates(modifiedEntities);
}

void Application::handleAddEntities(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
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

	bool considerVisualization = true;
	if (ot::json::exists(_document, OT_ACTION_PARAM_MODEL_ConsiderVisualization)) {
		considerVisualization = ot::json::getBool(_document, OT_ACTION_PARAM_MODEL_ConsiderVisualization);
	}

	m_model->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, changeComment, saveModel, askForBranchCreation, considerVisualization);
}

void Application::handleUpdateTopologyEntity(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	std::list<ot::UID> topologyEntityIDList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_TopologyEntityIDList);
	std::list<ot::UID> topologyEntityVersionList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_TopologyEntityVersionList);
	std::string comment = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Description);
	
	bool considerVisualization = true;
	if (ot::json::exists(_document, OT_ACTION_PARAM_MODEL_ConsiderVisualization)) {
		considerVisualization = ot::json::getBool(_document, OT_ACTION_PARAM_MODEL_ConsiderVisualization);
	}

	m_model->updateTopologyEntities(topologyEntityIDList, topologyEntityVersionList, comment, considerVisualization);
}

void Application::handleAddGeometryOperation(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
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
}

void Application::handleDeleteEntity(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}
	
	bool saveModel = ot::json::getBool(_document, OT_ACTION_PARAM_MODEL_SaveModel);

	if (_document.HasMember(OT_ACTION_PARAM_MODEL_EntityNameList)) {
		std::list<std::string> entityNameList = ot::json::getStringList(_document, OT_ACTION_PARAM_MODEL_EntityNameList);
		m_model->deleteEntitiesFromModel(entityNameList, saveModel);
	}
	else {
		ot::UIDList entityIDList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_EntityIDList);
		m_model->deleteEntitiesFromModel(entityIDList, saveModel);
	}
}

void Application::handleMeshingCompleted(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	ot::UID meshEntityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);

	m_model->setMeshingActive(meshEntityID, false);
}

std::string Application::handleGetEntityInformationByID(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	std::list<ot::UID> entityIDList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_EntityIDList);

	return this->getEntityInformation(entityIDList);
}

std::string Application::handleGetEntityInformationByName(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
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
		throw ot::Exception::ObjectNotFound("No model created yet");
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
		throw ot::Exception::ObjectNotFound("No model created yet");
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
		throw ot::Exception::ObjectNotFound("No model created yet");
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
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	ot::JsonDocument newDoc;
	newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(m_model->getAllGeometryEntitiesForMeshing(), newDoc.GetAllocator()), newDoc.GetAllocator());

	return newDoc.toJson();
}

std::string Application::handleGetCurrentVisualizationModelID() {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	ot::JsonDocument newDoc;
	newDoc.AddMember(OT_ACTION_PARAM_BASETYPE_UID, m_model->getVisualizationModel(), newDoc.GetAllocator());

	return newDoc.toJson();
}

void Application::handleEntitiesSelected(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
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
}

std::string Application::handleGetEntityIdentifier(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}
	if (!this->isUiConnected()) {
		OT_LOG_E("Frontend is not connected");
		throw ot::Exception::ObjectNotFound("Frontend is not connected");
	}
	
	const int numberOfIdentifier = ot::json::getInt(_document, OT_ACTION_PARAM_MODEL_ENTITY_IDENTIFIER_AMOUNT);
	ot::UIDList requestedUIDs, requestedVersions;
	for (int i = 0; i < numberOfIdentifier; i++)
	{
		requestedUIDs.push_back(m_model->createEntityUID());
		requestedVersions.push_back(m_model->createEntityUID());
	}
	const std::string& requestingService = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);
	const std::string& subsequentFunction = ot::json::getString(_document, OT_ACTION_PARAM_CallbackAction);

	ot::JsonDocument replyDoc;
	replyDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(subsequentFunction, replyDoc.GetAllocator()), replyDoc.GetAllocator());
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
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	std::string	collectionName = ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME);
	std::string folder = ot::json::getString(_document, OT_ACTION_PARAM_Folder);
	std::string className = ot::json::getString(_document, OT_ACTION_PARAM_Type);
	bool recursive = ot::json::getBool(_document, OT_ACTION_PARAM_Recursive);

	std::string actualOpenedProject = DataBase::instance().getCollectionName();
	CrossCollectionDatabaseWrapper wrapper(collectionName);
	
	ModelState secondary(m_model->getSessionCount(), static_cast<unsigned int>(m_model->getServiceID()));
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
	DataBase::instance().prefetchDocumentsFromStorage(prefetchIdandVersion);
	ot::UIDList entityIDList, entityVersionList;
	for (auto& identifier : prefetchIdandVersion)
	{
		auto doc = bsoncxx::builder::basic::document{};
		if (!DataBase::instance().getDocumentFromEntityIDandVersion(identifier.first, identifier.second, doc))
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

std::string Application::handleViewsFromProjectType() {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	ProjectTypeManager typeManager(m_model->getProjectType());
	return typeManager.getViews();
}

ot::ReturnMessage Application::handleVisualisationDataRequest(ot::JsonDocument& _document) {
	ot::UID entityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);

	ot::VisualisationCfg visualisationCfg;
	visualisationCfg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Visualisation_Config));

	try {
		m_visualisationHandler.handleVisualisationRequest(entityID, visualisationCfg);
	}
	catch (std::exception& e) {
		OT_LOG_W(e.what());
		return ot::ReturnMessage(ot::ReturnMessage::Failed, e.what());
	}

	return ot::ReturnMessage::Ok;
}

void Application::handleImportTableFile(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	std::string mode = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Mode);

	if (mode == OT_ACTION_VALUE_FILE_Mode_Name) {
		std::string fileName = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Name);
		m_model->importTableFile(fileName, false);
	}
	else if (mode == OT_ACTION_VALUE_FILE_Mode_Content) {
		std::string content = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Content);
		unsigned long long uncompressedDataLength = ot::json::getUInt64(_document, OT_ACTION_PARAM_FILE_Content_UncompressedDataLength);

		const std::string tmpFileName = this->storeTemporaryFile(content, uncompressedDataLength);

		// Process the file content
		m_model->importTableFile(tmpFileName, true);
	}
}

// ##################################################################################################################################################################################################################

// Action handler: Properties

void Application::handleSetPropertiesFromJSON(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	std::list<ot::UID> entityIDList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_EntityIDList);

	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);
	ot::PropertyGridCfg cfg;
	cfg.setFromJsonObject(cfgObj);

	bool update = ot::json::getBool(_document, OT_ACTION_PARAM_MODEL_Update);
	bool itemsVisible = ot::json::getBool(_document, OT_ACTION_PARAM_MODEL_ItemsVisible);

	m_model->setPropertiesFromJson(entityIDList, cfg, update, itemsVisible);
}

std::string Application::handleGetEntityProperties(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
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
	for (const auto& item : entityProperties) {
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
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	std::string entityName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_EntityName);
	bool recursive = ot::json::getBool(_document, OT_ACTION_PARAM_Recursive);
	std::string propertyGroupFilter = ot::json::getString(_document, OT_ACTION_PARAM_Filter);

	EntityBase* entity = m_model->findEntityFromName(entityName);

	std::map<ot::UID, ot::PropertyGridCfg> entityProperties;

	if (entity) {
		m_model->getEntityProperties(entity->getEntityID(), recursive, propertyGroupFilter, entityProperties);
	}

	// Now we convert the results into JSON data
	ot::JsonDocument newDoc;
	std::list<ot::UID> entityIDList;
	ot::JsonArray propertyList;

	for (const auto& item : entityProperties) {
		entityIDList.push_back(item.first);
		ot::JsonObject cfgObj;
		item.second.addToJsonObject(cfgObj, newDoc.GetAllocator());
		propertyList.PushBack(cfgObj, newDoc.GetAllocator());
	}

	newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityIDList, newDoc.GetAllocator()), newDoc.GetAllocator());
	newDoc.AddMember(OT_ACTION_PARAM_MODEL_PropertyList, propertyList, newDoc.GetAllocator());

	return newDoc.toJson();
}

void Application::handleUpdatePropertyGrid(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	m_model->updatePropertyGrid();
}

void Application::handleAddPropertiesToEntities(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	std::list<ot::UID> entityIDList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_EntityIDList);

	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);
	ot::PropertyGridCfg cfg;
	cfg.setFromJsonObject(cfgObj);

	m_model->addPropertiesToEntities(entityIDList, cfg);
}

void Application::handleUpdatePropertiesOfEntities(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	std::list<ot::UID> entityIDList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_EntityIDList);

	std::string json = ot::json::getString(_document, OT_ACTION_PARAM_JSON);

	m_model->updatePropertiesOfEntities(entityIDList, json);
}

void Application::handleDeleteProperty(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	std::list<ot::UID> entityIDList = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_EntityIDList);
	std::string propertyName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_EntityName);
	std::string groupName = "";
	if (ot::json::exists(_document, OT_PARAM_GROUP)) {
		groupName = ot::json::getString(_document, OT_PARAM_GROUP);
	}
	m_model->deleteProperty(entityIDList, propertyName, groupName);
}

// ##################################################################################################################################################################################################################

// Action handler: Versions

std::string Application::handleGetCurrentVersion() {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	return m_model->getCurrentModelVersion();
}

void Application::handleActivateVersion(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	std::string version = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_Version);

	m_model->activateVersion(version);
}

void Application::handleVersionSelected(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	std::string version = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_Version);

	m_model->versionSelected(version);
}

void Application::handleVersionDeselected(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	m_model->versionDeselected();
}

void Application::handleSetVersionLabel(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	std::string version = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_Version);
	std::string label = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_VersionLabel);

	m_model->setVersionLabel(version, label);
}

// ##################################################################################################################################################################################################################

// Action handler: UI callbacks

void Application::handlePromptResponse(ot::JsonDocument& _document) {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		throw ot::Exception::ObjectNotFound("No model created yet");
	}

	std::string response = ot::json::getString(_document, OT_ACTION_PARAM_RESPONSE);
	std::string answer = ot::json::getString(_document, OT_ACTION_PARAM_ANSWER);
	std::string parameter1 = ot::json::getString(_document, OT_ACTION_PARAM_PARAMETER1);

	ot::MessageDialogCfg::BasicButton actualAnswer = ot::MessageDialogCfg::stringToButton(answer);

	m_model->promptResponse(response, actualAnswer, parameter1);
}

ot::ReturnMessage Application::handleShowTable(ot::JsonDocument& _document) {
	const std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);
	ot::VisualisationCfg visualisationCfg;
	visualisationCfg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Visualisation_Config));
	visualisationCfg.setVisualisationType(OT_ACTION_CMD_UI_TABLE_Setup);

	std::map<std::string, ot::UID> entityMap = m_model->getEntityNameToIDMap();
	auto entityIDByName = entityMap.find(tableName);
	if (entityIDByName != m_model->getEntityNameToIDMap().end()) {
		ot::UID tableID = entityIDByName->second;
		m_visualisationHandler.handleVisualisationRequest(tableID, visualisationCfg);
		return ot::ReturnMessage::Ok;
	}
	else {
		OT_LOG_E("Request to show table " + tableName + " could not be handled, since it is not part of the current state");
		return ot::ReturnMessage::Failed;
	}
}

void Application::handleModelDialogConfirmed(ot::JsonDocument& _document) {
	std::string modelInfo = ot::json::getString(_document, OT_ACTION_PARAM_ModelInfo);
	std::string folder = ot::json::getString(_document, OT_ACTION_PARAM_Folder);
	std::string elementType = ot::json::getString(_document, OT_ACTION_PARAM_ElementType);
	std::string selectedModel = ot::json::getString(_document, OT_ACTION_PARAM_Value);
	ot::UID entityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);

	// First add the model entity to the Project
	m_libraryManagementWrapper.createModelTextEntity(modelInfo, folder, elementType, selectedModel);

	// Now update the property according to the dialog (confirm or cancel)
	m_libraryManagementWrapper.updatePropertyOfEntity(entityID, true, folder + "/" + selectedModel);
}

void Application::handleModelDialogCanceled(ot::JsonDocument& _document) {
	ot::UID entityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);

	// Now update the property according to the dialog (confirm or cancel)
	m_libraryManagementWrapper.updatePropertyOfEntity(entityID, false, "");
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

bool Application::queuedRequestToFrontend(const ot::JsonDocument& _request, std::list<std::pair<ot::UID, ot::UID>>& _prefetchIDs) {
	if (!this->isUiConnected()) return false;
	std::string tmp;
	return this->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, _request, _prefetchIDs, tmp);
}

void Application::flushRequestsToFrontEnd()
{
	this->flushQueuedHttpRequests(OT_INFO_SERVICE_TYPE_UI);
}

// ##################################################################################################################################################################################################################

// Required functions

void Application::initialize() {
	if (m_model) {
		OT_LOG_E("Model already created!");
		return;
	}

	m_model = new Model(this->getProjectName(), this->getProjectType(), this->getCollectionName());
}

void Application::uiConnected(ot::components::UiComponent* _ui) {
	ot::JsonDocument registerDoc;
	registerDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_RegisterForModelEvents, registerDoc.GetAllocator()), registerDoc.GetAllocator());
	registerDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, this->getServiceID(), registerDoc.GetAllocator());
	
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
	
	m_model->uiIsAvailable(_ui);
}

void Application::preShutdown() {
	if (!m_model) {
		OT_LOG_E("No model created yet");
		return;
	}

	//m_model->projectSave("", false);

	// Here we disconnect all viewers, but leave them alone
	m_model->detachAllViewer();

	delete m_model;
	m_model = nullptr;
}

void Application::shuttingDown() {
	m_asyncActionMutex.lock();
	m_continueAsyncActionWorker = false;
	m_asyncActionMutex.unlock();
}

// ##################################################################################################################################################################################################################

// Private functions

void Application::addButtons()
{
	if (this->getProjectType() != OT_ACTION_PARAM_SESSIONTYPE_HIERARCHICAL) {
		m_fileHandler.addButtons(getUiComponent());

		m_plotHandler.addButtons(getUiComponent());
		m_selectionHandler.subscribe(&m_plotHandler);

		m_materialHandler.addButtons(getUiComponent());
		m_selectionHandler.subscribe(&m_materialHandler);
	}

	m_projectInformationHandler.addButtons(getUiComponent());
}

std::string Application::storeTemporaryFile(const std::string& _content, uint64_t _uncompressedDataLength) {
	// Create a tmp file from uncompressing the data
		// Decode the encoded string into binary data
	int decoded_compressed_data_length = Base64decode_len(_content.c_str());
	char* decodedCompressedString = new char[decoded_compressed_data_length];

	Base64decode(decodedCompressedString, _content.c_str());

	// Decompress the data
	char* decodedString = new char[_uncompressedDataLength];
	uLongf destLen = (uLongf)_uncompressedDataLength;
	uLong  sourceLen = decoded_compressed_data_length;
	uncompress((Bytef*)decodedString, &destLen, (Bytef*)decodedCompressedString, sourceLen);

	delete[] decodedCompressedString;
	decodedCompressedString = nullptr;

	// Store the data in a temporary file
	std::string tmpFileName = DataBase::instance().getTmpFileName();

	std::ofstream file(tmpFileName, std::ios::binary);
	file.write(decodedString, _uncompressedDataLength);
	file.close();

	delete[] decodedString;
	decodedString = nullptr;

	return tmpFileName;
}

void Application::queueAction(ActionType _type, const ot::JsonDocument& _document) {
	ActionData newData;
	newData.type = _type;
	newData.document = _document.toJson();

	m_asyncActionMutex.lock();
	m_queuedActions.push_back(newData);
	m_asyncActionMutex.unlock();
}

void Application::asyncActionWorker() {
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
			//OT_LOG_W(message);
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
			//OT_LOG_W(message);
		}
	}

	m_selectionHandler.processSelectionChanged(selectedEntityIDsVerified, selectedVisibleEntityIDsVerified);
}

bool Application::getContinueAsyncActionWorker() {
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


	connectAction(OT_ACTION_CMD_PROJ_Save, this, &Application::handleProjectSave);
	connectAction(OT_ACTION_CMD_IsProjectOpen, this, &Application::handleCheckProjectOpen);
	connectAction(OT_ACTION_CMD_MODEL_SelectionChanged, this, &Application::handleSelectionChanged);
	connectAction(OT_ACTION_CMD_MODEL_ItemRenamed, this, &Application::handleItemRenamed);
	connectAction(OT_ACTION_CMD_SetVisualizationModel, this, &Application::handleSetVisualizationModel);
	connectAction(OT_ACTION_CMD_GetVisualizationModel, this, &Application::handleGetVisualizationModel);
	connectAction(OT_ACTION_CMD_MODEL_GetIsModified, this, &Application::handleGetIsModified);
	connectAction(OT_ACTION_CMD_MODEL_SetPropertiesFromJSON, this, &Application::handleSetPropertiesFromJSON);
	connectAction(OT_ACTION_CMD_MODEL_GenerateEntityIDs, this, &Application::handleGenerateEntityIDs);
	connectAction(OT_ACTION_CMD_MODEL_RequestImportTableFile, this, &Application::handleRequestImportTableFile);
	connectAction(OT_ACTION_CMD_MODEL_QueueMessages, this, &Application::handleQueueMessages);
	connectAction(OT_ACTION_CMD_MODEL_GetListOfFolderItems, this, &Application::handleGetListOfFolderItems);
	connectAction(OT_ACTION_CMD_MODEL_GetIDsOfFolderItemsOfType, this, &Application::handleGetIDsOfFolderItemsByType);
	connectAction(OT_ACTION_CMD_MODEL_UpdateVisualizationEntity, this, &Application::handleUpdateVisualizationEntity);
	connectAction(OT_ACTION_CMD_MODEL_UpdateGeometryEntity, this, &Application::handleUpdateGeometryEntity);
	connectAction(OT_ACTION_CMD_MODEL_ModelChangeOperationCompleted, this, &Application::handleModelChangeOperationCompleted);
	connectAction(OT_ACTION_CMD_MODEL_RequestUpdateVisualizationEntity, this, &Application::handleRequestUpdateVisualizationEntity);
	connectAction(OT_ACTION_CMD_MODEL_CheckParentUpdates, this, &Application::handleCheckParentUpdates);
	connectAction(OT_ACTION_CMD_MODEL_AddEntities, this, &Application::handleAddEntities);
	connectAction(OT_ACTION_CMD_MODEL_UpdateTopologyEntity, this, &Application::handleUpdateTopologyEntity);
	connectAction(OT_ACTION_CMD_MODEL_AddGeometryOperation, this, &Application::handleAddGeometryOperation);
	connectAction(OT_ACTION_CMD_MODEL_DeleteEntity, this, &Application::handleDeleteEntity);
	connectAction(OT_ACTION_CMD_MODEL_MeshingCompleted, this, &Application::handleMeshingCompleted);
	connectAction(OT_ACTION_CMD_MODEL_GetEntityInformationFromID, this, &Application::handleGetEntityInformationByID);
	connectAction(OT_ACTION_CMD_MODEL_GetEntityInformationFromName, this, &Application::handleGetEntityInformationByName);
	connectAction(OT_ACTION_CMD_MODEL_GetSelectedEntityInformation, this, &Application::handleGetSelectedEntityInformation);
	connectAction(OT_ACTION_CMD_MODEL_GetEntityChildInformationFromName, this, &Application::handleGetEntityChildInformationByName);
	connectAction(OT_ACTION_CMD_MODEL_GetEntityChildInformationFromID, this, &Application::handleGetEntityChildInformationByID);
	connectAction(OT_ACTION_CMD_MODEL_GetAllGeometryEntitiesForMeshing, this, &Application::handleGetAllGeometryEntitiesForMeshing);
	connectAction(OT_ACTION_CMD_MODEL_GetCurrentVisModelID, this, &Application::handleGetCurrentVisualizationModelID);
	
	connectAction(OT_ACTION_CMD_MODEL_EntitiesSelected, this, &Application::handleEntitiesSelected);
	connectAction(OT_ACTION_CMD_UI_PromptResponse, this, &Application::handlePromptResponse);
	connectAction(OT_ACTION_CMD_MODEL_GET_ENTITY_IDENTIFIER, this, &Application::handleGetEntityIdentifier);
	connectAction(OT_ACTION_CMD_MODEL_GET_ENTITIES_FROM_ANOTHER_COLLECTION, this, &Application::handleGetEntitiesFromAnotherCollection);
	connectAction(OT_ACTION_PARAM_MODEL_ViewsForProjectType, this, &Application::handleViewsFromProjectType);
	connectAction(OT_ACTION_CMD_MODEL_RequestVisualisationData, this, &Application::handleVisualisationDataRequest);
	connectAction(OT_ACTION_CMD_UI_TABLE_Setup, this, &Application::handleShowTable);
	connectAction(OT_ACTION_CMD_MODEL_ModelDialogConfirmed, this, &Application::handleModelDialogConfirmed);
	connectAction(OT_ACTION_CMD_MODEL_ModelDialogCanceled, this, &Application::handleModelDialogCanceled);
	connectAction(OT_ACTION_CMD_ImportTableFile, this, &Application::handleImportTableFile);

	// Properties
	connectAction(OT_ACTION_CMD_MODEL_GetEntityProperties, this, &Application::handleGetEntityProperties);
	connectAction(OT_ACTION_CMD_MODEL_GetEntityPropertiesFromName, this, &Application::handleGetEntityPropertiesByName);
	connectAction(OT_ACTION_CMD_MODEL_UpdatePropertyGrid, this, &Application::handleUpdatePropertyGrid);
	connectAction(OT_ACTION_CMD_MODEL_AddPropertiesToEntities, this, &Application::handleAddPropertiesToEntities);
	connectAction(OT_ACTION_CMD_MODEL_UpdatePropertiesOfEntities, this, &Application::handleUpdatePropertiesOfEntities);
	connectAction(OT_ACTION_CMD_MODEL_DeleteProperty, this, &Application::handleDeleteProperty);


	// Versions

	connectAction(OT_ACTION_CMD_MODEL_GetCurrentVersion, this, &Application::handleGetCurrentVersion);
	connectAction(OT_ACTION_CMD_MODEL_ActivateVersion, this, &Application::handleActivateVersion);
	connectAction(OT_ACTION_CMD_MODEL_VersionSelected, this, &Application::handleVersionSelected);
	connectAction(OT_ACTION_CMD_MODEL_VersionDeselected, this, &Application::handleVersionDeselected);
	connectAction(OT_ACTION_CMD_MODEL_SetVersionLabel, this, &Application::handleSetVersionLabel);
}

Application::~Application() {

}
