//! @file ExternalServicesComponent.cpp
//! @authors Alexander Kuester, Peter Thoma
//! @date September 2020
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "AppBase.h"
#include "ToolBar.h"
#include "UserSettings.h"
#include "ServiceDataUi.h"
#include "UserManagement.h"
#include "ViewerComponent.h"			// Viewer component
#include "ControlsManager.h"
#include "ShortcutManager.h"
#include "WebsocketClient.h"
#include "SelectEntitiesDialog.h"
#include "ExternalServicesComponent.h"	// Corresponding header

// OpenTwin header
#include "OTSystem/AppExitCodes.h"

#include "OTCore/Logger.h"
#include "OTCore/Color.h"
#include "OTCore/ThisService.h"
#include "OTCore/OwnerService.h"
#include "OTCore/OwnerServiceGlobal.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "OTCore/ReturnMessage.h"

#include "OTGui/GuiTypes.h"
#include "OTGui/TableRange.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/MessageDialogCfg.h"
#include "OTGui/PropertyDialogCfg.h"
#include "OTGui/PropertyStringList.h"
#include "OTGui/OnePropertyDialogCfg.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/SelectEntitiesDialogCfg.h"

#include "OTWidgets/Table.h"
#include "OTWidgets/PlotView.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/TableView.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/PropertyInput.h"
#include "OTWidgets/MessageDialog.h"
#include "OTWidgets/GraphicsPicker.h"
#include "OTWidgets/TextEditorView.h"
#include "OTWidgets/PropertyDialog.h"
#include "OTWidgets/GraphicsViewView.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/OnePropertyDialog.h"
#include "OTWidgets/PropertyGridGroup.h"
#include "OTWidgets/WidgetViewManager.h"
#include "OTWidgets/ModelLibraryDialog.h"
#include "OTWidgets/SignalBlockWrapper.h"
#include "OTWidgets/GraphicsLayoutItem.h"
#include "OTWidgets/GraphicsItemFactory.h"
#include "OTWidgets/StyledTextConverter.h"
#include "OTWidgets/VersionGraphManager.h"
#include "OTWidgets/VersionGraphManagerView.h"

#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/IpConverter.h"
#include "OTCommunication/Msg.h"

#include "CurveDatasetFactory.h"
#include "OTCore/String.h"

#include "StudioSuiteConnector/StudioSuiteConnectorAPI.h"
#include "LTSpiceConnector/LTSpiceConnectorAPI.h"
#include "ProgressUpdater.h"

// uiCore header
#include <akAPI/uiAPI.h>
#include <akCore/akCore.h>

// Qt header
#include <QtCore/qdir.h>						// QDir
#include <QtCore/qeventloop.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qapplication.h>

// ThirdParty header
#include "base64.h"
#include "zlib.h"

// std header
#include <chrono>
#include <thread>
#include <vector>
#include <fstream>
#include <iostream>

#include "OTCore/EntityName.h"

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
			OT_LOG_EAS("Error occured on invoke. Exiting...\nError: " + std::string(e.what()));
			exit(ot::AppExitCode::GeneralError);
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
			OT_LOG_EAS("Error occured on invoke. Exiting...\nError: " + std::string(e.what()));
			exit(ot::AppExitCode::GeneralError);
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
			OT_LOG_EAS("Error occured on invoke. Exiting...\nError: " + std::string(e.what()));
			exit(ot::AppExitCode::GeneralError);
		}
	};
}

namespace ot {
	namespace intern {
		void exitAsync(int _code) {
			exit(_code);
		}
	}
}

// ###################################################################################################

ExternalServicesComponent::ExternalServicesComponent(AppBase * _owner) :
	m_websocket{ nullptr },
	m_keepAliveTimer { nullptr },
	m_isRelayServiceRequired{ false },
	m_controlsManager{ nullptr },
	m_lockManager{ nullptr },
	m_owner{ _owner },
	m_prefetchingDataCompleted{ false },
	m_servicesUiSetupCompleted(false)
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
	commandDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, AppBase::instance()->getServiceID(), commandDoc.GetAllocator());
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

KeyboardCommandHandler* ExternalServicesComponent::addShortcut(ServiceDataUi* _sender, const std::string& _keySequence) {
	if (_keySequence.length() > 0) {
		ShortcutManager* manager = AppBase::instance()->shortcutManager();
		if (manager) {
			KeyboardCommandHandler* oldHandler = manager->handlerFromKeySequence(_keySequence.c_str());
			if (oldHandler) {
				OT_LOG_WAS("Shortcut for key sequence \"" + _keySequence + "\" already occupied by service \"" + oldHandler->creator()->getServiceName() +
					" (ID: " + std::to_string(oldHandler->creator()->getServiceID()) + ")\"\n");
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
	std::string authorizationURL = AppBase::instance()->getCurrentLoginData().getAuthorizationUrl();
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_ALL_USER_PROJECTS, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(AppBase::instance()->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(AppBase::instance()->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_FILTER, ot::JsonString("", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_LIMIT, 0, doc.GetAllocator());
	std::string response;
	if (!ot::msg::send("", authorizationURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))
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
				sendHttpRequest(EXECUTE, reciever->getServiceURL(), inDoc, response);
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
		{
			return false;
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS("Failed to delete model: " + std::string(_e.what()));
		return false;
	}
}

void ExternalServicesComponent::setVisualizationModel(ModelUIDtype modelID, ModelUIDtype visualizationModelID)
{
	OT_LOG_EAS("NOT IN USE ANYMORE");
	try {
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_SetVisualizationModel, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_VIEW_ID, rapidjson::Value(visualizationModelID), inDoc.GetAllocator());

		std::string response;
		for (auto reciever : m_modelViewNotifier) {
			sendHttpRequest(EXECUTE, reciever->getServiceURL(), inDoc, response);
			// Check if response is an error or warning
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				OT_LOG_EAS("Error response: " + response);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_EAS("Warning response: " + response);
				}
		}

	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
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
			sendHttpRequest(EXECUTE, reciever->getServiceURL(), inDoc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				OT_LOG_EAS("Error response: " + response);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_EAS("Warning response: " + response);
			}
		}
		//NOTE, WARNING, at this point only the last response will be taken into accout..

		ot::JsonDocument outDoc;
		outDoc.fromJson(response);
		ot::UID uid = outDoc[OT_ACTION_PARAM_BASETYPE_UID].GetInt64();
		return uid;
	}
	catch (...) {
		assert(0); // Error handling
		return 0;
	}
}

bool ExternalServicesComponent::isModelModified(ModelUIDtype modelID) {
	ot::JsonDocument inDoc;
	inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_GetIsModified, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());
	std::string response;

	for (auto reciever : m_modelViewNotifier) {
		sendHttpRequest(EXECUTE, reciever->getServiceURL(), inDoc, response);
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			OT_LOG_EAS("Error response: " + response);
		}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_EAS("Warning response: " + response);
			}
	}
	//NOTE, WARNING, at this point only the last response will be taken into accout..

	ot::JsonDocument outDoc;
	outDoc.fromJson(response);
	bool modified = outDoc[OT_ACTION_PARAM_BASETYPE_Bool].GetBool();
	return modified;
}

bool ExternalServicesComponent::isCurrentModelModified(void) {
	// Get the id of the curently active model
	ModelUIDtype modelID = AppBase::instance()->getViewerComponent()->getActiveDataModel();
	if (modelID == 0) {
		return false;  // No project currently active
	}
	else {
		return isModelModified(modelID);
	}
}

// ###################################################################################################

// Event handling

void ExternalServicesComponent::notify(ot::UID _senderId, ak::eventType _event, int _info1, int _info2) {
	try {

		if (_event == ak::etClicked || _event == ak::etEditingFinished)
		{
			auto receiver = this->getServiceFromNameType(m_controlsManager->objectCreator(_senderId));
			
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

				if (!sendHttpRequest(EXECUTE, receiver->getServiceURL(), doc, response)) {
					assert(0); // Failed to send HTTP request
				}
				// Check if response is an error or warning
				OT_ACTION_IF_RESPONSE_ERROR(response) {
					assert(0); // ERROR
					AppBase::instance()->showErrorPrompt(response, "Error");
				}
				else OT_ACTION_IF_RESPONSE_WARNING(response) {
					assert(0); // WARNING
					AppBase::instance()->showWarningPrompt(response, "Warning");
				}
			}
			else { executeAction(AppBase::instance()->getViewerComponent()->getActiveDataModel(), _senderId); }
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}
}

void ExternalServicesComponent::fillPropertyGrid(const std::string& _settings) {

}

void ExternalServicesComponent::modelSelectionChangedNotification(ModelUIDtype modelID, std::list<ModelUIDtype>& selectedEntityID, std::list<ModelUIDtype>& selectedVisibleEntityID) {
	ot::JsonDocument inDoc;
	inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_SelectionChanged, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_SelectedEntityIDs, ot::JsonArray(selectedEntityID, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_SelectedVisibleEntityIDs, ot::JsonArray(selectedVisibleEntityID, inDoc.GetAllocator()), inDoc.GetAllocator());

	std::string response;
	for (auto reciever : m_modelViewNotifier) {
		sendHttpRequest(EXECUTE, reciever->getServiceURL(), inDoc, response);
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			OT_LOG_EAS("Error response: " + response);
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			OT_LOG_EAS("Warning response: " + response);
		}
	}
}

void ExternalServicesComponent::itemRenamed(ModelUIDtype modelID, const std::string& newName) {
	ot::JsonDocument inDoc;
	inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ItemRenamed, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Name, rapidjson::Value(newName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());

	std::string response;
	for (auto reciever : m_modelViewNotifier) {
		sendHttpRequest(EXECUTE, reciever->getServiceURL(), inDoc, response);
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			OT_LOG_EAS("Error response: " + response);
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			OT_LOG_EAS("Warning response: " + response);
		}
	}
}

ot::Property* ExternalServicesComponent::createCleanedProperty(const ot::Property* const _item) {
	ot::PropertyGroup* rootGroup = _item->getRootGroup();
	if (!rootGroup) {
		OT_LOG_E("No parent group set");
		return nullptr;
	}

	rootGroup = rootGroup->createCopy(true);
	OTAssertNullptr(rootGroup);

	std::list<ot::Property*> propertyList = rootGroup->getAllProperties();
	if (!propertyList.size() == 1) {
		OT_LOG_E("Data mismatch");
		delete rootGroup;
		return nullptr;
	}

	// For the EntityList property we remove the value id since we dont know the id of the entity
	if (propertyList.front()->getPropertyType() == ot::PropertyStringList::propertyTypeString() && propertyList.front()->getSpecialType() == "EntityList") {
		ot::JsonDocument dataDoc;
		ot::JsonDocument newDataDoc;
		dataDoc.fromJson(propertyList.front()->getAdditionalPropertyData("EntityData"));
		newDataDoc.AddMember("ContainerName", ot::JsonString(ot::json::getString(dataDoc, "ContainerName"), newDataDoc.GetAllocator()), newDataDoc.GetAllocator());
		newDataDoc.AddMember("ContainerID", ot::json::getUInt64(dataDoc, "ContainerID"), newDataDoc.GetAllocator());
		newDataDoc.AddMember("ValueID", 0, newDataDoc.GetAllocator());
		//newDataDoc.AddMember("ValueID", ot::json::getUInt64(dataDoc, "ValueID"), newDataDoc.GetAllocator());
		propertyList.front()->addAdditionalPropertyData("EntityData", newDataDoc.toJson());
	}

	return propertyList.front();
}

void ExternalServicesComponent::propertyGridValueChanged(const ot::Property* _property) {
	try {
		// Get the currently selected model entities. We first get all visible entities only.
		std::list<ot::UID> selectedModelEntityIDs;
		getSelectedVisibleModelEntityIDs(selectedModelEntityIDs);
		bool itemsVisible = true;

		// If we do not have visible entities, then we also look for the hidden ones.
		if (selectedModelEntityIDs.empty())
		{
			getSelectedModelEntityIDs(selectedModelEntityIDs);
			itemsVisible = false;
		}

		// Finally send the string
		ot::UID modelID = AppBase::instance()->getViewerComponent()->getActiveDataModel();

		ot::Property* cleanedProperty = this->createCleanedProperty(_property);
		if (!cleanedProperty) {
			OT_LOG_EA("Failed to create cleaned property");
			return;
		}

		ot::PropertyGridCfg newConfig;
		newConfig.addRootGroup(cleanedProperty->getRootGroup());

		ot::JsonDocument doc;
		ot::JsonObject cfgObj;
		newConfig.addToJsonObject(cfgObj, doc.GetAllocator());

		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_SetPropertiesFromJSON, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ID, modelID, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(selectedModelEntityIDs, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_Update, true, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ItemsVisible, itemsVisible, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_Config, cfgObj, doc.GetAllocator());

		std::string response;

		for (auto reciever : m_modelViewNotifier) {
			sendHttpRequest(EXECUTE, reciever->getServiceURL(), doc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				OT_LOG_E(response);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_W(response);
			}
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
	}
}

void ExternalServicesComponent::propertyGridValueDeleteRequested(const ot::Property* _property) {
	AppBase::instance()->lockPropertyGrid(true);

	// Get the currently selected model entities. We first get all visible entities only.
	std::list<ot::UID> selectedModelEntityIDs;
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
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityName, ot::JsonString(_property->getPropertyName(), doc.GetAllocator()), doc.GetAllocator());
		auto parentGroup = _property->getParentGroup();
		if (parentGroup != nullptr)
		{
			doc.AddMember(OT_PARAM_GROUP, ot::JsonString(parentGroup->getName(), doc.GetAllocator()), doc.GetAllocator());
		}
		std::string response;

		for (auto reciever : m_modelViewNotifier) {
			sendHttpRequest(EXECUTE, reciever->getServiceURL(), doc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				OT_LOG_E(response);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_W(response);
			}
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
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

		ServiceDataUi *receiver = getService(replyToServiceID);

		if (receiver != nullptr)
		{
			std::string response;
			sendHttpRequest(EXECUTE, receiver->getServiceURL(), inDoc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				OT_LOG_EAS("Error response: " + response);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_EAS("Warning response: " + response);
			}
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
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
			sendHttpRequest(EXECUTE, reciever->getServiceURL(), doc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				OT_LOG_EAS("Error response: " + response);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_EAS("Warning response: " + response);
			}
		}


	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
	}
}

