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

#include "OpenTwinCommunication/Msg.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCore/Owner.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/LinearGradientPainter2D.h"
#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsEditorPackage.h"
#include "OTGui/GraphicsLayoutItemCfg.h"

// Open twin header
#include "OpenTwinFoundation/UiComponent.h"
#include "OpenTwinFoundation/ModelComponent.h"
#include <OpenTwinCommunication/ActionTypes.h>	

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
	: ot::ApplicationBase(OT_INFO_SERVICE_TYPE_DataProcessingService, OT_INFO_SERVICE_TYPE_DataProcessingService, new UiNotifier(), new ModelNotifier())
{
	
}

Application::~Application()
{

}

// ##################################################################################################################################################################################################################

// Custom functions



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

	const std::string pageName = "Data Processing";
	const std::string groupName = "Pipeline Handling";
	ot::Flags<ot::ui::lockType> modelWrite;
	modelWrite.setFlag(ot::ui::lockType::tlModelWrite);

	_ui->addMenuPage(pageName);
	_ui->addMenuGroup(pageName, groupName);
	_buttonRunPipeline.SetDescription(pageName, groupName, "Run");
	_ui->addMenuButton(_buttonRunPipeline, modelWrite, "Kriging");

	ot::GraphicsEditorPackage pckg("TestPackage", "Test title");
	ot::GraphicsCollectionCfg* controlBlockCollection = new ot::GraphicsCollectionCfg("Control Blocks", "Control Blocks");
	ot::GraphicsCollectionCfg* controlBlockDatabaseCollection = new ot::GraphicsCollectionCfg("Database", "Database");
	ot::GraphicsCollectionCfg* controlBlockVisualizationCollection = new ot::GraphicsCollectionCfg("Visualization", "Visualization");
	
	ot::GraphicsCollectionCfg* mathBlockCollection = new ot::GraphicsCollectionCfg("Mathematical Operations", "Mathematical Operations");
	ot::GraphicsCollectionCfg* customizedBlockCollection = new ot::GraphicsCollectionCfg("Customized Blocks", "Customized Blocks");
	

	controlBlockCollection->addChildCollection(controlBlockDatabaseCollection);
	controlBlockCollection->addChildCollection(controlBlockVisualizationCollection);

	auto black = ot::Color(255, 255, 255);
	auto blockSize = ot::Size2D(100, 60);
	/*ot::GraphicsGridLayoutItemCfg* layout = new ot::GraphicsGridLayoutItemCfg(2, 2);
	layout->setBorder(ot::Border(black, 2));
	layout->setSize();*/

	ot::GraphicsVBoxLayoutItemCfg* centralLayout = new ot::GraphicsVBoxLayoutItemCfg();
	centralLayout->setSize(blockSize);
	centralLayout->setBorder(ot::Border(black, 2));
	ot::GraphicsHBoxLayoutItemCfg* topLine= new ot::GraphicsHBoxLayoutItemCfg();
	topLine->setBorder(ot::Border(black, 2));

	ot::GraphicsTextItemCfg* label = new ot::GraphicsTextItemCfg("C", black);
	label->setName("Label");
	topLine->addChildItem(label);
	ot::GraphicsTextItemCfg* title = new ot::GraphicsTextItemCfg("Database Handle", black);
	title->setName("Title");
	topLine->addStrech();
	topLine->addChildItem(title);
	centralLayout->addChildItem(topLine);

	//ot::GraphicsImageItemCfg* image = new ot::GraphicsImageItemCfg("C:\\OpenTwin\\Deployment\\icons\\Default\\database.png");
	//centralLayout->addChildItem(image);

	controlBlockDatabaseCollection->addItem(centralLayout);


	//a2->addItem(createTestBlock2("Alpha 3"));
	pckg.addCollection(controlBlockCollection);
	pckg.addCollection(customizedBlockCollection);
	pckg.addCollection(mathBlockCollection);

	OT_rJSON_createDOC(doc);
	OT_rJSON_createValueObject(pckgObj);
	pckg.addToJsonObject(doc, pckgObj);

	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateEmptyGraphicsEditor);
	ot::rJSON::add(doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);
	ot::GlobalOwner::instance().addToJsonObject(doc, doc);

	std::string response;
	std::string req = ot::rJSON::toJSON(doc);

	if (!ot::msg::send("", m_uiComponent->serviceURL(), ot::QUEUE, req, response)) {
		assert(0);
	}

	if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Invalid response from UI");
		m_uiComponent->displayDebugMessage("Invalid response\n");
	}


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