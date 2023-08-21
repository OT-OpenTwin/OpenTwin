/*
 *  ApplicationBase.cpp
 *
 *  Created on: 01/02/2021
 *	Author: Alexander Kuester, Peter Thoma
 *  Copyright (c) 2021, OpenTwin
 */

// OpenTwin header
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCore/rJSON.h"

#include "OpenTwinCommunication/ActionTypes.h"		// action member and types definition
#include "OpenTwinCommunication/Msg.h"				// message sending
#include "OpenTwinCommunication/IpConverter.h"

#include "OpenTwinFoundation/SettingsData.h"
#include "OpenTwinFoundation/ApplicationBase.h"
#include "OpenTwinFoundation/ModelComponent.h"
#include "OpenTwinFoundation/UiComponent.h"
#include "OpenTwinFoundation/UiPluginComponent.h"
#include "OpenTwinFoundation/AbstractUiNotifier.h"
#include "OpenTwinFoundation/AbstractModelNotifier.h"
#include "OpenTwinFoundation/ModalCommandBase.h"

#include <DataBase.h>
#include "TemplateDefaultManager.h"

// Third party header
#include "curl/curl.h"

#undef GetObject

#define OT_INFO_SERVICE_ID_SessionService ""

ot::ApplicationBase::ApplicationBase(const std::string & _serviceName, const std::string & _serviceType, AbstractUiNotifier * _uiNotifier, AbstractModelNotifier * _modelNotifier)
	: ServiceBase(_serviceName, _serviceType), m_modelComponent(nullptr), m_uiComponent(nullptr), m_uiNotifier(_uiNotifier), m_modelNotifier(_modelNotifier), m_uiMessageQueuingEnabled(false)
{
	m_directoryService.doc = nullptr;
	m_directoryService.enabledCounter = 0;
	m_directoryService.service = nullptr;

	m_sessionService.doc = nullptr;
	m_sessionService.enabledCounter = 0;
	m_sessionService.service = nullptr;

	otAssert(m_uiNotifier, "The provided ui notifier is null");
	otAssert(m_modelNotifier, "The provided model notifier is null");
}

ot::ApplicationBase::~ApplicationBase()
{
	if (m_uiNotifier) { delete m_uiNotifier; }
	if (m_modelNotifier) { delete m_modelNotifier; }
	for (auto plugin : m_uiPluginComponents) delete plugin.second;
}

std::string ot::ApplicationBase::deploymentPath(void) const {
	char * buffer = new char[100];
	if (buffer == nullptr) {
		return "";
	}
	size_t bufferSize = 100;
	if (_dupenv_s(&buffer, &bufferSize, "OPENTWIN_DEV_ROOT") != 0) {
		std::cout << "[ERROR] Please specify the environment variable \"OPENTWIN_DEV_ROOT\"" << std::endl;
		return std::string();
	}
	std::string path = buffer;
	delete[] buffer;

	path.append("\\Deployment\\");
	return path;
}

// ##########################################################################################################################################

// Setter

void ot::ApplicationBase::setDirectoryServiceURL(const std::string& _url) {
	m_directoryService.doc = nullptr;
	m_directoryService.enabledCounter = 0;
	if (m_directoryService.service != nullptr) { delete m_directoryService.service; }
	m_directoryService.service = new ServiceBase(OT_INFO_SERVICE_TYPE_LocalDirectoryService, OT_INFO_SERVICE_TYPE_LocalDirectoryService, _url, invalidServiceID);
	m_serviceIdMap.insert_or_assign(invalidServiceID - 1, m_directoryService);
}

void ot::ApplicationBase::setSessionServiceURL(const std::string & _url)
{
	m_sessionService.doc = nullptr;
	m_sessionService.enabledCounter = 0;
	if (m_sessionService.service != nullptr) { delete m_sessionService.service; }
	m_sessionService.service = new ServiceBase(OT_INFO_SERVICE_TYPE_SessionService, OT_INFO_SERVICE_TYPE_SessionService, _url, invalidServiceID);
	m_serviceIdMap.insert_or_assign(invalidServiceID, m_sessionService);
}

void ot::ApplicationBase::setSessionID(const std::string &id)
{
	m_sessionID = id;
	size_t index = m_sessionID.find(':');
	m_projectName    = m_sessionID.substr(0, index);
	m_collectionName = m_sessionID.substr(index + 1);
}

