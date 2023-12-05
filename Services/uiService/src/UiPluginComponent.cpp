#include "UiPluginComponent.h"
#include "UiPluginManager.h"
#include "AppBase.h"
#include "ExternalServicesComponent.h"

#include "OTCore/JSON.h"
#include "OTCore/ServiceBase.h"
#include "OTCommunication/ActionTypes.h"

UiPluginComponent::UiPluginComponent(ExternalUiPlugin * _plugin) : m_plugin(_plugin) {}

UiPluginComponent::~UiPluginComponent() {}

// #########################################################################################

// Base class functions

void UiPluginComponent::sendMessageToService(const std::string& _message) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_PluginMessage, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, AppBase::instance()->serviceID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(AppBase::instance()->serviceURL(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(AppBase::instance()->serviceName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(AppBase::instance()->serviceType(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_PLUGIN_UID, m_plugin->uid(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_PLUGIN_NAME, ot::JsonString(m_plugin->name().toStdString(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESSAGE, ot::JsonString(_message, doc.GetAllocator()), doc.GetAllocator());
	std::string response;
	AppBase::instance()->getExternalServicesComponent()->sendHttpRequest(ExternalServicesComponent::EXECUTE, m_plugin->owner()->serviceURL(), doc, response);
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