void ExternalServicesComponent::prefetchDataThread(const std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> prefetchIDs)
{
	prefetchDocumentsFromStorage(projectName, prefetchIDs);
	m_prefetchingDataCompleted = true;
}

bool ExternalServicesComponent::projectIsOpened(const std::string &projectName, std::string &projectUser)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_IsProjectOpen, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());

	AppBase * app{ AppBase::instance() };

	std::string response;
	sendHttpRequest(EXECUTE, app->getCurrentLoginData().getGss().getConnectionUrl().toStdString(), doc.toJson(), response);

	// todo: add json return value containing true/false and username instead of empty string for more clarity
	if (response.empty()) return false;

	projectUser = response;
	return true;
}

// ###################################################################################################

// Messaging

bool ExternalServicesComponent::sendHttpRequest(RequestType operation, ot::OwnerService _service, ot::JsonDocument& doc, std::string& response) {
	auto it = m_serviceIdMap.find(_service.getId());
	if (it == m_serviceIdMap.end()) {
		OT_LOG_E("Failed to find service with id \"" + std::to_string(_service.getId()) + "\"");
		return false;
	}
	return sendHttpRequest(operation, it->second->getServiceURL(), doc, response);
}

bool ExternalServicesComponent::sendHttpRequest(RequestType operation, const ot::BasicServiceInformation& _serviceInformation, ot::JsonDocument& _doc, std::string& _response) {
	auto service = this->getService(_serviceInformation);
	if (service) {
		return this->sendHttpRequest(operation, service->getServiceURL(), _doc, _response);
	}
	else {
		return false;
	}
}

void ExternalServicesComponent::sendToModelService(const std::string& _message, std::string _response)
{
	sendHttpRequest(QUEUE, m_modelServiceURL, _message, _response);
}

bool ExternalServicesComponent::sendHttpRequest(RequestType operation, const std::string &url, ot::JsonDocument &doc, std::string &response) {
	return sendHttpRequest(operation, url, doc.toJson(), response); 
}

bool ExternalServicesComponent::sendHttpRequest(RequestType operation, const std::string &url, const std::string &message, std::string &response) {
	bool success = false;
	
	try {
		std::string globalServiceURL = AppBase::instance()->getServiceURL();

		if (m_websocket != nullptr) {
			// If a websocket is set send the request as a relayed request trough the relay service
			success = this->sendRelayedRequest(operation, url, message, response);
		}
		else {
			// If no websocket is set send the request directly via a curl request
			switch (operation) {
			case EXECUTE:
				success = ot::msg::send(globalServiceURL, url, ot::EXECUTE_ONE_WAY_TLS, message, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit);
				break;

			case QUEUE:
				success = ot::msg::send(globalServiceURL, url, ot::QUEUE, message, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit);
				break;

			default:
				OT_LOG_EA("Unknown request type");
				break;
			}
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}

	return success;
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

	if (!sendHttpRequest(EXECUTE, _sender->creator()->getServiceURL(), doc, response)) {
		assert(0); // Failed to send HTTP request
	}
	// Check if response is an error or warning
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		assert(0); // ERROR
		AppBase::instance()->showErrorPrompt(response, "Error");
		return false;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		assert(0); // WARNING
		AppBase::instance()->showWarningPrompt(response, "Warning");
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

		sendHttpRequest(EXECUTE, receiver->second->getServiceURL(), doc, response);
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			OT_LOG_EAS("Error response: " + response);
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			OT_LOG_EAS("Warning response: " + response);
		}

	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
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
			sendHttpRequest(EXECUTE, reciever->getServiceURL(), doc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				OT_LOG_EAS("Error response: " + response);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_EAS("Warning response: " + response);
			}
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}
}

void ExternalServicesComponent::versionSelected(const std::string& _version) {
	try {
		ServiceDataUi* model = this->getServiceFromNameType(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL);
		if (!model) {
			OT_LOG_EA(OT_INFO_SERVICE_TYPE_MODEL " service not found");
			return;
		}
		OT_LOG_D("Version selected { \"Version\": \"" + _version + "\" }");

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_VersionSelected, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(_version, doc.GetAllocator()), doc.GetAllocator());
		std::string response;

		this->sendHttpRequest(EXECUTE, model->getServiceURL(), doc, response);
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			OT_LOG_EAS("Error response: " + response);
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			OT_LOG_EAS("Warning response: " + response);
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}
}

void ExternalServicesComponent::versionDeselected(void) {
	try {
		ServiceDataUi* model = this->getServiceFromNameType(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL);
		if (!model) {
			OT_LOG_EA(OT_INFO_SERVICE_TYPE_MODEL " service not found");
			return;
		}
		
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_VersionDeselected, doc.GetAllocator()), doc.GetAllocator());
		std::string response;

		this->sendHttpRequest(EXECUTE, model->getServiceURL(), doc, response);
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			assert(0); // ERROR
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}
}

void ExternalServicesComponent::activateVersion(const std::string& _version)
{
	try {
		ServiceDataUi* model = this->getServiceFromNameType(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL);
		if (!model) {
			OT_LOG_EA(OT_INFO_SERVICE_TYPE_MODEL " service not found");
			return;
		}
		OT_LOG_D("Version requested { \"Version\": \"" + _version + "\" }");

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ActivateVersion, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(_version, doc.GetAllocator()), doc.GetAllocator());
		std::string response;

		this->sendHttpRequest(EXECUTE, model->getServiceURL(), doc, response);
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			assert(0); // ERROR
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}
}

// ###################################################################################################

// Project handling

std::list<ot::ProjectTemplateInformation> ExternalServicesComponent::getListOfProjectTemplates(void) {
	std::list<ot::ProjectTemplateInformation> result;

	AppBase* app{ AppBase::instance() };
	std::string response;

#ifndef OT_USE_GSS
	return result;
#endif
	// Request a session service from the global session service
	ot::JsonDocument gssDoc;
	gssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetListOfProjectTemplates, gssDoc.GetAllocator()), gssDoc.GetAllocator());
	gssDoc.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(app->getCurrentLoginData().getSessionUser(), gssDoc.GetAllocator()), gssDoc.GetAllocator());
	gssDoc.AddMember(OT_PARAM_DB_PASSWORD, ot::JsonString(app->getCurrentLoginData().getSessionPassword(), gssDoc.GetAllocator()), gssDoc.GetAllocator());
	
	if (!sendHttpRequest(EXECUTE, app->getCurrentLoginData().getGss().getConnectionUrl().toStdString(), gssDoc.toJson(), response)) {
		assert(0); // Failed to send
		OT_LOG_E("Failed to send \"Create new session\" request to the global session service");
		app->showErrorPrompt("Failed to send \"Create new session\" request to the global session service", "Error");
		return result;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		assert(0); // ERROR
		OT_LOG_E(response);
		app->showErrorPrompt(response, "Error");
		return result;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		assert(0); // WARNING
		OT_LOG_W(response);
		app->showWarningPrompt(response, "Warning");
		return result;
	}
	
	// Check response
	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	if (!responseDoc.IsArray()) {
		return result;
	}
	
	for (ot::JsonSizeType i = 0; i < responseDoc.Size(); i++) {
		ot::ConstJsonObject obj = ot::json::getObject(responseDoc, i);
		ot::ProjectTemplateInformation info;
		info.setFromJsonObject(obj);
		result.push_back(info);
	}

	return result;
}

bool ExternalServicesComponent::openProject(const std::string & _projectName, const std::string& _projectType, const std::string & _collectionName) {

	AppBase * app{ AppBase::instance() };
	try {
		ot::LogDispatcher::instance().setProjectName(_projectName);

		OT_LOG_D("Open project requested (Project name = \"" + _projectName + ")");

		ScopedLockManagerLock uiLock(m_lockManager, app->getBasicServiceInformation(), ot::LockAll);

		StudioSuiteConnectorAPI::openProject();

		m_currentSessionID = _projectName;
		m_currentSessionID.append(":").append(_collectionName);
		AppBase::instance()->SetCollectionName(_collectionName);

		std::string response;
#ifdef OT_USE_GSS
		// Request a session service from the global session service
		ot::JsonDocument gssDoc;
		gssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_CreateNewSession, gssDoc.GetAllocator()), gssDoc.GetAllocator());
		gssDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, gssDoc.GetAllocator()), gssDoc.GetAllocator());
		gssDoc.AddMember(OT_ACTION_PARAM_USER_NAME, ot::JsonString(app->getCurrentLoginData().getUserName(), gssDoc.GetAllocator()), gssDoc.GetAllocator());

		if (!sendHttpRequest(EXECUTE, app->getCurrentLoginData().getGss().getConnectionUrl().toStdString(), gssDoc.toJson(), response)) {
			OT_LOG_EA("Failed to send \"Create new session\" request to the global session service");
			app->showErrorPrompt("Failed to send \"Create new session\" request to the global session service", "Error");
			ot::LogDispatcher::instance().setProjectName("");
			return false;
		}
		ot::ReturnMessage responseMessage = ot::ReturnMessage::fromJson(response);
		if (responseMessage != ot::ReturnMessage::Ok) {
			app->showErrorPrompt(responseMessage.getWhat(), "Error");
			ot::LogDispatcher::instance().setProjectName("");
			return false;
		}

		m_sessionServiceURL = responseMessage.getWhat();

		OT_LOG_D("GSS provided the LSS at \"" + m_sessionServiceURL + "\"");
#endif

		app->setCurrentProjectName(_projectName);
		app->setCurrentProjectType(_projectType);

		// ##################################################################

		// Create new session command
		ot::JsonDocument sessionDoc;
		sessionDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_CreateNewSession, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());

		// Add project and user information
		sessionDoc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(_collectionName, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_PROJECT_NAME, ot::JsonString(_projectName, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_USER_NAME, ot::JsonString(AppBase::instance()->getCurrentLoginData().getUserName(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_PARAM_SETTINGS_USERCOLLECTION, ot::JsonString(AppBase::instance()->getCurrentUserCollection(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());

		// Add session information
		sessionDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_SESSION_TYPE, ot::JsonString(_projectType, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());

		// Add service information
		sessionDoc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_UI, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(OT_INFO_SERVICE_TYPE_UI, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_PORT, ot::JsonString(ot::IpConverter::portFromIp(m_uiServiceURL), sessionDoc.GetAllocator()), sessionDoc.GetAllocator()); // todo: rework -> create session -> send service port (ui uses the websocket and the port is not required)
		sessionDoc.AddMember(OT_ACTION_PARAM_HOST, ot::JsonString(ot::IpConverter::hostFromIp(m_uiServiceURL), sessionDoc.GetAllocator()), sessionDoc.GetAllocator()); // todo: rework -> create session -> send service port (ui uses the websocket and the port is not required)
		sessionDoc.AddMember(OT_ACTION_PARAM_START_RELAY, m_isRelayServiceRequired, sessionDoc.GetAllocator());

		// Add user credentials
		sessionDoc.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_PARAM_AUTH_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(app->getCurrentLoginData().getSessionUser(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_PARAM_DB_PASSWORD, ot::JsonString(app->getCurrentLoginData().getSessionPassword(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());

		response.clear();
		if (!sendHttpRequest(EXECUTE, m_sessionServiceURL, sessionDoc.toJson(), response)) {
			OT_LOG_EAS("Failed to send http request to Local Session Service at \"" + m_sessionServiceURL + "\"");
			app->showErrorPrompt("Failed to send http request to Local Session Service", "Connection Error");
			ot::LogDispatcher::instance().setProjectName("");
			return false;
		}
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			OT_LOG_EAS("Error response from  Local Session Service at \"" + m_sessionServiceURL + "\": " + response);
			app->showErrorPrompt("Failed to create Session. " + response, "Error");
			ot::LogDispatcher::instance().setProjectName("");
			return false;
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			OT_LOG_WAS("Warning response from  Local Session Service at \"" + m_sessionServiceURL + "\": " + response);
			app->showErrorPrompt("Failed to create Session. " + response, "Error");
			ot::LogDispatcher::instance().setProjectName("");
			return false;
		}
		
		// Check response
		ot::JsonDocument responseDoc;
		responseDoc.fromJson(response);

		// ##################################################################

		if (responseDoc.HasMember(OT_ACTION_PARAM_LogFlags)) {
			ot::ConstJsonArray logData = ot::json::getArray(responseDoc, OT_ACTION_PARAM_LogFlags);
			ot::LogFlags logFlags = ot::logFlagsFromJsonArray(logData);
			ot::LogDispatcher::instance().setLogFlags(logFlags);
			AppBase::instance()->updateLogIntensityInfo();
		}

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
		m_servicesUiSetupCompleted = false;

		ot::JsonDocument checkCommandDoc;
		checkCommandDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_CheckStartupCompleted, checkCommandDoc.GetAllocator()), checkCommandDoc.GetAllocator());
		checkCommandDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, checkCommandDoc.GetAllocator()), checkCommandDoc.GetAllocator());
		std::string checkCommandString = checkCommandDoc.toJson();

		OT_LOG_D("Waiting for Startup Completed");

		do
		{
			if (!sendHttpRequest(EXECUTE, m_sessionServiceURL, checkCommandString, response)) {
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
				OT_LOG_E("Invalid Session Service response: " + response);
				throw std::exception(("Invalid Session Service response: " + response).c_str());
			}
		} while (!startupReady);

		OT_LOG_D("Startup is completed");

		// Set service visible (will notify others that the UI is available)
		ot::JsonDocument visibilityCommand;
		visibilityCommand.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceShow, visibilityCommand.GetAllocator()), visibilityCommand.GetAllocator());
		visibilityCommand.AddMember(OT_ACTION_PARAM_SERVICE_ID, AppBase::instance()->getServiceID(), visibilityCommand.GetAllocator());
		visibilityCommand.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, visibilityCommand.GetAllocator()), visibilityCommand.GetAllocator());
		if (!sendHttpRequest(EXECUTE, m_sessionServiceURL, visibilityCommand.toJson(), response)) {
			throw std::exception("Failed to send http request");
		}
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			std::string ex("From ");
			ex.append(m_sessionServiceURL).append(": ").append(response);
			throw std::exception(ex.c_str());
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
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
			// Dont store this services information
			if (senderID == AppBase::instance()->getServiceID()) continue;

			if (senderType == OT_INFO_SERVICE_TYPE_MODEL)
			{
				determineViews(senderURL);
			}

			auto oldService = m_serviceIdMap.find(senderID);
			if (oldService == m_serviceIdMap.end()) {
				OT_LOG_D("Service registered { \"ServiceName\": \"" + senderName + "\", \"SenderID\": " + std::to_string(senderID) + " }");
				m_serviceIdMap.insert_or_assign(senderID, new ServiceDataUi{ senderName, senderType, senderURL, senderID });
			}
			else {
				OT_LOG_W("Duplicate service information provided by LSS { \"Name\": \"" + senderName + "\", \"Type\": \"" + senderType + "\" }");
			}
		}

