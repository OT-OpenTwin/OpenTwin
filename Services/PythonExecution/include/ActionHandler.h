#pragma once
#include <mutex>
#include <map>
#include <functional>
#include "OTCore/rJSON.h"
#include "OTCommunication/ActionTypes.h"
#include "PythonAPI.h"
#include "OTCore/ReturnMessage.h"

class ActionHandler
{
public:
	ActionHandler(const std::string& urlMasterService);
	const char* Handle(const char* json, const char* senderIP);

private:

	using handlerMethod = std::function<ot::ReturnMessage(OT_rJSON_doc&)>;
	std::mutex _mutex;
	const std::string _urlMasterService;
	std::map<std::string, handlerMethod> _handlingFunctions;
	std::map<std::string, handlerMethod> _checkParameterFunctions;
	handlerMethod _noParameterCheck = [](OT_rJSON_doc& doc) { return ot::ReturnMessage(ot::ReturnMessage::Ok, "No check performed."); };
	PythonAPI _pythonAPI;

	ot::ReturnMessage ShutdownProcess(OT_rJSON_doc& doc);
	ot::ReturnMessage Execute(OT_rJSON_doc& doc);
	ot::ReturnMessage CheckParameterExecute(OT_rJSON_doc& doc);

};
