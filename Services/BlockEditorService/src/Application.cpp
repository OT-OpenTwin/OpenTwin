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
#include "OTCore/ReturnMessage.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTCommunication/Msg.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/LinearGradientPainter2D.h"

#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/GraphicsVBoxLayoutItemCfg.h"
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"
#include "OTGui/GraphicsGridLayoutItemCfg.h"
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTGui/GraphicsFlowItemBuilder.h"
#include "OTGui/GraphicsTextItemCfg.h"
#include "OTGui/GraphicsStackItemCfg.h"

#include "OTGui/LinearGradientPainter2D.h"

Application * g_instance{ nullptr };

#define EXAMPLE_NAME_Block1 "Alpha 1"
#define EXAMPLE_NAME_Block2 "Bravo 2"
#define EXAMPLE_NAME_Block3 "Super mega ultra duper flex mex hex dex rex jax lax Gamma 3"
#define EXAMPLE_NAME_Block4 "Delta 4"
#define EXAMPLE_NAME_Block5 "Echo 5"

namespace ottest {
	static unsigned long long currentBlockUid = 0;

	ot::GraphicsFlowItemConnector getDefaultConnectorStyle(void) {
		ot::GraphicsFlowItemConnector cfg;

		cfg.setTextColor(ot::Color(0, 0, 0));

		return cfg;
	}

	ot::GraphicsItemCfg* createTestBlock1(const std::string& _name) {
		ot::GraphicsFlowItemBuilder flow;
		flow.setTitleBackgroundColor(0, 255, 0);
		flow.setBackgroundImagePath("Images/Test2");
		flow.setDefaultConnectorStyle(ottest::getDefaultConnectorStyle());
		flow.setBackgroundColor(ot::Color(255, 255, 255));
		//flow->setBackgroundImagePath("Default/python");

		flow.addLeft("SomeIn1", "Run", ot::GraphicsFlowItemConnector::Square, ot::Color::Blue);
		flow.addLeft("SomeIn2", "Test", ot::GraphicsFlowItemConnector::Square, ot::Color::Orange);
		flow.addRight("SomeOut1", "Success", ot::GraphicsFlowItemConnector::Square, ot::Color::Blue);
		flow.addRight("SomeOut2", "Failed", ot::GraphicsFlowItemConnector::Square, ot::Color::Yellow);

		return flow.createGraphicsItem(_name, _name);
	}

	ot::GraphicsItemCfg* createTestBlock2(const std::string& _name) {
		ot::GraphicsFlowItemBuilder flow;
		flow.setTitleBackgroundColor(255, 0, 0);
		flow.setDefaultConnectorStyle(ottest::getDefaultConnectorStyle());
		//flow->setBackgroundImagePath("Default/python");
		flow.setLeftTitleCornerImagePath("Images/Test3");

		flow.addLeft("SomeIn1", "Run", ot::GraphicsFlowItemConnector::Square, ot::Color::Blue);
		flow.addLeft("SomeIn2", "Test", ot::GraphicsFlowItemConnector::Square, ot::Color::Orange);
		flow.addLeft("SomeIn3", "Perform shutdown", ot::GraphicsFlowItemConnector::Circle, ot::Color::IndianRed);
		flow.addRight("SomeOut1", "Success", ot::GraphicsFlowItemConnector::TriangleLeft, ot::Color::Blue);
		flow.addRight("SomeOut2", "Failed", ot::GraphicsFlowItemConnector::TriangleRight, ot::Color::Yellow);

		return flow.createGraphicsItem(_name, _name);
	}

	ot::GraphicsItemCfg* createTestBlock3(const std::string& _name) {
		ot::GraphicsFlowItemBuilder flow;
		
		flow.setTitleBackgroundGradientColor(ot::Color(ot::Color::Orange));
		flow.setDefaultConnectorStyle(ottest::getDefaultConnectorStyle());
		
		flow.addLeft("SomeIn1", "Run", ot::GraphicsFlowItemConnector::Square, ot::Color::Blue);
		flow.addRight("SomeOut1", "Success", ot::GraphicsFlowItemConnector::Square, ot::Color::Blue);
		flow.addRight("SomeOut2", "Failed", ot::GraphicsFlowItemConnector::Square, ot::Color::Yellow);

		return flow.createGraphicsItem(_name, _name);
	}