#ifdef OT_USE_GSS
		// Start the session service health check
		ot::startSessionServiceHealthCheck(m_sessionServiceURL);
#endif // OT_USE_GSS

		assert(m_keepAliveTimer == nullptr);
		m_keepAliveTimer = new QTimer(this);
		connect(m_keepAliveTimer, SIGNAL(timeout()), this, SLOT(keepAlive()));
		m_keepAliveTimer->start(60000);

		uiLock.setNoUnlock();

		OT_LOG_D("Open project completed");

		return true;
	}
	catch (const std::exception & e) {
		OT_LOG_EAS(e.what());
		app->showErrorPrompt(e.what(), "Error");
		ot::LogDispatcher::instance().setProjectName("");
		return false;
	}
}

void ExternalServicesComponent::closeProject(bool _saveChanges) {
	try {
		AppBase* app{ AppBase::instance() };

		UserSettings::instance().clear();

		app->initializeDefaultUserSettings();

		OT_LOG_D("Closing project { name = \"" + app->getCurrentProjectName() + "\"; SaveChanges = " + (_saveChanges ? "True" : "False"));

		std::string projectName = app->getCurrentProjectName();
		if (projectName.length() == 0) { return; }

		// Remove all notifiers
		m_modelViewNotifier.clear();

		app->storeSessionState();

		// Notify the session service that the sesion should be closed now
		ot::JsonDocument shutdownCommand;
		shutdownCommand.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSession, shutdownCommand.GetAllocator()), shutdownCommand.GetAllocator());
		shutdownCommand.AddMember(OT_ACTION_PARAM_SERVICE_ID, app->getServiceID(), shutdownCommand.GetAllocator());
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

		if (m_keepAliveTimer != nullptr)
		{
			m_keepAliveTimer->stop();
			delete m_keepAliveTimer;

			m_keepAliveTimer = nullptr;
		}

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
			m_lockManager->cleanService(s.second->getBasicServiceInformation(), false, true);
			m_controlsManager->serviceDisconnected(s.second->getBasicServiceInformation());
			app->shortcutManager()->creatorDestroyed(s.second);
			delete s.second;
		}
		m_lockManager->cleanService(AppBase::instance()->getViewerComponent()->getBasicServiceInformation());
		m_controlsManager->serviceDisconnected(AppBase::instance()->getViewerComponent()->getBasicServiceInformation());

		app->shortcutManager()->clearViewerHandler();
		app->clearNavigationTree();
		app->clearPropertyGrid();
		app->clearGraphicsPickerData();

		// Clear all maps
		m_serviceToUidMap.clear();
		m_serviceIdMap.clear();

		// Close all the tabs in the tab widget for the viewer
		{
			ot::SignalBlockWrapper sigBlock(&ot::WidgetViewManager::instance());
			app->closeAllViewerTabs();
		}

		//NOTE, add service notification to close the session, or deregister the uiService
		app->setServiceID(ot::invalidServiceID);
		m_currentSessionID = "";
		app->clearSessionInformation();

		app->replaceInfoMessage(c_buildInfo);

		if (m_websocket != nullptr) { delete m_websocket; m_websocket = nullptr; }

		OT_LOG_D("Close project done");

		ot::LogDispatcher::instance().setProjectName("");
	}
	catch (const std::exception & e) {
		OT_LOG_E(e.what());
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
			sendHttpRequest(EXECUTE, reciever->getServiceURL(), inDoc, response);
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
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
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

// Slots

char* ExternalServicesComponent::performAction(const char* json, const char* senderIP) {
	using namespace std::chrono_literals;
	static bool lock = false;

	while (lock) {
		std::this_thread::sleep_for(1ms);
	}

	lock = true;

	char* retval = ot::ActionDispatcher::instance().dispatchWrapper(json, senderIP, ot::QUEUE);

	lock = false;
	
	return retval;
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

void ExternalServicesComponent::queueAction(const char* json, const char* senderIP) {
	using namespace std::chrono_literals;
	static bool lock = false;

	while (lock) {
		std::this_thread::sleep_for(1ms);

		QEventLoop loop;
		loop.processEvents();
	}

	lock = true;

	ot::ActionDispatcher::instance().dispatch(json, ot::QUEUE);

	delete[] senderIP;
	senderIP = nullptr;

	// Now notify the end of the currently processed message
	if (m_websocket != nullptr) {
		m_websocket->finishedProcessingQueuedMessage();
	}

	lock = false;
}

void ExternalServicesComponent::deallocateData(const char *data)
{
	delete[] data;
}

void ExternalServicesComponent::shutdownAfterSessionServiceDisconnected(void) {
	ot::stopSessionServiceHealthCheck();
	AppBase::instance()->showErrorPrompt("The session service has died unexpectedly. The application will be closed now.", "Error");
	std::thread exitThread(&ot::intern::exitAsync, ot::AppExitCode::LSSNotRunning);
	exitThread.detach();
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
	ot::LockTypeFlags lockFlags;
	lockFlags.setFlag(ot::LockModelWrite);
	lockFlags.setFlag(ot::LockViewWrite);
	lockFlags.setFlag(ot::LockModelRead);

	m_lockManager->lock(AppBase::instance()->getBasicServiceInformation(), lockFlags);
}

void ExternalServicesComponent::unlockGui(void)
{
	ot::LockTypeFlags lockFlags;
	lockFlags.setFlag(ot::LockModelWrite);
	lockFlags.setFlag(ot::LockViewWrite);
	lockFlags.setFlag(ot::LockModelRead);

	m_lockManager->unlock(AppBase::instance()->getBasicServiceInformation(), lockFlags);
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

std::vector<std::array<double, 3>> ExternalServicesComponent::getVectorDoubleArrayFromDocument(ot::JsonDocument& doc, const std::string& itemNname) {
	std::vector<std::array<double, 3>> result;
	rapidjson::Value vectorDouble = doc[itemNname.c_str()].GetArray();
	result.resize(vectorDouble.Size() / 3);

	for (unsigned int i = 0; i < vectorDouble.Size() / 3; i++) {
		result[i][0] = vectorDouble[3 * i].GetDouble();
		result[i][1] = vectorDouble[3 * i + 1].GetDouble();
		result[i][2] = vectorDouble[3 * i + 2].GetDouble();
	}
	return result;
}

OldTreeIcon ExternalServicesComponent::getOldTreeIconsFromDocument(ot::JsonDocument &doc)
{
	OldTreeIcon treeIcons;

	treeIcons.size = doc[OT_ACTION_PARAM_UI_TREE_IconSize].GetInt();
	treeIcons.visibleIcon = doc[OT_ACTION_PARAM_UI_TREE_IconItemVisible].GetString();
	treeIcons.hiddenIcon = doc[OT_ACTION_PARAM_UI_TREE_IconItemHidden].GetString();

	return treeIcons;
}

void ExternalServicesComponent::getVectorNodeFromDocument(ot::JsonDocument& doc, const std::string& itemName, std::vector<Geometry::Node>& result) {
	rapidjson::Value nodeList = doc[itemName.c_str()].GetArray();
	int numberOfNodes = nodeList.Size() / 8;
	for (unsigned int i = 0; i < numberOfNodes; i++) {
		Geometry::Node n;
		n.setCoords(nodeList[i * 8].GetDouble(), nodeList[i * 8 + 1].GetDouble(), nodeList[i * 8 + 2].GetDouble());
		n.setNormals(nodeList[i * 8 + 3].GetDouble(), nodeList[i * 8 + 4].GetDouble(), nodeList[i * 8 + 5].GetDouble());
		n.setUVpar(nodeList[i * 8 + 6].GetDouble(), nodeList[i * 8 + 7].GetDouble());
		result.push_back(n);
	}
}

void ExternalServicesComponent::getListTriangleFromDocument(ot::JsonDocument& doc, const std::string& itemName, std::list <Geometry::Triangle >& result) {
	rapidjson::Value triangleList = doc[itemName.c_str()].GetArray();
	int numberOfTriangles = triangleList.Size() / 4;
	for (unsigned int i = 0; i < numberOfTriangles; i++) {
		Geometry::Triangle t(triangleList[i * 4].GetInt64(), triangleList[i * 4 + 1].GetInt64(), triangleList[i * 4 + 2].GetInt64(), triangleList[i * 4 + 3].GetInt64());

		result.push_back(t);
	}
}

void ExternalServicesComponent::getListEdgeFromDocument(ot::JsonDocument& doc, const std::string& itemName, std::list <Geometry::Edge >& result) {
	rapidjson::Value edgeList = doc[itemName.c_str()].GetArray();
	long numberOfEdges = edgeList[0].GetInt64();
	long edgeIndex = 1;

	for (long i = 0; i < numberOfEdges; i++) {
		Geometry::Edge e;
		e.setFaceId(edgeList[edgeIndex].GetInt64());
		edgeIndex++;
		e.setNpoints(edgeList[edgeIndex].GetInt64());
		edgeIndex++;

		for (long np = 0; np < e.getNpoints(); np++) {
			e.setPoint(np, edgeList[edgeIndex].GetDouble(), edgeList[edgeIndex + 1].GetDouble(), edgeList[edgeIndex + 2].GetDouble());
			edgeIndex += 3;
		}

		result.push_back(e);
	}
}

// ###################################################################################################

// Viewer helper functions

void ExternalServicesComponent::getSelectedModelEntityIDs(std::list<ModelUIDtype> &selected) {
	AppBase::instance()->getViewerComponent()->getSelectedModelEntityIDs(selected);
}

void ExternalServicesComponent::getSelectedVisibleModelEntityIDs(std::list<ModelUIDtype> &selected) {
	AppBase::instance()->getViewerComponent()->getSelectedVisibleModelEntityIDs(selected);
}

// ###################################################################################################

// Private functions

void ExternalServicesComponent::removeServiceFromList(std::vector<ServiceDataUi *> &list, ServiceDataUi *service)
{
	auto item = std::find(list.begin(), list.end(), service);

	while (item != list.end())
	{
		list.erase(item);
		item = std::find(list.begin(), list.end(), service);
	}
}

ot::UID ExternalServicesComponent::getServiceUiUid(ServiceDataUi * _service) {
	auto itm = m_serviceToUidMap.find(_service->getServiceName());
	if (itm == m_serviceToUidMap.end()) {
		ot::UID newUID{ ak::uiAPI::createUid() };
		m_serviceToUidMap.insert_or_assign(_service->getServiceName(), newUID);
		return newUID;
	}
	else {
		return itm->second;
	}
}

ServiceDataUi * ExternalServicesComponent::getService(ot::serviceID_t _serviceID) {
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

ServiceDataUi * ExternalServicesComponent::getService(const ot::BasicServiceInformation& _serviceInfo) {

	for (auto s : m_serviceIdMap) {
		if (s.second->getBasicServiceInformation() == _serviceInfo) {
			return s.second;
		}
	}
	OT_LOG_WAS("Failed to find service { \"Service.Name\": \"" + _serviceInfo.serviceName() + "\"; \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }");
	return nullptr;
}

ServiceDataUi* ExternalServicesComponent::getServiceFromName(const std::string& _serviceName) {
	for (auto service : m_serviceIdMap)
	{
		if (service.second->getServiceName() == _serviceName)
		{
			return service.second;
		}
	}

	OT_LOG_WAS("Failed to find service { \"Service.Name\": \"" + _serviceName + "\" }");
	return nullptr;
}

ServiceDataUi* ExternalServicesComponent::getServiceFromNameType(const ot::BasicServiceInformation& _info) {
	return this->getServiceFromNameType(_info.serviceName(), _info.serviceType());
}

ServiceDataUi* ExternalServicesComponent::getServiceFromNameType(const std::string& _serviceName, const std::string& _serviceType) {
	for (auto service : m_serviceIdMap)
	{
		if (service.second->getServiceName() == _serviceName && service.second->getServiceType() == _serviceType)
		{
			return service.second;
		}
	}

	OT_LOG_WAS("Failed to find service { \"Service.Name\": \"" + _serviceName + "\"; \"Service.Type\": \"" + _serviceType + "\" }");
	return nullptr;
}

// ###################################################################################################

// Action handler

std::string ExternalServicesComponent::handleSetLogFlags(ot::JsonDocument& _document) {
	ot::ConstJsonArray flags = ot::json::getArray(_document, OT_ACTION_PARAM_Flags);
	ot::LogDispatcher::instance().setLogFlags(ot::logFlagsFromJsonArray(flags));
	AppBase::instance()->updateLogIntensityInfo();
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ExternalServicesComponent::handleCompound(ot::JsonDocument& _document) {
	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);
	rapidjson::Value documents = _document[OT_ACTION_PARAM_PREFETCH_Documents].GetArray();
	rapidjson::Value prefetchID = _document[OT_ACTION_PARAM_PREFETCH_ID].GetArray();
	rapidjson::Value prefetchVersion = _document[OT_ACTION_PARAM_PREFETCH_Version].GetArray();

	ot::LockTypeFlags lockFlags(ot::LockAll);
	
	ScopedLockManagerLock uiLock(m_lockManager, AppBase::instance()->getBasicServiceInformation(), lockFlags);

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
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(25ms);
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
	
	std::thread exitThread(&ot::intern::exitAsync, ot::AppExitCode::Success);
	exitThread.detach();

	return "";
}

std::string ExternalServicesComponent::handlePreShutdown(ot::JsonDocument& _document) {
	OT_LOG_D("Pre shutdown received");

	return "";
}

std::string ExternalServicesComponent::handleEmergencyShutdown(ot::JsonDocument& _document) {
	AppBase::instance()->showErrorPrompt("An unexpected error occurred and the session needs to be closed.", "Error");
	
	std::thread exitThread(&ot::intern::exitAsync, ot::AppExitCode::EmergencyShutdown);
	exitThread.detach();

	return "";
}

std::string ExternalServicesComponent::handleConnectionLoss(ot::JsonDocument& _document) {
	AppBase::instance()->showErrorPrompt("The session needs to be closed, since the connection to the server has been lost.\n\nPlease note that the project may remain locked for up to two minutes before it can be reopened.", "Error");

	std::thread exitThread(&ot::intern::exitAsync, ot::AppExitCode::LSSNotRunning);
	exitThread.detach();

	return "";
}

std::string ExternalServicesComponent::handleServiceConnected(ot::JsonDocument& _document) {
	ot::serviceID_t senderID = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string senderURL = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_URL);
	std::string senderName = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_NAME);
	std::string senderType = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_TYPE);

	if (senderName.empty()) {
		OT_LOG_EA("Service name is empty");
		return "";
	}

	if (senderType.empty()) {
		OT_LOG_EA("Service type is empty");
		return "";
	}

	// Check for duplicate service names
	for (const auto& it : m_serviceIdMap) {
		if (it.second->getBasicServiceInformation().serviceName() == senderName) {
			OT_LOG_EAS("Service with the name  \"" +  senderName + "\" already registered.");
			return "";
		}
	}

	ServiceDataUi* connectedService = new ServiceDataUi(senderName, senderType, senderURL, senderID);
	m_serviceIdMap.insert_or_assign(senderID, connectedService);

	OT_LOG_D("Service registered { \"ServiceName\": \"" + senderName + "\", \"SenderID\": " + std::to_string(senderID) + " }");

	return "";
}

std::string ExternalServicesComponent::handleServiceDisconnected(ot::JsonDocument& _document) {
	ot::serviceID_t senderID = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string senderURL = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_URL);
	std::string senderName = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_NAME);
	std::string senderType = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_TYPE);

	auto itm = m_serviceIdMap.find(senderID);
	if (itm != m_serviceIdMap.end()) {
		ServiceDataUi* actualService = itm->second;
		assert(actualService != nullptr);

		// Clean up elements
		m_lockManager->cleanService(actualService->getBasicServiceInformation(), false, true);
		m_controlsManager->serviceDisconnected(actualService->getBasicServiceInformation());
		AppBase::instance()->shortcutManager()->creatorDestroyed(actualService);

		// Clean up entry
		m_serviceIdMap.erase(actualService->getServiceID());
		removeServiceFromList(m_modelViewNotifier, actualService);

		OT_LOG_D("Service deregistered { \"ServiceName\": \"" + senderName + "\", \"SenderID\": " + std::to_string(senderID) + " }");

		delete actualService;
	}

	return "";
}

