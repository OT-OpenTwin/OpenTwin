// @otlicense
// File: ApplicationBase.cpp
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

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/ThisService.h"

#include "OTGui/Property.h"
#include "OTGui/PropertyGroup.h"
#include "OTGuiAPI/GuiAPIManager.h"

#include "OTCommunication/Msg.h"				// message sending
#include "OTCommunication/ActionTypes.h"		// action member and types definition
#include "OTCommunication/IpConverter.h"
#include "OTCommunication/ActionTypes.h"        // Action types
#include "OTCommunication/ActionDispatcher.h"

#include "OTModelAPI/ModelServiceAPI.h"
#include "OTModelAPI/ModelAPIManager.h"

#include "OTServiceFoundation/ApplicationBase.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/AbstractUiNotifier.h"
#include "OTServiceFoundation/AbstractModelNotifier.h"
#include "OTServiceFoundation/ModalCommandBase.h"
#include "OTServiceFoundation/FrontendLogNotifier.h"
#include "OTServiceFoundation/InitialSelectionHelper.h"

#include "DataBase.h"
#include "Document\DocumentAccess.h"
#include "Connection\ConnectionAPI.h"

#include "TemplateDefaultManager.h"

#include "OTServiceFoundation/ExternalServicesComponent.h"

#include "InvalidUID.h"

// Third party header
#include "curl/curl.h"
#include "OTServiceFoundation/MessageQueueHandler.h"

#undef GetObject

ot::ApplicationBase::ApplicationBase(const std::string & _serviceName, const std::string & _serviceType, AbstractUiNotifier * _uiNotifier, AbstractModelNotifier * _modelNotifier)
	: ServiceBase(_serviceName, _serviceType), m_modelComponent(nullptr), m_uiComponent(nullptr), m_uiNotifier(_uiNotifier),
	m_modelNotifier(_modelNotifier), m_uiMessageQueuingEnabled(false), m_sessionService(nullptr), m_directoryService(nullptr),
	m_initialSelectionHelper(nullptr)
{
	new FrontendLogNotifier(this); // Log Dispatcher gets the ownership of the notifier.

	// Connect actions
	m_actionHandler.connectAction(OT_ACTION_CMD_KeySequenceActivated, this, &ApplicationBase::handleKeySequenceActivated, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_SettingsItemChanged, this, &ApplicationBase::handleSettingsItemChanged, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_RegisterNewLibraryManagementService, this, &ApplicationBase::handleRegisterNewLMS, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_GetDebugInformation, this, &ApplicationBase::handleGetDebugInformation, ot::SECURE_MESSAGE_TYPES);
}

ot::ApplicationBase::~ApplicationBase()
{
	if (m_uiNotifier) {
		delete m_uiNotifier;
		m_uiNotifier = nullptr;
	}
	if (m_modelNotifier) {
		delete m_modelNotifier;
		m_modelNotifier = nullptr;
	}
	if (m_initialSelectionHelper) {
		delete m_initialSelectionHelper;
		m_initialSelectionHelper = nullptr;
	}
}

// ##########################################################################################################################################

// Setter

void ot::ApplicationBase::setSessionServiceURL(const std::string & _url)
{
	if (m_sessionService != nullptr)
	{
		delete m_sessionService;
		m_sessionService = nullptr;
	}
	
	m_sessionService = new ServiceBase(OT_INFO_SERVICE_TYPE_LocalSessionService, OT_INFO_SERVICE_TYPE_LocalSessionService, _url, invalidServiceID);
}

void ot::ApplicationBase::initiallySelectEntity(const std::string& _entityName) {
	if (m_initialSelectionHelper) {
		OT_LOG_WA("Initial selection helper is already initialized...");
		return;
	}

	m_initialSelectionHelper = new ot::InitialSelectionHelper;
	m_initialSelectionHelper->setMode(ot::InitialSelectionHelper::Mode::SelectByName);
	m_initialSelectionHelper->setEntityName(_entityName);

	if (m_modelComponent) {
		m_initialSelectionHelper->setModelUrl(m_modelComponent->getServiceURL());
	}
	if (m_uiComponent) {
		m_initialSelectionHelper->setUiUrl(m_uiComponent->getServiceURL());
	}

	m_initialSelectionHelper->runIfReady();
}