// ##########################################################################################################################################

// Getter

ot::ServiceBase * ot::ApplicationBase::getConnectedServiceByID(serviceID_t _id) {
	auto itm{ m_serviceIdMap.find(_id) };
	if (itm == m_serviceIdMap.end()) {
		assert(0); // Invalid ID
		return nullptr;
	}
	return itm->second.service;
}

ot::ServiceBase * ot::ApplicationBase::getConnectedServiceByName(const std::string & _name) {
	auto itm{ m_serviceNameMap.find(_name) };
	if (itm == m_serviceNameMap.end()) {
		assert(0); // Invalid ID
		return nullptr;
	}
	return itm->second.service;
}

ot::components::UiPluginComponent * ot::ApplicationBase::getUiPluginByUID(unsigned long long _pluginUID) {
	auto it = m_uiPluginComponents.find(_pluginUID);
	if (it != m_uiPluginComponents.end()) return it->second;
	otAssert(0, "Unknown plugin UID");
	return nullptr;
}

ot::components::UiPluginComponent * ot::ApplicationBase::getUiPluginByName(const std::string& _pluginName) {
	for (auto p : m_uiPluginComponents) {
		if (p.second->pluginName() == _pluginName) return p.second;
	}
	otAssert(0, "Unknown plugin Name");
	return nullptr;
}

bool ot::ApplicationBase::pluginExists(unsigned long long _pluginUID) {
	return m_uiPluginComponents.find(_pluginUID) != m_uiPluginComponents.end();
}

bool ot::ApplicationBase::pluginExists(const std::string& _pluginName) {
	for (auto p : m_uiPluginComponents) {
		if (p.second->pluginName() == _pluginName) { return true; }
	}
	return false;
}

// ##########################################################################################################################################

// IO

void ot::ApplicationBase::enableMessageQueuing(
	const std::string &			_service,
	bool						_flag
) {
	auto destination = m_serviceNameMap.find(_service);
	if (destination == m_serviceNameMap.end()) {
		otAssert(0, "Destination service not found");
		return;
	}
	destination->second.enabledCounter += (_flag ? 1 : -1);

	if (destination->second.enabledCounter < 0) {
		otAssert(0, "Enabled counter reached a negative value");
		return;
	}

	if (destination->second.enabledCounter == 0)
	{
		flushQueuedHttpRequests(_service);
	}
}

void ot::ApplicationBase::flushQueuedHttpRequests(const std::string & _service)
{
	auto destination = m_serviceNameMap.find(_service);
	if (destination == m_serviceNameMap.end()) {
		otAssert(0, "Destination service not found");
		return;
	}

	if (destination->second.doc != nullptr)
	{
		rapidjson::Document * queuedDoc = destination->second.doc;

		std::string response;
		if (queuedDoc->IsObject()) {
			sendHttpRequest(QUEUE, destination->second.service->serviceURL(), *queuedDoc, response);

			delete queuedDoc;
			destination->second.doc = nullptr;
		}
		else {
			otAssert(0, "The queued doc is not a object");
		}
	}
}

std::string ot::ApplicationBase::sendMessage(bool _queue, const std::string & _serviceName, OT_rJSON_doc & _doc)
{
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIds;
	return sendMessage(_queue, _serviceName, _doc, prefetchIds);
}

