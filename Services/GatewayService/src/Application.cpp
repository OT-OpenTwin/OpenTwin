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
	this->connectAction(OT_ACTION_CMD_CheckGatewayReady, this, &Application::handleCheckReady, ot::SECURE_MESSAGE_TYPES);
	this->connectAction(OT_ACTION_CMD_GatewayRun, this, &Application::handleRunGateway, ot::SECURE_MESSAGE_TYPES);
}


// ###########################################################################################################################################################################################################################################################################################################################

// Private: Action handler

ot::ReturnMessage Application::handleCheckReady(ot::JsonDocument& _document)
{
	return ot::ReturnMessage::True;
}

ot::ReturnMessage Application::handleRunGateway(ot::JsonDocument& _document)
{
	return ot::ReturnMessage::ok("");
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Constructor

Application::Application() :
	ot::ApplicationBase(
		OT_INFO_SERVICE_TYPE_GatewayService,
		OT_INFO_SERVICE_TYPE_GatewayService,
		new ot::AbstractUiNotifier(),
		new ot::AbstractModelNotifier()
	)
{}
