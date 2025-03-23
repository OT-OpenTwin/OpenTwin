//! @file Application.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// DebugService header
#include "Application.h"

// OpenTwin header
#include "OTCore/ReturnMessage.h"
#include "OTCore/ThisComputerInfo.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/AbstractUiNotifier.h"
#include "OTServiceFoundation/AbstractModelNotifier.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

#define OT_DEBUG_SERVICE_PAGE_NAME "Debug"

Application::Application() :
	ot::ApplicationBase(OT_INFO_SERVICE_TYPE_DebugService, OT_INFO_SERVICE_TYPE_DebugService, new ot::AbstractUiNotifier(), new ot::AbstractModelNotifier()) {
	// Add buttons here
	std::list<ButtonInfo> buttons;

	buttons.push_back(ButtonInfo("Test", "Hello", "SmileyGlasses", std::bind(&Application::testHello, this)));




	// --------------------------------------------------------------------------------------------------------+
	// Buttons will be added to the map                                                                        |
	for (ButtonInfo& btn : buttons) { //                                                                       |
		m_testButtons.emplace(OT_DEBUG_SERVICE_PAGE_NAME ":" + btn.group + ":" + btn.name, std::move(btn)); // |
	}                                 //                                                                       |
	// --------------------------------------------------------------------------------------------------------+
}

// ###########################################################################################################################################################################################################################################################################################################################

// Button callbacks

void Application::testHello(void) {
	if (this->getFeatureEnabled(DebugServiceConfig::FeatureFlag::SleepOnHello)) {
		OT_LOG_I("Sleeping for " + std::to_string(this->getFeatureSleepTime()) + "ms");
		std::this_thread::sleep_for(std::chrono::milliseconds(this->getFeatureSleepTime()));
		OT_LOG_I("Woke up");
	}

	if (this->getFeatureEnabled(DebugServiceConfig::FeatureFlag::ExitOnHello)) {
		OT_LOG_I("Performing exit on \"hello\"");
		exit(0);
	}

	OT_LOG_T("Hello :-)\n\nThis Computer Info:\n" + ot::ThisComputerInfo::toInfoString(ot::ThisComputerInfo::GatherAllMode));
}

// ###########################################################################################################################################################################################################################################################################################################################

// General feature handling

void Application::actionAboutToBePerformed(const char* _json) {
	ot::JsonDocument doc;
	if (!doc.fromJson(_json)) {
		OT_LOG_EAS("Failed to deserialize request: \"" + std::string(_json) + "\"");
		return;
	}

	std::string action = ot::json::getString(doc, OT_ACTION_MEMBER);

	if (action == OT_ACTION_CMD_Ping) {
		if (this->getFeatureEnabled(DebugServiceConfig::FeatureFlag::ExitOnPing)) {
			OT_LOG_I("Performing exit on ping");
			exit(0);
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// Default methods

Application* g_instance{ nullptr };

Application * Application::instance(void) {
	if (g_instance == nullptr) { g_instance = new Application; }
	return g_instance;
}

void Application::deleteInstance(void) {
	if (g_instance) { delete g_instance; }
	g_instance = nullptr;
}

Application::~Application()
{

}

std::string Application::handleExecuteModelAction(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);

	auto it = m_testButtons.find(action);
	if (it != m_testButtons.end()) {
		it->second.callback();
	}
	else {
		OT_LOG_WAS("Unknown model action \"" + action + "\"");
	}

	return std::string();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Required functions

void Application::run(void) {
	if (this->getFeatureEnabled(DebugServiceConfig::FeatureFlag::ExitOnRun)) {
		OT_LOG_I("Performing exit on run");
		exit(0);
	}
	// Add code that should be executed when the service is started and may start its work
}

std::string Application::processAction(const std::string & _action, ot::JsonDocument & _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument & _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);

	_ui->addMenuPage(OT_DEBUG_SERVICE_PAGE_NAME);

	std::list<std::string> addedGroups;
	for (const auto& it : m_testButtons) {
		// Add group if needed
		if (std::find(addedGroups.begin(), addedGroups.end(), it.second.group) == addedGroups.end()) {
			_ui->addMenuGroup(OT_DEBUG_SERVICE_PAGE_NAME, it.second.group);
			addedGroups.push_back(it.second.group);
		}

		_ui->addMenuButton(OT_DEBUG_SERVICE_PAGE_NAME, it.second.group, it.second.name, it.second.title, ot::LockModelWrite | ot::LockViewRead | ot::LockViewWrite, it.second.icon);
	}

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{

}

void Application::modelDisconnected(const ot::components::ModelComponent * _model)
{

}

void Application::serviceConnected(ot::ServiceBase * _service)
{

}

void Application::serviceDisconnected(const ot::ServiceBase * _service)
{

}

void Application::preShutdown(void) {
	if (this->getFeatureEnabled(DebugServiceConfig::FeatureFlag::ExitOnPreShutdown)) {
		OT_LOG_I("Performing exit on pre shutdown");
		exit(0);
	}
}

void Application::shuttingDown(void)
{

}

bool Application::startAsRelayService(void) const
{
	return false;	// Do not want the service to start a relay service. Otherwise change to true
}

ot::PropertyGridCfg Application::createSettings(void) const {
	return ot::PropertyGridCfg();
}

void Application::settingsSynchronized(const ot::PropertyGridCfg& _dataset) {

}

bool Application::settingChanged(const ot::Property * _item) {
	return false;
}

// ###########################################################################################################################################################################################################################################################################################################################
