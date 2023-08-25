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
#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsEditorPackage.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/GraphicsFlowItemCfg.h"

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

std::string Application::handleNewGraphicsItemConnection(OT_rJSON_doc& _document) {
	std::string editorName = ot::rJSON::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);

	ot::UID originUid = ot::rJSON::getULongLong(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_OriginId);
	std::string originConnectable = ot::rJSON::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_OriginConnetableName);

	ot::UID destUid = ot::rJSON::getULongLong(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_DestId);
	std::string destConnectable = ot::rJSON::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_DestConnetableName);

	m_uiComponent->displayMessage("New connection dropped { o.i: \"" + std::to_string(originUid) + "\", o.c: \"" + originConnectable + 
		"\", d.i: \"" + std::to_string(destUid) + "\", d.c: \"" + destConnectable + 
		"\" } at editor { name: \"" + editorName + "\" }\n");

	return std::string(OT_ACTION_RETURN_VALUE_OK);
}

std::string Application::handleGraphicsSelectionChanged(OT_rJSON_doc& _document) {
	std::string editorName = ot::rJSON::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);
	auto sel = ot::rJSON::getULongLongList(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemIds);

	std::string msg = "Selection changed: [";
	bool f = true;
	for (auto s : sel) {
		if (!f) msg.append("; ");
		f = false;
		msg.append(std::to_string(s));
	}
	msg.append("]\n");

	m_uiComponent->displayMessage(msg);

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

ot::GraphicsItemCfg* createTestBlock2(const std::string& _name) {
	ot::GraphicsVBoxLayoutItemCfg* centralLayout = new ot::GraphicsVBoxLayoutItemCfg;
	centralLayout->setName(_name);
	centralLayout->setTitle(_name);

	ot::GraphicsTextItemCfg* title = new ot::GraphicsTextItemCfg;
	title->setName("Title");
	title->setText("Title");

	ot::GraphicsRectangularItemCfg* leftRect = new ot::GraphicsRectangularItemCfg;
	leftRect->setName("Left");
	leftRect->setSize(ot::Size2D(20, 20));
	leftRect->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);

	centralLayout->addChildItem(title);
	centralLayout->addChildItem(leftRect);

	return centralLayout;
}

ot::GraphicsItemCfg* createTestBlock3(const std::string& _name) {
	ot::GraphicsVBoxLayoutItemCfg* centralLayout = new ot::GraphicsVBoxLayoutItemCfg;
	centralLayout->setName(_name);
	
	ot::GraphicsTextItemCfg* tit = new ot::GraphicsTextItemCfg;
	tit->setText(_name);
	
	ot::GraphicsGridLayoutItemCfg* grid = new ot::GraphicsGridLayoutItemCfg(2, 3);
	grid->setColumnStretch(1, 1);

	ot::GraphicsHBoxLayoutItemCfg* b1 = new ot::GraphicsHBoxLayoutItemCfg;
	ot::GraphicsRectangularItemCfg* r1 = new ot::GraphicsRectangularItemCfg;
	r1->setSize(ot::Size2D(10, 10));
	r1->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
	ot::GraphicsTextItemCfg* t1 = new ot::GraphicsTextItemCfg;
	t1->setText("Test 1");
	b1->addChildItem(r1);
	b1->addChildItem(t1);
	b1->addStrech(1);

	ot::GraphicsHBoxLayoutItemCfg* b2 = new ot::GraphicsHBoxLayoutItemCfg;
	ot::GraphicsRectangularItemCfg* r2 = new ot::GraphicsRectangularItemCfg;
	r2->setSize(ot::Size2D(10, 10));
	r2->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
	ot::GraphicsTextItemCfg* t2 = new ot::GraphicsTextItemCfg;
	t2->setText("Test 2");
	b2->addChildItem(r2);
	b2->addChildItem(t2);
	b2->addStrech(1);

	ot::GraphicsHBoxLayoutItemCfg* b3 = new ot::GraphicsHBoxLayoutItemCfg;
	ot::GraphicsRectangularItemCfg* r3 = new ot::GraphicsRectangularItemCfg;
	r3->setSize(ot::Size2D(10, 10));
	r3->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
	ot::GraphicsTextItemCfg* t3 = new ot::GraphicsTextItemCfg;
	t3->setText("Test 3");
	b3->addChildItem(r3);
	b3->addChildItem(t3);
	b3->addStrech(1);

	centralLayout->addChildItem(tit);
	centralLayout->addChildItem(grid, 1);

	grid->addChildItem(0, 0, b1);
	grid->addChildItem(0, 2, b2);
	grid->addChildItem(1, 2, b3);

	return centralLayout;
}

ot::GraphicsItemCfg* createTestBlock4(const std::string& _name) {
	ot::GraphicsFlowItemCfg flow;
	flow.setTitleBackgroundColor(255, 0, 0);
	//flow->setBackgroundImagePath("Default/python");

	flow.addInput("SomeIn1", "Run", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Blue);
	flow.addInput("SomeIn2", "Test", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Orange);
	flow.addInput("SomeIn3", "Perform shutdown", ot::GraphicsFlowConnectorCfg::Circle, ot::Color::IndianRed);
	flow.addOutput("SomeOut1", "Success", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Blue);
	flow.addOutput("SomeOut2", "Failed", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Yellow);
	flow.addOutput("SomeOut3", "Error", ot::GraphicsFlowConnectorCfg::Circle, ot::Color::Red);

	return flow.createGraphicsItem(_name, _name);
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
		a1->addItem(createTestBlock4("Alpha 1"));
		a->addChildCollection(a2);
		a2->addItem(createTestBlock4("Alpha 2"));
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