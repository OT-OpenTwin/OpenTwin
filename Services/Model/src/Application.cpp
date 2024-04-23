//! @file Application.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// Model header
#include "stdafx.h"
#include "Application.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCommunication/ActionTypes.h"

Application* Application::instance(void) {
	static Application* g_instance{ nullptr };
	if (g_instance == nullptr) { g_instance = new Application; }
	return g_instance;
}

// ##################################################################################################################################################################################################################

// Required functions

void Application::run(void) {

}

std::string Application::processAction(const std::string& _action, ot::JsonDocument& _doc) {
	
	return "";
}

std::string Application::processMessage(ServiceBase* _sender, const std::string& _message, ot::JsonDocument& _doc) {

	return "";
}

void Application::uiConnected(ot::components::UiComponent* _ui) {

}

void Application::uiDisconnected(const ot::components::UiComponent* _ui) {

}

void Application::uiPluginConnected(ot::components::UiPluginComponent* _uiPlugin) {

}

void Application::modelConnected(ot::components::ModelComponent* _model) {

}

void Application::modelDisconnected(const ot::components::ModelComponent* _model) {

}

void Application::serviceConnected(ot::ServiceBase* _service) {

}

void Application::serviceDisconnected(const ot::ServiceBase* _service) {

}

void Application::preShutdown(void) {

}

void Application::shuttingDown(void) {

}

bool Application::startAsRelayService(void) const {
	return false;
}

ot::SettingsData* Application::createSettings(void) {
	return nullptr;
}

void Application::settingsSynchronized(ot::SettingsData* _dataset) {

}

bool Application::settingChanged(ot::AbstractSettingsItem* _item) {
	return false;
}

// ##################################################################################################################################################################################################################

// Private functions

Application::Application() 
	: ot::ApplicationBase(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL, nullptr, nullptr)
{

}

Application::~Application() {

}