std::string ot::ApplicationBase::sendMessage(bool _queue, const std::string & _serviceName, OT_rJSON_doc & _doc, std::list<std::pair<UID, UID>> & _prefetchIds)
{
	auto destination = m_serviceNameMap.find(_serviceName);
	if (destination == m_serviceNameMap.end()) {
		otAssert(0, "Destination service not found");
		return "";
	}

	if (destination->second.enabledCounter > 0 && _queue)
	{
		rapidjson::Document *queuedDoc = nullptr;
		
		if (destination->second.doc == nullptr)
		{
			queuedDoc = new rapidjson::Document;
			queuedDoc->SetObject();

			ot::rJSON::add(*queuedDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_Compound);

			OT_rJSON_createValueArray(docs);
			OT_rJSON_createValueArray(prefetchedIDs);
			OT_rJSON_createValueArray(prefetchedVersions);

			ot::rJSON::add(*queuedDoc, OT_ACTION_PARAM_PROJECT_NAME, m_projectName);
			ot::rJSON::add(*queuedDoc, OT_ACTION_PARAM_PREFETCH_Documents, docs);
			ot::rJSON::add(*queuedDoc, OT_ACTION_PARAM_PREFETCH_ID, prefetchedIDs);
			ot::rJSON::add(*queuedDoc, OT_ACTION_PARAM_PREFETCH_Version, prefetchedVersions);

			destination->second.doc = queuedDoc;
		}
		else
		{
			queuedDoc = destination->second.doc;
		}

		rapidjson::Document::AllocatorType& allocator = queuedDoc->GetAllocator();

		rapidjson::Value v(_doc, allocator);
		(*queuedDoc)[OT_ACTION_PARAM_PREFETCH_Documents].PushBack(v, allocator);

		for (auto id : _prefetchIds)
		{
			(*queuedDoc)[OT_ACTION_PARAM_PREFETCH_ID].PushBack(id.first, allocator);
			(*queuedDoc)[OT_ACTION_PARAM_PREFETCH_Version].PushBack(id.second, allocator);
		}
		return "";
	}
	else
	{
		std::string response;
		sendHttpRequest(_queue ? QUEUE : EXECUTE, destination->second.service->serviceURL(), _doc, response);
		return response;
	}
}

std::string ot::ApplicationBase::broadcastMessage(bool _queue, const std::string& _message) {
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_SendBroadcastMessage);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_ID, m_serviceID);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SESSION_ID, m_sessionID);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MESSAGE, _message);

	std::string response;
	sendHttpRequest(_queue ? QUEUE : EXECUTE, m_sessionService.service->serviceURL(), doc, response);
	return response;
}

std::string ot::ApplicationBase::broadcastMessage(bool _queue, OT_rJSON_doc& _doc) {
	return broadcastMessage(_queue, rJSON::toJSON(_doc));
}

void ot::ApplicationBase::addModalCommand(ot::ModalCommandBase *command)
{
	otAssert(std::find(m_modalCommands.begin(), m_modalCommands.end(), command) == m_modalCommands.end(), "modal command is alreay in list"); // Ensure that the command is not yet in the list

	m_modalCommands.push_back(command);
}

void ot::ApplicationBase::removeModalCommand(ot::ModalCommandBase *command)
{
	otAssert(std::find(m_modalCommands.begin(), m_modalCommands.end(), command) != m_modalCommands.end(), "modal command is not in list");  // Ensure that the command is in the list

	m_modalCommands.erase(std::find(m_modalCommands.begin(), m_modalCommands.end(), command));
}

std::string ot::ApplicationBase::processActionWithModalCommands(const std::string & _action, OT_rJSON_doc & _doc)
{
	if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
	{
		std::string action = ot::rJSON::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);

		// Now process actions for all modal commands
		std::list<ModalCommandBase *> currentModalCommands = m_modalCommands; // We need to work on a copy, since the command might be erased from the list

		for (auto command : currentModalCommands)
		{
			if (command->executeAction(action, _doc))
			{
				// The action was handled by the modal command, so no further processing necessary
				return "";
			}
		}
	}	
	else if (_action == OT_ACTION_CMD_MODEL_SelectionChanged)
	{
		m_selectedEntities = ot::rJSON::getULongLongList(_doc, OT_ACTION_PARAM_MODEL_SelectedEntityIDs);

		for (auto command : m_modalCommands)
		{
			command->modelSelectionChanged(m_selectedEntities);
		}

		modelSelectionChanged();

		uiComponent()->sendUpdatedControlState();

		return "";  // No further processing necessary
	}

	OT_LOG_D("Action \"" + _action + "\" does not have any handler nor is a modal command");

	// Otherwise process the local actions
	return processAction(_action, _doc);
}

// ##########################################################################################################################################

// Protected functions

bool ot::ApplicationBase::sendHttpRequest(ot::MessageType _operation, const std::string& _url, OT_rJSON_doc& _doc, std::string& _response)
{
	return sendHttpRequest(_operation, _url, ot::rJSON::toJSON(_doc), _response);
}

bool ot::ApplicationBase::sendHttpRequest(ot::MessageType _operation, const std::string& _url, const std::string& _message, std::string& _response)
{
	return ot::msg::send(serviceURL(), _url, _operation, _message, _response);
}

bool ot::ApplicationBase::EnsureDataBaseConnection(void)
{
	DataBase::GetDataBase()->setProjectName(m_collectionName);

	return DataBase::GetDataBase()->InitializeConnection(m_databaseURL, m_siteID);;
}

