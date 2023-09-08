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
#include <OpenTwinCommunication/ActionTypes.h>	
#include "OpenTwinCommunication/Msg.h"
#include "TemplateDefaultManager.h"


#include "BlockItemManager.h"
#include "ClassFactory.h"

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
#include "TemplateDefaultManager.h"
#include "ResultCollectionHandler.h"

void Application::run(void)
{
	if (!EnsureDataBaseConnection())
	{
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();
	}

	// Add code that should be executed when the service is started and may start its work
}

#include "BlockEntityHandler.h"
#include "BlockHandlerDatabaseAccess.h"
#include "BlockHandlerPlot1D.h"
#include "MeasurementCampaignHandler.h"

std::string Application::processAction(const std::string & _action, OT_rJSON_doc & _doc)
{
	if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
	{
		std::string action = ot::rJSON::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
		if (action == _buttonRunPipeline.GetFullDescription())
		{

			_pipelineManager.RunAll();
			//std::unique_ptr<EntityBlockDatabaseAccess> block(new EntityBlockDatabaseAccess(m_modelComponent->createEntityUID(),nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService));
			//std::list<std::string>projects{"KWT_Demo"};
			//block->createProperties(projects, *projects.begin());
			//
			//BlockHandlerDatabaseAccess dbAccess(block.get());
			//BlockHandler::genericDataBlock parameter, result;
			//result = dbAccess.Execute(parameter);

			//std::unique_ptr<EntityBlockDatabaseAccess> block(new EntityBlockDatabaseAccess(m_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService));
			//BlockHandlerPlot1D plot;
			//plot.Execute(result);
		}
	}
	else if (_action == OT_ACTION_CMD_MODEL_PropertyChanged)
	{
		assert(m_selectedEntities.size() == 1);
		std::list<ot::EntityInformation> entityInfos;
		m_modelComponent->getEntityInformation(m_selectedEntities, entityInfos);
		ClassFactory classFactory;
		auto entBase =	m_modelComponent->readEntityFromEntityIDandVersion(entityInfos.begin()->getID(), entityInfos.begin()->getVersion(), classFactory);
		auto dbAccess =	dynamic_cast<EntityBlockDatabaseAccess*>(entBase);
		if (dbAccess != nullptr)
		{
			const std::string queryDimension = dbAccess->getQueryDimension();
			const std::string projectName = dbAccess->getSelectedProjectName();
			ResultCollectionHandler resultCollection;
			const std::string collectionName = resultCollection.getProjectCollection(projectName);

			MeasurementCampaignHandler mcHandler;
			mcHandler.ConnectToCollection(collectionName, projectName);
		}
	}
	else if (_action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemDropped)
	{
		std::string itemName = ot::rJSON::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName);
		ot::UID itemID = ot::rJSON::getULongLong(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemId);
		std::string editorName = ot::rJSON::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);	
		
		auto blockEntity = _blockEntHandler.CreateBlock(editorName, itemName, itemID);
		if (blockEntity != nullptr)
		{
			ot::UIDList topoEntID{ blockEntity->getEntityID() }, topoEntVers{ blockEntity->getEntityStorageVersion() }, dataEnt{};
			std::list<bool> forceVis{false,false};
			m_modelComponent->addEntitiesToModel(topoEntID, topoEntVers, forceVis, dataEnt, dataEnt, dataEnt, "Added Block: " + itemName);
		}

	}
	else if (_action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_ConnectionDropped)
	{
		ot::UID uidOrigin = ot::rJSON::getULongLong(_doc,OT_ACTION_PARAM_GRAPHICSEDITOR_OriginId);
		ot::UID uidDestination = ot::rJSON::getULongLong(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_DestId);
		std::string connectorNameOrigin = ot::rJSON::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_OriginConnetableName);
		std::string connectorNameDestination = ot::rJSON::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_DestConnetableName);

		_blockEntHandler.AddBlockConnection(uidOrigin, uidDestination, connectorNameOrigin, connectorNameDestination);
	}

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

	BlockItemManager blockItemManger;
	blockItemManger.OrderUIToCreateBlockPicker();
	
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::uiPluginConnected(ot::components::UiPluginComponent * _uiPlugin) {

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{
	_blockEntHandler.setModelComponent(_model);
	_pipelineManager.setModelComponent(_model);
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