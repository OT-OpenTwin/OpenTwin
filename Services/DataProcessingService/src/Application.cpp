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


#include "BlockPickerManager.h"

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
void Application::run(void)
{
	if (!EnsureDataBaseConnection())
	{
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();
	}
	// Add code that should be executed when the service is started and may start its work
}

#include "EntityBlockDatabaseAccess.h"
#include "ClassFactory.h"
#include "BlockHandlerDatabaseAccess.h"

std::string Application::processAction(const std::string & _action, OT_rJSON_doc & _doc)
{
	if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
	{
		std::string action = ot::rJSON::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
		if (action == _buttonRunPipeline.GetFullDescription())
		{
			std::unique_ptr<EntityBlockDatabaseAccess> block(new EntityBlockDatabaseAccess(m_modelComponent->createEntityUID(),nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService));
			block->setName("Processing Blocks/Database Access");
			ot::Connector co0(ot::ConnectorType::Source, "C0");
			ot::Connector co1(ot::ConnectorType::Filter, "C1");
			block->AddConnector(co0);
			block->AddConnector(co1);

			ot::BlockConnection bo0("Hans.C0", "Peter.C0");
			ot::BlockConnection bo1("Hans.C0", "GustavC0");
			block->AddOutgoingConnection(bo0);
			block->AddOutgoingConnection(bo1);

			std::list<std::string>projects{"Test"};
			block->createProperties(projects, *projects.begin());
			
			BlockHandlerDatabaseAccess handlerDatabaseAccess(block.get());
			BlockHandler::genericDataBlock parameter;
			auto result = handlerDatabaseAccess.Execute(parameter);
			block->StoreToDataBase();

			ot::UIDList entID{ block->getEntityID() }, entVers{block->getEntityStorageVersion()}, dataEnt;
			std::list<bool> forceVis{ false };
			m_modelComponent->addEntitiesToModel(entID, entVers, forceVis, dataEnt, dataEnt, dataEnt, "Added a block entity");

			ClassFactory classFactory;
			auto tempBase = m_modelComponent->readEntityFromEntityIDandVersion(block->getEntityID(), block->getEntityStorageVersion(), classFactory);
			auto temp = dynamic_cast<EntityBlockDatabaseAccess*>(tempBase);
		}
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

	BlockPickerManager blockPickerManger;
	blockPickerManger.OrderUIToCreateBlockPicker();
	
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