// ##########################################################################################################################################

// Private functions

std::string ot::ApplicationBase::handleKeySequenceActivated(OT_rJSON_doc& _document) {
	if (m_uiNotifier) {
		std::string keySequence = ot::rJSON::getString(_document, OT_ACTION_PARAM_UI_KeySequence);
		m_uiNotifier->shortcutActivated(keySequence);
		return OT_ACTION_RETURN_VALUE_OK;
	}
	else {
		otAssert(0, "No UI connected");
		return OT_ACTION_RETURN_INDICATOR_Error "No UI connected";
	}
}

std::string ot::ApplicationBase::handleSettingsItemChanged(OT_rJSON_doc& _document) {
	ot::SettingsData * data = ot::SettingsData::parseFromJsonDocument(_document);
	if (data) {
		std::list<ot::AbstractSettingsItem *> items = data->items();
		if (items.empty()) {
			delete data;
			return OT_ACTION_RETURN_INDICATOR_Error "SettingsData does not contain any items";
		}
		else if (items.size() > 1) {
			delete data;
			return OT_ACTION_RETURN_INDICATOR_Error "SettingsData does contain more than 1 item";
		}
		if (settingChanged(items.front())) {
			ot::SettingsData * newData = createSettings();
			newData->saveToDatabase(m_databaseURL, m_siteID, DataBase::GetDataBase()->getUserName(), DataBase::GetDataBase()->getUserPassword(), m_DBuserCollection);
			if (newData) {
				if (m_uiComponent) {
					m_uiComponent->sendSettingsData(newData);
				}
				else {
					otAssert(0, "Settings changed received but no UI is conencted");
				}
				delete newData;
			}
		}
		else {
			ot::SettingsData * newData = createSettings();
			if (newData) {
				newData->saveToDatabase(m_databaseURL, m_siteID, DataBase::GetDataBase()->getUserName(), DataBase::GetDataBase()->getUserPassword(), m_DBuserCollection);
				delete newData;
			}
		}
		delete data;
		return OT_ACTION_RETURN_VALUE_OK;
	}
	else {
		return OT_ACTION_RETURN_INDICATOR_Error "Failed to parse SettingsData";
	}
}

std::string ot::ApplicationBase::handleContextMenuItemClicked(OT_rJSON_doc& _document) {
	if (m_uiNotifier) {
		std::string menuName = rJSON::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ContextMenuName);
		std::string itemName = rJSON::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ContextMenuItemName);
		m_uiNotifier->contextMenuItemClicked(menuName, itemName);
		return OT_ACTION_RETURN_VALUE_OK;
	}
	else {
		otAssert(0, "No UI connected");
		return OT_ACTION_RETURN_INDICATOR_Error "No UI connected";
	}
}

std::string ot::ApplicationBase::handleContextMenuItemCheckedChanged(OT_rJSON_doc& _document) {
	if (m_uiNotifier) {
		std::string menuName = rJSON::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ContextMenuName);
		std::string itemName = rJSON::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ContextMenuItemName);
		bool isChecked = rJSON::getBool(_document, OT_ACTION_PARAM_UI_CONTROL_CheckedState);
		m_uiNotifier->contextMenuItemCheckedChanged(menuName, itemName, isChecked);
		return OT_ACTION_RETURN_VALUE_OK;
	}
	else {
		otAssert(0, "No UI connected");
		return OT_ACTION_RETURN_INDICATOR_Error "No UI connected";
	}
}