std::string ExternalServicesComponent::handleShutdownRequestedByService(ot::JsonDocument& _document) {
	OTAssert(0, "External shutdown not supported");	// Add external shutdown

	return "";
}

std::string ExternalServicesComponent::handleServiceSetupCompleted(ot::JsonDocument& _document) {
	ot::serviceID_t id = ot::ThisService::getIdFromJsonDocument(_document);
	auto it = m_serviceIdMap.find(id);
	if (it == m_serviceIdMap.end()) {
		OT_LOG_E("Unknown service (" + std::to_string(id) + ")");
		return "";
	}

	it->second->setUiInitializationCompleted();

	// Check if all services completed the startup
	for (const auto it : m_serviceIdMap) {
		if (!it.second->isUiInitializationCompleted()) return "";
	}

	// Here we know that all services completed the startup -> switch to main view and restore state
	m_servicesUiSetupCompleted = true;

	AppBase::instance()->switchToViewMenuTab();
	m_lockManager->unlock(AppBase::instance()->getBasicServiceInformation(), ot::LockAll);

	AppBase::instance()->restoreSessionState();

	return "";
}

std::string ExternalServicesComponent::handleDisplayMessage(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	
	AppBase::instance()->appendInfoMessage(QString::fromStdString(message));

	return "";
}

std::string ExternalServicesComponent::handleDisplayStyledMessage(ot::JsonDocument& _document) {
	ot::StyledTextBuilder builder(ot::json::getObject(_document, OT_ACTION_PARAM_MESSAGE));

	AppBase::instance()->appendHtmlInfoMessage(ot::StyledTextConverter::toHtml(builder));

	return "";
}

std::string ExternalServicesComponent::handleReportError(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	AppBase::instance()->showErrorPrompt(message, "Open Twin");

	return "";
}

std::string ExternalServicesComponent::handleReportWarning(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	AppBase::instance()->showWarningPrompt(message, "Open Twin");

	return "";
}

std::string ExternalServicesComponent::handleReportInformation(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	AppBase::instance()->showInfoPrompt(message, "Open Twin");

	return "";
}

std::string ExternalServicesComponent::handlePromptInformation(ot::JsonDocument& _document) {
	ot::MessageDialogCfg config;
	config.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	std::string promptResponse = ot::json::getString(_document, OT_ACTION_PARAM_RESPONSE);
	std::string sender = ot::json::getString(_document, OT_ACTION_PARAM_SENDER);
	std::string parameter1 = ot::json::getString(_document, OT_ACTION_PARAM_PARAMETER1);

	ot::MessageDialogCfg::BasicButton result = AppBase::instance()->showPrompt(config);

	std::string queryResult = ot::MessageDialogCfg::toString(result);

	ot::JsonDocument docOut;
	docOut.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_PromptResponse, docOut.GetAllocator()), docOut.GetAllocator());
	docOut.AddMember(OT_ACTION_PARAM_RESPONSE, ot::JsonString(promptResponse, docOut.GetAllocator()), docOut.GetAllocator());
	docOut.AddMember(OT_ACTION_PARAM_ANSWER, ot::JsonString(queryResult, docOut.GetAllocator()), docOut.GetAllocator());
	docOut.AddMember(OT_ACTION_PARAM_PARAMETER1, ot::JsonString(parameter1, docOut.GetAllocator()), docOut.GetAllocator());

	if (this->getServiceFromNameType(sender, sender) != nullptr)
	{
		std::string senderUrl = this->getServiceFromNameType(sender, sender)->getServiceURL();

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
	if (s->second->getServiceName() == OT_INFO_SERVICE_TYPE_MODEL)
	{
		m_modelServiceURL = s->second->getServiceURL();
	}
		
	m_modelViewNotifier.push_back(s->second);

	OT_LOG_D("Service with ID \"" + std::to_string(s->second->getServiceID()) + "\" was registered from model view events");

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

	OT_LOG_D("Service with ID \"" + std::to_string(s->second->getServiceID()) + "\" was deregistered from model view events");

	return "";
}