	ot::GraphicsItemCfg* createTestItem1(const std::string& _name) {
		ot::GraphicsVBoxLayoutItemCfg* root = new ot::GraphicsVBoxLayoutItemCfg;
		root->setName(_name);
		root->setTitle(_name);

		ot::GraphicsHBoxLayoutItemCfg* top = new ot::GraphicsHBoxLayoutItemCfg;
		top->setName(_name + "_top");

		ot::GraphicsGridLayoutItemCfg* mid = new ot::GraphicsGridLayoutItemCfg(2, 3);
		mid->setName(_name + "_mid");

		ot::GraphicsHBoxLayoutItemCfg* bot = new ot::GraphicsHBoxLayoutItemCfg;
		bot->setName(_name + "_bot");

		root->addChildItem(top);
		root->addChildItem(mid);
		root->addChildItem(bot);

		ot::GraphicsRectangularItemCfg* r1 = new ot::GraphicsRectangularItemCfg;
		r1->setName(_name + "_r1");
		r1->setSize(ot::Size2DD(10., 10.));

		ot::GraphicsRectangularItemCfg* r2 = new ot::GraphicsRectangularItemCfg;
		r2->setName(_name + "_r2");
		r2->setSize(ot::Size2DD(15., 15.));

		ot::GraphicsRectangularItemCfg* r3 = new ot::GraphicsRectangularItemCfg;
		r3->setName(_name + "_r3");
		r3->setSize(ot::Size2DD(5., 5.));

		ot::GraphicsRectangularItemCfg* r4 = new ot::GraphicsRectangularItemCfg;
		r4->setName(_name + "_r4");
		r4->setSize(ot::Size2DD(20., 20.));

		ot::GraphicsRectangularItemCfg* r5 = new ot::GraphicsRectangularItemCfg;
		r5->setName(_name + "_r5");
		r5->setSize(ot::Size2DD(12., 12.));

		ot::GraphicsRectangularItemCfg* r6 = new ot::GraphicsRectangularItemCfg;
		r6->setName(_name + "_r6");
		r6->setSize(ot::Size2DD(12., 12.));

		ot::GraphicsRectangularItemCfg* r7 = new ot::GraphicsRectangularItemCfg;
		r7->setName(_name + "_r7");
		r7->setSize(ot::Size2DD(12., 12.));

		ot::GraphicsRectangularItemCfg* r8 = new ot::GraphicsRectangularItemCfg;
		r8->setName(_name + "_r8");
		r8->setSize(ot::Size2DD(8., 8.));
		r8->setAlignment(ot::AlignLeft);

		top->addStrech(1);
		top->addChildItem(r1);
		top->addStrech(1);

		mid->addChildItem(0, 0, r2);
		mid->addChildItem(0, 1, r3);
		mid->addChildItem(1, 1, r4);
		mid->addChildItem(1, 2, r5);

		bot->addChildItem(r6);
		bot->addStrech(1);
		bot->addChildItem(r7);

		root->addChildItem(r8);

		return root;
	}