void ot::ApplicationBase::initiallySelectFirstChildEntityOf(const std::string& _parentEntityName) {
	if (m_initialSelectionHelper != nullptr) {
		OT_LOG_WA("Initial selection helper is already initialized...");
		return;
	}

	m_initialSelectionHelper = new ot::InitialSelectionHelper;
	m_initialSelectionHelper->setMode(ot::InitialSelectionHelper::Mode::SelectFirstChild);
	m_initialSelectionHelper->setEntityName(_parentEntityName);

	if (m_modelComponent) {
		m_initialSelectionHelper->setModelUrl(m_modelComponent->getServiceURL());
	}
	if (m_uiComponent) {
		m_initialSelectionHelper->setUiUrl(m_uiComponent->getServiceURL());
	}

	m_initialSelectionHelper->runIfReady();
}

// ##########################################################################################################################################

// Getter

std::string ot::ApplicationBase::deploymentPath() const {
	char* buffer = new char[100];
	if (buffer == nullptr) {
		return "";
	}
	size_t bufferSize = 100;
	if (_dupenv_s(&buffer, &bufferSize, "OPENTWIN_DEV_ROOT") != 0) {
		OT_LOG_E("Please specify the environment variable \"OPENTWIN_DEV_ROOT\"");
		return std::string();
	}
	std::string path = buffer;
	delete[] buffer;

	path.append("\\Deployment\\");
	return path;
}

ot::ServiceBase * ot::ApplicationBase::getConnectedServiceByID(serviceID_t _id) {
	auto itm{ m_serviceIdMap.find(_id) };
	if (itm == m_serviceIdMap.end()) {
		assert(0); // Invalid ID
		return nullptr;
	}
	return itm->second;
}

ot::ServiceBase * ot::ApplicationBase::getConnectedServiceByName(const std::string & _name) {
	auto itm{ m_serviceNameMap.find(_name) };
	if (itm == m_serviceNameMap.end()) {
		assert(0); // Invalid ID
		return nullptr;
	}
	return itm->second;
}

void ot::ApplicationBase::prefetchDocumentsFromStorage(const std::list<UID>& _entities) {
	// First get the version information for all entities
	std::list<ot::EntityInformation> entityInfo;
	ModelServiceAPI::getEntityInformation(_entities, entityInfo);

	// Now prefetch the documents
	prefetchDocumentsFromStorage(entityInfo);
}

void ot::ApplicationBase::prefetchDocumentsFromStorage(const std::list<ot::EntityInformation>& _entityInfo) {
	std::list<std::pair<UID, UID>> prefetchIdandVersion;

	for (const auto& entity : _entityInfo) {
		if (entity.getEntityID() != ot::getInvalidUID() && entity.getEntityVersion() != ot::getInvalidUID()) {
			m_prefetchedEntityVersions[entity.getEntityID()] = entity.getEntityVersion();

			prefetchIdandVersion.push_back(std::pair<UID, UID>(entity.getEntityID(), entity.getEntityVersion()));
		}
	}

	DataBase::instance().prefetchDocumentsFromStorage(prefetchIdandVersion);
}

ot::UID ot::ApplicationBase::getPrefetchedEntityVersion(UID _entityID) {
	if (m_prefetchedEntityVersions.count(_entityID) == 0) {
		return ot::getInvalidUID();
	}
	else {
		return m_prefetchedEntityVersions[_entityID];
	}
}

// ##########################################################################################################################################

// IO

void ot::ApplicationBase::enableMessageQueuing(const std::string & _service, bool _enableQueing) 
{
	auto serviceInfoByName= m_serviceNameMap.find(_service);
	if (serviceInfoByName == m_serviceNameMap.end()) {
		OTAssert(0, "Destination service not found");
		return;
	}
	
	ServiceBase* destinationServiceInfo = serviceInfoByName->second;
	if (_enableQueing)
	{
		MessageQueueHandler::instance().flushIfAppropriate(1,destinationServiceInfo);
	}
	else
	{
		MessageQueueHandler::instance().flushIfAppropriate(-1, destinationServiceInfo);
	}
}

void ot::ApplicationBase::flushQueuedHttpRequests(const std::string & _service)
{
	auto serviceInfoByName = m_serviceNameMap.find(_service);
	if (serviceInfoByName == m_serviceNameMap.end()) {
		OTAssert(0, "Destination service not found");
		return;
	}
	
	ServiceBase* destinationServiceInfo = serviceInfoByName->second;

	MessageQueueHandler::instance().flushServiceBuffer(destinationServiceInfo);
}

