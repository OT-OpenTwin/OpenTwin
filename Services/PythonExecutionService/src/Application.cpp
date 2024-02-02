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
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include <vector>
#include <string>
#include "OTCore/Variable.h"
#include "OTCore/TypeNames.h"
#include "TemplateDefaultManager.h"

#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include "DataBase.h"
#include "OTCore/ReturnMessage.h"

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
	: ot::ApplicationBase(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE, OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE, new UiNotifier(), new ModelNotifier())
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
	if (EnsureDataBaseConnection())
	{
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();
	}
	if (_subprocessHandler == nullptr)
	{
		_subprocessHandler = new SubprocessHandler(sessionID());
	}
	const std::string dbURL = DataBase::GetDataBase()->getDataBaseServerURL();
	const std::string userName= DataBase::GetDataBase()->getUserName();
	const std::string psw = DataBase::GetDataBase()->getUserPassword();
	const std::string siteID = this->siteID();
	const std::string collectionName = this->m_collectionName;
	const int sessionID = Application::instance()->getSessionCount();
	const int serviceID = Application::instance()->getServiceIDAsInt();
	_subprocessHandler->setDatabase(dbURL, userName, psw,collectionName,siteID,sessionID,serviceID);
}


std::string Application::processAction(const std::string & _action, ot::JsonDocument& _doc)
{
	try
	{
		std::string returnMessage = "";
		if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
		{			
			std::string action = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
			OT_LOG_D("Executing action: " + action);
			if (action == OT_ACTION_CMD_PYTHON_EXECUTE_Scripts)
			{
				if (_doc.HasMember(OT_ACTION_CMD_PYTHON_Scripts) && _doc.HasMember(OT_ACTION_CMD_PYTHON_Parameter))
				{
					if (_doc[OT_ACTION_CMD_PYTHON_Scripts].IsArray() && _doc[OT_ACTION_CMD_PYTHON_Parameter].IsArray())
					{
						ot::JsonDocument subprocessDoc;
						auto& scripts = _doc[OT_ACTION_CMD_PYTHON_Scripts];
						subprocessDoc.AddMember(OT_ACTION_CMD_PYTHON_Scripts, scripts, subprocessDoc.GetAllocator());
						auto& parameter = _doc[OT_ACTION_CMD_PYTHON_Parameter];
						subprocessDoc.AddMember(OT_ACTION_CMD_PYTHON_Parameter, parameter,subprocessDoc.GetAllocator());
						
						if (_doc.HasMember(OT_ACTION_CMD_PYTHON_Portdata_Names) && _doc[OT_ACTION_CMD_PYTHON_Portdata_Names].IsArray())
						{
							assert(_doc.HasMember(OT_ACTION_CMD_PYTHON_Portdata_Data));
							auto& portData = _doc[OT_ACTION_CMD_PYTHON_Portdata_Data];
							subprocessDoc.AddMember(OT_ACTION_CMD_PYTHON_Portdata_Data, portData, subprocessDoc.GetAllocator());
							auto& portNames = _doc[OT_ACTION_CMD_PYTHON_Portdata_Names];

							subprocessDoc.AddMember(OT_ACTION_CMD_PYTHON_Portdata_Names, portNames,subprocessDoc.GetAllocator());
						}
						subprocessDoc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_PYTHON_EXECUTE_Scripts, subprocessDoc.GetAllocator()), subprocessDoc.GetAllocator());
						ot::ReturnMessage msg = _subprocessHandler->Send(subprocessDoc.toJson());
						return msg.toJson();
					}
					else
					{
						ot::ReturnMessage msg(ot::ReturnMessage::Failed, "Scripts and parameter are not of expected type.");
						return msg.toJson();
					}
				}
										
			}
		}
		return returnMessage;
	}
	catch (std::runtime_error& e)
	{
		std::string errorMessage = "Failed to execute action " + _action + " due to runtime error: " + e.what();
		m_uiComponent->displayMessage(errorMessage);
		return errorMessage;
	}
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument& _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	if (_subprocessHandler == nullptr)
	{
		_subprocessHandler = new SubprocessHandler(sessionID());
	}
	_subprocessHandler->setUIComponent(_ui);
}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::uiPluginConnected(ot::components::UiPluginComponent * _uiPlugin) {

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{
	if (_subprocessHandler == nullptr)
	{
		_subprocessHandler = new SubprocessHandler(sessionID());
	}
	_subprocessHandler->setModelComponent(_model);
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