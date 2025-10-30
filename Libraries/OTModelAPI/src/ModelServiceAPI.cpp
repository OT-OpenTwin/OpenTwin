// @otlicense

// OpenTwin header
#include "OTCommunication/ActionTypes.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTModelAPI/ModelAPIManager.h"

std::list<std::string> ot::ModelServiceAPI::getListOfFolderItems(const std::string& _folder, bool recursive) {
	std::list<std::string> folderItems;

	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetListOfFolderItems, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Folder, JsonString(_folder, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Recursive, recursive, requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ModelAPIManager::sendToModel(EXECUTE, requestDoc, response)) {
		return folderItems;
	}
	
	// Process the result
	JsonDocument responseDoc;
	responseDoc.fromJson(response);

	// Check if the received document contains the required information
	if (!responseDoc.HasMember(OT_ACTION_PARAM_BASETYPE_List)) {
		OT_LOG_EA("ERROR: The member \"list\" is missing");
		return folderItems;
	}

	// Get information and iterate trough entries
	std::list<std::string> items = json::getStringList(responseDoc, OT_ACTION_PARAM_BASETYPE_List);

	for (auto i : items) {
		folderItems.push_back(i);
	}

	return folderItems;
}

std::list<ot::UID> ot::ModelServiceAPI::getIDsOfFolderItemsOfType(const std::string& _folder, const std::string& _entityClassName, bool recursive) {
	std::list<ot::UID> folderItemIDs;

	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetIDsOfFolderItemsOfType, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Folder, JsonString(_folder, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Type, JsonString(_entityClassName, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Recursive, recursive, requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ModelAPIManager::sendToModel(EXECUTE, requestDoc, response)) {
		return folderItemIDs;
	}
	
	// Process the result
	JsonDocument responseDoc;
	responseDoc.fromJson(response);
	folderItemIDs = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_BASETYPE_List);

	return folderItemIDs;
}

std::string ot::ModelServiceAPI::getCurrentModelVersion(void) {
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetCurrentVersion, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ModelAPIManager::sendToModel(EXECUTE, requestDoc, response)) {
		return std::string();
	}
	
	// Process the result from the model service

	std::string modelVersion = response;

	return modelVersion;
}

ot::UID ot::ModelServiceAPI::getCurrentVisualizationModelID(void) {
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetCurrentVisModelID, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ModelAPIManager::sendToModel(EXECUTE, requestDoc, response)) {
		return 0;
	}

	// Process the result from the model service
	JsonDocument responseDoc;
	responseDoc.fromJson(response);

	UID visModelID = ot::json::getUInt64(responseDoc, OT_ACTION_PARAM_BASETYPE_UID);

	return visModelID;
}

void ot::ModelServiceAPI::getAvailableMeshes(std::string& _meshFolderName, UID& _meshFolderID, std::string& _meshName, UID& _meshID) {
	std::list<std::string> meshFolder = { "Meshes" };
	std::list<EntityInformation> meshFolderInfo;
	getEntityInformation(meshFolder, meshFolderInfo);

	assert(meshFolderInfo.size() == 1);
	if (meshFolderInfo.size() != 1) return;

	_meshFolderName = meshFolderInfo.front().getEntityName();
	_meshFolderID = meshFolderInfo.front().getEntityID();

	std::list<EntityInformation> meshInfo;
	getEntityChildInformation(_meshFolderName, meshInfo, false);

	if (meshInfo.empty()) return;

	_meshName = meshInfo.front().getEntityName();
	_meshID = meshInfo.front().getEntityID();
}

void ot::ModelServiceAPI::getAvailableScripts(std::string& _scriptFolderName, UID& _scriptFolderID, std::string& _scriptName, UID& _scriptID) {
	std::list<std::string> scriptFolder = { "Scripts" };
	std::list<EntityInformation> scriptFolderInfo;
	getEntityInformation(scriptFolder, scriptFolderInfo);

	assert(scriptFolderInfo.size() == 1);
	if (scriptFolderInfo.size() != 1) return;

	_scriptFolderName = scriptFolderInfo.front().getEntityName();
	_scriptFolderID = scriptFolderInfo.front().getEntityID();

	std::list<EntityInformation> scriptInfo;
	getEntityChildInformation(_scriptFolderName, scriptInfo, false);

	if (scriptInfo.empty()) return;

	_scriptName = scriptInfo.front().getEntityName();
	_scriptID = scriptInfo.front().getEntityID();
}


