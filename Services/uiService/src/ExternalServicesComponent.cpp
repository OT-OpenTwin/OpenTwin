/*
 * ExternalServicesComponent.cpp
 *
 *  Created on: September 21, 2020
 *	Author: Alexander Kuester, Peter Thoma
 *  Copyright (c) 2020 openTwin
 */

// wrapper header
#include "ExternalServicesComponent.h"	// Corresponding header
#include "ViewerComponent.h"			// Viewer component
#include "ControlsManager.h"
#include "AppBase.h"
#include "ToolBar.h"
#include "ShortcutManager.h"
#include "UserSettings.h"
#include "ContextMenuManager.h"
#include "UiPluginManager.h"
#include "SelectEntitiesDialog.h"

// Qt header
#include <QtCore/qdir.h>						// QDir
#include <QtCore/qeventloop.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qmessagebox.h>

// OpenTwin header
#include "OTCore/ServiceBase.h"
#include "OTCore/OTAssert.h"
#include "OTCore/Logger.h"
#include "OTCore/Color.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTCore/OwnerManagerTemplate.h"
#include "OTCore/OwnerService.h"
#include "OTCore/OwnerServiceGlobal.h"
#include "OTCore/SimpleFactory.h"
#include "OTCore/GenericDataStructMatrix.h"

#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/IpConverter.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/UiTypes.h"

#include "OTServiceFoundation/SettingsData.h"
#include "OTServiceFoundation/TableRange.h"
#include "OTServiceFoundation/ContextMenu.h"

#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/MessageDialogCfg.h"
#include "OTGui/PropertyDialogCfg.h"
#include "OTGui/OnePropertyDialogCfg.h"
#include "OTGui/SelectEntitiesDialogCfg.h"
#include "OTGui/GraphicsLayoutItemCfg.h"

#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsLayoutItem.h"
#include "OTWidgets/GraphicsFactory.h"
#include "OTWidgets/GraphicsPicker.h"
#include "OTWidgets/GraphicsViewView.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/TextEditorView.h"
#include "OTWidgets/MessageDialog.h"
#include "OTWidgets/PropertyDialog.h"
#include "OTWidgets/OnePropertyDialog.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/PropertyInput.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"
#include "OTWidgets/IconManager.h"

#include "StudioSuiteConnector/StudioSuiteConnectorAPI.h"

// Curl
#include "curl/curl.h"					// Curl

// AK header
#include <akAPI/uiAPI.h>
#include <akGui/aColor.h>
#include <akCore/akCore.h>

#include "base64.h"
#include "zlib.h"

#include "WebsocketClient.h"

#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

const QString c_buildInfo = "Open Twin - Build " + QString(__DATE__) + " - " + QString(__TIME__) + "\n\n";

static std::thread * g_sessionServiceHealthCheckThread{ nullptr };
static bool g_runSessionServiceHealthCheck{ false };

#undef GetObject

extern "C"
{
	_declspec(dllexport) const char *performAction(const char *json, const char *senderIP)
	{
		char *retval = nullptr;
		try {
			OT_LOG("Perform action: " + std::string(json), ot::INBOUND_MESSAGE_LOG);

			QMetaObject::invokeMethod(AppBase::instance()->getExternalServicesComponent(), "performAction", /*Qt::BlockingQueuedConnection*/
				Qt::DirectConnection, Q_RETURN_ARG(char *, retval), Q_ARG(const char*, json), Q_ARG(const char*, senderIP));
		}
		catch (const std::exception & e) {
			int x = 1;
			assert(0); // Error
		}
		catch (...) {
			int x = 1;
			assert(0); // Error
		}
		return retval;
	};

	_declspec(dllexport) const char *queueAction(const char *json, const char *senderIP)
	{
		char *retval = nullptr;
		try {

			char *dataCopy = new char[strlen(json) + 1];
			strcpy(dataCopy, json);

			char *senderIPCopy = new char[strlen(senderIP) + 1];
			strcpy(senderIPCopy, senderIP);

			OT_LOG("Queue action: " + std::string(json), ot::QUEUED_INBOUND_MESSAGE_LOG);

			QMetaObject::invokeMethod(AppBase::instance()->getExternalServicesComponent(), "queueAction", Qt::QueuedConnection, Q_ARG(const char*, dataCopy), Q_ARG(const char*, senderIPCopy));
		}
		catch (const std::exception & e) {
			int x = 1;
			assert(0); // Error
		}
		catch (...) {
			int x = 1;
			assert(0); // Error
		}
		return retval;
	};

	_declspec(dllexport) void deallocateData(const char *data)
	{
		try {
			// std::cout << "deallocateData: ";
			if (data != nullptr)
			{
				QMetaObject::invokeMethod(AppBase::instance()->getExternalServicesComponent(), "deallocateData", Qt::QueuedConnection, Q_ARG(const char*, data));
			}
		}
		catch (const std::exception & e) {
			assert(0); // Error
		}
		catch (...) {
			assert(0); // Error
		}
	};
}

#ifdef _DEBUG
void _outputDebugMessage(const std::string & _msg) {
	OutputDebugStringA("[OPEN TWIN] [DEBUG] ");
	OutputDebugStringA(_msg.c_str());
	OutputDebugStringA("\n");
	std::cout << _msg << std::endl;
}
#endif // _DEBUG

// ###################################################################################################

ExternalServicesComponent::ExternalServicesComponent(AppBase * _owner) :
	m_websocket{ nullptr },
	m_isRelayServiceRequired{ false },
	m_controlsManager{ nullptr },
	m_lockManager{ nullptr },
	m_owner{ _owner },
	m_prefetchingDataCompleted{ false }
{
	m_controlsManager = new ControlsManager;
	m_lockManager = new LockManager(m_owner);
}

ExternalServicesComponent::~ExternalServicesComponent(void)
{
	if (m_websocket != nullptr) delete m_websocket;
	m_websocket = nullptr;
	if (m_controlsManager != nullptr) { delete m_controlsManager; }
	m_controlsManager = nullptr;
	if (m_lockManager != nullptr) { delete m_lockManager; }
	m_lockManager = nullptr;
}

void ExternalServicesComponent::shutdown(void) {
	if (m_currentSessionID.length() == 0) { return; }
	ot::JsonDocument commandDoc;
	commandDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSession, commandDoc.GetAllocator()), commandDoc.GetAllocator());
	commandDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, AppBase::instance()->serviceID(), commandDoc.GetAllocator());
	commandDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, commandDoc.GetAllocator()), commandDoc.GetAllocator());
	std::string response;
	sendHttpRequest(EXECUTE, m_sessionServiceURL, commandDoc, response);

	//NOTE, at this point we have to decide how to handle the shutdown of the uiService
}

// ###################################################################################################

// Configuration

void ExternalServicesComponent::setRelayServiceIsRequired(void) {
	m_isRelayServiceRequired = true;
	std::cout << "The relay service parameter was set: True" << std::endl;
}

void ExternalServicesComponent::setMessagingRelay(const std::string &relayAddress)
{
	if (!relayAddress.empty())
	{
		size_t index1 = relayAddress.find(':');
		size_t index2 = relayAddress.find(':', index1 + 1);

		std::string url = relayAddress.substr(0, index1);
		std::string httpPort = relayAddress.substr(index1 + 1, index2 - index1 - 1);
		std::string wsPort = relayAddress.substr(index2 + 1);

		m_uiRelayServiceHTTP = url + ":" + httpPort;
		m_uiRelayServiceWS = url + ":" + wsPort;
	}
}

// ###################################################################################################

// UI Element creation

KeyboardCommandHandler* ExternalServicesComponent::addShortcut(ot::ServiceBase* _sender, const std::string& _keySequence) {
	if (_keySequence.length() > 0) {
		ShortcutManager* manager = AppBase::instance()->shortcutManager();
		if (manager) {
			KeyboardCommandHandler* oldHandler = manager->handlerFromKeySequence(_keySequence.c_str());
			if (oldHandler) {
				OT_LOG_WAS("Shortcut for key sequence \"" + _keySequence + "\" already occupied by service \"" + oldHandler->creator()->serviceName() +
					" (ID: " + std::to_string(oldHandler->creator()->serviceID()) + ")\"\n");
				return nullptr;
			}
			KeyboardCommandHandler* newHandler = new KeyboardCommandHandler(_sender, AppBase::instance(), _keySequence.c_str());
			manager->addHandler(newHandler);
			return newHandler;
		}
		else {
			assert(0);
			return nullptr;
		}
	}
	else {
		OT_LOG_EA("No key sequence provided for shortcut");
		return nullptr;
	}
}

// ###################################################################################################

// 3D View

std::list<std::string> ExternalServicesComponent::GetAllUserProjects()
{
	std::string authorizationURL = AppBase::instance()->getAuthorizationServiceURL();
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_ALL_USER_PROJECTS, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(AppBase::instance()->getCredentialUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(AppBase::instance()->getCredentialUserPasswordClear(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_FILTER, ot::JsonString("", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_LIMIT, 0, doc.GetAllocator());
	std::string response;
	if (!ot::msg::send("", authorizationURL, ot::EXECUTE, doc.toJson(), response))
	{
		throw std::exception("Could not get the projectlist of the authorization service.");
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	const rapidjson::Value& projectArray = responseDoc["projects"];
	assert(projectArray.IsArray());
	std::list<std::string> projectList;
	for (auto& element : projectArray.GetArray())
	{
		std::string projectDescription = element.GetString();
		ot::JsonDocument projectDescriptionDoc;
		projectDescriptionDoc.fromJson(projectDescription);
		projectList.push_back(projectDescriptionDoc[OT_PARAM_AUTH_NAME].GetString());
	}
	return projectList;
}

void ExternalServicesComponent::prefetchDocumentsFromStorage(const std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs)
{
	AppBase::instance()->getViewerComponent()->prefetchDocumentsFromStorage(projectName, prefetchIDs);
}

ModelUIDtype ExternalServicesComponent::createModel(
	const std::string &						_projectName,
	const std::string &						_collectionName
) {
	assert(0); //NOTE, Not in use anymore
	return 0;
}

bool ExternalServicesComponent::deleteModel(ModelUIDtype modelID)
{
	try {
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_Delete, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());
		std::string response;
		try {
			for (auto reciever : m_modelViewNotifier) {
				sendHttpRequest(EXECUTE, reciever->serviceURL(), inDoc, response);
				// Check if response is an error or warning
				OT_ACTION_IF_RESPONSE_ERROR(response) {
					assert(0); // ERROR
				}
				else OT_ACTION_IF_RESPONSE_WARNING(response) {
					assert(0); // WARNING
				}
			}
			return true;
		}
		catch (std::out_of_range)
		{ return false; }
	}
	catch (...) {
		assert(0); // Error handling
		return false;
	}
}

void ExternalServicesComponent::setVisualizationModel(ModelUIDtype modelID, ModelUIDtype visualizationModelID)
{
	assert(0); // Not in use anymore
	try {
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_SetVisualizationModel, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_VIEW_ID, rapidjson::Value(visualizationModelID), inDoc.GetAllocator());

		std::string response;
		for (auto reciever : m_modelViewNotifier) {
			sendHttpRequest(EXECUTE, reciever->serviceURL(), inDoc, response);
			// Check if response is an error or warning
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				assert(0); // ERROR
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
			}
		}

	}
	catch (...) {
		assert(0); // Error handling
	}
}

ModelUIDtype ExternalServicesComponent::getVisualizationModel(ModelUIDtype modelID)
{
	try {
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetVisualizationModel, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());

		std::string response;

		for (auto reciever : m_modelViewNotifier) {
			sendHttpRequest(EXECUTE, reciever->serviceURL(), inDoc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				assert(0); // ERROR
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
			}
		}
		//NOTE, WARNING, at this point only the last response will be taken into accout..

		ot::JsonDocument outDoc;
		outDoc.fromJson(response);
		ak::UID uid = outDoc[OT_ACTION_PARAM_BASETYPE_UID].GetInt64();
		return uid;
	}
	catch (...) {
		assert(0); // Error handling
		return 0;
	}
}

bool ExternalServicesComponent::isModelModified(ModelUIDtype modelID)
{
	try {
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_GetIsModified, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());
		std::string response;

		for (auto reciever : m_modelViewNotifier) {
			sendHttpRequest(EXECUTE, reciever->serviceURL(), inDoc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				assert(0); // ERROR
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
			}
		}
		//NOTE, WARNING, at this point only the last response will be taken into accout..

		ot::JsonDocument outDoc;
		outDoc.fromJson(response);
		bool modified = outDoc[OT_ACTION_PARAM_BASETYPE_Bool].GetBool();
		return modified;
	}
	catch (...) {
		assert(0); // Error handling
		return false;
	}
}

bool ExternalServicesComponent::isCurrentModelModified(void)
{
	try {
		// Get the id of the curently active model
		ModelUIDtype modelID = AppBase::instance()->getViewerComponent()->getActiveDataModel();
		if (modelID == 0) return false;  // No project currently active

		return isModelModified(modelID);
	}
	catch (...) {
		assert(0); // Error handling
	}

	return false;
}

// ###################################################################################################

// Event handling

void ExternalServicesComponent::notify(ak::UID _senderId, ak::eventType _event, int _info1, int _info2) {
	try {

		if (_event == ak::etClicked || _event == ak::etEditingFinished)
		{
			auto receiver = m_controlsManager->objectCreator(_senderId);
			if (receiver != nullptr) {
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteAction, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(ak::uiAPI::object::getObjectUniqueName(_senderId).toStdString(), doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_ID, AppBase::instance()->getViewerComponent()->getActiveDataModel(), doc.GetAllocator());
				std::string response;

				if (_event == ak::etEditingFinished)
				{
					std::string editText = ak::uiAPI::niceLineEdit::text(_senderId).toStdString();
					doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectText, ot::JsonString(editText, doc.GetAllocator()), doc.GetAllocator());
				}

				if (!sendHttpRequest(EXECUTE, receiver->serviceURL(), doc, response)) {
					assert(0); // Failed to send HTTP request
				}
				// Check if response is an error or warning
				OT_ACTION_IF_RESPONSE_ERROR(response) {
					assert(0); // ERROR
					AppBase::instance()->showErrorPrompt(response.c_str(), "Error");
				}
				else OT_ACTION_IF_RESPONSE_WARNING(response) {
					assert(0); // WARNING
					AppBase::instance()->showWarningPrompt(response.c_str(), "Warning");
				}
			}
			else { executeAction(AppBase::instance()->getViewerComponent()->getActiveDataModel(), _senderId); }
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}
	catch (...) {
		OT_LOG_EA("Unknown error");
	}
}

void ExternalServicesComponent::fillPropertyGrid(const std::string& _settings) {

}

