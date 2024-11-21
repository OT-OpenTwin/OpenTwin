//! @file ApplicationBase.cpp
//!
//! @author Alexander Kuester (alexk95)
//! @date February 2021
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/OTAssert.h"
#include "OTCore/Logger.h"
#include "OTCore/ThisService.h"

#include "OTGui/Property.h"
#include "OTGui/PropertyGroup.h"
#include "OTGuiAPI/GuiAPIManager.h"

#include "OTCommunication/ActionTypes.h"		// action member and types definition
#include "OTCommunication/Msg.h"				// message sending
#include "OTCommunication/IpConverter.h"

#include "OTServiceFoundation/ApplicationBase.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/UiPluginComponent.h"
#include "OTServiceFoundation/AbstractUiNotifier.h"
#include "OTServiceFoundation/AbstractModelNotifier.h"
#include "OTServiceFoundation/ModalCommandBase.h"
#include "OTServiceFoundation/ErrorWarningLogFrontendNotifier.h"

#include "DataBase.h"
#include "Document\DocumentAccess.h"
#include "Connection\ConnectionAPI.h"

#include "TemplateDefaultManager.h"

#include "OTServiceFoundation/ExternalServicesComponent.h"
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

	new ErrorWarningLogFrontendNotifier(this); // Log Dispatcher gets the ownership of the notifier.
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

void ot::ApplicationBase::setSessionID(const std::string& _id)
{
	m_sessionID = _id;
	size_t index = m_sessionID.find(':');
	if (index == std::string::npos) {
		OT_LOG_EAS("Invalid session id format: \"" + _id + "\"");
	}
	else {
		m_projectName = m_sessionID.substr(0, index);
		m_collectionName = m_sessionID.substr(index + 1);
		LogDispatcher::instance().setProjectName(m_projectName);
	}	
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
	OTAssert(0, "Unknown plugin UID");
	return nullptr;
}

ot::components::UiPluginComponent * ot::ApplicationBase::getUiPluginByName(const std::string& _pluginName) {
	for (auto p : m_uiPluginComponents) {
		if (p.second->pluginName() == _pluginName) return p.second;
	}
	OTAssert(0, "Unknown plugin Name");
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
		OTAssert(0, "Destination service not found");
		return;
	}
	destination->second.enabledCounter += (_flag ? 1 : -1);

	if (destination->second.enabledCounter < 0) {
		OTAssert(0, "Enabled counter reached a negative value");
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
		OTAssert(0, "Destination service not found");
		return;
	}

	if (destination->second.doc != nullptr)
	{
		JsonDocument* queuedDoc = destination->second.doc;

		std::string response;
		if (queuedDoc->IsObject()) {
			ot::msg::send(this->getServiceURL(), destination->second.service->getServiceURL(), ot::QUEUE, queuedDoc->toJson(), response);

			delete queuedDoc;
			destination->second.doc = nullptr;
		}
		else {
			OTAssert(0, "The queued doc is not a object");
		}
	}
}

bool ot::ApplicationBase::sendMessage(bool _queue, const std::string & _serviceName, const JsonDocument& _doc, std::string& _response)
{
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIds;
	return this->sendMessage(_queue, _serviceName, _doc, prefetchIds, _response);
}

