#pragma once
#include <mutex>
#include <map>
#include <functional>
#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "PythonAPI.h"
#include "OTCore/ReturnMessage.h"

class ActionHandler
{
public:
	ActionHandler();
	ot::ReturnMessage Handle(ot::JsonDocument& doc);
private:

	using handlerMethod = std::function<ot::ReturnMessage(ot::JsonDocument&)>;
	std::mutex _mutex;
	const std::string _urlMasterService;
	std::map<std::string, handlerMethod> _handlingFunctions;
	std::map<std::string, handlerMethod> _checkParameterFunctions;
	handlerMethod _noParameterCheck = [](ot::JsonDocument& doc) { return ot::ReturnMessage(ot::ReturnMessage::Ok, "No check performed."); };
	PythonAPI _pythonAPI;

	ot::ReturnMessage handlePing(ot::JsonDocument& _doc);
	ot::ReturnMessage ShutdownProcess(ot::JsonDocument& doc);
	ot::ReturnMessage ExecuteScript(ot::JsonDocument& doc);
	ot::ReturnMessage ExecuteCommand(ot::JsonDocument& doc);
	ot::ReturnMessage Initialise(ot::JsonDocument& doc);

	ot::ReturnMessage CheckParameterExecuteScript(ot::JsonDocument& doc);
	ot::ReturnMessage CheckParameterExecuteCommand(ot::JsonDocument& doc);

};
#pragma once