void ExternalServicesComponent::modelSelectionChangedNotification(ModelUIDtype modelID, std::list<ModelUIDtype> &selectedEntityID, std::list<ModelUIDtype> &selectedVisibleEntityID)
{
	try {
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_SelectionChanged, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_SelectedEntityIDs, ot::JsonArray(selectedEntityID, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_SelectedVisibleEntityIDs, ot::JsonArray(selectedVisibleEntityID, inDoc.GetAllocator()), inDoc.GetAllocator());

		std::string response;
		for (auto reciever : m_modelViewNotifier) {
			sendHttpRequest(EXECUTE, reciever->serviceURL(), inDoc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				assert(0); // ERROR
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
			}
		}
	}
	catch (const std::exception & e) {
		assert(0); // Error handling
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::itemRenamed(ModelUIDtype modelID, const std::string &newName)
{
	try {
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ItemRenamed, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Name, rapidjson::Value(newName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());

		std::string response;
		for (auto reciever : m_modelViewNotifier) {
			sendHttpRequest(EXECUTE, reciever->serviceURL(), inDoc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				assert(0); // ERROR
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
			}
		}
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::propertyGridValueChanged(const std::string& _groupName, const std::string& _itemName)
{
	AppBase::instance()->lockPropertyGrid(true);

	try {
		ot::PropertyGridItem* itm = AppBase::instance()->findProperty(_groupName, _itemName);
		if (!itm) {
			OT_LOG_E("Property not found");
			return;
		}
		
		ot::PropertyGridCfg cfg;
		ot::PropertyGroup* cfgGroup = new ot::PropertyGroup(_groupName);
		cfgGroup->addProperty(itm->createProperty());
		cfg.addRootGroup(cfgGroup);

		// Get the currently selected model entities. We first get all visible entities only.
		std::list<ak::UID> selectedModelEntityIDs;
		getSelectedVisibleModelEntityIDs(selectedModelEntityIDs);
		bool itemsVisible = true;

		// If we do not have visible entities, then we also look for the hidden ones.
		if (selectedModelEntityIDs.empty())
		{
			getSelectedModelEntityIDs(selectedModelEntityIDs);
			itemsVisible = false;
		}

		// Finally send the string
		ak::UID modelID = AppBase::instance()->getViewerComponent()->getActiveDataModel();
		
		ot::JsonDocument doc;
		ot::JsonObject cfgObj;
		cfg.addToJsonObject(cfgObj, doc.GetAllocator());

		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_SetPropertiesFromJSON, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ID, modelID, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(selectedModelEntityIDs, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_Update, false, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ItemsVisible, itemsVisible, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_Config, cfgObj, doc.GetAllocator());
		
		std::string response;

		for (auto reciever : m_modelViewNotifier) {
			sendHttpRequest(EXECUTE, reciever->serviceURL(), doc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				assert(0); // ERROR
			}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
		}
		}

	}
	catch (...) {
		assert(0); // Error handling
	}

	AppBase::instance()->lockPropertyGrid(false);
}

void ExternalServicesComponent::propertyGridValueDeleted(const std::string& _groupName, const std::string& _itemName)
{
	AppBase::instance()->lockPropertyGrid(true);

	// Get the currently selected model entities. We first get all visible entities only.
	std::list<ak::UID> selectedModelEntityIDs;
	getSelectedVisibleModelEntityIDs(selectedModelEntityIDs);
	bool itemsVisible = true;

	// If we do not have visible entities, then we also look for the hidden ones.
	if (selectedModelEntityIDs.empty())
	{
		getSelectedModelEntityIDs(selectedModelEntityIDs);
		itemsVisible = false;
	}

	try {
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_DeleteProperty, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(selectedModelEntityIDs, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityName, ot::JsonString(_itemName, doc.GetAllocator()), doc.GetAllocator());
		std::string response;

		for (auto reciever : m_modelViewNotifier) {
			sendHttpRequest(EXECUTE, reciever->serviceURL(), doc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				assert(0); // ERROR
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				assert(0); // WARNING
			}
		}
	}
	catch (...) {
		assert(0); // Error handling
	}

	AppBase::instance()->lockPropertyGrid(false);
}

void ExternalServicesComponent::entitiesSelected(ModelUIDtype modelID, ot::serviceID_t replyToServiceID, const std::string &selectionAction, const std::string &selectionInfo, std::list<std::string> &optionNames, std::list<std::string> &optionValues)
{
	try {
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_EntitiesSelected, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_SelectionAction, rapidjson::Value(selectionAction.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_SelectionInfo, rapidjson::Value(selectionInfo.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_OptNames, ot::JsonArray(optionNames, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_OptValues, ot::JsonArray(optionValues, inDoc.GetAllocator()), inDoc.GetAllocator());

		ot::ServiceBase *receiver = getService(replyToServiceID);

		if (receiver != nullptr)
		{
			std::string response;
			sendHttpRequest(EXECUTE, receiver->serviceURL(), inDoc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				assert(0); // ERROR
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				assert(0); // WARNING
			}
		}
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::executeAction(ModelUIDtype modelID, ModelUIDtype buttonID)
{
	try {
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteAction, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ID, modelID, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(ak::uiAPI::object::getObjectUniqueName(buttonID).toStdString(), doc.GetAllocator()), doc.GetAllocator());
		std::string response;

		for (auto reciever : m_modelViewNotifier) {
			sendHttpRequest(EXECUTE, reciever->serviceURL(), doc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				assert(0); // ERROR
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
			}
		}


	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::prefetchDataThread(const std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> prefetchIDs)
{
	prefetchDocumentsFromStorage(projectName, prefetchIDs);
	m_prefetchingDataCompleted = true;
}

void ExternalServicesComponent::contextMenuItemClicked(ot::ServiceBase * _sender, const std::string& _menuName, const std::string& _itemName) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_ContextMenuItemClicked, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ContextMenuName, ot::JsonString(_menuName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ContextMenuItemName, ot::JsonString(_itemName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	sendHttpRequest(EXECUTE, _sender->serviceURL(), doc, response);

	// Check if response is an error or warning
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		OT_LOG_E(response);
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		OT_LOG_W(response);
	}
}

void ExternalServicesComponent::contextMenuItemCheckedChanged(ot::ServiceBase * _sender, const std::string& _menuName, const std::string& _itemName, bool _isChecked) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_ContextMenuItemCheckedChanged, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ContextMenuName, ot::JsonString(_menuName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ContextMenuItemName, ot::JsonString(_itemName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_CheckedState, _isChecked, doc.GetAllocator());

	std::string response;
	sendHttpRequest(EXECUTE, _sender->serviceURL(), doc, response);

	// Check if response is an error or warning
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		OT_LOG_E(response);
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		OT_LOG_W(response);
	}
}

bool ExternalServicesComponent::projectIsOpened(const std::string &projectName, std::string &projectUser)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_IsProjectOpen, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());

	AppBase * app{ AppBase::instance() };

	std::string response;
	sendHttpRequest(EXECUTE, m_globalSessionServiceURL, doc.toJson(), response);

	// todo: add json return value containing true/false and username instead of empty string for more clarity
	if (response.empty()) return false;

	projectUser = response;
	return true;
}


// ###################################################################################################

// Messaging

size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
	try {
		data->append((char*)ptr, size * nmemb);
		return size * nmemb;
	}
	catch (...) {
		assert(0); // Error handling
	}
	return 0;
}

bool ExternalServicesComponent::sendHttpRequest(RequestType operation, ot::OwnerService _service, ot::JsonDocument& doc, std::string& response) {
	auto it = m_serviceIdMap.find(_service.getId());
	if (it == m_serviceIdMap.end()) {
		OT_LOG_E("Failed to find service with id \"" + std::to_string(_service.getId()) + "\"");
		return false;
	}
	return sendHttpRequest(operation, it->second->serviceURL(), doc, response);
}

bool ExternalServicesComponent::sendHttpRequest(RequestType operation, const ot::BasicServiceInformation& _service, ot::JsonDocument& doc, std::string& response) {
	auto s = this->getService(_service);
	if (s) {
		return this->sendHttpRequest(operation, s->serviceURL(), doc, response);
	}
	else {
		return false;
	}
}

bool ExternalServicesComponent::sendHttpRequest(RequestType operation, const std::string &url, ot::JsonDocument &doc, std::string &response)
{
	try { return sendHttpRequest(operation, url, doc.toJson(), response); }
	catch (...) {
		assert(0); // Error handling
		return false;
	}
}

bool ExternalServicesComponent::sendHttpRequest(RequestType operation, const std::string &url, const std::string &message, std::string &response) {
	try {

		std::string globalServiceURL = AppBase::instance()->getServiceURL();

		if (m_websocket != nullptr) { return sendRelayedRequest(operation, url, message, response); }

		bool success = false;


		switch (operation)
		{
		case EXECUTE:
			success = ot::msg::send(globalServiceURL, url, ot::EXECUTE_ONE_WAY_TLS, message, response);
			break;
		case QUEUE:
			success = ot::msg::send(globalServiceURL, url, ot::QUEUE, message, response);
			break;
		default:
			assert(0);
		}

		return success;
	}
	catch (...) {
		assert(0); // Error handling
		return false;
	}
}

bool ExternalServicesComponent::sendRelayedRequest(RequestType operation, const std::string &url, const std::string &json, std::string &response)
{
	assert(m_websocket != nullptr);

	// This function is sending the request through the UI relay service to the destination

	// Now we convert the document to a string 

	// Now we add the destination url and the operation mode to the string 
	// In this case, we can simply strip it from the message in the relay service without decoding the message part itself.
	std::string mode;
	switch (operation)
	{
	case EXECUTE:
		mode = "execute";
		break;
	case QUEUE:
		mode = "queue";
		break;
	default:
		assert(0); // Unknown operation
	}

	std::string request(mode);
	request.append("\n").append(url).append("\n").append(json);

	OT_LOG("Sending message to (Receiver = \"" + url + "\"; Endpoint = " + (operation == EXECUTE ? "Execute" : (operation == QUEUE ? "Queue" : "Execute one way TLS")) + "). Message = \"" + json + "\"", ot::OUTGOING_MESSAGE_LOG);

	// And finally send it through the websocket
	m_websocket->sendMessage(request, response);

	OT_LOG("...Sending message to (Receiver = \"" + url + "\"; Endpoint = " + (operation == EXECUTE ? "Execute" : (operation == QUEUE ? "Queue" : "Execute one way TLS")) + ") completed. Response = \"" + response + "\"", ot::OUTGOING_MESSAGE_LOG);

	return true;
}

bool ExternalServicesComponent::sendKeySequenceActivatedMessage(KeyboardCommandHandler * _sender) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_KeySequenceActivated, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_KeySequence, ot::JsonString(_sender->keySequence().toStdString(), doc.GetAllocator()), doc.GetAllocator());
	std::string response;

	if (!sendHttpRequest(EXECUTE, _sender->creator()->serviceURL(), doc, response)) {
		assert(0); // Failed to send HTTP request
	}
	// Check if response is an error or warning
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		assert(0); // ERROR
		AppBase::instance()->showErrorPrompt(response.c_str(), "Error");
		return false;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		assert(0); // WARNING
		AppBase::instance()->showWarningPrompt(response.c_str(), "Warning");
		return false;
	}
	return true;
}

void ExternalServicesComponent::sendRubberbandResultsToService(ot::serviceID_t _serviceId, const std::string& _note, const std::string& _pointJson, const std::vector<double> &transform) {
	try {
		auto receiver = m_serviceIdMap.find(_serviceId);
		if (receiver == m_serviceIdMap.end()) {
			assert(0);	// service disconnected / invalid
			return;
		}

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_CreateGeometryFromRubberbandData, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_VIEW_RUBBERBAND_Note, ot::JsonString(_note, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_VIEW_RUBBERBAND_PointDocument, ot::JsonString(_pointJson, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_VIEW_RUBBERBAND_Transform, ot::JsonArray(transform, doc.GetAllocator()), doc.GetAllocator());
		std::string response;

		sendHttpRequest(EXECUTE, receiver->second->serviceURL(), doc, response);
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			assert(0); // ERROR
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
		}

	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::requestUpdateVTKEntity(unsigned long long modelEntityID)
{
	try {

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_RequestUpdateVisualizationEntity, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, modelEntityID, doc.GetAllocator());
		std::string response;

		for (auto reciever : m_modelViewNotifier)
		{
			sendHttpRequest(EXECUTE, reciever->serviceURL(), doc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				assert(0); // ERROR
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				assert(0); // WARNING
			}
		}
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::activateVersion(const std::string &version)
{
	try {

		if (this->getServiceFromNameType(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL) != nullptr)
		{
			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ActivateVersion, doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(version, doc.GetAllocator()), doc.GetAllocator());
			std::string response;

			this->sendHttpRequest(EXECUTE, this->getServiceFromNameType(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL)->serviceURL(), doc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				assert(0); // ERROR
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				assert(0); // WARNING
			}
		}
	}
	catch (...) {
		assert(0); // Error handling
	}
}


// ###################################################################################################

// Project handling

std::list<std::string> ExternalServicesComponent::getListOfProjectTypes(void)
{
	std::list<std::string> projectTypes;

	AppBase* app{ AppBase::instance() };
	std::string response;

#ifdef OT_USE_GSS
	// Request a session service from the global session service
	ot::JsonDocument gssDoc;
	gssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_CreateNewSession, gssDoc.GetAllocator()), gssDoc.GetAllocator());
	gssDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, gssDoc.GetAllocator()), gssDoc.GetAllocator());
	gssDoc.AddMember(OT_ACTION_PARAM_USER_NAME, ot::JsonString(app->getCurrentUserName(), gssDoc.GetAllocator()), gssDoc.GetAllocator());

	if (!sendHttpRequest(EXECUTE, m_globalSessionServiceURL, gssDoc.toJson(), response)) {
		assert(0); // Failed to send
		OT_LOG_E("Failed to send \"Create new session\" request to the global session service");
		app->showErrorPrompt("Failed to send \"Create new session\" request to the global session service", "Error");
		return projectTypes;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		assert(0); // ERROR
		OT_LOG_E(response);
		app->showErrorPrompt(response.c_str(), "Error");
		return projectTypes;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		assert(0); // WARNING
		OT_LOG_W(response);
		app->showWarningPrompt(response.c_str(), "Warning");
		return projectTypes;
	}
	m_sessionServiceURL = response;

	OT_LOG_D("GSS provided the LSS at \"" + m_sessionServiceURL + "\"");
#endif

	ot::JsonDocument sessionDoc;
	sessionDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetListOfProjectTypes, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());

	// Add user credentials
	sessionDoc.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(app->getCredentialUserName(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
	sessionDoc.AddMember(OT_PARAM_AUTH_PASSWORD, ot::JsonString(app->getCredentialUserPasswordClear(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());

	response.clear();

	if (!sendHttpRequest(EXECUTE, m_sessionServiceURL, sessionDoc.toJson(), response)) {
		assert(0); // Failed to send
		OT_LOG_E("Failed to send http request to Local Session Service at \"" + m_sessionServiceURL + "\"");
		app->showErrorPrompt("Failed to send http request to Local Session Service", "Connection Error");
		return projectTypes;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		assert(0); // ERROR
		OT_LOG_E("Error response from  Local Session Service at \"" + m_sessionServiceURL + "\": " + response);
		app->showErrorPrompt("Failed to create Session. " + QString::fromStdString(response), "Error");
		return projectTypes;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		assert(0); // WARNING
		OT_LOG_W("Warning response from  Local Session Service at \"" + m_sessionServiceURL + "\": " + response);
		app->showErrorPrompt("Failed to create Session. " + QString::fromStdString(response), "Error");
		return projectTypes;
	}

	// Check response
	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	projectTypes = ot::json::getStringList(responseDoc, OT_ACTION_PARAM_SESSION_TYPE_LIST);

	return projectTypes;
}

void ExternalServicesComponent::openProject(const std::string & projectName, const std::string& projectType, const std::string & collectionName) {

	AppBase * app{ AppBase::instance() };

	app->lockWelcomeScreen(true);

	try {
		OT_LOG_D("Open project requested (Project name = \"" + projectName + ")");

		m_lockManager->lock(app, ot::ui::tlAll);

		StudioSuiteConnectorAPI::openProject();

		m_currentSessionID = projectName;
		m_currentSessionID.append(":").append(collectionName);
		AppBase::instance()->SetCollectionName(collectionName);

		std::string response;
#ifdef OT_USE_GSS
		// Request a session service from the global session service
		ot::JsonDocument gssDoc;
		gssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_CreateNewSession, gssDoc.GetAllocator()), gssDoc.GetAllocator());
		gssDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, gssDoc.GetAllocator()), gssDoc.GetAllocator());
		gssDoc.AddMember(OT_ACTION_PARAM_USER_NAME, ot::JsonString(app->getCurrentUserName(), gssDoc.GetAllocator()), gssDoc.GetAllocator());

		if (!sendHttpRequest(EXECUTE, m_globalSessionServiceURL, gssDoc.toJson(), response)) {
			assert(0); // Failed to send
			OT_LOG_E("Failed to send \"Create new session\" request to the global session service");
			app->showErrorPrompt("Failed to send \"Create new session\" request to the global session service", "Error");
			return;
		}
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			assert(0); // ERROR
			OT_LOG_E(response);
			app->showErrorPrompt(response.c_str(), "Error");
			return;
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
			OT_LOG_W(response);
			app->showWarningPrompt(response.c_str(), "Warning");
			return;
		}
		m_sessionServiceURL = response;

		OT_LOG_D("GSS provided the LSS at \"" + m_sessionServiceURL + "\"");
#endif

		app->setCurrentProjectName(projectName);

		// ##################################################################

		// Create new session command
		ot::JsonDocument sessionDoc;
		sessionDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_CreateNewSession, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());

		// Add project and user information
		sessionDoc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(collectionName, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_PROJECT_NAME, ot::JsonString(projectName, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_USER_NAME, ot::JsonString(AppBase::instance()->getCurrentUserName(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_PARAM_SETTINGS_USERCOLLECTION, ot::JsonString(AppBase::instance()->getCurrentUserCollection(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());

		// Add session information
		sessionDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_SESSION_TYPE, ot::JsonString(projectType, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());

		// Add service information
		sessionDoc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_UI, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(OT_INFO_SERVICE_TYPE_UI, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_PORT, ot::JsonString(ot::IpConverter::portFromIp(m_uiServiceURL), sessionDoc.GetAllocator()), sessionDoc.GetAllocator()); // todo: rework -> create session -> send service port (ui uses the websocket and the port is not required)
		sessionDoc.AddMember(OT_ACTION_PARAM_START_RELAY, m_isRelayServiceRequired, sessionDoc.GetAllocator());

		// Add user credentials
		sessionDoc.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(app->getCredentialUserName(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_PARAM_AUTH_PASSWORD, ot::JsonString(app->getCredentialUserPasswordClear(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());

		response.clear();
		if (!sendHttpRequest(EXECUTE, m_sessionServiceURL, sessionDoc.toJson(), response)) {
			assert(0); // Failed to send
			OT_LOG_E("Failed to send http request to Local Session Service at \"" + m_sessionServiceURL + "\"");
			app->showErrorPrompt("Failed to send http request to Local Session Service", "Connection Error");
			return;
		}
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			assert(0); // ERROR
			OT_LOG_E("Error response from  Local Session Service at \"" + m_sessionServiceURL + "\": " + response);
			app->showErrorPrompt("Failed to create Session. " + QString::fromStdString(response), "Error");
			return;
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
			OT_LOG_W("Warning response from  Local Session Service at \"" + m_sessionServiceURL + "\": " + response);
			app->showErrorPrompt("Failed to create Session. " + QString::fromStdString(response), "Error");
			return;
		}
		
		// Check response
		ot::JsonDocument responseDoc;
		responseDoc.fromJson(response);

		// ##################################################################

		app->setServiceID(ot::json::getUInt(responseDoc, OT_ACTION_PARAM_SERVICE_ID));

		// Ensure to update the window title containing the project name
		app->setCurrentProjectIsModified(false);

		if (m_isRelayServiceRequired) {
			std::string websocketIP = ot::json::getString(responseDoc, OT_ACTION_PARAM_WebsocketURL);

			if (m_websocket) delete m_websocket;
			m_websocket = new WebsocketClient(websocketIP);

			OT_LOG_D("Created websocket client (WebsocketURL = \"" + websocketIP + "\")");
		}
		response = "";

		// TabToolBar tab order
		QStringList ttbOrder;
		std::list<std::string> ttbOrderL = ot::json::getStringList(responseDoc, OT_ACTION_PARAM_UI_ToolBarTabOrder);
		for (auto orderItem : ttbOrderL) {
			ttbOrder.push_back(orderItem.c_str());
		}
		AppBase::instance()->setTabToolBarTabOrder(ttbOrder);

		// ##################################################################

		// Now we check whether the startup sequence is completed
		bool startupReady = false;

		ot::JsonDocument checkCommandDoc;
		checkCommandDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_CheckStartupCompleted, checkCommandDoc.GetAllocator()), checkCommandDoc.GetAllocator());
		checkCommandDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, checkCommandDoc.GetAllocator()), checkCommandDoc.GetAllocator());
		std::string checkCommandString = checkCommandDoc.toJson();

		OT_LOG_D("Waiting for Startup Completed");

		do
		{
			if (!sendHttpRequest(EXECUTE, m_sessionServiceURL, checkCommandString, response)) {
				m_lockManager->unlock(AppBase::instance(), ot::ui::tlAll);
				throw std::exception("Failed to send http request to Session Service");
			}
			if (response == OT_ACTION_RETURN_VALUE_TRUE) {
				startupReady = true;
			}
			else if (response == OT_ACTION_RETURN_VALUE_FALSE) {
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(1s);
			}
			else {
				m_lockManager->unlock(AppBase::instance(), ot::ui::tlAll);
				OT_LOG_E("Invalid Session Service response: " + response);
				throw std::exception(("Invalid Session Service response: " + response).c_str());
			}
		} while (!startupReady);

		OT_LOG_D("Startup is completed");

		// Set service visible (will notify others that the UI is available)
		ot::JsonDocument visibilityCommand;
		visibilityCommand.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceShow, visibilityCommand.GetAllocator()), visibilityCommand.GetAllocator());
		visibilityCommand.AddMember(OT_ACTION_PARAM_SERVICE_ID, AppBase::instance()->serviceID(), visibilityCommand.GetAllocator());
		visibilityCommand.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, visibilityCommand.GetAllocator()), visibilityCommand.GetAllocator());
		if (!sendHttpRequest(EXECUTE, m_sessionServiceURL, visibilityCommand.toJson(), response)) {
			throw std::exception("Failed to send http request");
		}
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			m_lockManager->unlock(AppBase::instance(), ot::ui::tlAll);
			std::string ex("From ");
			ex.append(m_sessionServiceURL).append(": ").append(response);
			throw std::exception(ex.c_str());
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			m_lockManager->unlock(AppBase::instance(), ot::ui::tlAll);
			std::string ex("From ");
			ex.append(m_sessionServiceURL).append(": ").append(response);
			throw std::exception(ex.c_str());
		}

		// Add services that are running in this session to the services list
		ot::JsonDocument serviceListDoc;
		serviceListDoc.fromJson(response);
		auto serviceList = ot::json::getObjectList(serviceListDoc, OT_ACTION_PARAM_SESSION_SERVICES);
		for (auto serviceJSON : serviceList) {
			std::string senderURL = ot::json::getString(serviceJSON, OT_ACTION_PARAM_SERVICE_URL);
			std::string senderName = ot::json::getString(serviceJSON, OT_ACTION_PARAM_SERVICE_NAME);
			std::string senderType = ot::json::getString(serviceJSON, OT_ACTION_PARAM_SERVICE_TYPE);
			ot::serviceID_t senderID = ot::json::getUInt(serviceJSON, OT_ACTION_PARAM_SERVICE_ID);

			if (senderType == OT_INFO_SERVICE_TYPE_MODEL)
			{
				determineViews(senderURL);
			}

			auto oldService = m_serviceIdMap.find(senderID);
			if (oldService == m_serviceIdMap.end()) {
				m_serviceIdMap.insert_or_assign(senderID, new ot::ServiceBase{ senderName, senderType, senderURL, senderID });
			}
		}

