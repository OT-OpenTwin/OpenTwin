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
#include "SubprocessDebugConfigurator.h"
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
}


std::string Application::processAction(const std::string & _action, ot::JsonDocument& _doc)
{
	try
	{
		std::string returnMessage = "";
		if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
		{
			
			std::string action = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
			if (action == OT_ACTION_CMD_PYTHON_EXECUTE)
			{
				if (_doc.HasMember(OT_ACTION_CMD_PYTHON_Scripts) && _doc.HasMember(OT_ACTION_CMD_PYTHON_Parameter))
				{
					if (_doc[OT_ACTION_CMD_PYTHON_Scripts].IsArray() && _doc[OT_ACTION_CMD_PYTHON_Parameter].IsArray())
					{
						ot::JsonDocument subprocessDoc;
						auto& scripts = _doc[OT_ACTION_CMD_PYTHON_Scripts];
						subprocessDoc.AddMember(OT_ACTION_CMD_PYTHON_Scripts, scripts, subprocessDoc.GetAllocator());
						auto& parameter = _doc[OT_ACTION_CMD_PYTHON_Parameter];
						ot::rJSON::add(subprocessDoc, OT_ACTION_CMD_PYTHON_Parameter, parameter);

						if (ot::rJSON::memberExists(_doc, OT_ACTION_CMD_PYTHON_Portdata_Names) && _doc[OT_ACTION_CMD_PYTHON_Portdata_Names].IsArray())
						{
							assert(ot::rJSON::memberExists(_doc, OT_ACTION_CMD_PYTHON_Portdata_Data));
							auto& portData = _doc[OT_ACTION_CMD_PYTHON_Portdata_Data];
							ot::rJSON::add(subprocessDoc, OT_ACTION_CMD_PYTHON_Portdata_Data, portData);
							auto& portNames = _doc[OT_ACTION_CMD_PYTHON_Portdata_Names];
							ot::rJSON::add(subprocessDoc, OT_ACTION_CMD_PYTHON_Portdata_Names, portNames);
						}

						return _subprocessHandler->SendExecutionOrder(subprocessDoc);					
					}
					else
					{
						ot::ReturnMessage msg(ot::ReturnMessage::Failed, "Scripts and parameter are not of expected type.");
						return msg.toJson();
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

				ot::JsonDocument message;
				message.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteAction, message.GetAllocator()), message.GetAllocator());
				message.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_PYTHON_Initialization, message.GetAllocator()), message.GetAllocator());
				message.AddMember("ModelService.URL", ot::JsonString(urlModelservice, message.GetAllocator()), message.GetAllocator());
				message.AddMember("Service.ID", m_serviceID, message.GetAllocator());
				message.AddMember("Session.ID", ot::JsonString(m_sessionID, message.GetAllocator()), message.GetAllocator());
				message.AddMember("DataBase.PWD", ot::JsonString(pwd, message.GetAllocator()), message.GetAllocator());
				message.AddMember("DataBase.Username", ot::JsonString(userName, message.GetAllocator()), message.GetAllocator());
				message.AddMember("DataBase.URL", ot::JsonString(m_databaseURL, message.GetAllocator()), message.GetAllocator());
				returnMessage = message.toJson();
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

}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::uiPluginConnected(ot::components::UiPluginComponent * _uiPlugin) {

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{
	OT_LOG_D("Starting to create the subprocess");
#ifdef _DEBUG
	try
	{
		SubprocessDebugConfigurator configurator;
		auto modelService = m_serviceNameMap[OT_INFO_SERVICE_TYPE_MODEL];
		std::string urlModelservice = modelService.service->serviceURL();
		int startPort = SubprocessHandler::getStartPort();
		std::string subserviceURL = m_serviceURL.substr(0, m_serviceURL.find(":") + 1) + std::to_string(startPort);
		configurator.CreateConfiguration(m_serviceURL, subserviceURL, urlModelservice, m_databaseURL, m_serviceID, m_sessionID);
		_subprocessHandler = new SubprocessHandler(m_serviceURL);
		_subprocessHandler->setSubprocessURL(subserviceURL);
	}
	catch (std::exception& e)
	{
		OT_LOG_E(e.what());
		throw e;
	}
#else
	try
	{
		_subprocessHandler = new SubprocessHandler(m_serviceURL);
		std::thread workerThread(&SubprocessHandler::Create, _subprocessHandler, m_serviceURL);
		workerThread.detach();
	}
	catch (std::exception& e)
	{
		OT_LOG_E(e.what());
		throw e;
	}
#endif // DEBUG
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