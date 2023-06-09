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
#include "OpenTwinFoundation/UiComponent.h"
#include "OpenTwinFoundation/ModelComponent.h"
#include "OTBlockEditorAPI/BlockCategoryConfiguration.h"
#include "OTBlockEditorAPI/BlockLayers.h"
#include "OTBlockEditorAPI/BlockConfiguration.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/LinearGradientPainter2D.h"

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

void Application::editorClosed(const std::string& _editorName) {

}

std::string Application::handleExecuteModelAction(OT_rJSON_doc& _document) {
	std::string action = ot::rJSON::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	if (action == "Block Editor:Test:Empty") return createEmptyTestEditor();
	else assert(0); // Unhandled button action
	return std::string();
}

ot::BlockConfiguration* createTestBlockConfig(const ot::Color& _color) {
	// Create a custom block
	ot::BlockConfiguration* block = new ot::BlockConfiguration("Test", "Test");

	// ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###

	// Define block size limitations (min, max)
	block->setWidthLimits(ot::LengthLimitation(80, -1));
	block->setHeightLimits(ot::LengthLimitation(40, -1));

	// Allow the user to use the block
	block->setIsUserMoveable(true);

	// ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###

	// Create a background layer with a rectangcular shape and rounded border
	ot::RectangleBlockLayerConfiguration* bgLayer = new ot::RectangleBlockLayerConfiguration(new ot::FillPainter2D(ot::Color(0, 128, 255)), ot::Color(0, 0, 0), 2, 50);
	block->addLayer(bgLayer);

	// ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###

	// Create text layer
	ot::TextBlockLayerConfiguration* layer0 = new ot::TextBlockLayerConfiguration("Hello World!", ot::Color(255, 0, 0));
	layer0->setMargins(10., 5., 5., 5.);

	// Setup font for text layer
	ot::Font f = layer0->textFont();
	f.setSize(8);
	layer0->setTextFont(f);

	// Add text layer
	block->addLayer(layer0);

	// ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###

	return block;
}

std::list<ot::BlockCategoryConfiguration*> createTestCategories() {
	ot::BlockCategoryConfiguration* root1 = new ot::BlockCategoryConfiguration("r1", "Root 1");
	ot::BlockCategoryConfiguration* r1A = new ot::BlockCategoryConfiguration("A", "A");
	ot::BlockCategoryConfiguration* r1B = new ot::BlockCategoryConfiguration("B", "B");
	root1->addChild(r1A);
	r1A->addItem(createTestBlockConfig(ot::Color(255, 128, 0)));
	root1->addChild(r1B);
	r1B->addItem(createTestBlockConfig(ot::Color(255, 0, 128)));

	ot::BlockCategoryConfiguration* root2 = new ot::BlockCategoryConfiguration("r2", "Root 2");
	ot::BlockCategoryConfiguration* r2C = new ot::BlockCategoryConfiguration("C", "C");
	root2->addChild(r2C);
	r2C->addItem(createTestBlockConfig(ot::Color(128, 255, 0)));
	r2C->addItem(createTestBlockConfig(ot::Color(0, 255, 128)));

	ot::BlockCategoryConfiguration* root3 = new ot::BlockCategoryConfiguration("r3", "Root 3");
	ot::BlockCategoryConfiguration* r3D = new ot::BlockCategoryConfiguration("D", "D");
	root3->addChild(r3D);
	r3D->addItem(createTestBlockConfig(ot::Color(128, 0, 255)));
	r3D->addItem(createTestBlockConfig(ot::Color(0, 128, 255)));

	std::list<ot::BlockCategoryConfiguration*> rootItems;
	rootItems.push_back(root1);
	rootItems.push_back(root2);
	rootItems.push_back(root3);

	return rootItems;
}

std::string Application::createEmptyTestEditor(void) {
	if (m_uiComponent) {
		ot::BlockEditorConfigurationPackage pckg("AlwaysNumberOne", "Block Editor");
		pckg.setTopLevelBlockCategories(createTestCategories());

		if (!ot::BlockConfigurationAPI::createEmptyBlockEditor(this, m_uiComponent->serviceURL(), pckg)) {
			m_uiComponent->displayMessage("Failed to create empty block editor");
			return OT_ACTION_RETURN_INDICATOR_Error "Failed to create empty block editor";
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