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
#include <vector>
#include <string>
#include "OpenTwinCore/Variable.h"
#include "OpenTwinCore/TypeNames.h"
#include "TemplateDefaultManager.h"
#include "SubprocessDebugConfigurator.h"

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
	if(_subprocessHandler != nullptr)
	{
		delete _subprocessHandler;
		_subprocessHandler = nullptr;
	}
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
#ifdef _DEBUG
	SubprocessDebugConfigurator configurator;
	auto modelService =	m_serviceNameMap[OT_INFO_SERVICE_TYPE_MODEL];
	std::string urlModelservice	= modelService.service->serviceURL();
	int startPort = SubprocessHandler::getStartPort();
	std::string subserviceURL =  m_serviceURL.substr(0, m_serviceURL.find(":")+1) + std::to_string(startPort);
	configurator.CreateConfiguration(m_serviceURL, subserviceURL, urlModelservice, m_databaseURL, m_serviceID,m_sessionID);
	_subprocessHandler = new SubprocessHandler(m_serviceURL);
	_subprocessHandler->setSubprocessURL(subserviceURL);
#else
	_subprocessHandler = new SubprocessHandler(m_serviceURL);
	std::thread workerThread(&SubprocessHandler::Create,_subprocessHandler, m_serviceURL);
	workerThread.detach();
	
#endif // DEBUG


}

#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include "DataBase.h"
#include "OpenTwinCore/ReturnMessage.h"

std::string Application::processAction(const std::string & _action, OT_rJSON_doc & _doc)
{
	try
	{
		std::string returnMessage = "";
		if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
		{
			
			std::string action = ot::rJSON::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
			if (action == OT_ACTION_CMD_PYTHON_EXECUTE)
			{
				if (ot::rJSON::memberExists(_doc, OT_ACTION_CMD_PYTHON_Scripts) && ot::rJSON::memberExists(_doc, OT_ACTION_CMD_PYTHON_Parameter))
				{
					if (_doc[OT_ACTION_CMD_PYTHON_Scripts].IsArray() && _doc[OT_ACTION_CMD_PYTHON_Parameter].IsArray())
					{

						OT_rJSON_createDOC(subprocessDoc);
						auto& scripts = _doc[OT_ACTION_CMD_PYTHON_Scripts];
						ot::rJSON::add(subprocessDoc, OT_ACTION_CMD_PYTHON_Scripts, scripts);
						auto& parameter = _doc[OT_ACTION_CMD_PYTHON_Parameter];
						ot::rJSON::add(subprocessDoc, OT_ACTION_CMD_PYTHON_Parameter, parameter);
						return _subprocessHandler->SendExecutionOrder(subprocessDoc);					
					}
					else
					{
						return ot::ReturnMessage(ot::ReturnStatus::Failed(),"Scripts and parameter are not of expected type.");
					}
				}
										
			}
			else if (action == OT_ACTION_CMD_PYTHON_Request_Initialization)
			{
				_subprocessHandler->setReceivedInitializationRequest();
				auto modelService = m_serviceNameMap[OT_INFO_SERVICE_TYPE_MODEL];
				std::string urlModelservice = modelService.service->serviceURL();
				std::string userName = DataBase::GetDataBase()->getUserName();
				std::string pwd = DataBase::GetDataBase()->getUserPassword();


				OT_rJSON_createDOC(message);
				ot::rJSON::add(message, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_ExecuteAction);
				ot::rJSON::add(message, OT_ACTION_PARAM_MODEL_ActionName, OT_ACTION_CMD_PYTHON_Initialization);
				ot::rJSON::add(message, "ModelService.URL", urlModelservice);
				ot::rJSON::add(message, "Service.ID", m_serviceID);
				ot::rJSON::add(message, "Session.ID", m_sessionID);
				ot::rJSON::add(message, "DataBase.PWD", pwd);
				ot::rJSON::add(message, "DataBase.Username", userName);
				ot::rJSON::add(message, "DataBase.URL", m_databaseURL);
				returnMessage = ot::rJSON::toJSON(message);
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

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, OT_rJSON_doc & _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{

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