bool ot::ApplicationBase::sendMessage(bool _queue, const std::string& _serviceName, const JsonDocument& _doc, const ot::msg::RequestFlags& _requestFlags) {
	std::list<std::pair<UID, UID>> prefetchIds;
	std::string tmp;
	return this->sendMessage(_queue, _serviceName, _doc, prefetchIds, tmp, _requestFlags);
}

bool ot::ApplicationBase::sendMessage(bool _queue, const std::list<std::string>& _serviceNames, const JsonDocument& _doc, const ot::msg::RequestFlags& _requestFlags) {
	bool allOk = true;
	std::list<std::pair<UID, UID>> prefetchIds;
	for (const std::string& serviceName : _serviceNames) {
		std::string tmp;
		if (!this->sendMessage(_queue, serviceName, _doc, prefetchIds, tmp, _requestFlags)) {
			allOk = false;
		}
	}
	return allOk;
}

bool ot::ApplicationBase::sendMessage(bool _queue, const std::string & _serviceName, const JsonDocument& _doc, std::string& _response, const ot::msg::RequestFlags& _requestFlags)
{
	std::list<std::pair<UID, UID>> prefetchIds;
	return this->sendMessage(_queue, _serviceName, _doc, prefetchIds, _response, _requestFlags);
}

bool ot::ApplicationBase::sendMessage(bool _queue, const std::string & _serviceName, const JsonDocument& _doc, std::list<std::pair<UID, UID>> & _prefetchIds, std::string& _response, const ot::msg::RequestFlags& _requestFlags)
{
	if (_serviceName == this->getServiceName()) {
		OT_LOG_EA("Attempt to send message to self detected. Aborting message send.");
		return false;
	}

	auto serviceInfoByServiceName = m_serviceNameMap.find(_serviceName);
	if (serviceInfoByServiceName == m_serviceNameMap.end()) {
		OT_LOG_EAS("Could not find service by name: \"" + _serviceName + "\"");
		return false;
	}
	ServiceBase* destinationServiceInfo = serviceInfoByServiceName->second;
	
	const bool messageWasQueued = MessageQueueHandler::instance().addToQueueIfRequired(destinationServiceInfo, m_projectName, _doc, _prefetchIds);

	if (!messageWasQueued)
	{
		return ot::msg::send(this->getServiceURL(), destinationServiceInfo->getServiceURL(), (_queue ? QUEUE : EXECUTE), _doc.toJson(), _response, 0, _requestFlags);
	}
	else
	{
		return true;
	}
}

void ot::ApplicationBase::sendMessageAsync(bool _queue, const std::string& _serviceName, const JsonDocument& _doc, const ot::msg::RequestFlags& _requestFlags) {
	std::list<std::pair<UID, UID>> prefetchIds;
	return this->sendMessageAsync(_queue, _serviceName, _doc, prefetchIds, _requestFlags);
}

void ot::ApplicationBase::sendMessageAsync(bool _queue, const std::list<std::string>& _serviceNames, const JsonDocument& _doc, const ot::msg::RequestFlags& _requestFlags) {
	std::list<std::string> receiverUrls;
	for (const std::string& serviceName : _serviceNames) {
		if (serviceName != this->getServiceName()) {
			auto serviceInfoByServiceName = m_serviceNameMap.find(serviceName);
			if (serviceInfoByServiceName == m_serviceNameMap.end()) {
				OT_LOG_EAS("Could not find service by name: \"" + serviceName + "\"");
				continue;
			}
			ServiceBase* destinationServiceInfo = serviceInfoByServiceName->second;
			receiverUrls.push_back(destinationServiceInfo->getServiceURL());
		}
	}

	if (!receiverUrls.empty()) {
		ot::msg::sendAsync(this->getServiceURL(), receiverUrls, (_queue ? QUEUE : EXECUTE), std::move(_doc.toJson()), 0, _requestFlags);
	}
}

void ot::ApplicationBase::sendMessageAsync(bool _queue, const std::string& _serviceName, const JsonDocument& _doc, std::list<std::pair<UID, UID>>& _prefetchIds, const ot::msg::RequestFlags& _requestFlags) {
	if (_serviceName == this->getServiceName()) {
		OT_LOG_EA("Attempt to send message to self detected. Aborting async message send.");
		return;
	}

	auto serviceInfoByServiceName = m_serviceNameMap.find(_serviceName);
	if (serviceInfoByServiceName == m_serviceNameMap.end()) {
		OT_LOG_EAS("Could not find service by name: \"" + _serviceName + "\"");
		return;
	}

	ServiceBase* destinationServiceInfo = serviceInfoByServiceName->second;
	ot::msg::sendAsync(this->getServiceURL(), destinationServiceInfo->getServiceURL(), (_queue ? QUEUE : EXECUTE), std::move(_doc.toJson()), 0, _requestFlags);
}

