/*
 * Application.cpp
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

// Service header
#include "Application.h"
#include "ModelNotifier.h"
#include "UiNotifier.h"

// Open twin header
#include "OpenTwinCore/Owner.h"
#include "OpenTwinCore/ReturnMessage.h"
#include "OpenTwinFoundation/UiComponent.h"
#include "OpenTwinFoundation/ModelComponent.h"
#include "OpenTwinCommunication/Msg.h"
#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/GraphicsFlowItemCfg.h"


Application * g_instance{ nullptr };

#define EXAMPLE_NAME_BLOCK1 "First"
#define EXAMPLE_NAME_Block2 "Second"


Application * Application::instance(void) {
	if (g_instance == nullptr) { g_instance = new Application; }
	return g_instance;
}

void Application::deleteInstance(void) {
	if (g_instance) { delete g_instance; }
	g_instance = nullptr;
}

Application::Application()
	: ot::ApplicationBase(OT_INFO_SERVICE_TYPE_CircuitSimulatorService, OT_INFO_SERVICE_TYPE_CircuitSimulatorService, new UiNotifier(), new ModelNotifier())
{
	
}

Application::~Application()
{

}

// ##################################################################################################################################################################################################################

// Custom functions
std::string Application::handleExecuteModelAction(OT_rJSON_doc& _document) {
	std::string action = ot::rJSON::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	if (action == "Circuit Simulator:Edit:New Circuit") return 	createNewCircuitEditor();
	else {
		OT_LOG_W("Unknown model action");
		assert(0);
	}// Unhandled button action
	return std::string();
}

namespace ottest
{
	ot::GraphicsItemCfg* createTestBlock1(const std::string _name)
	{
		ot::GraphicsFlowItemCfg flow;
		flow.setTitleBackgroundColor(0, 255, 0);
		flow.setBackgroundImagePath("Default/Cuboid");
		flow.setBackgroundColor(ot::Color(0, 255, 255));

		flow.addLeft("Input1", "Connect", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Black);
		flow.addRight("Output1", "Connect", ot::GraphicsFlowConnectorCfg::Circle, ot::Color::Black);

		return flow.createGraphicsItem(_name, _name);
	}

	ot::GraphicsItemCfg* createTestBlock2(const std::string _name)
	{
		ot::GraphicsFlowItemCfg flow;
		flow.setTitleBackgroundColor(0, 255, 0);
		flow.setBackgroundImagePath("Default/Cylinder");
		flow.setBackgroundColor(ot::Color(0, 255, 255));

		flow.addLeft("Input1", "Connect", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Blue);
		flow.addRight("Output1", "Connect", ot::GraphicsFlowConnectorCfg::Circle, ot::Color::Blue);

		return flow.createGraphicsItem(_name, _name);
	}
}

std::string Application:: createNewCircuitEditor(void)
{
	if (m_uiComponent) {
		

		ot::GraphicsNewEditorPackage pckg("Circuit", "Circuit Network");
		ot::GraphicsCollectionCfg* a = new ot::GraphicsCollectionCfg("CircuitElements", "Circuit Elements");
		ot::GraphicsCollectionCfg* a1 = new ot::GraphicsCollectionCfg("PassiveElements", "Passive Elements");
		
		a->addChildCollection(a1);
		a1->addItem(ottest::createTestBlock1(EXAMPLE_NAME_BLOCK1)); // In die Funktion kommt wie bei playground getItem bspw.
		a1->addItem(ottest::createTestBlock2(EXAMPLE_NAME_Block2));
		
		pckg.addCollection(a);

		OT_rJSON_createDOC(doc);
		OT_rJSON_createValueObject(pckgObj);
		pckg.addToJsonObject(doc, pckgObj);

		ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateEmptyGraphicsEditor);
		ot::rJSON::add(doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);
		ot::GlobalOwner::instance().addToJsonObject(doc, doc); //Eigentlich nur die OT_JSON_MEMBER_GlobalOwnerId gewrapped

		std::string response;
		std::string req = ot::rJSON::toJSON(doc);

		OT_LOG_D("Requesting empty graphics editor ( editor = \"" + pckg.name() + "\"; title = \"" + pckg.title() + "\" )");

		if (!ot::msg::send("", m_uiComponent->serviceURL(), ot::QUEUE, req, response)) {
			return OT_ACTION_RETURN_VALUE_FAILED;
		}
	}

	return OT_ACTION_RETURN_VALUE_OK;
}


	




// ##################################################################################################################################################################################################################

// Required functions

void Application::run(void)
{
	if (!EnsureDataBaseConnection())
	{
		assert(0);
	}
}

std::string Application::processAction(const std::string & _action, OT_rJSON_doc & _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, OT_rJSON_doc & _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);

	_ui->addMenuPage("Circuit Simulator");
	_ui->addMenuGroup("Circuit Simulator", "Edit");
	_ui->addMenuGroup("Circuit Simulator", "Simulate");
	_ui->addMenuButton("Circuit Simulator", "Edit","New Circuit", "New Circuit", ot::ui::lockType::tlModelWrite | ot::ui::tlViewRead | ot::ui::tlViewWrite, "Add","Default");
	_ui->addMenuButton("Circuit Simulator","Simulate","New Simulation","New Simulation", ot::ui::lockType::tlModelWrite | ot::ui::tlViewRead | ot::ui::tlViewWrite, "Add", "Default");

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);

}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::uiPluginConnected(ot::components::UiPluginComponent * _uiPlugin) {

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

}

void Application::shuttingDown(void)
{

}

bool Application::startAsRelayService(void) const
{
	return false;	// Do not want the service to start a relay service. Otherwise change to true
}

ot::SettingsData * Application::createSettings(void) {
	return nullptr;
}

void Application::settingsSynchronized(ot::SettingsData * _dataset) {

}

bool Application::settingChanged(ot::AbstractSettingsItem * _item) {
	return false;
}

// ##################################################################################################################################################################################################################