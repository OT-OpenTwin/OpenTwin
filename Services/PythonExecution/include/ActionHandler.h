#pragma once
#include <mutex>
#include <map>
#include <functional>
#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "PythonAPI.h"
#include "OTCore/ReturnMessage.h"

class ActionHandler {
public:
	static ActionHandler& instance(void);

	ot::ReturnMessage handleAction(const ot::JsonDocument& doc);

private:
	ActionHandler();

	using handlerMethod = std::function<ot::ReturnMessage(const ot::JsonDocument&)>;
	
	std::mutex m_mutex;
	const std::string m_urlMasterService;
	
	std::map<std::string, handlerMethod> m_handlingFunctions;
	std::map<std::string, handlerMethod> m_checkParameterFunctions;
	
	handlerMethod m_noParameterCheck = [](const ot::JsonDocument& doc) { return ot::ReturnMessage(ot::ReturnMessage::Ok, "No check performed."); };
	PythonAPI m_pythonAPI;

	ot::ReturnMessage handlePing(const ot::JsonDocument& _doc);
	ot::ReturnMessage shutdownProcess(const ot::JsonDocument& doc);
	ot::ReturnMessage executeScript(const ot::JsonDocument& doc);
	ot::ReturnMessage executeCommand(const ot::JsonDocument& doc);
	ot::ReturnMessage initialise(const ot::JsonDocument& doc);

	ot::ReturnMessage checkParameterExecuteScript(const ot::JsonDocument& doc);
	ot::ReturnMessage checkParameterExecuteCommand(const ot::JsonDocument& doc);

};