#ifdef OT_USE_GSS
		// Start the session service health check
		ot::startSessionServiceHealthCheck(m_sessionServiceURL);
#endif // OT_USE_GSS

		m_lockManager->unlock(AppBase::instance(), ot::ui::tlAll);

		OT_LOG_D("Open project completed");
	}
	catch (const std::exception & e) {
		assert(0);
		OT_LOG_E(e.what());
		app->lockWelcomeScreen(false);
		app->showErrorPrompt(e.what(), "Error");
	}
	catch (...) {
		assert(0);
		OT_LOG_E("[FATAL] Unknown error");
		app->lockWelcomeScreen(false);
		app->showErrorPrompt("Unknown error occured while creating a new session", "Fatal Error");
	}
}

void ExternalServicesComponent::determineViews(const std::string &modelServiceURL)
{
	ot::JsonDocument sendingDoc;

	sendingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_PARAM_MODEL_ViewsForProjectType, sendingDoc.GetAllocator()), sendingDoc.GetAllocator());

	std::string response;
	sendHttpRequest(EXECUTE, modelServiceURL, sendingDoc, response);
	// Check if response is an error or warning
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		assert(0); // ERROR
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		assert(0); // WARNING
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	bool visible3D = responseDoc[OT_ACTION_PARAM_UI_TREE_Visible3D].GetBool();
	bool visible1D = responseDoc[OT_ACTION_PARAM_UI_TREE_Visible1D].GetBool();
	bool visibleTable = responseDoc[OT_ACTION_PARAM_UI_TREE_VisibleTable].GetBool();
	bool visibleBlockPicker = responseDoc[OT_ACTION_PARAM_UI_TREE_VisibleBlockPicker].GetBool();

	AppBase* app{ AppBase::instance() };

	app->setVisible3D(visible3D);
	app->setVisible1D(visible1D);
	app->setVisibleTable(visibleTable);
	app->setVisibleBlockPicker(visibleBlockPicker);
}

void ExternalServicesComponent::closeProject(bool _saveChanges) {
	try {
		UserSettings::instance()->clear();

		AppBase * app{ AppBase::instance() };

		OT_LOG_D("Closing project { name = \"" + app->getCurrentProjectName() + "\"; SaveChanges = " + (_saveChanges ? "True" : "False"));

		std::string projectName = app->getCurrentProjectName();
		if (projectName.length() == 0) { return; }

		// Remove all notifiers
		m_modelViewNotifier.clear();

		// Notify the session service that the sesion should be closed now
		ot::JsonDocument shutdownCommand;
		shutdownCommand.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSession, shutdownCommand.GetAllocator()), shutdownCommand.GetAllocator());
		shutdownCommand.AddMember(OT_ACTION_PARAM_SERVICE_ID, app->serviceID(), shutdownCommand.GetAllocator());
		shutdownCommand.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, shutdownCommand.GetAllocator()), shutdownCommand.GetAllocator());

		std::string response;
		if (!sendHttpRequest(EXECUTE, m_sessionServiceURL, shutdownCommand, response)) {
			OT_LOG_EA("Failed to send shutdown session request to LSS");
		}
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			OT_LOG_EAS("Invalid response: " + response);
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			OT_LOG_WAS("Invalid response: " + response);
		}

#ifdef OT_USE_GSS
		// Stop the session service health check
		ot::stopSessionServiceHealthCheck();
#endif // OT_USE_GSS

		QEventLoop eventLoop;
		//eventLoop.processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);
		eventLoop.processEvents(QEventLoop::AllEvents);

		// Get the id of the curently active model
		ModelUIDtype modelID = app->getViewerComponent()->getActiveDataModel();

		//NOTE, model ids will no longer be used in the future
		if (modelID == 0) return;  // No project currently active
		modelID = 1;

		// Notify the websocket that the project is closing (do not worry if the relay service shuts down)
		if (m_websocket != nullptr) { m_websocket->isClosing(); }

		// Now get the id of the corresponding visualization model
		ModelUIDtype visualizationModel = app->getViewerComponent()->getActiveViewerModel();

		// Deactivate the visualization model (this will also remove the tree entries)
		app->getViewerComponent()->deactivateCurrentlyActiveModel();

		// Close the model (potentially with saving).
		// This operation will also post delete queries. In this case, no attempt will be made to 
		// remove entities in the visualization model


		// Delete the corresponding visualization model. This will also detach the currently active viewers such that 
		// they no longer refer to the visualization model. The viewer widget itself can not be deleted, since it is still 
		// attached to a tab. The tab with the dear viewer will therefore need to be removed separately.
		if (visualizationModel > 0)
		{
			app->getViewerComponent()->deleteModel(visualizationModel);
		}

		// Reset all service information
		for (auto s : m_serviceIdMap) {
			m_lockManager->cleanService(s.second, false, true);
			m_controlsManager->serviceDisconnected(s.second);
			app->shortcutManager()->creatorDestroyed(s.second);
			app->contextMenuManager()->serviceDisconnected(s.second);
			m_owner->uiPluginManager()->unloadPlugins(s.second);
			delete s.second;
		}
		m_lockManager->cleanService(app->getViewerComponent());
		m_controlsManager->serviceDisconnected(app->getViewerComponent());

		app->shortcutManager()->clearViewerHandler();
		app->clearNavigationTree();
		app->clearPropertyGrid();
		app->contextMenuManager()->serviceDisconnected(nullptr);
		app->globalGraphicsPicker()->clear();

		// Clear all maps
		m_serviceToUidMap.clear();
		m_serviceIdMap.clear();

		// Close all the tabs in the tab widget for the viewer
		app->closeAllViewerTabs();

		//NOTE, add service notification to close the session, or deregister the uiService
		app->setServiceID(ot::invalidServiceID);
		m_currentSessionID = "";
		app->clearSessionInformation();

		app->replaceInfoMessage(c_buildInfo);

		if (m_websocket != nullptr) { delete m_websocket; m_websocket = nullptr; }

		OT_LOG_D("Close project done");
	}
	catch (const std::exception & e) {
		OT_LOG_E(e.what());
	}
	catch (...) {
		OT_LOG_E("Unknown error");
	}
}

void ExternalServicesComponent::saveProject() {
	try {
		// Get the id of the curently active model
		ModelUIDtype modelID = AppBase::instance()->getViewerComponent()->getActiveDataModel();
		if (modelID == 0) return;  // No project currently active

		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_PROJ_Save, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());

		std::string response;

		for (auto reciever : m_modelViewNotifier) {
			sendHttpRequest(EXECUTE, reciever->serviceURL(), inDoc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				assert(0); // ERROR
			}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
		}
		}

		AppBase::instance()->setCurrentProjectIsModified(false);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

// ###################################################################################################

// File operations

std::list<std::string> ExternalServicesComponent::RequestFileNames(const std::string& dialogTitle, const std::string& fileMask)
{
	std::list<std::string> selectedFilesStd;

	try {
		QFileDialog dialog(nullptr);
		dialog.setFileMode(QFileDialog::ExistingFiles);
		dialog.setWindowTitle(dialogTitle.c_str());
		dialog.setWindowFilePath(QDir::currentPath());
		dialog.setNameFilter(QString(fileMask.c_str()) + " ;; All files (*.*)");
		
		if (dialog.exec())
		{
			QStringList selectedFiles = dialog.selectedFiles();
			for (QString& file : selectedFiles)
			{
				selectedFilesStd.push_back(file.toStdString());
			}
			return selectedFilesStd;
		}
	}
	catch (std::exception& e)
	{
		throw std::exception(("File could not be loaded due to this exeption: " + std::string(e.what())).c_str());
	}

	return selectedFilesStd; // This is just to keep the compiler happy.
}

void ExternalServicesComponent::ReadFileContent(const std::string &fileName, std::string &fileContent, unsigned long long &uncompressedDataLength)
{
	fileContent.clear();

	// Read the file content
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	int data_length = (int)file.tellg();
	file.seekg(0, std::ios::beg);

	char *data = new char[data_length];
	if (!file.read(data, data_length)) return;

	uncompressedDataLength = data_length;

	// Compress the file data content
	uLong compressedSize = compressBound((uLong)data_length);

	char *compressedData = new char[compressedSize];
	compress((Bytef*)compressedData, &compressedSize, (Bytef*)data, data_length);

	delete[] data;
	data = nullptr;

	// Convert the binary to an encoded string
	int encoded_data_length = Base64encode_len(compressedSize);
	char* base64_string = new char[encoded_data_length];

	Base64encode(base64_string, compressedData, compressedSize); // "base64_string" is a then null terminated string that is an encoding of the binary data pointed to by "data"

	delete[] compressedData;
	compressedData = nullptr;

	fileContent = std::string(base64_string);

	delete[] base64_string;
	base64_string = nullptr;
}

// ###################################################################################################

// Table operations
void ExternalServicesComponent::SetColourOfSelectedRange(ModelUIDtype visModelID, ot::Color background)
{
	ViewerAPI::ChangeColourOfSelection(visModelID, background);
}


void ExternalServicesComponent::RequestTableSelection(ModelUIDtype visModelID, std::string URL, std::string subsequentFunction)
{
	std::vector<ot::TableRange> ranges = ViewerAPI::GetSelectedTableRange(visModelID);
	std::pair<ot::UID, ot::UID> tableIdentifyer = ViewerAPI::GetActiveTableIdentifyer(visModelID);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteFunction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(subsequentFunction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, tableIdentifyer.first, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion, tableIdentifyer.second, doc.GetAllocator());

	ot::JsonArray vectOfRanges;
	for (auto range : ranges)
	{
		ot::JsonObject tmp;
		range.addToJsonObject(tmp, doc.GetAllocator());
		vectOfRanges.PushBack(tmp, doc.GetAllocator());
	}
	doc.AddMember("Ranges", vectOfRanges, doc.GetAllocator());
	
	std::string response;
	sendHttpRequest(EXECUTE, URL, doc, response);
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		assert(0); // ERROR
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response)
	{
	assert(0); // WARNING
	}
	
}

// Slots

char *ExternalServicesComponent::performAction(const char *json, const char *senderIP)
{
	using namespace std::chrono_literals;
	static bool lock = false;
	while (lock) std::this_thread::sleep_for(1ms);
	lock = true;

	try {
		char *retval = ot::ActionDispatcher::instance().dispatchWrapper(json, senderIP, ot::QUEUE);
		
		lock = false;
		return retval;
	}
	catch (...) {
		OT_LOG_EA("[FATAL] Unknown error occured");
		lock = false;
		return nullptr;
	}
}

void ExternalServicesComponent::InformSenderAboutFinishedAction(std::string URL, std::string subsequentFunction)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_RETURN_VALUE_TRUE, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(subsequentFunction, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	sendHttpRequest(EXECUTE, URL, doc, response);
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		assert(0); // ERROR
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) 
	{
		assert(0); // WARNING
	}
}

void ExternalServicesComponent::queueAction(const char *json, const char *senderIP)
{
	using namespace std::chrono_literals;
	static bool lock = false;
	while (lock) std::this_thread::sleep_for(1ms);
	lock = true;

	try {
		ot::ActionDispatcher::instance().dispatch(json, ot::QUEUE);

		delete[] senderIP;
		senderIP = nullptr;

		// Now notify the end of the currently processed message
		if (m_websocket != nullptr)
		{
			m_websocket->finishedProcessingQueuedMessage();
		}
	}
	catch (...) {
		assert(0); // Error handling
	}

	lock = false;
}

void ExternalServicesComponent::deallocateData(const char *data)
{
	delete[] data;
}

void ExternalServicesComponent::shutdownAfterSessionServiceDisconnected(void) {
	ot::stopSessionServiceHealthCheck();
	ak::uiAPI::promptDialog::show("The session service has died unexpectedly. The application will be closed now.", "Error", ak::promptOkIconLeft, "DialogError", "Default", AppBase::instance()->mainWindow());
	exit(0);
}

void ExternalServicesComponent::sendExecuteRequest(const char* url, const char* message)
{
	std::string responseString;
	sendHttpRequest(EXECUTE, url, message, responseString);

	delete[] url; url = nullptr;
	delete[] message; message = nullptr;
}

char* ExternalServicesComponent::sendExecuteRequestWithAnswer(const char* url, const char* message)
{
	std::string responseString;
	sendHttpRequest(EXECUTE, url, message, responseString);

	delete[] url; url = nullptr;
	delete[] message; message = nullptr;

	char* response = new char[responseString.length() + 1];
	strcpy(response, responseString.c_str());

	return response;
}

void ExternalServicesComponent::setProgressState(bool visible, const char* message, bool continuous)
{
	AppBase* app = AppBase::instance();
	assert(app != nullptr);
	if (app != nullptr) app->setProgressBarVisibility(message, visible, continuous);

	delete[] message;
	message = nullptr;
}

void ExternalServicesComponent::setProgressValue(int percentage)
{
	AppBase* app = AppBase::instance();
	assert(app != nullptr);
	if (app != nullptr) app->setProgressBarValue(percentage);
}

void ExternalServicesComponent::lockGui(void)
{
	ot::Flags<ot::ui::lockType> lockFlags;
	lockFlags.setFlag(ot::ui::lockType::tlModelWrite);
	lockFlags.setFlag(ot::ui::lockType::tlViewWrite);
	lockFlags.setFlag(ot::ui::lockType::tlModelRead);

	m_lockManager->lock(nullptr, lockFlags);
}

void ExternalServicesComponent::unlockGui(void)
{
	ot::Flags<ot::ui::lockType> lockFlags;
	lockFlags.setFlag(ot::ui::lockType::tlModelWrite);
	lockFlags.setFlag(ot::ui::lockType::tlViewWrite);
	lockFlags.setFlag(ot::ui::lockType::tlModelRead);

	m_lockManager->unlock(nullptr, lockFlags);
}

void ExternalServicesComponent::showError(const char* message)
{
	AppBase* app = AppBase::instance();
	assert(app != nullptr);
	if (app != nullptr) app->showErrorPrompt(message, "Error");

	delete[] message;
	message = nullptr;
}

void ExternalServicesComponent::showInformation(const char* message)
{
	AppBase* app = AppBase::instance();
	assert(app != nullptr);
	if (app != nullptr) app->showInfoPrompt(message, "Success");

	delete[] message;
	message = nullptr;
}

void ExternalServicesComponent::activateModelVersion(const char* version)
{
	activateVersion(version);

	delete[] version;
	version = nullptr;
}

// ###################################################################################################

// JSON helper functions

std::vector<std::array<double, 3>> ExternalServicesComponent::getVectorDoubleArrayFromDocument(ot::JsonDocument &doc, const std::string &itemNname)
{
	std::vector<std::array<double, 3>> result;
	try {
		rapidjson::Value vectorDouble = doc[itemNname.c_str()].GetArray();
		result.resize(vectorDouble.Size() / 3);

		for (unsigned int i = 0; i < vectorDouble.Size() / 3; i++)
		{
			result[i][0] = vectorDouble[3 * i].GetDouble();
			result[i][1] = vectorDouble[3 * i + 1].GetDouble();
			result[i][2] = vectorDouble[3 * i + 2].GetDouble();
		}
	}
	catch (...) {
		assert(0); // Error handling
	}
	return result;
}

TreeIcon ExternalServicesComponent::getTreeIconsFromDocument(ot::JsonDocument &doc)
{
	TreeIcon treeIcons;

	try {
		treeIcons.size = doc[OT_ACTION_PARAM_UI_TREE_IconSize].GetInt();
		treeIcons.visibleIcon = doc[OT_ACTION_PARAM_UI_TREE_IconItemVisible].GetString();
		treeIcons.hiddenIcon = doc[OT_ACTION_PARAM_UI_TREE_IconItemHidden].GetString();
	}
	catch (...) 
	{
		treeIcons.size = 0;
		treeIcons.visibleIcon.clear();
		treeIcons.hiddenIcon.clear();
	}

	return treeIcons;
}