void ot::ApplicationBase::__serviceConnected(const std::string & _name, const std::string & _type, const std::string & _url, serviceID_t _id) {
	// Prepare information to store data
	structServiceInformation info;
	info.doc = nullptr; info.enabledCounter = 0;

	// Check for duplicate
	if (m_serviceIdMap.find(_id) != m_serviceIdMap.end()) {
		assert(0); // service already registered
		return;
	}

	if (_type == OT_INFO_SERVICE_TYPE_UI) {
		// Store information
		assert(m_uiComponent == nullptr);
		m_uiComponent = new components::UiComponent(_name, _type, _url, _id, this);
		info.service = m_uiComponent;
		m_serviceIdMap.insert_or_assign(_id, info);
		m_serviceNameMap.insert_or_assign(_name, info);
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults("UI Configuration");

		enableMessageQueuing(m_uiComponent->serviceName(), true);
		SettingsData * serviceSettings = createSettings();
		if (serviceSettings) {
			m_uiComponent->sendSettingsData(serviceSettings);
			delete serviceSettings;
		}
		uiConnected(m_uiComponent);
		m_uiComponent->sendUpdatedControlState();
		enableMessageQueuing(m_uiComponent->serviceName(), false);
	}
	else if (_type == OT_INFO_SERVICE_TYPE_MODEL) {
		// Store information
		assert(m_modelComponent == nullptr);
		m_modelComponent = new components::ModelComponent(_name, _type, _url, _id, this);
		info.service = m_modelComponent;
		m_serviceIdMap.insert_or_assign(_id, info);
		m_serviceNameMap.insert_or_assign(_name, info);
		modelConnected(m_modelComponent);
	}
	else {
		// Store information
		auto s = new ServiceBase(_name, _type, _url, _id);
		info.service = s;
		m_serviceIdMap.insert_or_assign(_id, info);
		m_serviceNameMap.insert_or_assign(_name, info);
		serviceConnected(s);
	}
}

void ot::ApplicationBase::__serviceDisconnected(const std::string & _name, const std::string & _type, const std::string & _url, serviceID_t _id) {
	auto itm = m_serviceIdMap.find(_id);
	assert(itm != m_serviceIdMap.end());

	if (_type == OT_INFO_SERVICE_TYPE_UI) {
		assert(itm->second.service == (ServiceBase *)m_uiComponent);
		uiDisconnected(m_uiComponent);
		delete m_uiComponent;
		m_uiComponent = nullptr;
	}
	else if (_type == OT_INFO_SERVICE_TYPE_MODEL) {
		assert(itm->second.service == (ServiceBase *)m_modelComponent);
		modelDisconnected(m_modelComponent);

		delete m_modelComponent;
		m_modelComponent = nullptr;
	}
	else {
		serviceDisconnected(itm->second.service);
		delete itm->second.service;
	}
	if (itm->second.doc != nullptr) { delete itm->second.doc; }
	m_serviceIdMap.erase(_id);
	m_serviceNameMap.erase(_name);
}

void ot::ApplicationBase::__shuttingDown(bool _requestedAsCommand) {
	if (!_requestedAsCommand) preShutdown();
	else shuttingDown();
}

std::string ot::ApplicationBase::__processMessage(const std::string & _message, OT_rJSON_doc & _doc, serviceID_t _senderID) {
#ifdef _DEBUG
	auto sender{ getConnectedServiceByID(_senderID) };
	assert(sender != nullptr); // Sender was not registered
	return processMessage(sender, _message, _doc);
#else
	return processMessage(getConnectedServiceByID(_senderID), _message, _doc);
#endif // _DEBUG
}

void ot::ApplicationBase::__addUiPlugin(components::UiPluginComponent * _component) {
	if (_component == nullptr) { return; }
	auto it = m_uiPluginComponents.find(_component->pluginUID());
	if (it != m_uiPluginComponents.end()) {
		delete it->second;
	}
	for (auto it : m_uiPluginComponents) {
		if (it.second->pluginName() == _component->pluginName()) {
			delete it.second;
			break;
		}
	}
	m_uiPluginComponents.insert_or_assign(_component->pluginUID(), _component);
}

void ot::ApplicationBase::prefetchDocumentsFromStorage(const std::list<UID> &entities)
{
	// First get the version information for all entities
	std::list<ot::EntityInformation> entityInfo;
	m_modelComponent->getEntityInformation(entities, entityInfo);

	// Now prefetch the documents
	prefetchDocumentsFromStorage(entityInfo);
}

void ot::ApplicationBase::prefetchDocumentsFromStorage(const std::list<ot::EntityInformation> &entityInfo)
{
	std::list<std::pair<UID, UID>> prefetchIdandVersion;

	for (auto entity : entityInfo)
	{
		m_prefetchedEntityVersions[entity.getID()] = entity.getVersion();

		prefetchIdandVersion.push_back(std::pair<UID, UID>(entity.getID(), entity.getVersion()));
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIdandVersion);
}

ot::UID ot::ApplicationBase::getPrefetchedEntityVersion(UID entityID)
{
	otAssert(m_prefetchedEntityVersions.count(entityID) > 0, "The entity was not prefetched");

	return m_prefetchedEntityVersions[entityID];
}

