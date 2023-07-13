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
	// Add code that should be executed when the service is started and may start its work
}

#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

std::string Application::processAction(const std::string & _action, OT_rJSON_doc & _doc)
{
	try
	{
		std::string returnMessage = "";
		if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
		{
			std::string action = ot::rJSON::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
			if (action == OT_ACTION_CMD_PYTHON_EXECUTE_STRINGS)
			{
				std::string subsequentFunction = _doc[OT_ACTION_PARAM_MODEL_FunctionName].GetString();
				std::string senderURL = _doc[OT_ACTION_PARAM_SENDER_URL].GetString();

				auto scriptArray = _doc["Scripts"].GetArray();
				
				std::list<std::string> scripts;
				for (auto& element : scriptArray)
				{
					scripts.push_back(element.GetString());
				}

				auto parameterArray = _doc["Parameter"].GetArray();

				std::list<std::optional<std::list<variable_t>>> allParameter;
				
				for (uint32_t i = 0; i < parameterArray.Size(); i++)
				{
					std::list<variable_t> parameter;
					if (parameterArray[i].IsString())
					{
						parameter.emplace_back(parameterArray[i].GetString());
					}
					else if (parameterArray[i].IsInt())
					{
						parameter.emplace_back(parameterArray[i].GetInt());
					}
					else if (parameterArray[i].IsDouble())
					{
						parameter.emplace_back(parameterArray[i].GetDouble());
					}
					else if (parameterArray[i].IsBool())
					{
						parameter.emplace_back(parameterArray[i].GetBool());
					}
					else if (parameterArray[i].IsArray())
					{
						//ToDo
					}
					else if (parameterArray[i].IsNull())
					{
						allParameter.push_back({});
						continue;
					}
					allParameter.emplace_back(parameter);
				}
				std::thread workerThread(&Application::ProcessScriptExecution,this, scripts, allParameter, subsequentFunction);
				workerThread.detach();
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

void Application::ProcessScriptExecution(std::list<std::string> scripts, std::list<std::optional<std::list<variable_t>>> allParameter, const std::string subsequentFunction)
{

	std::list<variable_t> result =	_pythonAPI.Execute(scripts, allParameter);
		
	OT_rJSON_createDOC(newDocument);
	OT_rJSON_createValueArray(rJsonResult);
	VariableToJSONConverter converter;
	for (auto& element : result)
	{
		rapidjson::Value rJsonVal = converter.Convert(std::move(element));
		rJsonResult.PushBack(rJsonVal, newDocument.GetAllocator());
	}
	ot::rJSON::add(newDocument, "Result", rJsonResult);

	ot::rJSON::add(newDocument, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_ExecuteFunction);
	ot::rJSON::add(newDocument, OT_ACTION_PARAM_MODEL_FunctionName, subsequentFunction);
	/*Application::instance()->sendMessage(true, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, newDocument);*/

}


// ##################################################################################################################################################################################################################