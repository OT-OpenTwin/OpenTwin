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

Application* g_instance{ nullptr };






// -----
#include "OTGui/TableCfg.h"

std::string Application::test(void) {
	OT_LOG_E("Test error log");
	OT_LOG_W("Test warning log");

	ot::JsonDocument doc;
	ot::BasicServiceInformation info = getBasicServiceInformation();
	info.addToJsonObject(doc, doc.GetAllocator());

	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_TABLE_Setup, doc.GetAllocator()), doc.GetAllocator());
	
	int rows = 2000;
	int cols = 1000;
	ot::TableCfg cfg(rows, cols);
	cfg.setEntityName("Tester");

	for (int c = 0; c < cols; c++) {
		cfg.setColumnHeader(c, "Title " + std::to_string(c + 1));
	}

	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			cfg.setCellText(r, c, "Cell (r = " + std::to_string(r) + "; c = " + std::to_string(c) + ")");
		}
	}

	ot::JsonObject cfgObj;
	cfg.addToJsonObject(cfgObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, cfgObj, doc.GetAllocator());

	std::string tmp;
	m_uiComponent->sendMessage(true, doc, tmp);
	return "";
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

std::string Application::handleExecuteModelAction(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	if (action == "Test:Test:Test") return test();
	else { 
		OT_LOG_W("Unknown model action");
		assert(0); }// Unhandled button action
	return std::string();
}

std::string Application::handleEditorSaveRequested(ot::JsonDocument& _document) {
	// Get the editor name and the current text
	std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Name);
	std::string editorText = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Text);

	OT_LOG_D("Save request for text editor \"" + editorName + "\" with text:\n" + editorText);

	// ...

	// Create response document
	ot::JsonDocument doc;
	
	// Add the "SetSaved" action to notify the editor that the changes have been saved
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_TEXTEDITOR_SetSaved, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_TEXTEDITOR_Name, ot::JsonString(editorName, doc.GetAllocator()), doc.GetAllocator());

	// Send the request
	std::string response;
	std::string req = doc.toJson();
	if (!ot::msg::send("", m_uiComponent->getServiceURL(), ot::QUEUE, req, response)) {
		return OT_ACTION_RETURN_VALUE_FAILED;
	}

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::handleOnePropertyDialogValue(ot::JsonDocument& _document) {
	// Get the dialog name that was set when requesting the dialog (DialogCfg::setName())
	std::string dialogName = ot::json::getString(_document, OT_ACTION_PARAM_ObjectName);

	// Check the dialog name
	if (dialogName == "MyIntegerDialog") {

		// Get the value, in this example the value type is integer
		int value = ot::json::getInt(_document, OT_ACTION_PARAM_Value);

		m_uiComponent->displayMessage("Dialog test resulted with value: " + std::to_string(value) + "\n");
	}
	else if (dialogName == "MyDoubleDialog") {

		// Get the value, in this example the value type is integer
		double value = ot::json::getDouble(_document, OT_ACTION_PARAM_Value);

		m_uiComponent->displayMessage("Dialog test resulted with value: " + std::to_string(value) + "\n");
	}
	else if (dialogName == "MyBoolDialog") {

		// Get the value, in this example the value type is integer
		bool value = ot::json::getBool(_document, OT_ACTION_PARAM_Value);

		m_uiComponent->displayMessage(std::string("Dialog test resulted with value: ") + (value ? "True\n" : "False\n"));
	}
	else if (dialogName == "MyStringDialog") {

		// Get the value, in this example the value type is integer
		std::string value = ot::json::getString(_document, OT_ACTION_PARAM_Value);

		m_uiComponent->displayMessage(std::string("Dialog test resulted with value: ") + value + "\n");
	}
	else if (dialogName == "MyStringListDialog") {

		// Get the value, in this example the value type is integer
		std::string value = ot::json::getString(_document, OT_ACTION_PARAM_Value);

		m_uiComponent->displayMessage(std::string("Dialog test resulted with value: ") + value + "\n");
	}
	else if (dialogName == "MyColorDialog") {

		// Get the value, in this example the value type is integer
		ot::ConstJsonObject value = ot::json::getObject(_document, OT_ACTION_PARAM_Value);
		ot::Color c;
		c.setFromJsonObject(value);
		m_uiComponent->displayMessage(std::string("Dialog test resulted with value: ") + std::to_string(c.r()) + ", " + std::to_string(c.g()) + ", " + std::to_string(c.b()) + ", " + std::to_string(c.a()) + "\n");
	}
	else if (dialogName == "MyFileDialog") {

		// Get the value, in this example the value type is integer
		std::string value = ot::json::getString(_document, OT_ACTION_PARAM_Value);

		m_uiComponent->displayMessage(std::string("Dialog test resulted with value: ") + value + "\n");
	}
	else if (dialogName == "MyDirDialog") {

		// Get the value, in this example the value type is integer
		std::string value = ot::json::getString(_document, OT_ACTION_PARAM_Value);

		m_uiComponent->displayMessage(std::string("Dialog test resulted with value: ") + value + "\n");
	}
	return std::string();
}

std::string Application::handleTableSaved(ot::JsonDocument& _document) {
	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);
	ot::TableCfg cfg;
	cfg.setFromJsonObject(cfgObj);

	ot::JsonDocument responseDoc;
	responseDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_TABLE_SetSaved, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_NAME, ot::JsonString(cfg.getEntityName(), responseDoc.GetAllocator()), responseDoc.GetAllocator());

	std::string tmp;
	m_uiComponent->sendMessage(true, responseDoc, tmp);
	return "";
}

// ##################################################################################################################################################################################################################

// Required functions

void Application::run(void)
{
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

	_ui->addMenuPage("Test");
	_ui->addMenuGroup("Test", "Test");
	_ui->addMenuButton("Test", "Test", "Test", "Test", ot::LockModelWrite | ot::LockViewRead | ot::LockViewWrite, "Execute");

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

ot::PropertyGridCfg Application::createSettings(void) const {
	return ot::PropertyGridCfg();
}

void Application::settingsSynchronized(const ot::PropertyGridCfg& _dataset) {

}

bool Application::settingChanged(const ot::Property * _item) {
	return false;
}

// ##################################################################################################################################################################################################################
