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
#include "OpenTwinFoundation/UiComponent.h"
#include "OpenTwinFoundation/ModelComponent.h"
#include "OpenTwinCommunication/Msg.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/LinearGradientPainter2D.h"
#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsEditorPackage.h"
#include "OTGui/GraphicsLayoutItemCfg.h"

Application * g_instance{ nullptr };

Application * Application::instance(void) {
	if (g_instance == nullptr) { g_instance = new Application; }
	return g_instance;
}

void Application::deleteInstance(void) {
	if (g_instance) { delete g_instance; }
	g_instance = nullptr;
}

Application::Application()
	: ot::ApplicationBase(OT_INFO_SERVICE_TYPE_BlockEditorService, OT_INFO_SERVICE_TYPE_BlockEditorService, new UiNotifier(), new ModelNotifier())
{
	
}

Application::~Application()
{

}

// ##################################################################################################################################################################################################################

// Custom functions

std::string Application::handleExecuteModelAction(OT_rJSON_doc& _document) {
	std::string action = ot::rJSON::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	if (action == "Block Editor:Test:Empty") return createEmptyTestEditor();
	else assert(0); // Unhandled button action
	return std::string();
}

ot::GraphicsItemCfg* createTestBlock(const std::string& _name) {
	ot::GraphicsVBoxLayoutItemCfg* centralLayout = new ot::GraphicsVBoxLayoutItemCfg;

	ot::GraphicsTextItemCfg* title = new ot::GraphicsTextItemCfg;
	title->setName(_name);
	title->setText(_name);
	title->setBorder(ot::Border(ot::Color(rand() % 255, rand() % 255, rand() % 255), 2));

	ot::GraphicsHBoxLayoutItemCfg* midLayout = new ot::GraphicsHBoxLayoutItemCfg;

	ot::GraphicsRectangularItemCfg* leftRect = new ot::GraphicsRectangularItemCfg;
	leftRect->setSize(ot::Size2D(20, 20));
	ot::GraphicsRectangularItemCfg* rightRect = new ot::GraphicsRectangularItemCfg;
	rightRect->setSize(ot::Size2D(30, 30));

	midLayout->addChildItem(leftRect);
	midLayout->addStrech(1);
	midLayout->addChildItem(rightRect);

	centralLayout->addChildItem(title);
	centralLayout->addChildItem(midLayout, 1);

	return centralLayout;
}

ot::GraphicsItemCfg* createTestBlock2(const std::string& _name) {
	ot::GraphicsVBoxLayoutItemCfg* centralLayout = new ot::GraphicsVBoxLayoutItemCfg;

	ot::GraphicsTextItemCfg* title = new ot::GraphicsTextItemCfg;
	title->setName(_name);
	title->setText(_name);
	title->setBorder(ot::Border(ot::Color(rand() % 255, rand() % 255, rand() % 255), 2));

	ot::GraphicsHBoxLayoutItemCfg* midLayout = new ot::GraphicsHBoxLayoutItemCfg;

	ot::GraphicsRectangularItemCfg* leftRect = new ot::GraphicsRectangularItemCfg;
	ot::GraphicsRectangularItemCfg* rightRect = new ot::GraphicsRectangularItemCfg;

	midLayout->addChildItem(leftRect);
	midLayout->addStrech(1);
	midLayout->addChildItem(rightRect);

	centralLayout->addChildItem(title);
	centralLayout->addChildItem(midLayout, 1);

	return centralLayout;
}

std::string Application::createEmptyTestEditor(void) {
	if (m_uiComponent) {
		/*ot::BlockEditorConfigurationPackage pckg("AlwaysNumberOne", "Block Editor");
		pckg.setTopLevelBlockCategories(createTestCategories());

		if (!ot::BlockConfigurationAPI::createEmptyBlockEditor(this, m_uiComponent->serviceURL(), pckg)) {
			m_uiComponent->displayMessage("Failed to create empty block editor");
			return OT_ACTION_RETURN_INDICATOR_Error "Failed to create empty block editor";
		}*/

		ot::GraphicsEditorPackage pckg("TestPackage", "Test title");
		ot::GraphicsCollectionCfg* a = new ot::GraphicsCollectionCfg("A", "A");
		ot::GraphicsCollectionCfg* a1 = new ot::GraphicsCollectionCfg("1", "1");
		ot::GraphicsCollectionCfg* a2 = new ot::GraphicsCollectionCfg("2", "2");
		a->addChildCollection(a1);
		a1->addItem(createTestBlock("Alpha 1"));
		a->addChildCollection(a2);
		a2->addItem(createTestBlock2("Alpha 2"));
		pckg.addCollection(a);

		OT_rJSON_createDOC(doc);
		OT_rJSON_createValueObject(pckgObj);
		pckg.addToJsonObject(doc, pckgObj);

		ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateEmptyGraphicsEditor);
		ot::rJSON::add(doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);
		ot::GlobalOwner::instance().addToJsonObject(doc, doc);

		std::string response;
		if (!ot::msg::send("", m_uiComponent->serviceURL(), ot::QUEUE, ot::rJSON::toJSON(doc), response)) {
			return OT_ACTION_RETURN_VALUE_FAILED;
		}

		if (response != OT_ACTION_RETURN_VALUE_OK) {
			OT_LOG_E("Invalid response from UI");
			m_uiComponent->displayDebugMessage("Invalid response\n");
		}
	}
	return OT_ACTION_RETURN_VALUE_OK;
}

// ##################################################################################################################################################################################################################

// Required functions

void Application::run(void)
{
	// Add code that should be executed when the service is started and may start its work
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

	_ui->addMenuPage("Block Editor");
	_ui->addMenuGroup("Block Editor", "Test");
	_ui->addMenuButton("Block Editor", "Test", "Empty", "Create empty", ot::ui::lockType::tlModelWrite | ot::ui::tlViewRead | ot::ui::tlViewWrite, "Add");


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