void ExternalServicesComponent::getVectorNodeFromDocument(ot::JsonDocument &doc, const std::string &itemName, std::vector<Geometry::Node> &result)
{
	try {
		rapidjson::Value nodeList = doc[itemName.c_str()].GetArray();
		int numberOfNodes = nodeList.Size() / 8;
		for (unsigned int i = 0; i < numberOfNodes; i++)
		{
			Geometry::Node n;
			n.setCoords(nodeList[i * 8].GetDouble(), nodeList[i * 8 + 1].GetDouble(), nodeList[i * 8 + 2].GetDouble());
			n.setNormals(nodeList[i * 8 + 3].GetDouble(), nodeList[i * 8 + 4].GetDouble(), nodeList[i * 8 + 5].GetDouble());
			n.setUVpar(nodeList[i * 8 + 6].GetDouble(), nodeList[i * 8 + 7].GetDouble());
			result.push_back(n);
		}
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::getListTriangleFromDocument(ot::JsonDocument &doc, const std::string &itemName, std::list <Geometry::Triangle > &result)
{
	try {
		rapidjson::Value triangleList = doc[itemName.c_str()].GetArray();
		int numberOfTriangles = triangleList.Size() / 4;
		for (unsigned int i = 0; i < numberOfTriangles; i++)
		{
			Geometry::Triangle t(triangleList[i * 4].GetInt64(), triangleList[i * 4 + 1].GetInt64(), triangleList[i * 4 + 2].GetInt64(), triangleList[i * 4 + 3].GetInt64());

			result.push_back(t);
		}
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::getListEdgeFromDocument(ot::JsonDocument &doc, const std::string &itemName, std::list <Geometry::Edge > &result)
{
	try {
		rapidjson::Value edgeList = doc[itemName.c_str()].GetArray();
		long numberOfEdges = edgeList[0].GetInt64();
		long edgeIndex = 1;

		for (long i = 0; i < numberOfEdges; i++)
		{
			Geometry::Edge e;
			e.setFaceId(edgeList[edgeIndex].GetInt64());
			edgeIndex++;
			e.setNpoints(edgeList[edgeIndex].GetInt64());
			edgeIndex++;

			for (long np = 0; np < e.getNpoints(); np++)
			{
				e.setPoint(np, edgeList[edgeIndex].GetDouble(), edgeList[edgeIndex + 1].GetDouble(), edgeList[edgeIndex + 2].GetDouble());
				edgeIndex += 3;
			}

			result.push_back(e);
		}
	}
	catch (...) {
		assert(0); // Error handling
	}
}

// ###################################################################################################

// Viewer helper functions

void ExternalServicesComponent::getSelectedModelEntityIDs(std::list<ModelUIDtype> &selected)
{
	try {
		AppBase::instance()->getViewerComponent()->getSelectedModelEntityIDs(selected);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::getSelectedVisibleModelEntityIDs(std::list<ModelUIDtype> &selected)
{
	try {
		AppBase::instance()->getViewerComponent()->getSelectedVisibleModelEntityIDs(selected);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

// ###################################################################################################

// Private functions

void ExternalServicesComponent::removeServiceFromList(std::vector<ot::ServiceBase *> &list, ot::ServiceBase *service)
{
	auto item = std::find(list.begin(), list.end(), service);

	while (item != list.end())
	{
		list.erase(item);
		item = std::find(list.begin(), list.end(), service);
	}
}

ak::UID ExternalServicesComponent::getServiceUiUid(ot::ServiceBase * _service) {
	auto itm = m_serviceToUidMap.find(_service->serviceName());
	if (itm == m_serviceToUidMap.end()) {
		ak::UID newUID{ ak::uiAPI::createUid() };
		m_serviceToUidMap.insert_or_assign(_service->serviceName(), newUID);
		return newUID;
	}
	else {
		return itm->second;
	}
}

ot::ServiceBase * ExternalServicesComponent::getService(ot::serviceID_t _serviceID) {
	auto service{ m_serviceIdMap.find(_serviceID) };
	if (service == m_serviceIdMap.end()) {
//		assert(0);
		std::string ex("A service with the id \"");
		ex.append(std::to_string(_serviceID));
		ex.append("\" was not registered before");
		throw std::exception(ex.c_str());
	}
	
	return service->second;
}

ot::ServiceBase * ExternalServicesComponent::getService(const ot::BasicServiceInformation& _serviceInfo) {

	for (auto s : m_serviceIdMap) {
		if (s.second->getBasicServiceInformation() == _serviceInfo) {
			return s.second;
		}
	}
	OT_LOG_WAS("Failed to find service { \"Service.Name\": \"" + _serviceInfo.serviceName() + "\"; \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }");
	return nullptr;
}

ot::ServiceBase* ExternalServicesComponent::getServiceFromNameType(const std::string& _serviceName, const std::string& _serviceType) {
	for (auto service : m_serviceIdMap)
	{
		if (service.second->serviceName() == _serviceName && service.second->serviceType() == _serviceType)
		{
			return service.second;
		}
	}

	OT_LOG_WAS("Failed to find service { \"Service.Name\": \"" + _serviceName + "\"; \"Service.Type\": \"" + _serviceType + "\" }");
	return nullptr;
}

// ###################################################################################################

// Action handler

std::string ExternalServicesComponent::handleCompound(ot::JsonDocument& _document) {
	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);
	rapidjson::Value documents = _document[OT_ACTION_PARAM_PREFETCH_Documents].GetArray();
	rapidjson::Value prefetchID = _document[OT_ACTION_PARAM_PREFETCH_ID].GetArray();
	rapidjson::Value prefetchVersion = _document[OT_ACTION_PARAM_PREFETCH_Version].GetArray();

	ot::Flags<ot::ui::lockType> lockFlags(ot::ui::tlAll);
	m_lockManager->lock(nullptr, lockFlags);

	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIDs;
	size_t numberPrefetch = prefetchID.Size();
	assert(prefetchVersion.Size() == numberPrefetch);

	for (size_t i = 0; i < numberPrefetch; i++)
	{
		prefetchIDs.push_back(std::pair<unsigned long long, unsigned long long>(prefetchID[i].GetInt64(), prefetchVersion[i].GetInt64()));
	}

	if (!prefetchIDs.empty())
	{
		m_prefetchingDataCompleted = false;
		std::thread workerThread(&ExternalServicesComponent::prefetchDataThread, this, projectName, prefetchIDs);

		while (!m_prefetchingDataCompleted)
		{
			QApplication::processEvents();
			Sleep(25);
		}

		workerThread.join();
		QApplication::processEvents();
	}

	int numberActions = documents.Size();

	// Nofify the viewer about the bulk processing (this e.g. avoids sending of unnecessary selection changed messages)
	AppBase::instance()->getViewerComponent()->setProcessingGroupOfMessages(true);

	// Disable tree sorting for better performance
	AppBase::instance()->setNavigationTreeSortingEnabled(false);
	AppBase::instance()->setNavigationTreeMultiselectionEnabled(false);

	bool tmp;
	for (long i = 0; i < numberActions; i++)
	{
		rapidjson::Value subdoc = documents[i].GetObject();

		ot::JsonDocument d;
		d.CopyFrom(subdoc, d.GetAllocator());

		std::string action = ot::json::getString(d, OT_ACTION_MEMBER);

		ot::ActionDispatcher::instance().dispatchLocked(action, d, tmp, ot::QUEUE);
	}

	// Re enable tree sorting
	AppBase::instance()->setNavigationTreeSortingEnabled(true);
	AppBase::instance()->setNavigationTreeMultiselectionEnabled(true);

	// Nofify the viewer about the end of the bulk processing
	AppBase::instance()->getViewerComponent()->setProcessingGroupOfMessages(false);

	m_lockManager->unlock(nullptr, lockFlags);

	return "";
}

std::string ExternalServicesComponent::handleMessage(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	ot::serviceID_t senderID = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	
	std::string msg("Message from ");
	msg.append(std::to_string(senderID)).append(": ").append(message);

	OT_LOG_I(msg);

	return "";
}

std::string ExternalServicesComponent::handleShutdown(ot::JsonDocument& _document) {
	OT_LOG_D("Showdown received");
	AppBase::instance()->showErrorPrompt("Shutdown requested by session service.", "Error");
	exit(0);	//NOTE, Add external shutdown

	return "";
}

std::string ExternalServicesComponent::handlePreShutdown(ot::JsonDocument& _document) {
	OT_LOG_D("Pre shutdown received");

	return "";
}

std::string ExternalServicesComponent::handleEmergencyShutdown(ot::JsonDocument& _document) {
	QString msg("An unexpected error occurred and the session needs to be closed.");
	ak::uiAPI::promptDialog::show(msg, "Open Twin", ak::promptOkIconLeft, "DialogError", "Default", AppBase::instance()->mainWindow());
	exit(1);

	return "";
}

std::string ExternalServicesComponent::handleServiceConnected(ot::JsonDocument& _document) {
	ot::serviceID_t senderID = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string senderURL = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_URL);
	std::string senderName = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_NAME);
	std::string senderType = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_TYPE);
	ot::ServiceBase* connectedService = new ot::ServiceBase(senderName, senderType, senderURL, senderID);
	m_serviceIdMap.insert_or_assign(senderID, connectedService);

	return "";
}

std::string ExternalServicesComponent::handleServiceDisconnected(ot::JsonDocument& _document) {
	ot::serviceID_t senderID = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string senderURL = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_URL);
	std::string senderName = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_NAME);
	std::string senderType = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_TYPE);

	auto itm = m_serviceIdMap.find(senderID);
	if (itm != m_serviceIdMap.end()) {
		ot::ServiceBase* actualService = itm->second;
		assert(actualService != nullptr);

		// Clean up elements
		m_lockManager->cleanService(actualService, false, true);
		m_controlsManager->serviceDisconnected(actualService);
		m_owner->uiPluginManager()->unloadPlugins(actualService);
		AppBase::instance()->shortcutManager()->creatorDestroyed(actualService);
		AppBase::instance()->contextMenuManager()->serviceDisconnected(actualService);

		// Clean up entry
		m_serviceIdMap.erase(actualService->serviceID());
		removeServiceFromList(m_modelViewNotifier, actualService);

		delete actualService;
	}

	return "";
}

std::string ExternalServicesComponent::handleShutdownRequestedByService(ot::JsonDocument& _document) {
	OTAssert(0, "External shutdown not supported");	// Add external shutdown

	return "";
}

std::string ExternalServicesComponent::handleModelExecuteFunction(ot::JsonDocument& _document) {
	const std::string subsequentFunction = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_FunctionName);

	ot::UIDList entityIDs = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_EntityIDList);
	ot::UIDList entityVersions = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_EntityVersionList);

	if (subsequentFunction == m_fileHandler.GetStoreFileFunctionName())
	{
		ot::JsonDocument  reply = m_fileHandler.StoreFileInDataBase(entityIDs, entityVersions);
		std::string response;
		sendHttpRequest(QUEUE, m_fileHandler.GetSenderURL(), reply, response);
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			OT_LOG_EAS(response);
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			OT_LOG_WAS(response);
		}
	}

	return "";
}

std::string ExternalServicesComponent::handleDisplayMessage(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	
	AppBase::instance()->appendInfoMessage(QString::fromStdString(message));

	return "";
}

std::string ExternalServicesComponent::handleDisplayDebugMessage(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	
	AppBase::instance()->appendDebugMessage(QString::fromStdString(message));

	return "";
}

std::string ExternalServicesComponent::handleReportError(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	ak::uiAPI::promptDialog::show(message.c_str(), "Open Twin", ak::promptIconLeft, "DialogError", "Default", AppBase::instance()->mainWindow());

	return "";
}

std::string ExternalServicesComponent::handleReportWarning(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	ak::uiAPI::promptDialog::show(message.c_str(), "Open Twin", ak::promptIconLeft, "DialogWarning", "Default", AppBase::instance()->mainWindow());

	return "";
}

std::string ExternalServicesComponent::handleReportInformation(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	ak::uiAPI::promptDialog::show(message.c_str(), "Open Twin", ak::promptIconLeft, "DialogInformation", "Default", AppBase::instance()->mainWindow());

	return "";
}

std::string ExternalServicesComponent::handlePromptInformation(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	std::string icon = ot::json::getString(_document, OT_ACTION_PARAM_ICON);
	std::string options = ot::json::getString(_document, OT_ACTION_PARAM_OPTIONS);
	std::string promptResponse = ot::json::getString(_document, OT_ACTION_PARAM_RESPONSE);
	std::string sender = ot::json::getString(_document, OT_ACTION_PARAM_SENDER);
	std::string parameter1 = ot::json::getString(_document, OT_ACTION_PARAM_PARAMETER1);

	ak::promptType promptType = ak::promptIconLeft;

	if (options == "YesNo")
	{
		promptType = ak::promptType::promptYesNoIconLeft;
	}
	else if (options == "YesNoCancel")
	{
		promptType = ak::promptType::promptYesNoCancelIconLeft;
	}
	else if (options == "OkCancel")
	{
		promptType = ak::promptType::promptOkCancelIconLeft;
	}
	else if (options == "Ok")
	{
		promptType = ak::promptType::promptOkIconLeft;
	}
	else
	{
		assert(0); // Unknown options
	}

	ak::dialogResult result = ak::uiAPI::promptDialog::show(message.c_str(), "Open Twin", promptType, icon.c_str(), "Default", AppBase::instance()->mainWindow());

	std::string queryResult;

	switch (result)
	{
	case ak::dialogResult::resultCancel: queryResult = "Cancel"; break;
	case ak::dialogResult::resultIgnore: queryResult = "Ignore"; break;
	case ak::dialogResult::resultNo:     queryResult = "No"; break;
	case ak::dialogResult::resultYes:    queryResult = "Yes"; break;
	case ak::dialogResult::resultNone:   queryResult = "None"; break;
	case ak::dialogResult::resultOk:     queryResult = "Ok"; break;
	case ak::dialogResult::resultRetry:  queryResult = "Retry"; break;
	default:
		assert(0); // Unknown type
	}

	ot::JsonDocument docOut;
	docOut.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_PromptResponse, docOut.GetAllocator()), docOut.GetAllocator());
	docOut.AddMember(OT_ACTION_PARAM_RESPONSE, ot::JsonString(promptResponse, docOut.GetAllocator()), docOut.GetAllocator());
	docOut.AddMember(OT_ACTION_PARAM_ANSWER, ot::JsonString(queryResult, docOut.GetAllocator()), docOut.GetAllocator());
	docOut.AddMember(OT_ACTION_PARAM_PARAMETER1, ot::JsonString(parameter1, docOut.GetAllocator()), docOut.GetAllocator());

	if (this->getServiceFromNameType(sender, sender) != nullptr)
	{
		std::string senderUrl = this->getServiceFromNameType(sender, sender)->serviceURL();

		std::string response;
		if (!sendHttpRequest(QUEUE, senderUrl, docOut, response)) {
			throw std::exception("Failed to send http request");
		}
	}

	return "";
}

std::string ExternalServicesComponent::handleRegisterForModelEvents(ot::JsonDocument& _document) {
	ot::serviceID_t senderID(ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID));
	auto s = m_serviceIdMap.find(senderID);

	// Check if the service was registered before
	if (s == m_serviceIdMap.end()) {
		std::string ex{ "A service with the id \"" };
		ex.append(std::to_string(senderID));
		ex.append("\" was not registered before");
		throw std::exception(ex.c_str());
	}
	if (s->second->serviceName() == OT_INFO_SERVICE_TYPE_MODEL)
	{
		m_modelServiceURL = s->second->serviceURL();
	}

	m_modelViewNotifier.push_back(s->second);

	OT_LOG_D("Service with ID \"" + std::to_string(s->second->serviceID()) + "\" was registered from model view events");

	return "";
}

std::string ExternalServicesComponent::handleDeregisterForModelEvents(ot::JsonDocument& _document) {
	ot::serviceID_t senderID(ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID));
	auto s = m_serviceIdMap.find(senderID);

	// Check if the service was registered before
	if (s == m_serviceIdMap.end()) {
		std::string ex("A service with the id \"");
		ex.append(std::to_string(senderID));
		ex.append("\" was not registered before");
		throw std::exception(ex.c_str());
	}

	this->removeServiceFromList(m_modelViewNotifier, s->second);

	OT_LOG_D("Service with ID \"" + std::to_string(s->second->serviceID()) + "\" was deregistered from model view events");

	return "";
}

std::string ExternalServicesComponent::handleGenerateUIDs(ot::JsonDocument& _document) {
	int count = _document[OT_ACTION_PARAM_COUNT].GetInt();
	// empty id list
	std::vector<ak::UID> idList;
	for (int i = 0; i < count; i++) { idList.push_back(ak::uiAPI::createUid()); }
	// json object contains the list

	ot::JsonDocument returnDoc;
	ot::JsonArray uidArray;
	for (ot::UID id : idList)
	{
		uidArray.PushBack(id, returnDoc.GetAllocator());
	}
	returnDoc.AddMember("idList", uidArray, returnDoc.GetAllocator());

	return returnDoc.toJson();
}