std::string ExternalServicesComponent::handleGenerateUIDs(ot::JsonDocument& _document) {
	int count = _document[OT_ACTION_PARAM_COUNT].GetInt();
	// empty id list
	std::vector<ot::UID> idList;
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

	bool loadContent = false;
	if (_document.HasMember(OT_ACTION_PARAM_FILE_LoadContent)) {
		loadContent = ot::json::getBool(_document, OT_ACTION_PARAM_FILE_LoadContent);
	}

	bool loadMultiple = false;
	if (_document.HasMember(OT_ACTION_PARAM_FILE_LoadMultiple)) {
		loadMultiple = ot::json::getBool(_document, OT_ACTION_PARAM_FILE_LoadMultiple);
	}

	try {
		if (loadMultiple) {
			QStringList fileNames = QFileDialog::getOpenFileNames(
				nullptr,
				dialogTitle.c_str(),
				QDir::currentPath(),
				QString(fileMask.c_str()));

			if (!fileNames.isEmpty()) {
				ot::JsonDocument inDoc;
				inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteFunction, inDoc.GetAllocator()), inDoc.GetAllocator());
				inDoc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, rapidjson::Value(subsequentFunction.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
				inDoc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString(fileMask, inDoc.GetAllocator()), inDoc.GetAllocator());

				ot::JsonArray fileNamesJson, fileContents, fileModes, uncompressedDataLengths;
				{
					std::string progressBarMessage = "Importing files";
					std::unique_ptr<ProgressUpdater> updater(nullptr);
					if (loadContent)
					{
						updater.reset(new ProgressUpdater(progressBarMessage, fileNames.size()));
					}

					uint32_t counter(0);
					std::string message = ("Import of " + std::to_string(fileNames.size()) + " file(s): ");
					AppBase::instance()->appendInfoMessage(QString::fromStdString(message));

					auto startTime = std::chrono::system_clock::now();
					for (QString& fileName : fileNames)
					{
						counter++;
						std::string localEncodingString = fileName.toLocal8Bit().constData();
						const std::string utf8String = fileName.toStdString();

						ot::JsonString fileNameJson(utf8String, inDoc.GetAllocator());
						fileNamesJson.PushBack(fileNameJson, inDoc.GetAllocator());
						if (loadContent)
						{
							std::string fileContent;
							uint64_t uncompressedDataLength{ 0 };
							// The file can not be directly accessed from the remote site and we need to send the file content over the communication
							ReadFileContent(localEncodingString, fileContent, uncompressedDataLength);
							fileContents.PushBack(ot::JsonString(fileContent, inDoc.GetAllocator()), inDoc.GetAllocator());
							uncompressedDataLengths.PushBack(static_cast<int64_t>(uncompressedDataLength), inDoc.GetAllocator());
							fileModes.PushBack(ot::JsonString(OT_ACTION_VALUE_FILE_Mode_Content, inDoc.GetAllocator()), inDoc.GetAllocator());
							assert(updater != nullptr);
							updater->triggerUpdate(counter);
						}
					}
					auto endTime = std::chrono::system_clock::now();
					uint64_t millisec = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
					//AppBase::instance()->("Import of files: " + std::to_string(millisec) + " ms\n");
					message = (std::to_string(millisec) + " ms\n");

					AppBase::instance()->appendInfoMessage(QString::fromStdString(message));
				}
				inDoc.AddMember(OT_ACTION_PARAM_FILE_OriginalName, fileNamesJson, inDoc.GetAllocator());
				if (loadContent) {
					inDoc.AddMember(OT_ACTION_PARAM_FILE_Content, fileContents, inDoc.GetAllocator());
					inDoc.AddMember(OT_ACTION_PARAM_FILE_Content_UncompressedDataLength, uncompressedDataLengths, inDoc.GetAllocator());
					inDoc.AddMember(OT_ACTION_PARAM_FILE_Mode, fileModes, inDoc.GetAllocator());
				}


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
		}
		else
		{

			QString fileName = QFileDialog::getOpenFileName(
				nullptr,
				dialogTitle.c_str(),
				QDir::currentPath(),
				QString(fileMask.c_str()));

			if (fileName != "")
			{
				ot::JsonDocument inDoc;
				inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteFunction, inDoc.GetAllocator()), inDoc.GetAllocator());
				inDoc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, rapidjson::Value(subsequentFunction.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
				inDoc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString(fileMask, inDoc.GetAllocator()), inDoc.GetAllocator());

				if (loadContent)
				{
					std::string fileContent;
					unsigned long long uncompressedDataLength{ 0 };

					std::string localEncodingFileName = fileName.toLocal8Bit().constData();

					// The file can not be directly accessed from the remote site and we need to send the file content over the communication
					ReadFileContent(localEncodingFileName, fileContent, uncompressedDataLength);
					inDoc.AddMember(OT_ACTION_PARAM_FILE_Content, rapidjson::Value(fileContent.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
					inDoc.AddMember(OT_ACTION_PARAM_FILE_Content_UncompressedDataLength, rapidjson::Value(uncompressedDataLength), inDoc.GetAllocator());
					// We need to send the file content
					inDoc.AddMember(OT_ACTION_PARAM_FILE_Mode, rapidjson::Value(OT_ACTION_VALUE_FILE_Mode_Content, inDoc.GetAllocator()), inDoc.GetAllocator());
				}
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
		}
		return ot::ReturnMessage().toJson();
	}
	catch (std::exception& _e)
	{
		 //ToDo: Display here
		return ot::ReturnMessage(ot::ReturnMessage::Failed, _e.what()).toJson();
	}
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
	AppBase::instance()->showInfoPrompt("The file has been successfully saved:\n" + fileName, dialogTitle);
	
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
		QString(fileMask.c_str()));

	if (fileName != "")
	{
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteFunction, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_FILE_OriginalName, rapidjson::Value(fileName.toStdString().c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, rapidjson::Value(subsequentFunction.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString(fileMask, inDoc.GetAllocator()), inDoc.GetAllocator());

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

std::string ExternalServicesComponent::handleAddMenuPage(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

	ServiceDataUi* service = getService(serviceId);

	ot::UID p = AppBase::instance()->getToolBar()->addPage(getServiceUiUid(service), pageName.c_str());
	//NOTE, add corresponding functions in uiServiceAPI
	ak::uiAPI::object::setObjectUniqueName(p, pageName.c_str());
	m_controlsManager->uiElementCreated(service->getBasicServiceInformation(), p, false);

	return "";
}

std::string ExternalServicesComponent::handleAddMenuGroup(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	std::string groupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_GroupName);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

	ServiceDataUi* service = getService(serviceId);

	//NOTE, add corresponding functions in uiServiceAPI
	ot::UID pageUID{ ak::uiAPI::object::getUidFromObjectUniqueName(pageName.c_str()) };
	//NOTE, add error handling
	assert(pageUID != ak::invalidUID);

	ot::UID g = AppBase::instance()->getToolBar()->addGroup(getServiceUiUid(service), pageUID, groupName.c_str());
	//NOTE, add corresponding functions in uiServiceAPI
	ak::uiAPI::object::setObjectUniqueName(g, (pageName + ":" + groupName).c_str());
	m_controlsManager->uiElementCreated(service->getBasicServiceInformation(), g, false);

	return "";
}

std::string ExternalServicesComponent::handleAddMenuSubgroup(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	std::string groupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_GroupName);
	std::string subgroupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_SubgroupName);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	
	ServiceDataUi* service = getService(serviceId);

	//NOTE, add corresponding functions in uiServiceAPI
	ot::UID groupUID{ ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName).c_str()) };
	//NOTE, add error handling
	assert(groupUID != ak::invalidUID);

	ot::UID sg = AppBase::instance()->getToolBar()->addSubGroup(getServiceUiUid(service), groupUID, subgroupName.c_str());
	//NOTE, add corresponding functions in uiServiceAPI
	ak::uiAPI::object::setObjectUniqueName(sg, (pageName + ":" + groupName + ":" + subgroupName).c_str());
	m_controlsManager->uiElementCreated(service->getBasicServiceInformation(), sg, false);

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
	
	ServiceDataUi* senderService = getService(serviceId);
	
	ot::LockTypeFlags flags = (ot::LockAll);

	if (_document.HasMember(OT_ACTION_PARAM_ElementLockTypes)) {
		flags = ot::stringListToLockTypeFlags(ot::json::getStringList(_document, OT_ACTION_PARAM_ElementLockTypes));
		flags.setFlag(ot::LockAll);	// Add the all flag to all external push buttons
	}

	ot::UID parentUID;
	if (subgroupName.length() == 0) {
		parentUID = ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName).c_str());
	}
	else {
		parentUID = ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName + ":" + subgroupName).c_str());
	}
	//NOTE, add error handling
	assert(parentUID != ak::invalidUID);

	// Here we need to pass the iconName as string once the functionality is added to the uiManager
	ot::UID buttonID = AppBase::instance()->getToolBar()->addToolButton(getServiceUiUid(senderService), parentUID, iconName.c_str(), iconFolder.c_str(), text.c_str());

	if (subgroupName.length() == 0) {
		ak::uiAPI::object::setObjectUniqueName(buttonID, (pageName + ":" + groupName + ":" + buttonName).c_str());
	}
	else {
		ak::uiAPI::object::setObjectUniqueName(buttonID, (pageName + ":" + groupName + ":" + subgroupName + ":" + buttonName).c_str());
	}
	m_controlsManager->uiElementCreated(senderService->getBasicServiceInformation(), buttonID);
	m_lockManager->uiElementCreated(senderService->getBasicServiceInformation(), buttonID, flags);

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
	ot::LockTypeFlags flags = ot::stringListToLockTypeFlags(ot::json::getStringList(_document, OT_ACTION_PARAM_ElementLockTypes));
	flags.setFlag(ot::LockAll);	// Add the all flag to all external checkboxes

	ServiceDataUi* service = getService(serviceId);

	//NOTE, add corresponding functions in uiServiceAPI
	ot::UID parentID;
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
	ot::UID boxID = AppBase::instance()->getToolBar()->addCheckBox(getServiceUiUid(service), parentID, boxText.c_str(), checked);

	//NOTE, add corresponding functions in uiServiceAPI
	if (subgroupName.length() > 0) {
		ak::uiAPI::object::setObjectUniqueName(boxID, (pageName + ":" + groupName + ":" + subgroupName + ":" + boxName).c_str());
	}
	else {
		ak::uiAPI::object::setObjectUniqueName(boxID, (pageName + ":" + groupName + ":" + boxName).c_str());
	}

	m_controlsManager->uiElementCreated(service->getBasicServiceInformation(), boxID);
	m_lockManager->uiElementCreated(service->getBasicServiceInformation(), boxID, flags);

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
	ot::LockTypeFlags flags = ot::stringListToLockTypeFlags(ot::json::getStringList(_document, OT_ACTION_PARAM_ElementLockTypes));
	flags.setFlag(ot::LockAll);	// Add the all flag to all external checkboxes

	ServiceDataUi* service = getService(serviceId);
	
	ot::UID parentID;
	if (subgroupName.length() > 0) {
		parentID = ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName + ":" + subgroupName).c_str());
	}
	else {
		parentID = ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName).c_str());
	}

	//NOTE, add error handling
	assert(parentID != ak::invalidUID);

	// Here we need to pass the iconName as string once the functionality is added to the uiManager
	ot::UID editID = AppBase::instance()->getToolBar()->addNiceLineEdit(getServiceUiUid(service), parentID, editLabel.c_str(), editText.c_str());

	//NOTE, add corresponding functions in uiServiceAPI
	if (subgroupName.length() > 0) {
		ak::uiAPI::object::setObjectUniqueName(editID, (pageName + ":" + groupName + ":" + subgroupName + ":" + editName).c_str());
	}
	else {
		ak::uiAPI::object::setObjectUniqueName(editID, (pageName + ":" + groupName + ":" + editName).c_str());
	}
	m_controlsManager->uiElementCreated(service->getBasicServiceInformation(), editID);
	m_lockManager->uiElementCreated(service->getBasicServiceInformation(), editID, flags);

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

	ServiceDataUi* service = getService(serviceId);

	this->addShortcut(service, keySequence);

	return "";
}

std::string ExternalServicesComponent::handleSetCheckboxValue(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string controlName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	bool checked = _document[OT_ACTION_PARAM_UI_CONTROL_CheckedState].GetBool();

	ot::UID objectID = ak::uiAPI::object::getUidFromObjectUniqueName(controlName.c_str());

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

	ot::UID objectID = ak::uiAPI::object::getUidFromObjectUniqueName(controlName.c_str());

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
	AppBase::instance()->switchToMenuTab(pageName);

	return "";
}

std::string ExternalServicesComponent::handleRemoveElements(ot::JsonDocument& _document) {
	std::list<std::string> itemList = ot::json::getStringList(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectNames);
	
	std::vector<ot::UID> uidList;
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

	ServiceDataUi* service = getService(serviceId);
	
	for (auto controlName : enabled)
	{
		//NOTE, add functionallity to uiServiceAPI
		auto uid = ak::uiAPI::object::getUidFromObjectUniqueName(controlName.c_str());
		if (uid != ak::invalidUID)
		{
			m_lockManager->enable(service->getBasicServiceInformation(), uid, true);
		}
	}

	for (auto controlName : disabled)
	{
		//NOTE, add functionallity to uiServiceAPI
		auto uid = ak::uiAPI::object::getUidFromObjectUniqueName(controlName.c_str());
		if (uid != ak::invalidUID)
		{
			m_lockManager->disable(service->getBasicServiceInformation(), uid);
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
	ot::UID visualizationModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	
	AppBase::instance()->getViewerComponent()->resetAllViews3D(visualizationModelID);
	
	return "";
}

std::string ExternalServicesComponent::handleRefreshView(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);

	AppBase::instance()->getViewerComponent()->refreshAllViews(visualizationModelID);
	
	return "";
}

std::string ExternalServicesComponent::handleClearSelection(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	
	AppBase::instance()->getViewerComponent()->clearSelection(visualizationModelID);
	
	return "";
}

std::string ExternalServicesComponent::handleRefreshSelection(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	
	AppBase::instance()->getViewerComponent()->refreshSelection(visualizationModelID);
	
	return "";
}

std::string ExternalServicesComponent::handleSelectObject(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ot::UID entityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	
	AppBase::instance()->getViewerComponent()->selectObject(visualizationModelID, entityID);
	
	return "";
}

std::string ExternalServicesComponent::handleAddNodeFromFacetData(ot::JsonDocument& _document) {
	ot::JsonSizeType tmp;

	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string treeName = ot::json::getString(_document, OT_ACTION_PARAM_UI_TREE_Name);
	double* surfaceColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, tmp);
	double* edgeColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, tmp);
	ot::UID modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
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
	OldTreeIcon treeIcons = getOldTreeIconsFromDocument(_document);

	std::map<ot::UID, std::string> faceNameMap;

	AppBase::instance()->getViewerComponent()->addNodeFromFacetData(visModelID, treeName, surfaceColorRGB, edgeColorRGB, modelEntityID, treeIcons, backFaceCulling,
		offsetFactor, editable, nodes, triangles, edges, faceNameMap, errors, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected);
	
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
	OldTreeIcon treeIcons = getOldTreeIconsFromDocument(_document);
	std::vector<double> transformation;

	if (ot::json::exists(_document, OT_ACTION_PARAM_MODEL_ITM_Transformation))
	{
		transformation = ot::json::getDoubleVector(_document, OT_ACTION_PARAM_MODEL_ITM_Transformation);
	}
	else
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
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string treeName = ot::json::getString(_document, OT_ACTION_PARAM_UI_TREE_Name);
	ModelUIDtype modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	OldTreeIcon treeIcons = getOldTreeIconsFromDocument(_document);
	bool editable = _document[OT_ACTION_PARAM_MODEL_ITM_IsEditable].GetBool();
	ot::VisualisationTypes visualisationTypes;
	visualisationTypes.setFromJsonObject(_document.GetConstObject());
	AppBase::instance()->getViewerComponent()->addVisualizationContainerNode(visModelID, treeName, modelEntityID, treeIcons, editable, visualisationTypes);
	
	return "";
}

std::string ExternalServicesComponent::handleAddSceneNode(ot::JsonDocument& _document)
{
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string treeName = ot::json::getString(_document, OT_ACTION_PARAM_UI_TREE_Name);
	ot::UID modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	OldTreeIcon treeIcons = getOldTreeIconsFromDocument(_document);
	bool editable = _document[OT_ACTION_PARAM_MODEL_ITM_IsEditable].GetBool();
	ot::VisualisationTypes visualisationTypes;
	visualisationTypes.setFromJsonObject(_document.GetConstObject());
	ViewerAPI::addVisualizationNode(visModelID, treeName, modelEntityID, treeIcons, editable,visualisationTypes);

	return "";
}

std::string ExternalServicesComponent::handleAddVis2D3DNode(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string treeName = ot::json::getString(_document, OT_ACTION_PARAM_UI_TREE_Name);
	ModelUIDtype modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	OldTreeIcon treeIcons = getOldTreeIconsFromDocument(_document);
	bool editable = _document[OT_ACTION_PARAM_MODEL_ITM_IsEditable].GetBool();
	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();

	ot::UID visualizationDataID = _document[OT_ACTION_PARAM_MODEL_DataID].GetUint64();
	ot::UID visualizationDataVersion = _document[OT_ACTION_PARAM_MODEL_DataVersion].GetUint64();

	AppBase::instance()->getViewerComponent()->addVisualizationVis2D3DNode(visModelID, treeName, modelEntityID, treeIcons, isHidden, editable, projectName, visualizationDataID, visualizationDataVersion);
	
	return "";
}

std::string ExternalServicesComponent::handleUpdateVis2D3DNode(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ModelUIDtype modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);

	ot::UID visualizationDataID = _document[OT_ACTION_PARAM_MODEL_DataID].GetUint64();
	ot::UID visualizationDataVersion = _document[OT_ACTION_PARAM_MODEL_DataVersion].GetUint64();

	AppBase::instance()->getViewerComponent()->updateVisualizationVis2D3DNode(visModelID, modelEntityID, projectName, visualizationDataID, visualizationDataVersion);
	
	return "";
}

std::string ExternalServicesComponent::handleUpdateColor(ot::JsonDocument& _document) {
	ot::JsonSizeType tmp;

	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
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

	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ModelUIDtype modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	double* colorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, tmp);
	
	AppBase::instance()->getViewerComponent()->updateMeshColor(visModelID, modelEntityID, colorRGB);

	return "";
}

std::string ExternalServicesComponent::handleUpdateFacetsFromDataBase(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ModelUIDtype modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	ModelUIDtype entityID = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	ModelUIDtype entityVersion = _document[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();

	AppBase::instance()->getViewerComponent()->updateObjectFacetsFromDataBase(visModelID, modelEntityID, entityID, entityVersion);
	
	return "";
}

std::string ExternalServicesComponent::handleRemoveShapes(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::list<ot::UID> entityID = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_ITM_ID);
	
	AppBase::instance()->getViewerComponent()->removeShapes(visualizationModelID, entityID);
	
	return "";
}

std::string ExternalServicesComponent::handleTreeStateRecording(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	bool flag = _document[OT_ACTION_PARAM_MODEL_State].GetBool();

	AppBase::instance()->getViewerComponent()->setTreeStateRecording(visualizationModelID, flag);

	return "";
}

