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
	if (action == "Block Editor:Test:Empty") return createEmptyTestEditor();
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

	// Send the request
	std::string response;
	std::string req = doc.toJson();
	if (!ot::msg::send("", m_uiComponent->serviceURL(), ot::QUEUE, req, response)) {
		return OT_ACTION_RETURN_VALUE_FAILED;
	}

	return OT_ACTION_RETURN_VALUE_OK;
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