	ot::GraphicsItemCfg* createTestItem2(const std::string& _name) {
		ot::GraphicsVBoxLayoutItemCfg* root = new ot::GraphicsVBoxLayoutItemCfg;
		root->setName(_name);
		root->setTitle(_name);

		ot::GraphicsStackItemCfg* s1 = new ot::GraphicsStackItemCfg;
		s1->setName(_name + "s1");

		ot::GraphicsStackItemCfg* s2 = new ot::GraphicsStackItemCfg;
		s2->setName(_name + "s2");

		ot::GraphicsHBoxLayoutItemCfg* h1 = new ot::GraphicsHBoxLayoutItemCfg;
		h1->setName(_name + "h1");

		ot::GraphicsHBoxLayoutItemCfg* h2 = new ot::GraphicsHBoxLayoutItemCfg;
		h2->setName(_name + "h2");

		ot::GraphicsRectangularItemCfg* r1 = new ot::GraphicsRectangularItemCfg;
		r1->setName(_name + "r1");
		r1->setBackgroundPainer(new ot::FillPainter2D(ot::Color(255, 0, 0)));
		r1->setSize(ot::Size2DD(10., 10.));

		ot::GraphicsRectangularItemCfg* r2 = new ot::GraphicsRectangularItemCfg;
		r2->setName(_name + "r2");
		r2->setBackgroundPainer(new ot::FillPainter2D(ot::Color(0, 255, 0)));
		r2->setSize(ot::Size2DD(10., 10.));

		ot::GraphicsRectangularItemCfg* r3 = new ot::GraphicsRectangularItemCfg;
		r3->setName(_name + "r3");
		r3->setBackgroundPainer(new ot::FillPainter2D(ot::Color(0, 0, 255)));
		r3->setSize(ot::Size2DD(100., 10.));

		ot::GraphicsTextItemCfg* t1 = new ot::GraphicsTextItemCfg;
		t1->setName(_name + "t1");
		t1->setText("Hello my darkest of the Worlds");

		root->addChildItem(s1);
		root->addChildItem(s2);

		s1->addItemTop(h1, true, false);
		s2->addItemTop(h2, true, false);

		h1->addChildItem(r1);
		h1->addChildItem(r2);

		h2->addChildItem(t1);

		return root;
	}

}

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

	// We allow all items and do NOT store any information
	
	// Get item information
	std::string itemName = ot::rJSON::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName);
	std::string editorName = ot::rJSON::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);

	OT_rJSON_val posObj = _document[OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition].GetObject();
	ot::Point2DD pos;
	pos.setFromJsonObject(posObj);

	// Here we would now check and store the item information
	OT_LOG_D("Handling new graphics item request ( name = \"" + itemName + "\"; editor = \"" + editorName + "\"; x = " + std::to_string(pos.x()) + "; y = " + std::to_string(pos.y()) + " )");

	// Create package
	ot::GraphicsScenePackage pckg("TestPackage");

	// Create item configuration for the item to add
	ot::GraphicsItemCfg* itm = nullptr;
	if (itemName == EXAMPLE_NAME_Block1) itm = ottest::createTestBlock1(EXAMPLE_NAME_Block1);
	else if (itemName == EXAMPLE_NAME_Block2) itm = ottest::createTestBlock2(EXAMPLE_NAME_Block2);
	else if (itemName == EXAMPLE_NAME_Block3) itm = ottest::createTestBlock3(EXAMPLE_NAME_Block3);
	else if (itemName == EXAMPLE_NAME_Block4) itm = ottest::createTestItem1(EXAMPLE_NAME_Block4);
	else if (itemName == EXAMPLE_NAME_Block5) itm = ottest::createTestItem2(EXAMPLE_NAME_Block5);
	else {
		m_uiComponent->displayMessage("[ERROR] Unknown item: " + itemName + "\n");
		return OT_ACTION_RETURN_VALUE_FAILED;
	}
	itm->setPosition(pos);
	itm->setUid(std::to_string(++ottest::currentBlockUid));
	pckg.addItem(itm);

	OT_rJSON_createDOC(reqDoc);
	ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem);

	OT_rJSON_createValueObject(pckgObj);
	pckg.addToJsonObject(reqDoc, pckgObj);
	ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);

	this->getBasicServiceInformation().addToJsonObject(reqDoc, reqDoc);

	m_uiComponent->sendMessage(true, reqDoc);

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok, ot::rJSON::toJSON(pckgObj));
}