std::string ExternalServicesComponent::handleRequestFileForReading(ot::JsonDocument& _document) {
	std::string dialogTitle = ot::json::getString(_document, OT_ACTION_PARAM_UI_DIALOG_TITLE);
	std::string fileMask = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Mask);
	std::string subsequentFunction = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_FunctionName);
	std::string senderURL = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);
	bool loadContent = ot::json::getBool(_document, OT_ACTION_PARAM_FILE_LoadContent);

	QString fileName = QFileDialog::getOpenFileName(
		nullptr,
		dialogTitle.c_str(),
		QDir::currentPath(),
		QString(fileMask.c_str()) + " ;; All files (*.*)");

	if (fileName != "")
	{
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteFunction, inDoc.GetAllocator()), inDoc.GetAllocator());

		if (loadContent)
		{
			std::string fileContent;
			unsigned long long uncompressedDataLength{ 0 };

			// The file can not be directly accessed from the remote site and we need to send the file content over the communication
			ReadFileContent(fileName.toStdString(), fileContent, uncompressedDataLength);
			inDoc.AddMember(OT_ACTION_PARAM_FILE_Content, rapidjson::Value(fileContent.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
			inDoc.AddMember(OT_ACTION_PARAM_FILE_Content_UncompressedDataLength, rapidjson::Value(uncompressedDataLength), inDoc.GetAllocator());
			// We need to send the file content
			inDoc.AddMember(OT_ACTION_PARAM_FILE_Mode, rapidjson::Value(OT_ACTION_VALUE_FILE_Mode_Content, inDoc.GetAllocator()), inDoc.GetAllocator());
		}
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, rapidjson::Value(subsequentFunction.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_FILE_OriginalName, rapidjson::Value(fileName.toStdString().c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());

		std::string response;
		sendHttpRequest(EXECUTE, senderURL, inDoc, response);

		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			assert(0); // ERROR
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
		}
	}

	return "";
}

std::string ExternalServicesComponent::handleStoreFileInDatabase(ot::JsonDocument& _document) {
	std::string dialogTitle = ot::json::getString(_document, OT_ACTION_PARAM_UI_DIALOG_TITLE);
	std::string fileMask = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Mask);
	std::string entityType = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Type);
	try
	{
		std::list<std::string> absoluteFilePaths = RequestFileNames(dialogTitle, fileMask);

		if (absoluteFilePaths.size() != 0)
		{
			ot::JsonDocument sendingDoc;

			int requiredIdentifierPairsPerFile = 2;
			const int numberOfUIDs = static_cast<int>(absoluteFilePaths.size()) * requiredIdentifierPairsPerFile;

			sendingDoc.AddMember(OT_ACTION_PARAM_MODEL_ENTITY_IDENTIFIER_AMOUNT, numberOfUIDs, sendingDoc.GetAllocator());
			const std::string url = uiServiceURL();
			sendingDoc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(url, sendingDoc.GetAllocator()), sendingDoc.GetAllocator());
			sendingDoc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(m_fileHandler.GetStoreFileFunctionName(), sendingDoc.GetAllocator()), sendingDoc.GetAllocator());
			sendingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_GET_ENTITY_IDENTIFIER, sendingDoc.GetAllocator()), sendingDoc.GetAllocator());

			std::string response;
			sendHttpRequest(QUEUE, m_modelServiceURL, sendingDoc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				assert(0); // ERROR
			}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
		}

		std::list<std::string> takenNames = ot::json::getStringList(_document, OT_ACTION_PARAM_FILE_TAKEN_NAMES);
		std::string senderName = ot::json::getString(_document, OT_ACTION_PARAM_SENDER);
		std::string entityPath = ot::json::getString(_document, OT_ACTION_PARAM_NAME);
		std::string subsequentFunction = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_FunctionName);
		std::string senderURL = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);
		m_fileHandler.SetNewFileImportRequest(std::move(senderURL), std::move(subsequentFunction), std::move(senderName), std::move(takenNames), std::move(absoluteFilePaths), std::move(entityPath), entityType);
		}
	}
	catch (std::exception& e)
	{
		OT_LOG_E(e.what());
		AppBase::instance()->appendInfoMessage("Failed to load file due to: " + QString(e.what()));
	}

	return "";
}

std::string ExternalServicesComponent::handleSaveFileContent(ot::JsonDocument& _document) {
	std::string dialogTitle = ot::json::getString(_document, OT_ACTION_PARAM_UI_DIALOG_TITLE);
	std::string fileName = ot::json::getString(_document, OT_ACTION_PARAM_FILE_OriginalName);
	std::string fileContent = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Content);
	ot::UID uncompressedDataLength = ot::json::getUInt64(_document, OT_ACTION_PARAM_FILE_Content_UncompressedDataLength);

	// Decode the encoded string into binary data
	int decoded_compressed_data_length = Base64decode_len(fileContent.c_str());
	char* decodedCompressedString = new char[decoded_compressed_data_length];

	Base64decode(decodedCompressedString, fileContent.c_str());

	// Decompress the data
	char* decodedString = new char[uncompressedDataLength];
	uLongf destLen = (uLongf)uncompressedDataLength;
	uLong  sourceLen = decoded_compressed_data_length;
	uncompress((Bytef*)decodedString, &destLen, (Bytef*)decodedCompressedString, sourceLen);

	delete[] decodedCompressedString;
	decodedCompressedString = nullptr;

	// Store the data with the given file name

	std::ofstream file(fileName, std::ios::binary);
	file.write(decodedString, uncompressedDataLength);
	file.close();

	delete[] decodedString;
	decodedString = nullptr;

	// Show a success message
	ak::uiAPI::promptDialog::show(std::string("The file has been successfully saved:\n" + fileName).c_str(), dialogTitle.c_str(), ak::promptOkIconLeft, "DialogInformation", "Default", AppBase::instance()->mainWindow());
	
	return "";
}

std::string ExternalServicesComponent::handleSelectFilesForStoring(ot::JsonDocument& _document) {
	std::string dialogTitle = ot::json::getString(_document, OT_ACTION_PARAM_UI_DIALOG_TITLE);
	std::string fileMask = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Mask);
	std::string subsequentFunction = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_FunctionName);
	std::string senderURL = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);

	QString fileName = QFileDialog::getSaveFileName(
		nullptr,
		dialogTitle.c_str(),
		QDir::currentPath(),
		QString(fileMask.c_str()) + " ;; All files (*.*)");

	if (fileName != "")
	{
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteFunction, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_FILE_OriginalName, rapidjson::Value(fileName.toStdString().c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, rapidjson::Value(subsequentFunction.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());

		std::string response;

		sendHttpRequest(EXECUTE, senderURL, inDoc, response);

		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			assert(0); // ERROR
		}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		assert(0); // WARNING
		}
	}

	return "";
}

std::string ExternalServicesComponent::handleTableChange(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	const std::string functionName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_FunctionName);
	if (functionName == OT_ACTION_CMD_UI_VIEW_OBJ_Table_AddColumn)
	{
		bool insertLeft = ot::json::getBool(_document, OT_ACTION_PARAM_BASETYPE_Bool);
		ViewerAPI::AddToSelectedTableColumn(insertLeft, visualizationModelID);
	}
	else if (functionName == OT_ACTION_CMD_UI_VIEW_OBJ_Table_AddRow)
	{
		bool insertAbove = ot::json::getBool(_document, OT_ACTION_PARAM_BASETYPE_Bool);

		ViewerAPI::AddToSelectedTableRow(insertAbove, visualizationModelID);
	}
	else if (functionName == OT_ACTION_CMD_UI_VIEW_OBJ_Table_DeleteRow)
	{
		ViewerAPI::DeleteFromSelectedTableRow(visualizationModelID);
	}
	else
	{
		assert(functionName == OT_ACTION_CMD_UI_VIEW_OBJ_Table_DeleteColumn);
		ViewerAPI::DeleteFromSelectedTableColumn(visualizationModelID);
	}

	return "";
}

std::string ExternalServicesComponent::handleFillPropertyGrid(ot::JsonDocument& _document) {
	ot::PropertyGridCfg cfg;
	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);
	cfg.setFromJsonObject(cfgObj);
	AppBase::instance()->setupPropertyGrid(cfg);
	return "";
}

std::string ExternalServicesComponent::handleAddMenuPage(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

	ot::ServiceBase* service = getService(serviceId);

	ak::UID p = AppBase::instance()->getToolBar()->addPage(getServiceUiUid(service), pageName.c_str());
	//NOTE, add corresponding functions in uiServiceAPI
	ak::uiAPI::object::setObjectUniqueName(p, pageName.c_str());
	m_controlsManager->uiElementCreated(service, p, false);

	return "";
}

std::string ExternalServicesComponent::handleAddMenuGroup(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	std::string groupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_GroupName);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

	ot::ServiceBase* service = getService(serviceId);

	//NOTE, add corresponding functions in uiServiceAPI
	ak::UID pageUID{ ak::uiAPI::object::getUidFromObjectUniqueName(pageName.c_str()) };
	//NOTE, add error handling
	assert(pageUID != ak::invalidUID);

	ak::UID g = AppBase::instance()->getToolBar()->addGroup(getServiceUiUid(service), pageUID, groupName.c_str());
	//NOTE, add corresponding functions in uiServiceAPI
	ak::uiAPI::object::setObjectUniqueName(g, (pageName + ":" + groupName).c_str());
	m_controlsManager->uiElementCreated(service, g, false);

	return "";
}

std::string ExternalServicesComponent::handleAddMenuSubgroup(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	std::string groupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_GroupName);
	std::string subgroupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_SubgroupName);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	
	ot::ServiceBase* service = getService(serviceId);

	//NOTE, add corresponding functions in uiServiceAPI
	ak::UID groupUID{ ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName).c_str()) };
	//NOTE, add error handling
	assert(groupUID != ak::invalidUID);

	ak::UID sg = AppBase::instance()->getToolBar()->addSubGroup(getServiceUiUid(service), groupUID, subgroupName.c_str());
	//NOTE, add corresponding functions in uiServiceAPI
	ak::uiAPI::object::setObjectUniqueName(sg, (pageName + ":" + groupName + ":" + subgroupName).c_str());
	m_controlsManager->uiElementCreated(service, sg, false);

	return "";
}

std::string ExternalServicesComponent::handleAddMenuButton(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	std::string groupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_GroupName);
	std::string subgroupName = "";
	if (_document.HasMember(OT_ACTION_PARAM_UI_CONTROL_SubgroupName))
	{
		subgroupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_SubgroupName);
	}
	std::string buttonName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	std::string text = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectText);
	std::string iconName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_IconName);
	std::string iconFolder = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_IconFolder);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	ot::ContextMenu contextMenu("");
	if (_document.HasMember(OT_ACTION_PARAM_UI_CONTROL_ContextMenu)) {
		ot::ConstJsonObject contextMenuData = ot::json::getObject(_document, OT_ACTION_PARAM_UI_CONTROL_ContextMenu);
		contextMenu.setFromJsonObject(contextMenuData);
	}
	ot::ServiceBase* senderService = getService(serviceId);
	
	ot::Flags<ot::ui::lockType> flags = (ot::ui::lockType::tlAll);

	if (_document.HasMember(OT_ACTION_PARAM_ElementLockTypes)) {
		flags = ot::ui::toLockType(ot::json::getStringList(_document, OT_ACTION_PARAM_ElementLockTypes));
		flags.setFlag(ot::ui::lockType::tlAll);	// Add the all flag to all external push buttons
	}

	ak::UID parentUID;
	if (subgroupName.length() == 0) {
		parentUID = ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName).c_str());
	}
	else {
		parentUID = ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName + ":" + subgroupName).c_str());
	}
	//NOTE, add error handling
	assert(parentUID != ak::invalidUID);

	// Here we need to pass the iconName as string once the functionality is added to the uiManager
	ak::UID buttonID = AppBase::instance()->getToolBar()->addToolButton(getServiceUiUid(senderService), parentUID, iconName.c_str(), iconFolder.c_str(), text.c_str());

	if (subgroupName.length() == 0) {
		ak::uiAPI::object::setObjectUniqueName(buttonID, (pageName + ":" + groupName + ":" + buttonName).c_str());
	}
	else {
		ak::uiAPI::object::setObjectUniqueName(buttonID, (pageName + ":" + groupName + ":" + subgroupName + ":" + buttonName).c_str());
	}
	m_controlsManager->uiElementCreated(senderService, buttonID);
	m_lockManager->uiElementCreated(senderService, buttonID, flags);

	ak::uiAPI::registerUidNotifier(buttonID, this);

	if (buttonID != ak::invalidUID) {
		if (_document.HasMember(OT_ACTION_PARAM_UI_KeySequence)) {
			std::string keySquence = ot::json::getString(_document, OT_ACTION_PARAM_UI_KeySequence);
			if (keySquence.length() > 0) {
				KeyboardCommandHandler* newHandler = addShortcut(senderService, keySquence.c_str());
				if (newHandler) {
					newHandler->attachToEvent(buttonID, ak::etClicked);
					text.append(" (").append(keySquence).append(")");
					ak::uiAPI::toolButton::setToolTip(buttonID, text.c_str());
				}
			}
		}

		if (contextMenu.hasItems()) {
			AppBase::instance()->contextMenuManager()->createItem(senderService, getServiceUiUid(senderService), buttonID, contextMenu);
		}
	}

	return "";
}

std::string ExternalServicesComponent::handleAddMenuCheckbox(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	std::string groupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_GroupName);
	std::string subgroupName = ""; // Subgroup is optional
	if (_document.HasMember(OT_ACTION_PARAM_UI_CONTROL_SubgroupName)) {
		subgroupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_SubgroupName);
	}
	std::string boxName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	std::string boxText = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectText);
	bool checked = _document[OT_ACTION_PARAM_UI_CONTROL_CheckedState].GetBool();
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	auto flags = ot::ui::toLockType(ot::json::getStringList(_document, OT_ACTION_PARAM_ElementLockTypes));
	flags.setFlag(ot::ui::lockType::tlAll);	// Add the all flag to all external checkboxes

	ot::ServiceBase* service = getService(serviceId);

	//NOTE, add corresponding functions in uiServiceAPI
	ak::UID parentID;
	if (subgroupName.length() > 0) {
		parentID = ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName + ":" + subgroupName).c_str());
	}
	else {
		parentID = ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName).c_str());
	}

	//NOTE, add error handling
	assert(parentID != ak::invalidUID);
	if (parentID == ak::invalidUID) return "";

	// Here we need to pass the iconName as string once the functionality is added to the uiManager
	ak::UID boxID = AppBase::instance()->getToolBar()->addCheckBox(getServiceUiUid(service), parentID, boxText.c_str(), checked);

	//NOTE, add corresponding functions in uiServiceAPI
	if (subgroupName.length() > 0) {
		ak::uiAPI::object::setObjectUniqueName(boxID, (pageName + ":" + groupName + ":" + subgroupName + ":" + boxName).c_str());
	}
	else {
		ak::uiAPI::object::setObjectUniqueName(boxID, (pageName + ":" + groupName + ":" + boxName).c_str());
	}

	m_controlsManager->uiElementCreated(service, boxID);
	m_lockManager->uiElementCreated(service, boxID, flags);

	ak::uiAPI::registerUidNotifier(boxID, this);

	return "";
}

std::string ExternalServicesComponent::handleAddMenuLineEdit(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	std::string groupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_GroupName);
	std::string subgroupName = "";
	if (_document.HasMember(OT_ACTION_PARAM_UI_CONTROL_SubgroupName)) {
		subgroupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_SubgroupName);
	}
	std::string editName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	std::string editText = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectText);
	std::string editLabel = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectLabelText);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	auto flags = ot::ui::toLockType(ot::json::getStringList(_document, OT_ACTION_PARAM_ElementLockTypes));
	flags.setFlag(ot::ui::lockType::tlAll);	// Add the all flag to all external checkboxes

	ot::ServiceBase* service = getService(serviceId);
	
	ak::UID parentID;
	if (subgroupName.length() > 0) {
		parentID = ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName + ":" + subgroupName).c_str());
	}
	else {
		parentID = ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName).c_str());
	}

	//NOTE, add error handling
	assert(parentID != ak::invalidUID);

	// Here we need to pass the iconName as string once the functionality is added to the uiManager
	ak::UID editID = AppBase::instance()->getToolBar()->addNiceLineEdit(getServiceUiUid(service), parentID, editLabel.c_str(), editText.c_str());

	//NOTE, add corresponding functions in uiServiceAPI
	if (subgroupName.length() > 0) {
		ak::uiAPI::object::setObjectUniqueName(editID, (pageName + ":" + groupName + ":" + subgroupName + ":" + editName).c_str());
	}
	else {
		ak::uiAPI::object::setObjectUniqueName(editID, (pageName + ":" + groupName + ":" + editName).c_str());
	}
	m_controlsManager->uiElementCreated(service, editID);
	m_lockManager->uiElementCreated(service, editID, flags);

	ak::uiAPI::registerUidNotifier(editID, this);

	return "";
}

std::string ExternalServicesComponent::handleActivateToolbarTab(ot::JsonDocument& _document) {
	std::string tabName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_TabName);

	AppBase::instance()->activateToolBarTab(tabName.c_str());

	return "";
}

std::string ExternalServicesComponent::handleAddShortcut(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string keySequence = ot::json::getString(_document, OT_ACTION_PARAM_UI_KeySequence);

	ot::ServiceBase* service = getService(serviceId);

	this->addShortcut(service, keySequence);

	return "";
}

std::string ExternalServicesComponent::handleSetCheckboxValue(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string controlName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	bool checked = _document[OT_ACTION_PARAM_UI_CONTROL_CheckedState].GetBool();

	ak::UID objectID = ak::uiAPI::object::getUidFromObjectUniqueName(controlName.c_str());

	assert(objectID != ak::invalidUID);
	if (objectID == ak::invalidUID) return "";

	ak::uiAPI::checkBox::setChecked(objectID, checked);
	
	return "";
}

std::string ExternalServicesComponent::handleSetLineEditValue(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string controlName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	std::string editText = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectText);
	bool error = _document[OT_ACTION_PARAM_UI_CONTROL_ErrorState].GetBool();

	ak::UID objectID = ak::uiAPI::object::getUidFromObjectUniqueName(controlName.c_str());

	assert(objectID != ak::invalidUID);
	if (objectID == ak::invalidUID) return "";

	if (ak::uiAPI::object::type(objectID) == ak::otLineEdit) {
		ak::uiAPI::lineEdit::setText(objectID, editText.c_str());
		ak::uiAPI::lineEdit::setErrorState(objectID, error);
	}
	else if (ak::uiAPI::object::type(objectID) == ak::otNiceLineEdit) {
		ak::uiAPI::niceLineEdit::setText(objectID, editText.c_str());
		ak::uiAPI::niceLineEdit::setErrorState(objectID, error);
	}
	else {
		assert(0); // Invalid object type
	}
	
	return "";
}

std::string ExternalServicesComponent::handleSwitchMenuTab(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	AppBase::instance()->switchToTab(pageName);

	return "";
}

