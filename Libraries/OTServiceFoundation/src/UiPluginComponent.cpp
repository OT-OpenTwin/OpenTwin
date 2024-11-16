#include "OTCore/OTAssert.h"

#include "OTCommunication/ActionTypes.h"

#include "OTServiceFoundation/UiPluginComponent.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ApplicationBase.h"

ot::components::UiPluginComponent::UiPluginComponent(unsigned long long _pluginUID, const std::string& _pluginName, ApplicationBase * _application, UiComponent * _uiComponent)
	: m_pluginUID(_pluginUID), m_pluginName(_pluginName), m_application(_application), m_uiComponent(_uiComponent) {}

ot::components::UiPluginComponent::~UiPluginComponent() {}

// ##################################################################################################

void ot::components::UiPluginComponent::sendQueuedMessage(const std::string& _action, const std::string& _message) {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_PluginMessage, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_PLUGIN_ACTION_MEMBER, JsonString(_action, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_uiComponent->getServiceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, JsonString(m_uiComponent->getServiceURL(), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, JsonString(m_uiComponent->getServiceName(), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, JsonString(m_uiComponent->getServiceType(), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_PLUGIN_UID, m_pluginUID, cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_PLUGIN_NAME, JsonString(m_pluginName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MESSAGE, JsonString(_message, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_uiComponent->getServiceName(), cmdDoc, response);
}

void ot::components::UiPluginComponent::sendQueuedMessage(const std::string& _action, JsonDocument& _jsonDocument) {
	sendQueuedMessage(_action, _jsonDocument.toJson());
}