void ot::ApplicationBase::addModalCommand(ot::ModalCommandBase *command)
{
	OTAssert(std::find(m_modalCommands.begin(), m_modalCommands.end(), command) == m_modalCommands.end(), "modal command is alreay in list"); // Ensure that the command is not yet in the list

	m_modalCommands.push_back(command);
}

void ot::ApplicationBase::removeModalCommand(ot::ModalCommandBase *command)
{
	OTAssert(std::find(m_modalCommands.begin(), m_modalCommands.end(), command) != m_modalCommands.end(), "modal command is not in list");  // Ensure that the command is in the list

	m_modalCommands.erase(std::find(m_modalCommands.begin(), m_modalCommands.end(), command));
}

std::string ot::ApplicationBase::processActionWithModalCommands(const std::string & _action, JsonDocument & _doc)
{
	/* if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
	{
		std::string action = json::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);

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
	else */ 
	if (_action == OT_ACTION_CMD_MODEL_SelectionChanged)
	{
		auto selectedEntityInfos =	json::getObjectList(_doc, OT_ACTION_PARAM_MODEL_EntityInfo);
		m_selectedEntityInfos.clear();
		m_selectedEntities.clear();
		for (auto& selectedEntityInfo : selectedEntityInfos)
		{
			ot::EntityInformation entityInfo;
			entityInfo.setFromJsonObject(selectedEntityInfo);
			m_selectedEntityInfos.push_back(entityInfo);
			m_selectedEntities.push_back(entityInfo.getEntityID());
		}

		for (auto command : m_modalCommands)
		{
			command->modelSelectionChanged(m_selectedEntities);
		}

		modelSelectionChanged();

		getUiComponent()->sendUpdatedControlState();

		return "";  // No further processing necessary
	}

	else if (_action == OT_ACTION_CMD_MODEL_PropertyChanged)
	{
		auto serialisedChangedEntityInfos = json::getObjectList(_doc, OT_ACTION_PARAM_MODEL_EntityInfo);
		std::list<ot::EntityInformation> changedEntityInfos;
		for (auto& serialisedChangedEntityInfo : serialisedChangedEntityInfos)
		{
			ot::EntityInformation info;
			info.setFromJsonObject(serialisedChangedEntityInfo);
			changedEntityInfos.push_back(info);
		}

		ot::UIDList changedEntityIDs = json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityIDList);
		for (ot::UID& changedEntityID : changedEntityIDs)
		{
			for (auto selectedEntityInfo = m_selectedEntityInfos.begin(); selectedEntityInfo != m_selectedEntityInfos.end();)
			{
				if (selectedEntityInfo->getEntityID() == changedEntityID)
				{
					selectedEntityInfo = m_selectedEntityInfos.erase(selectedEntityInfo);
				}
				else
				{
					++selectedEntityInfo;
				}
			}
		}
		m_selectedEntityInfos.insert(m_selectedEntityInfos.end(), changedEntityInfos.begin(), changedEntityInfos.end());
		propertyChanged(_doc);
		return "";
	}

	OT_LOG_D("Action \"" + _action + "\" does not have any handler nor is a modal command");

	// Otherwise process the local actions
	return processAction(_action, _doc);
}

// ##########################################################################################################################################

// Protected functions

void ot::ApplicationBase::initializeDefaultTemplate() {
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();
}