std::string ExternalServicesComponent::handleRemoveElements(ot::JsonDocument& _document) {
	std::list<std::string> itemList = ot::json::getStringList(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectNames);
	
	std::vector<ak::UID> uidList;
	for (auto itm : itemList) {
		uidList.push_back(ak::uiAPI::object::getUidFromObjectUniqueName(itm.c_str()));
	}
	m_controlsManager->destroyUiControls(uidList);

	for (auto itm : uidList) {
		m_lockManager->uiElementDestroyed(itm);
	}

	return "";
}

std::string ExternalServicesComponent::handleSetControlsEnabledState(ot::JsonDocument& _document) {
	std::list<std::string> enabled = ot::json::getStringList(_document, OT_ACTION_PARAM_UI_EnabledControlsList);
	std::list<std::string> disabled = ot::json::getStringList(_document, OT_ACTION_PARAM_UI_DisabledControlsList);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

	ot::ServiceBase* service = getService(serviceId);
	
	for (auto controlName : enabled)
	{
		//NOTE, add functionallity to uiServiceAPI
		auto uid = ak::uiAPI::object::getUidFromObjectUniqueName(controlName.c_str());
		if (uid != ak::invalidUID)
		{
			m_lockManager->enable(service, uid, true);
		}
	}

	for (auto controlName : disabled)
	{
		//NOTE, add functionallity to uiServiceAPI
		auto uid = ak::uiAPI::object::getUidFromObjectUniqueName(controlName.c_str());
		if (uid != ak::invalidUID)
		{
			m_lockManager->disable(service, uid);
		}
	}

	return "";
}

std::string ExternalServicesComponent::handleSetToolTip(ot::JsonDocument& _document) {
	std::string item = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	std::string text = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectText);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

	auto uid = ak::uiAPI::object::getUidFromObjectUniqueName(item.c_str());
	if (uid != ak::invalidUID)
	{
		ak::uiAPI::object::setToolTip(uid, text.c_str());
	}
	
	return "";
}

std::string ExternalServicesComponent::handleResetView(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	
	AppBase::instance()->getViewerComponent()->resetAllViews3D(visualizationModelID);
	
	return "";
}

std::string ExternalServicesComponent::handleRefreshView(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);

	AppBase::instance()->getViewerComponent()->refreshAllViews(visualizationModelID);
	
	return "";
}

std::string ExternalServicesComponent::handleClearSelection(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	
	AppBase::instance()->getViewerComponent()->clearSelection(visualizationModelID);
	
	return "";
}

std::string ExternalServicesComponent::handleRefreshSelection(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	
	AppBase::instance()->getViewerComponent()->refreshSelection(visualizationModelID);
	
	return "";
}

std::string ExternalServicesComponent::handleSelectObject(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ak::UID entityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	
	AppBase::instance()->getViewerComponent()->selectObject(visualizationModelID, entityID);
	
	return "";
}

std::string ExternalServicesComponent::handleAddNodeFromFacetData(ot::JsonDocument& _document) {
	ot::JsonSizeType tmp;

	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string treeName = ot::json::getString(_document, OT_ACTION_PARAM_UI_TREE_Name);
	double* surfaceColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, tmp);
	double* edgeColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, tmp);
	ak::UID modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	bool backFaceCulling = ot::json::getBool(_document, OT_ACTION_PARAM_MODEL_ITM_BACKFACE_Culling);
	double offsetFactor = _document[OT_ACTION_PARAM_MODEL_ITM_OffsetFactor].GetDouble();
	bool editable = _document[OT_ACTION_PARAM_MODEL_ITM_IsEditable].GetBool();
	bool selectChildren = _document[OT_ACTION_PARAM_MODEL_ITM_SelectChildren].GetBool();
	bool manageParentVisibility = _document[OT_ACTION_PARAM_MODEL_ITM_ManageParentVis].GetBool();
	bool manageChildVisibility = _document[OT_ACTION_PARAM_MODEL_ITM_ManageChildVis].GetBool();
	bool showWhenSelected = _document[OT_ACTION_PARAM_MODEL_ITM_ShowWhenSelected].GetBool();
	std::vector<Geometry::Node> nodes;
	getVectorNodeFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Nodes, nodes);
	std::list<Geometry::Triangle> triangles;
	getListTriangleFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Triangles, triangles);
	std::list<Geometry::Edge> edges;
	getListEdgeFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Edges, edges);
	std::string errors = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Errors);
	TreeIcon treeIcons = getTreeIconsFromDocument(_document);

	AppBase::instance()->getViewerComponent()->addNodeFromFacetData(visModelID, treeName, surfaceColorRGB, edgeColorRGB, modelEntityID, treeIcons, backFaceCulling,
		offsetFactor, editable, nodes, triangles, edges, errors, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected);
	
	return "";
}

std::string ExternalServicesComponent::handleAddNodeFromDataBase(ot::JsonDocument& _document) {
	ot::JsonSizeType tmp;

	ViewerUIDtype visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string treeName = ot::json::getString(_document, OT_ACTION_PARAM_UI_TREE_Name);
	double* surfaceColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, tmp);
	double* edgeColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, tmp);
	std::string materialType = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_MaterialType);
	std::string textureType = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_TextureType);
	bool reflective = _document[OT_ACTION_PARAM_MODEL_ITM_TextureReflective].GetBool();
	ModelUIDtype modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	bool backFaceCulling = _document[OT_ACTION_PARAM_MODEL_ITM_BACKFACE_Culling].GetBool();
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	double offsetFactor = _document[OT_ACTION_PARAM_MODEL_ITM_OffsetFactor].GetDouble();
	bool editable = _document[OT_ACTION_PARAM_MODEL_ITM_IsEditable].GetBool();
	bool selectChildren = _document[OT_ACTION_PARAM_MODEL_ITM_SelectChildren].GetBool();
	bool manageParentVisibility = _document[OT_ACTION_PARAM_MODEL_ITM_ManageParentVis].GetBool();
	bool manageChildVisibility = _document[OT_ACTION_PARAM_MODEL_ITM_ManageChildVis].GetBool();
	bool showWhenSelected = _document[OT_ACTION_PARAM_MODEL_ITM_ShowWhenSelected].GetBool();
	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);
	ModelUIDtype entityID = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	ModelUIDtype entityVersion = _document[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
	TreeIcon treeIcons = getTreeIconsFromDocument(_document);
	std::vector<double> transformation;
	try
	{
		transformation = ot::json::getDoubleVector(_document, OT_ACTION_PARAM_MODEL_ITM_Transformation);
	}
	catch (std::exception)
	{
		// There is no transformation attached. Set up a unity transform
		transformation.resize(16, 0.0);
		transformation[0] = transformation[5] = transformation[10] = transformation[15] = 1.0;
	}

	AppBase::instance()->getViewerComponent()->addNodeFromFacetDataBase(visModelID, treeName, surfaceColorRGB, edgeColorRGB, materialType, textureType, reflective,
		modelEntityID, treeIcons, backFaceCulling, offsetFactor, isHidden, editable, projectName, entityID,
		entityVersion, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected, transformation);
	
	return "";
}

std::string ExternalServicesComponent::handleAddContainerNode(ot::JsonDocument& _document) {
	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string treeName = ot::json::getString(_document, OT_ACTION_PARAM_UI_TREE_Name);
	ModelUIDtype modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	TreeIcon treeIcons = getTreeIconsFromDocument(_document);
	bool editable = _document[OT_ACTION_PARAM_MODEL_ITM_IsEditable].GetBool();

	AppBase::instance()->getViewerComponent()->addVisualizationContainerNode(visModelID, treeName, modelEntityID, treeIcons, editable);
	
	return "";
}

std::string ExternalServicesComponent::handleAddVis2D3DNode(ot::JsonDocument& _document) {
	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string treeName = ot::json::getString(_document, OT_ACTION_PARAM_UI_TREE_Name);
	ModelUIDtype modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	TreeIcon treeIcons = getTreeIconsFromDocument(_document);
	bool editable = _document[OT_ACTION_PARAM_MODEL_ITM_IsEditable].GetBool();
	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();

	ak::UID visualizationDataID = _document[OT_ACTION_PARAM_MODEL_DataID].GetUint64();
	ak::UID visualizationDataVersion = _document[OT_ACTION_PARAM_MODEL_DataVersion].GetUint64();

	AppBase::instance()->getViewerComponent()->addVisualizationVis2D3DNode(visModelID, treeName, modelEntityID, treeIcons, isHidden, editable, projectName, visualizationDataID, visualizationDataVersion);
	
	return "";
}

std::string ExternalServicesComponent::handleUpdateVis2D3DNode(ot::JsonDocument& _document) {
	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ModelUIDtype modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);

	ak::UID visualizationDataID = _document[OT_ACTION_PARAM_MODEL_DataID].GetUint64();
	ak::UID visualizationDataVersion = _document[OT_ACTION_PARAM_MODEL_DataVersion].GetUint64();

	AppBase::instance()->getViewerComponent()->updateVisualizationVis2D3DNode(visModelID, modelEntityID, projectName, visualizationDataID, visualizationDataVersion);
	
	return "";
}

std::string ExternalServicesComponent::handleUpdateColor(ot::JsonDocument& _document) {
	ot::JsonSizeType tmp;

	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ModelUIDtype modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	double* surfaceColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, tmp);
	double* edgeColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, tmp);
	std::string materialType = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_MaterialType);
	std::string textureType = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_TextureType);
	bool reflective = _document[OT_ACTION_PARAM_MODEL_ITM_TextureReflective].GetBool();
	
	AppBase::instance()->getViewerComponent()->updateObjectColor(visModelID, modelEntityID, surfaceColorRGB, edgeColorRGB, materialType, textureType, reflective);
	
	return "";
}

std::string ExternalServicesComponent::handleUpdateMeshColor(ot::JsonDocument& _document) {
	ot::JsonSizeType tmp;

	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ModelUIDtype modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	double* colorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, tmp);
	
	AppBase::instance()->getViewerComponent()->updateMeshColor(visModelID, modelEntityID, colorRGB);

	return "";
}

std::string ExternalServicesComponent::handleUpdateFacetsFromDataBase(ot::JsonDocument& _document) {
	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ModelUIDtype modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	ModelUIDtype entityID = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	ModelUIDtype entityVersion = _document[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();

	AppBase::instance()->getViewerComponent()->updateObjectFacetsFromDataBase(visModelID, modelEntityID, entityID, entityVersion);
	
	return "";
}

std::string ExternalServicesComponent::handleRemoveShapes(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::list<ak::UID> entityID = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_ITM_ID);
	
	AppBase::instance()->getViewerComponent()->removeShapes(visualizationModelID, entityID);

	//If entity is has a block item associated, it gets removed from all editors.
	std::list<ot::GraphicsViewView*> views = AppBase::instance()->getAllGraphicsEditors();
	for (auto view : views) {
		for (auto uid : entityID) {
			view->removeItem(uid);
			view->removeConnection(uid);
		}
	}
	
	std::list<std::string> curveNames = ViewerAPI::getSelectedCurves(visualizationModelID);
	ViewerAPI::removeSelectedCurveNodes(visualizationModelID);

	ot::JsonDocument requestDoc;
	ot::JsonArray jCurveNames;
	for (auto& curveName : curveNames)
	{
		jCurveNames.PushBack(ot::JsonString(curveName, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	}
	requestDoc.AddMember(OT_ACTION_PARAM_VIEW1D_CurveNames, jCurveNames, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_DeleteCurvesFromPlots, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	
	std::string response;
	sendHttpRequest(QUEUE, m_modelServiceURL, requestDoc, response);

	OT_ACTION_IF_RESPONSE_ERROR(response) {
		OT_LOG_EAS(response);
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response)
	{
		OT_LOG_WAS(response);
	}

	return "";
}

std::string ExternalServicesComponent::handleTreeStateRecording(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	bool flag = _document[OT_ACTION_PARAM_MODEL_State].GetBool();

	AppBase::instance()->getViewerComponent()->setTreeStateRecording(visualizationModelID, flag);

	return "";
}

std::string ExternalServicesComponent::handleSetShapeVisibility(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::list<ak::UID> visibleID = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_ITM_ID_Visible);
	std::list<ak::UID> hiddenID = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_ITM_ID_Hidden);
	
	AppBase::instance()->getViewerComponent()->setShapeVisibility(visualizationModelID, visibleID, hiddenID);
	
	return "";
}

std::string ExternalServicesComponent::handleHideEntities(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::list<ak::UID> hiddenID = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_ITM_ID_Hidden);

	AppBase::instance()->getViewerComponent()->hideEntities(visualizationModelID, hiddenID);
	
	return "";
}

std::string ExternalServicesComponent::handleShowBranch(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string branchName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_BRANCH);

	AppBase::instance()->getViewerComponent()->showBranch(visualizationModelID, branchName);
	
	return "";
}

std::string ExternalServicesComponent::handleHideBranch(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string branchName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_BRANCH);

	AppBase::instance()->getViewerComponent()->hideBranch(visualizationModelID, branchName);
	
	return "";
}

std::string ExternalServicesComponent::handleAddAnnotationNode(ot::JsonDocument& _document) {
	ot::JsonSizeType tmp;

	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	ak::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	double* edgeColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, tmp);
	std::vector<std::array<double, 3>> points = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points);
	std::vector<std::array<double, 3>> points_rgb = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points_Colors);
	std::vector<std::array<double, 3>> triangle_p1 = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p1);
	std::vector<std::array<double, 3>> triangle_p2 = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p2);
	std::vector<std::array<double, 3>> triangle_p3 = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p3);
	std::vector<std::array<double, 3>> triangle_rgb = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_Color);
	TreeIcon treeIcons = getTreeIconsFromDocument(_document);

	AppBase::instance()->getViewerComponent()->addVisualizationAnnotationNode(visModelID, name, uid, treeIcons, isHidden, edgeColorRGB, points, points_rgb, triangle_p1, triangle_p2, triangle_p3, triangle_rgb);
	
	return "";
}

std::string ExternalServicesComponent::handleAddAnnotationNodeFromDataBase(ot::JsonDocument& _document) {
	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	ak::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);
	ak::UID entityID = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	ak::UID entityVersion = _document[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
	TreeIcon treeIcons = getTreeIconsFromDocument(_document);

	ViewerAPI::addVisualizationAnnotationNodeDataBase(visModelID, name, uid, treeIcons, isHidden, projectName, entityID, entityVersion);

	return "";
}

std::string ExternalServicesComponent::handleAddMeshNodeFromFacetDataBase(ot::JsonDocument& _document) {
	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	ak::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	double edgeColorRGB[3];
	edgeColorRGB[0] = _document[OT_ACTION_PARAM_MODEL_EDGE_COLOR_R].GetDouble();
	edgeColorRGB[1] = _document[OT_ACTION_PARAM_MODEL_EDGE_COLOR_G].GetDouble();
	edgeColorRGB[2] = _document[OT_ACTION_PARAM_MODEL_EDGE_COLOR_B].GetDouble();
	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);
	ak::UID entityID = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	ak::UID entityVersion = _document[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
	bool displayTetEdges = _document[OT_ACTION_PARAM_MODEL_TET_DISPLAYEDGES].GetBool();
	TreeIcon treeIcons = getTreeIconsFromDocument(_document);

	ViewerAPI::addVisualizationMeshNodeFromFacetDataBase(visModelID, name, uid, treeIcons, edgeColorRGB, displayTetEdges, projectName, entityID, entityVersion);
	
	return "";
}

std::string ExternalServicesComponent::handleAddCartesianMeshNode(ot::JsonDocument& _document) {
	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	ak::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	double edgeColorRGB[3];
	edgeColorRGB[0] = _document[OT_ACTION_PARAM_MODEL_EDGE_COLOR_R].GetDouble();
	edgeColorRGB[1] = _document[OT_ACTION_PARAM_MODEL_EDGE_COLOR_G].GetDouble();
	edgeColorRGB[2] = _document[OT_ACTION_PARAM_MODEL_EDGE_COLOR_B].GetDouble();
	double meshLineColorRGB[3];
	meshLineColorRGB[0] = _document[OT_ACTION_PARAM_MODEL_MESHLINE_COLOR_R].GetDouble();
	meshLineColorRGB[1] = _document[OT_ACTION_PARAM_MODEL_MESHLINE_COLOR_G].GetDouble();
	meshLineColorRGB[2] = _document[OT_ACTION_PARAM_MODEL_MESHLINE_COLOR_B].GetDouble();
	std::vector<double> meshCoordsX = ot::json::getDoubleVector(_document, OT_ACTION_PARAM_MESH_CartesianCoordX);
	std::vector<double> meshCoordsY = ot::json::getDoubleVector(_document, OT_ACTION_PARAM_MESH_CartesianCoordY);
	std::vector<double> meshCoordsZ = ot::json::getDoubleVector(_document, OT_ACTION_PARAM_MESH_CartesianCoordZ);
	bool showMeshLines = _document[OT_ACTION_PARAM_MESH_ShowMeshLines].GetBool();
	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);
	ak::UID faceListEntityID = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	ak::UID faceListEntityVersion = _document[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
	ak::UID nodeListEntityID = _document[OT_ACTION_PARAM_MESH_NODE_ID].GetUint64();
	ak::UID nodeListEntityVersion = _document[OT_ACTION_PARAM_MESH_NODE_VERSION].GetUint64();
	TreeIcon treeIcons = getTreeIconsFromDocument(_document);

	ViewerAPI::addVisualizationCartesianMeshNode(visModelID, name, uid, treeIcons, isHidden, edgeColorRGB, meshLineColorRGB, showMeshLines, meshCoordsX, meshCoordsY, meshCoordsZ, projectName, faceListEntityID, faceListEntityVersion, nodeListEntityID, nodeListEntityVersion);
	
	return "";
}

std::string ExternalServicesComponent::handleCartesianMeshNodeShowLines(ot::JsonDocument& _document) {
	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ak::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	bool showMeshLines = _document[OT_ACTION_PARAM_MESH_ShowMeshLines].GetBool();

	ViewerAPI::visualizationCartesianMeshNodeShowLines(visModelID, uid, showMeshLines);
	
	return "";
}

