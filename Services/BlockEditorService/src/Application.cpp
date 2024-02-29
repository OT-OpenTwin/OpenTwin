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
#include "OTGui/PropertyDialogCfg.h"
#include "OTGui/OnePropertyDialogCfg.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyInt.h"
#include "OTGui/PropertyDouble.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyStringList.h"

std::string Application::test(void) {
	// Create json document
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_OnePropertyDialog, doc.GetAllocator()), doc.GetAllocator());
	
	// Create property to edit
	ot::PropertyInt* prop = new ot::PropertyInt;
	prop->setPropertyName("<name>");   // Is not relevant for the OnePropertyDialog
	prop->setPropertyTitle("<title>"); // Title that will be displayed to the user
	prop->setRange(1, 99);             // Value range for the user input
	prop->setValue(2);                 // Inital value

	// Create dialog configuration
	ot::OnePropertyDialogCfg cfg(prop);
	cfg.setName("MyIntegerDialog");       // Dialog name that may be used to identify the dialog (e.g. when the service wants to request different dialogs)
	cfg.setTitle("MyIntegerDialogTitle");            // Title that will be displayed to the user
	cfg.setFlags(ot::DialogCfg::CancelOnNoChange); // If set the dialog will behave as if the user pressed cancel when the user did not change any value before confirming

	// Serialize
	ot::JsonObject cfgObj;
	cfg.addToJsonObject(cfgObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, cfgObj, doc.GetAllocator());

	// Add service information
	this->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());
	// or use "Application::instance()->" instead of "this" when using outside of class Application
	
	// Send
	m_uiComponent->sendMessage(true, doc);
	
	//--- OR ---
	std::string response; // Response will be empty when calling queue
	if (!ot::msg::send("<sender url>", "<UI url>", ot::QUEUE, doc.toJson(), response)) {
		// Error handling
	}

	return "";
}
/*
std::string Application::test(void) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_PropertyDialog, doc.GetAllocator()), doc.GetAllocator());
	

	ot::PropertyDialogCfg cfg;
	cfg.setTitle("Testing");

	ot::PropertyBool* cb1 = new ot::PropertyBool("Enabled", false);
	cfg.defaultGroup()->addProperty(cb1);

	ot::PropertyGroup* g1 = new ot::PropertyGroup("G1", "Group 1");
	ot::PropertyInt* i1 = new ot::PropertyInt("Int", 1);
	i1->setMin(0);
	i1->setMax(99);
	g1->addProperty(i1);

	ot::PropertyDouble* d1 = new ot::PropertyDouble("Double", 1.1);
	d1->setMin(0.0);
	d1->setMax(1000.);
	d1->setPrecision(4);
	g1->addProperty(d1);

	ot::PropertyGroup* g2 = new ot::PropertyGroup("G2", "Group 2");
	ot::PropertyString* s1 = new ot::PropertyString("String", "Test");
	s1->setMaxLength(10);
	g2->addProperty(s1);

	ot::PropertyStringList* sl1 = new ot::PropertyStringList("StringList", "Test", { "Test", "Other", "Some" });
	g2->addProperty(sl1);

	cfg.addRootGroup(g1);
	cfg.addRootGroup(g2);

	ot::JsonObject cfgObj;
	cfg.addToJsonObject(cfgObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, cfgObj, doc.GetAllocator());
	m_uiComponent->sendMessage(true, doc);
	return "";
}
*/






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
	this->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

	// Add the "SetSaved" action to notify the editor that the changes have been saved
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_TEXTEDITOR_SetSaved, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_TEXTEDITOR_Name, ot::JsonString(editorName, doc.GetAllocator()), doc.GetAllocator());

	// Send the request
	std::string response;
	std::string req = doc.toJson();
	if (!ot::msg::send("", m_uiComponent->serviceURL(), ot::QUEUE, req, response)) {
		return OT_ACTION_RETURN_VALUE_FAILED;
	}

	return OT_ACTION_RETURN_VALUE_OK;
}

// OT_HANDLER(handleOnePropertyDialogValue, Application, OT_ACTION_CMD_UI_OnePropertyDialogValue, ot::SECURE_MESSAGE_TYPES);
std::string Application::handleOnePropertyDialogValue(ot::JsonDocument& _document) {
	// Get the dialog name that was set when requesting the dialog (DialogCfg::setName())
	std::string dialogName = ot::json::getString(_document, OT_ACTION_PARAM_ObjectName);

	// Check the dialog name
	if (dialogName == "MyIntegerDialog") {

		// Get the value, in this example the value type is integer
		int value = ot::json::getInt(_document, OT_ACTION_PARAM_Value);

		m_uiComponent->displayMessage("Dialog test resulted with value: " + std::to_string(value));
	}
	return std::string();
}

std::string Application::createEmptyTestEditor(void) {
	if (m_uiComponent) {
		// Create json document
		ot::JsonDocument doc;
		this->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

		// Add action and editor information
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_TEXTEDITOR_SetText, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_TEXTEDITOR_Name, ot::JsonString("MyEditorName", doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_TEXTEDITOR_Text, ot::JsonString("Hello\nWorld", doc.GetAllocator()), doc.GetAllocator());

		// If the title is not provided the name will be set as title
		doc.AddMember(OT_ACTION_PARAM_TEXTEDITOR_Title, ot::JsonString("New Text Editor", doc.GetAllocator()), doc.GetAllocator());

		// Send the request
		std::string response;
		std::string req = doc.toJson();
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
	_ui->addMenuButton("Test", "Test", "Test", "Test", ot::ui::lockType::tlModelWrite | ot::ui::tlViewRead | ot::ui::tlViewWrite, "Execute");

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