void ot::ModelServiceAPI::addEntitiesToModel(const std::list<UID>& _topologyEntityIDList, const std::list<UID>& _topologyEntityVersionList, const std::list<bool>& _topologyEntityForceVisible, const std::list<UID>& _dataEntityIDList, const std::list<UID>& _dataEntityVersionList, const std::list<UID>& _dataEntityParentList, const std::string& _changeComment, bool askForBranchCreation, bool saveModel, bool _considerVisualization) {
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_AddEntities, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityIDList, JsonArray(_topologyEntityIDList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityVersionList, JsonArray(_topologyEntityVersionList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityForceShowList, JsonArray(_topologyEntityForceVisible, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataEntityIDList, JsonArray(_dataEntityIDList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataEntityVersionList, JsonArray(_dataEntityVersionList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataEntityParentList, JsonArray(_dataEntityParentList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Description, JsonString(_changeComment, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_AskForBranchCreation, askForBranchCreation, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_ConsiderVisualization, _considerVisualization, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_SaveModel, saveModel, requestDoc.GetAllocator());

	// Send the command
	std::string response;
	ModelAPIManager::sendToModel(EXECUTE, requestDoc, response);
}

void ot::ModelServiceAPI::addEntitiesToModel(std::list<UID>&& _topologyEntityIDList, std::list<UID>&& _topologyEntityVersionList, std::list<bool>&& _topologyEntityForceVisible, std::list<UID>&& _dataEntityIDList, std::list<UID>&& _dataEntityVersionList, std::list<UID>&& _dataEntityParentList, const std::string& _changeComment, bool askForBranchCreation, bool saveModel, bool _considerVisualization) {
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_AddEntities, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityIDList, JsonArray(_topologyEntityIDList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityVersionList, JsonArray(_topologyEntityVersionList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityForceShowList, JsonArray(_topologyEntityForceVisible, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataEntityIDList, JsonArray(_dataEntityIDList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataEntityVersionList, JsonArray(_dataEntityVersionList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataEntityParentList, JsonArray(_dataEntityParentList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Description, JsonString(_changeComment, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_AskForBranchCreation, askForBranchCreation, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_ConsiderVisualization, _considerVisualization, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_SaveModel, saveModel, requestDoc.GetAllocator());

	// Send the command
	std::string response;
	ModelAPIManager::sendToModel(EXECUTE, requestDoc, response);
}

void ot::ModelServiceAPI::addEntitiesToModel(const NewModelStateInfo& _newModelStateInfos, const std::string& _changeComment, bool _askForBranchCreation, bool _saveModel, bool _considerVisualization)
{
	addEntitiesToModel(_newModelStateInfos.getTopologyEntityIDs(),
		_newModelStateInfos.getTopologyEntityVersions(),
		_newModelStateInfos.getTopologyForceVisible(),
		_newModelStateInfos.getDataEntityIDs(),
		_newModelStateInfos.getDataEntityVersions(),
		_newModelStateInfos.getDataEntityParentIDs(),
		_changeComment, _askForBranchCreation, _saveModel, _considerVisualization);
}

void ot::ModelServiceAPI::addGeometryOperation(UID _newEntityID, UID _newEntityVersion, std::string _newEntityName,
	std::list<UID>& _dataEntityIDList, std::list<UID>& _dataEntityVersionList, std::list<UID>& _dataEntityParentList, std::list<std::string>& _childrenList,
	const std::string& _changeComment) {
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_AddGeometryOperation, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, _newEntityID, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion, _newEntityVersion, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityName, JsonString(_newEntityName, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataEntityIDList, JsonArray(_dataEntityIDList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataEntityVersionList, JsonArray(_dataEntityVersionList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataEntityParentList, JsonArray(_dataEntityParentList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityChildrenList, JsonArray(_childrenList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Description, JsonString(_changeComment, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	ModelAPIManager::sendToModel(EXECUTE, requestDoc, response);
}

void ot::ModelServiceAPI::deleteEntityFromModel(UID _entityID, bool _saveModel) {
	ModelServiceAPI::deleteEntitiesFromModel(UIDList{ _entityID }, _saveModel);
}

void ot::ModelServiceAPI::deleteEntityFromModel(const std::string& _entityName, bool _saveModel) {
	ModelServiceAPI::deleteEntitiesFromModel(std::list<std::string>{ _entityName }, _saveModel);
}

void ot::ModelServiceAPI::deleteEntitiesFromModel(const std::list<std::string>& _entityNameList, bool _saveModel) {
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_DeleteEntity, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityNameList, JsonArray(_entityNameList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_SaveModel, _saveModel, requestDoc.GetAllocator());

	// Send the command
	std::string response;
	ModelAPIManager::sendToModel(EXECUTE, requestDoc, response);
}

void ot::ModelServiceAPI::deleteEntitiesFromModel(const UIDList& _entityIDList, bool _saveModel) {
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_DeleteEntity, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, JsonArray(_entityIDList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_SaveModel, _saveModel, requestDoc.GetAllocator());

	// Send the command
	std::string response;
	ModelAPIManager::sendToModel(EXECUTE, requestDoc, response);
}

bool ot::ModelServiceAPI::getEntityInformation(UID _entity, EntityInformation& _entityInfo) {
	UIDList entities{ _entity };
	std::list<EntityInformation> entityInfoList;

	getEntityInformation(entities, entityInfoList);

	if (entityInfoList.empty()) return false;

	_entityInfo = entityInfoList.front();

	return true;
}

bool ot::ModelServiceAPI::getEntityInformation(const std::string& _entity, EntityInformation& _entityInfo) {
	std::list<std::string> entities{ _entity };
	std::list<EntityInformation> entityInfoList;

	getEntityInformation(entities, entityInfoList);

	if (entityInfoList.empty()) return false;

	_entityInfo = entityInfoList.front();

	return true;
}

void ot::ModelServiceAPI::getEntityInformation(const std::list<UID>& _entities, std::list<EntityInformation>& _entityInfo) {
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetEntityInformationFromID, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, JsonArray(_entities, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ModelAPIManager::sendToModel(EXECUTE, requestDoc, response)) {
		return;
	}

	// Process the result from the model service
	_entityInfo.clear();

	JsonDocument responseDoc;
	responseDoc.fromJson(response);

	std::list<UID> entityIDs;
	std::list<UID> entityVersions;
	std::list<std::string> entityNames;
	std::list<std::string> entityTypes;

	entityIDs = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
	entityVersions = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityVersionList);
	entityNames = ot::json::getStringList(responseDoc, OT_ACTION_PARAM_MODEL_EntityNameList);
	entityTypes = ot::json::getStringList(responseDoc, OT_ACTION_PARAM_MODEL_EntityTypeList);

	std::vector<UID> entityIDVec(entityIDs.begin(), entityIDs.end());
	std::vector<UID> entityVersionVec(entityVersions.begin(), entityVersions.end());
	std::vector<std::string> entityNameVec(entityNames.begin(), entityNames.end());
	std::vector<std::string> entityTypeVec(entityTypes.begin(), entityTypes.end());

	for (size_t index = 0; index < entityIDVec.size(); index++) {
		EntityInformation info;
		info.setEntityID(entityIDVec[index]);
		info.setEntityVersion(entityVersionVec[index]);
		info.setEntityName(entityNameVec[index]);
		info.setEntityType(entityTypeVec[index]);

		_entityInfo.push_back(info);
	}
}

void ot::ModelServiceAPI::getEntityInformation(const std::list<std::string>& _entities, std::list<EntityInformation>& _entityInfo) {
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetEntityInformationFromName, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityNameList, JsonArray(_entities, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ModelAPIManager::sendToModel(EXECUTE, requestDoc, response)) {
		return;
	}

	// Process the result from the model service
	_entityInfo.clear();

	JsonDocument responseDoc;
	responseDoc.fromJson(response);

	std::list<UID> entityIDs;
	std::list<UID> entityVersions;
	std::list<std::string> entityNames;
	std::list<std::string> entityTypes;

	entityIDs = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
	entityVersions = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityVersionList);
	entityNames = ot::json::getStringList(responseDoc, OT_ACTION_PARAM_MODEL_EntityNameList);
	entityTypes = ot::json::getStringList(responseDoc, OT_ACTION_PARAM_MODEL_EntityTypeList);

	std::vector<UID> entityIDVec(entityIDs.begin(), entityIDs.end());
	std::vector<UID> entityVersionVec(entityVersions.begin(), entityVersions.end());
	std::vector<std::string> entityNameVec(entityNames.begin(), entityNames.end());
	std::vector<std::string> entityTypeVec(entityTypes.begin(), entityTypes.end());

	for (size_t index = 0; index < entityIDVec.size(); index++) {
		EntityInformation info;
		info.setEntityID(entityIDVec[index]);
		info.setEntityVersion(entityVersionVec[index]);
		info.setEntityName(entityNameVec[index]);
		info.setEntityType(entityTypeVec[index]);

		_entityInfo.push_back(info);
	}
}

void ot::ModelServiceAPI::getSelectedEntityInformation(std::list<EntityInformation>& _entityInfo, const std::string& typeFilter) {
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetSelectedEntityInformation, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	if (typeFilter != "") {
		requestDoc.AddMember(OT_ACTION_PARAM_SETTINGS_Type, JsonString(typeFilter, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	}

	// Send the command
	std::string response;
	if (!ModelAPIManager::sendToModel(EXECUTE, requestDoc, response)) {
		return;
	}

	// Process the result from the model service
	_entityInfo.clear();

	JsonDocument responseDoc;
	responseDoc.fromJson(response);

	std::list<UID> entityIDs;
	std::list<UID> entityVersions;
	std::list<std::string> entityNames;
	std::list<std::string> entityTypes;

	entityIDs = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
	entityVersions = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityVersionList);
	entityNames = ot::json::getStringList(responseDoc, OT_ACTION_PARAM_MODEL_EntityNameList);
	entityTypes = ot::json::getStringList(responseDoc, OT_ACTION_PARAM_MODEL_EntityTypeList);

	std::vector<UID> entityIDVec(entityIDs.begin(), entityIDs.end());
	std::vector<UID> entityVersionVec(entityVersions.begin(), entityVersions.end());
	std::vector<std::string> entityNameVec(entityNames.begin(), entityNames.end());
	std::vector<std::string> entityTypeVec(entityTypes.begin(), entityTypes.end());

	for (size_t index = 0; index < entityIDVec.size(); index++) {
		EntityInformation info;
		info.setEntityID(entityIDVec[index]);
		info.setEntityVersion(entityVersionVec[index]);
		info.setEntityName(entityNameVec[index]);
		info.setEntityType(entityTypeVec[index]);

		_entityInfo.push_back(info);
	}
}

void ot::ModelServiceAPI::getEntityChildInformation(const std::string& _entity, std::list<EntityInformation>& _entityInfo, bool recursive) {
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetEntityChildInformationFromName, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Name, JsonString(_entity, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Recursive, recursive, requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ModelAPIManager::sendToModel(EXECUTE, requestDoc, response)) {
		return;
	}

	// Process the result from the model service
	_entityInfo.clear();

	JsonDocument responseDoc;
	responseDoc.fromJson(response);

	std::list<UID> entityIDs;
	std::list<UID> entityVersions;
	std::list<std::string> entityNames;
	std::list<std::string> entityTypes;

	entityIDs = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
	entityVersions = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityVersionList);
	entityNames = ot::json::getStringList(responseDoc, OT_ACTION_PARAM_MODEL_EntityNameList);
	entityTypes = ot::json::getStringList(responseDoc, OT_ACTION_PARAM_MODEL_EntityTypeList);

	std::vector<UID> entityIDVec(entityIDs.begin(), entityIDs.end());
	std::vector<UID> entityVersionVec(entityVersions.begin(), entityVersions.end());
	std::vector<std::string> entityNameVec(entityNames.begin(), entityNames.end());
	std::vector<std::string> entityTypeVec(entityTypes.begin(), entityTypes.end());

	for (size_t index = 0; index < entityIDVec.size(); index++) {
		EntityInformation info;
		info.setEntityID(entityIDVec[index]);
		info.setEntityVersion(entityVersionVec[index]);
		info.setEntityName(entityNameVec[index]);
		info.setEntityType(entityTypeVec[index]);

		_entityInfo.push_back(info);
	}
}

void ot::ModelServiceAPI::getEntityChildInformation(UID _entity, std::list<EntityInformation>& _entityInfo, bool recursive) {
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetEntityChildInformationFromID, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID, _entity, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Recursive, recursive, requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ModelAPIManager::sendToModel(EXECUTE, requestDoc, response)) {
		return;
	}

	// Process the result from the model service
	_entityInfo.clear();

	JsonDocument responseDoc;
	responseDoc.fromJson(response);

	std::list<UID> entityIDs;
	std::list<UID> entityVersions;
	std::list<std::string> entityNames;
	std::list<std::string> entityTypes;

	entityIDs = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
	entityVersions = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityVersionList);
	entityNames = ot::json::getStringList(responseDoc, OT_ACTION_PARAM_MODEL_EntityNameList);
	entityTypes = ot::json::getStringList(responseDoc, OT_ACTION_PARAM_MODEL_EntityTypeList);

	std::vector<UID> entityIDVec(entityIDs.begin(), entityIDs.end());
	std::vector<UID> entityVersionVec(entityVersions.begin(), entityVersions.end());
	std::vector<std::string> entityNameVec(entityNames.begin(), entityNames.end());
	std::vector<std::string> entityTypeVec(entityTypes.begin(), entityTypes.end());

	for (size_t index = 0; index < entityIDVec.size(); index++) {
		EntityInformation info;
		info.setEntityID(entityIDVec[index]);
		info.setEntityVersion(entityVersionVec[index]);
		info.setEntityName(entityNameVec[index]);
		info.setEntityType(entityTypeVec[index]);

		_entityInfo.push_back(info);
	}
}

void ot::ModelServiceAPI::addPropertiesToEntities(std::list<UID>& _entityList, const ot::PropertyGridCfg& _configuration) {
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_AddPropertiesToEntities, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, JsonArray(_entityList, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	JsonObject cfgObj;
	_configuration.addToJsonObject(cfgObj, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Config, cfgObj, requestDoc.GetAllocator());

	// Send the command
	std::string response;
	ModelAPIManager::sendToModel(EXECUTE, requestDoc, response);
}

void ot::ModelServiceAPI::getEntityProperties(UID _entity, bool _recursive, const std::string& _propertyGroupFilter, std::map<UID, EntityProperties>& _entityProperties) {
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetEntityProperties, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, _entity, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Recursive, _recursive, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Filter, JsonString(_propertyGroupFilter, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ModelAPIManager::sendToModel(EXECUTE, requestDoc, response)) {
		return;
	}

	// Process the result from the model service
	_entityProperties.clear();

	JsonDocument responseDoc;
	responseDoc.fromJson(response);

	std::list<UID> entityIDs;
	std::list<ot::ConstJsonObject> entityProperties;

	entityIDs = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
	entityProperties = ot::json::getObjectList(responseDoc, OT_ACTION_PARAM_MODEL_PropertyList);

	auto prop = entityProperties.begin();
	for (auto id : entityIDs) {
		if (prop == entityProperties.end()) {
			OT_LOG_E("List size mismatch");
			return;
		}
		ot::PropertyGridCfg cfg;
		cfg.setFromJsonObject(*prop);

		EntityProperties props;
		props.buildFromConfiguration(cfg, nullptr);

		_entityProperties[id] = props;

		prop++;
	}
}

void ot::ModelServiceAPI::getEntityProperties(const std::string& entityName, bool _recursive, const std::string& _propertyGroupFilter, std::map<UID, EntityProperties>& _entityProperties) {
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetEntityPropertiesFromName, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityName, JsonString(entityName, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Recursive, _recursive, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Filter, JsonString(_propertyGroupFilter, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ModelAPIManager::sendToModel(EXECUTE, requestDoc, response)) {
		return;
	}

	// Process the result from the model service
	_entityProperties.clear();

	JsonDocument responseDoc;
	responseDoc.fromJson(response);

	std::list<UID> entityIDs;
	std::list<ot::ConstJsonObject> entityProperties;

	entityIDs = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
	entityProperties = ot::json::getObjectList(responseDoc, OT_ACTION_PARAM_MODEL_PropertyList);

	auto prop = entityProperties.begin();
	for (auto id : entityIDs) {
		ot::PropertyGridCfg cfg;
		cfg.setFromJsonObject(*prop);

		EntityProperties props;
		props.buildFromConfiguration(cfg, nullptr);

		_entityProperties[id] = props;

		prop++;
	}
}

void ot::ModelServiceAPI::updateTopologyEntities(const NewModelStateInfo& _updatedEntities, const std::string& _comment, bool _considerVisualization) {
	updateTopologyEntities(_updatedEntities.getTopologyEntityIDs(), _updatedEntities.getTopologyEntityVersions(), _comment, _considerVisualization);
}

void ot::ModelServiceAPI::updateTopologyEntities(const ot::UIDList& topologyEntityIDs, const ot::UIDList& topologyEntityVersions, const std::string& comment, bool _considerVisualization) {
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_UpdateTopologyEntity, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityIDList, JsonArray(topologyEntityIDs, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityVersionList, JsonArray(topologyEntityVersions, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Description, JsonString(comment, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_ConsiderVisualization, _considerVisualization, requestDoc.GetAllocator());
	// Send the command
	std::string response;
	ModelAPIManager::sendToModel(EXECUTE, requestDoc, response);
}

void ot::ModelServiceAPI::enableMessageQueueing(bool flag) {
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_QueueMessages, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_QUEUE_FLAG, flag, requestDoc.GetAllocator());

	// Send the command
	std::string response;
	ModelAPIManager::sendToModel(EXECUTE, requestDoc, response);
}

void ot::ModelServiceAPI::modelChangeOperationCompleted(const std::string& description) {
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_ModelChangeOperationCompleted, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_Description, JsonString(description, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	ModelAPIManager::sendToModel(EXECUTE, requestDoc, response);
}

void ot::ModelServiceAPI::updatePropertyGrid() {
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_UpdatePropertyGrid, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	std::string response;
	ModelAPIManager::sendToModel(EXECUTE, requestDoc, response);
}
	