std::string ExternalServicesComponent::handleAddCartesianMeshItem(ot::JsonDocument& _document) {
	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	ak::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	std::vector<int> facesList = ot::json::getIntVector(_document, OT_ACTION_PARAM_MODEL_ITM_FacesList);
	TreeIcon treeIcons = getTreeIconsFromDocument(_document);
	double colorRGB[3];
	colorRGB[0] = _document[OT_ACTION_PARAM_MESH_ITEM_COLOR_R].GetDouble();
	colorRGB[1] = _document[OT_ACTION_PARAM_MESH_ITEM_COLOR_G].GetDouble();
	colorRGB[2] = _document[OT_ACTION_PARAM_MESH_ITEM_COLOR_B].GetDouble();

	ViewerAPI::addVisualizationCartesianMeshItemNode(visModelID, name, uid, treeIcons, isHidden, facesList, colorRGB);

	return "";
}

std::string ExternalServicesComponent::handleTetMeshNodeTetEdges(ot::JsonDocument& _document) {
	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ak::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	bool displayMeshEdges = _document[OT_ACTION_PARAM_MODEL_TET_DISPLAYEDGES].GetBool();
	
	ViewerAPI::visualizationTetMeshNodeTetEdges(visModelID, uid, displayMeshEdges);

	return "";
}

std::string ExternalServicesComponent::handleAddMeshItemFromFacetDatabase(ot::JsonDocument& _document) {
	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	ak::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);
	ak::UID entityID = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	ak::UID entityVersion = _document[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
	TreeIcon treeIcons = getTreeIconsFromDocument(_document);
	long long tetEdgesID = _document[OT_ACTION_PARAM_MODEL_TETEDGES_ID].GetUint64();;
	long long tetEdgesVersion = _document[OT_ACTION_PARAM_MODEL_TETEDGES_Version].GetUint64();

	ViewerAPI::addVisualizationMeshItemNodeFromFacetDataBase(visModelID, name, uid, treeIcons, isHidden, projectName, entityID, entityVersion, tetEdgesID, tetEdgesVersion);
	
	return "";
}

std::string ExternalServicesComponent::handleAddText(ot::JsonDocument& _document) {
	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	ak::UID uid = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	bool isEditable = _document[OT_ACTION_PARAM_MODEL_ITM_IsEditable].GetBool();

	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);
	ak::UID textID = _document[OT_ACTION_PARAM_TEXT_ID].GetUint64();
	ak::UID textVersion = _document[OT_ACTION_PARAM_TEXT_VERSION].GetUint64();

	TreeIcon treeIcons = getTreeIconsFromDocument(_document);

	ViewerAPI::addVisualizationTextNode(visModelID, name, uid, treeIcons, isHidden, projectName, textID, textVersion);

	return "";
}

std::string ExternalServicesComponent::handleAddTable(ot::JsonDocument& _document) {
	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	ak::UID uid = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	bool isEditable = _document[OT_ACTION_PARAM_MODEL_ITM_IsEditable].GetBool();

	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);
	ak::UID tableID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	ak::UID tableVersion = _document[OT_ACTION_PARAM_MODEL_EntityVersion].GetUint64();

	TreeIcon treeIcons = getTreeIconsFromDocument(_document);

	ViewerAPI::addVisualizationTableNode(visModelID, name, uid, treeIcons, isHidden, projectName, tableID, tableVersion);

	return "";
}

std::string ExternalServicesComponent::handleGetTableSelection(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string senderURL = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);
	std::string subsequentFunction = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_FunctionName);
	ot::ConstJsonObject serializedColor = ot::json::getObject(_document, OT_ACTION_PARAM_COLOUR_BACKGROUND);

	try {
		ot::Color colour;
		colour.setFromJsonObject(serializedColor);
		std::pair<ot::UID, ot::UID> activeTableIdentifyer = ViewerAPI::GetActiveTableIdentifyer(visualizationModelID);

		if (activeTableIdentifyer.first != -1)
		{
			SetColourOfSelectedRange(visualizationModelID, colour);
			RequestTableSelection(visualizationModelID, senderURL, subsequentFunction);
		}

	}
	catch (std::exception& e)
	{
		OT_LOG_E(e.what());
		AppBase::instance()->appendInfoMessage("Table selection request could not be handled due to exception: " + QString(e.what()));
	}

	return "";
}

std::string ExternalServicesComponent::handleShowTable(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ak::UID tableEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	ak::UID tableEntityVersion = _document[OT_ACTION_PARAM_MODEL_EntityVersion].GetUint64();
	std::string senderURL = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);
	std::string subsequentFunction = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_FunctionName);
	try
	{
		bool refreshColouring = ViewerAPI::setTable(visualizationModelID, tableEntityID, tableEntityVersion);

		if (refreshColouring && subsequentFunction != "")
		{
			std::string tableName = ViewerAPI::getTableName(visualizationModelID);

			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteFunction, doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(subsequentFunction, doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_MODEL_EntityName, ot::JsonString(tableName, doc.GetAllocator()), doc.GetAllocator());

			std::string response;
			sendHttpRequest(EXECUTE, senderURL, doc, response);
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				OT_LOG_E(response);
				AppBase::instance()->appendInfoMessage("Response while refreshing colour was false");
			}
		else OT_ACTION_IF_RESPONSE_WARNING(response)
		{
			OT_LOG_W(response);
			AppBase::instance()->appendInfoMessage("Response while refreshing colour was false");
		}
		}
	}
	catch (std::exception& e)
	{
		OT_LOG_E(e.what());
		AppBase::instance()->appendInfoMessage("Table could not be shown due to exception: " + QString(e.what()));
	}

	return "";
}

std::string ExternalServicesComponent::handleSetTable(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	auto tableContent = ot::json::getObject(_document, OT_ACTION_PARAM_Value);
	ot::GenericDataStructMatrix data;
	data.setFromJsonObject(tableContent);
	ViewerAPI::showTable(visualizationModelID, data);

	return "";
}

std::string ExternalServicesComponent::handleSelectRanges(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();

	auto listOfSerializedRanges = ot::json::getObjectList(_document, "Ranges");
	std::vector<ot::TableRange> ranges;
	ranges.reserve(listOfSerializedRanges.size());
	for (auto range : listOfSerializedRanges)
	{
		ot::TableRange tableRange;
		tableRange.setFromJsonObject(range);
		ranges.push_back(tableRange);
	}
	try
	{
		ViewerAPI::setTableSelection(visualizationModelID, ranges);
	}
	catch (std::exception& e)
	{
		OT_LOG_E(e.what());
		AppBase::instance()->appendInfoMessage("Table selection could not be executed due to exception: " + QString(e.what()));
	}

	return "";
}

std::string ExternalServicesComponent::handleColorSelection(ot::JsonDocument& _document) {
	ak::UID visualizationModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	ot::ConstJsonObject serializedColor = ot::json::getObject(_document, OT_ACTION_PARAM_COLOUR_BACKGROUND);

	try
	{
		ot::Color colour;
		colour.setFromJsonObject(serializedColor);
		ViewerAPI::ChangeColourOfSelection(visualizationModelID, colour);
	}
	catch (std::exception& e)
	{
		OT_LOG_E(e.what());
		AppBase::instance()->appendInfoMessage("Setting color of table selection could not be executed due to exception: " + QString(e.what()));
	}

	return "";
}

std::string ExternalServicesComponent::handleAddPlot1D(ot::JsonDocument& _document) {
	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	ak::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);

	int gridColor[3] = { 0, 0, 0 };
	std::string title = ot::json::getString(_document, OT_ACTION_PARAM_VIEW1D_Title);
	std::string plotType = ot::json::getString(_document, OT_ACTION_PARAM_VIEW1D_PlotType);
	std::string plotQuantity = ot::json::getString(_document, OT_ACTION_PARAM_VIEW1D_PlotQuantity);
	bool grid = _document[OT_ACTION_PARAM_VIEW1D_Grid].GetBool();
	bool legend = _document[OT_ACTION_PARAM_VIEW1D_Legend].GetBool();
	bool logscaleX = _document[OT_ACTION_PARAM_VIEW1D_LogscaleX].GetBool();
	bool logscaleY = _document[OT_ACTION_PARAM_VIEW1D_LogscaleY].GetBool();
	bool autoscaleX = _document[OT_ACTION_PARAM_VIEW1D_AutoscaleX].GetBool();
	bool autoscaleY = _document[OT_ACTION_PARAM_VIEW1D_AutoscaleY].GetBool();
	double xmin = _document[OT_ACTION_PARAM_VIEW1D_Xmin].GetDouble();
	double xmax = _document[OT_ACTION_PARAM_VIEW1D_Xmax].GetDouble();
	double ymin = _document[OT_ACTION_PARAM_VIEW1D_Ymin].GetDouble();
	double ymax = _document[OT_ACTION_PARAM_VIEW1D_Ymax].GetDouble();
	gridColor[0] = _document[OT_ACTION_PARAM_VIEW1D_GridColorR].GetInt();
	gridColor[1] = _document[OT_ACTION_PARAM_VIEW1D_GridColorG].GetInt();
	gridColor[2] = _document[OT_ACTION_PARAM_VIEW1D_GridColorB].GetInt();

	std::list<ak::UID> curvesID = ot::json::getUInt64List(_document, OT_ACTION_PARAM_VIEW1D_CurveIDs);
	std::list<ak::UID> curvesVersions = ot::json::getUInt64List(_document, OT_ACTION_PARAM_VIEW1D_CurveVersions);
	std::list<std::string> curvesNames = ot::json::getStringList(_document, OT_ACTION_PARAM_VIEW1D_CurveNames);

	TreeIcon treeIcons = getTreeIconsFromDocument(_document);

	ViewerAPI::addVisualizationPlot1DNode(visModelID, name, uid, treeIcons, isHidden, projectName, title, plotType, plotQuantity, grid, gridColor, legend, logscaleX, logscaleY, autoscaleX, autoscaleY, xmin, xmax, ymin, ymax, curvesID, curvesVersions, curvesNames);

	return "";
}

std::string ExternalServicesComponent::handlePlot1DPropertiesChanged(ot::JsonDocument& _document) {
	int gridColor[3] = { 0, 0, 0 };
	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ak::UID uid = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	std::string title = ot::json::getString(_document, OT_ACTION_PARAM_VIEW1D_Title);
	std::string plotType = ot::json::getString(_document, OT_ACTION_PARAM_VIEW1D_PlotType);
	std::string plotQuantity = ot::json::getString(_document, OT_ACTION_PARAM_VIEW1D_PlotQuantity);
	bool grid = _document[OT_ACTION_PARAM_VIEW1D_Grid].GetBool();
	bool legend = _document[OT_ACTION_PARAM_VIEW1D_Legend].GetBool();
	bool logscaleX = _document[OT_ACTION_PARAM_VIEW1D_LogscaleX].GetBool();
	bool logscaleY = _document[OT_ACTION_PARAM_VIEW1D_LogscaleY].GetBool();
	bool autoscaleX = _document[OT_ACTION_PARAM_VIEW1D_AutoscaleX].GetBool();
	bool autoscaleY = _document[OT_ACTION_PARAM_VIEW1D_AutoscaleY].GetBool();
	double xmin = _document[OT_ACTION_PARAM_VIEW1D_Xmin].GetDouble();
	double xmax = _document[OT_ACTION_PARAM_VIEW1D_Xmax].GetDouble();
	double ymin = _document[OT_ACTION_PARAM_VIEW1D_Ymin].GetDouble();
	double ymax = _document[OT_ACTION_PARAM_VIEW1D_Ymax].GetDouble();
	gridColor[0] = _document[OT_ACTION_PARAM_VIEW1D_GridColorR].GetInt();
	gridColor[1] = _document[OT_ACTION_PARAM_VIEW1D_GridColorG].GetInt();
	gridColor[2] = _document[OT_ACTION_PARAM_VIEW1D_GridColorB].GetInt();

	ViewerAPI::visualizationPlot1DPropertiesChanged(visModelID, uid, title, plotType, plotQuantity, grid, gridColor, legend, logscaleX, logscaleY, autoscaleX, autoscaleY, xmin, xmax, ymin, ymax);

	return "";
}

std::string ExternalServicesComponent::handleResult1DPropertiesChanged(ot::JsonDocument& _document) {
	ak::UID visModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	ot::UIDList entityIDs = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_ITM_ID);
	ot::UIDList entityVersions = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_ITM_Version);
	auto entityVersion = entityVersions.begin();
	for (ot::UID entityID : entityIDs)
	{
		OTAssert(entityVersion != entityVersions.end(), "List size mismatch");

		ViewerAPI::visualizationResult1DPropertiesChanged(visModelID, entityID, *entityVersion);
		entityVersion++;
	}

	return "";
}

std::string ExternalServicesComponent::handleEnterEntitySelectionMode(ot::JsonDocument& _document) {
	ak::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string selectionType = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_SelectionType);
	bool allowMultipleSelection = _document[OT_ACTION_PARAM_MODEL_ITM_Selection_AllowMultipleSelection].GetBool();
	std::string selectionFilter = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Selection_Filter);
	std::string selectionAction = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Selection_Action);
	std::string selectionMessage = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Selection_Message);
	ot::serviceID_t replyToService = ot::json::getUInt(_document, OT_ACTION_PARAM_MODEL_REPLYTO);
	std::list<std::string> optionNames = ot::json::getStringList(_document, OT_ACTION_PARAM_MODEL_ITM_Selection_OptNames);
	std::list<std::string> optionValues = ot::json::getStringList(_document, OT_ACTION_PARAM_MODEL_ITM_Selection_OptValues);

	AppBase::instance()->getViewerComponent()->enterEntitySelectionMode(visModelID, replyToService, selectionType, allowMultipleSelection, selectionFilter, selectionAction, selectionMessage, optionNames, optionValues);

	return "";
}

std::string ExternalServicesComponent::handleSetModifiedState(ot::JsonDocument& _document) {
	ak::UID visModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	bool modifiedState = ot::json::getBool(_document, OT_ACTION_PARAM_MODEL_ModifiedState);

	AppBase::instance()->getViewerComponent()->isModified(visModelID, modifiedState);

	return "";
}

std::string ExternalServicesComponent::handleSetProgressVisibility(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	bool visible = ot::json::getBool(_document, OT_ACTION_PARAM_UI_CONTROL_VisibleState);
	bool continuous = ot::json::getBool(_document, OT_ACTION_PARAM_UI_CONTROL_ContinuousState);

	AppBase* app = AppBase::instance();
	assert(app != nullptr);
	if (app != nullptr) app->setProgressBarVisibility(message.c_str(), visible, continuous);

	return "";
}

std::string ExternalServicesComponent::handleSetProgressValue(ot::JsonDocument& _document) {
	int percentage = ot::json::getInt(_document, OT_ACTION_PARAM_PERCENT);

	AppBase* app = AppBase::instance();
	assert(app != nullptr);
	if (app != nullptr) app->setProgressBarValue(percentage);

	return "";
}

std::string ExternalServicesComponent::handleFreeze3DView(ot::JsonDocument& _document) {
	ak::UID visModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	bool flag = ot::json::getBool(_document, OT_ACTION_PARAM_UI_CONTROL_BOOLEAN_STATE);

	AppBase::instance()->getViewerComponent()->freeze3DView(visModelID, flag);

	return "";
}

std::string ExternalServicesComponent::handleCreateModel(ot::JsonDocument& _document) {
	AppBase* app = AppBase::instance();

	// Create a model and a view
	ModelUIDtype modelID = app->createModel();
	ViewerUIDtype viewID = app->createView(modelID, app->getCurrentProjectName());
	app->getViewerComponent()->activateModel(modelID);

	auto service = m_serviceIdMap.find(ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID));
	if (service == m_serviceIdMap.end()) { throw std::exception("Sender service was not registered"); }

	// Write data to JSON string
	ot::JsonDocument docOut;
	docOut.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_SetVisualizationModel, docOut.GetAllocator()), docOut.GetAllocator());
	docOut.AddMember(OT_ACTION_PARAM_MODEL_ID, modelID, docOut.GetAllocator());
	docOut.AddMember(OT_ACTION_PARAM_VIEW_ID, viewID, docOut.GetAllocator());

	std::string response;
	if (!sendHttpRequest(QUEUE, service->second->serviceURL(), docOut, response)) {
		throw std::exception("Failed to send http request");
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::string ex(OT_ACTION_RETURN_INDICATOR_Error "From ");
		ex.append(service->second->serviceURL()).append(": ").append(response);
		throw std::exception(ex.c_str());
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::string ex(OT_ACTION_RETURN_INDICATOR_Error "From ");
		ex.append(service->second->serviceURL()).append(": ").append(response);
		throw std::exception(ex.c_str());
	}

	app->switchToViewTab();
	app->lockWelcomeScreen(false);

	return "";
}

std::string ExternalServicesComponent::handleCreateView(ot::JsonDocument& _document) {
	auto manager = AppBase::instance();

	ot::JsonDocument docOut;
	docOut.SetObject();
	docOut.AddMember(OT_ACTION_PARAM_VIEW_ID, manager->createView(ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID),
		AppBase::instance()->getCurrentProjectName()), docOut.GetAllocator());

	return docOut.toJson();

	return "";
}

std::string ExternalServicesComponent::handleSetEntityName(ot::JsonDocument& _document) {
	ak::UID entityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ITM_ID);
	std::string entityName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Name);

	ViewerAPI::setEntityName(entityID, entityName);

	return "";
}

std::string ExternalServicesComponent::handleRenameEntity(ot::JsonDocument& _document) {
	std::string fromPath = ot::json::getString(_document, OT_ACTION_PARAM_PATH_FROM);
	std::string toPath = ot::json::getString(_document, OT_ACTION_PARAM_PATH_To);

	ViewerAPI::renameEntityPath(fromPath, toPath);

	return "";
}

std::string ExternalServicesComponent::handleCreateRubberband(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string note = ot::json::getString(_document, OT_ACTION_PARAM_VIEW_RUBBERBAND_Note);
	std::string cfg = ot::json::getString(_document, OT_ACTION_PARAM_VIEW_RUBBERBAND_Document);

	ViewerAPI::createRubberband(AppBase::instance()->getViewerComponent()->getActiveViewerModel(), serviceId, note, cfg);

	return "";
}