std::string ExternalServicesComponent::handleSetShapeVisibility(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::list<ot::UID> visibleID = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_ITM_ID_Visible);
	std::list<ot::UID> hiddenID = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_ITM_ID_Hidden);
	
	AppBase::instance()->getViewerComponent()->setShapeVisibility(visualizationModelID, visibleID, hiddenID);
	
	return "";
}

std::string ExternalServicesComponent::handleHideEntities(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::list<ot::UID> hiddenID = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_ITM_ID_Hidden);

	AppBase::instance()->getViewerComponent()->hideEntities(visualizationModelID, hiddenID);
	
	return "";
}

std::string ExternalServicesComponent::handleShowBranch(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string branchName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_BRANCH);

	AppBase::instance()->getViewerComponent()->showBranch(visualizationModelID, branchName);
	
	return "";
}

std::string ExternalServicesComponent::handleHideBranch(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string branchName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_BRANCH);

	AppBase::instance()->getViewerComponent()->hideBranch(visualizationModelID, branchName);
	
	return "";
}

std::string ExternalServicesComponent::handleAddAnnotationNode(ot::JsonDocument& _document) {
	ot::JsonSizeType tmp;

	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	ot::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	double* edgeColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, tmp);
	std::vector<std::array<double, 3>> points = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points);
	std::vector<std::array<double, 3>> points_rgb = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points_Colors);
	std::vector<std::array<double, 3>> triangle_p1 = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p1);
	std::vector<std::array<double, 3>> triangle_p2 = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p2);
	std::vector<std::array<double, 3>> triangle_p3 = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p3);
	std::vector<std::array<double, 3>> triangle_rgb = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_Color);
	OldTreeIcon treeIcons = getOldTreeIconsFromDocument(_document);

	AppBase::instance()->getViewerComponent()->addVisualizationAnnotationNode(visModelID, name, uid, treeIcons, isHidden, edgeColorRGB, points, points_rgb, triangle_p1, triangle_p2, triangle_p3, triangle_rgb);
	
	return "";
}

std::string ExternalServicesComponent::handleAddAnnotationNodeFromDataBase(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	ot::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);
	ot::UID entityID = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	ot::UID entityVersion = _document[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
	OldTreeIcon treeIcons = getOldTreeIconsFromDocument(_document);

	ViewerAPI::addVisualizationAnnotationNodeDataBase(visModelID, name, uid, treeIcons, isHidden, projectName, entityID, entityVersion);

	return "";
}

std::string ExternalServicesComponent::handleAddMeshNodeFromFacetDataBase(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	ot::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	double edgeColorRGB[3];
	edgeColorRGB[0] = _document[OT_ACTION_PARAM_MODEL_EDGE_COLOR_R].GetDouble();
	edgeColorRGB[1] = _document[OT_ACTION_PARAM_MODEL_EDGE_COLOR_G].GetDouble();
	edgeColorRGB[2] = _document[OT_ACTION_PARAM_MODEL_EDGE_COLOR_B].GetDouble();
	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);
	ot::UID entityID = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	ot::UID entityVersion = _document[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
	bool displayTetEdges = _document[OT_ACTION_PARAM_MODEL_TET_DISPLAYEDGES].GetBool();
	OldTreeIcon treeIcons = getOldTreeIconsFromDocument(_document);

	ViewerAPI::addVisualizationMeshNodeFromFacetDataBase(visModelID, name, uid, treeIcons, edgeColorRGB, displayTetEdges, projectName, entityID, entityVersion);
	
	return "";
}

std::string ExternalServicesComponent::handleAddCartesianMeshNode(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	ot::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
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
	ot::UID faceListEntityID = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	ot::UID faceListEntityVersion = _document[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
	ot::UID nodeListEntityID = _document[OT_ACTION_PARAM_MESH_NODE_ID].GetUint64();
	ot::UID nodeListEntityVersion = _document[OT_ACTION_PARAM_MESH_NODE_VERSION].GetUint64();
	OldTreeIcon treeIcons = getOldTreeIconsFromDocument(_document);

	ViewerAPI::addVisualizationCartesianMeshNode(visModelID, name, uid, treeIcons, isHidden, edgeColorRGB, meshLineColorRGB, showMeshLines, meshCoordsX, meshCoordsY, meshCoordsZ, projectName, faceListEntityID, faceListEntityVersion, nodeListEntityID, nodeListEntityVersion);
	
	return "";
}

std::string ExternalServicesComponent::handleCartesianMeshNodeShowLines(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ot::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	bool showMeshLines = _document[OT_ACTION_PARAM_MESH_ShowMeshLines].GetBool();

	ViewerAPI::visualizationCartesianMeshNodeShowLines(visModelID, uid, showMeshLines);
	
	return "";
}

std::string ExternalServicesComponent::handleAddCartesianMeshItem(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	ot::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	std::vector<int> facesList = ot::json::getIntVector(_document, OT_ACTION_PARAM_MODEL_ITM_FacesList);
	OldTreeIcon treeIcons = getOldTreeIconsFromDocument(_document);
	double colorRGB[3];
	colorRGB[0] = _document[OT_ACTION_PARAM_MESH_ITEM_COLOR_R].GetDouble();
	colorRGB[1] = _document[OT_ACTION_PARAM_MESH_ITEM_COLOR_G].GetDouble();
	colorRGB[2] = _document[OT_ACTION_PARAM_MESH_ITEM_COLOR_B].GetDouble();

	ViewerAPI::addVisualizationCartesianMeshItemNode(visModelID, name, uid, treeIcons, isHidden, facesList, colorRGB);

	return "";
}

std::string ExternalServicesComponent::handleTetMeshNodeTetEdges(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ot::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	bool displayMeshEdges = _document[OT_ACTION_PARAM_MODEL_TET_DISPLAYEDGES].GetBool();
	
	ViewerAPI::visualizationTetMeshNodeTetEdges(visModelID, uid, displayMeshEdges);

	return "";
}

std::string ExternalServicesComponent::handleAddMeshItemFromFacetDatabase(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	ot::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);
	ot::UID entityID = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	ot::UID entityVersion = _document[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
	OldTreeIcon treeIcons = getOldTreeIconsFromDocument(_document);
	long long tetEdgesID = _document[OT_ACTION_PARAM_MODEL_TETEDGES_ID].GetUint64();;
	long long tetEdgesVersion = _document[OT_ACTION_PARAM_MODEL_TETEDGES_Version].GetUint64();

	ViewerAPI::addVisualizationMeshItemNodeFromFacetDataBase(visModelID, name, uid, treeIcons, isHidden, projectName, entityID, entityVersion, tetEdgesID, tetEdgesVersion);
	
	return "";
}

std::string ExternalServicesComponent::handleEnterEntitySelectionMode(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
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
	ot::UID visModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
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
	ot::UID visModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
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
	if (!sendHttpRequest(QUEUE, service->second->getServiceURL(), docOut, response)) {
		throw std::exception("Failed to send http request");
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::string ex(OT_ACTION_RETURN_INDICATOR_Error "From ");
		ex.append(service->second->getServiceURL()).append(": ").append(response);
		throw std::exception(ex.c_str());
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::string ex(OT_ACTION_RETURN_INDICATOR_Error "From ");
		ex.append(service->second->getServiceURL()).append(": ").append(response);
		throw std::exception(ex.c_str());
	}

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
	ot::UID entityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ITM_ID);
	std::string entityName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Name);

	ViewerAPI::setEntityName(entityID, entityName);

	return "";
}

std::string ExternalServicesComponent::handleRenameEntity(ot::JsonDocument& _document) {
	std::string fromPath = ot::json::getString(_document, OT_ACTION_PARAM_PATH_FROM);
	std::string toPath = ot::json::getString(_document, OT_ACTION_PARAM_PATH_To);

	AppBase::instance()->renameEntity(fromPath, toPath);

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
	ot::LockTypeFlags flags = ot::stringListToLockTypeFlags(ot::json::getStringList(_document, OT_ACTION_PARAM_ElementLockTypes));
	m_lockManager->lock(getService(serviceId)->getBasicServiceInformation(), flags);

	return "";
}

std::string ExternalServicesComponent::handleUnlock(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	ot::LockTypeFlags flags = ot::stringListToLockTypeFlags(ot::json::getStringList(_document, OT_ACTION_PARAM_ElementLockTypes));
	m_lockManager->unlock(getService(serviceId)->getBasicServiceInformation(), flags);

	return "";
}

std::string ExternalServicesComponent::handleAddSettingsData(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

	ot::PropertyGridCfg config;
	config.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	ServiceDataUi* service = getService(serviceId);
	if (service) {
		UserSettings::instance().addSettings(service->getBasicServiceInformation().serviceName(), config);
	}
	else {
		OT_LOG_EAS("Service with the ID (" + std::to_string(serviceId) + ") is not registered");
		AppBase::instance()->appendInfoMessage("[ERROR] Dispatch: " OT_ACTION_CMD_UI_AddSettingsData ": Service not registered");
	}

	return "";
}

std::string ExternalServicesComponent::handleAddIconSearchPath(ot::JsonDocument& _document) {
#ifdef _DEBUG
	std::string iconPath = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_IconFolder);
	try {
		ot::IconManager::addSearchPath(QString::fromStdString(iconPath));
		OT_LOG_D("Icon search path added: \"" + iconPath + "\"");
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS("[ERROR] Failed to add icon search path: " + std::string(_e.what()));
	}
#endif // _DEBUG

	return "";
}

std::string ExternalServicesComponent::handleGetDebugInformation(ot::JsonDocument& _document) {

	return "";
}

// Property Grid

std::string ExternalServicesComponent::handleFillPropertyGrid(ot::JsonDocument& _document) {
	ot::PropertyGridCfg cfg;
	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);
	cfg.setFromJsonObject(cfgObj);
	AppBase::instance()->setupPropertyGrid(cfg);
	return "";
}

std::string ExternalServicesComponent::handleClearModalPropertyGrid(ot::JsonDocument& _document) {
	AppBase::instance()->clearModalPropertyGrid();
	return "";
}

std::string ExternalServicesComponent::handleFocusPropertyGridItem(ot::JsonDocument& _document) {
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_PROPERTY_Name);
	std::string group = ot::json::getString(_document, OT_ACTION_PARAM_PROPERTY_Group);
	AppBase::instance()->focusPropertyGridItem(group, name);
	return "";
}

// Version Graph

std::string ExternalServicesComponent::handleSetVersionGraph(ot::JsonDocument& _document) {
	ot::VersionGraphManagerView* graphManager = AppBase::instance()->getVersionGraph();
	if (!graphManager) {
		OT_LOG_E("Version graph does not exist");
		return OT_ACTION_RETURN_INDICATOR_Error "Version graph not created";
	}

	ot::VersionGraphCfg config;
	config.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	config.setActiveVersionName(ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_ACTIVE));
	config.setActiveBranchName(ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_BRANCH));

	graphManager->getVersionGraphManager()->setupConfig(std::move(config));
		
	return "";
}

std::string ExternalServicesComponent::handleSetVersionGraphActive(ot::JsonDocument& _document) {
	ot::VersionGraphManagerView* graphManager = AppBase::instance()->getVersionGraph();
	if (!graphManager) {
		OT_LOG_E("Version graph does not exist");
		return OT_ACTION_RETURN_INDICATOR_Error "Version graph not created";
	}

	std::string activeVersion = ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_ACTIVE);
	std::string activeBranch = ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_BRANCH);

	OT_LOG_D("Activating version { \"Version\": \"" + activeVersion + "\", \"Branch\": \"" + activeBranch + "\" }");

	graphManager->getVersionGraphManager()->activateVersion(activeVersion, activeBranch);

	return "";
}

std::string ExternalServicesComponent::handleRemoveVersionGraphVersions(ot::JsonDocument& _document) {
	ot::VersionGraphManagerView* graphManager = AppBase::instance()->getVersionGraph();
	if (!graphManager) {
		OT_LOG_E("Version graph does not exist");
		return OT_ACTION_RETURN_INDICATOR_Error "Version graph not created";
	}

	std::list<std::string> versions = ot::json::getStringList(_document, OT_ACTION_PARAM_List);

	graphManager->getVersionGraphManager()->removeVersions(versions);

	return "";
}

std::string ExternalServicesComponent::handleAddAndActivateVersionGraphVersion(ot::JsonDocument& _document) {
	ot::VersionGraphManagerView* graphManager = AppBase::instance()->getVersionGraph();
	if (!graphManager) {
		OT_LOG_E("Version graph does not exist");
		return OT_ACTION_RETURN_INDICATOR_Error "Version graph not created";
	}

	std::string activeBranch = ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_BRANCH);
	ot::VersionGraphVersionCfg* newVersion = graphManager->getVersionGraphManager()->insertVersion(ot::json::getObject(_document, OT_ACTION_PARAM_Config));
	if (newVersion) {
		graphManager->getVersionGraphManager()->activateVersion(newVersion->getName(), activeBranch);
	}

	return "";
}

// Graphics Editor	

std::string ExternalServicesComponent::handleFillGraphicsPicker(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	ot::GraphicsPickerCollectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	AppBase::instance()->addGraphicsPickerPackage(pckg, info);

	return "";
}

std::string ExternalServicesComponent::handleCreateGraphicsEditor(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	ot::GraphicsNewEditorPackage pckg("", "");
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	AppBase::instance()->addGraphicsPickerPackage(pckg, info);

	ot::WidgetView::InsertFlags insertFlags(ot::WidgetView::NoInsertFlags);
	ot::GraphicsViewView* view = AppBase::instance()->findOrCreateGraphicsEditor(pckg.name(), QString::fromStdString(pckg.title()), info, insertFlags);

	return "";
}

