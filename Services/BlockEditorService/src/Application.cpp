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
	else { 
		OT_LOG_W("Unknown model action");
		assert(0); }// Unhandled button action
	return std::string();
}

std::string Application::handleNewGraphicsItem(OT_rJSON_doc& _document) {
	std::string name = ot::rJSON::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName);
	ot::UID uid = ot::rJSON::getULongLong(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemId);
	std::string editorName = ot::rJSON::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);

	m_uiComponent->displayMessage("New item dropped { name: \"" + name + "\", UID: \"" + std::to_string(uid) + "\" } at editor { name: \"" + editorName + "\" }\n");

	return std::string(OT_ACTION_RETURN_VALUE_OK);
}

ot::GraphicsItemCfg* createTestBlock(const std::string& _name) {
	ot::GraphicsImageItemCfg* bot = new ot::GraphicsImageItemCfg;
	bot->setSize(ot::Size2D(100, 60));
	bot->setName(_name + "i");
	bot->setTitle(_name + "i");
	bot->setImagePath("Default/BugGreen");
	bot->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);

	ot::GraphicsTextItemCfg* topA = new ot::GraphicsTextItemCfg("Senor");
	topA->setName(_name + "tA");
	topA->setTitle(_name + "tA");

	ot::GraphicsTextItemCfg* topB = new ot::GraphicsTextItemCfg("Buggo");
	topB->setName(_name + "tB");
	topB->setTitle(_name + "tB");

	ot::GraphicsVBoxLayoutItemCfg* top = new ot::GraphicsVBoxLayoutItemCfg;
	top->setName(_name + "t");
	top->setTitle(_name + "t");
	top->addChildItem(topA);
	top->addChildItem(topB);
	top->setSize(ot::Size2D(100, 50));

	ot::GraphicsRectangularItemCfg* backgr = new ot::GraphicsRectangularItemCfg;
	backgr->setName(_name + "b");
	backgr->setTitle(_name + "b");
	backgr->setCornerRadius(5);
	backgr->setSize(ot::Size2D(100, 50));
	ot::GraphicsStackItemCfg* cfgTop = new ot::GraphicsStackItemCfg(bot, top);
	cfgTop->setName(_name + "s");
	cfgTop->setTitle(_name + "s");

	ot::GraphicsStackItemCfg* cfg = new ot::GraphicsStackItemCfg(backgr, cfgTop);
	cfg->setName(_name);
	cfg->setTitle(_name);
	cfg->setSize(ot::Size2D(100, 50));
	return cfg;
}

ot::GraphicsItemCfg* createTestBlock3(const std::string& _name) {
	ot::GraphicsVBoxLayoutItemCfg* centralLayout = new ot::GraphicsVBoxLayoutItemCfg;

	ot::GraphicsTextItemCfg* title = new ot::GraphicsTextItemCfg;
	title->setName(_name);
	title->setText(_name);
	title->setBorder(ot::Border(ot::Color(rand() % 255, rand() % 255, rand() % 255), 2));

	ot::GraphicsHBoxLayoutItemCfg* midLayout = new ot::GraphicsHBoxLayoutItemCfg;

	ot::GraphicsRectangularItemCfg* leftRect = new ot::GraphicsRectangularItemCfg;
	leftRect->setSize(ot::Size2D(20, 20));
	leftRect->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);

	ot::GraphicsRectangularItemCfg* rightRect = new ot::GraphicsRectangularItemCfg;
	rightRect->setSize(ot::Size2D(30, 30));
	rightRect->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);

	midLayout->addChildItem(leftRect);
	midLayout->addStrech(1);
	midLayout->addChildItem(rightRect);

	centralLayout->addChildItem(title);
	centralLayout->addChildItem(midLayout, 1);

	return centralLayout;
}

ot::GraphicsItemCfg* createTestBlock2(const std::string& _name) {
	ot::GraphicsGridLayoutItemCfg* centralLayout = new ot::GraphicsGridLayoutItemCfg(2, 2);

	ot::GraphicsTextItemCfg* title = new ot::GraphicsTextItemCfg;
	title->setName(_name);
	title->setText(_name);
	title->setBorder(ot::Border(ot::Color(rand() % 255, rand() % 255, rand() % 255), 2));

	ot::GraphicsRectangularItemCfg* leftRect = new ot::GraphicsRectangularItemCfg;
	leftRect->setSize(ot::Size2D(20, 20));
	leftRect->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
	ot::GraphicsRectangularItemCfg* rightRect = new ot::GraphicsRectangularItemCfg;
	rightRect->setSize(ot::Size2D(30, 30));
	rightRect->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);

	centralLayout->addChildItem(0, 0, title);
	centralLayout->addChildItem(0, 1, rightRect);
	centralLayout->addChildItem(1, 0, leftRect);

	centralLayout->setColumnStretch(0, 1);
	centralLayout->setRowStretch(1, 1);

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
		a1->addItem(createTestBlock3("Alpha 2"));
		a->addChildCollection(a2);
		a2->addItem(createTestBlock2("Alpha 3"));
		pckg.addCollection(a);

		OT_rJSON_createDOC(doc);
		OT_rJSON_createValueObject(pckgObj);
		pckg.addToJsonObject(doc, pckgObj);

		ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateEmptyGraphicsEditor);
		ot::rJSON::add(doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);
		ot::GlobalOwner::instance().addToJsonObject(doc, doc); //Eigentlich nur die OT_JSON_MEMBER_GlobalOwnerId gewrapped

		std::string response;
		std::string req = ot::rJSON::toJSON(doc);
		OT_LOG_D("GraphicsEditor requested with: " + req);
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