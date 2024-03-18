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
#include "OTCommunication/IpConverter.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/UiTypes.h"

#include "OTServiceFoundation/SettingsData.h"
#include "OTServiceFoundation/Dispatcher.h"
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
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/MessageDialog.h"
#include "OTWidgets/PropertyDialog.h"
#include "OTWidgets/OnePropertyDialog.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"

#include "StudioSuiteConnector/StudioSuiteConnectorAPI.h"

// Curl
#include "curl/curl.h"					// Curl

// AK header
#include <akAPI/uiAPI.h>
#include <akGui/aColor.h>

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

void ExternalServicesComponent::addMenuPage(ot::ServiceBase * _sender, const std::string &pageName)
{
	try {
		ak::UID p = AppBase::instance()->getToolBar()->addPage(getServiceUiUid(_sender), pageName.c_str());
		//NOTE, add corresponding functions in uiServiceAPI
		ak::uiAPI::object::setObjectUniqueName(p, pageName.c_str());
		m_controlsManager->uiElementCreated(_sender, p, false);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::addMenuGroup(ot::ServiceBase * _sender, const std::string &pageName, const std::string &groupName)
{
	try {
		//NOTE, add corresponding functions in uiServiceAPI
		ak::UID pageUID{ ak::uiAPI::object::getUidFromObjectUniqueName(pageName.c_str()) };
		//NOTE, add error handling
		assert(pageUID != ak::invalidUID);

		ak::UID g = AppBase::instance()->getToolBar()->addGroup(getServiceUiUid(_sender), pageUID, groupName.c_str());
		//NOTE, add corresponding functions in uiServiceAPI
		ak::uiAPI::object::setObjectUniqueName(g, (pageName + ":" + groupName).c_str());
		m_controlsManager->uiElementCreated(_sender, g, false);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::addMenuSubgroup(ot::ServiceBase * _sender, const std::string &pageName, const std::string &groupName, const std::string &subgroupName) {
	try {
		//NOTE, add corresponding functions in uiServiceAPI
		ak::UID groupUID{ ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName).c_str()) };
		//NOTE, add error handling
		assert(groupUID != ak::invalidUID);

		ak::UID sg = AppBase::instance()->getToolBar()->addSubGroup(getServiceUiUid(_sender), groupUID, subgroupName.c_str());
		//NOTE, add corresponding functions in uiServiceAPI
		ak::uiAPI::object::setObjectUniqueName(sg, (pageName + ":" + groupName + ":" + subgroupName).c_str());
		m_controlsManager->uiElementCreated(_sender, sg, false);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

ak::UID ExternalServicesComponent::addMenuPushButton(ot::ServiceBase * _sender, const std::string & _pageName, const std::string & _groupName, const std::string &_subgroupName, const std::string & _buttonName,
	const std::string & _text, const std::string & _iconName, const std::string & _iconFolder, const ot::Flags<ot::ui::lockType> & _lockFlags)
{
	try {
		ak::UID parentUID;
		if (_subgroupName.length() == 0) {
			parentUID = ak::uiAPI::object::getUidFromObjectUniqueName((_pageName + ":" + _groupName).c_str());
		}
		else {
			parentUID = ak::uiAPI::object::getUidFromObjectUniqueName((_pageName + ":" + _groupName + ":" + _subgroupName).c_str());
		}
		//NOTE, add error handling
		assert(parentUID != ak::invalidUID);

		// Here we need to pass the iconName as string once the functionality is added to the uiManager
		ak::UID buttonID = AppBase::instance()->getToolBar()->addToolButton(getServiceUiUid(_sender), parentUID, _iconName.c_str(), _iconFolder.c_str(), _text.c_str());

		if (_subgroupName.length() == 0) {
			ak::uiAPI::object::setObjectUniqueName(buttonID, (_pageName + ":" + _groupName + ":" + _buttonName).c_str());
		}
		else {
			ak::uiAPI::object::setObjectUniqueName(buttonID, (_pageName + ":" + _groupName + ":" + _subgroupName + ":" + _buttonName).c_str());
		}
		m_controlsManager->uiElementCreated(_sender, buttonID);
		m_lockManager->uiElementCreated(_sender, buttonID, _lockFlags);

		ak::uiAPI::registerUidNotifier(buttonID, this);

		return buttonID;
	}
	catch (const std::exception& _e) {
		assert(0);
		return ak::invalidUID;
	}
	catch (...) {
		assert(0); // Error handling
		return ak::invalidUID;
	}
}

void ExternalServicesComponent::addMenuCheckBox(ot::ServiceBase * _sender, const std::string &pageName, const std::string &groupName, const std::string &subgroupName, const std::string &boxName, const std::string &boxText, bool checked, const ot::Flags<ot::ui::lockType> & _lockFlags)
{
	try {
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
		if (parentID == ak::invalidUID) return;

		// Here we need to pass the iconName as string once the functionality is added to the uiManager
		ak::UID boxID = AppBase::instance()->getToolBar()->addCheckBox(getServiceUiUid(_sender), parentID, boxText.c_str(), checked);

		//NOTE, add corresponding functions in uiServiceAPI
		if (subgroupName.length() > 0) {
			ak::uiAPI::object::setObjectUniqueName(boxID, (pageName + ":" + groupName + ":" + subgroupName + ":" + boxName).c_str());
		}
		else {
			ak::uiAPI::object::setObjectUniqueName(boxID, (pageName + ":" + groupName + ":" + boxName).c_str());
		}

		m_controlsManager->uiElementCreated(_sender, boxID);
		m_lockManager->uiElementCreated(_sender, boxID, _lockFlags);

		ak::uiAPI::registerUidNotifier(boxID, this);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::addMenuLineEdit(ot::ServiceBase * _sender, const std::string &pageName, const std::string &groupName, const std::string &subgroupName, const std::string &editName, const std::string &editText, const std::string &editLabel, const ot::Flags<ot::ui::lockType> & _lockFlags)
{
	try {
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

		// Here we need to pass the iconName as string once the functionality is added to the uiManager
		ak::UID editID = AppBase::instance()->getToolBar()->addNiceLineEdit(getServiceUiUid(_sender), parentID, editLabel.c_str(), editText.c_str());

		//NOTE, add corresponding functions in uiServiceAPI
		if (subgroupName.length() > 0) {
			ak::uiAPI::object::setObjectUniqueName(editID, (pageName + ":" + groupName + ":" + subgroupName + ":" + editName).c_str());
		}
		else {
			ak::uiAPI::object::setObjectUniqueName(editID, (pageName + ":" + groupName + ":" + editName).c_str());
		}
		m_controlsManager->uiElementCreated(_sender, editID);
		m_lockManager->uiElementCreated(_sender, editID, _lockFlags);

		ak::uiAPI::registerUidNotifier(editID, this);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

KeyboardCommandHandler * ExternalServicesComponent::addShortcut(ot::ServiceBase * _sender, const std::string& _keySequence) {
	ShortcutManager * manager = AppBase::instance()->shortcutManager();
	if (manager) {
		KeyboardCommandHandler * oldHandler = manager->handlerFromKeySequence(_keySequence.c_str());
		if (oldHandler) {
			std::string msg{ "Shortcut for key sequence \"" };
			msg.append(_keySequence).append("\" already occupied by service \"");
			msg.append(oldHandler->creator()->serviceName()).append(" (ID: ").append(std::to_string(oldHandler->creator()->serviceID())).append(")\"\n");
			displayInfoMessage(msg);
			return nullptr;
		}
		KeyboardCommandHandler * newHandler = new KeyboardCommandHandler(_sender, AppBase::instance(), _keySequence.c_str());
		manager->addHandler(newHandler);
		return newHandler;
	}
	else {
		assert(0);
		return nullptr;
	}
}

void ExternalServicesComponent::removeUIElements(const std::list<std::string> & _itemList)
{
	try {
		std::vector<ak::UID> uidList;
		for (auto itm : _itemList) {
			uidList.push_back(ak::uiAPI::object::getUidFromObjectUniqueName(itm.c_str()));
		}
		m_controlsManager->destroyUiControls(uidList);

		for (auto itm : uidList) {
			m_lockManager->uiElementDestroyed(itm);
		}
	}
	catch (...) {
		assert(0); // Error handling
	}
}

// ###################################################################################################

// UI Element manipulation

void ExternalServicesComponent::fillPropertyGrid(const std::string &settings)
{
	try {
		AppBase * app{ AppBase::instance() };
		app->clearPropertyGrid();

		// Read settings into JSOC Doc
		ot::JsonDocument doc;

		// Parse the document with the json string we have "received"
		doc.Parse(settings.c_str());

		// Check if the document is an object
		assert(doc.IsObject()); // Doc is not an object

		std::map<std::string, bool> groupMap;

		std::vector<std::vector<int>> groupColors = { {164, 147, 133}, {143, 128, 154}, {117, 139, 154}, {166, 135, 148}, {141, 158, 161} };
		int groupColorIndex = 0;

		for (rapidjson::Value::ConstMemberIterator i = doc.MemberBegin(); i != doc.MemberEnd(); i++)
		{
			std::string name = i->name.GetString();
			assert(i->value.IsObject());

			rapidjson::Value &item = doc[name.c_str()];

			std::string type = item["Type"].GetString();
			bool multipleValues = item["MultipleValues"].GetBool();
			bool readOnly = item["ReadOnly"].GetBool();
			bool protectedProperty = item["Protected"].GetBool();
			bool errorState = item["ErrorState"].GetBool();
			std::string group = item["Group"].GetString();

			if (!group.empty())
			{
				if (groupMap.find(group) == groupMap.end())
				{
					//double r = groupColors[groupColorIndex % groupColors.size()][0] / 255.0;
					//double g = groupColors[groupColorIndex % groupColors.size()][1] / 255.0;
					//double b = groupColors[groupColorIndex % groupColors.size()][2] / 255.0;
					int r = groupColors[groupColorIndex % groupColors.size()][0];
					int g = groupColors[groupColorIndex % groupColors.size()][1];
					int b = groupColors[groupColorIndex % groupColors.size()][2];

					app->addPropertyGroup(group.c_str(), ak::aColor(r, g, b), ak::aColor(0, 0, 0), ak::aColor(255, 0, 0));
					groupMap[group] = true;

					groupColorIndex++;
				}
			}

			if (type == "double")
			{
				double value = item["Value"].GetDouble();
				app->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, !protectedProperty, errorState, value);
			}
			else if (type == "integer")
			{
				long value = (long)(item["Value"].GetInt64());
				long min = INT_MIN;
				long max = INT_MAX;
				if (item.HasMember("ValueMin")) min = (long)item["ValueMin"].GetInt64();
				if (item.HasMember("ValueMax")) max = (long)item["ValueMax"].GetInt64();
				//if (item.HasMember("NumberInputMode")) app->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, value, min, max);
				//else app->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, value);
				app->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, !protectedProperty, errorState, value, min, max);
			}
			else if (type == "boolean")
			{
				bool value = item["Value"].GetBool();
				app->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, !protectedProperty, errorState, value);
			}
			else if (type == "string")
			{
				QString value = item["Value"].GetString();
				app->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, !protectedProperty, errorState, value);
			}
			else if (type == "selection")
			{
				std::vector<QString> selection;

				rapidjson::Value options = item["Options"].GetArray();

				for (int i = 0; i < options.Size(); i++)
				{
					selection.push_back(QString(options[i].GetString()));
				}

				QString value = item["Value"].GetString();
				app->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, !protectedProperty, errorState, selection, value);
			}
			else if (type == "color")
			{
				double colorR = item["ValueR"].GetDouble();
				double colorG = item["ValueG"].GetDouble();
				double colorB = item["ValueB"].GetDouble();

				ak::aColor value(colorR * 255, colorG * 255, colorB * 255, 0);
				app->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, !protectedProperty, errorState, value);
			}
			else if (type == "entitylist")
			{
				std::vector<QString> selection;

				rapidjson::Value options = item["Options"].GetArray();

				for (int i = 0; i < options.Size(); i++)
				{
					selection.push_back(QString(options[i].GetString()));
				}

				QString value = item["ValueName"].GetString();
				app->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, !protectedProperty, errorState, selection, value);
			}
			else if (type == "projectlist")
			{
				std::vector<QString> selection;

				std::list<std::string> userProjects = GetAllUserProjects();

				for (auto& project : userProjects)
				{
					selection.push_back(QString(project.c_str()));
				}

				QString value = item["Value"].GetString();
				app->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, !protectedProperty, errorState, selection, value);
			}
			else
			{
				assert(0); // Unsupported type
			}
		}
	}
	catch (const std::exception & e) {
		AppBase::instance()->showErrorPrompt(e.what(), "Error");
	}
	catch (...) {
		AppBase::instance()->showErrorPrompt("Unknown error", "Fatal error");
	}
}

