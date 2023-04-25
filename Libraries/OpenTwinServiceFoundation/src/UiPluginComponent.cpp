#include "OpenTwinCore/otAssert.h"

#include "OpenTwinCommunication/ActionTypes.h"

#include "OpenTwinFoundation/UiPluginComponent.h"
#include "OpenTwinFoundation/UiComponent.h"
#include "OpenTwinFoundation/ApplicationBase.h"

ot::components::UiPluginComponent::UiPluginComponent(unsigned long long _pluginUID, const std::string& _pluginName, ApplicationBase * _application, UiComponent * _uiComponent)
	: m_pluginUID(_pluginUID), m_pluginName(_pluginName), m_application(_application), m_uiComponent(_uiComponent) {}

ot::components::UiPluginComponent::~UiPluginComponent() {}

// ##################################################################################################

void ot::components::UiPluginComponent::sendQueuedMessage(const std::string& _action, const std::string& _message) {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_PluginMessage);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_PLUGIN_ACTION_MEMBER, _action);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_uiComponent->serviceID());
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_URL, m_uiComponent->serviceURL());
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_NAME, m_uiComponent->serviceName());
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_TYPE, m_uiComponent->serviceType());
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_PLUGIN_UID, m_pluginUID);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_PLUGIN_NAME, m_pluginName);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_MESSAGE, _message);
	m_application->sendMessage(true, m_uiComponent->serviceName(), cmdDoc);
}

void ot::components::UiPluginComponent::sendQueuedMessage(const std::string& _action, OT_rJSON_doc& _jsonDocument) {
	sendQueuedMessage(_action, rJSON::toJSON(_jsonDocument));
}