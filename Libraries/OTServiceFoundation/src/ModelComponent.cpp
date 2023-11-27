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
#include "EntityPlot1D.h"
#include "EntityResult1D.h"
#include "EntityResult1DData.h"
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
	ServiceBase{ _name, _type, _url, _id }, m_application{ _application }
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

// #########################################################################################################

// Model management

std::list<std::string> ot::components::ModelComponent::getListOfFolderItems(const std::string & _folder, bool recursive)
{
	std::list<std::string> folderItems;

	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetListOfFolderItems, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Folder, JsonString(_folder, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Recursive, recursive, requestDoc.GetAllocator());	

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		std::cout << "ERROR: Failed to get list of folder items: Failed to send HTTP request" << std::endl;
		return folderItems;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::cout << "ERROR: Failed to get list of folder items: " << response << std::endl;
		return folderItems;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::cout << "ERROR: Failed to get list of folder items: " << response << std::endl;
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
	
	for (auto i : items)
	{
		folderItems.push_back(i);
	}

	return folderItems;
}

std::list<ot::UID> ot::components::ModelComponent::getIDsOfFolderItemsOfType(const std::string &_folder, const std::string &_entityClassName, bool recursive)
{
	std::list<ot::UID> folderItemIDs;

	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetIDsOfFolderItemsOfType, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Folder, JsonString(_folder, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Type, JsonString(_entityClassName, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Recursive, recursive, requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		std::cout << "ERROR: Failed to get list of folder items: Failed to send HTTP request" << std::endl;
		return folderItemIDs;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::cout << "ERROR: Failed to get list of folder items: " << response << std::endl;
		return folderItemIDs;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::cout << "ERROR: Failed to get list of folder items: " << response << std::endl;
		return folderItemIDs;
	}

	// Process the result
	JsonDocument responseDoc;
	responseDoc.fromJson(response);
	folderItemIDs = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_BASETYPE_List);

	return folderItemIDs;
}

std::string ot::components::ModelComponent::getCurrentModelVersion(void)
{
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetCurrentVersion, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		std::cout << "ERROR: Failed to get new entity id's: Failed to send HTTP request" << std::endl;
		return "";
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::cout << "ERROR: Failed to get new entity id's: " << response << std::endl;
		return "";
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::cout << "ERROR: Failed to get new entity id's: " << response << std::endl;
		return "";
	}

	// Process the result from the model service

	JsonDocument responseDoc;
	responseDoc.fromJson(response);

	std::string modelVersion = ot::json::getString(responseDoc, OT_ACTION_PARAM_BASETYPE_Props);

	return modelVersion;
}

EntityBase * ot::components::ModelComponent::readEntityFromEntityIDandVersion(UID _entityID, UID _version, ClassFactoryHandler&classFactory)
{
	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(_entityID, _version, doc))
	{
		return nullptr;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	EntityBase *entity = classFactory.CreateEntity(entityType);

	if (entity != nullptr)
	{
		std::map<UID, EntityBase*> entityMap;
		entity->restoreFromDataBase(nullptr, nullptr, nullptr, doc_view, entityMap);
	}

	return entity;
}

ot::UID ot::components::ModelComponent::getCurrentVisualizationModelID(void) {
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetCurrentVisModelID, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		OT_LOG_EA("Failed to get visualization model id: Failed to send HTTP request");
		return 0;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::cout << "ERROR: Failed to get visualization model id: " << response << std::endl;
		return 0;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::cout << "ERROR: Failed to get visualization model id: " << response << std::endl;
		return 0;
	}

	// Process the result from the model service

	JsonDocument responseDoc;
	responseDoc.fromJson(response);

	UID visModelID = ot::json::getUInt64(responseDoc, OT_ACTION_PARAM_BASETYPE_UID);

	return visModelID;
}

void ot::components::ModelComponent::getAvailableMeshes(std::string & _meshFolderName, UID & _meshFolderID, std::string & _meshName, UID & _meshID) {
	std::list<std::string> meshFolder = { "Meshes" };
	std::list<EntityInformation> meshFolderInfo;
	getEntityInformation(meshFolder, meshFolderInfo);

	assert(meshFolderInfo.size() == 1);
	if (meshFolderInfo.size() != 1) return;

	_meshFolderName = meshFolderInfo.front().getName();
	_meshFolderID = meshFolderInfo.front().getID();

	std::list<EntityInformation> meshInfo;
	getEntityChildInformation(_meshFolderName, meshInfo);

	if (meshInfo.empty()) return;

	_meshName = meshInfo.front().getName();
	_meshID = meshInfo.front().getID();
}