bool ot::ApplicationBase::storeSettingToDataBase(const PropertyGridCfg& _config, const std::string& _databaseURL, const std::string& _siteID, const std::string& _userName, const std::string& _userPassword, const std::string& _userCollection) {
	if (_databaseURL.empty()) {
		OT_LOG_EA("DataBase URL not set");
		return false;
	}
	if (_userName.empty()) {
		OT_LOG_EA("User name not set");
		return false;
	}
	if (_userPassword.empty()) {
		OT_LOG_EA("User password not set");
		return false;
	}
	if (_userCollection.empty()) {
		OT_LOG_EA("User collection not set");
		return false;
	}

	std::string settingsKey = this->getBasicServiceInformation().serviceName() + "Settings";

	try
	{
		// Ensure that we are connected to the database server
		DataStorageAPI::ConnectionAPI::establishConnection(_databaseURL, _userName, _userPassword);

		// First, open a connection to the user's settings collection
		DataStorageAPI::DocumentAccess docManager("UserSettings", _userCollection);

		// Now we search for the document with the given name
		auto queryDoc = bsoncxx::builder::basic::document{};
		queryDoc.append(bsoncxx::builder::basic::kvp("SettingName", settingsKey));

		auto filterDoc = bsoncxx::builder::basic::document{};

		auto result = docManager.GetDocument(std::move(queryDoc.extract()), std::move(filterDoc.extract()));

		JsonDocument propertyDoc;
		_config.addToJsonObject(propertyDoc, propertyDoc.GetAllocator());

		if (!result.getSuccess())
		{
			// The setting does not yet exist -> write a new one
			auto newDoc = bsoncxx::builder::basic::document{};
			newDoc.append(bsoncxx::builder::basic::kvp("SettingName", settingsKey));
			newDoc.append(bsoncxx::builder::basic::kvp("Data", propertyDoc.toJson()));

			docManager.InsertDocumentToDatabase(newDoc.extract(), false);
		}
		else
		{
			// The setting already exists -> replace the settings
			try
			{
				// Find the entry corresponding to the project in the collection
				auto doc_find = bsoncxx::builder::stream::document{} << "SettingName" << settingsKey << bsoncxx::builder::stream::finalize;

				auto doc_modify = bsoncxx::builder::stream::document{}
					<< "$set" << bsoncxx::builder::stream::open_document
					<< "Data" << propertyDoc.toJson()
					<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

				mongocxx::collection collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection("UserSettings", _userCollection);

				bsoncxx::stdx::optional<mongocxx::result::update> result = collection.update_many(doc_find.view(), doc_modify.view());
			}
			catch (std::exception _e)
			{
				OT_LOG_EAS(_e.what());
				return false;
			}
		}
	}
	catch (std::exception _e)
	{
		OT_LOG_EAS(_e.what());
		return false;
	}

	return true;  // Successfully stored the settings
}

ot::PropertyGridCfg ot::ApplicationBase::getSettingsFromDataBase(const std::string& _databaseURL, const std::string& _siteID, const std::string& _userName, const std::string& _userPassword, const std::string& _userCollection) {
	if (_databaseURL.empty()) {
		OT_LOG_EA("DataBase URL not set");
		return PropertyGridCfg();
	}
	if (_userName.empty()) {
		OT_LOG_EA("User name not set");
		return PropertyGridCfg();
	}
	if (_userPassword.empty()) {
		OT_LOG_EA("User password not set");
		return PropertyGridCfg();
	}
	if (_userCollection.empty()) {
		OT_LOG_EA("User collection not set");
		return PropertyGridCfg();
	}

	std::string settingsKey = this->getBasicServiceInformation().serviceName() + "Settings";

	try
	{
		// Ensure that we are connected to the database server
		DataStorageAPI::ConnectionAPI::establishConnection(_databaseURL, _userName, _userPassword);

		// First, open a connection to the user's settings collection
		DataStorageAPI::DocumentAccess docManager("UserSettings", _userCollection);

		// Now we search for the document with the given name
		auto queryDoc = bsoncxx::builder::basic::document{};
		queryDoc.append(bsoncxx::builder::basic::kvp("SettingName", settingsKey));

		auto filterDoc = bsoncxx::builder::basic::document{};

		auto result = docManager.GetDocument(std::move(queryDoc.extract()), std::move(filterDoc.extract()));

		if (!result.getSuccess())
		{
			return PropertyGridCfg();  // We could not find the document, but this is a standard case when the settings have not yet been stored
		}

		// Now we have found some settings, so retrieve the data
		std::string settingsJSON;
		try
		{
			bsoncxx::builder::basic::document doc;
			doc.append(bsoncxx::builder::basic::kvp("Found", result.getBsonResult().value()));

			auto doc_view = doc.view()["Found"].get_document().view();

			settingsJSON = doc_view["Data"].get_utf8().value.data();
		}
		catch (std::exception _e)
		{
			// Something went wrong with accessing the settings data
			OT_LOG_EAS(_e.what());
			return PropertyGridCfg();
		}

		// Create new settings from json string
		JsonDocument importedSettings;
		importedSettings.fromJson(settingsJSON);

		PropertyGridCfg newConfig;
		newConfig.setFromJsonObject(importedSettings.getConstObject());

		return newConfig;
	}
	catch (std::exception _e)
	{
		OT_LOG_EAS(_e.what());
		return PropertyGridCfg();
	}
}

// ##########################################################################################################################################