bool ot::ApplicationBase::sendMessage(bool _queue, const std::string & _serviceName, const JsonDocument& _doc, std::list<std::pair<UID, UID>> & _prefetchIds, std::string& _response)
{
	auto destination = m_serviceNameMap.find(_serviceName);
	if (destination == m_serviceNameMap.end()) {
		OTAssert(0, "Destination service not found");
		return "";
	}

	if (destination->second.enabledCounter > 0 && _queue)
	{
		JsonDocument *queuedDoc = nullptr;
		
		if (destination->second.doc == nullptr)
		{
			queuedDoc = new JsonDocument;

			JsonArray docs;
			JsonArray prefetchedIDs;
			JsonArray prefetchedVersions;

			queuedDoc->AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_Compound, queuedDoc->GetAllocator());
			queuedDoc->AddMember(OT_ACTION_PARAM_PROJECT_NAME, JsonString(m_projectName, queuedDoc->GetAllocator()), queuedDoc->GetAllocator());
			queuedDoc->AddMember(OT_ACTION_PARAM_PREFETCH_Documents, docs, queuedDoc->GetAllocator());
			queuedDoc->AddMember(OT_ACTION_PARAM_PREFETCH_ID, prefetchedIDs, queuedDoc->GetAllocator());
			queuedDoc->AddMember(OT_ACTION_PARAM_PREFETCH_Version, prefetchedVersions, queuedDoc->GetAllocator());

			destination->second.doc = queuedDoc;
		}
		else
		{
			queuedDoc = destination->second.doc;
		}

		JsonAllocator& allocator = queuedDoc->GetAllocator();

		ot::JsonValue v(_doc, allocator);
		(*queuedDoc)[OT_ACTION_PARAM_PREFETCH_Documents].PushBack(v, allocator);

		for (auto id : _prefetchIds)
		{
			(*queuedDoc)[OT_ACTION_PARAM_PREFETCH_ID].PushBack(id.first, allocator);
			(*queuedDoc)[OT_ACTION_PARAM_PREFETCH_Version].PushBack(id.second, allocator);
		}

		return true;
	}
	else
	{
		return ot::msg::send(m_serviceURL, destination->second.service->getServiceURL(), (_queue ? QUEUE : EXECUTE), _doc.toJson(), _response);
	}
}

bool ot::ApplicationBase::broadcastMessage(bool _queue, const std::string& _message) {
	JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_SendBroadcastMessage, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_serviceID, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, JsonString(m_sessionID, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESSAGE, JsonString(_message, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	return ot::msg::send(m_serviceURL, m_sessionService.service->getServiceURL(), (_queue ? QUEUE : EXECUTE), doc.toJson(), response);
}

bool ot::ApplicationBase::broadcastMessage(bool _queue, const JsonDocument& _doc) {
	return this->broadcastMessage(_queue, _doc.toJson());
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
	if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
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
	else if (_action == OT_ACTION_CMD_MODEL_SelectionChanged)
	{
		m_selectedEntities = json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_SelectedEntityIDs);

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

bool ot::ApplicationBase::EnsureDataBaseConnection(void)
{
	DataBase::GetDataBase()->setProjectName(m_collectionName);

	return DataBase::GetDataBase()->InitializeConnection(m_databaseURL, m_siteID);;
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
		DataStorageAPI::ConnectionAPI::establishConnection(_databaseURL, _siteID, _userName, _userPassword);

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
		DataStorageAPI::ConnectionAPI::establishConnection(_databaseURL, _siteID, _userName, _userPassword);

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
		newConfig.setFromJsonObject(importedSettings.GetConstObject());

		return newConfig;
	}
	catch (std::exception _e)
	{
		OT_LOG_EAS(_e.what());
		return PropertyGridCfg();
	}
}

// ##########################################################################################################################################

// Private functions

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

std::string ot::ApplicationBase::handleSettingsItemChanged(JsonDocument& _document) {
	PropertyGridCfg gridConfig;
	gridConfig.setFromJsonObject(json::getObject(_document, OT_ACTION_PARAM_Config));

	std::list<Property*> properties = gridConfig.getAllProperties();
	if (properties.empty()) return std::string();

	bool requireSettingsUpdate = false;
	for (const Property* prop : properties) {
		if (this->settingChanged(prop)) {
			requireSettingsUpdate = true;
		}
	}

	PropertyGridCfg newSettings = this->createSettings();
	if (newSettings.isEmpty()) return "";

	if (!this->storeSettingToDataBase(newSettings, m_databaseURL, m_siteID, DataBase::GetDataBase()->getUserName(), DataBase::GetDataBase()->getUserPassword(), m_DBuserCollection)) {
		return OT_ACTION_RETURN_INDICATOR_Error "Failed to store settings";
	}

	if (requireSettingsUpdate) {
		m_uiComponent->sendSettingsData(newSettings);
	}

	return "";
}

std::string ot::ApplicationBase::handleContextMenuItemClicked(JsonDocument& _document) {
	if (m_uiNotifier) {
		std::string menuName = json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ContextMenuName);
		std::string itemName = json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ContextMenuItemName);
		m_uiNotifier->contextMenuItemClicked(menuName, itemName);
		return OT_ACTION_RETURN_VALUE_OK;
	}
	else {
		OTAssert(0, "No UI connected");
		return OT_ACTION_RETURN_INDICATOR_Error "No UI connected";
	}
}

