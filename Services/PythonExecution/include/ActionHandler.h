#pragma once
#include <mutex>
#include <map>
#include <functional>
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "PythonAPI.h"
#include "OpenTwinCore/ReturnMessage.h"

class ActionHandler
{
public:
	ActionHandler(const std::string& urlMasterService);
	const char* Handle(const char* json, const char* senderIP);

private:
	using handlerMethod = std::function<void(OT_rJSON_doc& _doc)>;
	using testingMethod = std::function<ot::ReturnMessage(OT_rJSON_doc& _doc)>;
	std::mutex _mutex;
	const std::string _urlMasterService;
	std::map<std::string, handlerMethod> _handlingFunctions;
	std::map<std::string, testingMethod> _checkParameterFunctions;
	testingMethod _noParameterCheck = [](OT_rJSON_doc& doc) {return ot::ReturnMessage(OT_ACTION_RETURN_VALUE_OK, "No check performed."); };
	PythonAPI _pythonAPI;

	void ShutdownProcess(OT_rJSON_doc& doc);
	void Execute(OT_rJSON_doc& doc);
	ot::ReturnMessage CheckParameterExecute(OT_rJSON_doc& doc);
};
