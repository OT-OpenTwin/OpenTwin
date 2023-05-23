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
				std::string msmdName = _doc["MSMD"].GetString();
				std::string senderURL = _doc[OT_ACTION_PARAM_SENDER_URL].GetString();

				auto scriptArray = _doc["Scripts"].GetArray();
				std::vector<std::string> scripts;
				for (auto& element : scriptArray)
				{
					scripts.push_back(element.GetString());
				}

				auto parameterArray = _doc["Parameter"].GetArray();
				std::vector<ot::VariableBundle> allParameter;
				for (auto& element : parameterArray)
				{
					auto subDocument = element.GetObject();
					ot::VariableBundle parameter;
					for (auto& subElement : subDocument)
					{
						std::string name = subElement.name.GetString();
						std::string type = subElement.value.GetString();
						if (type == ot::TypeNames::getStringTypeName())
						{
							ot::Variable<std::string> var(name, type);
							parameter.AddVariable(var);
						}
						else if (type == ot::TypeNames::getDoubleTypeName())
						{
							ot::Variable<double> var(name, type);
							parameter.AddVariable(var);
						}
						else if (type == ot::TypeNames::getFloatTypeName())
						{
							ot::Variable<float> var(name, type);
							parameter.AddVariable(var);
						}
						else if (type == ot::TypeNames::getInt32TypeName())
						{
							ot::Variable<int32_t> var(name, type);
							parameter.AddVariable(var);
						}
						else if (type == ot::TypeNames::getInt64TypeName())
						{
							ot::Variable<int64_t> var(name, type);
							parameter.AddVariable(var);
						}
						else if (type == ot::TypeNames::getBoolTypeName())
						{
							ot::Variable<bool> var(name, type);
							parameter.AddVariable(var);
						}
					}
					allParameter.push_back(parameter);
				}
				std::thread workerThread(&Application::ProcessScriptExecution,this, scripts, allParameter, subsequentFunction, msmdName);
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

void Application::ProcessScriptExecution(std::vector<std::string> scripts, std::vector<ot::VariableBundle> allParameter, std::string subsequentFunction, std::string msmdName)
{

	_pythonAPI.InterpreteString(scripts, allParameter);


	OT_rJSON_createDOC(newDocument);

	OT_rJSON_createValueArray(parameter);
	for (auto& element : allParameter)
	{
		OT_rJSON_createValueObject(subDoc);

		for (ot::Variable<int32_t>& intVariable : (*element.GetVariablesInt32()))
		{
			ot::rJSON::add(newDocument, subDoc, intVariable.name, intVariable.value);
		}
		for (ot::Variable<int64_t>& intVariable : (*element.GetVariablesInt64()))
		{
			//Takes only unsinged int64!!
			//ot::rJSON::add(newDocument, subDoc, intVariable.name, intVariable.value);
		}
		for (ot::Variable<std::string>& intVariable : (*element.GetVariablesString()))
		{
			ot::rJSON::add(newDocument, subDoc, intVariable.name, intVariable.value);
		}
		for (ot::Variable<bool>& intVariable : (*element.GetVariablesBool()))
		{
			ot::rJSON::add(newDocument, subDoc, intVariable.name, intVariable.value);
		}
		for (ot::Variable<double>& intVariable : (*element.GetVariablesDouble()))
		{
			ot::rJSON::add(newDocument, subDoc, intVariable.name, intVariable.value);
		}
		for (ot::Variable<float>& intVariable : (*element.GetVariablesFloat()))
		{
			ot::rJSON::add(newDocument, subDoc, intVariable.name, intVariable.value);
		}
		parameter.PushBack(subDoc, newDocument.GetAllocator());
	}
	ot::rJSON::add(newDocument, "Parameter", parameter);

	ot::rJSON::add(newDocument, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_ExecuteFunction);
	ot::rJSON::add(newDocument, OT_ACTION_PARAM_MODEL_FunctionName, subsequentFunction);
	ot::rJSON::add(newDocument, "MSMD", msmdName);
	Application::instance()->sendMessage(true, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, newDocument);
}

// ##################################################################################################################################################################################################################