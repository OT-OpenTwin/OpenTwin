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
#include "OpenTwinCore/ReturnMessage.h"
#include "OpenTwinCore/OwnerServiceGlobal.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OpenTwinFoundation/UiComponent.h"
#include "OpenTwinFoundation/ModelComponent.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCommunication/Msg.h"
#include "TemplateDefaultManager.h"

#include "ClassFactory.h"
#include "ExternalDependencies.h"

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
#include "EntityCoordinates2D.h"
#include "DataBase.h"
#include "CrossCollectionAccess.h"
#include "MeasurementCampaignFactory.h"

std::string Application::processAction(const std::string & _action, OT_rJSON_doc & _doc)
{
	if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
	{
		std::string action = ot::rJSON::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
		if (action == _buttonRunPipeline.GetFullDescription())
		{
			_pipelineManager.RunAll();
		}
	}
	else if (_action == OT_ACTION_CMD_MODEL_PropertyChanged)
	{
		assert(m_selectedEntities.size() == 1);
		std::list<ot::EntityInformation> entityInfos;
		m_modelComponent->getEntityInformation(m_selectedEntities, entityInfos);
		ClassFactory classFactory;
		auto entBase =	m_modelComponent->readEntityFromEntityIDandVersion(entityInfos.begin()->getID(), entityInfos.begin()->getVersion(), classFactory);
		auto dbAccess =	std::shared_ptr<EntityBlockDatabaseAccess>(dynamic_cast<EntityBlockDatabaseAccess*>(entBase));

		if (dbAccess != nullptr)
		{
			auto modelService = instance()->getConnectedServiceByName(OT_INFO_SERVICE_TYPE_MODEL);
			PropertyHandlerDatabaseAccessBlock::instance().PerformUpdateIfRequired(dbAccess, instance()->sessionServiceURL(), modelService->serviceURL());
		}
	}
	else if (_action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem)
	{
		std::string itemName = ot::rJSON::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName);
		std::string editorName = ot::rJSON::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);	
		
		OT_rJSON_val posObj = _doc[OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition].GetObject();
		ot::Point2DD position;
		position.setFromJsonObject(posObj);
		
		//Needs to be set once but modelConnect event cannot be used currently, since the modelstate in that point in time is a dummy.
		ExternalDependencies dependencies;
		if (dependencies.getPythonScriptFolderID() == 0)
		{
			ot::EntityInformation entityInfo;
			m_modelComponent->getEntityInformation("Scripts", entityInfo);
			ExternalDependencies dependencies;
			dependencies.setPythonScriptFolderID(entityInfo.getID());
		}

		_blockEntityHandler.CreateBlockEntity(editorName, itemName,position);
		//BlockItemManager blockItemManager;
		//OT_rJSON_doc reqDoc = blockItemManager.CreateBlockItem(itemName, blockID, position);

		//ot::GraphicsScenePackage pckg("Data Processing");

		//
		//ot::GraphicsItemCfg* itm = blockItemManager.GetBlockConfiguration(itemName);
		//itm->setPosition(position);
		//itm->setUid(std::to_string(blockID));
		//pckg.addItem(itm);

		//OT_rJSON_createDOC(reqDoc);
		//ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem);

		//OT_rJSON_createValueObject(pckgDoc);
		//pckg.addToJsonObject(reqDoc, pckgDoc);
		//ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgDoc);

		//ot::OwnerServiceGlobal::instance().addToJsonObject(reqDoc, reqDoc);
		//m_uiComponent->sendMessage(true, reqDoc);

	}
	else if (_action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection)
	{
				
		OT_rJSON_checkMember(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, Object);
		OT_rJSON_val pckgObj = _doc[OT_ACTION_PARAM_GRAPHICSEDITOR_Package].GetObject();
		
		ot::GraphicsConnectionPackage pckg;
		pckg.setFromJsonObject(pckgObj);

		// Store connection information
		_blockEntityHandler.AddBlockConnection(pckg.connections());
		
		// Request UI to add connections
		//OT_rJSON_createDOC(reqDoc);
		//ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection);

		//OT_rJSON_createValueObject(reqPckgObj);
		//pckg.addToJsonObject(reqDoc, reqPckgObj);
		//ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, reqPckgObj);
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
	_blockEntityHandler.setUIComponent(_ui);
	_blockEntityHandler.OrderUIToCreateBlockPicker();
	
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);

	PropertyHandlerDatabaseAccessBlock::instance().setUIComponent(_ui);
}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::uiPluginConnected(ot::components::UiPluginComponent * _uiPlugin) {

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{
	_blockEntityHandler.setModelComponent(_model);
	_pipelineManager.setModelComponent(_model);
	PropertyHandlerDatabaseAccessBlock::instance().setModelComponent(_model);
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