// @otlicense
// File: ActionHandler.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
	
	handlerMethod m_noParameterCheck = [](const ot::JsonDocument& _doc) { return ot::ReturnMessage(ot::ReturnMessage::Ok, "No check performed."); };
	PythonAPI m_pythonAPI;

	ot::ReturnMessage handlePing(const ot::JsonDocument& _doc);
	ot::ReturnMessage shutdownProcess(const ot::JsonDocument& _doc);
	ot::ReturnMessage executeScript(const ot::JsonDocument& _doc);
	void loadPortDataToBuffer(const ot::JsonDocument& _doc);
	std::string writeReturnDataToDatabase();

	ot::ReturnMessage executeCommand(const ot::JsonDocument& _doc);
	ot::ReturnMessage initialise(const ot::JsonDocument& _doc);
	ot::ReturnMessage setLogFlags(const ot::JsonDocument& _doc);

	ot::ReturnMessage checkParameterExecuteScript(const ot::JsonDocument& _doc);
	ot::ReturnMessage checkParameterExecuteCommand(const ot::JsonDocument& _doc);

};