std::string ot::ApplicationBase::handleContextMenuItemCheckedChanged(JsonDocument& _document) {
	if (m_uiNotifier) {
		std::string menuName = json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ContextMenuName);
		std::string itemName = json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ContextMenuItemName);
		bool isChecked = json::getBool(_document, OT_ACTION_PARAM_UI_CONTROL_CheckedState);
		m_uiNotifier->contextMenuItemCheckedChanged(menuName, itemName, isChecked);
		return OT_ACTION_RETURN_VALUE_OK;
	}
	else {
		OTAssert(0, "No UI connected");
		return OT_ACTION_RETURN_INDICATOR_Error "No UI connected";
	}
}

void ot::ApplicationBase::__serviceConnected(const std::string & _name, const std::string & _type, const std::string & _url, serviceID_t _id) {
	// Prepare information to store data
	structServiceInformation info;
	info.doc = nullptr; info.enabledCounter = 0;

	// Check for duplicate
	if (m_serviceIdMap.find(_id) != m_serviceIdMap.end()) {
		OT_LOG_EAS("Service already registered { \"Name\": \"" + _name + "\", \"Type\": \"" + _type + "\", \"URL\": \"" + _url + "\", \"ID\": " + std::to_string(_id) + " }");
		return;
	}

	if (_type == OT_INFO_SERVICE_TYPE_UI) {
		if (m_uiComponent) {
			OT_LOG_EA("UI component already registered. Multiple UIs not supported");
			return;
		}
		// Store information
		m_uiComponent = new components::UiComponent(_name, _type, _url, _id, this);
		info.service = m_uiComponent;
		m_serviceIdMap.insert_or_assign(_id, info);
		m_serviceNameMap.insert_or_assign(_name, info);
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults("UI Configuration");

		this->enableMessageQueuing(m_uiComponent->getServiceName(), true);

		m_uiComponent->sendSettingsData(this->createSettings());
		
		this->uiConnected(m_uiComponent);
		m_uiComponent->sendUpdatedControlState();
		m_uiComponent->notifyUiSetupCompleted();
		this->enableMessageQueuing(m_uiComponent->getServiceName(), false);

		GuiAPIManager::instance().frontendConnected(*m_uiComponent);
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
		GuiAPIManager::instance().frontendDisconnected();
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

std::string ot::ApplicationBase::__processMessage(const std::string & _message, JsonDocument & _doc, serviceID_t _senderID) {
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
		m_prefetchedEntityVersions[entity.getEntityID()] = entity.getEntityVersion();

		prefetchIdandVersion.push_back(std::pair<UID, UID>(entity.getEntityID(), entity.getEntityVersion()));
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIdandVersion);
}

ot::UID ot::ApplicationBase::getPrefetchedEntityVersion(UID entityID)
{
	OTAssert(m_prefetchedEntityVersions.count(entityID) > 0, "The entity was not prefetched");

	return m_prefetchedEntityVersions[entityID];
}

std::string ot::ApplicationBase::getLogInUserName() const
{
	return ot::intern::ExternalServicesComponent::instance().getLoggedInUserName();
}

std::string ot::ApplicationBase::getLogInUserPsw() const
{
	return ot::intern::ExternalServicesComponent::instance().getLoggedInUserPassword(); 
}

