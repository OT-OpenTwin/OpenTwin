// @otlicense
// File: Application.cpp
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

// DebugService header
#include "Application.h"

// OpenTwin System header
#include "OTSystem/DateTime.h"
#include "OTSystem/OperatingSystem.h"

// OpenTwin Core header
#include "OTCore/ReturnMessage.h"
#include "OTCore/ContainerHelper.h"
#include "OTCore/ThisComputerInfo.h"

// OpenTwin Communication header
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

// OpenTwin ServiceFoundation header
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/AbstractUiNotifier.h"
#include "OTServiceFoundation/AbstractModelNotifier.h"

// std header
#include <thread>
#include <fstream>

Application& Application::instance() {
	static Application g_instance;
	return g_instance;
}

Application::~Application()
{

}

// ###########################################################################################################################################################################################################################################################################################################################

// Required functions

void Application::initialize()
{
	OT_LOG_T("Ini");
}

void Application::run()
{
	OT_LOG_E("Gateway service cannot be run without arguments.");
	exit(ot::AppExitCode::ArgumentMissingError);
}

void Application::runWithArgs(const std::map<std::string, std::string>& _args)
{
	auto it = _args.find(OT_ACTION_PARAM_Ticket);
	if (it == _args.end()) {
		OT_LOG_E("Missing ticket argument.");
		exit(ot::AppExitCode::ArgumentMissingError);
	}
	m_requestTicket = it->second;
}

ot::ReturnMessage Application::handleGatewayAction(ot::JsonDocument& _document)
{
	using namespace ot;

	std::string ticket = json::getString(_document, OT_ACTION_PARAM_Ticket);
	if (ticket != m_requestTicket) {
		const std::string errorMsg = "Invalid ticket in gateway action.";
		OT_LOG_E(errorMsg);
		exit(ot::AppExitCode::RequestBlocked);
		return ReturnMessage::failed(errorMsg);
	}

	std::string actionTypeStr = json::getString(_document, OT_ACTION_PARAM_Gateway_Action);
	if (actionTypeStr.empty()) {
		const std::string errorMsg = "Missing action type in gateway action.";
		OT_LOG_E(errorMsg);
		return ReturnMessage::failed(errorMsg);
	}

	auto actionType = Gateway::stringToGatewayActionType(actionTypeStr);

	switch (actionType)
	{
	case Gateway::GatewayActionType::Unknown:
		OT_LOG_E("Unknown gateway action type.");
		return ReturnMessage::failed("Unknown gateway action type.");

	case Gateway::GatewayActionType::Ping:
		OT_LOG_D("Ping received via gateway action.");
		return ReturnMessage::ok(actionTypeStr);

	case Gateway::GatewayActionType::Shutdown:
		OT_LOG_I("Shutdown requested via gateway action.");
		// ...
		exit(ot::AppExitCode::Success);

	case Gateway::GatewayActionType::SendToService:
		return handleGatewayActionSendToService(_document, false);
	case Gateway::GatewayActionType::QueueToService:
		return handleGatewayActionSendToService(_document, true);
	default:
		OT_LOG_ES("Unknown gateway action type \"" << actionTypeStr << "\"");
		return ReturnMessage::failed("Unknown gateway action type \"" + actionTypeStr + "\"");
	}
}

ot::ReturnMessage Application::handleGatewayActionSendToService(ot::JsonDocument& _document, bool _queue)
{
	using namespace ot;

	// Check request
	std::string serviceName = json::getString(_document, OT_ACTION_PARAM_SERVICE_NAME);
	if (serviceName.empty()) {
		const std::string errorMsg = "Missing service name in gateway action.";
		OT_LOG_E(errorMsg);
		return ReturnMessage::failed(errorMsg);
	}
	
	// Check if service is connected
	auto* service = getConnectedServiceByName(serviceName);
	if (!service)
	{
		const std::string errorMsg = "Service \"" + serviceName + "\" is not connected.";
		OT_LOG_E(errorMsg);
		return ReturnMessage::failed(errorMsg);
	}
	std::string serviceUrl = service->getServiceURL();

	// Get request data
	ConstJsonObject requestData = json::getObject(_document, OT_ACTION_PARAM_Data);
	std::string request = json::toJson(requestData);
	std::string requestResponse;
	
	MessageType messageType = (_queue ? MessageType::QUEUE : MessageType::EXECUTE);
	if (!msg::send(getServiceURL(), serviceUrl, messageType, request, requestResponse, msg::defaultTimeout, msg::DefaultFlagsNoExit))
	{
		const std::string errorMsg = "Failed to send request to service \"" + serviceName + "\".";
		OT_LOG_E(errorMsg);
		return ReturnMessage::failed(errorMsg);
	}

	return ot::ReturnMessage::ok(requestResponse);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Action handler

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Constructor

Application::Application() :
	ot::ApplicationBase(
		OT_INFO_SERVICE_TYPE_GatewayService,
		OT_INFO_SERVICE_TYPE_GatewayService,
		new ot::AbstractUiNotifier(),
		new ot::AbstractModelNotifier()
	)
{
	// Connect actions (except gateway actions)
	connectAction(OT_ACTION_CMD_PerformGatewayAction, this, &Application::handleGatewayAction, ot::ALL_MESSAGE_TYPES);

}