// Private: Action handler

std::string ot::ApplicationBase::handleKeySequenceActivated(JsonDocument& _document) {
	if (m_uiNotifier) {
		std::string keySequence = json::getString(_document, OT_ACTION_PARAM_UI_KeySequence);
		m_uiNotifier->shortcutActivated(keySequence);
		return OT_ACTION_RETURN_VALUE_OK;
	}
	else {
		OTAssert(0, "No UI connected");
		return OT_ACTION_RETURN_INDICATOR_Error "No UI connected";
	}
}

ot::ReturnMessage ot::ApplicationBase::handleSettingsItemChanged(JsonDocument& _document) {
	PropertyGridCfg gridConfig;
	gridConfig.setFromJsonObject(json::getObject(_document, OT_ACTION_PARAM_Config));

	std::list<Property*> properties = gridConfig.getAllProperties();
	if (properties.empty()) {
		return ot::ReturnMessage::Ok;
	}

	bool requireSettingsUpdate = false;
	for (const Property* prop : properties) {
		if (this->settingChanged(prop)) {
			requireSettingsUpdate = true;
		}
	}

	PropertyGridCfg newSettings = this->createSettings();
	if (newSettings.isEmpty()) {
		return ot::ReturnMessage::Ok;
	}

	if (!this->storeSettingToDataBase(newSettings, m_databaseURL, m_siteID, DataBase::instance().getUserName(), DataBase::instance().getUserPassword(), m_dbUserCollection)) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Failed to store settings");
	}

	if (requireSettingsUpdate) {
		m_uiComponent->sendSettingsData(newSettings);
	}

	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage ot::ApplicationBase::handleRegisterNewLMS(JsonDocument& _document) {
	m_lmsUrl = ot::json::getString(_document, OT_ACTION_PARAM_LIBRARYMANAGEMENT_SERVICE_URL);
	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage ot::ApplicationBase::handleGetDebugInformation(JsonDocument& _document) {
	JsonDocument debugInfo;
	JsonObject basicInfo;

	JsonObject serviceInfo;
	ServiceBase::addToJsonObject(serviceInfo, debugInfo.GetAllocator());
	basicInfo.AddMember("ServiceInformation", serviceInfo, debugInfo.GetAllocator());

	basicInfo.AddMember("UiMessageQueuingEnabled", m_uiMessageQueuingEnabled, debugInfo.GetAllocator());

	// Service Name Map
	JsonArray serviceNameArr;
	for (const auto& servicePair : m_serviceNameMap) {
		JsonObject obj;
		obj.AddMember("Key", JsonString(servicePair.first, debugInfo.GetAllocator()), debugInfo.GetAllocator());
		if (servicePair.second) {
			JsonObject serviceObj;
			servicePair.second->addToJsonObject(serviceObj, debugInfo.GetAllocator());
			obj.AddMember("Value", serviceObj, debugInfo.GetAllocator());
		}
		else {
			obj.AddMember("Value", JsonNullValue(), debugInfo.GetAllocator());
		}
	}
	basicInfo.AddMember("ServiceNameMap", serviceNameArr, debugInfo.GetAllocator());

	// Service ID Map
	JsonArray serviceIdArr;
	for (const auto& servicePair : m_serviceIdMap) {
		JsonObject obj;
		obj.AddMember("Key", servicePair.first, debugInfo.GetAllocator());
		if (servicePair.second) {
			JsonObject serviceObj;
			servicePair.second->addToJsonObject(serviceObj, debugInfo.GetAllocator());
			obj.AddMember("Value", serviceObj, debugInfo.GetAllocator());
		}
		else {
			obj.AddMember("Value", JsonNullValue(), debugInfo.GetAllocator());
		}
	}
	
	// LSS
	if (m_sessionService) {
		JsonObject sessionServiceObj;
		m_sessionService->addToJsonObject(sessionServiceObj, debugInfo.GetAllocator());
		basicInfo.AddMember("SessionService", sessionServiceObj, debugInfo.GetAllocator());
	}
	else {
		basicInfo.AddMember("SessionService", JsonNullValue(), debugInfo.GetAllocator());
	}

	// LDS
	if (m_directoryService) {
		JsonObject directoryServiceObj;
		m_directoryService->addToJsonObject(directoryServiceObj, debugInfo.GetAllocator());
		basicInfo.AddMember("DirectoryService", directoryServiceObj, debugInfo.GetAllocator());
	}
	else {
		basicInfo.AddMember("DirectoryService", JsonNullValue(), debugInfo.GetAllocator());
	}

	// Info
	basicInfo.AddMember("AuthorizationUrl", JsonString(m_authUrl, debugInfo.GetAllocator()), debugInfo.GetAllocator());
	basicInfo.AddMember("DatabaseUrl", JsonString(m_databaseURL, debugInfo.GetAllocator()), debugInfo.GetAllocator());
	basicInfo.AddMember("SiteID", JsonString(m_siteID, debugInfo.GetAllocator()), debugInfo.GetAllocator());
	basicInfo.AddMember("LMS.Url", JsonString(m_lmsUrl, debugInfo.GetAllocator()), debugInfo.GetAllocator());
	
	basicInfo.AddMember("SessionID", JsonString(m_sessionID, debugInfo.GetAllocator()), debugInfo.GetAllocator());
	basicInfo.AddMember("ProjectName", JsonString(m_projectName, debugInfo.GetAllocator()), debugInfo.GetAllocator());
	basicInfo.AddMember("CollectionName", JsonString(m_collectionName, debugInfo.GetAllocator()), debugInfo.GetAllocator());
	basicInfo.AddMember("ProjectType", JsonString(m_projectType, debugInfo.GetAllocator()), debugInfo.GetAllocator());

	basicInfo.AddMember("Login.UserName", JsonString(m_loggedInUserName, debugInfo.GetAllocator()), debugInfo.GetAllocator());
	basicInfo.AddMember("Login.Password", JsonString(m_loggedInUserPassword, debugInfo.GetAllocator()), debugInfo.GetAllocator());
	basicInfo.AddMember("DataBase.UserName", JsonString(m_dataBaseUserName, debugInfo.GetAllocator()), debugInfo.GetAllocator());
	basicInfo.AddMember("DataBase.Password", JsonString(m_dataBaseUserPassword, debugInfo.GetAllocator()), debugInfo.GetAllocator());

	basicInfo.AddMember("ModalCommands.Count", static_cast<uint64_t>(m_modalCommands.size()), debugInfo.GetAllocator());

	JsonArray selectedEntitiesArr;
	for (const auto& entityID : m_selectedEntities) {
		selectedEntitiesArr.PushBack(entityID, debugInfo.GetAllocator());
	}
	basicInfo.AddMember("SelectedEntities", selectedEntitiesArr, debugInfo.GetAllocator());

	JsonArray prefetchedEntitiesArr;
	for (const auto& entityPair : m_prefetchedEntityVersions) {
		JsonObject obj;
		obj.AddMember("EntityID", entityPair.first, debugInfo.GetAllocator());
		obj.AddMember("VersionID", entityPair.second, debugInfo.GetAllocator());
		prefetchedEntitiesArr.PushBack(obj, debugInfo.GetAllocator());
	}

	basicInfo.AddMember("UserCollection", JsonString(m_dbUserCollection, debugInfo.GetAllocator()), debugInfo.GetAllocator());
	debugInfo.AddMember("BasicInformation", basicInfo, debugInfo.GetAllocator());

	JsonObject customInfo;
	this->addDebugInformation(customInfo, debugInfo.GetAllocator());
	if (customInfo.MemberCount() > 0) {
		debugInfo.AddMember("CustomInformation", customInfo, debugInfo.GetAllocator());
	}
	
	return ot::ReturnMessage(ot::ReturnMessage::Ok, debugInfo);
}

// ##########################################################################################################################################

// Private: Internal functions

void ot::ApplicationBase::setSessionIDPrivate(const std::string& _id) {
	m_sessionID = _id;
	size_t index = m_sessionID.find(':');
	if (index == std::string::npos) {
		OT_LOG_EAS("Invalid session id format: \"" + _id + "\"");
	}
	else {
		m_projectName = m_sessionID.substr(0, index);
		m_collectionName = m_sessionID.substr(index + 1);
		LogDispatcher::instance().setProjectName(m_projectName);
		DataBase::instance().setCollectionName(m_collectionName);
	}
}

bool ot::ApplicationBase::initializeDataBaseConnectionPrivate() {
	return DataBase::instance().initializeConnection(m_databaseURL);;
}

void ot::ApplicationBase::serviceConnectedPrivate(const ot::ServiceBase& _service) {
	// Prepare information to store data

	// Check for duplicate
	if (m_serviceIdMap.find(_service.getServiceID()) != m_serviceIdMap.end()) {
		OT_LOG_EAS("Service already registered { \"Name\": \"" + _service.getServiceName() + "\", \"Type\": \"" + _service.getServiceType() + "\", \"URL\": \"" + _service.getServiceURL() + "\", \"ID\": " + std::to_string(_service.getServiceID()) + " }");
		return;
	}
	if (_service.getServiceType() == OT_INFO_SERVICE_TYPE_UI) {
		if (m_uiComponent) {
			OT_LOG_EA("UI component already registered. Multiple UIs not supported");
			return;
		}
		// Store information
		m_uiComponent = new components::UiComponent(_service, this);

		GuiAPIManager::instance().frontendConnected(*m_uiComponent);

		m_serviceIdMap.insert_or_assign(_service.getServiceID(), m_uiComponent);
		m_serviceNameMap.insert_or_assign(_service.getServiceName(), m_uiComponent);

		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults("UI Configuration");

		this->enableMessageQueuing(m_uiComponent->getServiceName(), true);

		m_uiComponent->sendSettingsData(this->createSettings());

		this->uiConnected(m_uiComponent);
		m_uiComponent->sendUpdatedControlState();
		m_uiComponent->notifyUiSetupCompleted();
		this->enableMessageQueuing(m_uiComponent->getServiceName(), false);
		if (m_initialSelectionHelper) {
			m_initialSelectionHelper->setUiUrl(_service.getServiceURL());
			m_initialSelectionHelper->runIfReady();
		}
	}
	else if (_service.getServiceType() == OT_INFO_SERVICE_TYPE_MODEL) {
		// Store information
		assert(m_modelComponent == nullptr);
		ot::ModelAPIManager::setModelServiceURL(_service.getServiceURL());
		m_modelComponent = new components::ModelComponent(_service, this);

		m_serviceIdMap.insert_or_assign(_service.getServiceID(), m_modelComponent);
		m_serviceNameMap.insert_or_assign(_service.getServiceName(), m_modelComponent);

		modelConnected(m_modelComponent);

		if (m_initialSelectionHelper) {
			m_initialSelectionHelper->setModelUrl(_service.getServiceURL());
			m_initialSelectionHelper->runIfReady();
		}
	}
	else {
		// Store information
		ot::ServiceBase* newService = new ServiceBase(_service);
		m_serviceIdMap.insert_or_assign(_service.getServiceID(), newService);
		m_serviceNameMap.insert_or_assign(_service.getServiceName(), newService);

		serviceConnected(*newService);
	}
}

void ot::ApplicationBase::serviceDisconnectedPrivate(serviceID_t _id) {
	auto serviceByID = m_serviceIdMap.find(_id);
	assert(serviceByID != m_serviceIdMap.end());
	auto& serviceInfo = serviceByID->second;
	std::string serviceName = serviceInfo->getServiceName();

	if (serviceInfo->getServiceType() == OT_INFO_SERVICE_TYPE_UI) {

		assert(serviceInfo == (ServiceBase*)m_uiComponent);

		uiDisconnected(m_uiComponent);
		MessageQueueHandler::instance().clearServiceBuffer(serviceInfo);

		if (m_uiComponent != nullptr) {
			delete m_uiComponent;
			m_uiComponent = nullptr;
		}

		GuiAPIManager::instance().frontendDisconnected();

		if (m_initialSelectionHelper) {
			m_initialSelectionHelper->setUiUrl(std::string());
		}
	}
	else if (serviceInfo->getServiceType() == OT_INFO_SERVICE_TYPE_MODEL) {
		assert(serviceInfo == (ServiceBase*)m_modelComponent);
		ot::ModelAPIManager::setModelServiceURL(std::string());
		modelDisconnected(m_modelComponent);
		MessageQueueHandler::instance().clearServiceBuffer(serviceInfo);

		if (m_modelComponent != nullptr) {
			delete m_modelComponent;
			m_modelComponent = nullptr;
		}

		if (m_initialSelectionHelper) {
			m_initialSelectionHelper->setModelUrl(std::string());
		}
	}
	else {
		serviceDisconnected(*serviceInfo);
		MessageQueueHandler::instance().clearServiceBuffer(serviceInfo);
		if (serviceInfo != nullptr) {
			delete serviceInfo;
			serviceInfo = nullptr;
		}
	}

	m_serviceIdMap.erase(_id);
	m_serviceNameMap.erase(serviceName);
}

void ot::ApplicationBase::shuttingDownPrivate(bool _requestedAsCommand) {
	if (!_requestedAsCommand) preShutdown();
	else shuttingDown();
}