std::string ExternalServicesComponent::handleAddGraphicsItem(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	ot::GraphicsScenePackage pckg("");
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	ot::WidgetView::InsertFlags insertFlags(ot::WidgetView::NoInsertFlags);
	ot::GraphicsViewView* editor = AppBase::instance()->findOrCreateGraphicsEditor(pckg.name(), QString::fromStdString(pckg.name()), info, insertFlags);

	for (auto graphicsItemCfg : pckg.items()) {
		ot::GraphicsItem* graphicsItem = ot::GraphicsItemFactory::itemFromConfig(graphicsItemCfg, true);
		if (graphicsItem != nullptr) {
			//const double xCoordinate = graphicsItemCfg->getPosition().x();
			//const double yCoordinate = graphicsItemCfg->getPosition().y();
			//graphicsItem->getQGraphicsItem()->setPos(QPointF(xCoordinate, yCoordinate));
			editor->getGraphicsView()->addItem(graphicsItem);
		}
	}

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(editor, true);
	
	return "";
}

std::string ExternalServicesComponent::handleRemoveGraphicsItem(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	ot::UIDList itemUids = ot::json::getUInt64List(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemIds);

	if (_document.HasMember(OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName)) {
		// Specific view

		std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);

		ot::WidgetView::InsertFlags insertFlags(ot::WidgetView::NoInsertFlags);
		ot::GraphicsViewView* editor = AppBase::instance()->findOrCreateGraphicsEditor(editorName, QString::fromStdString(editorName), info, insertFlags);

		if (editor) {
			for (auto itemUID : itemUids) {
				editor->getGraphicsView()->removeItem(itemUID);
			}
		}
	}
	else {
		// Any view

		std::list<ot::GraphicsViewView*> views = AppBase::instance()->getAllGraphicsEditors();
		for (auto v : views) {
			for (auto uid : itemUids) {
				v->getGraphicsView()->removeItem(uid);
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

	ot::WidgetView::InsertFlags insertFlags(ot::WidgetView::NoInsertFlags);
	ot::GraphicsViewView* editor = AppBase::instance()->findOrCreateGraphicsEditor(pckg.name(), QString::fromStdString(pckg.name()), info, insertFlags);

	for (const auto& connection : pckg.connections()) {
		editor->getGraphicsView()->addConnectionIfConnectedItemsExist(connection);
	}

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(editor, true);
	
	return "";
}

std::string ExternalServicesComponent::handleRemoveGraphicsConnection(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	ot::GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	if (!pckg.name().empty()) {
		// Specific editor
		ot::WidgetView::InsertFlags insertFlags(ot::WidgetView::NoInsertFlags);
		ot::GraphicsViewView* editor = AppBase::instance()->findOrCreateGraphicsEditor(pckg.name(), QString::fromStdString(pckg.name()), info, insertFlags);

		for (auto connection : pckg.connections()) {
			editor->getGraphicsView()->removeConnection(connection.getUid());
		}
	}
	else {
		// Any editor

		std::list<ot::GraphicsViewView*> views = AppBase::instance()->getAllGraphicsEditors();
		for (auto view : views) {
			for (auto connection : pckg.connections()) {
				view->getGraphicsView()->removeConnection(connection.getUid());
			}
		}
	}

	return "";
}

// Plot

std::string ExternalServicesComponent::handleAddPlot1D_New(ot::JsonDocument& _document) {
	// Get infos from message document
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	ot::WidgetView::InsertFlags insertFlags(ot::WidgetView::NoInsertFlags);
	if (!ot::json::getBool(_document, OT_ACTION_PARAM_VIEW_SetActiveView)) {
		insertFlags |= ot::WidgetView::KeepCurrentFocus;
	}

	bool refreshData = ot::json::getBool(_document, OT_ACTION_PARAM_OverwriteContent);
	
	// Get/create plot view that matches the plot config 
	ot::Plot1DCfg plotConfig;
	plotConfig.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));
	
	const ot::PlotView* plotView = AppBase::instance()->findOrCreatePlot(plotConfig, info, insertFlags);
	ot::Plot* plot = plotView->getPlot();

	// If the data needs to be refreshed, all curves are newly build. Other changes can be performed on already loaded curves.
	if (refreshData)
	{
		// Clear plot if exists
		plot->clear(true);

		// Create curves
		const std::string collectionName = AppBase::instance()->getCollectionName();
		CurveDatasetFactory curveFactory(collectionName);

		ot::ConstJsonArray curveCfgs = ot::json::getArray(_document, OT_ACTION_PARAM_VIEW1D_CurveConfigs);
		std::list<ot::PlotDataset*> dataSets;
		std::list<std::string> curveIDDescriptions;

		const std::string xAxisParameter = plotConfig.getXAxisParameter();
		const std::list<ValueComparisionDefinition>& queries = plotConfig.getQueries();
		bool useLimitedNbOfCurves = plotConfig.getUseLimitNbOfCurves();
		int32_t limitOfCurves = plotConfig.getLimitOfCurves();

		for (uint32_t i = 0; i < curveCfgs.Size(); i++) {
			ot::ConstJsonObject curveCfgSerialised = ot::json::getObject(curveCfgs, i);
			const std::string t = ot::json::toJson(curveCfgs);
			ot::Plot1DCurveCfg curveCfg;

			curveCfg.setFromJsonObject(curveCfgSerialised);

			const ot::QueryInformation& queryInformation = curveCfg.getQueryInformation();
			bool curveHasDataToVisualise = false;
			if (xAxisParameter != "") {
				for (auto parameter : queryInformation.m_parameterDescriptions) {
					if (parameter.m_label == xAxisParameter) {
						curveHasDataToVisualise = true;
					}
				}
			}
			else {
				curveHasDataToVisualise = true;
			}

			if (curveHasDataToVisualise) {
				std::list<ot::PlotDataset*> newCurveDatasets = curveFactory.createCurves(plotConfig, curveCfg, xAxisParameter, queries);
				dataSets.splice(dataSets.begin(), newCurveDatasets);
				
				std::list<std::string> newCurveIDDescriptions = curveFactory.getCurveIDDescriptions();
				curveIDDescriptions.splice(curveIDDescriptions.begin(), newCurveIDDescriptions);
								
				if (useLimitedNbOfCurves && dataSets.size() > limitOfCurves) {
					break;
				}
			}
			else
			{
				AppBase::instance()->appendInfoMessage(QString(("Curve " + curveCfg.getTitle() + " cannot be visualised since it does not have data for the selected X-Axis parameter: " + xAxisParameter + "\n").c_str()));
			}
		}

		//Now we add the data sets to the plot and visualise them
		int32_t curveCounter(1);
		plot->setConfig(plotConfig);
		std::string displayMessage("");
		auto curveIDDescription = curveIDDescriptions.begin();

		for (ot::PlotDataset* dataSet : dataSets) {
			if (!useLimitedNbOfCurves || (useLimitedNbOfCurves && curveCounter <= limitOfCurves))
			{
				dataSet->setOwnerPlot(plot);
				dataSet->updateCurveVisualization();
				plot->addDatasetToCache(dataSet);
				dataSet->attach();
				if (curveIDDescription != curveIDDescriptions.end() && !curveIDDescription->empty())
				{
					displayMessage += *curveIDDescription;
					curveIDDescription++;
				}
			}
			else
			{
				delete dataSet;
				dataSet = nullptr;
			}
			curveCounter++;
		}
		if (!displayMessage.empty())
		{
			AppBase::instance()->appendInfoMessage(QString::fromStdString(displayMessage));
		}
	}
	else
	{
		const ot::Plot1DCfg& oldConfig = plot->getPlot()->getConfiguration();
		if (plotConfig.getXLabelAxisAutoDetermine())
		{
			plotConfig.setAxisLabelX(oldConfig.getAxisLabelX());
		}
		if (plotConfig.getYLabelAxisAutoDetermine())
		{
			plotConfig.setAxisLabelY(oldConfig.getAxisLabelY());
		}
		plot->setConfig(plotConfig);
	}
	// Now we refresh the plot visualisation.
	plot->refresh();
	plot->resetView();
	
	// Lastly we notify the scene nodes that they have a state change to view opened.
	const auto& viewerType = plotView->getViewData().getViewType();
	ot::UID globalActiveViewModel = -1;
	ViewerAPI::notifySceneNodeAboutViewChange(globalActiveViewModel, plotConfig.getEntityName(), ot::ViewChangedStates::viewOpened, viewerType);

	return "";
}

std::string ExternalServicesComponent::handleUpdateCurve(ot::JsonDocument& _document) {
	const std::string plotName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);
	const ot::PlotView* plotView = AppBase::instance()->findPlot(plotName);

	if (plotView != nullptr)
	{
		ot::Plot1DCurveCfg config;
		config.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_VIEW1D_CurveConfigs));
		ot::Plot* plot = plotView->getPlot();
		const std::list<ot::PlotDataset*>& allDatasets = plot->getAllDatasets();

		for (ot::PlotDataset* dataSet : allDatasets) {
			if (dataSet->getEntityName() == config.getEntityName()) {
				const std::string curveNameBase =  dataSet->getCurveNameBase();
				
				const std::string newNameFull = dataSet->getConfig().getEntityName();
				const std::string newNameShort = ot::EntityName::getSubName(newNameFull).value();

				std::string curveTitle = dataSet->getConfig().getTitle();
				
				curveTitle = ot::String::replace(curveTitle, curveNameBase, newNameShort);

				ot::Plot1DCurveCfg curveCfg = dataSet->getConfig();
				curveCfg.setTitle(curveTitle);
				
				dataSet->setConfig(curveCfg);
				dataSet->setCurveNameBase(newNameShort);

				dataSet->updateCurveVisualization();
			}
		}
		
		plot->refresh();
	}
	else
	{
		OT_LOG_E("Requested curve update could not identify the corresponding plot");
	}

	return "";
}

// Text Editor

std::string ExternalServicesComponent::handleSetupTextEditor(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	ot::WidgetView::InsertFlags insertFlags(ot::WidgetView::NoInsertFlags);
	if (!ot::json::getBool(_document, OT_ACTION_PARAM_VIEW_SetActiveView)) {
		insertFlags |= ot::WidgetView::KeepCurrentFocus;
	}
	
	ot::TextEditorCfg config;
	config.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	const bool overwriteContent = ot::json::getBool(_document, OT_ACTION_PARAM_OverwriteContent);
	ot::TextEditorView* editor = AppBase::instance()->findTextEditor(config.getEntityName());
	if (editor) {
		editor->getTextEditor()->setupFromConfig(config, overwriteContent);

		if (!(insertFlags & ot::WidgetView::KeepCurrentFocus)) {
			AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(editor, true);
		}
	}
	else {
		editor = AppBase::instance()->createNewTextEditor(config, info, insertFlags);
	}
	
	editor->getTextEditor()->setContentSaved();
	const std::string& name = editor->getViewData().getEntityName();
	const auto& viewerType = editor->getViewData().getViewType();
	ot::UID globalActiveViewModel = -1;
	ViewerAPI::notifySceneNodeAboutViewChange(globalActiveViewModel, name, ot::ViewChangedStates::viewOpened, viewerType);

	return "";
}

std::string ExternalServicesComponent::handleSetTextEditorSaved(ot::JsonDocument& _document) {
	std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Name);
	ot::TextEditorView* editor = AppBase::instance()->findTextEditor(editorName);

	if (editor) {
		editor->getTextEditor()->setContentSaved();
	}

	return "";
}

std::string ExternalServicesComponent::handleSetTextEditorModified(ot::JsonDocument& _document) {
	std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Name);
	ot::TextEditorView* editor = AppBase::instance()->findTextEditor(editorName);

	if (editor) {
		editor->getTextEditor()->setContentChanged();
	}

	return "";
}

std::string ExternalServicesComponent::handleCloseTextEditor(ot::JsonDocument& _document) {
	std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Name);
	AppBase::instance()->closeTextEditor(editorName);

	return "";
}

std::string ExternalServicesComponent::handleCloseAllTextEditors(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	AppBase::instance()->closeAllTextEditors(info);

	return "";
}

// Table

std::string ExternalServicesComponent::handleSetupTable(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.GetConstObject());

	ot::WidgetView::InsertFlags insertFlags(ot::WidgetView::NoInsertFlags);
	if (_document.HasMember(OT_ACTION_PARAM_VIEW_SetActiveView)) {
		if (!ot::json::getBool(_document, OT_ACTION_PARAM_VIEW_SetActiveView)) {
			insertFlags |= ot::WidgetView::KeepCurrentFocus;
		}
	}
	
	bool overrideCurrentContent = true;
	if (_document.HasMember(OT_ACTION_PARAM_OverwriteContent)) {
		overrideCurrentContent = ot::json::getBool(_document, OT_ACTION_PARAM_OverwriteContent);
	}
	
	bool keepCurrentEntitySelection = false;
	if (_document.HasMember(OT_ACTION_PARAM_KeepCurrentEntitySelection)) {
		keepCurrentEntitySelection = ot::json::getBool(_document, OT_ACTION_PARAM_KeepCurrentEntitySelection);
	}

	ot::TableCfg config;
	config.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	AppBase::ViewHandlingFlags viewHandlingFlags = AppBase::instance()->getViewHandlingConfigFlags();
	if (keepCurrentEntitySelection) {
		AppBase::instance()->setViewHandlingConfigFlags(viewHandlingFlags | AppBase::ViewHandlingConfig::SkipEntitySelection);
	}

	ot::TableView* table = AppBase::instance()->findTable(config.getEntityName());
	if (table == nullptr) {
		table = AppBase::instance()->createNewTable(config, info, insertFlags);
	}
	else if (overrideCurrentContent) {
		table->getTable()->setupFromConfig(config);
		if (!(insertFlags & ot::WidgetView::KeepCurrentFocus)) {
			AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);
		}
		table->getTable()->setContentChanged(false);
	}
	
	AppBase::instance()->setViewHandlingConfigFlags(viewHandlingFlags);

	const std::string& name = table->getViewData().getEntityName();
	const auto& viewerType = table->getViewData().getViewType();
	ot::UID globalActiveViewModel = -1;
	ViewerAPI::notifySceneNodeAboutViewChange(globalActiveViewModel, name, ot::ViewChangedStates::viewOpened, viewerType);

	return "";
}