std::string Application::handleRemoveGraphicsItem(OT_rJSON_doc& _document) {

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string Application::handleNewGraphicsItemConnection(OT_rJSON_doc& _document) {
	OT_rJSON_checkMember(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, Object);
	OT_rJSON_val pckgObj = _document[OT_ACTION_PARAM_GRAPHICSEDITOR_Package].GetObject();

	ot::GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(pckgObj);

	// Here we would check and store the connection information
	OT_LOG_D("Handling new graphics item connection request ( editor = \"" + pckg.name() + "\" )");

	// Request UI to add connections
	OT_rJSON_createDOC(reqDoc);
	ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection);

	// Add received package to reuest (all connections are allowed)
	OT_rJSON_createValueObject(reqPckgObj);
	pckg.addToJsonObject(reqDoc, reqPckgObj);
	ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, reqPckgObj);

	this->getBasicServiceInformation().addToJsonObject(reqDoc, reqDoc);

	m_uiComponent->sendMessage(true, reqDoc);

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string Application::handleRemoveGraphicsItemConnection(OT_rJSON_doc& _document) {
	std::string editorName = ot::rJSON::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);

	OT_rJSON_checkMember(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, Object);
	OT_rJSON_val pckgObj = _document[OT_ACTION_PARAM_GRAPHICSEDITOR_Package].GetObject();

	ot::GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(pckgObj);

	// Here we would check and remove the connection information
	OT_LOG_D("Handling remove graphics item connection request ( editor = \"" + pckg.name() + "\" )");

	// Request UI to add connections
	OT_rJSON_createDOC(reqDoc);
	ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveConnection);

	// Add received package to reuest (all connections are allowed)
	OT_rJSON_createValueObject(reqPckgObj);
	pckg.addToJsonObject(reqDoc, reqPckgObj);
	ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, reqPckgObj);
	ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName, editorName);

	this->getBasicServiceInformation().addToJsonObject(reqDoc, reqDoc);

	m_uiComponent->sendMessage(true, reqDoc);

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string Application::handleGraphicsSelectionChanged(OT_rJSON_doc& _document) {
	std::string editorName = ot::rJSON::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);
	auto sel = ot::rJSON::getULongLongList(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemIds);

	OT_LOG_D("Graphics Editor selection changed ( editor = \"" + editorName + "\" )");

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string Application::handleGraphicsItemMoved(OT_rJSON_doc& _document) {
	OT_rJSON_checkMember(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition, Object);

	std::string editorName = ot::rJSON::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);
	std::string itemUid = ot::rJSON::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemId);

	ot::Point2DD itmPos;
	OT_rJSON_val itemPosObj = _document[OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition].GetObject();
	itmPos.setFromJsonObject(itemPosObj);

	OT_LOG_D("Graphics Editor item moved ( editor = \"" + editorName + "\"; uid = \"" + itemUid + "\"; x = " + std::to_string(itmPos.x()) + "; y = " + std::to_string(itmPos.y()) + " )");

	return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
}

std::string Application::createEmptyTestEditor(void) {
	if (m_uiComponent) {
		/*ot::BlockEditorConfigurationPackage pckg("AlwaysNumberOne", "Block Editor");
		pckg.setTopLevelBlockCategories(createTestCategories());

		if (!ot::BlockConfigurationAPI::createEmptyBlockEditor(this, m_uiComponent->serviceURL(), pckg)) {
			m_uiComponent->displayMessage("Failed to create empty block editor");
			return OT_ACTION_RETURN_INDICATOR_Error "Failed to create empty block editor";
		}*/

		ot::GraphicsNewEditorPackage pckg("TestPackage", "Test title");
		ot::GraphicsCollectionCfg* a = new ot::GraphicsCollectionCfg("A", "A");
		ot::GraphicsCollectionCfg* a1 = new ot::GraphicsCollectionCfg("1", "1");
		ot::GraphicsCollectionCfg* a2 = new ot::GraphicsCollectionCfg("2", "2");
		a->addChildCollection(a1);
		a1->addItem(ottest::createTestBlock1(EXAMPLE_NAME_Block1));
		a1->addItem(ottest::createTestBlock2(EXAMPLE_NAME_Block2));
		a->addChildCollection(a2);
		//a2->addItem(ottest::createTestBlock3(EXAMPLE_NAME_Block3));
		//a2->addItem(ottest::createTestItem1(EXAMPLE_NAME_Block4));
		a2->addItem(ottest::createTestItem2(EXAMPLE_NAME_Block5));
		pckg.addCollection(a);

		OT_rJSON_createDOC(doc);
		OT_rJSON_createValueObject(pckgObj);
		pckg.addToJsonObject(doc, pckgObj);

		ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor);
		ot::rJSON::add(doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);
		this->getBasicServiceInformation().addToJsonObject(doc, doc);

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