std::string ExternalServicesComponent::handleLock(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	auto flags = ot::ui::toLockType(ot::json::getStringList(_document, OT_ACTION_PARAM_ElementLockTypes));
	m_lockManager->lock(getService(serviceId), flags);

	return "";
}

std::string ExternalServicesComponent::handleUnlock(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	auto flags = ot::ui::toLockType(ot::json::getStringList(_document, OT_ACTION_PARAM_ElementLockTypes));
	m_lockManager->unlock(getService(serviceId), flags);

	return "";
}

std::string ExternalServicesComponent::handleAddSettingsData(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	ot::ServiceBase* service = getService(serviceId);
	if (service) {
		UserSettings::instance()->addFromService(service, _document);
	}
	else {
		AppBase::instance()->appendInfoMessage("[ERROR] Dispatch: " OT_ACTION_CMD_UI_AddSettingsData ": Service not registered");

	}

	return "";
}

std::string ExternalServicesComponent::handleAddIconSearchPath(ot::JsonDocument& _document) {
#ifdef _DEBUG
	std::string iconPath = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_IconFolder);
	try {
		ot::IconManager::instance().addSearchPath(QString::fromStdString(iconPath));
		AppBase::instance()->appendDebugMessage("[ERROR] Added icon search path: " + QString::fromStdString(iconPath));
	}
	catch (...) {
		AppBase::instance()->appendInfoMessage("[ERROR] Failed to add icon search path: Path not found");
	}
#endif // _DEBUG

	return "";
}

std::string ExternalServicesComponent::handleSetVersionGraph(ot::JsonDocument& _document) {
	ak::UID visModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	std::list<std::string> versionList = ot::json::getStringList(_document, OT_ACTION_PARAM_UI_GRAPH_VERSION);
	std::list<std::string> parentList = ot::json::getStringList(_document, OT_ACTION_PARAM_UI_GRAPH_PARENT);
	std::list<std::string> descriptionList = ot::json::getStringList(_document, OT_ACTION_PARAM_UI_GRAPH_DESCRIPTION);
	std::string activeVersion = ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_ACTIVE);
	std::string activeBranch = ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_BRANCH);

	std::list<std::tuple<std::string, std::string, std::string>> versionGraph;

	auto description = descriptionList.begin();
	auto parent = parentList.begin();

	for (auto version : versionList)
	{
		versionGraph.push_back(std::tuple<std::string, std::string, std::string>(version, *parent, *description));
		parent++;
		description++;
	}

	ViewerAPI::setVersionGraph(visModelID, versionGraph, activeVersion, activeBranch);

	return "";
}

std::string ExternalServicesComponent::handleSetVersionGraphActive(ot::JsonDocument& _document) {
	ak::UID visModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	std::string activeVersion = ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_ACTIVE);
	std::string activeBranch = ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_BRANCH);

	ViewerAPI::setVersionGraphActive(visModelID, activeVersion, activeBranch);

	return "";
}

std::string ExternalServicesComponent::handleRemoveVersionGraphVersions(ot::JsonDocument& _document) {
	ak::UID visModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	std::list<std::string> versions = ot::json::getStringList(_document, OT_ACTION_PARAM_UI_GRAPH_VERSION);

	ViewerAPI::removeVersionGraphVersions(visModelID, versions);

	return "";
}

std::string ExternalServicesComponent::handleAddAndActivateVersionGraphVersion(ot::JsonDocument& _document) {
	ak::UID visModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	std::string newVersion = ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_VERSION);
	std::string parentVersion = ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_PARENT);
	std::string description = ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_DESCRIPTION);
	std::string activeBranch = ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_BRANCH);

	ViewerAPI::addNewVersionGraphStateAndActivate(visModelID, newVersion, activeBranch, parentVersion, description);

	return "";
}

// Graphics Editor	

std::string ExternalServicesComponent::handleFillGraphicsPicker(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	ot::GraphicsPickerCollectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	AppBase::instance()->globalGraphicsPicker()->add(pckg);

	return "";
}

std::string ExternalServicesComponent::handleCreateGraphicsEditor(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	ot::GraphicsNewEditorPackage pckg("", "");
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	AppBase::instance()->findOrCreateGraphicsEditor(pckg.name(), QString::fromStdString(pckg.title()), info);

	AppBase::instance()->globalGraphicsPicker()->add(pckg);

	return "";
}

std::string ExternalServicesComponent::handleAddGraphicsItem(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	ot::GraphicsScenePackage pckg("");
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	ot::GraphicsView* editor = AppBase::instance()->findOrCreateGraphicsEditor(pckg.name(), QString::fromStdString(pckg.name()), info);

	for (auto graphicsItemCfg : pckg.items()) {
		ot::GraphicsItem* graphicsItem = ot::GraphicsFactory::itemFromConfig(graphicsItemCfg, true);
		if (graphicsItem != nullptr) {
			const double xCoordinate = graphicsItemCfg->position().x();
			const double yCoordinate = graphicsItemCfg->position().y();
			graphicsItem->getQGraphicsItem()->setPos(QPointF(xCoordinate, yCoordinate));
			editor->addItem(graphicsItem);
		}
	}

	return "";
}

std::string ExternalServicesComponent::handleRemoveGraphicsItem(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	ot::UIDList itemUids = ot::json::getUInt64List(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemIds);

	if (_document.HasMember(OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName)) {
		// Specific view

		std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);
		ot::GraphicsView* editor = AppBase::instance()->findOrCreateGraphicsEditor(editorName, QString::fromStdString(editorName), info);

		if (editor) {
			for (auto itemUID : itemUids) {
				editor->removeItem(itemUID);
			}
		}
	}
	else {
		// Any view

		std::list<ot::GraphicsViewView*> views = AppBase::instance()->getAllGraphicsEditors();
		for (auto v : views) {
			for (auto uid : itemUids) {
				v->removeItem(uid);
			}
		}
	}

	return "";
}

std::string ExternalServicesComponent::handleAddGraphicsConnection(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	ot::GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	ot::GraphicsView* editor = AppBase::instance()->findOrCreateGraphicsEditor(pckg.name(), QString::fromStdString(pckg.name()), info);

	for (const auto& connection : pckg.connections()) {
		editor->addConnectionIfConnectedItemsExist(connection);
	}

	return "";
}

std::string ExternalServicesComponent::handleRemoveGraphicsConnection(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	ot::GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	if (!pckg.name().empty()) {
		// Specific editor

		ot::GraphicsView* editor = AppBase::instance()->findOrCreateGraphicsEditor(pckg.name(), QString::fromStdString(pckg.name()), info);

		for (auto connection : pckg.connections()) {
			editor->removeConnection(connection.getUid());
		}
	}
	else {
		// Any editor

		std::list<ot::GraphicsViewView*> views = AppBase::instance()->getAllGraphicsEditors();
		for (auto view : views) {
			for (auto connection : pckg.connections()) {
				view->removeConnection(connection.getUid());
			}
		}
	}

	return "";
}

// Text Editor

std::string ExternalServicesComponent::handleSetTextEditorText(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Name);
	std::string editorText = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Text);

	std::string editorTitle = editorName;
	if (_document.HasMember(OT_ACTION_PARAM_TEXTEDITOR_Title)) {
		editorTitle = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Title);
	}

	ot::TextEditorView* editor = AppBase::instance()->findOrCreateTextEditor(editorName, QString::fromStdString(editorTitle), info);
	editor->setPlainText(QString::fromStdString(editorText));
	editor->setContentChanged(false);

	return "";
}

std::string ExternalServicesComponent::handleSetTextEditorSaved(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Name);
	ot::TextEditorView* editor = AppBase::instance()->findTextEditor(editorName, info);

	if (editor) {
		editor->setContentChanged(false);
	}

	return "";
}

std::string ExternalServicesComponent::handleSetTextEditorModified(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Name);
	ot::TextEditorView* editor = AppBase::instance()->findTextEditor(editorName, info);

	if (editor) {
		editor->setContentChanged(true);
	}

	return "";
}

std::string ExternalServicesComponent::handleCloseTextEditor(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Name);
	AppBase::instance()->closeTextEditor(editorName, info);

	return "";
}

std::string ExternalServicesComponent::handleCloseAllTextEditors(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	AppBase::instance()->closeAllTextEditors(info);

	return "";
}

// Studio Suite API

std::string ExternalServicesComponent::handleStudioSuiteImport(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_MEMBER);
	return StudioSuiteConnectorAPI::processAction(action, _document, AppBase::instance()->getCurrentProjectName(), this, AppBase::instance()->mainWindow()->windowIcon());
}

std::string ExternalServicesComponent::handleStudioSuiteCommit(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_MEMBER);
	return StudioSuiteConnectorAPI::processAction(action, _document, AppBase::instance()->getCurrentProjectName(), this, AppBase::instance()->mainWindow()->windowIcon());
}

std::string ExternalServicesComponent::handleStudioSuiteGet(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_MEMBER);
	return StudioSuiteConnectorAPI::processAction(action, _document, AppBase::instance()->getCurrentProjectName(), this, AppBase::instance()->mainWindow()->windowIcon());
}

std::string ExternalServicesComponent::handleStudioSuiteUpload(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_MEMBER);
	return StudioSuiteConnectorAPI::processAction(action, _document, AppBase::instance()->getCurrentProjectName(), this, AppBase::instance()->mainWindow()->windowIcon());
}

std::string ExternalServicesComponent::handleStudioSuiteDownload(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_MEMBER);
	return StudioSuiteConnectorAPI::processAction(action, _document, AppBase::instance()->getCurrentProjectName(), this, AppBase::instance()->mainWindow()->windowIcon());
}

std::string ExternalServicesComponent::handleStudioSuiteCopy(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_MEMBER);
	return StudioSuiteConnectorAPI::processAction(action, _document, AppBase::instance()->getCurrentProjectName(), this, AppBase::instance()->mainWindow()->windowIcon());
}

std::string ExternalServicesComponent::handleStudioSuiteInformation(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_MEMBER);
	return StudioSuiteConnectorAPI::processAction(action, _document, AppBase::instance()->getCurrentProjectName(), this, AppBase::instance()->mainWindow()->windowIcon());
}

std::string ExternalServicesComponent::handleStudioSuiteSetCSTFile(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_MEMBER);
	return StudioSuiteConnectorAPI::processAction(action, _document, AppBase::instance()->getCurrentProjectName(), this, AppBase::instance()->mainWindow()->windowIcon());
}

// Dialogs

std::string ExternalServicesComponent::handleEntitySelectionDialog(ot::JsonDocument& _document) {
	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);
	const ot::UID entityToBeExtended = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);

	ot::SelectEntitiesDialogCfg pckg;
	pckg.setFromJsonObject(cfgObj);

	SelectEntitiesDialog dia(pckg, nullptr);
	dia.showDialog();

	if (dia.dialogResult() == ot::Dialog::Ok && dia.selectionHasChanged()) {
		std::list<std::string> selectedItems = dia.selectedItemPaths();
		if (selectedItems.size() != 0)
		{
			ot::JsonDocument responseDoc;
			ot::JsonArray jSelectedItemsNames;
			for (const std::string& selectedItem : selectedItems)
			{
				jSelectedItemsNames.PushBack(ot::JsonString(selectedItem, responseDoc.GetAllocator()), responseDoc.GetAllocator());
			}
			responseDoc.AddMember(OT_ACTION_PARAM_UI_TREE_SelectedItems, jSelectedItemsNames, responseDoc.GetAllocator());
			responseDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, entityToBeExtended, responseDoc.GetAllocator());
			responseDoc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_UpdateCurvesOfPlot, responseDoc.GetAllocator());
			std::string response;
			sendHttpRequest(QUEUE, m_modelServiceURL, responseDoc, response);
		}
	}

	return "";
}

std::string ExternalServicesComponent::handlePropertyDialog(ot::JsonDocument& _document) {
	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);

	ot::PropertyDialogCfg pckg;
	pckg.setFromJsonObject(cfgObj);

	ot::PropertyDialog dia(pckg, nullptr);
	dia.showDialog();

	if (dia.dialogResult() == ot::Dialog::Ok) {

	}

	return "";
}

std::string ExternalServicesComponent::handleOnePropertyDialog(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);
	const std::string subsequentFunction = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_FunctionName);
	ot::OnePropertyDialogCfg pckg;
	pckg.setFromJsonObject(cfgObj);

	ot::OnePropertyDialog dia(pckg, nullptr);
	dia.showDialog();

	if (dia.dialogResult() == ot::Dialog::Ok) {
		ot::JsonDocument responseDoc;
		responseDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteFunction, responseDoc.GetAllocator()), responseDoc.GetAllocator());
		responseDoc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(subsequentFunction, responseDoc.GetAllocator()), responseDoc.GetAllocator());
		dia.addPropertyInputValueToJson(responseDoc, OT_ACTION_PARAM_Value, responseDoc.GetAllocator());

		std::string response;
		sendHttpRequest(EXECUTE, info, responseDoc, response);
	}

	return "";
}

std::string ExternalServicesComponent::handleMessageDialog(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);

	ot::MessageDialogCfg cfg;
	cfg.setFromJsonObject(cfgObj);

	ot::MessageDialogCfg::BasicButton result = ot::MessageDialog::showDialog(cfg);

	ot::JsonDocument responseDoc;
	responseDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_MessageDialogValue, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_ObjectName, ot::JsonString(cfg.name(), responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_Value, ot::JsonString(ot::MessageDialogCfg::buttonToString(result), responseDoc.GetAllocator()), responseDoc.GetAllocator());

	std::string response;
	sendHttpRequest(EXECUTE, info, responseDoc, response);

	return "";
}

// Plugin handling

std::string ExternalServicesComponent::handlePluginSearchPath(ot::JsonDocument& _document) {
#ifdef _DEBUG
	std::string pluginPath = ot::json::getString(_document, OT_ACTION_PARAM_UI_PLUGIN_PATH);
	m_owner->uiPluginManager()->addPluginSearchPath(pluginPath.c_str());
	AppBase::instance()->appendDebugMessage("[ERROR] Added UI plugin search path: " + QString::fromStdString(pluginPath));
#endif // _DEBUG
	return "";
}

std::string ExternalServicesComponent::handleRequestPlugin(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string pluginName = ot::json::getString(_document, OT_ACTION_PARAM_UI_PLUGIN_NAME);
	std::string pluginPath = ot::json::getString(_document, OT_ACTION_PARAM_UI_PLUGIN_PATH);
	ot::ServiceBase* service = getService(serviceId);
	ak::UID pluginUid = m_owner->uiPluginManager()->loadPlugin(pluginName.c_str(), pluginPath.c_str(), service);

	if (pluginUid) {
		AppBase* app = AppBase::instance();

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_RequestPluginSuccess, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, app->serviceID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(app->getServiceURL(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(app->serviceName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(app->serviceType(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_UI_PLUGIN_NAME, ot::JsonString(pluginName, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_UI_PLUGIN_UID, pluginUid, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_UI_PLUGIN_PATH, ot::JsonString(pluginPath, doc.GetAllocator()), doc.GetAllocator());

		std::string response;
		sendHttpRequest(EXECUTE, service->serviceURL(), doc, response);

		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) { assert(0); }
		else OT_ACTION_IF_RESPONSE_WARNING(response) { assert(0); }
	}
	return "";
}

std::string ExternalServicesComponent::handlePluginMessage(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string pluginAction = ot::json::getString(_document, OT_ACTION_PARAM_UI_PLUGIN_ACTION_MEMBER);
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	std::string pluginName = ot::json::getString(_document, OT_ACTION_PARAM_UI_PLUGIN_NAME);
	unsigned long long pluginUID = ot::json::getUInt64(_document, OT_ACTION_PARAM_UI_PLUGIN_UID);
	ot::ServiceBase* service = getService(serviceId);
	if (!AppBase::instance()->uiPluginManager()->forwardMessageToPlugin(pluginUID, pluginAction, message)) {
		return OT_ACTION_RETURN_INDICATOR_Error "Failed to process message";
	}
	return "";
}

// ###################################################################################################

void sessionServiceHealthChecker(std::string _sessionServiceURL) {
	// Create ping request
	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	std::string ping = pingDoc.toJson();

	bool sessionServiceDied{ false };
	while (g_runSessionServiceHealthCheck && !sessionServiceDied) {
		// Wait for 20s
		int ct{ 0 };
		while (ct++ < 20 && g_runSessionServiceHealthCheck && !sessionServiceDied) {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);
		}

		if (g_runSessionServiceHealthCheck) {
			// Try to send message and check the response
			std::string response;
			try {
				if (!ot::msg::send("", _sessionServiceURL, ot::EXECUTE_ONE_WAY_TLS, ping, response)) { sessionServiceDied = true; }
				else OT_ACTION_IF_RESPONSE_ERROR(response) { sessionServiceDied = true; }
				else OT_ACTION_IF_RESPONSE_WARNING(response) { sessionServiceDied = true; }
				else if (response != OT_ACTION_CMD_Ping) { sessionServiceDied = true; }
			}
			catch (...) { sessionServiceDied = true; }
		}
	}

	if (sessionServiceDied) {
#ifdef _DEBUG
		_outputDebugMessage("Session service has died unexpectedly. Shutting down...");
#endif // _DEBUG

		QMetaObject::invokeMethod(AppBase::instance()->getExternalServicesComponent(), "shutdownAfterSessionServiceDisconnected", Qt::QueuedConnection);
	}
}

void ot::startSessionServiceHealthCheck(const std::string& _sessionServiceURL) {
	if (g_sessionServiceHealthCheckThread) {
		assert(0); // Health check already running
		return;
	}
	g_runSessionServiceHealthCheck = true;
	g_sessionServiceHealthCheckThread = new std::thread(sessionServiceHealthChecker, _sessionServiceURL);
}

void ot::stopSessionServiceHealthCheck(void) {
	if (g_sessionServiceHealthCheckThread == nullptr) {
		assert(0); // No health check running
		return;
	}
	g_runSessionServiceHealthCheck = false;
	if (g_sessionServiceHealthCheckThread->joinable()) { g_sessionServiceHealthCheckThread->join(); }
	delete g_sessionServiceHealthCheckThread;
	g_sessionServiceHealthCheckThread = nullptr;
}