void ExternalServicesComponent::setCheckBoxValues(ot::ServiceBase * _sender, const std::string & _controlName, bool checked)
{
	try {
		ak::UID objectID = ak::uiAPI::object::getUidFromObjectUniqueName(_controlName.c_str());

		assert(objectID != ak::invalidUID);
		if (objectID == ak::invalidUID) return;

		ak::uiAPI::checkBox::setChecked(objectID, checked);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::setLineEditValues(ot::ServiceBase * _sender, const std::string & _controlName, const std::string &editText, bool error)
{
	try {
		ak::UID objectID = ak::uiAPI::object::getUidFromObjectUniqueName(_controlName.c_str());

		assert(objectID != ak::invalidUID);
		if (objectID == ak::invalidUID) return;

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
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::enableDisableControls(ot::ServiceBase * _sender, std::list<std::string> &enabled, std::list<std::string> &disabled)
{
	try {
		for (auto controlName : enabled)
		{
			//NOTE, add functionallity to uiServiceAPI
			auto uid = ak::uiAPI::object::getUidFromObjectUniqueName(controlName.c_str());
			if (uid != ak::invalidUID)
			{
				m_lockManager->enable(_sender, uid, true);
			}
		}

		for (auto controlName : disabled)
		{
			//NOTE, add functionallity to uiServiceAPI
			auto uid = ak::uiAPI::object::getUidFromObjectUniqueName(controlName.c_str());
			if (uid != ak::invalidUID)
			{
				m_lockManager->disable(_sender, uid);
			}
		}
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::setTooltipText(ot::ServiceBase * _sender, const std::string &item, const std::string &text)
{
	try
	{
		auto uid = ak::uiAPI::object::getUidFromObjectUniqueName(item.c_str());
		if (uid != ak::invalidUID)
		{
			ak::uiAPI::object::setToolTip(uid, text.c_str());
		}
	}
	catch (...)
	{
		assert(0);
	}
}

void ExternalServicesComponent::displayInfoMessage(const std::string& _message)
{
	AppBase::instance()->appendInfoMessage(_message.c_str());
};

void ExternalServicesComponent::displayDebugMessage(const std::string& _message) {
	AppBase::instance()->appendDebugMessage(_message.c_str());
}

// ###################################################################################################

// 3D View

void ExternalServicesComponent::resetAllViews3D(ModelUIDtype visualizationModelID)
{
	try {
		AppBase::instance()->getViewerComponent()->resetAllViews3D(visualizationModelID);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::refreshAllViews(ModelUIDtype visualizationModelID)
{
	try {
		AppBase::instance()->getViewerComponent()->refreshAllViews(visualizationModelID);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::clearSelection(ModelUIDtype visualizationModelID)
{
	try {
		AppBase::instance()->getViewerComponent()->clearSelection(visualizationModelID);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::refreshSelection(ModelUIDtype visualizationModelID)
{
	try {
		AppBase::instance()->getViewerComponent()->refreshSelection(visualizationModelID);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::selectObject(ModelUIDtype visualizationModelID, ak::UID entityID)
{
	try {
		AppBase::instance()->getViewerComponent()->selectObject(visualizationModelID, entityID);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::addVisualizationNodeFromFacetData(ModelUIDtype visModelID, const std::string &treeName, double surfaceColorRGB[3],
	double edgeColorRGB[3], ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling, double offsetFactor, bool isEditable, std::vector<Geometry::Node> &nodes,
	std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::string &errors, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected)
{
	try {
		AppBase::instance()->getViewerComponent()->addNodeFromFacetData(visModelID, treeName, surfaceColorRGB, edgeColorRGB, modelEntityID, treeIcons, backFaceCulling, 
																		offsetFactor, isEditable, nodes, triangles, edges, errors, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::addVisualizationNodeFromFacetDataBase(ViewerUIDtype visModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling,
	double offsetFactor, bool isHidden, bool isEditable, const std::string &projectName, ak::UID entityID, ak::UID entityVersion, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double> &transformation)
{
	AppBase::instance()->getViewerComponent()->addNodeFromFacetDataBase(visModelID, treeName, surfaceColorRGB, edgeColorRGB, materialType, textureType, reflective, 
																		modelEntityID, treeIcons, backFaceCulling, offsetFactor, isHidden, isEditable, projectName, entityID, 
																		entityVersion, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected,  transformation);
}

void ExternalServicesComponent::addVisualizationContainerNode(ModelUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool editable)
{
	try {
		AppBase::instance()->getViewerComponent()->addVisualizationContainerNode(visModelID, treeName, modelEntityID, treeIcons, editable);
	}
	catch (const std::exception &e) {
		int x = 1;
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::addVisualizationVis2D3DNode(ModelUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool isHidden, bool editable, const std::string &projectName, ak::UID visualizationDataID, ak::UID visualizationDataVersion)
{
	AppBase::instance()->getViewerComponent()->addVisualizationVis2D3DNode(visModelID, treeName, modelEntityID, treeIcons, isHidden, editable, projectName, visualizationDataID, visualizationDataVersion);
}

void ExternalServicesComponent::updateVisualizationVis2D3DNode(ModelUIDtype visModelID, ModelUIDtype modelEntityID, const std::string &projectName, ak::UID visualizationDataID, ak::UID visualizationDataVersion)
{
	AppBase::instance()->getViewerComponent()->updateVisualizationVis2D3DNode(visModelID, modelEntityID, projectName, visualizationDataID, visualizationDataVersion);
}

void ExternalServicesComponent::addVisualizationAnnotationNode(ModelUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool isHidden,
	const double edgeColorRGB[3],
	const std::vector<std::array<double, 3>> &points,
	const std::vector<std::array<double, 3>> &points_rgb,
	const std::vector<std::array<double, 3>> &triangle_p1,
	const std::vector<std::array<double, 3>> &triangle_p2,
	const std::vector<std::array<double, 3>> &triangle_p3,
	const std::vector<std::array<double, 3>> &triangle_rgb)
{
	try {
		AppBase::instance()->getViewerComponent()->addVisualizationAnnotationNode(visModelID, treeName, modelEntityID, treeIcons, isHidden, edgeColorRGB,
			points, points_rgb,
			triangle_p1, triangle_p2, triangle_p3, triangle_rgb);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::addVisualizationAnnotationNodeDataBase(ModelUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, ak::UID entityID, ak::UID entityVersion)
{
	ViewerAPI::addVisualizationAnnotationNodeDataBase(visModelID, treeName, modelEntityID, treeIcons, isHidden, projectName, entityID, entityVersion);
}

void ExternalServicesComponent::addVisualizationMeshNodeFromFacetDataBase(ModelUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, double edgeColorRGB[3], bool displayTetEdges, const std::string &projectName, ak::UID entityID, ak::UID entityVersion)
{
	ViewerAPI::addVisualizationMeshNodeFromFacetDataBase(visModelID, treeName, modelEntityID, treeIcons, edgeColorRGB, displayTetEdges, projectName, entityID, entityVersion);
}

void ExternalServicesComponent::addVisualizationCartesianMeshNode(ModelUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool isHidden, double edgeColorRGB[3], double meshLineColorRGB[3], bool showMeshLines, const std::vector<double> &meshCoordsX, const std::vector<double> &meshCoordsY, const std::vector<double> &meshCoordsZ,
																  const std::string &projectName, ak::UID faceListEntityID, ak::UID faceListEntityVersion, ak::UID nodeListEntityID, ak::UID nodeListEntityVersion)
{
	ViewerAPI::addVisualizationCartesianMeshNode(visModelID, treeName, modelEntityID, treeIcons, isHidden, edgeColorRGB, meshLineColorRGB, showMeshLines, meshCoordsX, meshCoordsY, meshCoordsZ, projectName, faceListEntityID, faceListEntityVersion, nodeListEntityID, nodeListEntityVersion);
}

void ExternalServicesComponent::visualizationCartesianMeshNodeShowLines(ModelUIDtype visModelID, ModelUIDtype modelEntityID, bool showMeshLines)
{
	ViewerAPI::visualizationCartesianMeshNodeShowLines(visModelID, modelEntityID, showMeshLines);
}

void ExternalServicesComponent::visualizationTetMeshNodeTetEdges(ModelUIDtype visModelID, ModelUIDtype modelEntityID, bool displayTetEdges)
{
	ViewerAPI::visualizationTetMeshNodeTetEdges(visModelID, modelEntityID, displayTetEdges);
}

void ExternalServicesComponent::addVisualizationMeshItemNodeFromFacetDataBase(ModelUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, ak::UID entityID, ak::UID entityVersion, long long tetEdgesID, long long tetEdgesVersion)
{
	ViewerAPI::addVisualizationMeshItemNodeFromFacetDataBase(visModelID, treeName, modelEntityID, treeIcons, isHidden, projectName, entityID, entityVersion, tetEdgesID, tetEdgesVersion);
}

void ExternalServicesComponent::addVisualizationCartesianMeshItemNode(ModelUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool isHidden, std::vector<int> &facesList, double color[3])
{
	ViewerAPI::addVisualizationCartesianMeshItemNode(visModelID, treeName, modelEntityID, treeIcons, isHidden, facesList, color);
}

void ExternalServicesComponent::addVisualizationTextNode(ak::UID visModelID, const std::string &treeName, ak::UID modelEntityID, const TreeIcon &treeIcons, bool isHidden,
	const std::string &projectName, ak::UID textID, ak::UID textVersion)
{
	ViewerAPI::addVisualizationTextNode(visModelID, treeName, modelEntityID, treeIcons, isHidden, projectName, textID, textVersion);
}

void ExternalServicesComponent::addVisualizationTableNode(ak::UID visModelID, const std::string &treeName, ak::UID modelEntityID, const TreeIcon &treeIcons, bool isHidden,
														  const std::string &projectName, ak::UID tableID, ak::UID tableVersion)
{
	ViewerAPI::addVisualizationTableNode(visModelID, treeName, modelEntityID, treeIcons, isHidden, projectName, tableID, tableVersion);
}



void ExternalServicesComponent::addVisualizationPlot1DNode(ak::UID visModelID, const std::string &treeName, ak::UID modelEntityID, const TreeIcon &treeIcons, bool isHidden,
														   const std::string &projectName, const std::string &title, const std::string &plotType, const std::string &plotQuantity, bool grid, int gridColor[], bool legend, bool logscaleX, bool logscaleY,
														   bool autoscaleX, bool autoscaleY, double xmin, double xmax, double ymin, double ymax, std::list<ak::UID> &curvesID, std::list<ak::UID> &curvesVersions,
														   std::list<std::string> &curvesNames)
{
	ViewerAPI::addVisualizationPlot1DNode(visModelID, treeName, modelEntityID, treeIcons, isHidden,	projectName, title, plotType, plotQuantity, grid, gridColor, legend, logscaleX, logscaleY, autoscaleX, autoscaleY, xmin, xmax, ymin, ymax, curvesID, curvesVersions, curvesNames);
}

void ExternalServicesComponent::visualizationPlot1DPropertiesChanged(ak::UID visModelID, ak::UID modelEntityID, const std::string &title, const std::string &plotType, const std::string &plotQuantity, bool grid, int gridColor[], bool legend, bool logscaleX, bool logscaleY,
															bool autoscaleX, bool autoscaleY, double xmin, double xmax, double ymin, double ymax)
{
	ViewerAPI::visualizationPlot1DPropertiesChanged(visModelID, modelEntityID, title, plotType, plotQuantity, grid, gridColor, legend, logscaleX, logscaleY, autoscaleX, autoscaleY, xmin, xmax, ymin, ymax);
}

void ExternalServicesComponent::visualizationResult1DPropertiesChanged(ModelUIDtype visModelID, ak::UID entityID, ak::UID entityVersion)
{
	ViewerAPI::visualizationResult1DPropertiesChanged(visModelID, entityID, entityVersion);
}

void ExternalServicesComponent::updateObjectColor(ModelUIDtype visModelID, ModelUIDtype modelEntityID, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective)
{
	try {
		AppBase::instance()->getViewerComponent()->updateObjectColor(visModelID, modelEntityID, surfaceColorRGB, edgeColorRGB, materialType, textureType, reflective);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::updateMeshColor(ModelUIDtype visModelID, ModelUIDtype modelEntityID, double colorRGB[3])
{
	try {
		AppBase::instance()->getViewerComponent()->updateMeshColor(visModelID, modelEntityID, colorRGB);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::updateObjectFacetsFromDataBase(ViewerUIDtype visModelID, ViewerUIDtype modelEntityID, unsigned long long entityID, unsigned long long entityVersion)
{
	try {
		AppBase::instance()->getViewerComponent()->updateObjectFacetsFromDataBase(visModelID, modelEntityID, entityID, entityVersion);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::enterEntitySelectionMode(ModelUIDtype visualizationModelID, ot::serviceID_t replyTo, const std::string &selectionType, bool allowMultipleSelection, const std::string &selectionFilter, const std::string &selectionAction, const std::string &selectionMessage,
														 std::list<std::string> &optionNames, std::list<std::string> &optionValues)
{
	try {
		AppBase::instance()->getViewerComponent()->enterEntitySelectionMode(visualizationModelID, replyTo, selectionType, allowMultipleSelection, selectionFilter, selectionAction, selectionMessage, optionNames, optionValues);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::freeze3DView(ViewerUIDtype visModelID, bool flag)
{
	try {
		AppBase::instance()->getViewerComponent()->freeze3DView(visModelID, flag);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::isModified(ModelUIDtype visualizationModelID, bool modifiedState)
{
	try {
		AppBase::instance()->getViewerComponent()->isModified(visualizationModelID, modifiedState);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

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

void ExternalServicesComponent::removeShapesFromVisualization(ModelUIDtype visualizationModelID, std::list<ModelUIDtype> entityID)
{
	try {
		AppBase::instance()->getViewerComponent()->removeShapes(visualizationModelID, entityID);
		
		//If entity is has a block item associated, it gets removed from all editors.
		std::list<ot::GraphicsView*> views = AppBase::instance()->getAllGraphicsEditors();
		for (auto view : views) {
			for (auto uid : entityID) {
				view->removeItem(uid);
				view->removeConnection(uid);
			}
		}
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::setTreeStateRecording(ModelUIDtype visualizationModelID, bool flag)
{
	try {
		AppBase::instance()->getViewerComponent()->setTreeStateRecording(visualizationModelID, flag);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::setShapeVisibility(ModelUIDtype visualizationModelID, std::list<ModelUIDtype> visibleID, std::list<ModelUIDtype> hiddenID)
{
	try {
		AppBase::instance()->getViewerComponent()->setShapeVisibility(visualizationModelID, visibleID, hiddenID);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::hideEntities(ModelUIDtype visualizationModelID, std::list<ModelUIDtype> hiddenID)
{
	try {
		AppBase::instance()->getViewerComponent()->hideEntities(visualizationModelID, hiddenID);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::showBranch(ModelUIDtype visualizationModelID, const std::string &branchName)
{
	try {
		AppBase::instance()->getViewerComponent()->showBranch(visualizationModelID, branchName);
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::hideBranch(ModelUIDtype visualizationModelID, const std::string &branchName)
{
	try {
		AppBase::instance()->getViewerComponent()->hideBranch(visualizationModelID, branchName);
	}
	catch (...) {
		assert(0); // Error handling
	}
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

std::string ExternalServicesComponent::getCommonPropertiesAsJson(ModelUIDtype modelID, const std::list<ModelUIDtype> &entityIDList)
{
	try {
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_CommonPropertiesJSON, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());
		AddUIDListToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_EntityIDList, entityIDList);
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

		ot::JsonDocument outDoc;
		outDoc.fromJson(response);
		std::string props = outDoc[OT_ACTION_PARAM_MODEL_PropertyList].GetString();
		return props;
	}
	catch (...) {
		assert(0); // Error handling
		return "";
	}
}

void ExternalServicesComponent::setPropertiesFromJson(ModelUIDtype modelID, const std::list<ModelUIDtype> &entityIDList, std::string props, bool update, bool itemsVisible)
{
	try {
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_SetPropertiesFromJSON, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ID, modelID, doc.GetAllocator());
		AddUIDListToJsonDoc(doc, OT_ACTION_PARAM_MODEL_EntityIDList, entityIDList);
		doc.AddMember(OT_ACTION_PARAM_MODEL_PropertyList, ot::JsonString(props, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_Update, update, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ItemsVisible, itemsVisible, doc.GetAllocator());
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

void ExternalServicesComponent::modelSelectionChangedNotification(ModelUIDtype modelID, std::list<ModelUIDtype> &selectedEntityID, std::list<ModelUIDtype> &selectedVisibleEntityID)
{
	try {
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_SelectionChanged, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());
		AddUIDListToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_SelectedEntityIDs, selectedEntityID);
		AddUIDListToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_SelectedVisibleEntityIDs, selectedVisibleEntityID);

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

void ExternalServicesComponent::propertyGridValueChanged(int itemID)
{
	AppBase::instance()->lockPropertyGrid(true);

	try {
		std::string name = AppBase::instance()->getPropertyName(itemID).toStdString();

		ot::JsonDocument doc;
		doc.SetObject();

		ot::JsonDocument::AllocatorType& allocator = doc.GetAllocator();

		rapidjson::Value container(rapidjson::kObjectType);
		rapidjson::Value jsonType(rapidjson::kStringType);

		switch (AppBase::instance()->getPropertyType(itemID))
		{
		case ak::vtDouble:
		{
			jsonType.SetString("double");

			rapidjson::Value jsonValue(rapidjson::kNumberType);
			jsonValue.SetDouble(AppBase::instance()->getPropertyValueDouble(itemID));

			container.AddMember("Value", jsonValue, allocator);
		}
		break;
		case ak::vtInt:
		{
			jsonType.SetString("integer");

			rapidjson::Value jsonValue(rapidjson::kNumberType);
			jsonValue.SetInt64(AppBase::instance()->getPropertyValueInt(itemID));

			container.AddMember("Value", jsonValue, allocator);
		}
		break;
		case ak::vtBool:
		{
			jsonType.SetString("boolean");

			rapidjson::Value jsonValue(rapidjson::kNumberType);
			jsonValue.SetBool(AppBase::instance()->getPropertyValueBool(itemID));

			container.AddMember("Value", jsonValue, allocator);
		}
		break;
		case ak::vtString:
		{
			jsonType.SetString("string");

			rapidjson::Value jsonValue(AppBase::instance()->getPropertyValueString(itemID).toStdString().c_str(), allocator);

			container.AddMember("Value", jsonValue, allocator);
		}
		break;
		case ak::vtSelection:
		{
			jsonType.SetString("selection");

			rapidjson::Value jsonValue(AppBase::instance()->getPropertyValueSelection(itemID).toStdString().c_str(), allocator);

			rapidjson::Value jsonOptions(rapidjson::kArrayType);

			std::vector<QString> selection = AppBase::instance()->getPropertyPossibleSelection(itemID);
			for (auto option : selection)
			{
				rapidjson::Value val(option.toStdString().c_str(), allocator);
				jsonOptions.PushBack(val, allocator);;
			}

			container.AddMember("Options", jsonOptions, allocator);
			container.AddMember("Value", jsonValue, allocator);
		}
		break;
		case ak::vtColor:
		{
			jsonType.SetString("color");

			rapidjson::Value jsonValueR(rapidjson::kNumberType);
			rapidjson::Value jsonValueG(rapidjson::kNumberType);
			rapidjson::Value jsonValueB(rapidjson::kNumberType);

			ak::aColor color = AppBase::instance()->getPropertyValueColor(itemID);

			jsonValueR.SetDouble(color.r() / 255.0);
			jsonValueG.SetDouble(color.g() / 255.0);
			jsonValueB.SetDouble(color.b() / 255.0);

			container.AddMember("ValueR", jsonValueR, allocator);
			container.AddMember("ValueG", jsonValueG, allocator);
			container.AddMember("ValueB", jsonValueB, allocator);
		}
		break;
		default:
			assert(0); // Unknown type
			return;
		}

		// Now create the Json string from the document and send it to the model

		rapidjson::Value jsonMixed(rapidjson::kNumberType);
		jsonMixed.SetBool(false);

		rapidjson::Value jsonProtected(rapidjson::kNumberType);
		jsonProtected.SetBool(!AppBase::instance()->getPropertyIsDeletable(itemID));

		container.AddMember("Type", jsonType, allocator);
		container.AddMember("MultipleValues", jsonMixed, allocator);
		container.AddMember("Protected", jsonProtected, allocator);

		doc.AddMember(rapidjson::Value::StringRefType(name.c_str()), container, allocator);

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
		setPropertiesFromJson(modelID, selectedModelEntityIDs, doc.toJson(), true, itemsVisible);

	}
	catch (...) {
		assert(0); // Error handling
	}

	AppBase::instance()->lockPropertyGrid(false);
}

void ExternalServicesComponent::propertyGridValueDeleted(int itemID)
{
	AppBase::instance()->lockPropertyGrid(true);

	std::string propertyName = AppBase::instance()->getPropertyName(itemID).toStdString();

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
		AddUIDListToJsonDoc(doc, OT_ACTION_PARAM_MODEL_EntityIDList, selectedModelEntityIDs);
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityName, ot::JsonString(propertyName, doc.GetAllocator()), doc.GetAllocator());
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
		AddStringListToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_Selection_OptNames, optionNames);
		AddStringListToJsonDoc(inDoc, OT_ACTION_PARAM_MODEL_ITM_Selection_OptValues, optionValues);

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

std::string ExternalServicesComponent::dispatchAction(ot::JsonDocument & _doc, const char * _senderIP)
{
	std::string action;
	try {
		if (_doc.HasMember(OT_ACTION_MEMBER)) {
			action = ot::json::getString(_doc, OT_ACTION_MEMBER);

#ifdef _DEBUG
			std::cout << ">>> Dispatch action: " << action << std::endl;
			{
				std::string msg{ "Dispatch action: " };
				msg.append(action).append("\n");
				OutputDebugStringA(msg.c_str());
			}
			//ot::Dispatcher::instance()->dispatch(action, _doc);
#endif // _DEBUG

			if (action == OT_ACTION_CMD_Compound)
			{
				std::string projectName = ot::json::getString(_doc, OT_ACTION_PARAM_PROJECT_NAME);
				rapidjson::Value documents = _doc[OT_ACTION_PARAM_PREFETCH_Documents].GetArray();
				rapidjson::Value prefetchID = _doc[OT_ACTION_PARAM_PREFETCH_ID].GetArray();
				rapidjson::Value prefetchVersion = _doc[OT_ACTION_PARAM_PREFETCH_Version].GetArray();

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


				for (long i = 0; i < numberActions; i++)
				{
					rapidjson::Value subdoc = documents[i].GetObject();

					ot::JsonDocument d;
					d.CopyFrom(subdoc, d.GetAllocator());
					dispatchAction(d, _senderIP);
				}

				// Re enable tree sorting
				AppBase::instance()->setNavigationTreeSortingEnabled(true);
				AppBase::instance()->setNavigationTreeMultiselectionEnabled(true);

				// Nofify the viewer about the end of the bulk processing
				AppBase::instance()->getViewerComponent()->setProcessingGroupOfMessages(false);

				m_lockManager->unlock(nullptr, lockFlags);
			}
			else if (action == OT_ACTION_CMD_MODEL_ExecuteFunction)
			{
				const std::string subsequentFunction = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_FunctionName);

				ot::UIDList entityIDs = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityIDList);
				ot::UIDList entityVersions = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityVersionList);
				
				if (subsequentFunction == m_fileHandler.GetStoreFileFunctionName())
				{
					ot::JsonDocument  reply = m_fileHandler.StoreFileInDataBase(entityIDs, entityVersions);
					std::string response;
					sendHttpRequest(QUEUE, m_fileHandler.GetSenderURL(), reply, response);
					// Check if response is an error or warning
					OT_ACTION_IF_RESPONSE_ERROR(response) {
						assert(0); // ERROR
					}
					else OT_ACTION_IF_RESPONSE_WARNING(response) {
						assert(0); // WARNING
					}
					}
			}		
			else if (action == OT_ACTION_CMD_UI_DisplayMessage)
			{
				std::string message = ot::json::getString(_doc, OT_ACTION_PARAM_MESSAGE);
				displayInfoMessage(message);
			}
			else if (action == OT_ACTION_CMD_UI_DisplayDebugMessage) {
				std::string message = ot::json::getString(_doc, OT_ACTION_PARAM_MESSAGE);
				displayDebugMessage(message);
			}
			else if (action == OT_ACTION_CMD_UI_ReportError)
			{
				std::string message = ot::json::getString(_doc, OT_ACTION_PARAM_MESSAGE);
				ak::uiAPI::promptDialog::show(message.c_str(), "Open Twin", ak::promptIconLeft, "DialogError", "Default", AppBase::instance()->mainWindow());
			}
			else if (action == OT_ACTION_CMD_UI_ReportWarning)
			{
				std::string message = ot::json::getString(_doc, OT_ACTION_PARAM_MESSAGE);
				ak::uiAPI::promptDialog::show(message.c_str(), "Open Twin", ak::promptIconLeft, "DialogWarning", "Default", AppBase::instance()->mainWindow());
			}
			else if (action == OT_ACTION_CMD_UI_ReportInformation)
			{
				std::string message = ot::json::getString(_doc, OT_ACTION_PARAM_MESSAGE);
				ak::uiAPI::promptDialog::show(message.c_str(), "Open Twin", ak::promptIconLeft, "DialogInformation", "Default", AppBase::instance()->mainWindow());
			}
			else if (action == OT_ACTION_CMD_UI_PromptInformation)
			{
#pragma message("Invalid Service Information (sender type missing)")
				std::string message = ot::json::getString(_doc, OT_ACTION_PARAM_MESSAGE);
				std::string icon = ot::json::getString(_doc, OT_ACTION_PARAM_ICON);
				std::string options = ot::json::getString(_doc, OT_ACTION_PARAM_OPTIONS);
				std::string promptResponse = ot::json::getString(_doc, OT_ACTION_PARAM_RESPONSE);
				std::string sender = ot::json::getString(_doc, OT_ACTION_PARAM_SENDER);
				std::string parameter1 = ot::json::getString(_doc, OT_ACTION_PARAM_PARAMETER1);

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
			}
			else if (action == OT_ACTION_CMD_UI_RegisterForModelEvents) {
				ot::serviceID_t senderID(ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID));
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

				std::cout << "Service with ID \"" << s->second->serviceID() << "\" was registered from model view events" << std::endl;
			}
			else if (action == OT_ACTION_CMD_UI_DeregisterForModelEvents) {
				ot::serviceID_t senderID(ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID));
				auto s = m_serviceIdMap.find(senderID);

				// Check if the service was registered before
				if (s == m_serviceIdMap.end()) {
					std::string ex("A service with the id \"");
					ex.append(std::to_string(senderID));
					ex.append("\" was not registered before");
					throw std::exception(ex.c_str());
				}

				removeServiceFromList(m_modelViewNotifier, s->second);

				std::cout << "Service with ID \"" << s->second->serviceID() << "\" was deregistered from model view events" << std::endl;
			}
			else if (action == OT_ACTION_CMD_ServiceShutdown) {
				std::cout << "Shutdown from session service" << std::endl;
				AppBase::instance()->showErrorPrompt("Shutdown requested by session service.", "Error");
				exit(0);	//NOTE, Add external shutdown
			}
			else if (action == OT_ACTION_CMD_ServicePreShutdown) {
				std::cout << "Pre shutdown from session service" << std::endl;
			}
			else if (action == OT_ACTION_CMD_UI_GenerateUIDs)
			{
				int count = _doc[OT_ACTION_PARAM_COUNT].GetInt();
				// empty id list
				std::vector<ak::UID> idList;
				for (int i = 0; i < count; i++) { idList.push_back(ak::uiAPI::createUid()); }
				// json object contains the list
				return getReturnJSONFromVector(idList);
			}
			else if (action == OT_ACTION_CMD_UI_RequestFileForReading)
			{
				std::string dialogTitle = ot::json::getString(_doc, OT_ACTION_PARAM_UI_DIALOG_TITLE);
				std::string fileMask = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Mask);
				std::string subsequentFunction = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_FunctionName);
				std::string senderURL = ot::json::getString(_doc, OT_ACTION_PARAM_SENDER_URL);
				bool loadContent = ot::json::getBool(_doc, OT_ACTION_PARAM_FILE_LoadContent);

				requestFileForReading(dialogTitle, fileMask, subsequentFunction, senderURL, loadContent);
			}
			else if (action == OT_Action_CMD_UI_StoreFileInDataBase)
			{
				std::string dialogTitle = ot::json::getString(_doc, OT_ACTION_PARAM_UI_DIALOG_TITLE);
				std::string fileMask = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Mask);
				std::string entityType = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Type);
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

						std::list<std::string> takenNames = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_TAKEN_NAMES);
						std::string senderName = ot::json::getString(_doc, OT_ACTION_PARAM_SENDER);
						std::string entityPath = ot::json::getString(_doc, OT_ACTION_PARAM_NAME);
						std::string subsequentFunction = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_FunctionName);
						std::string senderURL = ot::json::getString(_doc, OT_ACTION_PARAM_SENDER_URL);
						m_fileHandler.SetNewFileImportRequest(std::move(senderURL), std::move(subsequentFunction), std::move(senderName), std::move(takenNames), std::move(absoluteFilePaths), std::move(entityPath), entityType);
					}
				}
				catch (std::exception& e)
				{
					displayInfoMessage("Failed to load file due to: " + std::string(e.what()));
				}
			}
			else if (action == OT_ACTION_CMD_UI_SaveFileContent)
			{
				std::string dialogTitle = ot::json::getString(_doc, OT_ACTION_PARAM_UI_DIALOG_TITLE);
				std::string fileName = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_OriginalName);
				std::string fileContent = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Content);
				ot::UID uncompressedDataLength = ot::json::getUInt64(_doc, OT_ACTION_PARAM_FILE_Content_UncompressedDataLength);

				saveFileContent(dialogTitle, fileName, fileContent, uncompressedDataLength);
			}
			else if (action == OT_ACTION_CMD_UI_SelectFileForStoring)
			{
				std::string dialogTitle = ot::json::getString(_doc, OT_ACTION_PARAM_UI_DIALOG_TITLE);
				std::string fileMask = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Mask);
				std::string subsequentFunction = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_FunctionName);
				std::string senderURL = ot::json::getString(_doc, OT_ACTION_PARAM_SENDER_URL);

				selectFileForStoring(dialogTitle, fileMask, subsequentFunction, senderURL);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_Table_AddRow)
			{
				bool insertAbove = ot::json::getBool(_doc, OT_ACTION_PARAM_BASETYPE_Bool);
				ak::UID visualizationModelID = ot::json::getUInt64(_doc, OT_ACTION_PARAM_MODEL_ID);
				ViewerAPI::AddToSelectedTableRow(insertAbove, visualizationModelID);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_Table_DeleteRow)
			{
				ak::UID visualizationModelID = ot::json::getUInt64(_doc, OT_ACTION_PARAM_MODEL_ID);
				ViewerAPI::DeleteFromSelectedTableRow(visualizationModelID);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_Table_AddColumn)
			{
				bool insertLeft = ot::json::getBool(_doc, OT_ACTION_PARAM_BASETYPE_Bool);
				ak::UID visualizationModelID = ot::json::getUInt64(_doc, OT_ACTION_PARAM_MODEL_ID);
				ViewerAPI::AddToSelectedTableColumn(insertLeft,visualizationModelID);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_Table_DeleteColumn)
			{
				ak::UID visualizationModelID = ot::json::getUInt64(_doc, OT_ACTION_PARAM_MODEL_ID);
				ViewerAPI::DeleteFromSelectedTableColumn(visualizationModelID);
			}
			
			else if (action == OT_ACTION_CMD_UI_FillPropertyGrid)
			{
				std::string settings = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_PropertyGridSettingsJSON);
				fillPropertyGrid(settings);
			}
			else if (action == OT_ACTION_CMD_UI_AddMenuPage)
			{
				std::string pageName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_PageName);
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				addMenuPage(getService(serviceId), pageName);
			}
			else if (action == OT_ACTION_CMD_UI_AddMenuGroup)
			{
				std::string pageName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_PageName);
				std::string groupName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_GroupName);
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				addMenuGroup(getService(serviceId), pageName, groupName);
			}
			else if (action == OT_ACTION_CMD_UI_AddMenuSubgroup)
			{
				std::string pageName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_PageName);
				std::string groupName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_GroupName);
				std::string subgroupName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_SubgroupName);
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				addMenuSubgroup(getService(serviceId), pageName, groupName, subgroupName);
			}
			else if (action == OT_ACTION_CMD_UI_AddMenuButton)
			{
				std::string pageName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_PageName);
				std::string groupName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_GroupName);
				std::string subgroupName = "";
				if (_doc.HasMember(OT_ACTION_PARAM_UI_CONTROL_SubgroupName))
				{
					subgroupName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_SubgroupName);
				}
				std::string buttonName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
				std::string text = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectText);
				std::string iconName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_IconName);
				std::string iconFolder = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_IconFolder);
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				ot::ContextMenu contextMenu("");
				if (_doc.HasMember(OT_ACTION_PARAM_UI_CONTROL_ContextMenu)) {
					ot::ConstJsonObject contextMenuData = ot::json::getObject(_doc, OT_ACTION_PARAM_UI_CONTROL_ContextMenu);
					contextMenu.setFromJsonObject(contextMenuData);
				}
				ot::ServiceBase * senderService = getService(serviceId);
				ak::UID btnUid;

				if (_doc.HasMember(OT_ACTION_PARAM_ElementLockTypes)) {
					auto flags = ot::ui::toLockType(ot::json::getStringList(_doc, OT_ACTION_PARAM_ElementLockTypes));
					flags.setFlag(ot::ui::lockType::tlAll);	// Add the all flag to all external push buttons
					btnUid = addMenuPushButton(senderService, pageName, groupName, subgroupName, buttonName, text, iconName, iconFolder, flags);
				}
				else {
					btnUid = addMenuPushButton(senderService, pageName, groupName, subgroupName, buttonName, text, iconName, iconFolder, ot::Flags<ot::ui::lockType>(ot::ui::lockType::tlAll));
				}

				if (btnUid != ak::invalidUID) {
					if (_doc.HasMember(OT_ACTION_PARAM_UI_KeySequence)) {
						std::string keySquence = ot::json::getString(_doc, OT_ACTION_PARAM_UI_KeySequence);
						if (keySquence.length() > 0) {
							KeyboardCommandHandler * newHandler = addShortcut(senderService, keySquence.c_str());
							if (newHandler) {
								newHandler->attachToEvent(btnUid, ak::etClicked);
								text.append(" (").append(keySquence).append(")");
								ak::uiAPI::toolButton::setToolTip(btnUid, text.c_str());
							}
						}
					}

					if (contextMenu.hasItems()) {
						AppBase::instance()->contextMenuManager()->createItem(senderService, getServiceUiUid(senderService), btnUid, contextMenu);
					}
				}
			}
			else if (action == OT_ACTION_CMD_UI_ActivateToolbarTab)
			{
				std::string tabName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_TabName);

				AppBase::instance()->activateToolBarTab(tabName.c_str());
			}
			else if (action == OT_ACTION_CMD_UI_AddMenuCheckbox)
			{
				std::string pageName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_PageName);
				std::string groupName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_GroupName);
				std::string subgroupName = ""; // Subgroup is optional
				try { subgroupName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_SubgroupName); }
				catch (...) {}
				std::string boxName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
				std::string boxText = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectText);
				bool checked = _doc[OT_ACTION_PARAM_UI_CONTROL_CheckedState].GetBool();
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				auto flags = ot::ui::toLockType(ot::json::getStringList(_doc, OT_ACTION_PARAM_ElementLockTypes));
				flags.setFlag(ot::ui::lockType::tlAll);	// Add the all flag to all external checkboxes

				addMenuCheckBox(getService(serviceId), pageName, groupName, subgroupName, boxName, boxText, checked, flags);
			}
			else if (action == OT_ACTION_CMD_UI_AddShortcut) {
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				std::string keySequence = ot::json::getString(_doc, OT_ACTION_PARAM_UI_KeySequence);
				if (keySequence.length() > 0) {
					addShortcut(getService(serviceId), keySequence);
				}
				else {
					assert(0);
				}
			}
			else if (action == OT_ACTION_CMD_UI_AddMenuLineEdit)
			{
				std::string pageName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_PageName);
				std::string groupName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_GroupName);
				std::string subgroupName = ""; // Subgroup is optional
				try { subgroupName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_SubgroupName); }
				catch (...) {}
				std::string editName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
				std::string editText = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectText);
				std::string editLabel = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectLabelText);
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				auto flags = ot::ui::toLockType(ot::json::getStringList(_doc, OT_ACTION_PARAM_ElementLockTypes));
				flags.setFlag(ot::ui::lockType::tlAll);	// Add the all flag to all external checkboxes

				addMenuLineEdit(getService(serviceId), pageName, groupName, subgroupName, editName, editText, editLabel, flags);
			}
			else if (action == OT_ACTION_CMD_UI_SetCheckboxValues)
			{
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				std::string controlName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
				bool checked = _doc[OT_ACTION_PARAM_UI_CONTROL_CheckedState].GetBool();

				setCheckBoxValues(getService(serviceId), controlName, checked);
			}
			else if (action == OT_ACTION_CMD_UI_SetLineEditValues)
			{
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				std::string controlName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
				std::string editText = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectText);
				bool error = _doc[OT_ACTION_PARAM_UI_CONTROL_ErrorState].GetBool();

				setLineEditValues(getService(serviceId), controlName, editText, error);
			}
			else if (action == OT_ACTION_CMD_UI_SwitchMenuTab)
			{
				std::string pageName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_PageName);
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				AppBase::instance()->switchToTab(pageName);
			}
			else if (action == OT_ACTION_CMD_UI_RemoveElements)
			{
				std::list<std::string> itemList = ot::json::getStringList(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectNames);
				removeUIElements(itemList);
			}
			else if (action == OT_ACTION_CMD_UI_EnableDisableControls)
			{
				std::list<std::string> enabled = ot::json::getStringList(_doc, OT_ACTION_PARAM_UI_EnabledControlsList);
				std::list<std::string> disabled = ot::json::getStringList(_doc, OT_ACTION_PARAM_UI_DisabledControlsList);
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);

				enableDisableControls(getService(serviceId), enabled, disabled);
			}
			else if (action == OT_ACTION_CMD_UI_OBJ_SetToolTip)
			{
				std::string item = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
				std::string text = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectText);
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);

				setTooltipText(getService(serviceId), item, text);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_Reset)
			{
				ak::UID visualizationModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				resetAllViews3D(visualizationModelID);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_Refresh)
			{
				ak::UID visualizationModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				refreshAllViews(visualizationModelID);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_ClearSelection)
			{
				ak::UID visualizationModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				clearSelection(visualizationModelID);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_RefreshSelection)
			{
				ak::UID visualizationModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				refreshSelection(visualizationModelID);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_SelectObject)
			{
				ak::UID visualizationModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				ak::UID entityID = _doc[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
				selectObject(visualizationModelID, entityID);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_AddNodeFromFacetData)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string treeName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_TREE_Name);
				double *surfaceColorRGB = getDoubleFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB);
				double *edgeColorRGB = getDoubleFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB);
				ak::UID modelEntityID = _doc[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
				bool backFaceCulling = ot::json::getBool(_doc, OT_ACTION_PARAM_MODEL_ITM_BACKFACE_Culling);
				double offsetFactor = _doc[OT_ACTION_PARAM_MODEL_ITM_OffsetFactor].GetDouble();
				bool editable = _doc[OT_ACTION_PARAM_MODEL_ITM_IsEditable].GetBool();
				bool selectChildren = _doc[OT_ACTION_PARAM_MODEL_ITM_SelectChildren].GetBool();
				bool manageParentVisibility = _doc[OT_ACTION_PARAM_MODEL_ITM_ManageParentVis].GetBool();
				bool manageChildVisibility = _doc[OT_ACTION_PARAM_MODEL_ITM_ManageChildVis].GetBool();
				bool showWhenSelected = _doc[OT_ACTION_PARAM_MODEL_ITM_ShowWhenSelected].GetBool();
				std::vector<Geometry::Node> nodes;
				getVectorNodeFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_Nodes, nodes);
				std::list<Geometry::Triangle> triangles;
				getListTriangleFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_Triangles, triangles);
				std::list<Geometry::Edge> edges;
				getListEdgeFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_Edges, edges);
				std::string errors = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ITM_Errors);
				TreeIcon treeIcons = getTreeIconsFromDocument(_doc);
				addVisualizationNodeFromFacetData(visModelID, treeName, surfaceColorRGB, edgeColorRGB, modelEntityID, treeIcons, backFaceCulling, offsetFactor, editable, nodes, triangles, edges, errors, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_AddNodeFromDataBase)
			{
				ViewerUIDtype visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string treeName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_TREE_Name);
				double *surfaceColorRGB = getDoubleFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB);
				double *edgeColorRGB = getDoubleFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB);
				std::string materialType = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ITM_MaterialType);
				std::string textureType = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ITM_TextureType);
				bool reflective = _doc[OT_ACTION_PARAM_MODEL_ITM_TextureReflective].GetBool();
				ModelUIDtype modelEntityID = _doc[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
				bool backFaceCulling = _doc[OT_ACTION_PARAM_MODEL_ITM_BACKFACE_Culling].GetBool();
				bool isHidden = _doc[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
				double offsetFactor = _doc[OT_ACTION_PARAM_MODEL_ITM_OffsetFactor].GetDouble();
				bool editable = _doc[OT_ACTION_PARAM_MODEL_ITM_IsEditable].GetBool();
				bool selectChildren = _doc[OT_ACTION_PARAM_MODEL_ITM_SelectChildren].GetBool();
				bool manageParentVisibility = _doc[OT_ACTION_PARAM_MODEL_ITM_ManageParentVis].GetBool();
				bool manageChildVisibility = _doc[OT_ACTION_PARAM_MODEL_ITM_ManageChildVis].GetBool();
				bool showWhenSelected = _doc[OT_ACTION_PARAM_MODEL_ITM_ShowWhenSelected].GetBool();
				std::string projectName = ot::json::getString(_doc, OT_ACTION_PARAM_PROJECT_NAME);
				ModelUIDtype entityID = _doc[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
				ModelUIDtype entityVersion = _doc[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
				TreeIcon treeIcons = getTreeIconsFromDocument(_doc);
				std::vector<double> transformation;
				try
				{
					transformation = ot::json::getDoubleVector(_doc, OT_ACTION_PARAM_MODEL_ITM_Transformation);
				}
				catch (std::exception)
				{
					// There is no transformation attached. Set up a unity transform
					transformation.resize(16, 0.0);
					transformation[0] = transformation[5] = transformation[10] = transformation[15] = 1.0;
				}

				addVisualizationNodeFromFacetDataBase(visModelID, treeName, surfaceColorRGB, edgeColorRGB, materialType, textureType, reflective, modelEntityID, treeIcons, backFaceCulling, offsetFactor, isHidden, editable, projectName, entityID, entityVersion, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected, transformation);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_AddContainerNode)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string treeName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_TREE_Name);
				ModelUIDtype modelEntityID = _doc[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
				TreeIcon treeIcons = getTreeIconsFromDocument(_doc);
				bool editable = _doc[OT_ACTION_PARAM_MODEL_ITM_IsEditable].GetBool();
				addVisualizationContainerNode(visModelID, treeName, modelEntityID, treeIcons, editable);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_AddVis2D3DNode)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string treeName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_TREE_Name);
				ModelUIDtype modelEntityID = _doc[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
				TreeIcon treeIcons = getTreeIconsFromDocument(_doc);
				bool editable = _doc[OT_ACTION_PARAM_MODEL_ITM_IsEditable].GetBool();
				std::string projectName = ot::json::getString(_doc, OT_ACTION_PARAM_PROJECT_NAME);
				bool isHidden = _doc[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();

				ak::UID visualizationDataID = _doc[OT_ACTION_PARAM_MODEL_DataID].GetUint64();
				ak::UID visualizationDataVersion = _doc[OT_ACTION_PARAM_MODEL_DataVersion].GetUint64();

				addVisualizationVis2D3DNode(visModelID, treeName, modelEntityID, treeIcons, isHidden, editable, projectName, visualizationDataID, visualizationDataVersion);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_UpdateVis2D3DNode)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				ModelUIDtype modelEntityID = _doc[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
				std::string projectName = ot::json::getString(_doc, OT_ACTION_PARAM_PROJECT_NAME);

				ak::UID visualizationDataID = _doc[OT_ACTION_PARAM_MODEL_DataID].GetUint64();
				ak::UID visualizationDataVersion = _doc[OT_ACTION_PARAM_MODEL_DataVersion].GetUint64();

				updateVisualizationVis2D3DNode(visModelID, modelEntityID, projectName, visualizationDataID, visualizationDataVersion);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_UpdateColor)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				ModelUIDtype modelEntityID = _doc[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
				double *surfaceColorRGB = getDoubleFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB);
				double *edgeColorRGB = getDoubleFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB);
				std::string materialType = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ITM_MaterialType);
				std::string textureType = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ITM_TextureType);
				bool reflective = _doc[OT_ACTION_PARAM_MODEL_ITM_TextureReflective].GetBool();
				updateObjectColor(visModelID, modelEntityID, surfaceColorRGB, edgeColorRGB, materialType, textureType, reflective);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_UpdateMeshColor)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				ModelUIDtype modelEntityID = _doc[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
				double* colorRGB = getDoubleFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB);
				updateMeshColor(visModelID, modelEntityID, colorRGB);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_UpdateFacetsFromDataBase)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				ModelUIDtype modelEntityID = _doc[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
				ModelUIDtype entityID = _doc[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
				ModelUIDtype entityVersion = _doc[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();

				updateObjectFacetsFromDataBase(visModelID, modelEntityID, entityID, entityVersion);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_RemoveShapes)
			{
				///NOTE
				ak::UID visualizationModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::list<ak::UID> entityID = getListFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_ID);
				removeShapesFromVisualization(visualizationModelID, entityID);

				std::list<std::string> curveNames= ViewerAPI::getSelectedCurves(visualizationModelID);
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
					assert(0); // ERROR
				}
				else OT_ACTION_IF_RESPONSE_WARNING(response)
				{
					assert(0); // WARNING
				}
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_TreeStateRecording)
			{
				ak::UID visualizationModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				bool flag = _doc[OT_ACTION_PARAM_MODEL_State].GetBool();
				setTreeStateRecording(visualizationModelID, flag);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_SetShapeVisibility)
			{
				ak::UID visualizationModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::list<ak::UID> visibleID = getListFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_ID_Visible);
				std::list<ak::UID> hiddenID = getListFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_ID_Hidden);
				setShapeVisibility(visualizationModelID, visibleID, hiddenID);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_HideEntities)
			{
				ak::UID visualizationModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::list<ak::UID> hiddenID = getListFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_ID_Hidden);

				hideEntities(visualizationModelID, hiddenID);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_ShowBranch)
			{
				ak::UID visualizationModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string branchName = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ITM_BRANCH);

				showBranch(visualizationModelID, branchName);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_HideBranch)
			{
				ak::UID visualizationModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string branchName = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ITM_BRANCH);

				hideBranch(visualizationModelID, branchName);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_AddAnnotationNode)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string name = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
				ak::UID UID = _doc[OT_ACTION_PARAM_UI_UID].GetUint64();
				bool isHidden = _doc[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
				double *edgeColorRGB = getDoubleFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB);
				std::vector<std::array<double, 3>> points = getVectorDoubleArrayFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_Points);
				std::vector<std::array<double, 3>> points_rgb = getVectorDoubleArrayFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_Points_Colors);
				std::vector<std::array<double, 3>> triangle_p1 = getVectorDoubleArrayFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p1);
				std::vector<std::array<double, 3>> triangle_p2 = getVectorDoubleArrayFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p2);
				std::vector<std::array<double, 3>> triangle_p3 = getVectorDoubleArrayFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p3);
				std::vector<std::array<double, 3>> triangle_rgb = getVectorDoubleArrayFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_Color);
				TreeIcon treeIcons = getTreeIconsFromDocument(_doc);
				addVisualizationAnnotationNode(visModelID, name, UID, treeIcons, isHidden, edgeColorRGB, points, points_rgb, triangle_p1, triangle_p2, triangle_p3, triangle_rgb);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_AddAnnotationNodeFromDatabase)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string name = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
				ak::UID UID = _doc[OT_ACTION_PARAM_UI_UID].GetUint64();
				bool isHidden = _doc[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
				std::string projectName = ot::json::getString(_doc, OT_ACTION_PARAM_PROJECT_NAME);
				ak::UID entityID = _doc[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
				ak::UID entityVersion = _doc[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
				TreeIcon treeIcons = getTreeIconsFromDocument(_doc);
				addVisualizationAnnotationNodeDataBase(visModelID, name, UID, treeIcons, isHidden, projectName, entityID, entityVersion);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_AddMeshNodeFromFacetDatabase)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string name = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
				ak::UID UID = _doc[OT_ACTION_PARAM_UI_UID].GetUint64();
				double edgeColorRGB[3];
				edgeColorRGB[0] = _doc[OT_ACTION_PARAM_MODEL_EDGE_COLOR_R].GetDouble();
				edgeColorRGB[1] = _doc[OT_ACTION_PARAM_MODEL_EDGE_COLOR_G].GetDouble();
				edgeColorRGB[2] = _doc[OT_ACTION_PARAM_MODEL_EDGE_COLOR_B].GetDouble();
				std::string projectName = ot::json::getString(_doc, OT_ACTION_PARAM_PROJECT_NAME);
				ak::UID entityID = _doc[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
				ak::UID entityVersion = _doc[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
				bool displayTetEdges = _doc[OT_ACTION_PARAM_MODEL_TET_DISPLAYEDGES].GetBool();
				TreeIcon treeIcons = getTreeIconsFromDocument(_doc);
				addVisualizationMeshNodeFromFacetDataBase(visModelID, name, UID, treeIcons, edgeColorRGB, displayTetEdges, projectName, entityID, entityVersion);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_AddCartesianMeshNode)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string name = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
				ak::UID UID = _doc[OT_ACTION_PARAM_UI_UID].GetUint64();
				bool isHidden = _doc[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
				double edgeColorRGB[3];
				edgeColorRGB[0] = _doc[OT_ACTION_PARAM_MODEL_EDGE_COLOR_R].GetDouble();
				edgeColorRGB[1] = _doc[OT_ACTION_PARAM_MODEL_EDGE_COLOR_G].GetDouble();
				edgeColorRGB[2] = _doc[OT_ACTION_PARAM_MODEL_EDGE_COLOR_B].GetDouble();
				double meshLineColorRGB[3];
				meshLineColorRGB[0] = _doc[OT_ACTION_PARAM_MODEL_MESHLINE_COLOR_R].GetDouble();
				meshLineColorRGB[1] = _doc[OT_ACTION_PARAM_MODEL_MESHLINE_COLOR_G].GetDouble();
				meshLineColorRGB[2] = _doc[OT_ACTION_PARAM_MODEL_MESHLINE_COLOR_B].GetDouble();
				std::vector<double> meshCoordsX = getVectorDoubleFromDocument(_doc, OT_ACTION_PARAM_MESH_CartesianCoordX);
				std::vector<double> meshCoordsY = getVectorDoubleFromDocument(_doc, OT_ACTION_PARAM_MESH_CartesianCoordY);
				std::vector<double> meshCoordsZ = getVectorDoubleFromDocument(_doc, OT_ACTION_PARAM_MESH_CartesianCoordZ);
				bool showMeshLines = _doc[OT_ACTION_PARAM_MESH_ShowMeshLines].GetBool();
				std::string projectName = ot::json::getString(_doc, OT_ACTION_PARAM_PROJECT_NAME);
				ak::UID faceListEntityID = _doc[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
				ak::UID faceListEntityVersion = _doc[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
				ak::UID nodeListEntityID = _doc[OT_ACTION_PARAM_MESH_NODE_ID].GetUint64();
				ak::UID nodeListEntityVersion = _doc[OT_ACTION_PARAM_MESH_NODE_VERSION].GetUint64();
				TreeIcon treeIcons = getTreeIconsFromDocument(_doc);
				addVisualizationCartesianMeshNode(visModelID, name, UID, treeIcons, isHidden, edgeColorRGB, meshLineColorRGB, showMeshLines, meshCoordsX, meshCoordsY, meshCoordsZ, projectName, faceListEntityID, faceListEntityVersion, nodeListEntityID, nodeListEntityVersion);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_CartesianMeshNodeShowLines)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				ak::UID UID = _doc[OT_ACTION_PARAM_UI_UID].GetUint64();
				bool showMeshLines = _doc[OT_ACTION_PARAM_MESH_ShowMeshLines].GetBool();
				visualizationCartesianMeshNodeShowLines(visModelID, UID, showMeshLines);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_AddCartesianMeshItem)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string name = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
				ak::UID UID = _doc[OT_ACTION_PARAM_UI_UID].GetUint64();
				bool isHidden = _doc[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
				std::vector<int> facesList = getVectorIntFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_FacesList);
				TreeIcon treeIcons = getTreeIconsFromDocument(_doc);
				double colorRGB[3];
				colorRGB[0] = _doc[OT_ACTION_PARAM_MESH_ITEM_COLOR_R].GetDouble();
				colorRGB[1] = _doc[OT_ACTION_PARAM_MESH_ITEM_COLOR_G].GetDouble();
				colorRGB[2] = _doc[OT_ACTION_PARAM_MESH_ITEM_COLOR_B].GetDouble();
				addVisualizationCartesianMeshItemNode(visModelID, name, UID, treeIcons, isHidden, facesList, colorRGB);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_TetMeshNodeTetEdges)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				ak::UID UID = _doc[OT_ACTION_PARAM_UI_UID].GetUint64();
				bool displayMeshEdges = _doc[OT_ACTION_PARAM_MODEL_TET_DISPLAYEDGES].GetBool();
				visualizationTetMeshNodeTetEdges(visModelID, UID, displayMeshEdges);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_AddMeshItemFromFacetDatabase)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string name = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
				ak::UID UID = _doc[OT_ACTION_PARAM_UI_UID].GetUint64();
				bool isHidden = _doc[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
				std::string projectName = ot::json::getString(_doc, OT_ACTION_PARAM_PROJECT_NAME);
				ak::UID entityID = _doc[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
				ak::UID entityVersion = _doc[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
				TreeIcon treeIcons = getTreeIconsFromDocument(_doc);
				long long tetEdgesID = _doc[OT_ACTION_PARAM_MODEL_TETEDGES_ID].GetUint64();;
				long long tetEdgesVersion = _doc[OT_ACTION_PARAM_MODEL_TETEDGES_Version].GetUint64();

				addVisualizationMeshItemNodeFromFacetDataBase(visModelID, name, UID, treeIcons, isHidden, projectName, entityID, entityVersion, tetEdgesID, tetEdgesVersion);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_AddText)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string name = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
				ak::UID UID = _doc[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
				bool isHidden = _doc[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
				bool isEditable = _doc[OT_ACTION_PARAM_MODEL_ITM_IsEditable].GetBool();

				std::string projectName = ot::json::getString(_doc, OT_ACTION_PARAM_PROJECT_NAME);
				ak::UID textID = _doc[OT_ACTION_PARAM_TEXT_ID].GetUint64();
				ak::UID textVersion = _doc[OT_ACTION_PARAM_TEXT_VERSION].GetUint64();

				TreeIcon treeIcons = getTreeIconsFromDocument(_doc);

				addVisualizationTextNode(visModelID, name, UID, treeIcons, isHidden, projectName, textID, textVersion);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_AddTable)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string name = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
				ak::UID UID = _doc[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
				bool isHidden = _doc[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
				bool isEditable = _doc[OT_ACTION_PARAM_MODEL_ITM_IsEditable].GetBool();

				std::string projectName = ot::json::getString(_doc, OT_ACTION_PARAM_PROJECT_NAME);
				ak::UID tableID = _doc[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
				ak::UID tableVersion = _doc[OT_ACTION_PARAM_MODEL_EntityVersion].GetUint64();

				TreeIcon treeIcons = getTreeIconsFromDocument(_doc);

				addVisualizationTableNode(visModelID, name, UID, treeIcons, isHidden, projectName, tableID, tableVersion);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_GetTableSelection)
			{
				ak::UID visualizationModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string senderURL = ot::json::getString(_doc, OT_ACTION_PARAM_SENDER_URL);
				std::string subsequentFunction = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_FunctionName);
				ot::ConstJsonObject serializedColor = ot::json::getObject(_doc, OT_ACTION_PARAM_COLOUR_BACKGROUND);

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
					displayInfoMessage("Table selection request could not be handled due to exception: " + std::string(e.what()));
				}
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_ShowTable)
			{
				ak::UID visualizationModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				ak::UID tableEntityID = _doc[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
				ak::UID tableEntityVersion = _doc[OT_ACTION_PARAM_MODEL_EntityVersion].GetUint64();
				std::string senderURL = ot::json::getString(_doc, OT_ACTION_PARAM_SENDER_URL);
				std::string subsequentFunction = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_FunctionName);
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
							displayInfoMessage("Response while refreshing colour was false");
							//assert(0); // ERROR
						}
						else OT_ACTION_IF_RESPONSE_WARNING(response)
						{
							displayInfoMessage("Response while refreshing colour was false");
							assert(0); // WARNING
						}
					}
				}
				catch (std::exception& e)
				{
					displayInfoMessage("Table could not be shown due to exception: " + std::string(e.what()));
				}
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_SetTable)
			{
				ak::UID visualizationModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				auto tableContent= ot::json::getObject(_doc, OT_ACTION_PARAM_Value);
				ot::GenericDataStructMatrix data;
				data.setFromJsonObject(tableContent);
				ViewerAPI::showTable(visualizationModelID, data);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_SelectRanges)
			{
				ak::UID visualizationModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				
				auto listOfSerializedRanges = ot::json::getObjectList(_doc, "Ranges");
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
				catch(std::exception& e)
				{
					displayInfoMessage("Table selection could not be executed due to exception: " + std::string(e.what()));
				}
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_ColourSelection)
			{
				ak::UID visualizationModelID = ot::json::getUInt64(_doc, OT_ACTION_PARAM_MODEL_ID);
				ot::ConstJsonObject serializedColor = ot::json::getObject(_doc, OT_ACTION_PARAM_COLOUR_BACKGROUND);
				
				try
				{
					ot::Color colour;
					colour.setFromJsonObject(serializedColor);
					ViewerAPI::ChangeColourOfSelection(visualizationModelID, colour);
				}
				catch (std::exception& e)
				{
					displayInfoMessage("Setting colour of table selection could not be executed due to exception: " + std::string(e.what()));
				}
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_AddPlot1D)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string name = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
				ak::UID UID = _doc[OT_ACTION_PARAM_UI_UID].GetUint64();
				bool isHidden = _doc[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
				std::string projectName = ot::json::getString(_doc, OT_ACTION_PARAM_PROJECT_NAME);

				int gridColor[3] = { 0, 0, 0 };
				std::string title = ot::json::getString(_doc, OT_ACTION_PARAM_VIEW1D_Title);
				std::string plotType = ot::json::getString(_doc, OT_ACTION_PARAM_VIEW1D_PlotType);
				std::string plotQuantity = ot::json::getString(_doc, OT_ACTION_PARAM_VIEW1D_PlotQuantity);
				bool grid = _doc[OT_ACTION_PARAM_VIEW1D_Grid].GetBool();
				bool legend = _doc[OT_ACTION_PARAM_VIEW1D_Legend].GetBool();
				bool logscaleX = _doc[OT_ACTION_PARAM_VIEW1D_LogscaleX].GetBool();
				bool logscaleY = _doc[OT_ACTION_PARAM_VIEW1D_LogscaleY].GetBool();
				bool autoscaleX = _doc[OT_ACTION_PARAM_VIEW1D_AutoscaleX].GetBool();
				bool autoscaleY = _doc[OT_ACTION_PARAM_VIEW1D_AutoscaleY].GetBool();
				double xmin = _doc[OT_ACTION_PARAM_VIEW1D_Xmin].GetDouble();
				double xmax = _doc[OT_ACTION_PARAM_VIEW1D_Xmax].GetDouble();
				double ymin = _doc[OT_ACTION_PARAM_VIEW1D_Ymin].GetDouble();
				double ymax = _doc[OT_ACTION_PARAM_VIEW1D_Ymax].GetDouble();
				gridColor[0] = _doc[OT_ACTION_PARAM_VIEW1D_GridColorR].GetInt();
				gridColor[1] = _doc[OT_ACTION_PARAM_VIEW1D_GridColorG].GetInt();
				gridColor[2] = _doc[OT_ACTION_PARAM_VIEW1D_GridColorB].GetInt();

				std::list<ak::UID> curvesID = getListFromDocument(_doc, OT_ACTION_PARAM_VIEW1D_CurveIDs);
				std::list<ak::UID> curvesVersions = getListFromDocument(_doc, OT_ACTION_PARAM_VIEW1D_CurveVersions);
				std::list<std::string> curvesNames = getStringListFromDocument(_doc, OT_ACTION_PARAM_VIEW1D_CurveNames);

				TreeIcon treeIcons = getTreeIconsFromDocument(_doc);

				addVisualizationPlot1DNode(visModelID, name, UID, treeIcons, isHidden, projectName, title, plotType, plotQuantity, grid, gridColor, legend, logscaleX, logscaleY,
					autoscaleX, autoscaleY, xmin, xmax, ymin, ymax, curvesID, curvesVersions, curvesNames);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_Plot1DPropsChanged)
			{
				int gridColor[3] = { 0, 0, 0 };
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				ak::UID UID = _doc[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
				std::string title = ot::json::getString(_doc, OT_ACTION_PARAM_VIEW1D_Title);
				std::string plotType = ot::json::getString(_doc, OT_ACTION_PARAM_VIEW1D_PlotType);
				std::string plotQuantity = ot::json::getString(_doc, OT_ACTION_PARAM_VIEW1D_PlotQuantity);
				bool grid = _doc[OT_ACTION_PARAM_VIEW1D_Grid].GetBool();
				bool legend = _doc[OT_ACTION_PARAM_VIEW1D_Legend].GetBool();
				bool logscaleX = _doc[OT_ACTION_PARAM_VIEW1D_LogscaleX].GetBool();
				bool logscaleY = _doc[OT_ACTION_PARAM_VIEW1D_LogscaleY].GetBool();
				bool autoscaleX = _doc[OT_ACTION_PARAM_VIEW1D_AutoscaleX].GetBool();
				bool autoscaleY = _doc[OT_ACTION_PARAM_VIEW1D_AutoscaleY].GetBool();
				double xmin = _doc[OT_ACTION_PARAM_VIEW1D_Xmin].GetDouble();
				double xmax = _doc[OT_ACTION_PARAM_VIEW1D_Xmax].GetDouble();
				double ymin = _doc[OT_ACTION_PARAM_VIEW1D_Ymin].GetDouble();
				double ymax = _doc[OT_ACTION_PARAM_VIEW1D_Ymax].GetDouble();
				gridColor[0] = _doc[OT_ACTION_PARAM_VIEW1D_GridColorR].GetInt();
				gridColor[1] = _doc[OT_ACTION_PARAM_VIEW1D_GridColorG].GetInt();
				gridColor[2] = _doc[OT_ACTION_PARAM_VIEW1D_GridColorB].GetInt();

				visualizationPlot1DPropertiesChanged(visModelID, UID, title, plotType, plotQuantity, grid, gridColor, legend, logscaleX, logscaleY, autoscaleX, autoscaleY, xmin, xmax, ymin, ymax);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_OBJ_Result1DPropsChanged)
			{
				ak::UID visModelID = ot::json::getUInt64(_doc, OT_ACTION_PARAM_MODEL_ID);
				ot::UIDList entityIDs = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_ITM_ID);
				ot::UIDList entityVersions = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_ITM_Version);
				auto entityVersion = entityVersions.begin();
				for (ot::UID entityID : entityIDs)
				{
					visualizationResult1DPropertiesChanged(visModelID, entityID, *entityVersion);
					entityVersion++;
				}
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_EnterEntitySelectionMode)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string selectionType = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_SelectionType);
				bool allowMultipleSelection = _doc[OT_ACTION_PARAM_MODEL_ITM_Selection_AllowMultipleSelection].GetBool();
				std::string selectionFilter = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ITM_Selection_Filter);
				std::string selectionAction = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ITM_Selection_Action);
				std::string selectionMessage = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ITM_Selection_Message);
				ot::serviceID_t replyToService = ot::json::getUInt(_doc, OT_ACTION_PARAM_MODEL_REPLYTO);
				std::list<std::string> optionNames = getStringListFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_Selection_OptNames);
				std::list<std::string> optionValues = getStringListFromDocument(_doc, OT_ACTION_PARAM_MODEL_ITM_Selection_OptValues);
				enterEntitySelectionMode(visModelID, replyToService, selectionType, allowMultipleSelection, selectionFilter, selectionAction, selectionMessage, optionNames, optionValues);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_SetModifiedState)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				bool modifiedState = _doc[OT_ACTION_PARAM_MODEL_ModifiedState].GetBool();

				isModified(visModelID, modifiedState);
			}
			else if (action == OT_ACTION_CMD_UI_SetProgressVisibility)
			{
				std::string message = ot::json::getString(_doc, OT_ACTION_PARAM_MESSAGE);
				bool visible = ot::json::getBool(_doc, OT_ACTION_PARAM_UI_CONTROL_VisibleState);
				bool continuous = ot::json::getBool(_doc, OT_ACTION_PARAM_UI_CONTROL_ContinuousState);

				AppBase * app = AppBase::instance();
				assert(app != nullptr);
				if (app != nullptr) app->setProgressBarVisibility(message.c_str(), visible, continuous);
			}
			else if (action == OT_ACTION_CMD_UI_SetProgressbarValue)
			{
				int percentage = ot::json::getInt(_doc, OT_ACTION_PARAM_PERCENT);

				AppBase * app = AppBase::instance();
				assert(app != nullptr);
				if (app != nullptr) app->setProgressBarValue(percentage);
			}
			else if (action == OT_ACTION_CMD_UI_Freeze3DView)
			{
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				bool flag = _doc[OT_ACTION_PARAM_UI_CONTROL_BOOLEAN_STATE].GetBool();

				freeze3DView(visModelID, flag);
			}
			else if (action == OT_ACTION_CMD_ServiceConnected) {
				ot::serviceID_t senderID = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				std::string senderURL = ot::json::getString(_doc, OT_ACTION_PARAM_SERVICE_URL);
				std::string senderName = ot::json::getString(_doc, OT_ACTION_PARAM_SERVICE_NAME);
				std::string senderType = ot::json::getString(_doc, OT_ACTION_PARAM_SERVICE_TYPE);
				ot::ServiceBase * connectedService = new ot::ServiceBase(senderName, senderType, senderURL, senderID);
				m_serviceIdMap.insert_or_assign(senderID, connectedService);
			}
			else if (action == OT_ACTION_CMD_ServiceDisconnected) {
				ot::serviceID_t senderID = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				std::string senderURL = ot::json::getString(_doc, OT_ACTION_PARAM_SERVICE_URL);
				std::string senderName = ot::json::getString(_doc, OT_ACTION_PARAM_SERVICE_NAME);
				std::string senderType = ot::json::getString(_doc, OT_ACTION_PARAM_SERVICE_TYPE);
				auto itm = m_serviceIdMap.find(senderID);
				if (itm != m_serviceIdMap.end()) {
					ot::ServiceBase * actualService = itm->second;
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
			}
			else if (action == OT_ACTION_CMD_ShutdownRequestedByService) {
				assert(0);	// Add external shutdown
			}
			else if (action == OT_ACTION_CMD_Message) {
				std::string message = ot::json::getString(_doc, OT_ACTION_PARAM_MESSAGE);
				ot::serviceID_t senderID = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				//NOTE, add shutdown and pre shutdown requests
				std::string msg("Message from ");
				msg.append(std::to_string(senderID)).append(": ").append(message);
				std::cout << msg << std::endl;
			}
			else if (action == OT_ACTION_CMD_UI_MODEL_Create) {
				AppBase * app = AppBase::instance();

				// Create a model and a view
				ModelUIDtype modelID = app->createModel();
				ViewerUIDtype viewID = app->createView(modelID, app->getCurrentProjectName());
				app->getViewerComponent()->activateModel(modelID);

				auto service = m_serviceIdMap.find(ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID));
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

				//NOTE, check the better location for this
				//ak::uiAPI::window::setWaitingAnimationVisible(AppBase::instance()->getDefaultObjectUID(uiServiceAPI::defaultObject::doMainWindow), false);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_Create) {
				auto manager = AppBase::instance();

				ot::JsonDocument docOut;
				docOut.SetObject();
				docOut.AddMember(OT_ACTION_PARAM_VIEW_ID, manager->createView(ot::json::getUInt64(_doc, OT_ACTION_PARAM_MODEL_ID),
					AppBase::instance()->getCurrentProjectName()), docOut.GetAllocator());

				return docOut.toJson();
			}
			else if (action == OT_ACTION_CMD_ServiceEmergencyShutdown) {

				QString msg("An unexpected error occurred and the session needs to be closed.");
				ak::uiAPI::promptDialog::show(msg, "Open Twin", ak::promptOkIconLeft, "DialogError", "Default", AppBase::instance()->mainWindow());
				exit(1);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_SetEntityName) {
				ak::UID entityID = _doc[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
				std::string entityName = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ITM_Name);

				ViewerAPI::setEntityName(entityID, entityName);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_RenameEntityName) {
				std::string fromPath = ot::json::getString(_doc, OT_ACTION_PARAM_PATH_FROM);
				std::string toPath = ot::json::getString(_doc, OT_ACTION_PARAM_PATH_To);

				ViewerAPI::renameEntityPath(fromPath, toPath);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_CreateRubberband) {
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				std::string note = ot::json::getString(_doc, OT_ACTION_PARAM_VIEW_RUBBERBAND_Note);
				std::string cfg = ot::json::getString(_doc, OT_ACTION_PARAM_VIEW_RUBBERBAND_Document);

				ViewerAPI::createRubberband(AppBase::instance()->getViewerComponent()->getActiveViewerModel(), serviceId, note, cfg);
			}
			else if (action == OT_ACTION_CMD_UI_Lock) {
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				auto flags = ot::ui::toLockType(ot::json::getStringList(_doc, OT_ACTION_PARAM_ElementLockTypes));
				m_lockManager->lock(getService(serviceId), flags);
			}
			else if (action == OT_ACTION_CMD_UI_Unlock) {
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				auto flags = ot::ui::toLockType(ot::json::getStringList(_doc, OT_ACTION_PARAM_ElementLockTypes));
				m_lockManager->unlock(getService(serviceId), flags);
			}
			else if (action == OT_ACTION_CMD_UI_AddSettingsData) {
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				ot::ServiceBase * service = getService(serviceId);
				if (service) {
					UserSettings::instance()->addFromService(service, _doc);
				}
				else {
					AppBase::instance()->appendInfoMessage("[ERROR] Dispatch: " OT_ACTION_CMD_UI_AddSettingsData ": Service not registered");

				}
			}
			else if (action == OT_ACTION_CMD_UI_AddIconSearchPath) {
#ifdef _DEBUG
				std::string iconPath = ot::json::getString(_doc, OT_ACTION_PARAM_UI_CONTROL_IconFolder);
				try {
					ak::uiAPI::addIconSearchPath(iconPath.c_str());
					AppBase::instance()->appendDebugMessage("[ERROR] Added icon search path: " + QString::fromStdString(iconPath));
				}
				catch (...) {
					AppBase::instance()->appendInfoMessage("[ERROR] Failed to add icon search path: Path not found");
				}
#endif // _DEBUG
			}
			else if (action == OT_ACTION_CMD_UI_AddPluginSearchPath) {
#ifdef _DEBUG
				std::string pluginPath = ot::json::getString(_doc, OT_ACTION_PARAM_UI_PLUGIN_PATH);
				m_owner->uiPluginManager()->addPluginSearchPath(pluginPath.c_str());
				AppBase::instance()->appendDebugMessage("[ERROR] Added UI plugin search path: " + QString::fromStdString(pluginPath));
#endif // _DEBUG
			}
			else if (action == OT_ACTION_CMD_UI_RequestPlugin) {
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				std::string pluginName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_PLUGIN_NAME);
				std::string pluginPath = ot::json::getString(_doc, OT_ACTION_PARAM_UI_PLUGIN_PATH);
				ot::ServiceBase * service = getService(serviceId);
				ak::UID pluginUid = m_owner->uiPluginManager()->loadPlugin(pluginName.c_str(), pluginPath.c_str(), service);

				if (pluginUid) {
					AppBase * app = AppBase::instance();

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
			}
			else if (action == OT_ACTION_CMD_UI_PluginMessage) {
				ot::serviceID_t serviceId = ot::json::getUInt(_doc, OT_ACTION_PARAM_SERVICE_ID);
				std::string pluginAction = ot::json::getString(_doc, OT_ACTION_PARAM_UI_PLUGIN_ACTION_MEMBER);
				std::string message = ot::json::getString(_doc, OT_ACTION_PARAM_MESSAGE);
				std::string pluginName = ot::json::getString(_doc, OT_ACTION_PARAM_UI_PLUGIN_NAME);
				unsigned long long pluginUID = ot::json::getUInt64(_doc, OT_ACTION_PARAM_UI_PLUGIN_UID);
				ot::ServiceBase * service = getService(serviceId);
				if (!AppBase::instance()->uiPluginManager()->forwardMessageToPlugin(pluginUID, pluginAction, message)) {
					return OT_ACTION_RETURN_INDICATOR_Error "Failed to process message";
				}
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_SetVersionGraph) {
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::list<std::string> versionList = ot::json::getStringList(_doc, OT_ACTION_PARAM_UI_GRAPH_VERSION);
				std::list<std::string> parentList = ot::json::getStringList(_doc, OT_ACTION_PARAM_UI_GRAPH_PARENT);
				std::list<std::string> descriptionList = ot::json::getStringList(_doc, OT_ACTION_PARAM_UI_GRAPH_DESCRIPTION);
				std::string activeVersion = ot::json::getString(_doc, OT_ACTION_PARAM_UI_GRAPH_ACTIVE);
				std::string activeBranch = ot::json::getString(_doc, OT_ACTION_PARAM_UI_GRAPH_BRANCH);

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
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_SetVersionGraphActive) {
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string activeVersion = ot::json::getString(_doc, OT_ACTION_PARAM_UI_GRAPH_ACTIVE);
				std::string activeBranch = ot::json::getString(_doc, OT_ACTION_PARAM_UI_GRAPH_BRANCH);

				ViewerAPI::setVersionGraphActive(visModelID, activeVersion, activeBranch);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_RemoveVersionGraphVersions) {
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::list<std::string> versions = ot::json::getStringList(_doc, OT_ACTION_PARAM_UI_GRAPH_VERSION);

				ViewerAPI::removeVersionGraphVersions(visModelID, versions);
			}
			else if (action == OT_ACTION_CMD_UI_VIEW_AddAndActivateNewVersionGraphVersion) {
				ak::UID visModelID = _doc[OT_ACTION_PARAM_MODEL_ID].GetUint64();
				std::string newVersion = ot::json::getString(_doc, OT_ACTION_PARAM_UI_GRAPH_VERSION);
				std::string parentVersion = ot::json::getString(_doc, OT_ACTION_PARAM_UI_GRAPH_PARENT);
				std::string description = ot::json::getString(_doc, OT_ACTION_PARAM_UI_GRAPH_DESCRIPTION);
				std::string activeBranch = ot::json::getString(_doc, OT_ACTION_PARAM_UI_GRAPH_BRANCH);

				ViewerAPI::addNewVersionGraphStateAndActivate(visModelID, newVersion, activeBranch, parentVersion, description);
			}
			else if (action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_FillItemPicker) {
				ot::BasicServiceInformation info;
				info.setFromJsonObject(_doc.GetConstObject());

				ot::GraphicsPickerCollectionPackage pckg;
				pckg.setFromJsonObject(ot::json::getObject(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

				AppBase::instance()->globalGraphicsPicker()->add(pckg);
			}
			else if (action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor) {
				ot::BasicServiceInformation info;
				info.setFromJsonObject(_doc.GetConstObject());
								
				ot::GraphicsNewEditorPackage pckg("", "");
				pckg.setFromJsonObject(ot::json::getObject(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

				AppBase::instance()->findOrCreateGraphicsEditor(pckg.name(), QString::fromStdString(pckg.title()), info);

				AppBase::instance()->globalGraphicsPicker()->add(pckg);
			}
			else if (action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem) {
				ot::BasicServiceInformation info;
				info.setFromJsonObject(_doc.GetConstObject());

				ot::GraphicsScenePackage pckg("");
				pckg.setFromJsonObject(ot::json::getObject(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

				ot::GraphicsView * editor = AppBase::instance()->findOrCreateGraphicsEditor(pckg.name(), QString::fromStdString(pckg.name()), info);
				
				for (auto graphicsItemCfg : pckg.items()) {
					ot::GraphicsItem* graphicsItem = ot::GraphicsFactory::itemFromConfig(graphicsItemCfg, true);
					if (graphicsItem != nullptr) {
						const double xCoordinate = graphicsItemCfg->position().x();
						const double yCoordinate = graphicsItemCfg->position().y();
						graphicsItem->getQGraphicsItem()->setPos(QPointF(xCoordinate, yCoordinate));
						editor->addItem(graphicsItem);
					}
				}
			}
			else if (action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveItem) {
				ot::BasicServiceInformation info;
				info.setFromJsonObject(_doc.GetConstObject());

				ot::UIDList itemUids = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemIds);

				if (_doc.HasMember(OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName)) {
					// Specific view

					std::string editorName = ot::json::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);
					ot::GraphicsView* editor = AppBase::instance()->findOrCreateGraphicsEditor(editorName, QString::fromStdString(editorName), info);

					if (editor) {
						for (auto itemUID : itemUids) {
							editor->removeItem(itemUID);
						}
					}
				}
				else {
					// Any view

					std::list<ot::GraphicsView *> views = AppBase::instance()->getAllGraphicsEditors();
					for (auto v : views) {
						for (auto uid : itemUids) {
							v->removeItem(uid);
						}
					}
				}
				
			}
			else if (action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection) {
				ot::BasicServiceInformation info;
				info.setFromJsonObject(_doc.GetConstObject());

				ot::GraphicsConnectionPackage pckg;
				pckg.setFromJsonObject(ot::json::getObject(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

				ot::GraphicsView* editor = AppBase::instance()->findOrCreateGraphicsEditor(pckg.name(), QString::fromStdString(pckg.name()), info);
				
				for (const auto& connection : pckg.connections()) {
					editor->addConnectionIfConnectedItemsExist(connection);
				}

			}
			else if (action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveConnection) {
				ot::BasicServiceInformation info;
				info.setFromJsonObject(_doc.GetConstObject());

				ot::GraphicsConnectionPackage pckg;
				pckg.setFromJsonObject(ot::json::getObject(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

				if (!pckg.name().empty()) {
					// Specific editor

					ot::GraphicsView* editor = AppBase::instance()->findOrCreateGraphicsEditor(pckg.name(), QString::fromStdString(pckg.name()), info);

					for (auto connection : pckg.connections()) {
						editor->removeConnection(connection.getUid());
					}
				}
				else {
					// Any editor

					std::list<ot::GraphicsView*> views = AppBase::instance()->getAllGraphicsEditors();
					for (auto view : views) {
						for (auto connection : pckg.connections()) {
							view->removeConnection(connection.getUid());
						}
					}
				}
			}
			else if (action == OT_ACTION_CMD_UI_TEXTEDITOR_SetText) {
				ot::BasicServiceInformation info;
				info.setFromJsonObject(_doc.GetConstObject());

				std::string editorName = ot::json::getString(_doc, OT_ACTION_PARAM_TEXTEDITOR_Name);
				std::string editorText = ot::json::getString(_doc, OT_ACTION_PARAM_TEXTEDITOR_Text);

				std::string editorTitle = editorName;
				if (_doc.HasMember(OT_ACTION_PARAM_TEXTEDITOR_Title)) {
					editorTitle = ot::json::getString(_doc, OT_ACTION_PARAM_TEXTEDITOR_Title);
				}

				ot::TextEditor* editor = AppBase::instance()->findOrCreateTextEditor(editorName, QString::fromStdString(editorTitle), info);
				editor->setPlainText(QString::fromStdString(editorText));
				editor->setContentChanged(false);
			}
			else if (action == OT_ACTION_CMD_UI_TEXTEDITOR_SetSaved) {
				ot::BasicServiceInformation info;
				info.setFromJsonObject(_doc.GetConstObject());

				std::string editorName = ot::json::getString(_doc, OT_ACTION_PARAM_TEXTEDITOR_Name);
				ot::TextEditor* editor = AppBase::instance()->findTextEditor(editorName, info);

				if (editor) {
					editor->setContentChanged(false);
				}
			}
			else if (action == OT_ACTION_CMD_UI_TEXTEDITOR_SetModified) {
				ot::BasicServiceInformation info;
				info.setFromJsonObject(_doc.GetConstObject());

				std::string editorName = ot::json::getString(_doc, OT_ACTION_PARAM_TEXTEDITOR_Name);
				ot::TextEditor* editor = AppBase::instance()->findTextEditor(editorName, info);

				if (editor) {
					editor->setContentChanged(true);
				}
			}
			else if (action == OT_ACTION_CMD_UI_TEXTEDITOR_Close) {
				ot::BasicServiceInformation info;
				info.setFromJsonObject(_doc.GetConstObject());

				std::string editorName = ot::json::getString(_doc, OT_ACTION_PARAM_TEXTEDITOR_Name);
				AppBase::instance()->closeTextEditor(editorName, info);
			}
			else if (action == OT_ACTION_CMD_UI_TEXTEDITOR_CloseAll) {
				ot::BasicServiceInformation info;
				info.setFromJsonObject(_doc.GetConstObject());

				AppBase::instance()->closeAllTextEditors(info);
			}
			else if (action == OT_ACTION_CMD_UI_SS_IMPORT) {

				QString fileName = QFileDialog::getOpenFileName(
					nullptr,
					"Import CST File",
					QDir::currentPath(),
					QString("*.cst ;; All files (*.*)"));

				if (fileName == "") return "";

				ot::Flags<ot::ui::lockType> lockFlags;
				lockFlags.setFlag(ot::ui::lockType::tlModelWrite);
				lockFlags.setFlag(ot::ui::lockType::tlViewWrite);
				lockFlags.setFlag(ot::ui::lockType::tlModelRead);
				m_lockManager->lock(nullptr, lockFlags);

				std::string studioSuiteServiceURL = ot::json::getString(_doc, OT_ACTION_PARAM_SERVICE_URL);

				StudioSuiteConnectorAPI::setStudioServiceData(studioSuiteServiceURL, this);

				std::thread workerThread(StudioSuiteConnectorAPI::importProject, fileName.toStdString(), AppBase::instance()->getCurrentProjectName());
				workerThread.detach();
			}
			else if (action == OT_ACTION_CMD_UI_SS_COMMIT) {

				// TODO: Get change comment from dialog
				std::string changeComment = "Project updated";

				// TODO: Get file name from list
				QString fileName = QFileDialog::getOpenFileName(
					nullptr,
					"Commit CST File",
					QDir::currentPath(),
					QString("*.cst ;; All files (*.*)"));

				if (fileName == "") return "";

				ot::Flags<ot::ui::lockType> lockFlags;
				lockFlags.setFlag(ot::ui::lockType::tlModelWrite);
				lockFlags.setFlag(ot::ui::lockType::tlViewWrite);
				lockFlags.setFlag(ot::ui::lockType::tlModelRead);
				m_lockManager->lock(nullptr, lockFlags);

				std::string studioSuiteServiceURL = ot::json::getString(_doc, OT_ACTION_PARAM_SERVICE_URL);

				StudioSuiteConnectorAPI::setStudioServiceData(studioSuiteServiceURL, this);

				// We need to make sure that the project is at the correct version in OT
				std::string currentVersion = StudioSuiteConnectorAPI::getCurrentVersion(fileName.toStdString(), AppBase::instance()->getCurrentProjectName());
				activateVersion(currentVersion);

				std::thread workerThread(StudioSuiteConnectorAPI::commitProject, fileName.toStdString(), AppBase::instance()->getCurrentProjectName(), changeComment);
				workerThread.detach();
			}
			else if (action == OT_ACTION_CMD_UI_SS_GET) {

				// TODO: Get file name from list
				QString fileName = QFileDialog::getOpenFileName(
					nullptr,
					"Commit CST File",
					QDir::currentPath(),
					QString("*.cst ;; All files (*.*)"));

				if (fileName == "") return "";

				ot::Flags<ot::ui::lockType> lockFlags;
				lockFlags.setFlag(ot::ui::lockType::tlModelWrite);
				lockFlags.setFlag(ot::ui::lockType::tlViewWrite);
				lockFlags.setFlag(ot::ui::lockType::tlModelRead);
				m_lockManager->lock(nullptr, lockFlags);

				std::string studioSuiteServiceURL = ot::json::getString(_doc, OT_ACTION_PARAM_SERVICE_URL);
				std::string version = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_Version);

				StudioSuiteConnectorAPI::setStudioServiceData(studioSuiteServiceURL, this);

				std::thread workerThread(StudioSuiteConnectorAPI::getProject, fileName.toStdString(), AppBase::instance()->getCurrentProjectName(), version);
				workerThread.detach();
			}
			else if (action == OT_ACTION_CMD_UI_SS_UPLOAD) {

				std::list<ot::UID> entityIDList = getListFromDocument(_doc, OT_ACTION_PARAM_MODEL_EntityIDList);
				std::list<ot::UID> entityVersionList = getListFromDocument(_doc, OT_ACTION_PARAM_MODEL_EntityVersionList);

				std::thread workerThread(StudioSuiteConnectorAPI::uploadFiles, entityIDList, entityVersionList);
				workerThread.detach();
			}
			else if (action == OT_ACTION_CMD_UI_SS_COPY) {

				std::string newVersion = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_Version);

				std::thread workerThread(StudioSuiteConnectorAPI::copyFiles, newVersion);
				workerThread.detach();
			}
			else if (action == OT_ACTION_CMD_UI_EntitySelectionDialog) {
				ot::ConstJsonObject cfgObj = ot::json::getObject(_doc, OT_ACTION_PARAM_Config);
				const ot::UID entityToBeExtended = ot::json::getUInt64(_doc, OT_ACTION_PARAM_MODEL_EntityID);
								
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
							jSelectedItemsNames.PushBack(ot::JsonString(selectedItem, responseDoc.GetAllocator()),responseDoc.GetAllocator());
						}
						responseDoc.AddMember(OT_ACTION_PARAM_UI_TREE_SelectedItems, jSelectedItemsNames, responseDoc.GetAllocator());
						responseDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, entityToBeExtended ,responseDoc.GetAllocator());
						responseDoc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_UpdateCurvesOfPlot, responseDoc.GetAllocator());
						std::string response;
						sendHttpRequest(QUEUE, m_modelServiceURL, responseDoc, response);
					}
				}
			}
			else if (action == OT_ACTION_CMD_UI_PropertyDialog) {
				ot::ConstJsonObject cfgObj = ot::json::getObject(_doc, OT_ACTION_PARAM_Config);

				ot::PropertyDialogCfg pckg;
				pckg.setFromJsonObject(cfgObj);

				ot::PropertyDialog dia(pckg, nullptr);
				dia.showDialog();

				if (dia.dialogResult() == ot::Dialog::Ok) {
					
				}
			}
			else if (action == OT_ACTION_CMD_UI_OnePropertyDialog) {
				ot::BasicServiceInformation info;
				info.setFromJsonObject(_doc.GetConstObject());

				ot::ConstJsonObject cfgObj = ot::json::getObject(_doc, OT_ACTION_PARAM_Config);
				const std::string subsequentFunction = ot::json::getString(_doc,OT_ACTION_PARAM_MODEL_FunctionName);
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
			}
			else if (action == OT_ACTION_CMD_UI_MessageDialog) {
				ot::BasicServiceInformation info;
				info.setFromJsonObject(_doc.GetConstObject());

				ot::ConstJsonObject cfgObj = ot::json::getObject(_doc, OT_ACTION_PARAM_Config);

				ot::MessageDialogCfg cfg;
				cfg.setFromJsonObject(cfgObj);

				ot::MessageDialogCfg::BasicButton result = ot::MessageDialog::showDialog(cfg);

				ot::JsonDocument responseDoc;
				responseDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_MessageDialogValue, responseDoc.GetAllocator()), responseDoc.GetAllocator());
				responseDoc.AddMember(OT_ACTION_PARAM_ObjectName, ot::JsonString(cfg.name(), responseDoc.GetAllocator()), responseDoc.GetAllocator());
				responseDoc.AddMember(OT_ACTION_PARAM_Value, ot::JsonString(ot::MessageDialogCfg::buttonToString(result), responseDoc.GetAllocator()), responseDoc.GetAllocator());

				std::string response;
				sendHttpRequest(EXECUTE, info, responseDoc, response);
			}
			else
			{
				return OT_ACTION_RETURN_INDICATOR_Error "Unknown action";
			}
			return "";
		}
		else { return OT_ACTION_RETURN_INDICATOR_Error "Invalid command provided"; }
	}
	catch (const std::exception & e) {
		std::string ex(OT_ACTION_RETURN_INDICATOR_Error "When executing action \"");
		ex.append(action).append("\": ").append(e.what());
		std::cout << ex << std::endl;
#ifdef _DEBUG
		ex.append("\n");
		{ OutputDebugStringA(ex.c_str()); }
#endif // _DEBUG
		return ex.c_str();
	}
	catch (...) {
		std::cout << OT_ACTION_RETURN_UnknownError << std::endl;
#ifdef _DEBUG
		{ OutputDebugStringA("Unknown error occured\n"); }
#endif // _DEBUG
		return OT_ACTION_RETURN_UnknownError;
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
		assert(0); // ERROR
		printSWarning(_sender, ("Response for context menu item clicked: " + response).c_str());
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		assert(0); // ERROR
		printSError(_sender, ("Response for context menu item clicked: " + response).c_str());
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
		assert(0); // ERROR
		printSWarning(_sender, ("Response for context menu item checked changed: " + response).c_str());
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
	assert(0); // ERROR
	printSError(_sender, ("Response for context menu item checked changed: " + response).c_str());
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

	// And finally send it through the websocket
	m_websocket->sendMessage(request, response);

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

		std::cout << "Closing project: Done" << std::endl;
	}
	catch (const std::exception & e) {
		assert(0); // Error handling
	}
	catch (...) {
		assert(0); // Error handling
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
void ExternalServicesComponent::selectFileForStoring(const std::string & dialogTitle, const std::string & fileMask, const std::string & subsequentFunction, const std::string & senderURL)
{
	try {
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

	}
	catch (...)
	{
		assert(0); // Error handling
	}
}

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

void ExternalServicesComponent::requestFileForReading(const std::string &dialogTitle, const std::string &fileMask, const std::string &subsequentFunction, const std::string &senderURL, bool loadContent)
{
	try {
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
	}
	catch (...) {
		assert(0); // Error handling
	}
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

void ExternalServicesComponent::saveFileContent(const std::string &dialogTitle, const std::string &fileName, const std::string &fileContent, ot::UID uncompressedDataLength)
{
	// Decode the encoded string into binary data
	int decoded_compressed_data_length = Base64decode_len(fileContent.c_str());
	char* decodedCompressedString = new char[decoded_compressed_data_length];

	Base64decode(decodedCompressedString, fileContent.c_str());

	// Decompress the data
	char* decodedString = new char[uncompressedDataLength];
	uLongf destLen = (uLongf)uncompressedDataLength;
	uLong  sourceLen = decoded_compressed_data_length;
	uncompress((Bytef *)decodedString, &destLen, (Bytef *)decodedCompressedString, sourceLen);

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
		// std::cout << "Entered: ExternalServicesComponent::performAction " << json << std::endl;

		// Build a json document from the string
		ot::JsonDocument doc;

		// Parse the document with the json string we have "received"
		doc.Parse(json);

		// Check if the document is an object
		assert(doc.IsObject()); // Doc is not an object

		//if (doc.HasMember(OT_ACTION_MEMBER)) {
		//	std::string action = ot::json::getString(doc, "action");
		//	std::cout << "EXECUTE message: " << action << std::endl;
		//}

		std::string result = dispatchAction(doc, senderIP);

		// Send the response back
		char *retval = new char[result.length() + 1];
		strcpy(retval, result.c_str());

		// std::cout << " return: " << retval << std::endl;

		lock = false;
		return retval;
	}
	catch (...) {
		assert(0); // Error handling
		lock = false;
		return nullptr;
	}

	lock = false;
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
		// std::cout << "Entered: ExternalServicesComponent::queueAction " << json << std::endl;

		// Build a json document from the string
		ot::JsonDocument doc;

		//std::string info(json);

		// Parse the document with the json string we have "received"
		doc.Parse(json);

		// In this function, we got a copy of the originally created data, so we need to delete it here again
		delete[] json;
		json = nullptr;

		// Check if the document is an object
		assert(doc.IsObject()); // Doc is not an object

		//if (doc.HasMember(OT_ACTION_MEMBER)) {
		//	std::string action = ot::json::getString(doc, "action");
		//	std::cout << "QUEUE message: " << action << std::endl;
		//}

		dispatchAction(doc, senderIP);

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

	delete [] url; url = nullptr;
	delete [] message; message = nullptr;
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

// ###################################################################################################

// JSON helper functions

void ExternalServicesComponent::AddUIDListToJsonDoc(ot::JsonDocument &doc, const std::string &itemName, const std::list<ModelUIDtype> &list)
{
	try {
		rapidjson::Value listUID(rapidjson::kArrayType);
		for (auto id : list)
		{
			listUID.PushBack(id, doc.GetAllocator());
		}

		doc.AddMember(rapidjson::Value(itemName.c_str(), doc.GetAllocator()), listUID, doc.GetAllocator());
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::AddStringListToJsonDoc(ot::JsonDocument &doc, const std::string &itemName, const std::list<std::string> &list)
{
	try {
		rapidjson::Value listUID(rapidjson::kArrayType);
		for (auto id : list)
		{
			listUID.PushBack(rapidjson::Value(id.c_str(), doc.GetAllocator()), doc.GetAllocator());
		}

		doc.AddMember(rapidjson::Value(itemName.c_str(), doc.GetAllocator()), listUID, doc.GetAllocator());
	}
	catch (...) {
		assert(0); // Error handling
	}
}

void ExternalServicesComponent::AddUIDVectorToJsonDoc(ot::JsonDocument &doc, const std::string &itemName, const std::vector<ModelUIDtype> &vector)
{
	try {
		rapidjson::Value vectorUID(rapidjson::kArrayType);
		for (auto id : vector)
		{
			vectorUID.PushBack(id, doc.GetAllocator());
		}

		doc.AddMember(rapidjson::Value(itemName.c_str(), doc.GetAllocator()), vectorUID, doc.GetAllocator());
	}
	catch (...) {
		assert(0); // Error handling
	}
}

std::string ExternalServicesComponent::getReturnJSONFromString(std::string fileName)
{
	try {
		ot::JsonDocument newDoc;
		newDoc.AddMember(OT_ACTION_PARAM_FILE_Name, rapidjson::Value(fileName.c_str(), newDoc.GetAllocator()), newDoc.GetAllocator());

		return newDoc.toJson();
	}
	catch (...) {
		assert(0); // Error handling
		return "";
	}
}

std::string ExternalServicesComponent::getReturnJSONFromVector(std::vector<ModelUIDtype> idList)
{
	try {
		ot::JsonDocument newDoc;
		rapidjson::Value vectorUID(rapidjson::kArrayType);
		for (auto id : idList)
		{
			vectorUID.PushBack(id, newDoc.GetAllocator());
		}

		newDoc.AddMember("idList", vectorUID, newDoc.GetAllocator());

		return newDoc.toJson();
	}
	catch (...) {
		assert(0); // Error handling
		return "";
	}
}

std::list<ModelUIDtype> ExternalServicesComponent::getListFromDocument(ot::JsonDocument &doc, const std::string &itemName)
{
	std::list<ModelUIDtype> result;
	try {
		rapidjson::Value list = doc[itemName.c_str()].GetArray();
		for (unsigned int i = 0; i < list.Size(); i++) { result.push_back(list[i].GetUint64()); }
	}
	catch (...) {
		assert(0); // Error handling
	}
	return result;
}

std::list<std::string> ExternalServicesComponent::getStringListFromDocument(ot::JsonDocument &doc, const std::string &itemName)
{
	std::list<std::string> result;
	try {
		rapidjson::Value list = doc[itemName.c_str()].GetArray();
		for (unsigned int i = 0; i < list.Size(); i++) { result.push_back(list[i].GetString()); }
	}
	catch (...) {
		assert(0); // Error handling
	}
	return result;
}

double *ExternalServicesComponent::getDoubleFromDocument(ot::JsonDocument &doc, const std::string &itemName)
{
	try {
		rapidjson::Value doubleArray = doc[itemName.c_str()].GetArray();

		double *result = new double[doubleArray.Size()];

		for (unsigned int i = 0; i < doubleArray.Size(); i++)
		{
			result[i] = doubleArray[i].GetDouble();
		}

		return result;
	}
	catch (...) {
		assert(0); // Error handling
		return nullptr;
	}
}

std::vector<ModelUIDtype> ExternalServicesComponent::getVectorFromDocument(ot::JsonDocument &doc, const std::string &itemName)
{
	std::vector<ModelUIDtype> result;
	try {
		rapidjson::Value vector = doc[itemName.c_str()].GetArray();
		for (unsigned int i = 0; i < vector.Size(); i++) { result.push_back(vector[i].GetUint64()); }
	}
	catch (...) {
		assert(0); // Error handling
	}
	return result;
}

std::vector<double> ExternalServicesComponent::getVectorDoubleFromDocument(ot::JsonDocument &doc, const std::string &itemName)
{
	std::vector<double> result;
	try {
		rapidjson::Value vector = doc[itemName.c_str()].GetArray();
		for (unsigned int i = 0; i < vector.Size(); i++) { result.push_back(vector[i].GetDouble()); }
	}
	catch (...) {
		assert(0); // Error handling
	}
	return result;
}

std::vector<int> ExternalServicesComponent::getVectorIntFromDocument(ot::JsonDocument &doc, const std::string &itemName)
{
	std::vector<int> result;
	try {
		rapidjson::Value vector = doc[itemName.c_str()].GetArray();
		for (unsigned int i = 0; i < vector.Size(); i++) { result.push_back(vector[i].GetInt()); }
	}
	catch (...) {
		assert(0); // Error handling
	}
	return result;
}

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