std::string ExternalServicesComponent::handleSetTableSaved(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::TableView* table = AppBase::instance()->findTable(tableName);
	if (table == nullptr) {
		OT_LOG_EAS("Table \"" + tableName + "\" not found");
		return "";
	}

	table->getTable()->setContentChanged(false);
	return "";
}

std::string ExternalServicesComponent::handleSetTableModified(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::TableView* table = AppBase::instance()->findTable(tableName);
	if (table == nullptr) {
		OT_LOG_EAS("Table \"" + tableName + "\" not found");
		return "";
	}

	table->getTable()->setContentChanged(true);
	return "";
}

std::string ExternalServicesComponent::handleInsertTableRowAfter(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::TableView* table = AppBase::instance()->findTable(tableName);
	if (table == nullptr) {
		OT_LOG_EAS("Table \"" + tableName + "\" not found");
		return "";
	}

	int rowIndex = ot::json::getInt(_document, OT_ACTION_PARAM_Index);
	table->getTable()->insertRow(rowIndex + 1);
	table->getTable()->setContentChanged(true);

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);
	
	return "";
}

std::string ExternalServicesComponent::handleInsertTableRowBefore(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::TableView* table = AppBase::instance()->findTable(tableName);
	if (table == nullptr) {
		OT_LOG_EAS("Table \"" + tableName + "\" not found");
		return "";
	}

	int rowIndex = ot::json::getInt(_document, OT_ACTION_PARAM_Index);
	table->getTable()->insertRow(rowIndex);
	table->getTable()->setContentChanged(true);

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);

	return "";
}

std::string ExternalServicesComponent::handleRemoveTableRow(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::TableView* table = AppBase::instance()->findTable(tableName);
	if (table == nullptr) {
		OT_LOG_EAS("Table \"" + tableName + "\" not found");
		return "";
	}

	int rowIndex = ot::json::getInt(_document, OT_ACTION_PARAM_Index);
	table->getTable()->removeRow(rowIndex);
	table->getTable()->setContentChanged(true);

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);

	return "";
}

std::string ExternalServicesComponent::handleInsertTableColumnAfter(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::TableView* table = AppBase::instance()->findTable(tableName);
	if (table == nullptr) {
		OT_LOG_EAS("Table \"" + tableName + "\" not found");
		return "";
	}

	int columnIndex = ot::json::getInt(_document, OT_ACTION_PARAM_Index);
	table->getTable()->insertColumn(columnIndex + 1);
	table->getTable()->setContentChanged(true);

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);

	return "";
}

std::string ExternalServicesComponent::handleInsertTableColumnBefore(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::TableView* table = AppBase::instance()->findTable(tableName);
	if (table == nullptr) {
		OT_LOG_EAS("Table \"" + tableName + "\" not found");
		return "";
	}

	int columnIndex = ot::json::getInt(_document, OT_ACTION_PARAM_Index);
	table->getTable()->insertColumn(columnIndex);
	table->getTable()->setContentChanged(true);
	
	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);

	return "";
}

std::string ExternalServicesComponent::handleRemoveTableColumn(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::TableView* table = AppBase::instance()->findTable(tableName);
	if (table == nullptr) {
		OT_LOG_EAS("Table \"" + tableName + "\" not found");
		return "";
	}

	int columnIndex = ot::json::getInt(_document, OT_ACTION_PARAM_Index);
	table->getTable()->removeColumn(columnIndex);
	table->getTable()->setContentChanged(true);

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);

	return "";
}

std::string ExternalServicesComponent::handleCloseTable(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	AppBase::instance()->closeTable(tableName);

	return "";
}

std::string ExternalServicesComponent::handleSetTableSelection(ot::JsonDocument& _document) {
	// Get parameters
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	// Get ranges
	ot::ConstJsonObjectList rangesList = ot::json::getObjectList(_document, OT_ACTION_PARAM_Ranges);
	std::vector<ot::TableRange> ranges;
	ranges.reserve(rangesList.size());
	
	for (const ot::ConstJsonObject& rangeObject : rangesList) {
		ot::TableRange range;
		range.setFromJsonObject(rangeObject);
		ranges.push_back(range);
	}

	// Optional parameters
	bool clearSelection = false;
	if (_document.HasMember(OT_ACTION_PARAM_ClearSelection)) {
		clearSelection = ot::json::getBool(_document, OT_ACTION_PARAM_ClearSelection);
	}

	// Get table
	ot::TableView* table = AppBase::instance()->findTable(tableName);

	if (!table) {
		OT_LOG_EAS("Table \"" + tableName + "\" does not exist");
		return "";
	}

	// Apply selection
	if (clearSelection) {
		table->getTable()->clearSelection();
	}
	for (const ot::TableRange& range : ranges) {		
		table->getTable()->setRangeSelected(ot::QtFactory::toQTableRange(range), true);
	}

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);

	return "";
}

std::string ExternalServicesComponent::handleGetTableSelection(ot::JsonDocument& _document) {
	// Get parameters
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);
	std::string senderURL = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);
	std::string subsequentFunction = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_FunctionName);

	// Get table
	ot::TableView* table = AppBase::instance()->findTable(tableName);

	if (!table) {
		OT_LOG_EAS("Table \"" + tableName + "\" does not exist");
		return "";
	}

	this->sendTableSelectionInformation(senderURL, subsequentFunction, table);

	return "";
}

std::string ExternalServicesComponent::handleSetCurrentTableSelectionBackground(ot::JsonDocument& _document) {
	
	// Get parameters
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::Color color;
	color.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Color));

	// Get optional parameters
	bool callback = false;
	std::string callbackUrl;
	std::string callbackFunction;

	if (_document.HasMember(OT_ACTION_PARAM_RequestCallback)) {
		callback = ot::json::getBool(_document, OT_ACTION_PARAM_RequestCallback);
	}
	if (callback) {
		callbackUrl = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);
		callbackFunction = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_FunctionName);
	}

	// Optional parameters
	bool clearSelection = false;
	if (_document.HasMember(OT_ACTION_PARAM_ClearSelection)) {
		clearSelection = ot::json::getBool(_document, OT_ACTION_PARAM_ClearSelection);
	}
	bool clearSelectionAfter = false;
	if (_document.HasMember(OT_ACTION_PARAM_ClearSelectionAfter)) {
		clearSelectionAfter = ot::json::getBool(_document, OT_ACTION_PARAM_ClearSelectionAfter);
	}

	OT_LOG_D("Set Table range background optionals: callback=" + std::to_string(callback) + " ,callback url=" + callbackUrl + " ,callback function=" + callbackFunction + 
		" ,clearSelection=" + std::to_string(clearSelection) + " ,clear selection after=" + std::to_string(clearSelectionAfter));


	std::vector<ot::TableRange> ranges;
	if (_document.HasMember(OT_ACTION_PARAM_Ranges)) 
	{
		ot::ConstJsonObjectList rangesList = ot::json::getObjectList(_document, OT_ACTION_PARAM_Ranges);
		ranges.reserve(rangesList.size());
		for (const ot::ConstJsonObject& rangeObject : rangesList) {
			ot::TableRange range;
			range.setFromJsonObject(rangeObject);
			ranges.push_back(range);
		}
	}

	// Get table
	ot::TableView* table = AppBase::instance()->findTable(tableName);

	//!! Needs to be executed before, since the callback unlocks the ui lock
	if (callback) {
		this->sendTableSelectionInformation(callbackUrl, callbackFunction, table);
	}

	if (!table) {
		OT_LOG_EAS("Table \"" + tableName + "\" does not exist");
		return "";
	}

	// Apply selection
	if (clearSelection) {
		table->getTable()->clearSelection();
	}
	for (const ot::TableRange& range : ranges) {
		table->getTable()->setRangeSelected(ot::QtFactory::toQTableRange(range), true);
	}
	
	// Apply color
	table->getTable()->setSelectedCellsBackground(color);

	if (clearSelectionAfter) {
		table->getTable()->clearSelection();
	}

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);

	return "";
}

// Studio Suite API

std::string ExternalServicesComponent::handleStudioSuiteAction(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_MEMBER);
	return StudioSuiteConnectorAPI::processAction(action, _document, AppBase::instance()->getCurrentProjectName(), this, AppBase::instance()->mainWindow()->windowIcon());
}

// LTSpice API

std::string ExternalServicesComponent::handleLTSpiceAction(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_MEMBER);
	return LTSpiceConnectorAPI::processAction(action, _document, AppBase::instance()->getCurrentProjectName(), this, AppBase::instance()->mainWindow()->windowIcon());
}

// Dialogs
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
	responseDoc.AddMember(OT_ACTION_PARAM_ObjectName, ot::JsonString(cfg.getName(), responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_Value, ot::JsonString(ot::MessageDialogCfg::toString(result), responseDoc.GetAllocator()), responseDoc.GetAllocator());

	std::string response;
	sendHttpRequest(EXECUTE, info, responseDoc, response);

	return "";
}

std::string ExternalServicesComponent::handleModelLibraryDialog(ot::JsonDocument& _document) {

	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);
	ot::UID entityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);
	std::string collectionName = ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME);
	std::string targetFolder = ot::json::getString(_document, OT_ACTION_PARAM_Folder);
	std::string elementType = ot::json::getString(_document, OT_ACTION_PARAM_ElementType);
	std::string modelUrl = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_URL);
	std::string lmsUrl = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);
	std::string dbUserName = ot::json::getString(_document, OT_PARAM_DB_USERNAME);
	std::string dbUserPassword = ot::json::getString(_document, OT_PARAM_DB_PASSWORD);
	std::string dbServerUrl = ot::json::getString(_document, OT_ACTION_PARAM_DATABASE_URL);

	ot::ModelLibraryDialogCfg cfg;
	cfg.setFromJsonObject(cfgObj);

	// Prepare response doc
	ot::JsonDocument responseDoc;
	responseDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, entityID, responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(collectionName, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_Folder, ot::JsonString(targetFolder, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_ElementType, ot::JsonString(elementType, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(modelUrl, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(dbUserName, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_PARAM_DB_PASSWORD, ot::JsonString(dbUserPassword, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_DATABASE_URL, ot::JsonString(dbServerUrl, responseDoc.GetAllocator()), responseDoc.GetAllocator());

	// Show dialog
	ot::ModelLibraryDialog dia(std::move(cfg));
	if (dia.showDialog() == ot::Dialog::Ok) {
		responseDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_ModelDialogConfirmed, responseDoc.GetAllocator()), responseDoc.GetAllocator());
		responseDoc.AddMember(OT_ACTION_PARAM_Value, ot::JsonString(dia.getSelectedName(), responseDoc.GetAllocator()), responseDoc.GetAllocator());
	}
	else {
		responseDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_ModelDialogCanceled, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	}

	// Send response
	std::string response;
	if (!ot::msg::send("", lmsUrl, ot::EXECUTE, responseDoc.toJson(), response)) {
		OT_LOG_E("Failed to send message to LMS at \"" + lmsUrl + "\"");
	}

	return "";
}

// ###################################################################################################

// Private functions

void ExternalServicesComponent::determineViews(const std::string& modelServiceURL) {
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
		bool visibleBlockPicker = responseDoc[OT_ACTION_PARAM_UI_TREE_VisibleBlockPicker].GetBool();

		AppBase* app{ AppBase::instance() };

		app->setVisible3D(visible3D);
		app->setVisible1D(visible1D);
		app->setVisibleBlockPicker(visibleBlockPicker);
}

void ExternalServicesComponent::sendTableSelectionInformation(const std::string& _serviceUrl, const std::string& _callbackFunction, ot::TableView* _table) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteFunction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(_callbackFunction, doc.GetAllocator()), doc.GetAllocator());
	if (_table != nullptr)
	{
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, _table->getViewData().getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion, _table->getViewData().getEntityVersion(), doc.GetAllocator());
		
		AppBase::instance()->appendInfoMessage(QString::fromStdString("Loading table ranges.\n"));
		ot::JsonArray rangesArray;
		for (const QTableWidgetSelectionRange& qrange : _table->getTable()->selectedRanges()) {
			ot::JsonObject rangeObject;
			ot::TableRange range = ot::QtFactory::toTableRange(qrange);
			range.addToJsonObject(rangeObject, doc.GetAllocator());
			rangesArray.PushBack(rangeObject, doc.GetAllocator());
		}
		doc.AddMember(OT_ACTION_PARAM_Ranges, rangesArray, doc.GetAllocator());
	}
	std::string response;
	sendHttpRequest(EXECUTE, _serviceUrl, doc, response);
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		OT_LOG_EAS(response);
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		OT_LOG_WAS(response);
	}
}

void ExternalServicesComponent::keepAlive()
{
	// The keep alive signal is necessary, since a remote firewall will usually close the connection in case of 
	// inactivity of about 10 minutes.

	if (m_websocket != nullptr)
	{
		ot::JsonDocument pingDoc;
		pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
		std::string ping = pingDoc.toJson();

		std::string response;
		sendToModelService(ping, response);
	}
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
				if (!ot::msg::send("", _sessionServiceURL, ot::EXECUTE_ONE_WAY_TLS, ping, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
					sessionServiceDied = true;
				}
				else OT_ACTION_IF_RESPONSE_ERROR(response) {
					sessionServiceDied = true;
				}
				else OT_ACTION_IF_RESPONSE_WARNING(response) {
					sessionServiceDied = true;
				}
				else if (response != OT_ACTION_CMD_Ping) {
					sessionServiceDied = true;
				}
			}
			catch (const std::exception& _e) {
				OT_LOG_E(_e.what());
				sessionServiceDied = true;
			}
		}
	}

	if (sessionServiceDied) {
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
