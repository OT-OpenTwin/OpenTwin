#include "UiPluginComponent.h"
#include "UiPluginManager.h"
#include "AppBase.h"
#include "ExternalServicesComponent.h"

#include "OTCore/rJSON.h"
#include "OTCore/ServiceBase.h"
#include "OTCommunication/ActionTypes.h"

UiPluginComponent::UiPluginComponent(ExternalUiPlugin * _plugin) : m_plugin(_plugin) {}

UiPluginComponent::~UiPluginComponent() {}

// #########################################################################################

// Base class functions

void UiPluginComponent::sendMessageToService(const std::string& _message) {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_PluginMessage);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, AppBase::instance()->serviceID());
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_URL, AppBase::instance()->serviceURL());
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_NAME, AppBase::instance()->serviceName());
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_TYPE, AppBase::instance()->serviceType());
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_PLUGIN_UID, m_plugin->uid());
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_PLUGIN_NAME, m_plugin->name().toStdString());
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_MESSAGE, _message);
	std::string response;
	AppBase::instance()->getExternalServicesComponent()->sendHttpRequest(ExternalServicesComponent::EXECUTE, m_plugin->owner()->serviceURL(), cmdDoc, response);
	// Check if response is an error or warning
	OT_ACTION_IF_RESPONSE_ERROR(response) { assert(0); AppBase::instance()->appendInfoMessage(("[ERROR] Failed to send message to service: " + response + "\n").c_str()); }
	else OT_ACTION_IF_RESPONSE_WARNING(response) { assert(0); AppBase::instance()->appendInfoMessage(("[WARNING] Failed to send message to service: " + response + "\n").c_str()); }
}

void UiPluginComponent::appenInfoMessage(const QString& _message) {
	AppBase::instance()->appendInfoMessage(_message);
}

void UiPluginComponent::appenDebugMessage(const QString& _message) {
	AppBase::instance()->appendDebugMessage(_message);
}

bool UiPluginComponent::addNewTab(const QString& _tabTitle, QWidget * _widget) {
	AppBase::instance()->addTabToCentralView(_tabTitle, _widget);
	return true;
}