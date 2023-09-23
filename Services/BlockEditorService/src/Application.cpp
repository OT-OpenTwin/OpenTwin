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

#define EXAMPLE_NAME_Block1 "Alpha 1"
#define EXAMPLE_NAME_Block2 "Beta 2"
#define EXAMPLE_NAME_Block3 "Gamma 3"

namespace ottest {
	static unsigned long long currentBlockUid = 0;


	ot::GraphicsItemCfg* createTestBlock1(const std::string& _name) {
		ot::GraphicsFlowItemCfg flow;
		flow.setTitleBackgroundColor(0, 255, 0);
		//flow->setBackgroundImagePath("Default/python");

		flow.addLeft("SomeIn1", "Run", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Blue);
		flow.addLeft("SomeIn2", "Test", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Orange);
		flow.addLeft("SomeIn3", "Perform shutdown", ot::GraphicsFlowConnectorCfg::Circle, ot::Color::IndianRed);
		flow.addRight("SomeOut1", "Success", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Blue);
		flow.addRight("SomeOut2", "Failed", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Yellow);
		flow.addRight("SomeOut3", "Error", ot::GraphicsFlowConnectorCfg::Circle, ot::Color::Red);

		return flow.createGraphicsItem(_name, _name);
	}

	ot::GraphicsItemCfg* createTestBlock2(const std::string& _name) {
		ot::GraphicsFlowItemCfg flow;
		flow.setTitleBackgroundColor(255, 0, 0);
		//flow->setBackgroundImagePath("Default/python");

		flow.addLeft("SomeIn1", "Run", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Blue);
		flow.addLeft("SomeIn2", "Test", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Orange);
		flow.addLeft("SomeIn3", "Perform shutdown", ot::GraphicsFlowConnectorCfg::Circle, ot::Color::IndianRed);
		flow.addRight("SomeOut1", "Success", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Blue);
		flow.addRight("SomeOut2", "Failed", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Yellow);

		return flow.createGraphicsItem(_name, _name);
	}

	ot::GraphicsItemCfg* createTestBlock3(const std::string& _name) {
		ot::GraphicsFlowItemCfg flow;
		flow.setTitleBackgroundColor(0, 0, 255);
		//flow->setBackgroundImagePath("Default/python");

		flow.addLeft("SomeIn1", "Run", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Blue);
		flow.addRight("SomeOut1", "Success", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Blue);
		flow.addRight("SomeOut2", "Failed", ot::GraphicsFlowConnectorCfg::Square, ot::Color::Yellow);

		return flow.createGraphicsItem(_name, _name);
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

	ot::GlobalOwner::instance().addToJsonObject(reqDoc, reqDoc);

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

	ot::GlobalOwner::instance().addToJsonObject(reqDoc, reqDoc);
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

	ot::GlobalOwner::instance().addToJsonObject(reqDoc, reqDoc);
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
		a2->addItem(ottest::createTestBlock3(EXAMPLE_NAME_Block3));
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