// #########################################################################################################

// Entity management

void ot::components::ModelComponent::addEntitiesToModel(std::list<UID> & _topologyEntityIDList, std::list<UID> & _topologyEntityVersionList, std::list<bool> & _topologyEntityForceVisible,
														std::list<UID> & _dataEntityIDList, std::list<UID> & _dataEntityVersionList, std::list<UID> & _dataEntityParentList,
														const std::string & _changeComment)
{
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_AddEntities, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityIDList, JsonArray(_topologyEntityIDList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityVersionList, JsonArray(_topologyEntityVersionList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityForceShowList, JsonArray(_topologyEntityForceVisible, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataEntityIDList, JsonArray(_dataEntityIDList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataEntityVersionList, JsonArray(_dataEntityVersionList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataEntityParentList, JsonArray(_dataEntityParentList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Description, JsonString(_changeComment, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		std::cout << "ERROR: Failed to add entities to model: Failed to send HTTP request" << std::endl;
		return;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::cout << "ERROR: Failed to add entities to model: " << response << std::endl;
		return;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::cout << "ERROR: Failed to add entities to model: " << response << std::endl;
		return;
	}
}

void ot::components::ModelComponent::addEntitiesToModel(std::list<UID>&& _topologyEntityIDList, std::list<UID>&& _topologyEntityVersionList, std::list<bool>&& _topologyEntityForceVisible, std::list<UID>&& _dataEntityIDList, std::list<UID>&& _dataEntityVersionList, std::list<UID>&& _dataEntityParentList, const std::string& _changeComment)
{
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_AddEntities, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityIDList, JsonArray(_topologyEntityIDList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityVersionList, JsonArray(_topologyEntityVersionList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityForceShowList, JsonArray(_topologyEntityForceVisible, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataEntityIDList, JsonArray(_dataEntityIDList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataEntityVersionList, JsonArray(_dataEntityVersionList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataEntityParentList, JsonArray(_dataEntityParentList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Description, JsonString(_changeComment, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		OT_LOG_EA("Failed to add entities to model: Failed to send HTTP request");
		return;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		OT_LOG_EAS("Failed to add entities to model: " + response);
		return;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		OT_LOG_EAS("Failed to add entities to model:" + response);
		return;
	}
}

void ot::components::ModelComponent::addGeometryOperation(UID _newEntityID, UID _newEntityVersion, std::string _newEntityName,
														  std::list<UID> & _dataEntityIDList, std::list<UID> & _dataEntityVersionList, std::list<UID> & _dataEntityParentList, std::list<std::string> & _childrenList, 
														  const std::string & _changeComment)
{
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
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		std::cout << "ERROR: Failed to add entities to model: Failed to send HTTP request" << std::endl;
		return;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::cout << "ERROR: Failed to add entities to model: " << response << std::endl;
		return;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::cout << "ERROR: Failed to add entities to model: " << response << std::endl;
		return;
	}
}

void ot::components::ModelComponent::deleteEntitiesFromModel(std::list<std::string> & _entityNameList, bool _saveModel)
{
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_DeleteEntity, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityNameList, JsonArray(_entityNameList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_SaveModel, _saveModel, requestDoc.GetAllocator());
	
	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		OT_LOG_EA("Failed to delete entities from model : Failed to send HTTP request");
		return;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		OT_LOG_EAS("Failed to delete entities from model: " + response);
		return;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		OT_LOG_EAS("Failed to delete entities from model: " + response);
		return;
	}
}

void ot::components::ModelComponent::getEntityInformation(const std::list<UID> & _entities, std::list<EntityInformation> & _entityInfo)
{
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetEntityInformationFromID, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, JsonArray(_entities, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		OT_LOG_EA("Failed to get new entity id's: Failed to send HTTP request");
		return;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		OT_LOG_EAS("Failed to get new entity id's: " + response);
		return;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::cout << "ERROR: Failed to get new entity id's: " << response << std::endl;
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

	for (size_t index = 0; index < entityIDVec.size(); index++)
	{
		EntityInformation info;
		info.setID(entityIDVec[index]);
		info.setVersion(entityVersionVec[index]);
		info.setName(entityNameVec[index]);
		info.setType(entityTypeVec[index]);

		_entityInfo.push_back(info);
	}
}

void ot::components::ModelComponent::getEntityInformation(const std::string & _entity, EntityInformation & _entityInfo)
{
	std::list<std::string> entities{_entity};
	std::list<EntityInformation> entityInfoList;

	getEntityInformation(entities, entityInfoList);

	assert(entityInfoList.size() == 1);

	_entityInfo = entityInfoList.front();
}

void ot::components::ModelComponent::getEntityInformation(const std::list<std::string> & _entities, std::list<EntityInformation> & _entityInfo)
{
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetEntityInformationFromName, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityNameList, JsonArray(_entities, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		OT_LOG_EA("Failed to get new entity id's: Failed to send HTTP request");
		return;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		OT_LOG_EAS("Failed to get new entity id's: " + response);
		return;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		OT_LOG_EAS("Failed to get new entity id's: " + response);
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

	for (size_t index = 0; index < entityIDVec.size(); index++)
	{
		EntityInformation info;
		info.setID(entityIDVec[index]);
		info.setVersion(entityVersionVec[index]);
		info.setName(entityNameVec[index]);
		info.setType(entityTypeVec[index]);

		_entityInfo.push_back(info);
	}
}

void ot::components::ModelComponent::getSelectedEntityInformation(std::list<EntityInformation> & _entityInfo, const std::string &typeFilter)
{
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetSelectedEntityInformation, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SETTINGS_Type, JsonString(typeFilter, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
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

	for (size_t index = 0; index < entityIDVec.size(); index++)
	{
		EntityInformation info;
		info.setID(entityIDVec[index]);
		info.setVersion(entityVersionVec[index]);
		info.setName(entityNameVec[index]);
		info.setType(entityTypeVec[index]);

		_entityInfo.push_back(info);
	}
}

void ot::components::ModelComponent::getEntityChildInformation(const std::string & _entity, std::list<EntityInformation> & _entityInfo) {
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetEntityChildInformationFromName, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Name, JsonString(_entity, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
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

	for (size_t index = 0; index < entityIDVec.size(); index++)
	{
		EntityInformation info;
		info.setID(entityIDVec[index]);
		info.setVersion(entityVersionVec[index]);
		info.setName(entityNameVec[index]);
		info.setType(entityTypeVec[index]);

		_entityInfo.push_back(info);
	}
}

void ot::components::ModelComponent::getEntityChildInformation(UID _entity, std::list<EntityInformation> & _entityInfo) {
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetEntityChildInformationFromID, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID, _entity, requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
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

	for (size_t index = 0; index < entityIDVec.size(); index++)
	{
		EntityInformation info;
		info.setID(entityIDVec[index]);
		info.setVersion(entityVersionVec[index]);
		info.setName(entityNameVec[index]);
		info.setType(entityTypeVec[index]);

		_entityInfo.push_back(info);
	}
}

void ot::components::ModelComponent::addPropertiesToEntities(std::list<UID>& _entityList, const std::string& _propertiesJson)
{
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_AddPropertiesToEntities, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, JsonArray(_entityList, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_JSON, JsonString(_propertiesJson, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	
	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		std::cout << "ERROR: Failed to get new entity id's: Failed to send HTTP request" << std::endl;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::cout << "ERROR: Failed to get new entity id's: " << response << std::endl;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::cout << "ERROR: Failed to get new entity id's: " << response << std::endl;
	}
}

void ot::components::ModelComponent::getEntityProperties(UID _entity, bool _recursive, const std::string& _propertyGroupFilter, std::map<UID, EntityProperties>& _entityProperties) {
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetEntityProperties, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, _entity, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Recursive, _recursive, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Filter, JsonString(_propertyGroupFilter, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		std::cout << "ERROR: Failed to get entity properties: Failed to send HTTP request" << std::endl;
		return;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::cout << "ERROR: Failed to get entity properties: " << response << std::endl;
		return;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::cout << "ERROR: Failed to get entity properties: " << response << std::endl;
		return;
	}

	// Process the result from the model service
	_entityProperties.clear();

	JsonDocument responseDoc;
	responseDoc.fromJson(response);

	std::list<UID> entityIDs;
	std::list<std::string> entityProperties;

	entityIDs = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
	entityProperties = ot::json::getStringList(responseDoc, OT_ACTION_PARAM_MODEL_PropertyList);

	auto prop = entityProperties.begin();
	for (auto id : entityIDs)
	{
		EntityProperties props;
		props.buildFromJSON(*prop);

		_entityProperties[id] = props;

		prop++;
	}
}

void ot::components::ModelComponent::getEntityProperties(const std::string &entityName, bool _recursive, const std::string& _propertyGroupFilter, std::map<UID, EntityProperties>& _entityProperties) {
	// Prepare the request
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetEntityPropertiesFromName, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityName, JsonString(entityName, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Recursive, _recursive, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_Filter, JsonString(_propertyGroupFilter, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		std::cout << "ERROR: Failed to get entity properties: Failed to send HTTP request" << std::endl;
		return;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::cout << "ERROR: Failed to get entity properties: " << response << std::endl;
		return;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::cout << "ERROR: Failed to get entity properties: " << response << std::endl;
		return;
	}

	// Process the result from the model service
	_entityProperties.clear();

	JsonDocument responseDoc;
	responseDoc.fromJson(response);

	std::list<UID> entityIDs;
	std::list<std::string> entityProperties;

	entityIDs = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
	entityProperties = ot::json::getStringList(responseDoc, OT_ACTION_PARAM_MODEL_PropertyList);

	auto prop = entityProperties.begin();
	for (auto id : entityIDs)
	{
		EntityProperties props;
		props.buildFromJSON(*prop);

		_entityProperties[id] = props;

		prop++;
	}
}

void ot::components::ModelComponent::updatePropertyGrid()
{
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_UpdatePropertyGrid, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		std::cout << "ERROR: Failed to get entity properties: Failed to send HTTP request" << std::endl;
		return;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::cout << "ERROR: Failed to get entity properties: " << response << std::endl;
		return;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::cout << "ERROR: Failed to get entity properties: " << response << std::endl;
		return;
	}
}

void ot::components::ModelComponent::updateTopologyEntities(ot::UIDList& topologyEntityIDs, ot::UIDList& topologyEntityVersions, const std::string& comment)
{
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_UpdateTopologyEntity, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityIDList, JsonArray(topologyEntityIDs, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_TopologyEntityVersionList, JsonArray(topologyEntityVersions, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Description, JsonString(comment, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		std::cout << "ERROR: Failed to add entities to model: Failed to send HTTP request" << std::endl;
		return;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::cout << "ERROR: Failed to add entities to model: " << response << std::endl;
		return;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::cout << "ERROR: Failed to add entities to model: " << response << std::endl;
		return;
	}
}

// #########################################################################################################

// Entity management helper functions

EntityPlot1D *ot::components::ModelComponent::addPlot1DEntity(const std::string &name, const std::string &title, const std::list<std::pair<UID,std::string>> &curves)
{
	EntityPlot1D *plot = new EntityPlot1D(createEntityUID(), nullptr, nullptr, nullptr, nullptr, "Model");

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

EntityResult1D *ot::components::ModelComponent::addResult1DEntity(const std::string &name, const std::vector<double> &xdata, 
	const std::vector<double> &ydataRe, const std::vector<double> &ydataIm,
	const std::string &xlabel, const std::string &xunit,
	const std::string &ylabel, const std::string &yunit, int colorID, bool visible)
{
	EntityResult1D *curve = new EntityResult1D(createEntityUID(), nullptr, nullptr, nullptr, nullptr, "Model");

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


// #########################################################################################################

// UID management

ot::UID ot::components::ModelComponent::createEntityUID(void)
{
	return getUniqueUIDGenerator()->getUID();
}

DataStorageAPI::UniqueUIDGenerator* ot::components::ModelComponent::getUniqueUIDGenerator(void) 
{
	assert(uniqueUIDGenerator != nullptr);

	return uniqueUIDGenerator;
}

// #########################################################################################################

// Misc. helper functions

void ot::components::ModelComponent::importTableFile(const std::string &itemName)
{
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_ImportTableFile, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_NAME, JsonString(itemName, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
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

void ot::components::ModelComponent::enableMessageQueueing(bool flag)
{
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_QueueMessages, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_QUEUE_FLAG, flag, requestDoc.GetAllocator());

	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
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

void ot::components::ModelComponent::modelChangeOperationCompleted(const std::string &description)
{
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_ModelChangeOperationCompleted, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_Description, JsonString(description, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	
	// Send the command
	std::string response;
	if (!ot::msg::send(m_application->serviceURL(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
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

std::string  ot::components::ModelComponent::sendMessage(bool _queue, JsonDocument & _doc)
{
	return m_application->sendMessage(_queue, "Model", _doc);
}

// #########################################################################################################

// Helper functions for managing list of newly created entities

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

void ot::components::ModelComponent::storeNewEntities(const std::string &description)
{
	addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, description);
	clearNewEntityList();
}
